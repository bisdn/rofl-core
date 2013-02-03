/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cftentry.h"

using namespace rofl;

/*static*/std::set<cftentry*>		cftentry::cftentry_set;

cftentry::cftentry(
		cftentry_owner *owner,
		cofctl *ctl) :
		usage_cnt(0),
		owner(owner),
		flow_table(0),
		ctl(ctl),
		uid(0),
		removal_reason(OFPRR_DELETE),
		rx_packets(0),
		rx_bytes(0),
		out_port(OFPP_ANY),
		out_group(OFPG_ANY),
		flow_mod(0)
{
	cftentry::cftentry_set.insert(this);

	pthread_mutex_init(&usage_mutex, NULL);
	pthread_mutex_init(&flags_mutex, NULL);
	pthread_rwlock_init(&access_time_lock, NULL);

	make_info();
	WRITELOG(FTE, DBG, "cftentry(%p)::cftentry() [1] %s %s",
			this, c_str(), instructions.c_str());
	last_access_time = cclock::now();
}



cftentry::cftentry(
		cftentry_owner *owner,
		std::set<cftentry*> *flow_table,
		cofpacket *pack,
		cofctl *ctl) :
		usage_cnt(0),
		owner(owner),
		flow_table(flow_table),
		ctl(ctl),
		uid(0),
		instructions(pack->instructions),
		removal_reason(OFPRR_DELETE),
		rx_packets(0),
		rx_bytes(0),
		out_port(be32toh(pack->ofh_flow_mod->out_port)),
		out_group(be32toh(pack->ofh_flow_mod->out_group)),
		ofmatch(pack->match),
		m_flowmod(sizeof(struct ofp_flow_mod) - sizeof(struct ofp_match)),
		flow_mod((struct ofp_flow_mod*)m_flowmod.somem())
{
	cftentry::cftentry_set.insert(this);

	pthread_mutex_init(&usage_mutex, NULL);
	pthread_mutex_init(&flags_mutex, NULL);
	pthread_rwlock_init(&access_time_lock, NULL);

	// flow_mod copy remains in network byte order !!!
	// we copy the generic flow mod header only, i.e. ofp_match and all instructions are
	// stored in this->match and this->inlist !
	memcpy(m_flowmod.somem(), (uint8_t*)pack->ofh_flow_mod, m_flowmod.memlen());

	__update();

	 // remember, when this flow was created
	flow_create_time = cclock::now();
	last_access_time = cclock::now();

	// set hard timeout always
	if (be16toh(this->flow_mod->hard_timeout) != OFP_FLOW_PERMANENT)
	{
		register_timer(TIMER_FTE_HARD_TIMEOUT, be16toh(this->flow_mod->hard_timeout));
	}

	// set idle timeout only, when flow_mod->idle_timeout < flow_mod->hard_timeout
	if (be16toh(this->flow_mod->idle_timeout) != OFP_FLOW_PERMANENT)
	{
		register_timer(TIMER_FTE_IDLE_TIMEOUT, be16toh(this->flow_mod->idle_timeout));
	}

#ifndef NDEBUG
	// create info string for debugging
	make_info(); // todo if no debugging is specified this should not be used
#endif

	WRITELOG(FTE, DBG, "cftentry(%p)::cftentry() [2] %s %s", this, c_str(), instructions.c_str());


	if (flow_table)
	{
		flow_table->insert(this);
	}
}



cftentry::cftentry(
		cftentry const& fte) :
				usage_cnt(0),
				owner(owner),
				flow_table(0),
				ctl(ctl),
				uid(0),
				removal_reason(OFPRR_DELETE),
				rx_packets(0),
				rx_bytes(0),
				out_port(OFPP_ANY),
				out_group(OFPG_ANY),
				flow_mod(0)
{
	cftentry::cftentry_set.insert(this);

	pthread_mutex_init(&usage_mutex, NULL);
	pthread_mutex_init(&flags_mutex, NULL);
	pthread_rwlock_init(&access_time_lock, NULL);

	*this = fte;
}



