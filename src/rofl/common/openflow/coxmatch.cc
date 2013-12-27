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
	oxm_header = (struct rofl::openflow::ofp_oxm_hdr*)somem();
}


coxmatch::coxmatch(
		struct rofl::openflow::ofp_oxm_hdr* hdr,
		size_t oxm_len) :
		cmemory((uint8_t*)hdr, oxm_len)
{
	pthread_rwlock_init(&oxmlock, 0);
	oxm_header = (struct rofl::openflow::ofp_oxm_hdr*)somem();
}


coxmatch::coxmatch(
		struct openflow12::ofp_action_set_field *ach,
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
	oxm_header = (struct rofl::openflow::ofp_oxm_hdr*)somem();

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

	oxm_header = (struct openflow::ofp_oxm_hdr*)somem();

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
	oxm_header = (struct openflow::ofp_oxm_hdr*)somem();
}


struct openflow::ofp_oxm_hdr*
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
		oxm_generic = resize(buflen);
	}

	assign(buf, buflen);
	oxm_generic = somem();
}


#if 0
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
	case openflow::OFPXMC_OPENFLOW_BASIC:
		{
			switch (get_oxm_field()) {
			case openflow::OFPXMT_OFB_IPV6_SRC:
			case openflow::OFPXMT_OFB_IPV6_DST:
			case openflow::OFPXMT_OFB_IPV6_ND_TARGET:
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

			case openflow::OFPXMT_OFB_METADATA:
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

			case openflow::OFPXMT_OFB_ETH_DST:
			case openflow::OFPXMT_OFB_ETH_SRC:
			case openflow::OFPXMT_OFB_ARP_SHA:
			case openflow::OFPXMT_OFB_ARP_THA:
			case openflow::OFPXMT_OFB_IPV6_ND_SLL:
			case openflow::OFPXMT_OFB_IPV6_ND_TLL:
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

			case openflow::OFPXMT_OFB_IN_PORT:
			case openflow::OFPXMT_OFB_IN_PHY_PORT:
			case openflow::OFPXMT_OFB_MPLS_LABEL:
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

			case openflow::OFPXMT_OFB_IPV4_SRC:
			case openflow::OFPXMT_OFB_IPV4_DST:
			case openflow::OFPXMT_OFB_ARP_SPA:
			case openflow::OFPXMT_OFB_ARP_TPA:
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

			case openflow::OFPXMT_OFB_IPV6_FLABEL:
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

			case openflow::OFPXMT_OFB_ETH_TYPE:
			case openflow::OFPXMT_OFB_VLAN_VID:
			case openflow::OFPXMT_OFB_TCP_SRC:
			case openflow::OFPXMT_OFB_TCP_DST:
			case openflow::OFPXMT_OFB_UDP_SRC:
			case openflow::OFPXMT_OFB_UDP_DST:
			case openflow::OFPXMT_OFB_SCTP_SRC:
			case openflow::OFPXMT_OFB_SCTP_DST:
			case openflow::OFPXMT_OFB_ARP_OP:
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

			case openflow::OFPXMT_OFB_VLAN_PCP:
			case openflow::OFPXMT_OFB_IP_DSCP:
			case openflow::OFPXMT_OFB_IP_ECN:
			case openflow::OFPXMT_OFB_IP_PROTO:
			case openflow::OFPXMT_OFB_ICMPV4_TYPE:
			case openflow::OFPXMT_OFB_ICMPV4_CODE:
			case openflow::OFPXMT_OFB_ICMPV6_TYPE:
			case openflow::OFPXMT_OFB_ICMPV6_CODE:
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

	case openflow::OFPXMC_EXPERIMENTER:
		{
			switch (get_oxm_field()) {
			case openflow::OFPXMT_OFX_PPPOE_SID:
			case openflow::OFPXMT_OFX_PPP_PROT:
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

			case openflow::OFPXMT_OFX_PPPOE_CODE:
			case openflow::OFPXMT_OFX_PPPOE_TYPE:
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
#endif


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
	return (uint64_value() & (get_oxm_hasmask() ? uint64_mask() : 0xffffffffffffffffULL));
}



caddress
coxmatch::u32addr() const
{
	switch (get_oxm_class()) {
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_IPV4_SRC:
		case openflow::OFPXMT_OFB_IPV4_DST:
		case openflow::OFPXMT_OFB_ARP_SPA:
		case openflow::OFPXMT_OFB_ARP_TPA:
			break;
		default:
			throw eOxmInval();
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		// do nothing
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
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_ETH_DST:
		case openflow::OFPXMT_OFB_ETH_SRC:
		case openflow::OFPXMT_OFB_ARP_SHA:
		case openflow::OFPXMT_OFB_ARP_THA:
		case openflow::OFPXMT_OFB_IPV6_ND_SLL:
		case openflow::OFPXMT_OFB_IPV6_ND_TLL:
			break;
		default:
			throw eOxmInval();
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		// do nothing
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
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_IPV6_SRC:
		case openflow::OFPXMT_OFB_IPV6_DST:
		case openflow::OFPXMT_OFB_IPV6_ND_TARGET:
			break;
		default:
			throw eOxmInval();
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		// do nothing
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
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_VLAN_PCP:
		case openflow::OFPXMT_OFB_IP_DSCP:
		case openflow::OFPXMT_OFB_IP_ECN:
		case openflow::OFPXMT_OFB_IP_PROTO:
		case openflow::OFPXMT_OFB_ICMPV6_TYPE:
		case openflow::OFPXMT_OFB_ICMPV6_CODE:
		case openflow::OFPXMT_OFB_ICMPV4_TYPE:
		case openflow::OFPXMT_OFB_ICMPV4_CODE:
		case openflow::OFPXMT_OFB_MPLS_TC:
			return oxm_uint8t->byte;

		default:
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (get_oxm_field()) {
		default:
			return oxm_uint8t->byte;

		}
	} break;
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
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_VLAN_PCP:
		case openflow::OFPXMT_OFB_IP_DSCP:
		case openflow::OFPXMT_OFB_IP_ECN:
		case openflow::OFPXMT_OFB_IP_PROTO:
		case openflow::OFPXMT_OFB_ICMPV6_TYPE:
		case openflow::OFPXMT_OFB_ICMPV6_CODE:
		case openflow::OFPXMT_OFB_ICMPV4_TYPE:
		case openflow::OFPXMT_OFB_ICMPV4_CODE:
		case openflow::OFPXMT_OFB_MPLS_TC:
			return oxm_uint8t->mask;

		default:
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (get_oxm_field()) {
		default:
			return oxm_uint8t->mask;
		}
	} break;
	default:
		break;
	}
	throw eOxmInval();
}


uint16_t
coxmatch::uint16_value() const throw (eOxmInval)
{
	switch (get_oxm_class()) {
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_ETH_TYPE:
		case openflow::OFPXMT_OFB_VLAN_VID:
		case openflow::OFPXMT_OFB_TCP_SRC:
		case openflow::OFPXMT_OFB_TCP_DST:
		case openflow::OFPXMT_OFB_UDP_SRC:
		case openflow::OFPXMT_OFB_UDP_DST:
		case openflow::OFPXMT_OFB_SCTP_SRC:
		case openflow::OFPXMT_OFB_SCTP_DST:
		case openflow::OFPXMT_OFB_ARP_OP:
		case openflow::experimental::OFPXMT_OFX_PPPOE_SID:
		case openflow::experimental::OFPXMT_OFX_PPP_PROT:
			return be16toh(oxm_uint16t->word);

		default:
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (get_oxm_field()) {
		default:
			return be16toh(oxm_uint16t->word);
		}
	} break;
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
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_ETH_TYPE:
		case openflow::OFPXMT_OFB_VLAN_VID:
		case openflow::OFPXMT_OFB_TCP_SRC:
		case openflow::OFPXMT_OFB_TCP_DST:
		case openflow::OFPXMT_OFB_UDP_SRC:
		case openflow::OFPXMT_OFB_UDP_DST:
		case openflow::OFPXMT_OFB_SCTP_SRC:
		case openflow::OFPXMT_OFB_SCTP_DST:
		case openflow::OFPXMT_OFB_ARP_OP:
			return be16toh(oxm_uint16t->mask);

		default:
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (get_oxm_field()) {
		default:
			return be16toh(oxm_uint16t->mask);
		}
	} break;
	default:
		break;
	}
	throw eOxmInval();
}


