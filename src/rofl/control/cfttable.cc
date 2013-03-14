/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cfttable.h"

using namespace rofl;

uint64_t cfttable::all_matches =
		((uint64_t)1 << OFPXMT_OFB_IN_PORT 	) |
		((uint64_t)1 << OFPXMT_OFB_IN_PHY_PORT) |
		((uint64_t)1 << OFPXMT_OFB_METADATA	) |
		((uint64_t)1 << OFPXMT_OFB_ETH_DST	) |
		((uint64_t)1 << OFPXMT_OFB_ETH_SRC	) |
		((uint64_t)1 << OFPXMT_OFB_ETH_TYPE	) |
		((uint64_t)1 << OFPXMT_OFB_VLAN_VID	) |
		((uint64_t)1 << OFPXMT_OFB_VLAN_PCP	) |
		((uint64_t)1 << OFPXMT_OFB_IP_DSCP	) |
		((uint64_t)1 << OFPXMT_OFB_IP_ECN	) |
		((uint64_t)1 << OFPXMT_OFB_IP_PROTO	) |
		((uint64_t)1 << OFPXMT_OFB_IPV4_SRC	) |
		((uint64_t)1 << OFPXMT_OFB_IPV4_DST	) |
		((uint64_t)1 << OFPXMT_OFB_TCP_SRC	) |
		((uint64_t)1 << OFPXMT_OFB_TCP_DST	) |
		((uint64_t)1 << OFPXMT_OFB_UDP_SRC	) |
		((uint64_t)1 << OFPXMT_OFB_UDP_DST	) |
#if 0
		((uint64_t)1 << OFPXMT_OFB_SCTP_SRC	) |
		((uint64_t)1 << OFPXMT_OFB_SCTP_DST	) |
#endif
		((uint64_t)1 << OFPXMT_OFB_ICMPV4_TYPE) |
		((uint64_t)1 << OFPXMT_OFB_ICMPV4_CODE) |
		((uint64_t)1 << OFPXMT_OFB_ARP_OP	) |
		((uint64_t)1 << OFPXMT_OFB_ARP_SPA	) |
		((uint64_t)1 << OFPXMT_OFB_ARP_TPA	) |
		((uint64_t)1 << OFPXMT_OFB_ARP_SHA	) |
		((uint64_t)1 << OFPXMT_OFB_ARP_THA	) |
#if 0
		((uint64_t)1 << OFPXMT_OFB_IPV6_SRC	) |
		((uint64_t)1 << OFPXMT_OFB_IPV6_DST	) |
		((uint64_t)1 << OFPXMT_OFB_IPV6_FLABEL) |
		((uint64_t)1 << OFPXMT_OFB_ICMPV6_TYPE) |
		((uint64_t)1 << OFPXMT_OFB_ICMPV6_CODE) |
		((uint64_t)1 << OFPXMT_OFB_IPV6_ND_TARGET) |
		((uint64_t)1 << OFPXMT_OFB_IPV6_ND_SLL) |
		((uint64_t)1 << OFPXMT_OFB_IPV6_ND_TLL) |
#endif
		((uint64_t)1 << OFPXMT_OFB_MPLS_LABEL) |
		((uint64_t)1 << OFPXMT_OFB_MPLS_TC	) |
		((uint64_t)1 << OFPXMT_OFB_PPPOE_CODE) |
		((uint64_t)1 << OFPXMT_OFB_PPPOE_TYPE) |
		((uint64_t)1 << OFPXMT_OFB_PPPOE_SID) |
		((uint64_t)1 << OFPXMT_OFB_PPP_PROT);

uint32_t cfttable::all_instructions =
		(1 << OFPIT_GOTO_TABLE) 	|
		(1 << OFPIT_WRITE_METADATA)	|
		(1 << OFPIT_WRITE_ACTIONS)	|
		(1 << OFPIT_APPLY_ACTIONS)	|
		(1 << OFPIT_CLEAR_ACTIONS)	;

uint32_t cfttable::all_actions =
		(1 << OFPAT_OUTPUT) 		|
		(1 << OFPAT_COPY_TTL_OUT) 	|
		(1 << OFPAT_COPY_TTL_IN) 	|
		(1 << OFPAT_SET_MPLS_TTL) 	|
		(1 << OFPAT_DEC_MPLS_TTL) 	|
		(1 << OFPAT_PUSH_VLAN) 		|
		(1 << OFPAT_POP_VLAN) 		|
		(1 << OFPAT_PUSH_MPLS) 		|
		(1 << OFPAT_POP_MPLS) 		|
		(1 << OFPAT_SET_QUEUE) 		|
		(1 << OFPAT_GROUP) 			|
		(1 << OFPAT_SET_NW_TTL) 	|
		(1 << OFPAT_DEC_NW_TTL)		|
		(1 << OFPAT_SET_FIELD) 		|
		(1 << OFPAT_PUSH_PPPOE)		|
		(1 << OFPAT_POP_PPPOE);