cftentry::~cftentry()
{
	cftentry::cftentry_set.erase(this);

	WRITELOG(FTE, DBG, "cftentry(%p)::~cftentry() %s", this, c_str());

	if (flow_table)
	{
		flow_table->erase(this);
	}

	pthread_rwlock_destroy(&access_time_lock);
	pthread_mutex_destroy(&usage_mutex);
	pthread_mutex_destroy(&flags_mutex);
}




void
cftentry::handle_event(cevent const& ev)
{
	switch (ev.cmd) {
	case CFTENTRY_EVENT_IDLE_FOR_DELETION:
	{
		WRITELOG(CFWD, DBG, "cftentry(%p)::handle_event() "
				"idle for deletion", this);

		/*
		 * Our usage counter has dropped to 0. Execute self-destruction.
		 */
		Lock lock(&usage_mutex);
		if (0 == usage_cnt) {
			register_timer(TIMER_FTE_REMOVAL, 2);
		} else {
			notify(cevent(CFTENTRY_EVENT_IDLE_FOR_DELETION));
		}

		/*
		 * Our usage counter has dropped to 0. Send notification to our flow_table, we
		 * are ready for deletion.
		 */
		//if (owner) { owner->ftentry_idle_for_deletion(this); };
	}
		return;
	}
}



void
cftentry::handle_timeout(int opaque)
{
#if 0
	fprintf(stderr, "cftentry(%p)::handle_timeout() set: ", this);
	for (std::set<cftentry*>::iterator
			it = cftentry::cftentry_set.begin(); it != cftentry::cftentry_set.end(); ++it)
	{
		fprintf(stderr, "%p ", (*it));
	}
	fprintf(stderr, "\n");
#endif

	switch (opaque) {
	case TIMER_FTE_IDLE_TIMEOUT:
	{
		/*
		 *  Mark this instance for deletion by setting the TIMER-EXPIRED flag.
		 *  This prevents our hosting flow_table from assigning us to packet engines again.
		 *  Once, all packet engines have concluded their work and released the reference
		 *  to this cftentry instance, we will notify our flow_table, or if there is no
		 *  reference to us currently, inform our flow_table immediately.
		 */
		if (flags.test(CFTENTRY_FLAG_TIMER_EXPIRED))
		{
			// deletion of this cftentry instance already triggered, abort notification
			return;
		}

		/*
		 * check for last access time, if it has expired, schedule deletion,
		 * otherwise just reset the idle timeout timer
		 */
		{
			RwLock rwlock(&access_time_lock, RwLock::RWLOCK_READ);
			if (((cclock::now().ts.tv_sec - last_access_time.ts.tv_sec)) < be16toh(flow_mod->idle_timeout))
			{
				time_t remaining_time = be16toh(flow_mod->idle_timeout) -
											(cclock::now().ts.tv_sec - last_access_time.ts.tv_sec);
				register_timer(TIMER_FTE_IDLE_TIMEOUT, remaining_time);
				return;
			}
		}

		flags.set(CFTENTRY_FLAG_TIMER_EXPIRED);

		WRITELOG(CFWD, DBG, "cftentry(%p)::handle_timeout() "
				"idle-timeout", this);

		removal_reason = OFPRR_IDLE_TIMEOUT;

		{
			//RwLock lock(&usage_lock, RwLock::RWLOCK_READ);
			Lock lock(&usage_mutex);
			if (0 == usage_cnt)
			{
				if (pending_timer(TIMER_FTE_HARD_TIMEOUT))
				{
					cancel_timer(TIMER_FTE_HARD_TIMEOUT);
				}

				// send notification to owner => OWNER MUST REMOVE ALL REFERENCES TO THIS ENTRY FROM ITS INTERNAL STRUCTURES
				if (owner) { owner->ftentry_idle_timeout(this, be16toh(flow_mod->idle_timeout)); };
			}
		} // unlock lock here

		// initiate deletion of this cftentry => this may call this instance's destructor, so we must return at the end of this block
		schedule_deletion();
	}
		return;

	case TIMER_FTE_HARD_TIMEOUT:
	{
		/*
		 *  Mark this instance for deletion by setting the TIMER-EXPIRED flag.
		 *  This prevents our hosting flow_table from assigning us to packet engines again.
		 *  Once, all packet engines have concluded their work and released the reference
		 *  to this cftentry instance, we will notify our flow_table, or if there is no
		 *  reference to us currently, inform our flow_table immediately.
		 */
		if (flags.test(CFTENTRY_FLAG_TIMER_EXPIRED))
		{
			// deletion of this cftentry instance already triggered, abort notification
			return;
		}

		flags.set(CFTENTRY_FLAG_TIMER_EXPIRED);

		WRITELOG(CFWD, DBG, "cftentry(%p)::handle_timeout() "
				"hard-timeout", this);

		removal_reason = OFPRR_HARD_TIMEOUT;

		{
			//RwLock lock(&usage_lock, RwLock::RWLOCK_READ);
			Lock lock(&usage_mutex);
			if (0 == usage_cnt)
			{
				if (pending_timer(TIMER_FTE_IDLE_TIMEOUT))
				{
					cancel_timer(TIMER_FTE_IDLE_TIMEOUT);
				}

				// send notification to owner => OWNER MUST REMOVE ALL REFERENCES TO THIS ENTRY FROM ITS INTERNAL STRUCTURES
				if (owner) { owner->ftentry_hard_timeout(this, be16toh(flow_mod->hard_timeout)); };
			}
		} // unlock lock here

		// initiate deletion of this cftentry => this may call this instance's destructor, so we must return at the end of this block
		schedule_deletion();
	}
		return;
	case TIMER_FTE_REMOVAL: {

		cancel_all_timer();
		removal_reason = OFPRR_DELETE;
		// TODO: add owner->ftentry_removal(this);
		delete this;
	}
		return;

	default:
		WRITELOG(FTE, DBG, "cftentry(%p)::handle_timeout() unknown timer (%d)", this, opaque);
		break;
	}
}



