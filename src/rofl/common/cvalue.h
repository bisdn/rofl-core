/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cvalue.h
 *
 *  Created on: 29.10.2012
 *      Author: andreas
 */

#ifndef CVALUE_H
#define CVALUE_H 1

#include <string>

#ifdef __cplusplus
extern "C" {
#endif
	#include <inttypes.h>
#ifdef __cplusplus
}
#endif



#include <rofl/common/cmemory.h>
#include <rofl/common/cerror.h>

class eValueBase 			: public cerror {};
class eValueInval 			: public eValueBase {};


class cvalue :
	public cmemory
{
private:

		enum cvalue_type_t {
			CVALUE_TYPE_UNDEF = 1,
			CVALUE_TYPE_UINT8, 		//  8 bit unsigned byte
			CVALUE_TYPE_UINT16,		// 16 bit unsigned word
			CVALUE_TYPE_UINT32,		// 32 bit unsigned word
			CVALUE_TYPE_UINT64,		// 64 bit unsigned word
			CVALUE_TYPE_CHAR,		//  8 bit signed character
			CVALUE_TYPE_WORD,		// 16 bit signed word
			CVALUE_TYPE_INT,		// 32 bit signed word
			CVALUE_TYPE_STRING,		// string
			CVALUE_TYPE_OPAQUE,		// arbitrary opaque chunk of memory
		};

		uint8_t			value_type;	// see enum cvalue_type_t above

public:

		/**
		 *
		 */
		cvalue() :
			cmemory(0), value_type(CVALUE_TYPE_UNDEF) {};

		/**
		 *
		 */
		cvalue(uint8_t value) :
			cmemory(sizeof(uint8_t)), value_type(CVALUE_TYPE_UINT8)
		{
			*((uint8_t*)somem()) = value;
		};
		cvalue(uint16_t value) :
			cmemory(sizeof(uint16_t)), value_type(CVALUE_TYPE_UINT16)
		{
			*((uint16_t*)somem()) = value;
		};
		cvalue(uint32_t value) :
			cmemory(sizeof(uint32_t)), value_type(CVALUE_TYPE_UINT32)
		{
			*((uint32_t*)somem()) = value;
		};
		cvalue(uint64_t value) :
			cmemory(sizeof(uint64_t)), value_type(CVALUE_TYPE_UINT64)
		{
			*((uint64_t*)somem()) = value;
		};
		cvalue(std::string s_str) :
			cmemory(s_str.size() + 1), value_type(CVALUE_TYPE_STRING)
		{
			assign((unsigned char*)s_str.c_str(), s_str.length());
		};
		cvalue(uint8_t *body, size_t bodylen) :
			cmemory(bodylen), value_type(CVALUE_TYPE_OPAQUE)
		{
			assign(body, bodylen);
		};

		/**
		 *
		 */
		virtual
		~cvalue() {};

		/**
		 *
		 */
		cvalue(cvalue const& v)
		{
			*this = v;
		};

		/**
		 *
		 */
		cvalue& operator= (cvalue const& v)
		{
			if (this == &v)
				return *this;
			cmemory::operator= (v);
			value_type = v.value_type;
			return *this;
		};

		/**
		 *
		 */
		uint8_t&
		uint8() throw (eValueInval)
		{
			if (CVALUE_TYPE_UINT8 != value_type)
			{
				throw eValueInval();
			}
			return *((uint8_t*)somem());
		};

		uint16_t&
		uint16() throw (eValueInval)
		{
			if (CVALUE_TYPE_UINT16 != value_type)
			{
				throw eValueInval();
			}
			return *((uint16_t*)somem());
		};

		uint32_t&
		uint32() throw (eValueInval)
		{
			if (CVALUE_TYPE_UINT32 != value_type)
			{
				throw eValueInval();
			}
			return *((uint32_t*)somem());
		};

		uint64_t&
		uint64() throw (eValueInval)
		{
			if (CVALUE_TYPE_UINT64 != value_type)
			{
				throw eValueInval();
			}
			return *((uint64_t*)somem());
		};

		std::string
		s_str() throw (eValueInval)
		{
			if (CVALUE_TYPE_STRING != value_type)
			{
				throw eValueInval();
			}
			std::string s_str((const char*)somem(), memlen());
			return s_str;
		};

		cmemory
		memory() throw (eValueInval)
		{
			if (CVALUE_TYPE_OPAQUE != value_type)
			{
				throw eValueInval();
			}
			return cmemory(somem(), memlen());
		};
};


#endif /* CVALUE_H */
