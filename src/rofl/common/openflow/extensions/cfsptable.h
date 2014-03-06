/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFSPTABLE_H
#define CFSPTABLE_H

#include <list>
#include <string>
#include <iostream>
#include <algorithm>

#include "../../croflexception.h"
#include "../../ciosrv.h"
#include "../../cvastring.h"
#include "../../cpacket.h"
#include "cfspentry.h"

namespace rofl
{

class eFspTable 			: public RoflException {}; 	// base class cnstable errors
class eFspNoMatch 			: public eFspTable {};	// no matching entry found
class eFspEntryOverlap 		: public eFspTable {}; 	// new namespace entry overlaps
class eFspEntryNotFound 	: public eFspTable {};  // namespace entry not found
class eFspNotAllowed 		: public eFspTable {}; 	// ofmatch not inserted in registered namespace



class cfsptable : public ciosrv {
public: // per instance

	/** constructor
	 *
	 */
	cfsptable();

	/** destructor
	 *
	 */
	virtual
	~cfsptable();

	/** insert new namespace entry
	 *
	 */
	void
	insert_fsp_entry(
			cfspentry_owner *fspowner,
			cofmatch const& m,
			bool strict = false/*non-strict*/) throw (eFspEntryOverlap);

	/** delete namespace entry
	 *
	 */
	void
	delete_fsp_entry(
			cfspentry_owner *fspowner,
			cofmatch const& m,
			bool strict = true /*strict*/) throw (eFspEntryNotFound);


	/** drop all namespace entries for cofctrl
	 *
	 */
	void
	delete_fsp_entries(
			cfspentry_owner* fspowner);


	/** find matching entries
	 * returns an ordered list of cnsentry objects, first object was best match
	 */
	std::set<cfspentry*>
	find_matching_entries(
			uint32_t in_port,
			uint32_t total_len,
			cpacket& pack) throw (eFspNoMatch, eFrameInvalidSyntax);


	/** find matching entries
	 * returns an ordered list of cnsentry objects, first object was best match
	 */
	std::set<cfspentry*>
	find_matching_entries(
			cofmatch& match) throw (eFspNoMatch, eFrameInvalidSyntax);


	/**
	 *
	 */
	cfspentry*
	find_best_entry(
			uint32_t in_port,
			uint32_t total_len,
			cpacket& pack) throw (eFspNoMatch, eFrameInvalidSyntax);

	/** check whether a flow-mod is acceptable from
	 *
	 */
	void
	flow_mod_allowed(
			cfspentry_owner* fspowner,
			cofmatch const& m) throw (eFspNotAllowed);


protected: // data structures

	// table of namespace entries
	std::set<cfspentry*> fsp_table;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cfsptable const& fsptable) {
		for (std::set<cfspentry*>::const_iterator
				it = fsptable.fsp_table.begin(); it != fsptable.fsp_table.end(); ++it) {
			os << "  " << *(*it) << std::endl;
		}
		return os;
	};
};


class cfspsearch {
public:
	/** constructor
	 */
	cfspsearch(cofmatch const& m, bool strict) :
		ofmatch(m),
		strict(strict) {};

	~cfspsearch() {};

	/** call-operator
	 */
	void
	operator() (cfspentry *nse)
	{
		if (this->ofmatch.contains(nse->ofmatch, strict))
		{
			fsp_list.insert(nse);
		}
	};

	std::set<cfspentry*> fsp_list;		// set of overlapping cofmatch instances
	cofmatch ofmatch;					// cofmatch instance from nsp request
	bool strict;						// strict or non-strict search
};

}; // end of namespace

#endif
