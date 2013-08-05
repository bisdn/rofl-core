/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmatch.cc
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#include "coxmatch.h"

using namespace rofl;

coxmatch::coxmatch(size_t size) :
	cmemory(size)
{
	pthread_rwlock_init(&oxmlock, 0);
	oxm_header = (struct ofp_oxm_hdr*)somem();
}


coxmatch::coxmatch(
		struct ofp_oxm_hdr* hdr,
		size_t oxm_len) :
		cmemory((uint8_t*)hdr, oxm_len)
{
	pthread_rwlock_init(&oxmlock, 0);
	oxm_header = (struct ofp_oxm_hdr*)somem();
}


coxmatch::coxmatch(
		struct ofp_action_set_field *ach,
		size_t achlen)
{
	if (achlen < sizeof(uint32_t)) {
		throw eOxmInval();
	}

	pthread_rwlock_init(&oxmlock, 0);
	struct ofp_oxm_hdr *oxm_hdr = (struct ofp_oxm_hdr*)ach->field;
	size_t oxm_len = achlen - sizeof(uint32_t);

	if (oxm_len < sizeof(uint32_t)) {
		throw eOxmInval();
	}

	assign((uint8_t*)oxm_hdr, oxm_len);
	oxm_header = (struct ofp_oxm_hdr*)somem();

	// TODO: check hasmask and remaining length
}


coxmatch::coxmatch(
		const coxmatch& oxm)
{
	pthread_rwlock_init(&oxmlock, 0);
	*this = oxm;
}


coxmatch::~coxmatch()
{
	pthread_rwlock_destroy(&oxmlock);
}


coxmatch&
coxmatch::operator= (
		const coxmatch& oxm)
{
	if (this == &oxm)
		return *this;

	cmemory::operator= (oxm);

	oxm_header = (struct ofp_oxm_hdr*)somem();

	return *this;
}


bool
coxmatch::operator== (
		coxmatch const& oxm)
{
	return cmemory::operator== (oxm);
}



bool
coxmatch::operator!= (
		coxmatch const& oxm)
{
	return cmemory::operator!= (oxm);
}



bool
coxmatch::operator< (
		coxmatch const& oxm)
{
	if (get_oxm_class() != oxm.get_oxm_class())
	{
		return (get_oxm_class() < oxm.get_oxm_class());
	}
	if (get_oxm_field() != oxm.get_oxm_field())
	{
		return (get_oxm_field() < oxm.get_oxm_field());
	}
	size_t len = (memlen() < oxm.memlen()) ? memlen() : oxm.memlen();
	int rc = memcmp(somem(), oxm.somem(), len);

	if (rc < 0)
	{
		return true;
	}
	else if (rc > 0)
	{
		return false;
	}
	return false;
}


void
coxmatch::reset()
{
	clear();
	oxm_header = (struct ofp_oxm_hdr*)somem();
}


struct ofp_oxm_hdr*
coxmatch::sooxm() const
{
	return oxm_header;
}


size_t
coxmatch::length() const
{
	return memlen();
}



void
coxmatch::pack(
		uint8_t* buf,
		size_t buflen)
{
	if (buflen < length()) {
		throw eOxmInval();
	}

	memcpy(buf, somem(), memlen());
}



void
coxmatch::unpack(
		uint8_t* buf,
		size_t buflen)
{
	reset();

	if (buflen > memlen()) {
		oxm_header = (struct ofp_oxm_hdr*)resize(buflen);
	}

	assign(buf, buflen);
	oxm_header = (struct ofp_oxm_hdr*)somem();
}



