/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFMATCH_H
#define COFMATCH_H 1

#include <string>
#include <endian.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>
#ifndef be32toh
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/cmacaddr.h"
#include "rofl/common/cvastring.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

#include "rofl/common/openflow/coxmlist.h"

#include "rofl/common/openflow/experimental/matches/gtp_matches.h"
#include "rofl/common/openflow/experimental/matches/pppoe_matches.h"

namespace rofl {
namespace openflow {

class eOFmatchBase 			: public RoflException {};
class eOFmatchType 			: public eOFmatchBase {};
class eOFmatchInval 		: public eOFmatchBase {};
class eOFmatchNotFound 		: public eOFmatchBase {};
class eOFmatchInvalBadValue	: public eOFmatchInval {};


class cofmatch
{
	uint8_t 				of_version;
	coxmlist 				oxmtlvs;
	uint16_t				type;

public:

	/**
	 *
	 */
	cofmatch(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t type = rofl::openflow::OFPMT_OXM);

	/**
	 *
	 */
	cofmatch(
			const cofmatch &match);

	/**
	 *
	 */
	virtual 
	~cofmatch();

	/**
	 *
	 */
	cofmatch&
	operator= (
			const cofmatch& match);

	/**
	 *
	 */
	bool
	operator== (const cofmatch& m);



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
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	/**
	 *
	 */
	void
	check_prerequisites() const;

	/**
	 *
	 */
	void
	clear() { oxmtlvs.clear(); };

	/**
	 *
	 */
	bool
	contains(
			cofmatch const& match,
			bool strict = false)
	{
		return oxmtlvs.contains(match.get_oxmtlvs(), strict);
	};

	/**
	 *
	 */
	bool
	is_part_of(
			cofmatch const& match,
			uint16_t& exact_hits,
			uint16_t& wildcard_hits,
			uint16_t& missed)
	{
		return oxmtlvs.is_part_of(match.get_oxmtlvs(), exact_hits, wildcard_hits, missed);
	};

public:

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->of_version = ofp_version; };

	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };

	/**
	 *
	 */
	void
	set_type(uint16_t type) { this->type = type; };

	/**
	 *
	 */
	uint16_t
	get_type() const { return type; };

	/**
	 *
	 */
	coxmlist&
	set_oxmtlvs() { return oxmtlvs; };

	/**
	 *
	 */
	coxmlist const&
	get_oxmtlvs() const { return oxmtlvs; };

private:


	/**
	 *
	 */
	size_t
	length_with_padding();

	/**
	 *
	 */
	void
	pack_of10(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack_of10(
			uint8_t* buf, size_t buflen);


	/**
	 *
	 */
	void
	pack_of13(
			uint8_t* buf, size_t buflen);


	/**
	 *
	 */
	void
	unpack_of13(
			uint8_t* m,
			size_t mlen);


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmatch const& m) {
		os << rofl::indent(0) << "<cofmatch ofp-version:" << m.get_version() << " >" << std::endl;
			switch (m.of_version) {
			case rofl::openflow12::OFP_VERSION:
			case rofl::openflow13::OFP_VERSION: {
				os << rofl::indent(2) << "<type: " << m.type << " >" << std::endl;
			} break;
			}
		rofl::indent i(2);
		os << m.oxmtlvs;
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