cfttable::cfttable(
		cfttable_owner *owner,
		uint8_t table_id,
		uint32_t max_entries,
		uint64_t match,
		uint64_t wildcards,
		uint32_t write_actions,
		uint32_t apply_actions,
		uint64_t write_setfields,
		uint64_t apply_setfields,
		uint64_t metadata_match,
		uint64_t metadata_write,
		uint32_t instructions,
		uint32_t config) :
			owner(owner),
			table_id(table_id),
			match(match),
			wildcards(wildcards),
			write_actions(write_actions),
			apply_actions(apply_actions),
			write_setfields(write_setfields),
			apply_setfields(apply_setfields),
			metadata_match(metadata_match),
			metadata_write(metadata_write),
			instructions(instructions),
			config(config), // default flow-table behavior: send to controller
			max_entries(max_entries),
			active_count(0),
			lookup_count(0),
			matched_count(0)
{
	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::cfttable() constructor: %s", this, c_str());

	pthread_rwlock_init(&ft_rwlock, NULL);

	cvastring vas(32);
	table_name.assign(vas("table%04d", table_id));
}



cfttable::~cfttable()
{
	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::~cfttable() destructor: %s", this, c_str());

	{
		RwLock lock(&ft_rwlock, RwLock::RWLOCK_WRITE);

		while (not flow_table.empty())
		{
			delete *(flow_table.begin());
		}
	}

	pthread_rwlock_destroy(&ft_rwlock);
}



void
cfttable::reset()
{
	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::reset() ", this);

	RwLock lock(&ft_rwlock, RwLock::RWLOCK_WRITE);
	while (not flow_table.empty())
	{
		WRITELOG(CFTTABLE, DBG, "cfttable::reset() removing %s", (*flow_table.begin())->c_str());
		delete *(flow_table.begin());
	}
}


void
cfttable::handle_timeout(
		int opaque)
{
	switch (opaque) {
#if 0
	case CFTTABLE_TIMER_DELETE_TABLE:
	{
		for (std::set<cftentry*>::iterator
				it = deletion_list.begin(); it != deletion_list.end(); ++it)
		{
			delete (*it);
		}
		deletion_list.clear();
	}
		break;
#endif
	}
}



cftentry*
cfttable::cftentry_factory(
		std::set<cftentry*> *flow_table,
		cofpacket_flow_mod *pack)
{
	return new cftentry(pack->ofh_header->version, this, flow_table, pack);
}


#if 0
void
cfttable::ftentry_idle_for_deletion(
		cftentry *entry)
{
	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::ftentry_idle_for_deletion() "
			"cftentry: %p", this, entry);

	RwLock lock(&ft_rwlock, RwLock::RWLOCK_WRITE);

	/*
	 * all references to entry are gone. It is safe to call entry's destructor now.
	 */
	//owner->cftentry_delete(entry); // is that necessary and useful?
	/*
	 * calling entry's destructor also removes it from our internal flow_table set
	 */
	deletion_list.insert(entry);

	reset_timer(CFTTABLE_TIMER_DELETE_TABLE, 0);
}
#endif



void
cfttable::ftentry_idle_timeout(
		cftentry *entry,
		uint16_t timeout)
{
	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::ftentry_idle_timeout() "
				"cftentry: %p", this, entry);

	/*
	 * cftentry has been marked for deletion
	 * We notify cfttable_owner for any further actions, but do not delete entry here,
	 * as it still might be in use by some packet engines.
	 */
	try {
		// send IDLE-TIMEOUT notification to owner
		owner->cftentry_idle_timeout(entry);

		// remove pointer to entry from our own set of cftentry instances
		flow_table.erase(entry);

	} catch (cerror& e) {}

	// DO NOT SCHEDULE THE ENTRIES DELETION HERE! CFTENTRY WILL TAKE CARE OF ITS REMOVAL AUTONOMOUSLY!
	//entry->schedule_deletion();
}



void
cfttable::ftentry_hard_timeout(
		cftentry *entry,
		uint16_t timeout)
{
	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::ftentry_hard_timeout() "
				"cftentry: %p", this, entry);

	/*
	 * cftentry has been marked for deletion
	 * We notify cfttable_owner for any further actions, but do not delete entry here,
	 * as it still might be in use by some packet engines.
	 */
	try {
		// send HARD-TIMEOUT notification to owner
		owner->cftentry_hard_timeout(entry);

		// remove pointer to entry from our own set of cftentry instances
		flow_table.erase(entry);

	} catch (cerror& e) {}

	// DO NOT SCHEDULE THE ENTRIES DELETION HERE! CFTENTRY WILL TAKE CARE OF ITS REMOVAL AUTONOMOUSLY!
	//entry->schedule_deletion();
}