bool
cftentry::is_disabled()
{
	Lock fmutex(&usage_mutex);
	return (flags.test(CFTENTRY_FLAG_TIMER_EXPIRED));
}


void
cftentry::enable_entry()
{
	Lock fmutex(&usage_mutex);
	flags.reset(CFTENTRY_FLAG_TIMER_EXPIRED);
}


void
cftentry::disable_entry()
{
	Lock fmutex(&usage_mutex);
	flags.set(CFTENTRY_FLAG_TIMER_EXPIRED);
}


void
cftentry::schedule_deletion()
{
	WRITELOG(CFWD, DBG, "cftentry(%p)::schedule_deletion() "
			"usage_cnt: %d", this, usage_cnt);

	Lock fmutex(&usage_mutex);
	/*
	 * already scheduled for deletion?
	 */
	flags.set(CFTENTRY_FLAG_TIMER_EXPIRED);

	// flock relases usage_lock here, do not delete this cftentry instance before removing ulock from stack
	{
		//RwLock ulock(&usage_lock, RwLock::RWLOCK_READ);
		//Lock lock(&usage_mutex); // intentional WRITE-LOCK, we must not remove this instance unless we have acquired the writelock
		if (usage_cnt > 0)
		{
			return;
		}
	} // ulock relases usage_lock here, do not delete this cftentry instance before removing ulock from stack

	notify(cevent(CFTENTRY_EVENT_IDLE_FOR_DELETION));
	//delete this; return;
	//if (owner) { owner->ftentry_idle_for_deletion(this); };
}