uint32_t
coxmatch::uint32_value() const throw (eOxmInval)
{
	switch (get_oxm_class()) {
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_IN_PORT:
		case openflow::OFPXMT_OFB_IN_PHY_PORT:
		case openflow::OFPXMT_OFB_IPV4_SRC:
		case openflow::OFPXMT_OFB_IPV4_DST:
		case openflow::OFPXMT_OFB_ARP_SPA:
		case openflow::OFPXMT_OFB_ARP_TPA:
		case openflow::OFPXMT_OFB_MPLS_LABEL:
		case openflow::OFPXMT_OFB_IPV6_FLABEL:
			return be32toh(oxm_uint32t->dword);

		default:
			break;
		}
	}
	break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (get_oxm_field()) {
		default:
			return be32toh(oxm_uint32t->dword);
		}
	} break;
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
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_IN_PORT:
		case openflow::OFPXMT_OFB_IN_PHY_PORT:
		case openflow::OFPXMT_OFB_IPV4_SRC:
		case openflow::OFPXMT_OFB_IPV4_DST:
		case openflow::OFPXMT_OFB_ARP_SPA:
		case openflow::OFPXMT_OFB_ARP_TPA:
		case openflow::OFPXMT_OFB_MPLS_LABEL:
		case openflow::OFPXMT_OFB_IPV6_FLABEL:
			return be32toh(oxm_uint32t->mask);

		default:
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (get_oxm_field()) {
		default:
			return be32toh(oxm_uint32t->mask);
		}
	} break;
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
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_METADATA:
			memcpy(&value, oxm_uint64t->word, sizeof(uint64_t));
			return be64toh(value);

		default:
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (get_oxm_field()) {
		default:
			memcpy(&value, oxm_uint64t->word, sizeof(uint64_t));
			return be64toh(value);
		}
	} break;
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
		return 0xffffffffffffffffULL;
	}

	uint64_t mask;

	switch (get_oxm_class()) {
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_METADATA:
			memcpy(&mask, oxm_uint64t->mask, sizeof(uint64_t));
			return be64toh(mask);

		default:
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (get_oxm_field()) {
		default:
			memcpy(&mask, oxm_uint64t->mask, sizeof(uint64_t));
			return be64toh(mask);
		}
	} break;
	default:
		break;
	}
	throw eOxmInval();
}

