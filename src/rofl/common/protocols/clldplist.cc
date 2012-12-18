/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "clldplist.h"

using namespace rofl;

clldplist::clldplist()
{

}


clldplist::~clldplist()
{

}


std::vector<clldptlv>&
clldplist::unpack(
		uint8_t* tlvs,
		int tlvlen)
throw (eLLDPBadLen)
{
	reset(); // clears elems vector

	// sanity check: bclen must be of size at least of ofp_bucket
	if (tlvlen < (int)sizeof(struct clldptlv::lldp_tlv_hdr_t))
		return elems;

	// first instruction
	struct clldptlv::lldp_tlv_hdr_t *tlvhdr = (struct clldptlv::lldp_tlv_hdr_t*)tlvs;


	while (tlvlen > 0)
	{
		WRITELOG(CLLDPTLV, DBG, "clldplist(%p)::unpack() tlvhdr:%p tlvlen:%d", this, tlvhdr, tlvlen);

		if (tlvlen < (int)sizeof(uint16_t))
		{
			WRITELOG(CLLDPTLV, DBG, "clldplist(%p)::unpack() [1]", this);
			throw eLLDPBadLen();
		}

		uint16_t len = (be16toh(*((uint16_t*)tlvhdr)) & 0x01ff) + sizeof(clldptlv::lldp_tlv_hdr_t);

		//fprintf(stderr, "clldplist(%p)::unpack() tlvlen=%u len=%d\n", this, tlvlen, len);

		WRITELOG(CLLDPTLV, DBG, "clldplist(%p)::unpack() tlvhdr:%p tlvlen:%d len:%d", this, tlvhdr, tlvlen, len);

		if (0 == len)
		{
			WRITELOG(CLLDPTLV, DBG, "clldplist(%p)::unpack() [2]", this);
			throw eLLDPBadLen();
		}

		next() = clldptlv((uint8_t*)tlvhdr, len );

		tlvlen -= len;
		tlvhdr = (struct clldptlv::lldp_tlv_hdr_t*)(((uint8_t*)tlvhdr) + len);

		if (back().get_hdr_type() == clldptlv::LLDPTT_END)
		{
			return elems;
		}
	}

	return elems;
}


uint8_t*
clldplist::pack(
	uint8_t* tlvs,
	size_t tlvlen)
throw (eLLDPlistInval)
{
	size_t needed_inlen = length();

	if (tlvlen < needed_inlen)
		throw eLLDPlistInval();

	struct clldptlv::lldp_tlv_hdr_t *tlvhdr = (struct clldptlv::lldp_tlv_hdr_t*)tlvs; // first tlv header

	coflist<clldptlv>::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		clldptlv& tlv = (*it);

		tlvhdr = (struct clldptlv::lldp_tlv_hdr_t*)
				((uint8_t*)(tlv.pack(tlvhdr, tlv.length())) + tlv.length());
	}

	return tlvs;
}


size_t
clldplist::length() const
{
	size_t tlvlen = 0;
	coflist<clldptlv>::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		tlvlen += (*it).length();
	}
	return tlvlen;
}


const char*
clldplist::c_str()
{
	cvastring vas(1024);
	info.assign(vas("clldplist(%p) %d lldp tlv(s): ", this, elems.size()));
	coflist<clldptlv>::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		switch ((*it).get_hdr_type()) {
		case clldptlv::LLDPTT_END:
		{
			clldptlv_end tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case clldptlv::LLDPTT_CHASSIS_ID:
		{
			clldptlv_chassis_id tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case clldptlv::LLDPTT_PORT_ID:
		{
			clldptlv_port_id tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case clldptlv::LLDPTT_TTL:
		{
			clldptlv_ttl tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case clldptlv::LLDPTT_PORT_DESC:
		{
			clldptlv_port_desc tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case clldptlv::LLDPTT_SYSTEM_NAME:
		{
			clldptlv_sys_name tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case clldptlv::LLDPTT_SYSTEM_DESC:
		{
			clldptlv_sys_desc tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case clldptlv::LLDPTT_SYSTEM_CAPS:
		{
			clldptlv_sys_caps tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}
		case clldptlv::LLDPTT_MGMT_ADDR:
		{
			clldptlv_mgmt_addr tlv((*it));
			info.append(vas("\n  %s", tlv.c_str()));
			break;
		}

		}

		cmemory lmem((*it).length());
		(*it).pack((struct clldptlv::lldp_tlv_hdr_t*)lmem.somem(), lmem.memlen());
		info.append(vas(" %s", lmem.c_str()));
	}

	return info.c_str();
}


clldptlv&
clldplist::find_lldp_tlv(int type)
throw (eLLDPlistNotFound)
{
	coflist<clldptlv>::iterator it;
	if ((it = find_if(elems.begin(), elems.end(),
			clldptlv_find_by_type(type))) == elems.end())
	{
		throw eLLDPlistNotFound();
	}
	return (*it);
}



/* static */
void
clldplist::test()
{
	clldplist l1;

	l1[0] = clldptlv_chassis_id(clldptlv::LLDPCHIDST_PORT, std::string("eth0"));
	l1[1] = clldptlv_sys_caps(clldptlv::LLDP_SYSCAPS_BRIDGE |
								clldptlv::LLDP_SYSCAPS_ROUTER,
								clldptlv::LLDP_SYSCAPS_BRIDGE);
	l1.next() = clldptlv_port_desc(std::string("port eth0"));
	//l1.next() = clldptlv_sys_name(std::string("this is a longer system name for testing the lldp code"));
	l1.next() = clldptlv_end();

	fprintf(stderr, "l1: %s\n", l1.c_str());

	cmemory lmem(l1.length());
	l1.pack(lmem.somem(), lmem.memlen());

	fprintf(stderr, "lmem: %s\n", lmem.c_str());

	clldplist l2;

	l2.unpack(lmem.somem(), lmem.memlen());

	fprintf(stderr, "l2: %s\n", l2.c_str());
}

