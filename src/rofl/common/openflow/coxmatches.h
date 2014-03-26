/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmlist.h
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#ifndef COXMATCHES_H_
#define COXMATCHES_H_

#include <ostream>
#include <string>
#include <deque>
#include <map>

#include <algorithm>

#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

#include "rofl/common/openflow/coxmatch.h"
#include "rofl/common/openflow/experimental/matches/pppoe_matches.h"
#include "rofl/common/openflow/experimental/matches/gtp_matches.h"

namespace rofl {
namespace openflow {

class eOxmListBase 			: public RoflException {};
class eOxmListInval 		: public eOxmListBase {}; // invalid parameter
class eOxmListBadLen 		: public eOxmListBase {}; // bad length
class eOxmListNotFound 		: public eOxmListBase {}; // element not found
class eOxmListOutOfRange 	: public eOxmListBase {}; // out of range


/** this class contains a list of Openflow eXtensible Matches (OXM)
 * it does not contain a full struct ofp_match, see class cofmatch for this
 *
 */
class coxmatches
{
	std::map<uint32_t, coxmatch>	matches;

public:

	/**
	 *
	 */
	coxmatches();

	/**
	 *
	 */
	virtual
	~coxmatches();

	/**
	 *
	 */
	coxmatches(
			coxmatches const& oxmlist);

	/**
	 *
	 */
	coxmatches&
	operator= (
			coxmatches const& oxmlist);

	/**
	 *
	 */
	bool
	operator== (
			coxmatches const& oxmlist);


public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

public:

	/**
	 *
	 */
	std::map<uint32_t, coxmatch>&
	set_matches() { return matches; }

	/**
	 *
	 */
	std::map<uint32_t, coxmatch> const&
	get_matches() const { return matches; }

	/**
	 *
	 */
	coxmatch&
	add_match(
			coxmatch const& oxm);

	/**
	 *
	 */
	coxmatch&
	add_match(
			uint32_t oxm_id);

	/**
	 *
	 */
	void
	drop_match(
			uint32_t oxm_id);

	/**
	 *
	 */
	coxmatch&
	set_match(
			uint32_t oxm_id);

	/**
	 *
	 */
	coxmatch const&
	get_match(
			uint32_t oxm_id) const;


	/**
	 *
	 */
	bool
	has_match(
			uint32_t oxm_id) const;

public:

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	bool
	contains(
			coxmatches const& oxmlist,
			bool strict = false);

	/**
	 *
	 */
	bool
	is_part_of(
			coxmatches const& oxmlist,
			uint16_t& exact_hits,
			uint16_t& wildcard_hits,
			uint16_t& missed);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coxmatches const& oxl) {
		os << rofl::indent(0) << "<coxmatches #matches:" << oxl.matches.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, coxmatch>::const_iterator
				it = oxl.matches.begin(); it != oxl.matches.end(); ++it) {
			os << coxmatch_output(it->second);
		}
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COXMLIST_H_ */
