/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmlist.h
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#ifndef COXMLIST_H_
#define COXMLIST_H_

#include <ostream>
#include <string>
#include <deque>
#include <map>

#include <algorithm>

#include "../cmemory.h"
#include "../cerror.h"
#include "../cvastring.h"
#include "rofl/platform/unix/csyslog.h"
#include "../openflow/openflow_rofl_exceptions.h"

#include "coxmatch.h"

namespace rofl
{

class eOxmListBase 			: public cerror {};
class eOxmListInval 		: public eOxmListBase {}; // invalid parameter
class eOxmListBadLen 		: public eOxmListBase {}; // bad length
class eOxmListNotFound 		: public eOxmListBase {}; // element not found
class eOxmListOutOfRange 	: public eOxmListBase {}; // out of range


/** this class contains a list of Openflow eXtensible Matches (OXM)
 * it does not contain a full struct ofp_match, see class cofmatch for this
 *
 */
class coxmlist :
	public csyslog
{
private: // data structures

		std::string 										info;
		std::map<uint16_t, std::map<uint8_t, coxmatch*> >	matches;
							// key1: OXM TLV class, key2: OXM TLV field, value: ptr to coxmatch instance on heap


public: // methods


		/** constructor
		 *
		 */
		coxmlist();


		/** destructor
		 *
		 */
		virtual
		~coxmlist();


		/**
		 *
		 */
		coxmlist(
				coxmlist const& oxmlist);


		/**
		 *
		 */
		coxmlist&
		operator= (
				coxmlist const& oxmlist);


		/**
		 *
		 */
		bool
		operator== (
				coxmlist const& oxmlist);


		/**
		 * @brief	Returns number of OXM TLVs stored in this coxmlist instance.
		 */
		unsigned int
		get_n_matches() const;


public:


		/** stores cofinst instances in this->invec from a packed array struct ofp_instruction (e.g. in struct ofp_flow_mod)
		 *
		 */
		void
		unpack(
				uint8_t* buf,
				size_t buflen);


		/** builds an array of struct ofp_instruction from this->oxmvec
		 *
		 */
		void
		pack(
				uint8_t* buf,
				size_t buflen);


		/**
		 * @brief	Returns length of this coxmlist instance when begin packed.
		 */
		size_t
		length() const;



		/** erase oxmlist
		 *
		 */
		void
		erase(
				uint16_t oxm_class,
				uint8_t oxm_field);


		/** insert coxmatch instance
		 *
		 */
		void
		insert(
				coxmatch const& oxm);




		/**
		 *
		 */
		coxmatch&
		get_match(
				uint16_t ofm_class, uint8_t ofm_field);




		/**
		 *
		 */
		coxmatch const&
		get_const_match(
				uint16_t ofm_class, uint8_t ofm_field) const;





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
				coxmlist const& oxmlist,
				bool strict = false);



		/**
		 *
		 */
		bool
		is_part_of(
				coxmlist const& oxmlist,
				uint16_t& exact_hits,
				uint16_t& wildcard_hits,
				uint16_t& missed);


public:


		/**
		 *
		 */
		friend std::ostream&
		operator<< (std::ostream& os, coxmlist const& oxl)
		{
			for (std::map<uint16_t, std::map<uint8_t, coxmatch*> >::const_iterator
					it = oxl.matches.begin(); it != oxl.matches.end(); ++it) {
				for (std::map<uint8_t, coxmatch*>::const_iterator
						jt = it->second.begin(); jt != it->second.end(); ++jt) {
						os << "\t" << *(jt->second) << " " << std::endl;
				}
			}
			return os;
		};
};

}; // end of namespace

#endif /* COXMLIST_H_ */