void
cftentry::sem_inc() throw (eFtEntryUnAvail)
{
	/*
	 * THIS METHOD RUNS IN CONTEXT OF PACKET ENGINE THREAD!
	 */

	Lock ulock(&usage_mutex);
	/*
	 * security check: if flag TIMER-EXPIRED is set, we are rejecting requests for using this cftentry instance
	 */
	{
		//Lock fmutex(&flags_mutex);
		if (flags.test(CFTENTRY_FLAG_TIMER_EXPIRED))
		{
			WRITELOG(FTE, DBG, "cftentry(%p)::sem_inc() scheduled for removal, thus unavailable", this);
			throw eFtEntryUnAvail();
		}
	}

	//Lock ulock(&usage_mutex);
	++usage_cnt;

	WRITELOG(FTE, DBG, "cftentry(%p)::sem_inc() usage_cnt: %d", this, usage_cnt);
}



void
cftentry::sem_dec()
{
	/*
	 * THIS METHOD RUNS IN CONTEXT OF PACKET ENGINE THREAD!
	 */
	//{
		Lock lock(&usage_mutex);
		usage_cnt = (usage_cnt > 0) ? (usage_cnt - 1) : 0;
	//}

	WRITELOG(FTE, DBG, "cftentry(%p)::sem_dec() usage_cnt: %d", this, usage_cnt);

	//Lock fmutex(&flags_mutex);
	if ((flags.test(CFTENTRY_FLAG_TIMER_EXPIRED) && (0 == usage_cnt)))
	{
		WRITELOG(FTE, DBG, "cftentry(%p)::sem_dec() initiating removal", this);
		notify(cevent(CFTENTRY_EVENT_IDLE_FOR_DELETION));
	}
}




cftentry &
cftentry::operator=(const cftentry& fte)
{
	throw eDebug();

	if (this == &fte)
		return *this;

	this->rx_packets = fte.rx_packets;
	this->rx_bytes = fte.rx_bytes;
	this->owner = fte.owner;

	// remove ourselves from old flow_table
	if (flow_table)
	{
		flow_table->erase(this);
	}

	flow_table = fte.flow_table;

	if (flow_table)
	{
		flow_table->insert(this);
	}

	// copy generic flow mod header, match, and instruction list
	m_flowmod = fte.m_flowmod;
	ofmatch = fte.ofmatch;
	instructions = fte.instructions;

	// set auxiliary pointer
	flow_mod = (struct ofp_flow_mod*)m_flowmod.somem();

	return *this;
}