struct ofp_table_stats*
cfttable::get_table_stats(
		struct ofp_table_stats* table_stats,
		size_t table_stats_len)
throw (eFlowTableInval)
{
	if (table_stats_len < sizeof(struct ofp_table_stats))
		throw eFlowTableInval();

	/* uint8_t  */ table_stats->table_id 		= table_id;
	/* char[32] */ strncpy(table_stats->name, table_name.c_str(), OFP_MAX_TABLE_NAME_LEN);
	/* uint64_t */ table_stats->match 			= htobe64(match);
	/* uint64_t */ table_stats->wildcards 		= htobe64(wildcards);
	/* uint32_t */ table_stats->write_actions 	= htobe32(write_actions);
	/* uint32_t */ table_stats->apply_actions 	= htobe32(apply_actions);
	/* uint64_t */ table_stats->write_setfields = htobe64(write_setfields);
	/* uint64_t */ table_stats->apply_setfields = htobe64(apply_setfields);
	/* uint64_t */ table_stats->metadata_match  = htobe64(metadata_match);
	/* uint64_t */ table_stats->metadata_write  = htobe64(metadata_write);
	/* uint32_t */ table_stats->instructions 	= htobe32(instructions);
	/* uint32_t */ table_stats->config 			= htobe32(config);
	/* uint32_t */ table_stats->max_entries 	= htobe32(max_entries);
	/* uint32_t */ table_stats->active_count 	= htobe32(flow_table.size());
	/* uint64_t */ table_stats->lookup_count 	= htobe64(lookup_count);
	/* uint64_t */ table_stats->matched_count 	= htobe64(matched_count);

	return table_stats;
}


void
cfttable::set_table_stats(
		struct ofp_table_stats* table_stats,
		size_t table_stats_len)
throw (eFlowTableInval)
{
	if (table_stats_len < sizeof(struct ofp_table_stats))
		throw eFlowTableInval();

	if (table_stats->table_id != table_id)
		throw eFlowTableInval();

	table_name.assign(table_stats->name, OFP_MAX_TABLE_NAME_LEN);
	match 			= be64toh(table_stats->match);
	wildcards 		= be64toh(table_stats->wildcards);
	write_actions 	= be32toh(table_stats->write_actions);
	apply_actions 	= be32toh(table_stats->apply_actions);
	write_setfields = be64toh(table_stats->write_setfields);
	apply_setfields = be64toh(table_stats->apply_setfields);
	metadata_match 	= be64toh(table_stats->metadata_match);
	metadata_write 	= be64toh(table_stats->metadata_write);
	instructions 	= be32toh(table_stats->instructions);
	config 			= be32toh(table_stats->config);
	max_entries 	= be32toh(table_stats->max_entries);
	active_count 	= be32toh(table_stats->active_count);
	lookup_count 	= be64toh(table_stats->lookup_count);
	matched_count 	= be64toh(table_stats->matched_count);
}


void
cfttable::get_flow_stats(
		cmemory& body,
		uint32_t out_port,
		uint32_t out_group,
		uint64_t cookie,
		uint64_t cookie_mask,
		cofmatch const& match)
{
	for (std::set<cftentry*>::iterator
			it = flow_table.begin(); it != flow_table.end(); ++it)
	{
		(*it)->get_flow_stats(
						body,
						out_port,
						out_group,
						cookie,
						cookie_mask,
						match);
	}
}


void
cfttable::get_aggregate_flow_stats(
		uint64_t& packet_count,
		uint64_t& byte_count,
		uint64_t& flow_count,
		uint32_t out_port,
		uint32_t out_group,
		uint64_t cookie,
		uint64_t cookie_mask,
		cofmatch const& match)
{
	for (std::set<cftentry*>::iterator
			it = flow_table.begin(); it != flow_table.end(); ++it)
	{
		(*it)->get_aggregate_flow_stats(
						packet_count,
						byte_count,
						flow_count,
						out_port,
						out_group,
						cookie,
						cookie_mask,
						match);

#ifndef NDEBUG
		fprintf(stderr, "cfttable::get_aggregate_flow_stats() "
				"packet_count: 0x%"PRIx64" byte_count: 0x%"PRIx64" flow_count: 0x%"PRIx64,
				packet_count, byte_count, flow_count);
#endif
	}
}


bool
cfttable::policy_table_miss_controller()
{
	return (config == OFPTC_TABLE_MISS_CONTROLLER);
}


bool
cfttable::policy_table_miss_continue()
{
	return (config == OFPTC_TABLE_MISS_CONTINUE);
}


bool
cfttable::policy_table_miss_drop()
{
	return (config == OFPTC_TABLE_MISS_DROP);
}