const char*
coxmatch::c_str()
{
	cvastring vas(320);

	//info.assign(vas("coxmatch(%p): oxm_header: class:0x%x field:0x%x (%s) hasmask:%d length:%d OXM-TLV-len:%d oxm:%s",
#if 0
	info.assign(vas("OXM-TLV (0x%x:0x%02x) [%s] hm:%d len:%d len:%d oxm:%s",
			get_oxm_class(),
			get_oxm_field(),
			type2desc(get_oxm_class(), get_oxm_field()),
			get_oxm_hasmask(),
			get_oxm_length(),
			length(),
			cmemory::c_str()));
#endif

	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
		{
			switch (get_oxm_field()) {
			case OFPXMT_OFB_IPV6_SRC:
			case OFPXMT_OFB_IPV6_DST:
			case OFPXMT_OFB_IPV6_ND_TARGET:
				{
					fframe f_addr(oxm_ipv6addr->addr, 16);
					fframe f_mask((size_t)0);
					if (get_oxm_hasmask()) {
						f_mask = fframe(oxm_ipv6addr->mask, 16);
					}
					info.assign(vas("OXM-TLV [%s:%s] => hm:%d len:%d padded-len:%d addr:%s mask:%s",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							get_oxm_hasmask(),
							get_oxm_length(),
							length(),
							f_addr.c_str(),
							f_mask.c_str()));
				}
				break;

			case OFPXMT_OFB_METADATA:
				{
					info.assign(vas("OXM-TLV [%s:%s] => [%llx] hm:%d len:%d padded-len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							uint64_value(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;

			case OFPXMT_OFB_ETH_DST:
			case OFPXMT_OFB_ETH_SRC:
			case OFPXMT_OFB_ARP_SHA:
			case OFPXMT_OFB_ARP_THA:
			case OFPXMT_OFB_IPV6_ND_SLL:
			case OFPXMT_OFB_IPV6_ND_TLL:
				{
					cmacaddr maddr(oxm_uint48t->value, OFP_ETH_ALEN);

					info.assign(vas("OXM-TLV [%s:%s] => [%s] hm:%d len:%d padded-len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							maddr.c_str(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;

			case OFPXMT_OFB_IN_PORT:
			case OFPXMT_OFB_IN_PHY_PORT:
			case OFPXMT_OFB_MPLS_LABEL:
				{
					info.assign(vas("OXM-TLV [%s:%s] => [0x%x] hm:%d len:%d padded-len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							uint32_value(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;

			case OFPXMT_OFB_IPV4_SRC:
			case OFPXMT_OFB_IPV4_DST:
			case OFPXMT_OFB_ARP_SPA:
			case OFPXMT_OFB_ARP_TPA:
				{
					caddress addr(AF_INET, "0.0.0.0");
					addr.ca_s4addr->sin_addr.s_addr = htobe32(uint32_value());

					info.assign(vas("OXM-TLV [%s:%s] => [%s] hm:%d len:%d padded-len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							addr.addr_c_str(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;

			case OFPXMT_OFB_IPV6_FLABEL:
				{
					info.assign(vas("OXM-TLV [%s:%s] => [%d] hm:%d len:%d padded-len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							uint32_value(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;

			case OFPXMT_OFB_ETH_TYPE:
			case OFPXMT_OFB_VLAN_VID:
			case OFPXMT_OFB_TCP_SRC:
			case OFPXMT_OFB_TCP_DST:
			case OFPXMT_OFB_UDP_SRC:
			case OFPXMT_OFB_UDP_DST:
			case OFPXMT_OFB_SCTP_SRC:
			case OFPXMT_OFB_SCTP_DST:
			case OFPXMT_OFB_ARP_OP:
				{
					info.assign(vas("OXM-TLV [%s:%s] => [0x%04x] hm:%d len:%d padded-len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							uint16_value(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;

			case OFPXMT_OFB_VLAN_PCP:
			case OFPXMT_OFB_IP_DSCP:
			case OFPXMT_OFB_IP_ECN:
			case OFPXMT_OFB_IP_PROTO:
			case OFPXMT_OFB_ICMPV4_TYPE:
			case OFPXMT_OFB_ICMPV4_CODE:
			case OFPXMT_OFB_ICMPV6_TYPE:
			case OFPXMT_OFB_ICMPV6_CODE:
				{
					info.assign(vas("OXM-TLV [%s:%s] => [0x%02x] hm:%d len:%d padded-len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							uint8_value(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;

			default:
				{
					info.assign(vas("OXM-TLV [%s:%s] => [%s] hm:%d len:%d len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							cmemory::c_str(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;
			}
		}
		break;

	case OFPXMC_EXPERIMENTER:
		{
			switch (get_oxm_field()) {
			case OFPXMT_OFX_PPPOE_SID:
			case OFPXMT_OFX_PPP_PROT:
				{
					info.assign(vas("OXM-TLV [%s:%s] => [0x%04x] hm:%d len:%d padded-len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							uint16_value(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;

			case OFPXMT_OFX_PPPOE_CODE:
			case OFPXMT_OFX_PPPOE_TYPE:
				{
					info.assign(vas("OXM-TLV [%s:%s] => [0x%02x] hm:%d len:%d padded-len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							uint8_value(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;

			default:
				{
					info.assign(vas("OXM-TLV [%s:%s] => [%s] hm:%d len:%d len:%d",
							class2desc(get_oxm_class()),
							type2desc(get_oxm_class(), get_oxm_field()),
							cmemory::c_str(),
							get_oxm_hasmask(),
							get_oxm_length(),
							length()));
				}
				break;
			}
		}
		break;

	}


	return info.c_str();
}


void
coxmatch::set_oxm_class(
		uint16_t oxm_class)
{
	oxm_header->oxm_class = htobe16(oxm_class);
}


uint16_t
coxmatch::get_oxm_class() const
{
	return be16toh(oxm_header->oxm_class);
}


void
coxmatch::set_oxm_field(
		uint8_t oxm_field)
{
	oxm_header->oxm_field = (oxm_header->oxm_field & 0x01) | ((oxm_field & 0x7f) << 1);
}


uint8_t
coxmatch::get_oxm_field() const
{
	return ((oxm_header->oxm_field) >> 1);
}


void
coxmatch::set_oxm_hasmask(
		bool oxm_hasmask)
{
	oxm_header->oxm_field = (oxm_header->oxm_field & 0xfe) | oxm_hasmask;
}


bool
coxmatch::get_oxm_hasmask() const
{
	return (oxm_header->oxm_field & 0x01);
}


void
coxmatch::set_oxm_length(
		uint8_t oxm_len)
{
	oxm_header->oxm_length = oxm_len;
}


uint8_t
coxmatch::get_oxm_length()
{
	return oxm_header->oxm_length;
}



uint8_t
coxmatch::u8value() const
{
	return (uint8_value() & (get_oxm_hasmask() ? uint8_mask() : 0xff));
}



uint16_t
coxmatch::u16value() const
{
	return (uint16_value() & (get_oxm_hasmask() ? uint16_mask() : 0xffff));
}



uint32_t
coxmatch::u32value() const
{
	return (uint32_value() & (get_oxm_hasmask() ? uint32_mask() : 0xffffffff));
}



uint64_t
coxmatch::u64value() const
{
	return (uint64_value() & (get_oxm_hasmask() ? uint64_mask() : 0xffffffffffffffff));
}



caddress
coxmatch::u32addr() const
{
	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case OFPXMT_OFB_IPV4_SRC:
		case OFPXMT_OFB_IPV4_DST:
		case OFPXMT_OFB_ARP_SPA:
		case OFPXMT_OFB_ARP_TPA:
			break;
		default:
			throw eOxmInval();
			break;
		}
	} break;
	default: {
		throw eOxmInval();
	} break;
	}

	caddress addr(AF_INET);
	addr.ca_s4addr->sin_family = AF_INET;
	if (get_oxm_hasmask()) {
		addr.ca_s4addr->sin_addr.s_addr = (oxm_uint32t->dword & oxm_uint32t->mask);
	} else {
		addr.ca_s4addr->sin_addr.s_addr = oxm_uint32t->dword;
	}
	return addr;
}



cmacaddr
coxmatch::u48addr() const
{
	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case OFPXMT_OFB_ETH_DST:
		case OFPXMT_OFB_ETH_SRC:
		case OFPXMT_OFB_ARP_SHA:
		case OFPXMT_OFB_ARP_THA:
		case OFPXMT_OFB_IPV6_ND_SLL:
		case OFPXMT_OFB_IPV6_ND_TLL:
			break;
		default:
			throw eOxmInval();
			break;
		}
	} break;
	default: {
		throw eOxmInval();
	} break;
	}

	cmacaddr addr(oxm_uint48t->value, 6);
	if (get_oxm_hasmask()) {
		cmacaddr mask(oxm_uint48t->mask, 6);
		return (addr & mask);
	} else {
		return (addr);
	}
}



caddress
coxmatch::u128addr() const
{
	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case OFPXMT_OFB_IPV6_SRC:
		case OFPXMT_OFB_IPV6_DST:
		case OFPXMT_OFB_IPV6_ND_TARGET:
			break;
		default:
			throw eOxmInval();
			break;
		}
	} break;
	default: {
		throw eOxmInval();
	} break;
	}

	caddress addr(AF_INET6);
	addr.ca_s6addr->sin6_family = AF_INET6;
	memcpy(addr.ca_s6addr->sin6_addr.s6_addr, oxm_ipv6addr->addr, 16);
	if (get_oxm_hasmask()) {
		caddress mask(AF_INET6);
		mask.ca_s6addr->sin6_family = AF_INET6;
		memcpy(mask.ca_s6addr->sin6_addr.s6_addr, oxm_ipv6addr->mask, 16);
		return (addr & mask);
	} else {
		return (addr);
	}
}



uint8_t
coxmatch::uint8_value() const throw (eOxmInval)
{
	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
		{
			switch (get_oxm_field()) {
			case OFPXMT_OFB_VLAN_PCP:
			case OFPXMT_OFB_IP_DSCP:
			case OFPXMT_OFB_IP_ECN:
			case OFPXMT_OFB_IP_PROTO:
			case OFPXMT_OFB_ICMPV6_TYPE:
			case OFPXMT_OFB_ICMPV6_CODE:
			case OFPXMT_OFB_ICMPV4_TYPE:
			case OFPXMT_OFB_ICMPV4_CODE:
			case OFPXMT_OFB_MPLS_TC:
			case OFPXMT_OFX_PPPOE_CODE:
			case OFPXMT_OFX_PPPOE_TYPE:
				return oxm_uint8t->byte;

			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	throw eOxmInval();
}


uint8_t
coxmatch::uint8_mask() const throw (eOxmInval)
{
	if (not get_oxm_hasmask())
	{
		return 0xff;
	}

	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
		{
			switch (get_oxm_field()) {
			case OFPXMT_OFB_VLAN_PCP:
			case OFPXMT_OFB_IP_DSCP:
			case OFPXMT_OFB_IP_ECN:
			case OFPXMT_OFB_IP_PROTO:
			case OFPXMT_OFB_ICMPV6_TYPE:
			case OFPXMT_OFB_ICMPV6_CODE:
			case OFPXMT_OFB_ICMPV4_TYPE:
			case OFPXMT_OFB_ICMPV4_CODE:
			case OFPXMT_OFB_MPLS_TC:
			case OFPXMT_OFX_PPPOE_CODE:
			case OFPXMT_OFX_PPPOE_TYPE:
				return oxm_uint8t->mask;

			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	throw eOxmInval();
}


uint16_t
coxmatch::uint16_value() const throw (eOxmInval)
{
	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
		{
			switch (get_oxm_field()) {
			case OFPXMT_OFB_ETH_TYPE:
			case OFPXMT_OFB_VLAN_VID:
			case OFPXMT_OFB_TCP_SRC:
			case OFPXMT_OFB_TCP_DST:
			case OFPXMT_OFB_UDP_SRC:
			case OFPXMT_OFB_UDP_DST:
			case OFPXMT_OFB_SCTP_SRC:
			case OFPXMT_OFB_SCTP_DST:
			case OFPXMT_OFB_ARP_OP:
			case OFPXMT_OFX_PPPOE_SID:
			case OFPXMT_OFX_PPP_PROT:
				return be16toh(oxm_uint16t->word);

			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	throw eOxmInval();
}


uint16_t
coxmatch::uint16_mask() const throw (eOxmInval)
{
	if (not get_oxm_hasmask())
	{
		return 0xffff;
	}

	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
		{
			switch (get_oxm_field()) {
			case OFPXMT_OFB_ETH_TYPE:
			case OFPXMT_OFB_VLAN_VID:
			case OFPXMT_OFB_TCP_SRC:
			case OFPXMT_OFB_TCP_DST:
			case OFPXMT_OFB_UDP_SRC:
			case OFPXMT_OFB_UDP_DST:
			case OFPXMT_OFB_SCTP_SRC:
			case OFPXMT_OFB_SCTP_DST:
			case OFPXMT_OFB_ARP_OP:
			case OFPXMT_OFX_PPPOE_SID:
			case OFPXMT_OFX_PPP_PROT:
				return be16toh(oxm_uint16t->mask);

			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	throw eOxmInval();
}


uint32_t
coxmatch::uint32_value() const throw (eOxmInval)
{
	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
		{
			switch (get_oxm_field()) {
			case OFPXMT_OFB_IN_PORT:
			case OFPXMT_OFB_IN_PHY_PORT:
			case OFPXMT_OFB_IPV4_SRC:
			case OFPXMT_OFB_IPV4_DST:
			case OFPXMT_OFB_ARP_SPA:
			case OFPXMT_OFB_ARP_TPA:
			case OFPXMT_OFB_MPLS_LABEL:
			case OFPXMT_OFB_IPV6_FLABEL:
				return be32toh(oxm_uint32t->dword);

			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	throw eOxmInval();
}


uint32_t
coxmatch::uint32_mask() const throw (eOxmInval)
{
	if (not get_oxm_hasmask())
	{
		return 0xffffffff;
	}

	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
		{
			switch (get_oxm_field()) {
			case OFPXMT_OFB_IN_PORT:
			case OFPXMT_OFB_IN_PHY_PORT:
			case OFPXMT_OFB_IPV4_SRC:
			case OFPXMT_OFB_IPV4_DST:
			case OFPXMT_OFB_ARP_SPA:
			case OFPXMT_OFB_ARP_TPA:
			case OFPXMT_OFB_MPLS_LABEL:
			case OFPXMT_OFB_IPV6_FLABEL:
				return be32toh(oxm_uint32t->mask);

			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	throw eOxmInval();
}


uint64_t
coxmatch::uint64_value() const throw (eOxmInval)
{
	uint64_t value;

	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
		{
			switch (get_oxm_field()) {
			case OFPXMT_OFB_METADATA:
				memcpy(&value, oxm_uint64t->word, sizeof(uint64_t));
				return be64toh(value);

			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	throw eOxmInval();
}


uint64_t
coxmatch::uint64_mask() const throw (eOxmInval)
{
	if (not get_oxm_hasmask())
	{
		return 0xffffffffffffffff;
	}

	uint64_t mask;

	switch (get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
		{
			switch (get_oxm_field()) {
			case OFPXMT_OFB_METADATA:
				memcpy(&mask, oxm_uint64t->mask, sizeof(uint64_t));
				return be64toh(mask);

			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	throw eOxmInval();
}




coxmatch::oxm_classdesc_t oxm_classdesc[] = {
	{ OFPXMC_OPENFLOW_BASIC, 		"BASIC" },
	{ OFPXMC_EXPERIMENTER, 			"EXPERIMENTER" }
};


coxmatch::oxm_typedesc_t oxm_basic_typedesc[] = {
	{ OFPXMT_OFB_IN_PORT, 		"IN_PORT" },
	{ OFPXMT_OFB_IN_PHY_PORT, 	"IN_PHY_PORT" },
	{ OFPXMT_OFB_METADATA, 		"METADATA" },
	{ OFPXMT_OFB_ETH_DST, 		"ETH_DST" },
	{ OFPXMT_OFB_ETH_SRC, 		"ETH_SRC" },
	{ OFPXMT_OFB_ETH_TYPE, 		"ETH_TYPE" },
	{ OFPXMT_OFB_VLAN_VID, 		"VLAN_VID" },
	{ OFPXMT_OFB_VLAN_PCP, 		"VLAN_PCP" },
	{ OFPXMT_OFB_IP_DSCP, 		"IP_DSCP" },
	{ OFPXMT_OFB_IP_ECN, 		"IP_ECN" },
	{ OFPXMT_OFB_IP_PROTO, 		"IP_PROTO" },
	{ OFPXMT_OFB_IPV4_SRC, 		"IPV4_SRC" },
	{ OFPXMT_OFB_IPV4_DST, 		"IPV4_DST" },
	{ OFPXMT_OFB_TCP_SRC, 		"TCP_SRC" },
	{ OFPXMT_OFB_TCP_DST, 		"TCP_DST" },
	{ OFPXMT_OFB_UDP_SRC, 		"UDP_SRC" },
	{ OFPXMT_OFB_UDP_DST, 		"UDP_DST" },
	{ OFPXMT_OFB_SCTP_SRC, 		"SCTP_SRC" },
	{ OFPXMT_OFB_SCTP_DST, 		"SCTP_DST" },
	{ OFPXMT_OFB_ICMPV4_TYPE, 	"ICMPV4_TYPE" },
	{ OFPXMT_OFB_ICMPV4_CODE, 	"ICMPV4_CODE" },
	{ OFPXMT_OFB_ARP_OP, 		"ARP_OP" },
	{ OFPXMT_OFB_ARP_SPA, 		"ARP_SPA" },
	{ OFPXMT_OFB_ARP_TPA, 		"ARP_TPA" },
	{ OFPXMT_OFB_ARP_SHA, 		"ARP_SHA" },
	{ OFPXMT_OFB_ARP_THA, 		"ARP_THA" },
	{ OFPXMT_OFB_IPV6_SRC, 		"IPV6_SRC" },
	{ OFPXMT_OFB_IPV6_DST, 		"IPV6_DST" },
	{ OFPXMT_OFB_IPV6_FLABEL, 	"IPV6_FLABEL" },
	{ OFPXMT_OFB_ICMPV6_TYPE, 	"ICMPV6_TYPE" },
	{ OFPXMT_OFB_ICMPV6_CODE, 	"ICMPV6_CODE" },
	{ OFPXMT_OFB_IPV6_ND_TARGET,"IPV6_ND_TARGET" },
	{ OFPXMT_OFB_IPV6_ND_SLL, 	"IPV6_ND_SLL" },
	{ OFPXMT_OFB_IPV6_ND_TLL, 	"IPV6_ND_TLL" },
	{ OFPXMT_OFB_MPLS_LABEL, 	"MPLS_LABEL" },
	{ OFPXMT_OFB_MPLS_TC, 		"MPLS_TC" },
};



coxmatch::oxm_typedesc_t oxm_experimenter_typedesc[] = {
	{ OFPXMT_OFX_PPPOE_CODE, 	"PPPOE_CODE" },
	{ OFPXMT_OFX_PPPOE_TYPE, 	"PPPOE_TYPE" },
	{ OFPXMT_OFX_PPPOE_SID, 	"PPPOE_SID" },
	{ OFPXMT_OFX_PPP_PROT, 		"PPP_PROT" }
};



const char*
coxmatch::class2desc(uint16_t oxm_class)
{
	for (int i = 0; i < (int)(sizeof(oxm_classdesc) / sizeof(oxm_classdesc_t)); i++)
	{
		if (oxm_classdesc[i].oxm_class == oxm_class) {
			return oxm_classdesc[i].desc;
		}
	}
	return 0;
}



const char*
coxmatch::type2desc(uint16_t oxm_class, uint16_t oxm_field)
{
	switch (oxm_class) {
	case OFPXMC_OPENFLOW_BASIC:
		for (int i = 0; i < (int)(sizeof(oxm_basic_typedesc) / sizeof(oxm_typedesc_t)); i++) {
			if (oxm_basic_typedesc[i].type == oxm_field) {
				return oxm_basic_typedesc[i].desc;
			}
		}
		return 0;
	case OFPXMC_EXPERIMENTER:
		for (int i = 0; i < (int)(sizeof(oxm_experimenter_typedesc) / sizeof(oxm_typedesc_t)); i++) {
			if (oxm_experimenter_typedesc[i].type == oxm_field) {
				return oxm_experimenter_typedesc[i].desc;
			}
		}
		return 0;
	default:
		return 0;
	}
}




void
coxmatch::test()
{
#ifndef NDEBUG
	coxmatch oxm;
	fprintf(stderr, "default-oxm: %s\n", oxm.c_str());

	coxmatch_ofb_in_port in_port(48);
	fprintf(stderr, "OFPXMT_OFB_IN_PORT: %s\n", in_port.c_str());

	coxmatch_ofb_in_phy_port in_phy_port(48);
	fprintf(stderr, "OFPXMT_OFB_IN_PHY_PORT: %s\n", in_phy_port.c_str());

	coxmatch_ofb_metadata metadata(0xffeeffeeaabbaabb);
	fprintf(stderr, "OFPXMT_OFB_METADATA: %s\n", metadata.c_str());

	coxmatch_ofb_metadata metadata_mask(0xffeeffeeaabbaabb, 0x1122334444332211);
	fprintf(stderr, "OFPXMT_OFB_METADATA: %s\n", metadata_mask.c_str());

	coxmatch_ofb_eth_dst eth_dst(cmacaddr("11:11:11:11:11:11"));
	fprintf(stderr, "OFPXMT_OFB_ETH_DST: %s\n", eth_dst.c_str());

	coxmatch_ofb_eth_dst eth_dst_mask(cmacaddr("11:11:11:11:11:11"), cmacaddr("33:33:33:33:33:33"));
	fprintf(stderr, "OFPXMT_OFB_ETH_DST: %s\n", eth_dst_mask.c_str());

	coxmatch_ofb_eth_src eth_src(cmacaddr("22:22:22:22:22:22"));
	fprintf(stderr, "OFPXMT_OFB_ETH_SRC: %s\n", eth_src.c_str());

	coxmatch_ofb_eth_src eth_src_mask(cmacaddr("22:22:22:22:22:22"), cmacaddr("44:44:44:44:44:44"));
	fprintf(stderr, "OFPXMT_OFB_ETH_SRC: %s\n", eth_src_mask.c_str());

	coxmatch_ofb_eth_type eth_type(0x8863);
	fprintf(stderr, "OFPXMT_OFB_ETH_TYPE: %s\n", eth_type.c_str());

	coxmatch_ofb_vlan_vid vlan_vid(4095);
	fprintf(stderr, "OFPXMT_OFB_VLAN_VID: %s\n", vlan_vid.c_str());

	coxmatch_ofb_vlan_vid vlan_vid_mask(4095, 0x0600);
	fprintf(stderr, "OFPXMT_OFB_VLAN_VID: %s\n", vlan_vid_mask.c_str());

	coxmatch_ofb_vlan_pcp vlan_pcp(3);
	fprintf(stderr, "OFPXMT_OFB_VLAN_PCP: %s\n", vlan_pcp.c_str());

	coxmatch_ofb_ip_dscp ip_dscp(5);
	fprintf(stderr, "OFPXMT_OFB_IP_DSCP: %s\n", ip_dscp.c_str());

	coxmatch_ofb_ip_ecn ip_ecn(3);
	fprintf(stderr, "OFPXMT_OFB_IP_ECN: %s\n", ip_ecn.c_str());

	coxmatch_ofb_ip_proto ip_proto(6);
	fprintf(stderr, "OFPXMT_OFB_IP_PROTO: %s\n", ip_proto.c_str());

	coxmatch_ofb_ipv4_src ipv4_src(caddress(AF_INET, "1.1.1.1"));
	fprintf(stderr, "OFPXMT_OFB_IPV4_SRC: %s\n", ipv4_src.c_str());

	coxmatch_ofb_ipv4_src ipv4_src_mask(caddress(AF_INET, "1.1.1.1"), caddress(AF_INET, "3.3.3.3"));
	fprintf(stderr, "OFPXMT_OFB_IPV4_SRC: %s\n", ipv4_src_mask.c_str());

	coxmatch_ofb_ipv4_dst ipv4_dst(caddress(AF_INET, "2.2.2.2"));
	fprintf(stderr, "OFPXMT_OFB_IPV4_DST: %s\n", ipv4_dst.c_str());

	coxmatch_ofb_ipv4_dst ipv4_dst_mask(caddress(AF_INET, "2.2.2.2"), caddress(AF_INET, "4.4.4.4"));
	fprintf(stderr, "OFPXMT_OFB_IPV4_DST: %s\n", ipv4_dst_mask.c_str());

	coxmatch_ofb_tcp_src tcp_src(80);
	fprintf(stderr, "OFPXMT_OFB_TCP_SRC: %s\n", tcp_src.c_str());

	coxmatch_ofb_tcp_dst tcp_dst(8080);
	fprintf(stderr, "OFPXMT_OFB_TCP_DST: %s\n", tcp_dst.c_str());

	coxmatch_ofb_udp_src udp_src(80);
	fprintf(stderr, "OFPXMT_OFB_UDP_SRC: %s\n", udp_src.c_str());

	coxmatch_ofb_udp_dst udp_dst(8080);
	fprintf(stderr, "OFPXMT_OFB_UDP_DST: %s\n", udp_dst.c_str());

	coxmatch_ofb_sctp_src sctp_src(80);
	fprintf(stderr, "OFPXMT_OFB_SCTP_SRC: %s\n", sctp_src.c_str());

	coxmatch_ofb_sctp_dst sctp_dst(8080);
	fprintf(stderr, "OFPXMT_OFB_SCTP_DST: %s\n", sctp_dst.c_str());

	coxmatch_ofb_icmpv4_type icmpv4_type(1);
	fprintf(stderr, "OFPXMT_OFB_ICMPV4_TYPE: %s\n", icmpv4_type.c_str());

	coxmatch_ofb_icmpv4_code icmpv4_code(1);
	fprintf(stderr, "OFPXMT_OFB_ICMPV4_CODE: %s\n", icmpv4_code.c_str());

	coxmatch_ofb_arp_op arp_op(1);
	fprintf(stderr, "OFPXMT_OFB_ARP_OP: %s\n", arp_op.c_str());

	coxmatch_ofb_arp_spa arp_spa(0x01010101);
	fprintf(stderr, "OFPXMT_OFB_ARP_SPA: %s\n", arp_spa.c_str());

	coxmatch_ofb_arp_spa arp_spa_mask(0x01010101, 0x03030303);
	fprintf(stderr, "OFPXMT_OFB_ARP_SPA: %s\n", arp_spa_mask.c_str());

	coxmatch_ofb_arp_tpa arp_tpa(0x02020202);
	fprintf(stderr, "OFPXMT_OFB_ARP_TPA: %s\n", arp_tpa.c_str());

	coxmatch_ofb_arp_tpa arp_tpa_mask(0x02020202, 0x04040404);
	fprintf(stderr, "OFPXMT_OFB_ARP_TPA: %s\n", arp_tpa_mask.c_str());

	coxmatch_ofb_arp_sha arp_sha(cmacaddr("11:11:11:11:11:11"));
	fprintf(stderr, "OFPXMT_OFB_ARP_SHA: %s\n", arp_sha.c_str());

	coxmatch_ofb_arp_sha arp_sha_mask(cmacaddr("11:11:11:11:11:11"), cmacaddr("33:33:33:33:33:33"));
	fprintf(stderr, "OFPXMT_OFB_ARP_SHA: %s\n", arp_sha_mask.c_str());

	coxmatch_ofb_arp_tha arp_tha(cmacaddr("22:22:22:22:22:22"));
	fprintf(stderr, "OFPXMT_OFB_ARP_THA: %s\n", arp_tha.c_str());

	coxmatch_ofb_arp_tha arp_tha_mask(cmacaddr("22:22:22:22:22:22"), cmacaddr("44:44:44:44:44:44"));
	fprintf(stderr, "OFPXMT_OFB_ARP_THA: %s\n", arp_tha_mask.c_str());

	coxmatch_ofb_ipv6_src ipv6_src(caddress(AF_INET6, "1001::1"));
	fprintf(stderr, "OFPXMT_OFB_IPV6_SRC: %s\n", ipv6_src.c_str());

	coxmatch_ofb_ipv6_src ipv6_src_mask(caddress(AF_INET6, "1001::1"), caddress(AF_INET6, "3003::3"));
	fprintf(stderr, "OFPXMT_OFB_IPV6_SRC: %s\n", ipv6_src_mask.c_str());

	coxmatch_ofb_ipv6_dst ipv6_dst(caddress(AF_INET6, "2002::2"));
	fprintf(stderr, "OFPXMT_OFB_IPV6_DST: %s\n", ipv6_dst.c_str());

	coxmatch_ofb_ipv6_dst ipv6_dst_mask(caddress(AF_INET6, "2002::2"), caddress(AF_INET6, "4004::4"));
	fprintf(stderr, "OFPXMT_OFB_IPV6_DST: %s\n", ipv6_dst_mask.c_str());

	coxmatch_ofb_ipv6_flabel ipv6_flabel(3333);
	fprintf(stderr, "OFPXMT_OFB_IPV6_FLABEL: %s\n", ipv6_flabel.c_str());

	coxmatch_ofb_ipv6_flabel ipv6_flabel_mask(3333, 6666);
	fprintf(stderr, "OFPXMT_OFB_IPV6_FLABEL: %s\n", ipv6_flabel_mask.c_str());

	coxmatch_ofb_icmpv6_type icmpv6_type(1);
	fprintf(stderr, "OFPXMT_OFB_ICMPV6_TYPE: %s\n", icmpv6_type.c_str());

	coxmatch_ofb_icmpv6_code icmpv6_code(1);
	fprintf(stderr, "OFPXMT_OFB_ICMPV6_CODE: %s\n", icmpv6_code.c_str());

	coxmatch_ofb_mpls_label mpls_label(0x555);
	fprintf(stderr, "OFPXMT_OFB_MPLS_LABEL: %s\n", mpls_label.c_str());

	coxmatch_ofb_mpls_tc mpls_tc(7);
	fprintf(stderr, "OFPXMT_OFB_MPLS_TC: %s\n", mpls_tc.c_str());
#endif
}


template class coflist<coxmatch>;
