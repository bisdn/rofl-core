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

#include <string>
#include <deque>

#include <algorithm>

#include "../cmemory.h"
#include "../cerror.h"
#include "../cvastring.h"
#include "../../platform/unix/csyslog.h"

#include "coxmatch.h"

class eOxmListBase : public cerror {};
class eOxmListInval : public eOxmListBase {}; // invalid parameter
class eOxmListBadLen : public eOxmListBase {}; // bad length
class eOxmListNotFound : public eOxmListBase {}; // element not found
class eOxmListOutOfRange : public eOxmListBase {}; // out of range


/** this class contains a list of Openflow eXtensible Matches (OXM)
 * it does not contain a full struct ofp_match, see class cofmatch for this
 *
 */
class coxmlist :
	public csyslog
{
private: // data structures

		std::string 				info;
		//std::vector<coxmatch*> 	oxmvec;
		cmemory 					area;
		coxmatch** 					oxmvec;

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
		coxmlist& operator= (
				coxmlist const& oxmlist);


		/**
		 *
		 */
		void
		clear();


		/** returns number of coxmatch instances in oxmvec (excluding 0-pointers)
		 *
		 */
		size_t
		size() const;


		/**
		 *
		 */
		bool
		operator== (
				coxmlist& oxmlist);


		/**
		 *
		 */
		coxmatch&
		operator[] (
				size_t index) throw (eOxmListOutOfRange);


		/** stores cofinst instances in this->invec from a packed array struct ofp_instruction (e.g. in struct ofp_flow_mod)
		 *
		 */
		void
		unpack(
				struct ofp_oxm_hdr *oxm_hdr,
				size_t oxm_len)
					throw (eOxmListBadLen, eOxmListInval);


		/** builds an array of struct ofp_instruction from this->oxmvec
		 *
		 */
		struct ofp_oxm_hdr*
		pack(
				struct ofp_oxm_hdr *oxm_hdr,
				size_t oxm_len)
					throw (eOxmListInval);


		/** returns required length for array of struct ofp_instruction
		 * for all instructions defined in this->invec including padding!
		 */
		size_t
		length();


		/** dump info string
		 */
		const char*
		c_str();


		/** check for existence of specific OXM TLV
		 *
		 */
		bool
		exists(
				uint16_t oxm_class,
				uint8_t oxm_field);


		/** find a specific OXM TLV and return a reference
		 */
		coxmatch&
		oxm_find(
				uint16_t oxm_class,
				uint8_t oxm_field)
					throw (eOxmListNotFound);


		/** find a specific OXM TLV and return a copy
		 *
		 */
		coxmatch
		oxm_copy(
				uint16_t oxm_class,
				uint8_t oxm_field)
					throw (eOxmListNotFound);


		/** erase oxmlist
		 *
		 */
		void
		erase(
				uint16_t oxm_class,
				uint8_t oxm_field);


		/** insert new or replace existing oxm of [class,field] with value
		 *
		 */
		void
		oxm_replace_or_insert(
				uint16_t oxm_class,
				uint8_t oxm_field,
				uint32_t dword);

		/** insert new or replace existing oxm of [class,field] with value
		 *
		 */
		void
		oxm_replace_or_insert(
				uint16_t oxm_class,
				uint8_t oxm_field,
				uint16_t word);

		/** insert new or replace existing oxm of [class,field] with value
		 *
		 */
		void
		oxm_replace_or_insert(
				uint16_t oxm_class,
				uint8_t oxm_field,
				uint8_t byte);


		/**
		 *
		 */
		bool
		overlap(
				coxmlist const& oxm,
				bool strict = false);


		/**
		 *
		 */
		void
		calc_hits(
				coxmlist& oxmlist,
				uint16_t& exact_hits,
				uint16_t& wildcard_hits,
				uint16_t& missed);

		/**
		 *
		 */
		static void
		test();
};

#endif /* COXMLIST_H_ */
