/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cpppoelist.h"


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
	reset(); // clears elems vector

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


const char*
cpppoelist::c_str()
{
	cvastring vas(1024);
	info.assign(vas("cpppoelist(%p) %d lldp tlv(s): ", this, elems.size()));
	cpppoelist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		switch ((*it).get_hdr_type()) {
		case cpppoetlv::PPPOE_TAG_END_OF_LIST:
		{
			cpppoetlv_end tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case cpppoetlv::PPPOE_TAG_SERVICE_NAME:
		{
			cpppoetlv_service_name tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case cpppoetlv::PPPOE_TAG_AC_NAME:
		{
			cpppoetlv_ac_name tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case cpppoetlv::PPPOE_TAG_HOST_UNIQ:
		{
			cpppoetlv_host_uniq tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case cpppoetlv::PPPOE_TAG_AC_COOKIE:
		{
			cpppoetlv_ac_cookie tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case cpppoetlv::PPPOE_TAG_VENDOR_SPECIFIC:
		{
			cpppoetlv tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case cpppoetlv::PPPOE_TAG_RELAY_SESSION_ID:
		{
			cpppoetlv_relay_session_id tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case cpppoetlv::PPPOE_TAG_SERVICE_NAME_ERROR:
		{
			cpppoetlv_service_name_error tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case cpppoetlv::PPPOE_TAG_AC_SYSTEM_ERROR:
		{
			cpppoetlv_ac_system_error tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case cpppoetlv::PPPOE_TAG_GENERIC_ERROR:
		{
			cpppoetlv_generic_error tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}

		}

		cmemory lmem((*it).length());
		(*it).pack((struct cpppoetlv::pppoe_tag_hdr_t*)lmem.somem(), lmem.memlen());
		info.append(vas(" %s", lmem.c_str()));
	}

	return info.c_str();
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

	fprintf(stderr, "l1: %s\n", l1.c_str());

	cmemory lmem(l1.length());
	l1.pack(lmem.somem(), lmem.memlen());

	fprintf(stderr, "lmem: %s\n", lmem.c_str());

	cpppoelist l2;

	l2.unpack(lmem.somem(), lmem.memlen());

	fprintf(stderr, "l2: %s\n", l2.c_str());
}