void
cftentry::__update()
{
	/* problem: a list of actions may contain several 'outgoing' actions
	 * triggering queueing of a packet on an outgoing port or an outgoing
	 * queue attached to a port. On page 22 OF 1.0 spec it is stated,
	 * that after a queueing operation (action_enqueue) a packet should not
	 * be altered any more, however, the same statement for action_output
	 * is missing, i.e. a changing action may occur after an action_output call.
	 * Thus, we have to make sure proper copy-on-write operations when such
	 * conditions occur.
	 *
	 * btw. what if multiple ft-entries match for a single packet? this is for further study ...
	 *
	 * strategy:
	 * 1. if there is only a single outgoing action, do not copy internally, but
	 * queue the original packet from heap directly on outgoing queue, set pkb->pack
	 * to NULL in order to avoid any premature removal of this packet from heap.
	 * 2. if there are multiple outgoing actions, apply copy-on-write policy,
	 * i.e. for each outgoing action, a new clone of the packet (with all changes
	 * applied so far) is created and queued on the port.
	 * 3. if there are any non-outgoing actions defined after the last outgoing action,
	 * apply these normally.
	 */

	//RwLock lock(&usage_mutex, RwLock::RWLOCK_WRITE);
	Lock lock(&usage_mutex);

	this->flags &= ~CFTENTRY_FLAG_COPY_ON_WRITE;

	#ifndef NDEBUG
		WRITELOG(CFWD, DBG, "cftentry(%p)::__update() cofinlist: %s", this, instructions.c_str());
	#endif

	// if an instruction OFPIT_APPLY_ACTIONS exists and multiple ActionOutput instances exist,
	// mark packet for copy-on-write
	cofinlist::iterator it;
	if ((it = find_if(instructions.begin(), instructions.end(),
			cofinst_find_type(OFPIT_APPLY_ACTIONS))) != instructions.end())
	{
		cofinst& inst = (*it);

		WRITELOG(CFWD, DBG, "cftentry(%p)::__update() inst: %s",
				this, inst.c_str());

		// if multiple output actions, mark packet for copy-on-write
		if (inst.actions.count_action_type(OFPAT_OUTPUT) > 1)
		{
			this->flags |= CFTENTRY_FLAG_COPY_ON_WRITE;
		}
		// is last action is neither OUTPUT nor ENQUEUE, mark packet for copy-on-write
		if ((not inst.actions.empty()) &&
				(be16toh(inst.actions.back().oac_header->type) != OFPAT_OUTPUT))
		{
			this->flags |= CFTENTRY_FLAG_COPY_ON_WRITE;
		}

		cofaclist::reverse_iterator cofActionIter =
				find_if(inst.actions.rbegin(), inst.actions.rend(),
				cofaction_find_type(OFPAT_OUTPUT));
		if (not (inst.actions.rend() == cofActionIter)) {
			out_port = be32toh((*cofActionIter).oac_oacu.oacu_output->port);
		}
	}
}


int 
cftentry::hw_fte_rmvd(int fpc_handle, int cause)
{
	// TODO: recreate ftentry in hardware?

	return 0;
}


cofinst&
cftentry::find_inst(enum ofp_instruction_type type)
{
	//RwLock lock(&usage_mutex, RwLock::RWLOCK_READ);
	Lock lock(&usage_mutex);
	WRITELOG(FTE, DBG, "cftentry(%p)::find_inst() blub:\n%s",
			this, instructions.find_inst(type).c_str());
	//WRITELOG(FTE, DBG, "cftentry(%p)::find_inst() instructions:\n%s", this, inlist.c_str());
	return instructions.find_inst(type);
}




bool
cftentry::overlaps(
		cofmatch& match,
		bool strict /* =false (default)*/)
{
	return this->ofmatch.overlaps(match, strict);
}



void
cftentry::used(cpacket& pack)
{
	rx_packets++;
	rx_bytes += pack.framelen();

	WRITELOG(FTE, DBG, "cftentry(%p)::used() "
			"rx_packets: %lu rx_bytes: %lu", this, rx_packets, rx_bytes);

	RwLock rwlock(&access_time_lock, RwLock::RWLOCK_WRITE);
	last_access_time = cclock::now();

	WRITELOG(FTE, DBG, "cftentry(%p)::used() "
			"last_access_time: %s", this, last_access_time.c_str());
}



void
cftentry::update_flow_mod(cofpacket *pack) throw (eFteInvalid)
{
	if (OFPT_FLOW_MOD != pack->ofh_header->type)
	{
		throw eFteInvalid();
	}

	memcpy(m_flowmod.somem(), pack->ofh_flow_mod, m_flowmod.memlen());

	ofmatch = pack->match;
	instructions  = pack->instructions;

	__update();
}


