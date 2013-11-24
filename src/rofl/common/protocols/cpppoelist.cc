/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cpppoelist.h"

using namespace rofl;

cpppoelist::cpppoelist()
{

}


cpppoelist::~cpppoelist()
{

}


std::vector<cpppoetlv>&
cpppoelist::unpack(
		uint8_t* tlvs,
		int tlvlen)
throw (ePPPoEBadLen)
{
	clear(); // clears elems vector

	// sanity check: bclen must be of size at least of ofp_bucket
	if (tlvlen < (int)sizeof(struct cpppoetlv::pppoe_tag_hdr_t))
		return elems;

	// first instruction
	struct cpppoetlv::pppoe_tag_hdr_t *tlvhdr = (struct cpppoetlv::pppoe_tag_hdr_t*)tlvs;


	while (tlvlen > 0)
	{
		WRITELOG(CPPPOETLV, DBG, "cpppoelist(%p)::unpack() tlvhdr:%p tlvlen:%d", this, tlvhdr, tlvlen);

		if (tlvlen < (int)sizeof(struct cpppoetlv::pppoe_tag_hdr_t))
		{
			WRITELOG(CPPPOETLV, DBG, "cpppoelist(%p)::unpack() [1]", this);
			throw ePPPoEBadLen();
		}

		uint16_t len = (be16toh(tlvhdr->length)) + sizeof(cpppoetlv::pppoe_tag_hdr_t);

		//fprintf(stderr, "cpppoelist(%p)::unpack() tlvlen=%u len=%d\n", this, tlvlen, len);

		WRITELOG(CPPPOETLV, DBG, "cpppoelist(%p)::unpack() tlvhdr:%p tlvlen:%d len:%d", this, tlvhdr, tlvlen, len);

		if (0 == len)
		{
			WRITELOG(CPPPOETLV, DBG, "cpppoelist(%p)::unpack() [2]", this);
			throw ePPPoEBadLen();
		}

		if (tlvlen < len)
		{
			WRITELOG(CPPPOETLV, DBG, "cpppoelist(%p)::unpack() [3]", this);
			throw ePPPoEBadLen();
		}

		next() = cpppoetlv((uint8_t*)tlvhdr, len );

		//fprintf(stderr, "UUU => %s\n", c_str());

		tlvlen -= len;
		tlvhdr = (struct cpppoetlv::pppoe_tag_hdr_t*)(((uint8_t*)tlvhdr) + len);

		if (back().get_hdr_type() == cpppoetlv::PPPOE_TAG_END_OF_LIST)
		{
			return elems;
		}
	}

	return elems;
}


uint8_t*
cpppoelist::pack(
	uint8_t* tlvs,
	size_t tlvlen)
throw (ePPPoElistInval)
{
	size_t needed_inlen = length();

	if (tlvlen < needed_inlen)
		throw ePPPoElistInval();

	struct cpppoetlv::pppoe_tag_hdr_t *tlvhdr = (struct cpppoetlv::pppoe_tag_hdr_t*)tlvs; // first tlv header

	cpppoelist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		cpppoetlv& tlv = (*it);

		tlvhdr = (struct cpppoetlv::pppoe_tag_hdr_t*)
				((uint8_t*)(tlv.pack(tlvhdr, tlv.length())) + tlv.length());

		cmemory mem(tlvs, tlvlen);
		//fprintf(stderr, "XXX => mem: %s\n", mem.c_str());
	}

	return tlvs;
}


size_t
cpppoelist::length()
{
	size_t tlvlen = 0;
	cpppoelist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		tlvlen += (*it).length();
	}
	return tlvlen;
}



cpppoetlv&
cpppoelist::find_pppoe_tlv(int type)
throw (ePPPoElistNotFound)
{
	cpppoelist::iterator it;
	if ((it = find_if(elems.begin(), elems.end(),
			cpppoetlv_find_by_type(type))) == elems.end())
	{
		throw ePPPoElistNotFound();
	}
	return (*it);
}



/* static */
void
cpppoelist::test()
{
	cpppoelist l1;

	l1[0] = cpppoetlv_service_name(std::string("service name"));
	l1[1] = cpppoetlv_ac_name(std::string("ac name"));
	l1.next() = cpppoetlv_end();

	std::cerr << "l1: " << l1 << std::endl;

	cmemory lmem(l1.length());
	l1.pack(lmem.somem(), lmem.memlen());

	std::cerr << "lmem: " << lmem << std::endl;

	cpppoelist l2;

	l2.unpack(lmem.somem(), lmem.memlen());

	std::cerr << "l2: " << l2 << std::endl;
}