const char*
cfttable::c_str()
{
	cvastring vas(4096);
	info.append(vas("cfttable(%p)::flow_table table_id:%d config: %d =>\n",
			this, table_id, config));
	int i = 0;
	std::set<cftentry*>::iterator it;
	for (it = flow_table.begin(); it != flow_table.end(); ++it)
	{
		//info.append(vas("  %s\n\n", (*it)->c_str()));
		info.append(vas("  [%d]: %s\n", i++, (*it)->c_str()));
	}
	return info.c_str();
}


std::set<cftentry*>
cfttable::find_best_matches(
		cpacket* pack)
{
	/*
	 * strategy: unfortunately, OpenFlow does not define a control logic
	 * like iptables, all entries must be checked, i.e. we need a brute
	 * force search here. shit ... :(
	 *
	 * for all relevant fields as defined by the packet classification:
	 * 1.1. count the number of hits (exact match)
	 * 1.2. count the number of hits (wildcard match)
	 * 1.3. count the number of misses (no match)
	 *
	 * 2. build the sum of hits (exact + wildcard match)
	 *
	 * 3. from all flow table entries with the highest number of hits,
	 *    choose the one with the highest priority
	 *
	 * TODO(1): parallelize that operation, map and reduce on several CPU
	 *					cores with threads
	 *
	 * TODO(2): we need some form of caching to speed up this operation,
	 *				  once a flow has started, all follow-up packets should be switched
	 *          more faster
	 *
	 * TODO(3): what about some clever indexing, so Ethernet-only rules, IP-only rules
	 *          etc. are organized to reduce the entry space we have to check, very
	 *          similar to what openldap with indexing provides?
	 */


	try {

		WRITELOG(CFTTABLE, DBG, "cfttable(%p)::find_best_matches() #flow_table.entries=%d "
								 "dataframe: %s", this, flow_table.size(), pack->c_str());

		// calculates the qualifier for ethernet packet pointed to by iovec = {data, datalen}
		cftsearch ftsearch(pack);

		{
			RwLock lock(&ft_rwlock, RwLock::RWLOCK_READ);

			// for_each() takes a copy, not a reference, so we have to assign its return value back to ftsearch!
			ftsearch = for_each(flow_table.begin(), flow_table.end(), ftsearch);
		}

		WRITELOG(CFTTABLE, DBG, "cfttable(%p)::find_best_matches() %s", this, ftsearch.c_str());

		lookup_count++;

		if (not ftsearch.matching_entries.empty())
		{
			matched_count++;
		}

		/* search.macthing_entries contains now a list of matching cftentries with the highest
		 * number of hits and the same priority
		 */

		return ftsearch.matching_entries;

	} catch (eFlowTableClassificationFailed& e) {
		WRITELOG(CFTTABLE, WARN, "unable to classify packet");
	}

	std::set<cftentry*> empty;
	return empty;
}



cftentry*
cfttable::update_ft_entry(
		cfttable_owner *owner,
		cofpacket_flow_mod *pack) throw (eFlowTableInval)
{
	if ((OFPT_FLOW_MOD != pack->ofh_header->type) || (not pack->is_valid()))
	{
		throw eFlowTableInval();
	}

	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::update_ft_entry()", this);

	switch (pack->get_command()) {
	case OFPFC_ADD:
		return add_ft_entry(owner, pack);

	case OFPFC_MODIFY:
		return modify_ft_entry(owner, pack, false /* not strict */);

	case OFPFC_MODIFY_STRICT:
		return modify_ft_entry(owner, pack, true /* strict */);

	case OFPFC_DELETE:
		rem_ft_entry(owner, pack, false /* not strict */); break;

	case OFPFC_DELETE_STRICT:
		rem_ft_entry(owner, pack, true /* strict */); break;

	default:
		WRITELOG(CFTTABLE, WARN, "unknown flow mod command [%d] received",
				 pack->get_command()); break;
	}
	return (cftentry*)0;
}



cftentry*
cfttable::update_ft_entry(
		cfttable_owner *owner,
		cofpacket_flow_removed *pack) throw (eFlowTableInval)
{
	if ((OFPT_FLOW_MOD != pack->ofh_header->type) || (not pack->is_valid()))
	{
		throw eFlowTableInval();
	}

	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::update_ft_entry()", this);

	return (cftentry*)0;
}




