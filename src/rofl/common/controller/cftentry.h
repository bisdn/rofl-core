/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFTENTRY_H
#define CFTENTRY_H 1

#include <set>
#include <map>
#include <list>
#include <vector>
#include <bitset>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

#include "../openflow/openflow12.h"
#include <inttypes.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>

#ifdef __cplusplus
}
#endif

#include "../cclock.h"
#include "../cmemory.h"
#include "../cerror.h"
#include "../ciosrv.h"
#include "../cvastring.h"
#include "../thread_helper.h"
//#include "../cfwdelem.h"

#include "../openflow/cofmatch.h"
#include "../openflow/cofaction.h"
#include "../openflow/cofinlist.h"
#include "../openflow/cofpacket.h"
#include "../openflow/cofinst.h"

#include "../protocols/fetherframe.h"

class cfwdelem;
class cofctl;


/** we have to notify fpc entry generating instances
 * if we have to remove an entry due to lack of space on the TCAMs.
 */
class hw_fte_cb {
public:
	/** destructor */
	virtual ~hw_fte_cb() { };
	/** callback for sending notifications to our caller */
	virtual int
	hw_fte_rmvd(int fte_handle, int cause = 0) = 0;
};




class eFteBase : public cerror {};
class FteHwInsertFailed : public eFteBase {};
class FteHwDestroyFailed : public eFteBase {};
class eFteActionBadLen : public eFteBase {};
class eFteInstNotFound : public eFteBase {};
class eFteInvalid : public eFteBase {};
class eFteUnAvail : public eFteBase {};


// forward declaration
class cftentry_owner;



/**
 *
 *
 *
 */
