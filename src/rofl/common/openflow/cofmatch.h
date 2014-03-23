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

class eOFmatchBase 			: public RoflException {}; // error base class cofmatch
class eOFmatchType 			: public eOFmatchBase {};
class eOFmatchInval 		: public eOFmatchBase {};
class eOFmatchNotFound 		: public eOFmatchBase {};
class eOFmatchInvalBadValue	: public eOFmatchInval {};




class cofmatch
{
private: // data structures

	uint8_t 		of_version;		// OpenFlow version used for this cofmatch instance
	coxmlist 		oxmtlvs;		// list of all oxms
	cmemory 		memarea;

#define OFP10_MATCH_STATIC_LEN		(sizeof(struct openflow10::ofp_match))
#define OFP12_MATCH_STATIC_LEN  	(2*sizeof(uint16_t))
#define OFP13_MATCH_STATIC_LEN  	(2*sizeof(uint16_t))

public: // data structures

	union {
		uint8_t*							ofpu_match;
		struct openflow10::ofp_match*		ofpu10_match;
		struct openflow12::ofp_match*		ofpu12_match;
		struct openflow13::ofp_match*		ofpu13_match;
	} ofpu;

#define ofh_match  	ofpu.ofpu_match
#define ofh10_match ofpu.ofpu10_match
#define ofh12_match ofpu.ofpu12_match
#define ofh13_match ofpu.ofpu13_match



public: // methods

	/** constructor
	 *
	 */
	cofmatch(
			uint8_t of_version = openflow12::OFP_VERSION,
			uint16_t type = openflow::OFPMT_OXM);


	/** constructor
	 *
	 */
	template<class T>
	cofmatch(
			uint8_t of_version,
			T* match,
			size_t matchlen);


	/** copy constructor
	 *
	 */
	cofmatch(const cofmatch &m) 
	{
		*this = m;
	};


	/** destructor
	 *
	 */
	virtual 
	~cofmatch();


	/** assignment operator
	 */
	cofmatch&
	operator= (const cofmatch& m);

	/**
	 *
	 */
	bool
	operator== (const cofmatch& m);


#if 0
	/** less operator
	 *
	 */
	bool
	operator< (cofmatch const& m) const;
#endif

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) {
		this->of_version = ofp_version;

		switch (of_version) {
		case openflow::OFP_VERSION_UNKNOWN: {
			memarea.resize(openflow13::OFP_MATCH_STATIC_LEN);
			ofh_match = memarea.somem();
		} break;
		case openflow10::OFP_VERSION: {
			memarea.resize(openflow10::OFP_MATCH_STATIC_LEN);
			ofh10_match = (struct openflow10::ofp_match*)memarea.somem();
		} break;
		case openflow12::OFP_VERSION: {
			memarea.resize(openflow12::OFP_MATCH_STATIC_LEN);
			ofh12_match = (struct openflow12::ofp_match*)memarea.somem();
			ofh12_match->type 	= htobe16(rofl::openflow::OFPMT_OXM);
			ofh12_match->length = htobe16(length());
		} break;
		case openflow13::OFP_VERSION: {
			memarea.resize(openflow13::OFP_MATCH_STATIC_LEN);
			ofh13_match = (struct openflow13::ofp_match*)memarea.somem();
			ofh13_match->type 	= htobe16(rofl::openflow::OFPMT_OXM);
			ofh13_match->length = htobe16(length());
		} break;
		default: {
		};
		}
	};

	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };

	/** validate match structure
	 *
	 */
	void
	validate() throw (eOFmatchInval);


	/** return required length for packed cofmatch (includes padding to 64bit)
	 */
	size_t
	length() const;


private:


	size_t
	length_internal();


public:

	/*
	 * TODO: introduce a template
	 */

	uint8_t*
	pack(uint8_t* m, size_t mlen);

	void
	unpack(uint8_t* m, size_t mlen);

	/** copy internal struct ofp_match into specified ofp_match ptr 'm'
	 * @return pointer 'm'
	 *
	 */
	uint8_t*
	pack_of10(
			uint8_t* m,
			size_t mlen);


	/** copy ofp_match structure pointed to by 'm' into internal struct ofp_match
	 *
	 */
	void
	unpack_of10(
			uint8_t* m,
			size_t mlen);


	/** copy internal struct ofp_match into specified ofp_match ptr 'm'
	 * @return pointer 'm'
	 *
	 */
	uint8_t*
	pack_of12(
			uint8_t* m,
			size_t mlen);


	/** copy ofp_match structure pointed to by 'm' into internal struct ofp_match
	 *
	 */
	void
	unpack_of12(
			uint8_t* m,
			size_t mlen);


	/** copy internal struct ofp_match into specified ofp_match ptr 'm'
	 * @return pointer 'm'
	 *
	 */
	uint8_t*
	pack_of13(
			uint8_t* m,
			size_t mlen);


	/** copy ofp_match structure pointed to by 'm' into internal struct ofp_match
	 *
	 */
	void
	unpack_of13(
			uint8_t* m,
			size_t mlen);



	/**
	 * @brief	checks prerequisites for OF1.2 and beyond OXM TLV lists
	 */
	void
	check_prerequisites() const;


	
	/** check for an identical match between two ofp_match structures
	 */
	bool 
	operator== (
		cofmatch& m);


	/** reset structure
	 *
	 */
	void
	clear();


public:

	/**
	 *
	 */
	void
	set_type(uint16_t type);

	/**
	 *
	 */
	uint16_t
	get_type() const;

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

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmatch const& m) {
		os << indent(0) << "<cofmatch ";
			switch (m.of_version) {
			case openflow10::OFP_VERSION: {
				os << "OF1.0 ";
			} break;
			case openflow12::OFP_VERSION: {
				os << "OF1.2 type: " << be16toh(m.ofh12_match->type) << " ";
				os << "length: " << be16toh(m.ofh12_match->length) << " ";
			} break;
			case openflow13::OFP_VERSION: {
				os << "OF1.3 type: " << be16toh(m.ofh13_match->type) << " ";
				os << "length: " << be16toh(m.ofh13_match->length) << " ";
			} break;
			default: {
				os << "OF version " << m.of_version << " not supported";
			}
			}
		os << ">" << std::endl;
		indent i(2);
		os << m.oxmtlvs;
		return os;
	};
};



}; // end of namespace openflow
}; // end of namespace rofl

#endif