cftentry*
cfttable::add_ft_entry(
		cfttable_owner *owner,
		cofpacket_flow_mod *pack) throw(eFlowTableEntryOverlaps)
{
	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::add_ft_entry()", this);
	//dump_ftentries();

	class cftentry * fte = NULL;
	std::set<cftentry*>		delete_table;	// list of cftentry instances scheduled for deletion by mgmt action

	{
		RwLock lock(&ft_rwlock, RwLock::RWLOCK_WRITE);


		if (not flow_table.empty())
		{
			// check for OFPFF_CHECK_OVERLAP
			if (pack->get_flags() & OFPFF_CHECK_OVERLAP)
			{
				WRITELOG(CFTTABLE, DBG, "cfttable(%p)::add_ft_entry() "
						"checking for overlapping entries", this);

				if (find_if(flow_table.begin(), flow_table.end(),
						cftentry::ftentry_find_overlap(pack->match, false /* not strict */)) != flow_table.end())
				{
					WRITELOG(CFTTABLE, DBG, "cfttable(%p)::add_ft_entry() "
							"ftentry overlaps, sending error message back", this);

					// throw exception, calling instance has to send error message to controller
					//throw eFlowTableEntryOverlaps();
					throw eFlowModOverlap();
				}
			}

			// remove any duplicate ft-entry (equals strictly new ft-entry)
			std::set<cftentry*>::iterator it = flow_table.begin();
			while ((it = find_if(it, flow_table.end(),
						cftentry::ftentry_find_overlap(pack->match, true /* strict */))) != flow_table.end())
			{
				WRITELOG(CFTTABLE, DBG, "cfttable(%p)::add_ft_entry() deleting duplicate %p", this, (*it));
				(*it)->disable_entry();
				delete_table.insert(*it);
				++it;
			}
		}
		// TODO: check for invalid ports


		WRITELOG(CFTTABLE, DBG, "cfttable(%p)::add_ft_entry() [1]\n %s", this, c_str());

		// inserts automatically to this->flow_table
		fte = cftentry_factory(&flow_table, pack);
	}



	/*
	 * we cannot simply remove a cftentry instance, as it might be in use by some packet engine.
	 * Therefore, we mark all cftentry instances for deletion in std::set delete_table.
	 * This will result in calls to cfttable::cftentry_idle_for_deletion() where we can
	 * safely destroy these entries.
	 */
	if (not delete_table.empty())
	{
		for (std::set<cftentry*>::iterator
				it = delete_table.begin(); it != delete_table.end(); ++it)
		{
			flow_table.erase(*it);
			(*it)->schedule_deletion();
		}
		delete_table.clear();
	}

	update_group_ref_counts(fte);

	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::add_ft_entry() [2]\n %s", this, c_str());

	return fte;
}


cftentry*
cfttable::modify_ft_entry(
		cfttable_owner *owner,
		cofpacket_flow_mod *pack,
		bool strict /* = false (default) */)
{
	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::modify_ft_entry()", this);
	//dump_ftentries();

	RwLock lock(&ft_rwlock, RwLock::RWLOCK_WRITE);

	bool noupdates = true;

	std::set<cftentry*>::iterator it = flow_table.begin();
	class cftentry* fte = NULL;

	while (it != flow_table.end())
	{
		if ((it = find_if(it, flow_table.end(),
				cftentry::ftentry_find_overlap(pack->match, strict /* strict */))) != flow_table.end())
		{
			(*it)->update_flow_mod(pack);
			update_group_ref_counts((*it));
			noupdates = false;


			fte = (*it);

			++it;
		}
	}

	// if no updates were done, add new entry
	if (noupdates)
	{
		return add_ft_entry(owner, pack);
	}

	WRITELOG(CFTTABLE, DBG, "cfttable::modify_ft_entry()\n %s", fte->c_str());

	//dump_ftentries();

	return fte;
}