uint128__t
coxmatch::uint128_value() const throw (eOxmInval)
{
	uint128__t value;
	
	switch (get_oxm_class()) {
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_IPV6_SRC:
		case openflow::OFPXMT_OFB_IPV6_DST:
		case openflow::OFPXMT_OFB_IPV6_ND_TARGET:
		case openflow::OFPXMT_OFB_METADATA:
			memcpy(&value, oxm_ipv6addr->addr, sizeof(uint128__t));
#if __BYTE_ORDER == __LITTLE_ENDIAN
			SWAP_U128(value);
#endif
			return value;

		default:
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (get_oxm_field()) {
		default:
			memcpy(&value, oxm_ipv6addr->addr, sizeof(uint128__t));
#if __BYTE_ORDER == __LITTLE_ENDIAN
			SWAP_U128(value);
#endif
			return value;
		}
	} break;
	default:
		break;
	}
	throw eOxmInval();
}

uint128__t
coxmatch::uint128_mask() const throw (eOxmInval)
{
	uint128__t mask;
	//TODO translate to 128 bits
	if (not get_oxm_hasmask())
	{
		UINT128__T_HI(mask) = 0xffffffffffffffffULL;
		UINT128__T_LO(mask) = 0xffffffffffffffffULL;
		return mask;
	}

	switch (get_oxm_class()) {
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (get_oxm_field()) {
		case openflow::OFPXMT_OFB_IPV6_SRC:
		case openflow::OFPXMT_OFB_IPV6_DST:
		case openflow::OFPXMT_OFB_IPV6_ND_TARGET:
		case openflow::OFPXMT_OFB_METADATA:
			memcpy(&mask, oxm_ipv6addr->mask, sizeof(uint128__t));
#if __BYTE_ORDER == __LITTLE_ENDIAN
			SWAP_U128(mask);
#endif
			return mask;

		default:
			break;
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (get_oxm_field()) {
		default:
			memcpy(&mask, oxm_ipv6addr->mask, sizeof(uint128__t));
#if __BYTE_ORDER == __LITTLE_ENDIAN
			SWAP_U128(mask);
#endif
			return mask;
		}
	} break;
	default:
		break;
	}
	throw eOxmInval();
}

coxmatch::oxm_classdesc_t oxm_classdesc[] = {
	{ openflow::OFPXMC_OPENFLOW_BASIC, 		"BASIC" },
	{ openflow::OFPXMC_EXPERIMENTER, 			"EXPERIMENTER" }
};


