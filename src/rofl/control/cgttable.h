/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CGTTABLE_H
#define CGTTABLE_H 1

#include <map>
#include <string>

#include "../common/ciosrv.h"
#include "../common/cerror.h"
//#include "../cfwdelem.h" 

#include "cgtentry.h"
#include "../common/openflow/cofpacket.h"
#include "cftentry.h"

namespace rofl
{

  /* forward declaration */
  class cfwdelem;
  class cofctl;
  class cgtentry;
  class cgtentry_owner;
  class eGroupEntryInval;

/* error classes */
class eGroupTableBase           : public cerror {};
class eGroupTableInval          : public eGroupTableBase {}; // invalid flow-mod entry received
class eGroupTableExists         : public eGroupTableBase {};
class eGroupTableNoMatch        : public eGroupTableBase {}; // no matching entry found
class eGroupTableEntryOverlaps  : public eGroupTableBase {}; // entry overlaps with existing entry in flow-table
class eGroupTableClassificationFailed : public eGroupTableBase {}; // invalid packet frame for classification
class eGroupTableNotFound       : public eGroupTableBase {}; // element not found
class eGroupTableLoopDetected   : public eGroupTableBase {}; // loop found in group table
class eGroupTableModNonExisting : public eGroupTableBase {}; // enttry for modification not found
class eGroupTableGroupModInval  : public eGroupTableBase {};
class eGroupTableGroupModInvalID  : public eGroupTableBase {};
class eGroupTableGroupModBadOutPort : public eGroupTableBase {};


class cgttable :
	public ciosrv
{
public: // data structures

	//cfwdelem 							*fwdelem;		// parent cfwdelem instance
	std::map<uint32_t, cgtentry*> 		grp_table;		// flow_table: set of cftentries
	uint64_t 							lookup_count;	// lookup counter
	uint64_t 							matched_count;	// matched counter

public:

	/** constructor
	 */
//	cgttable(
//			cfwdelem *_fwdelem = (cfwdelem*)0);
	cgttable();


	/** destructor
	 */
	virtual
	~cgttable();


	/** copy constructor
	 */
	cgttable(
			const cgttable& gt);


	/** reset group table, i.e. clear all group entries
	 */
	void
	reset();


	/** assignment operator
	 */
	cgttable&
	operator= (const cgttable& gt);


	/** return group entry for group_id
	 * cgrptable grp_table;
	 * try {
	 *  ...
	 * 	grp_table[1234]->...
	 * 	...
	 * } catch (eGroupTableNotFound& e) { ... }
	 */
	cgtentry*
	operator[] (
			const uint32_t& grp_id) throw(eGroupTableNotFound);


	/** common entry method for add/update/delete a cgtentry
	 */
	cgtentry*
	update_gt_entry(
			cgtentry_owner *owner,
			struct ofp_group_mod *grp_mod);


	/** add a cgtentry
	 */
	cgtentry*
	add_gt_entry(
			cgtentry_owner *owner,
			struct ofp_group_mod *grp_mod) throw (eGroupTableExists,
								eGroupTableGroupModInvalID,
								eGroupTableGroupModBadOutPort,
								eGroupTableLoopDetected);


	/** update a cgtentry
	 */
	cgtentry*
	modify_gt_entry(
			cgtentry_owner *owner,
			struct ofp_group_mod *grp_mod) throw (eGroupTableGroupModInvalID,
			                                      eGroupTableModNonExisting,
			                                      eGroupTableLoopDetected);


	/** delete a cgtentry
	 */
	cgtentry*
	rem_gt_entry(
			cgtentry_owner *owner,
			struct ofp_group_mod *grp_mod) throw (eGroupTableGroupModInvalID,
								eGroupTableNotFound);


	/** get group stats for all group_ids
	 *
	 */
	void
	get_group_stats(
			cmemory& body);


	/** get group desc stats for all group_ids
	 *
	 */
	void
	get_group_desc_stats(
			cmemory& body);


	/** get group features stats for all group_ids
	 *
	 */
	void
	get_group_features_stats(
			cmemory& body);




private:

	/**
	 *
	 */
	void
	loop_check(
			cgtentry *gte,
			uint32_t loop_group_id) throw (eGroupTableLoopDetected);

};

}; // end of namespace

#endif