#if 0
void
cfttable::rem_ft_entry(
		cfttable_owner *owner,
		cofpacket* pack,
		bool strict /* = false (default) */)
{
	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::rem_ft_entry() pack->match: %s", this, pack->match.c_str());
	//dump_ftentries();

	RwLock lock(&ft_rwlock, RwLock::RWLOCK_WRITE);

	std::set<cftentry*>::iterator it;
	std::set<cftentry*>::iterator begin = flow_table.begin();

	while ((it = find_if(begin, flow_table.end(),
				cftentry::ftentry_find_overlap(pack->match, strict /* strict */))) != flow_table.end())
	{
		// (*it) points now to a cftentry instance whose flow_mod matches
		// the one received in FlowMod(DELETE_...)

		WRITELOG(CFTTABLE, DBG, "cfttable(%p)::rem_ft_entry() "
				"matching entry, checking out_port and out_group: %s",
				this, (*it)->c_str());

		// ok, both out_port and out_group are *_ANY, i.e. remove the cftentry instance
		if ((be32toh(pack->ofh_flow_mod->out_port) == OFPP_ANY) &&
				(be32toh(pack->ofh_flow_mod->out_group) == OFPG_ANY))
		{
			// remove the instance pointed to by (*it) (see below)
			goto delete_entry;
		}
		// if there is at least one action referring to flow_mod->out_port, remove it
		else if (be32toh(pack->ofh_flow_mod->out_group) == OFPG_ANY)
		{
			// find all OFPAT_OUTPUT actions ...



			// ... in OFPIT_APPLY_ACTIONS
			try {
				cofaclist& actions =  (*it)->find_inst(OFPIT_APPLY_ACTIONS).actions;
				cofaclist::iterator at;

				for (at = actions.begin(); at != actions.end(); ++at)
				{
					cofaction& action = (*at);

					if (be16toh(action.oac_header->type) == OFPAT_OUTPUT)
					{
						if (be32toh(action.oac_output->port) == be32toh(pack->ofh_flow_mod->out_port))
						{
							goto delete_entry;
						}
					}
				}

			}
			catch (eFteInstNotFound& e) { }



			// ... in OFPIT_WRITE_ACTIONS
			try {

				// ... in OFPIT_WRITE_ACTIONS (we're lucky here: only a single OFPAT_OUTPUT can exist here!)
				if (be32toh(
						(*it)->find_inst(OFPIT_WRITE_ACTIONS).find_action(OFPAT_OUTPUT).oac_output->port) ==
										be32toh(pack->ofh_flow_mod->out_port))
				{
					goto delete_entry;
				}

			}
			catch (eFteInstNotFound& e) { }
			catch (eInstructionActionNotFound& e) { }



		}
		else if (be32toh(pack->ofh_flow_mod->out_port) == OFPP_ANY)
		{
			// find all OFPAT_GROUP actions ...

			// ... in OFPIT_APPLY_ACTIONS
			try {
				cofaclist& actions =  (*it)->find_inst(OFPIT_APPLY_ACTIONS).actions;
				cofaclist::iterator at;

				for (at = actions.begin(); at != actions.end(); ++at)
				{
					cofaction& action = (*at);

					if (be16toh(action.oac_header->type) == OFPAT_GROUP)
					{
						if (be32toh(action.oac_group->group_id) == be32toh(pack->ofh_flow_mod->out_group))
						{
							goto delete_entry;
						}
					}
				}

			}
			catch (eFteInstNotFound& e) { }

			// ... in OFPIT_WRITE_ACTIONS
			try {

				// ... in OFPIT_WRITE_ACTIONS (we're lucky here: only a single OFPAT_OUTPUT can exist here!)
				if (be32toh((*it)->find_inst(OFPIT_WRITE_ACTIONS).find_action(OFPAT_GROUP).oac_group->group_id)
						== be32toh(pack->ofh_flow_mod->out_group))
				{

					goto delete_entry;
				}

			}
			catch (eFteInstNotFound& e) { }
			catch (eInstructionActionNotFound& e) { }
		}

		begin = ++it;
		continue;

delete_entry:

		WRITELOG(CFTTABLE, DBG, "cfttable(%p)::rem_ft_entry() REMOVE %s", this, (*it)->c_str());

		update_group_ref_counts((*it), false /* decrement reference count */);


		(*it)->schedule_deletion();

		flow_table.erase(*it);

		begin = flow_table.begin();
	}

	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::rem_ft_entry() cfttable.size():%d", this, flow_table.size());

	//dump_ftentries();
}
#endif


