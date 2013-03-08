/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFTTABLE_H
#define CFTTABLE_H 1

#include <string>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <inttypes.h>

#ifdef __cplusplus
}
#endif

#include "../common/ciosrv.h"
#include "../common/cerror.h"
#include "../common/cvastring.h"
#include "../common/thread_helper.h"


#include "../common/openflow/cofpacket.h"
#include "cftentry.h"

#if 0
#include "../common/protocols/fdataframe.h"
#endif

namespace rofl
{

/* error classes */
class eFlowTableInval 					: public cerror {}; // invalid flow-mod entry received
class eFlowTableNoMatch 				: public cerror {}; // no matching entry found
class eFlowTableEntryOverlaps 			: public cerror {}; // entry overlaps with existing entry in flow-table
class eFlowTableClassificationFailed 	: public cerror {}; // invalid packet frame for classification



class cfttable_owner {
public:
	virtual ~cfttable_owner() {};
	/**
	 *
	 */
	virtual void
	inc_group_reference_count(uint32_t groupid, cftentry *fte) = 0;
	/**
	 *
	 */
	virtual void
	dec_group_reference_count(uint32_t groupid, cftentry *fte) = 0;
	/**
	 *
	 */
	virtual void
	cftentry_idle_timeout(cftentry *fte) = 0;
	/**
	 *
	 */
	virtual void
	cftentry_hard_timeout(cftentry *fte) = 0;
#if 0
	/**
	 *
	 */
	virtual void
	cftentry_delete(cftentry *fte) = 0;
#endif
};



/**
 *
 *
 *
 */
class cfttable :
	public ciosrv,
	public cftentry_owner
{
private: // data structures

		enum cfttable_timer_t {
			CFTTABLE_TIMER_BASE 			= (0x551e << 16),
			CFTTABLE_TIMER_DELETE_TABLE 	= ((CFTTABLE_TIMER_BASE) + 0x0001),
		};

	cfttable_owner 			*owner; 		// owning instance of this cfttable (or NULL)
	std::string 			info; 			// info string
	pthread_rwlock_t 		ft_rwlock; 		// rwlock for this flowtable
#if 0
	std::set<cftentry*>		deletion_list;
#endif

public: // data structures

	static uint64_t 		all_matches;
	static uint32_t 		all_instructions;
	static uint32_t 		all_actions;

	std::set<cftentry*> 	flow_table; 	// flow_table: set of cftentries
	uint8_t 				table_id; 		// OF1.1 table identifier
	std::string 			table_name; 	// OF1.1 table name

	uint64_t 				match; 			// OF1.2 available matching fields
	uint64_t 				wildcards; 		// OF1.2 wildcards
	uint32_t 				write_actions; 	// OF1.1 supported actions for instruction WRITE_ACTIONS
	uint32_t 				apply_actions; 	// OF1.1 supported actions for instruction APPLY_ACTIONS
	uint64_t 				write_setfields;// OF1.2 supported SET_FIELD actions for instruction WRITE_ACTIONS
	uint64_t 				apply_setfields;// OF1.2 supported SET_FIELD actions for instruction APPLY_ACTIONS
	uint64_t 				metadata_match;	// OF1.2 Bits of metadata table can match.
	uint64_t 				metadata_write;	// OF1.2 Bits of metadata table can write.
	uint32_t 				instructions; 	// OF1.1 supported instructions
	uint32_t 				config; 		// flow-table configuration (OF1.1 OFPTC_* flags)
	uint32_t 				max_entries;	// max number of flow-table entries
	uint32_t 				active_count;  	// current number of flow-table entries (received from physical data path, flow_table.sie() otherwise!)
	uint64_t 				lookup_count; 	// number of lookups in this table
	uint64_t 				matched_count; 	// number of table hits


public:


	/** constructor
	 *
	 */
	cfttable(
			cfttable_owner *owner = (cfttable_owner*)0,
			uint8_t table_id = 0,
			uint32_t max_entries = 1024,
			uint64_t match = cfttable::all_matches,
			uint64_t wildcards = cfttable::all_matches,
			uint32_t write_actions = cfttable::all_actions,
			uint32_t apply_actions = cfttable::all_actions,
			uint64_t write_setfields = cfttable::all_matches,
			uint64_t apply_setfields = cfttable::all_matches,
			uint64_t metadata_match = 0,
			uint64_t metadata_write = 0,
			uint32_t instructions = cfttable::all_instructions,
			uint32_t config = OFPTC_TABLE_MISS_CONTROLLER);


	/** destructor
	 *
	 */
	virtual
	~cfttable();


	/**
	 * remove all cftentry instances from this flow-table
	 */
	void
	reset();


	/** return info string for this object
	 */
	const char*
	c_str();


	/**
	 *
	 */
	virtual cftentry*
	cftentry_factory(
			std::set<cftentry*> *flow_table,
			cofpacket_flow_mod *pack);


	/** returns true for table policy OFPTC_TABLE_MISS_CONTROLLER
	 *
	 */
	bool
	policy_table_miss_controller();


	/** returns true for table policy OFPTC_TABLE_MISS_CONTINUE
	 *
	 */
	bool
	policy_table_miss_continue();


	/** returns true for table policy OFPTC_TABLE_MISS_DROP
	 *
	 */
	bool
	policy_table_miss_drop();


	/** return table statistics
	 */
	struct ofp_table_stats*
	get_table_stats(
			struct ofp_table_stats* table_stats,
			size_t table_stats_len) throw (eFlowTableInval);

	/** set table statistics gathered from physical data path
	 */
	void
	set_table_stats(
			struct ofp_table_stats* table_stats,
			size_t table_stats_len) throw (eFlowTableInval);

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


	/** find all cftentries from this flowtable that match pack
	 */
	std::set<cftentry*>
	find_best_matches(
			cpacket* pack);

	/**
	 * add, update, remove flow table entry based on flow_mod
	 */
	cftentry*
	update_ft_entry(
			cfttable_owner *owner,
			cofpacket_flow_mod *pack) throw (eFlowTableInval);

	/**
	 * add, update, remove flow table entry based on flow_mod
	 */
	cftentry*
	update_ft_entry(
			cfttable_owner *owner,
			cofpacket_flow_removed *pack) throw (eFlowTableInval);


	/**
	 * find all cftentries that match a given struct ofp_match
	 * @return std::set<cftentry*>
	 */
	std::set<cftentry*>
	find_ft_entries(cofmatch const& match);


	/**
	 *
	 */
	void
	set_config(
			uint32_t config);


public: // overloaded from cftentry


#if 0
	/**
	 * @name	ftentry_delete
	 * @brief  	notifies owner about a timeout event for this cftentry instance
	 *
	 * This method is called, when a timeout
	 *
	 */
	virtual void
	ftentry_idle_for_deletion(cftentry *entry);
#endif


	/**
	 * @name	ftentry_idle_timeout
	 * @brief  	notifies owner about a timeout event for this cftentry instance
	 *
	 * This method is called, when a timeout
	 *
	 */
	virtual void
	ftentry_idle_timeout(cftentry *entry, uint16_t timeout);


	/**
	 * @name	ftentry_idle_timeout
	 * @brief  	notifies owner about a timeout event for this cftentry instance
	 *
	 * This method is called, when a timeout
	 *
	 */
	virtual void
	ftentry_hard_timeout(cftentry *entry, uint16_t timeout);


protected: // overloaded from ciosrv


	/**
	 *
	 */
	void
	handle_timeout(
			int opaque);


protected:

	//
	// specific management methods, adding, deleting,
	// modifying flow table entries
	//

	/**
	 * add an existing flow table entry
	 * @return int result code of operation
	 */
	cftentry*
	add_ft_entry(
			cfttable_owner *owner,
			cofpacket_flow_mod *pack) throw(eFlowTableEntryOverlaps);

	/**
	 * modify an existing flow table entry
	 * @return int result code of operation
	 */
	cftentry*
	modify_ft_entry(
			cfttable_owner *owner,
			cofpacket_flow_mod *pack,
			bool strict = false);

	/**
	 * remove an existing flow table entry
	 * @return int result code of operation
	 */
	void
	rem_ft_entry(
			cfttable_owner *owner,
			cofpacket_flow_mod *pack,
			bool strict = false);


#if 0
	/**
	 * find a cftentry based on a struct ofp_match
	 * @return cftentry pointer to matching cftentry
	 */
	cftentry*
	find_ft_entry(ofp_match *match) throw();
#endif


	/**
	 *
	 */
	void
	update_group_ref_counts(
			cftentry *fte,
			bool inc = true);


};












/**
 *
 *
 *
 *
 */
class cftsearch :
	public csyslog
{

public: // data structures

	std::set<cftentry*> matching_entries;

public: // methods

	/** constructor
	 *
	 */
	cftsearch(
			cpacket* pack);

	/** destructor
	 *
	 */
	virtual
	~cftsearch();


	/** call-operator
	 */
	void operator() (
			cftentry* ftentry);


	/** dump classification
	 */
	const char*
	c_str();


private: // data structures

	cpacket* pack;

	uint32_t __exact_hits;
	uint32_t __wildcard_hits;
	uint32_t __max_hits;
	uint32_t __priority;

	std::string info;
};

}; // end of namespace

#endif