void
cftentry::handle_stats_request(
	struct ofp_flow_stats **flow_stats,
	size_t *flow_stats_len)
{
	cclock since; // initializes with now()

	since -= flow_create_time;

	*flow_stats_len = /*base header ofp_flow_stats*/48 +
					ofmatch.length() + instructions.length();

	// allocate space for ofp_flow_stats struct
	if ((*flow_stats = (struct ofp_flow_stats*)calloc(1, *flow_stats_len)) == NULL)
	{
		throw eOutOFMemory();
	}

	(*flow_stats)->length 			= htobe16(*flow_stats_len);
	(*flow_stats)->table_id 		= 1;    // only a single table for now
	(*flow_stats)->duration_sec 	= htobe32(since.ts.tv_sec);
	(*flow_stats)->duration_nsec 	= htobe32(since.ts.tv_nsec);
	(*flow_stats)->priority 		= flow_mod->priority;
	(*flow_stats)->idle_timeout 	= flow_mod->idle_timeout;
	(*flow_stats)->hard_timeout 	= flow_mod->hard_timeout;
	(*flow_stats)->cookie 			= flow_mod->cookie;
	(*flow_stats)->packet_count 	= htobe64(rx_packets);
	(*flow_stats)->byte_count 		= htobe64(rx_bytes);

	ofmatch.pack(		// append struct ofp_match
			(struct ofp_match*)(&(*flow_stats)->match),
			ofmatch.length());

	instructions.pack(	// append struct ofp_instruction
			(struct ofp_instruction*)((uint8_t*)&((*flow_stats)->match) + ofmatch.length()),
			instructions.length());
}


void
cftentry::ofp_send_flow_stats(
	uint64_t dpid,
	uint32_t xid,
	uint16_t type,
	uint16_t flags,
	struct ofp_flow_stats_request *req,
	size_t reqlen)
{
	struct ofp_flow_stats *stats = NULL;
	int instlen = (be16toh(flow_mod->header.length)
				 - sizeof(struct ofp_flow_mod));

	cclock since;
	since -= flow_create_time;


	// allocate space for ofp_flow_stats struct
	if ((stats = (struct ofp_flow_stats*)calloc(1,
								sizeof(struct ofp_flow_stats) + instlen)) == NULL)
	{
		throw eOutOFMemory();
	}

	stats->length = htobe16(sizeof(struct ofp_flow_stats) + instlen);
	stats->table_id = 1;    // only a single table for now
	memcpy((uint8_t*)&(stats->match),
		   (uint8_t*)&(flow_mod->match), sizeof(struct ofp_match));

	stats->duration_sec 		= htobe32(since.ts.tv_sec);
	stats->duration_nsec 		= htobe32(since.ts.tv_nsec);
	stats->priority 			= flow_mod->priority;
	stats->idle_timeout 		= flow_mod->idle_timeout;
	stats->hard_timeout 		= flow_mod->hard_timeout;
	stats->cookie 				= flow_mod->cookie;
	stats->packet_count 		= htobe64(rx_packets);
	stats->byte_count 			= htobe64(rx_bytes);

	//memcpy(&(stats->instructions), &(flow_mod->instructions), instlen);

	free(stats);
}