void
cfttable::rem_ft_entry(
		cfttable_owner *owner,
		cofpacket_flow_mod* pack,
		bool strict /* = false (default) */)
{
	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::rem_ft_entry() pack->match: %s", this, pack->match.c_str());
	//dump_ftentries();

	RwLock lock(&ft_rwlock, RwLock::RWLOCK_WRITE);

	std::set<cftentry*> delete_table;
	uint32_t out_port 	= pack->get_out_port();
	uint32_t out_group 	= pack->get_out_group();

	for (std::set<cftentry*>::iterator it = flow_table.begin();
			it != flow_table.end(); ++it) {
		cftentry* entry = (*it);
		if (not entry->overlaps(pack->match, strict)) {
			continue;
		}

		if ((OFPP_ANY == out_port) && (OFPG_ANY == out_group)) {
			entry->disable_entry();
			delete_table.insert(entry);
		} else if (OFPG_ANY == out_group) {
			// find all OFPAT_OUTPUT actions ...

			// ... in OFPIT_APPLY_ACTIONS
			try {
				cofaclist& actions =  (*it)->find_inst(OFPIT_APPLY_ACTIONS).actions;
				cofaclist::iterator at;

				for (at = actions.begin(); at != actions.end(); ++at) {
					cofaction& action = (*at);

					if (be16toh(action.oac_header->type) == OFPAT_OUTPUT) {
						if (be32toh(action.oac_output->port) == out_port) {
							entry->disable_entry();
							delete_table.insert(entry);
						}
					}
				}
			} catch (eFteInstNotFound& e) {
			}

			// ... in OFPIT_WRITE_ACTIONS
			try {
				// ... in OFPIT_WRITE_ACTIONS (we're lucky here: only a single OFPAT_OUTPUT can exist here!)
				if (be32toh(
						entry->find_inst(OFPIT_WRITE_ACTIONS).
								find_action(OFPAT_OUTPUT).oac_output->port) == out_port) {
					entry->disable_entry();
					delete_table.insert(entry);
				}
			} catch (eFteInstNotFound& e) {
			} catch (eInstructionActionNotFound& e) {
			}
		} else if (OFPP_ANY == out_port) {
			// find all OFPAT_GROUP actions ...

			// ... in OFPIT_APPLY_ACTIONS
			try {
				cofaclist& actions =  (*it)->find_inst(OFPIT_APPLY_ACTIONS).actions;
				cofaclist::iterator at;
				for (at = actions.begin(); at != actions.end(); ++at) {
					cofaction& action = (*at);
					if (be16toh(action.oac_header->type) == OFPAT_GROUP) {
						if (be32toh(action.oac_group->group_id) == out_group) {
							entry->disable_entry();
							delete_table.insert(entry);
						}
					}
				}
			} catch (eFteInstNotFound& e) {
			}

			// ... in OFPIT_WRITE_ACTIONS
			try {
				// ... in OFPIT_WRITE_ACTIONS (we're lucky here: only a single OFPAT_OUTPUT can exist here!)
				if (be32toh((*it)->find_inst(OFPIT_WRITE_ACTIONS).
						find_action(OFPAT_GROUP).oac_group->group_id) == out_group) {
					entry->disable_entry();
					delete_table.insert(entry);
				}
			} catch (eFteInstNotFound& e) {
			} catch (eInstructionActionNotFound& e) {
			}
		}
	}

	/*
	 * we cannot simply remove a cftentry instance, as it might be in use by some packet engine.
	 * Therefore, we mark all cftentry instances for deletion in std::set delete_table.
	 * This will result in calls to cfttable::cftentry_idle_for_deletion() where we can
	 * safely destroy these entries.
	 */
	if (not delete_table.empty())
	{
		for (std::set<cftentry*>::iterator
				it = delete_table.begin(); it != delete_table.end(); ++it)
		{
			flow_table.erase(*it);
			(*it)->schedule_deletion();
		}
		delete_table.clear();
	}

	WRITELOG(CFTTABLE, DBG, "cfttable(%p)::rem_ft_entry() cfttable.size():%d", this, flow_table.size());

	//dump_ftentries();
}


void
cfttable::set_config(
		uint32_t config)
{
	RwLock lock(&ft_rwlock, RwLock::RWLOCK_WRITE);
	this->config = config;
}





std::set<cftentry*>
cfttable::find_ft_entries(cofmatch const& match)
{
	std::set<cftentry*> replies;

	RwLock lock(&ft_rwlock, RwLock::RWLOCK_READ);

	std::set<cftentry*>::iterator it;
	for (it = flow_table.begin(); it != flow_table.end(); ++it)
	{
		if ((*it)->ofmatch.overlaps(match, true /* strict */))
		{
			replies.insert((*it));
		}
	}

	return replies;
}



void
cfttable::update_group_ref_counts(
		cftentry *fte,
		bool inc)
{

	/*
	 * check for references to group table entries (OFPIT_APPLY_ACTIONS)
	 */
	try {
		cofinst& inst =	fte->instructions.find_inst(OFPIT_APPLY_ACTIONS);

		for (cofaclist::iterator
				it = inst.actions.begin(); it != inst.actions.end(); ++it)
		{
			cofaction& action = (*it);
			if (OFPAT_GROUP != action.get_type())
			{
				continue;
			}


			uint32_t group_id = be32toh(action.oac_group->group_id);
			if (inc)
			{
				owner->inc_group_reference_count(group_id, fte);
			}
			else
			{
				owner->dec_group_reference_count(group_id, fte);
			}

			//WRITELOG(CFTTABLE, DBG, "cfttable(%p)::update_group_ref_cnts() %s",
			//		this, fwdelem->group_table[group_id]->c_str());
		}

	} catch (eInListNotFound& e) { }


	/*
	 * check for references to group table entries (OFPIT_WRITE_ACTIONS)
	 */
	try {
		cofinst& inst =	fte->instructions.find_inst(OFPIT_WRITE_ACTIONS);

		for (cofaclist::iterator
				it = inst.actions.begin(); it != inst.actions.end(); ++it)
		{
			cofaction& action = (*it);
			if (OFPAT_GROUP != action.get_type())
			{
				continue;
			}

			uint32_t group_id = be32toh(action.oac_group->group_id);
			if (inc)
			{
				owner->inc_group_reference_count(group_id, fte);
			}
			else
			{
				owner->dec_group_reference_count(group_id, fte);
			}

			//WRITELOG(CFTTABLE, DBG, "cfttable(%p)::update_group_ref_cnts() %s",
			//		this, fwdelem->group_table[group_id]->c_str());
		}

	} catch (eInListNotFound& e) { }


}