coxmatch::oxm_typedesc_t oxm_basic_typedesc[] = {
	{ openflow::OFPXMT_OFB_IN_PORT, 		"IN_PORT" },
	{ openflow::OFPXMT_OFB_IN_PHY_PORT, 	"IN_PHY_PORT" },
	{ openflow::OFPXMT_OFB_METADATA, 		"METADATA" },
	{ openflow::OFPXMT_OFB_ETH_DST, 		"ETH_DST" },
	{ openflow::OFPXMT_OFB_ETH_SRC, 		"ETH_SRC" },
	{ openflow::OFPXMT_OFB_ETH_TYPE, 		"ETH_TYPE" },
	{ openflow::OFPXMT_OFB_VLAN_VID, 		"VLAN_VID" },
	{ openflow::OFPXMT_OFB_VLAN_PCP, 		"VLAN_PCP" },
	{ openflow::OFPXMT_OFB_IP_DSCP, 		"IP_DSCP" },
	{ openflow::OFPXMT_OFB_IP_ECN, 		"IP_ECN" },
	{ openflow::OFPXMT_OFB_IP_PROTO, 		"IP_PROTO" },
	{ openflow::OFPXMT_OFB_IPV4_SRC, 		"IPV4_SRC" },
	{ openflow::OFPXMT_OFB_IPV4_DST, 		"IPV4_DST" },
	{ openflow::OFPXMT_OFB_TCP_SRC, 		"TCP_SRC" },
	{ openflow::OFPXMT_OFB_TCP_DST, 		"TCP_DST" },
	{ openflow::OFPXMT_OFB_UDP_SRC, 		"UDP_SRC" },
	{ openflow::OFPXMT_OFB_UDP_DST, 		"UDP_DST" },
	{ openflow::OFPXMT_OFB_SCTP_SRC, 		"SCTP_SRC" },
	{ openflow::OFPXMT_OFB_SCTP_DST, 		"SCTP_DST" },
	{ openflow::OFPXMT_OFB_ICMPV4_TYPE, 	"ICMPV4_TYPE" },
	{ openflow::OFPXMT_OFB_ICMPV4_CODE, 	"ICMPV4_CODE" },
	{ openflow::OFPXMT_OFB_ARP_OP, 		"ARP_OP" },
	{ openflow::OFPXMT_OFB_ARP_SPA, 		"ARP_SPA" },
	{ openflow::OFPXMT_OFB_ARP_TPA, 		"ARP_TPA" },
	{ openflow::OFPXMT_OFB_ARP_SHA, 		"ARP_SHA" },
	{ openflow::OFPXMT_OFB_ARP_THA, 		"ARP_THA" },
	{ openflow::OFPXMT_OFB_IPV6_SRC, 		"IPV6_SRC" },
	{ openflow::OFPXMT_OFB_IPV6_DST, 		"IPV6_DST" },
	{ openflow::OFPXMT_OFB_IPV6_FLABEL, 	"IPV6_FLABEL" },
	{ openflow::OFPXMT_OFB_ICMPV6_TYPE, 	"ICMPV6_TYPE" },
	{ openflow::OFPXMT_OFB_ICMPV6_CODE, 	"ICMPV6_CODE" },
	{ openflow::OFPXMT_OFB_IPV6_ND_TARGET,"IPV6_ND_TARGET" },
	{ openflow::OFPXMT_OFB_IPV6_ND_SLL, 	"IPV6_ND_SLL" },
	{ openflow::OFPXMT_OFB_IPV6_ND_TLL, 	"IPV6_ND_TLL" },
	{ openflow::OFPXMT_OFB_MPLS_LABEL, 	"MPLS_LABEL" },
	{ openflow::OFPXMT_OFB_MPLS_TC, 		"MPLS_TC" },
};



coxmatch::oxm_typedesc_t oxm_experimenter_typedesc[] = {
	{ openflow::experimental::OFPXMT_OFX_PPPOE_CODE, 	"PPPOE_CODE" },
	{ openflow::experimental::OFPXMT_OFX_PPPOE_TYPE, 	"PPPOE_TYPE" },
	{ openflow::experimental::OFPXMT_OFX_PPPOE_SID, 	"PPPOE_SID" },
	{ openflow::experimental::OFPXMT_OFX_PPP_PROT, 		"PPP_PROT" }
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
	case openflow::OFPXMC_OPENFLOW_BASIC:
		for (int i = 0; i < (int)(sizeof(oxm_basic_typedesc) / sizeof(oxm_typedesc_t)); i++) {
			if (oxm_basic_typedesc[i].type == oxm_field) {
				return oxm_basic_typedesc[i].desc;
			}
		}
		return 0;
	case openflow::OFPXMC_EXPERIMENTER:
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