void
cftentry::get_flow_stats(
			cmemory& body,
			uint32_t out_port,
			uint32_t out_group,
			uint64_t cookie,
			uint64_t cookie_mask,
			cofmatch const& m)
{
	bool found = false;

	/*
	 * match must overlap
	 */
	if (not ofmatch.overlaps(m, false /*non-strict*/))
	{
		return;
	}

	/*
	 * out_port must match, if not OFPP_ANY
	 */
	if (OFPP_ANY != out_port)
	{
		try {
			cofinst& inst = instructions.find_inst(OFPIT_APPLY_ACTIONS);

			try {
				cofaction& action = inst.find_action(OFPAT_OUTPUT);
				if (be32toh(action.oac_output->port) == out_port)
				{
					found = true;
				}
			} catch (eAcListNotFound& e) {}

		} catch (eInListNotFound& e) {}

		try {
			cofinst& inst = instructions.find_inst(OFPIT_WRITE_ACTIONS);

			try {
				cofaction& action = inst.find_action(OFPAT_OUTPUT);
				if (be32toh(action.oac_output->port) == out_port)
				{
					found = true;
				}
			} catch (eAcListNotFound& e) {}

		} catch (eInListNotFound& e) {}
	}

	/*
	 * out_group must match, if not OFPG_ANY
	 */
	if (OFPG_ANY != out_group)
	{
		try {
			cofinst& inst = instructions.find_inst(OFPIT_APPLY_ACTIONS);

			try {
				cofaction& action = inst.find_action(OFPAT_GROUP);

				if (be32toh(action.oac_group->group_id) == out_group)
				{
					found = true;
				}
			} catch (eAcListNotFound& e) {}

		} catch (eInListNotFound& e) {}

		try {
			cofinst& inst = instructions.find_inst(OFPIT_WRITE_ACTIONS);

			try {
				cofaction& action = inst.find_action(OFPAT_GROUP);

				if (be32toh(action.oac_group->group_id) == out_group)
				{
					found = true;
				}
			} catch (eAcListNotFound& e) {}

		} catch (eInListNotFound& e) {}
	}

	if (not ((OFPP_ANY == out_port) && (OFPG_ANY == out_group)))
	{
		if (not found)
		{
			return;
		}
	}

	/*
	 * cookie must match
	 */
	if ((be64toh(flow_mod->cookie) & be64toh(flow_mod->cookie_mask)) !=
			(cookie & cookie_mask))
	{
		return;
	}


	/*
	 * create struct ofp_flow_stats and append to body
	 */
	cmemory fstats(OFP_FLOW_STATS_REPLY_STATIC_BODY_LEN + ofmatch.length() + instructions.length());
	struct ofp_flow_stats* flow_stats = (struct ofp_flow_stats*)fstats.somem();

	flow_stats->length 				= htobe16(fstats.memlen());
	flow_stats->table_id 			= flow_mod->table_id;
	flow_stats->duration_sec		= htobe32(flow_create_time.ts.tv_sec);
	flow_stats->duration_nsec		= htobe32(flow_create_time.ts.tv_nsec);
	flow_stats->priority			= flow_mod->priority;
	flow_stats->idle_timeout 		= flow_mod->idle_timeout;
	flow_stats->hard_timeout		= flow_mod->hard_timeout;
	flow_stats->cookie				= flow_mod->cookie;
	flow_stats->packet_count		= htobe64(rx_packets);
	flow_stats->byte_count			= htobe64(rx_bytes);

	ofmatch.pack(&(flow_stats->match), ofmatch.length());

	struct ofp_instruction *inst = (struct ofp_instruction*)((uint8_t*)&(flow_stats->match) + ofmatch.length());

	instructions.pack(inst, instructions.length());

	body += fstats;
}