/*
 *
 * class cftsearch
 *
 */


cftsearch::cftsearch(
		cpacket* pack) :
	pack(pack),
	__exact_hits(0),
	__wildcard_hits(0),
	__max_hits(0),
	__priority(0)
{
	WRITELOG(CFTTABLE, DBG, "cftsearch(%p)::cftsearch()", this);
}


cftsearch::~cftsearch()
{
	WRITELOG(CFTTABLE, DBG, "cftsearch(%p)::~cftsearch()", this);
}


const char*
cftsearch::c_str()
{
	cvastring vas(1024);
	info.assign(vas("cftsearch(%p)::cftsearch() hits => max:%d exact:%d wildcard:%d priority:%d\n"
			"dataframe: %s\n",
			this,
			__max_hits,
			__exact_hits,
			__wildcard_hits,
			__priority,
			pack->c_str()));

	std::set<cftentry*>::iterator it;
	for (it = matching_entries.begin(); it != matching_entries.end(); ++it)
	{
		info.append(vas("  %s", (*it)->c_str()));
	}

	return info.c_str();
}


void
cftsearch::operator() (
		cftentry *fte)
{
	if (0 == fte)
	{
		WRITELOG(CFTSEARCH, DBG, "cftsearch(%p)::operator() ignoring NULL pointer ", this);

		return;
	}

	if (fte->flags.test(cftentry::CFTENTRY_FLAG_TIMER_EXPIRED))
	{
		WRITELOG(CFTSEARCH, DBG, "cftsearch(%p)::operator() cftentry scheduled "
				"for removal, ignoring => cftentry:% p", this, fte);
		return;
	}

	uint16_t exact_hits = 0;
	uint16_t wildcard_hits = 0;
	uint16_t missed = 0;

	//pack->calc_hits(fte->ofmatch, exact_hits, wildcard_hits, missed);
	fte->ofmatch.is_matching(pack->match, exact_hits, wildcard_hits, missed);


	// get sum of exact hits and wildcard hits
	uint16_t hits = exact_hits + wildcard_hits;


	// no hits at all or at least one missed entry, return
	if ((hits == 0) || (missed > 0))
		return;

	WRITELOG(CFTSEARCH, DBG, "cftsearch(%p)::operator() results for cftentry:%p => "
			"hits => max:%d exact:%d wildcard:%d missed:%d priority:%d",
			this,
			fte,
			hits,
			exact_hits,
			wildcard_hits,
			missed,
			fte->get_priority());


	if ((hits > __max_hits) ||
		((hits == __max_hits) && (exact_hits > __exact_hits))) // new best hit
	{
		try {
			fte->sem_inc();					// increment fte semaphore by one

			WRITELOG(CFTSEARCH, DBG, "cftsearch(%p)::operator() new best hit cftentry:%p", this, fte);

			matching_entries.clear();		// remove all old entries from pkb->matches
			matching_entries.insert(fte);	// add fte as only entry to pkb->matches

			__max_hits = hits;
			__exact_hits = exact_hits;
			__wildcard_hits = wildcard_hits;
			__priority = fte->get_priority();

		} catch (eFtEntryUnAvail& e) {
			WRITELOG(CFTSEARCH, DBG, "cftsearch(%p)::operator() ignoring entry as it is unavailable => cftentry:%p", this, fte);
			return;
		}
	}
	else if ((hits == __max_hits) && (exact_hits == __exact_hits))  // also a best hit, check priority
	{
		try {
			fte->sem_inc();						// mark cftentry instance fte as being in use by cfwdengine

			if (fte->get_priority() > __priority)    // better priority
			{
				// remove all old entries from matching_entries
				matching_entries.clear();

				__priority = fte->get_priority();

				WRITELOG(CFTSEARCH, DBG, "cftsearch(%p)::operator() new best hit with higher priority => cftentry:%p", this, fte);
			}
			else if (fte->get_priority() == __priority)   // same match and priority
			{
				WRITELOG(CFTSEARCH, DBG, "cftsearch(%p)::operator() identical hit with same hits and priority => cftentry:%p", this, fte);
			}
			else   // lower priority, do nothing
			{
				WRITELOG(CFTSEARCH, DBG, "cftsearch(%p)::operator() ignoring entry with lower priority => cftentry:%p", this, fte);

				return;
			}

			matching_entries.insert(fte); 		// add additional entry to pkb->matches

		} catch (eFtEntryUnAvail& e) {
			WRITELOG(CFTSEARCH, DBG, "cftsearch(%p)::operator() ignoring entry as it is unavailable => cftentry:%p", this, fte);
			return;
		}
	}
	else // hits < __max_hits
	{
		WRITELOG(CFTSEARCH, DBG, "cftsearch(%p)::operator() ignoring entry with lower score => cftentry:%p", this, fte);

		return;
	}
}