class cftentry :
	public ciosrv,
	public hw_fte_cb
{
public:
	/** constructor
	 */
	cftentry(
			cfwdelem *fwdelem = NULL,
			cofctl *ofctrl = NULL);

	/** constructor
	 */
	cftentry(
		cftentry_owner *owner,
		std::set<cftentry*> *flow_table,
		cofpacket *pack,
		cfwdelem *fwdelem = NULL,
		cofctl *ofctrl = NULL);

	/** destructor
  	 */
	virtual
	~cftentry();

	/**
	 * copy constructor
	 */
	cftentry(const cftentry& fte);

	/**
	 * assignment operator
	 */
	cftentry&
	operator=(const cftentry& fte);

	/**
	 * dump ft-entry info
	 */
	const char* c_str();
	const char* s_str();

	/** find a specific instruction type
	 */
	cofinst&
	find_inst(enum ofp_instruction_type type);

	/** semaphore like method incrementing internal counter by one
	 * prevents this fte to be removed or updated, while a cfwdengine accesses
	 * this object
	 */
	void
	sem_inc() throw (eFteUnAvail);

	/** decrements the internal semaphore
	 *
	 */
	void
	sem_dec();

	/** erase this element thread safe
	 *
	 */
	void
	erase();


public:


	/**
	 * checks for overlaps in this and the other entry
	 */
	bool overlaps(cofmatch& match, bool strict = false);


	/**
	 * called, when this flow entry is selected for forwarding
	 */
	void used(
			cpacket& pack);



	//
	// methods called from class cpcp_dpath
	//

	/**
	 * update struct ofp_flow_mod
	 */
	virtual void
	update_flow_mod(cofpacket *pack) throw (eFteInvalid);

	/**
	 * send flow stats reply for this entry
	 */
	virtual void ofp_send_flow_stats(
		uint64_t dpid,
		uint32_t xid,
		uint16_t type,
		uint16_t flags,
		struct ofp_flow_stats_request *req,
		size_t reqlen);

	/**
	 * send flow stats reply for this entry
	 */
	void
	handle_stats_request(
		struct ofp_flow_stats **flow_stats,
		size_t *flow_stats_len);


	/** OFPST_FLOW_STATS
	 *
	 */
	void
	get_flow_stats(
				cmemory& body,
				uint32_t out_port,
				uint32_t out_group,
				uint64_t cookie,
				uint64_t cookie_mask,
				cofmatch const& match);


	/** OFPST_AGGREGATE_STATS
	 *
	 */
	void
	get_aggregate_flow_stats(
				uint64_t& packet_count,
				uint64_t& byte_count,
				uint64_t& flow_count,
				uint32_t out_port,
				uint32_t out_group,
				uint64_t cookie,
				uint64_t cookie_mask,
				cofmatch const& match);


	/**
	 *
	 */
	static void
	test();

public: // overloaded from hw_fte_cb

	/** receive notifications from hw_fte instance
	 */
	virtual int 
	hw_fte_rmvd(int fte_handle, int cause = 0);

	// cfwdelem that generated this instance
	cfwdelem *fwdelem;

	// cofctrl that generated this instance
	cofctl *ofctrl;
	
	//Unique id of the flowspace (usually used for transactions)
	uint64_t uid;



	// flags for this cpkbuff instance
	std::bitset<32> flags;
#define CPKBUFF_COPY_ON_WRITE   0x00000001      // copy packet before queueing on outgoing port

	enum cftentry_event_t {
		CFTENTRY_EVENT_DELETE_THIS = 1,
	};


#if 0
	// map of pointers to action structures
	std::list<struct ofp_action_header*> actions;
#endif


	// list of instructions
	cofinlist instructions;
	

	// reason for removing this flow entry
	uint8_t removal_reason;
	// time when this flow entry was created
	cclock flow_create_time;
	// number of packets handled by this ft-entry
	uint64_t rx_packets;
	// number of bytes handled by this ft-entry
	uint64_t rx_bytes;
	// output port for this ft-entry
	uint32_t out_port;
	// output group for this ft-entry
	uint32_t out_group;
	// cofmatch instance containing struct ofp_match for this ftentry
	cofmatch ofmatch;
	// memory area for storing the generic part of ofp_flow_mod
	cmemory m_flowmod;
	// copy of ofp_flow_mod structure (network byte order)
	struct ofp_flow_mod *flow_mod;

	enum cftentry_flag_t {
		CFTENTRY_COPY_ON_WRITE = (1 << 0),
		CFTENTRY_FLAG_DELETE_THIS = (1 << 1),
	};


protected: // overwritten from class cfwdelem

	/**
	 * handle event
	 */
	virtual void
	handle_event(cevent const& ev);

	/**
	 * handle timeout
	 */
	virtual void
	handle_timeout(int opaque);

	/** timer types for cftentry
	 *
	 */
	enum cftentry_timer_t {
		TIMER_FTE_IDLE_TIMEOUT = 1,
		TIMER_FTE_HARD_TIMEOUT = 2,
	};


protected:

	// ptr to entity owning this cftentry
	cftentry_owner *owner;

	// ptr to flow_table this entry belongs to
	std::set<cftentry*> *flow_table;

private: // methods

	/** parse actions from ofp_flow_mod and store in instvec
	 */
	void __update();

	/** prepare const info string
	 */
	void make_info();

private: // data structures

	pthread_mutex_t ftmutex; // mutex for this ftentry
	int ftsem; // semaphore counter
	std::string info; // info string

public:

	/** predicate for finding overlapping entries
	 */
	class ftentry_find_overlap {
	public:
		ftentry_find_overlap(cofmatch const& m, bool s) :
			ofmatch(m), strict(s) { };
		bool operator() (cftentry* fte) {
			return (fte->ofmatch.overlaps(ofmatch, strict /* strict */) &&
						(not fte->flags.test(CFTENTRY_EVENT_DELETE_THIS)));
		}
		cofmatch ofmatch;
		bool strict;
	};

};

/** callback class for sending notifications to cftentry owning entities
 */
class cftentry_owner {
public:
	virtual ~cftentry_owner() {};

	virtual void ftentry_timeout(cftentry *entry, uint16_t timeout) = 0;
};
#endif