void
cftentry::get_aggregate_flow_stats(
			uint64_t& packet_count,
			uint64_t& byte_count,
			uint64_t& flow_count,
			uint32_t out_port,
			uint32_t out_group,
			uint64_t cookie,
			uint64_t cookie_mask,
			cofmatch const& m)
{
	bool found = false;

	//fprintf(stderr, "\n\n A => out_port: 0x%x out_group: 0x%x\n", out_port, out_group);

	/*
	 * match must overlap
	 */
	if (not ofmatch.overlaps(m, false /*non-strict*/))
	{
		cofmatch test(m);
		WRITELOG(CFTTABLE, DBG, "cftentry::get_aggregate_flow_stats() "
				"MISMATCH => \ntest: %s\nofmatch: %s", test.c_str(), ofmatch.c_str());

		return;
	}

	/*
	 * out_port must match, if not OFPP_ANY
	 */
	if (OFPP_ANY != out_port)
	{
		try {
			cofinst& inst = instructions.find_inst(OFPIT_APPLY_ACTIONS);

			try {
				cofaction& action = inst.find_action(OFPAT_OUTPUT);
				if (be32toh(action.oac_output->port) == out_port)
				{
					goto found;
				}
			} catch (eAcListNotFound& e) {}

		} catch (eInListNotFound& e) {}

		try {
			cofinst& inst = instructions.find_inst(OFPIT_WRITE_ACTIONS);

			try {
				cofaction& action = inst.find_action(OFPAT_OUTPUT);
				if (be32toh(action.oac_output->port) == out_port)
				{
					goto found;
				}
			} catch (eAcListNotFound& e) {}

		} catch (eInListNotFound& e) {}
	}

	/*
	 * out_group must match, if not OFPG_ANY
	 */
	if (OFPG_ANY != out_group)
	{
		try {
			cofinst& inst = instructions.find_inst(OFPIT_APPLY_ACTIONS);

			try {
				cofaction& action = inst.find_action(OFPAT_GROUP);

				if (be32toh(action.oac_group->group_id) == out_group)
				{
					goto found;
				}
			} catch (eAcListNotFound& e) {}

		} catch (eInListNotFound& e) {}

		try {
			cofinst& inst = instructions.find_inst(OFPIT_WRITE_ACTIONS);

			try {
				cofaction& action = inst.find_action(OFPAT_GROUP);

				if (be32toh(action.oac_group->group_id) == out_group)
				{
					goto found;
				}
			} catch (eAcListNotFound& e) {}

		} catch (eInListNotFound& e) {}
	}

	//fprintf(stderr, "\n\n[R]\n\n\n");


	if (not ((OFPP_ANY == out_port) && (OFPG_ANY == out_group)))
	{
		//fprintf(stderr, "\n\n[S]\n\n\n");

		if (not found)
		{
			//fprintf(stderr, "\n\n[T]\n\n\n");

			return;
		}
	}

found:

	//fprintf(stderr, "\n\n[U]\n\n\n");

	/*
	 * cookie must match
	 */
	if ((be64toh(flow_mod->cookie) & be64toh(flow_mod->cookie_mask)) !=
			(cookie & cookie_mask))
	{
		//fprintf(stderr, "\n\n[V]\n\n\n");

		return;
	}

	//fprintf(stderr, "\n\n[W]\n\n\n");


	/*
	 * create struct ofp_flow_stats and append to body
	 */
	packet_count 	+= rx_packets;
	byte_count 		+= rx_bytes;
	flow_count 		+= 1;
}


void
cftentry::make_info()
{
	cvastring vas(4096);

	info.assign(vas("cftentry(%p) "
			"priority:%d "
			"cookie:0x%llx "
			"idle[%d] hard[%d] outport[0x%x] outgroup[0x%x] "
			"n_packets[%llu] n_bytes[%llu]\n"
			"ofpmatch:%s\n"
			"instructions:%s\n",
			this,
			be16toh(flow_mod->priority),
			be64toh(flow_mod->cookie),
	        // counters and timers
			be16toh(flow_mod->idle_timeout),
			be16toh(flow_mod->hard_timeout),
			out_port,
			out_group,
			(long long int)rx_packets,
			(long long int)rx_bytes,
			ofmatch.c_str(),
			instructions.c_str()));
}


const char*
cftentry::c_str()
{
	return info.c_str();
}


const char*
cftentry::s_str()
{
	return info.c_str();
}



void
cftentry::test()
{
	cftentry fe;

	cofinlist instructions;

	fprintf(stderr, "UUUUUU fe.inlist    => %s\n", fe.instructions.c_str());

	instructions[0] = cofinst_apply_actions();
	instructions[0].actions[0] = cofaction_output(1);
	instructions[0].actions[1] = cofaction_push_vlan(0x8100);
	instructions[0].actions[2] = cofaction_set_field(coxmatch_ofb_vlan_vid(4444));

	fprintf(stderr, "UUUUUU fe.inlist    => %s\n", fe.instructions.c_str());

	fe.instructions = instructions;

	fprintf(stderr, "VVVVVV fe.inlist    => %s\n", fe.instructions.c_str());
	fprintf(stderr, "VVVVVV instructions => %s\n", instructions.c_str());

	fprintf(stderr, "%s\n", fe.c_str());
}


