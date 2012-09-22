/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFSPTABLE_H
#define CFSPTABLE_H

#include <list>
#include <string>
#include <algorithm>

#include "../../cerror.h"
#include "../../ciosrv.h"
#include "../../cvastring.h"
#include "../../cpacket.h"
#include "cfspentry.h"

class eFspTable : public cerror {}; 			// base class cnstable errors
class eFspNoMatch : public eFspTable {};		// no matching entry found
class eFspEntryOverlap : public eFspTable {}; 	// new namespace entry overlaps
class eFspEntryNotFound : public eFspTable {};  // namespace entry not found
class eFspNotAllowed : public eFspTable {}; 	// ofmatch not inserted in registered namespace



class cfsptable : public ciosrv {
public: // static




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

	/** check whether a flow-mod is acceptable from
	 *
	 */
	void
	flow_mod_allowed(
			cfspentry_owner* fspowner,
			cofmatch const& m) throw (eFspNotAllowed);

	/** dump namespace table
	 *
	 */
	const char*
	c_str();



protected: // data structures

	// table of namespace entries
	std::set<cfspentry*> fsp_table;
	// info string
	std::string info;

};


class cfspsearch : public csyslog {
public:
	/** constructor
	 */
	cfspsearch(cofmatch const& m, bool strict) :
		ofmatch(m),
		strict(strict)
	{
#ifndef NDEBUG
		make_info();
#endif
		WRITELOG(UNKNOWN, ROFL_DBG, "cfspsearch(%p)::cfspsearch %s", this, c_str());
	};

	~cfspsearch()
	{
		WRITELOG(UNKNOWN, ROFL_DBG, "cfspsearch(%p)::~cfspsearch %s", this, c_str());
	};

	/** call-operator
	 */
	void
	operator() (cfspentry *nse)
	{
		if (this->ofmatch.overlaps(nse->ofmatch, strict))
		{
			fsp_list.insert(nse);
		}
	};

	/** dump classification
	 */
	const char*
	c_str()
	{
		make_info();
		return c_info;
	};

	/** make info string
	 */
	void
	make_info()
	{
		bzero(c_info, sizeof(c_info));
		snprintf(c_info, sizeof(c_info)-1, "cfspsearch(%p) #entries %d strict=%d %s",
				this, (int)fsp_list.size(), strict, ofmatch.c_str());
	};

	// set of overlapping cofmatch instances
	std::set<cfspentry*> fsp_list;
	// cofmatch instance from nsp request
	cofmatch ofmatch;
	// strict or non-strict search
	bool strict;

private:

	char c_info[512];             // info string (verbose)
};


#endif
