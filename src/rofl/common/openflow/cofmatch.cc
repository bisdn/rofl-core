/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofmatch.h"

using namespace rofl;

cofmatch::cofmatch(
		uint16_t type)
{
	//WRITELOG(COFMATCH, DBG, "cofmatch(%p)::cofmatch() [1]", this);

	bzero(&match, sizeof(match));

	match.type = htobe16(type);
	match.length = htobe16(length());

	reset();

	validate();
}

cofmatch::cofmatch(
	struct ofp_match *__match, size_t __matchlen)
{
	//WRITELOG(COFMATCH, DBG, "cofmatch(%p)::cofmatch() [2]", this);

	validate();
}

cofmatch::~cofmatch()
{
	//WRITELOG(COFMATCH, DBG, "cofmatch(%p)::~cofmatch()", this);
}


cofmatch&
cofmatch::operator= (const cofmatch& m)
{
	if (this == &m)
		return *this;

#if 0
	WRITELOG(COFMATCH, DBG, "cofmatch(%p)::operator=() m:%p", this, &m);

	WRITELOG(COFMATCH, DBG, "cofmatch(%p)::operator=() [1] *this: %s", this, this->c_str());
#endif

	match.type 		= m.match.type;
	match.length 	= m.match.length;
	oxmlist			= m.oxmlist;

#if 0
	WRITELOG(COFMATCH, DBG, "cofmatch(%p)::operator=() [2] *this: %s", this, this->c_str());
#endif

	validate();

	return *this;
}



bool
cofmatch::operator< (
		cofmatch const& m) const
{
	return (oxmlist < m.oxmlist);
}



void
cofmatch::reset()
{
	//WRITELOG(COFMATCH, DBG, "cofmatch(%p)::reset()", this);

	oxmlist.clear();
	match.length = htobe16(length());

}



bool
cofmatch::is_matching(
		cofmatch& other,
		uint16_t& exact_matches,
		uint16_t& wildcard_matches,
		uint16_t& missed)
{
	WRITELOG(COXMLIST, DBG, "cofmatch(%p)::is_matching()", this);

	exact_matches = 0;
	wildcard_matches = 0;
	missed = 0;

	coxmatch** left = this->oxmlist.oxmvec;
	coxmatch** right = other.oxmlist.oxmvec;

	for (unsigned int i = 0; i < OFPXMT_OFB_MAX; i++)
	{
		if ((coxmatch*)0 == left[i])
		{
			// left side is null => wildcard match
			wildcard_matches++;
#if 0
			WRITELOG(COXMLIST, DBG, "cofmatch(%p)::is_matching() "
					"wildcard match => left is 0", this);
#endif

		}
		else if (((coxmatch*)0 != left[i]) && ((coxmatch*)0 == right[i]))
		{
			// left side is non-null, but right side is null => miss
			missed++;

			WRITELOG(COXMLIST, DBG, "cofmatch(%p)::is_matching() "
					"miss => left is %s != right is 0", this,
					this->oxmlist[i].c_str());

			return false;
		}
		else if (this->oxmlist[i] != other.oxmlist[i])
		{
			// left and right side are non-null and do not match => miss

			WRITELOG(COXMLIST, DBG, "cofmatch(%p)::is_matching() "
					"miss => %s != %s", this,
					this->oxmlist[i].c_str(), other.oxmlist[i].c_str());

			missed++;

			return false;
		}
		else
		{
			// left and right side are non-null and match => exact match
			exact_matches++;

			WRITELOG(COXMLIST, DBG, "cofmatch(%p)::is_matching() "
					"exact match => %s == %s", this,
					this->oxmlist[i].c_str(), other.oxmlist[i].c_str());
		}
	}
	return true;
}




void
cofmatch::remove(
		uint16_t oxm_class,
		uint8_t oxm_field)
{
	oxmlist.erase(oxm_class, oxm_field);
}



void
cofmatch::validate() throw (eOFmatchInval)
{
	// TODO: apply OF1.2 prerequisites here
}


size_t
cofmatch::length_internal()
{
	/*
	 * returns length of struct ofp_match including padding
	 */
	size_t match_len = 2 * sizeof(uint16_t); // first two 16bit fields in struct ofp_match

	match_len += oxmlist.length();

	if (0 != (match_len % sizeof(uint64_t)))
	{
		match_len = ((match_len / sizeof(uint64_t)) + 1) * sizeof(uint64_t);
	}
	return match_len;
}

size_t
cofmatch::length()
{
	size_t total_length = sizeof(match.type) + sizeof(match.length) + oxmlist.length();

	size_t pad = (0x7 & total_length);
	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}
	return total_length;
}


struct ofp_match*
cofmatch::pack(struct ofp_match* m, size_t mlen) throw (eOFmatchInval)
{
	if (mlen < length())
	{
		throw eOFmatchInval();
	}


	//match.length = htobe16(sizeof(match.type) + sizeof(match.length) + oxmlist.length());

	//match.length = htobe16(length());
	/*
	 * fill in real length, i.e. excluding padding
	 */
	match.length = htobe16(2 * sizeof(uint16_t) + oxmlist.length());

	m->type 	= match.type;
	m->length 	= match.length;

	oxmlist.pack((struct ofp_oxm_hdr*)(m->oxm_fields), oxmlist.length());

	return m;
}


void
cofmatch::unpack(struct ofp_match* m, size_t mlen) throw (eOFmatchInval)
{
	if (mlen < (sizeof(uint16_t) + sizeof(uint16_t)))
	{
		throw eOFmatchInval();
	}

	match.type		= (m->type);
	match.length	= (m->length);

	if (OFPMT_OXM != be16toh(match.type)) {
		throw eBadMatchBadType();
	}

	mlen -= 2 * sizeof(uint16_t);

	if (mlen > 0)
	{
		oxmlist.unpack((struct ofp_oxm_hdr*)m->oxm_fields, mlen);
	}

	validate();
}


bool 
cofmatch::overlaps(
	cofmatch const& m,
	bool strict /* = false (default) */)
{
	return (oxmlist.overlap(m.oxmlist, strict));
}

	

bool 
cofmatch::operator== (
	cofmatch& m)
{
	return ((match.type == m.match.type) && (oxmlist == m.oxmlist));
}


const char*
cofmatch::c_str()
{
	cvastring vas(3172);

	info.assign(vas("cofmatch(%p) hdr.type:%d hdr.length:%d stored:%lu oxmlist.length:%lu oxmlist:%s",
			this,
			be16toh(match.type),
			be16toh(match.length),
			length(),
			oxmlist.length(),
			oxmlist.c_str()));

	return info.c_str();
}


void
cofmatch::set_type(uint16_t type)
{
	match.type = htobe16(type);
}


uint32_t
cofmatch::get_in_port() throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IN_PORT))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_IN_PORT].uint32_value();
}


void
cofmatch::set_in_port(uint32_t in_port)
{
	oxmlist[OFPXMT_OFB_IN_PORT] = coxmatch_ofb_in_port(in_port);
}


uint32_t
cofmatch::get_in_phy_port() throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IN_PHY_PORT))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_IN_PHY_PORT].uint32_value();
}


void
cofmatch::set_in_phy_port(uint32_t in_port)
{
	oxmlist[OFPXMT_OFB_IN_PHY_PORT] = coxmatch_ofb_in_phy_port(in_port);
}


uint64_t
cofmatch::get_metadata() throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_METADATA))
	{
		throw eOFmatchNotFound();
	}

	return (oxmlist[OFPXMT_OFB_METADATA].uint64_value() & oxmlist[OFPXMT_OFB_METADATA].uint64_mask());
}


void
cofmatch::set_metadata(uint64_t metadata)
{
	oxmlist[OFPXMT_OFB_METADATA] = coxmatch_ofb_metadata(metadata);
}


cmacaddr
cofmatch::get_eth_dst() throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_DST))
	{
		throw eOFmatchNotFound();
	}

	cmacaddr maddr(oxmlist[OFPXMT_OFB_ETH_DST].oxm_uint48t->value, OFP_ETH_ALEN);

	cmacaddr mmask("ff:ff:ff:ff:ff:ff");
	if (oxmlist[OFPXMT_OFB_ETH_DST].get_oxm_hasmask())
	{
		mmask.assign(oxmlist[OFPXMT_OFB_ETH_DST].oxm_uint48t->mask, OFP_ETH_ALEN);
	}

	return (maddr & mmask);
}


cmacaddr
cofmatch::get_eth_src() throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_SRC))
	{
		throw eOFmatchNotFound();
	}

	cmacaddr maddr(oxmlist[OFPXMT_OFB_ETH_SRC].oxm_uint48t->value, OFP_ETH_ALEN);

	cmacaddr mmask("ff:ff:ff:ff:ff:ff");
	if (oxmlist[OFPXMT_OFB_ETH_SRC].get_oxm_hasmask())
	{
		mmask.assign(oxmlist[OFPXMT_OFB_ETH_SRC].oxm_uint48t->mask, OFP_ETH_ALEN);
	}
	return (maddr & mmask);
}


void
cofmatch::set_eth_dst(cmacaddr const& maddr, cmacaddr const& mmask)
{
	if (cmacaddr("ff:ff:ff:ff:ff:ff") == mmask)
	{
		oxmlist[OFPXMT_OFB_ETH_DST] = coxmatch_ofb_eth_dst(maddr);
	}
	else
	{
		oxmlist[OFPXMT_OFB_ETH_DST] = coxmatch_ofb_eth_dst(maddr, mmask);
	}
}


void
cofmatch::set_eth_src(cmacaddr const& maddr, cmacaddr const& mmask)
{
	if (cmacaddr("ff:ff:ff:ff:ff:ff") == mmask)
	{
		oxmlist[OFPXMT_OFB_ETH_SRC] = coxmatch_ofb_eth_src(maddr);
	}
	else
	{
		oxmlist[OFPXMT_OFB_ETH_SRC] = coxmatch_ofb_eth_src(maddr, mmask);
	}
}


uint16_t
cofmatch::get_eth_type() throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_TYPE))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_ETH_TYPE].uint16_value();
}


void
cofmatch::set_eth_type(
		uint16_t dl_type)
{
	oxmlist[OFPXMT_OFB_ETH_TYPE] = coxmatch_ofb_eth_type(dl_type);
}



uint16_t
cofmatch::get_vlan_vid()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_VID))
	{
		throw eOFmatchNotFound();
	}

	return (oxmlist[OFPXMT_OFB_VLAN_VID].uint16_value() & oxmlist[OFPXMT_OFB_VLAN_VID].uint16_mask());
}



void
cofmatch::set_vlan_vid(
		uint16_t vid)
{
	oxmlist[OFPXMT_OFB_VLAN_VID] = coxmatch_ofb_vlan_vid(vid);
}



uint8_t
cofmatch::get_vlan_pcp()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_PCP))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_VLAN_PCP].uint8_value();
}



void
cofmatch::set_vlan_pcp(
		uint8_t pcp)
{
	oxmlist[OFPXMT_OFB_VLAN_PCP] = coxmatch_ofb_vlan_pcp(pcp);
}



uint32_t
cofmatch::get_mpls_label()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_LABEL))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_MPLS_LABEL].uint32_value();
}


void
cofmatch::set_mpls_label(
		uint32_t label)
{
	oxmlist[OFPXMT_OFB_MPLS_LABEL] = coxmatch_ofb_mpls_label(label);
}


uint8_t
cofmatch::get_mpls_tc()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_TC))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_MPLS_TC].uint8_value();
}


void
cofmatch::set_mpls_tc(
		uint8_t tc)
{
	oxmlist[OFPXMT_OFB_MPLS_TC] = coxmatch_ofb_mpls_tc(tc);
}



uint8_t
cofmatch::get_pppoe_type()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_TYPE))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_PPPOE_TYPE].uint8_value();
}



void
cofmatch::set_pppoe_type(
		uint8_t type)
{
	oxmlist[OFPXMT_OFB_PPPOE_TYPE] = coxmatch_ofb_pppoe_type(type);
}



uint8_t
cofmatch::get_pppoe_code()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_CODE))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_PPPOE_CODE].uint8_value();
}


void
cofmatch::set_pppoe_code(
		uint8_t code)
{
	oxmlist[OFPXMT_OFB_PPPOE_CODE] = coxmatch_ofb_pppoe_code(code);
}


uint16_t
cofmatch::get_pppoe_sessid()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_SID))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_PPPOE_SID].uint16_value();
}


void
cofmatch::set_pppoe_sessid(
		uint16_t sid)
{
	oxmlist[OFPXMT_OFB_PPPOE_SID] = coxmatch_ofb_pppoe_sid(sid);
}


uint16_t
cofmatch::get_ppp_prot()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPP_PROT))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_PPP_PROT].uint16_value();
}


void
cofmatch::set_ppp_prot(
		uint16_t prot)
{
	oxmlist[OFPXMT_OFB_PPP_PROT] = coxmatch_ofb_ppp_prot(prot);
}


caddress
cofmatch::get_ipv4_src()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_SRC))
	{
		throw eOFmatchNotFound();
	}

	caddress src(AF_INET, "0.0.0.0");
	if (oxmlist[OFPXMT_OFB_IPV4_SRC].get_oxm_hasmask())
	{
		src.ca_s4addr->sin_addr.s_addr = htobe32(oxmlist[OFPXMT_OFB_IPV4_SRC].uint32_value() & oxmlist[OFPXMT_OFB_IPV4_SRC].uint32_mask());
	}
	else
	{
		src.ca_s4addr->sin_addr.s_addr = htobe32(oxmlist[OFPXMT_OFB_IPV4_SRC].uint32_value());
	}

	return src;
}



void
cofmatch::set_ipv4_src(
		caddress const& src)
{
	oxmlist[OFPXMT_OFB_IPV4_SRC] = coxmatch_ofb_ipv4_src(src);
}


void
cofmatch::set_ipv4_src(
		caddress const& src,
		caddress const& mask)
{
	oxmlist[OFPXMT_OFB_IPV4_SRC] = coxmatch_ofb_ipv4_src(src, mask);
}



caddress
cofmatch::get_ipv4_dst()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_DST))
	{
		throw eOFmatchNotFound();
	}

	caddress dst(AF_INET, "0.0.0.0");
	if (oxmlist[OFPXMT_OFB_IPV4_DST].get_oxm_hasmask())
	{
		dst.ca_s4addr->sin_addr.s_addr = htobe32(oxmlist[OFPXMT_OFB_IPV4_DST].uint32_value() & oxmlist[OFPXMT_OFB_IPV4_DST].uint32_mask());
	}
	else
	{
		dst.ca_s4addr->sin_addr.s_addr = htobe32(oxmlist[OFPXMT_OFB_IPV4_DST].uint32_value());
	}

	return dst;
}



void
cofmatch::set_ipv4_dst(
		caddress const& dst)
{
	oxmlist[OFPXMT_OFB_IPV4_DST] = coxmatch_ofb_ipv4_dst(dst);
}


void
cofmatch::set_ipv4_dst(
		caddress const& dst,
		caddress const& mask)
{
	oxmlist[OFPXMT_OFB_IPV4_DST] = coxmatch_ofb_ipv4_dst(dst, mask);
}



uint16_t
cofmatch::get_arp_opcode()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ARP_OP))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_ARP_OP].uint16_value();
}



void
cofmatch::set_arp_opcode(
		uint16_t opcode)
{
	oxmlist[OFPXMT_OFB_ARP_OP] = coxmatch_ofb_arp_op(opcode);
}



cmacaddr
cofmatch::get_arp_sha()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ARP_SHA))
	{
		throw eOFmatchNotFound();
	}

	cmacaddr sha(oxmlist[OFPXMT_OFB_ARP_SHA].oxm_uint48t->value, OFP_ETH_ALEN);

	cmacaddr mask("ff:ff:ff:ff:ff:ff");
	if (oxmlist[OFPXMT_OFB_ARP_SHA].get_oxm_hasmask())
	{
		mask.assign(oxmlist[OFPXMT_OFB_ARP_SHA].oxm_uint48t->mask, OFP_ETH_ALEN);
	}

	return (sha & mask);
}



void
cofmatch::set_arp_sha(
		cmacaddr const& sha)
{
	oxmlist[OFPXMT_OFB_ARP_SHA] = coxmatch_ofb_arp_sha(sha);
}



cmacaddr
cofmatch::get_arp_tha()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ARP_THA))
	{
		throw eOFmatchNotFound();
	}

	cmacaddr tha(oxmlist[OFPXMT_OFB_ARP_THA].oxm_uint48t->value, OFP_ETH_ALEN);

	cmacaddr mask("ff:ff:ff:ff:ff:ff");
	if (oxmlist[OFPXMT_OFB_ARP_THA].get_oxm_hasmask())
	{
		mask.assign(oxmlist[OFPXMT_OFB_ARP_THA].oxm_uint48t->mask, OFP_ETH_ALEN);
	}

	return (tha & mask);
}



void
cofmatch::set_arp_tha(
		cmacaddr const& tha)
{
	oxmlist[OFPXMT_OFB_ARP_THA] = coxmatch_ofb_arp_tha(tha);
}



caddress
cofmatch::get_arp_spa()
		throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ARP_SPA))
	{
		throw eOFmatchNotFound();
	}

	caddress spa(AF_INET, "0.0.0.0");
	if (oxmlist[OFPXMT_OFB_ARP_SPA].get_oxm_hasmask())
	{
		spa.ca_s4addr->sin_addr.s_addr = htobe32(oxmlist[OFPXMT_OFB_ARP_SPA].uint32_value() & oxmlist[OFPXMT_OFB_ARP_SPA].uint32_mask());
	}
	else
	{
		spa.ca_s4addr->sin_addr.s_addr = htobe32(oxmlist[OFPXMT_OFB_ARP_SPA].uint32_value());
	}

	return spa;
}



void
cofmatch::set_arp_spa(
		caddress const& spa)
{
	oxmlist[OFPXMT_OFB_ARP_SPA] = coxmatch_ofb_arp_spa(spa);
}



caddress
cofmatch::get_arp_tpa()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ARP_TPA))
	{
		throw eOFmatchNotFound();
	}

	caddress tpa(AF_INET, "0.0.0.0");
	if (oxmlist[OFPXMT_OFB_ARP_TPA].get_oxm_hasmask())
	{
		tpa.ca_s4addr->sin_addr.s_addr = htobe32(oxmlist[OFPXMT_OFB_ARP_TPA].uint32_value() & oxmlist[OFPXMT_OFB_ARP_TPA].uint32_mask());
	}
	else
	{
		tpa.ca_s4addr->sin_addr.s_addr = htobe32(oxmlist[OFPXMT_OFB_ARP_TPA].uint32_value());
	}

	return tpa;
}



void
cofmatch::set_arp_tpa(
		caddress const& tpa)
{
	oxmlist[OFPXMT_OFB_ARP_TPA] = coxmatch_ofb_arp_tpa(tpa);
}



uint8_t
cofmatch::get_ip_proto()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IP_PROTO))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_IP_PROTO].uint8_value();
}



void
cofmatch::set_ip_proto(
		uint8_t proto)
{
	oxmlist[OFPXMT_OFB_IP_PROTO] = coxmatch_ofb_ip_proto(proto);
}



uint8_t
cofmatch::get_ip_dscp()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IP_DSCP))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_IP_DSCP].uint8_value();
}



void
cofmatch::set_ip_dscp(
		uint8_t dscp)
{
	oxmlist[OFPXMT_OFB_IP_DSCP] = coxmatch_ofb_ip_dscp(dscp);
}



uint8_t
cofmatch::get_ip_ecn()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IP_ECN))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_IP_ECN].uint8_value();
}



void
cofmatch::set_ip_ecn(
		uint8_t ecn)
{
	oxmlist[OFPXMT_OFB_IP_ECN] = coxmatch_ofb_ip_ecn(ecn);
}



uint16_t
cofmatch::get_icmpv4_type()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ICMPV4_TYPE))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_ICMPV4_TYPE].uint16_value();
}



void
cofmatch::set_icmpv4_type(
		uint16_t type)
{
	oxmlist[OFPXMT_OFB_ICMPV4_TYPE] = coxmatch_ofb_icmpv4_type(type);
}



uint16_t
cofmatch::get_icmpv4_code()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ICMPV4_CODE))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_ICMPV4_CODE].uint16_value();
}



void
cofmatch::set_icmpv4_code(
		uint16_t code)
{
	oxmlist[OFPXMT_OFB_ICMPV4_CODE] = coxmatch_ofb_icmpv4_code(code);
}



uint16_t
cofmatch::get_udp_src() throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_UDP_SRC))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_UDP_SRC].uint16_value();
}


void
cofmatch::set_udp_src(
		uint16_t src_port)
{
	oxmlist[OFPXMT_OFB_UDP_SRC] = coxmatch_ofb_udp_src(src_port);
}


uint16_t
cofmatch::get_udp_dst() throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_UDP_DST))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_UDP_DST].uint16_value();
}


void
cofmatch::set_udp_dst(
		uint16_t dst_port)
{
	oxmlist[OFPXMT_OFB_UDP_DST] = coxmatch_ofb_udp_dst(dst_port);
}



uint16_t
cofmatch::get_tcp_src()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_SRC))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_TCP_SRC].uint16_value();
}



void
cofmatch::set_tcp_src(
		uint16_t src_port)
{
	oxmlist[OFPXMT_OFB_TCP_SRC] = coxmatch_ofb_tcp_src(src_port);
}



uint16_t
cofmatch::get_tcp_dst()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_DST))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_TCP_DST].uint16_value();
}



void
cofmatch::set_tcp_dst(
		uint16_t dst_port)
{
	oxmlist[OFPXMT_OFB_TCP_DST] = coxmatch_ofb_tcp_dst(dst_port);
}




uint16_t
cofmatch::get_sctp_src()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_SCTP_SRC))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_SCTP_SRC].uint16_value();
}



void
cofmatch::set_sctp_src(
		uint16_t src_port)
{
	oxmlist[OFPXMT_OFB_SCTP_SRC] = coxmatch_ofb_sctp_src(src_port);
}



uint16_t
cofmatch::get_sctp_dst()
	throw (eOFmatchNotFound)
{
	if (not oxmlist.exists(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_SCTP_DST))
	{
		throw eOFmatchNotFound();
	}

	return oxmlist[OFPXMT_OFB_SCTP_DST].uint16_value();
}



void
cofmatch::set_sctp_dst(
		uint16_t dst_port)
{
	oxmlist[OFPXMT_OFB_SCTP_DST] = coxmatch_ofb_sctp_dst(dst_port);
}



#ifndef NDEBUG
void
cofmatch::test()
{
	cofmatch m;

	m.oxmlist[OFPXMT_OFB_ETH_SRC] 	= coxmatch_ofb_eth_src(cmacaddr("11:11:11:11:11:11"), cmacaddr("33:33:33:33:33:33"));
	m.oxmlist[OFPXMT_OFB_VLAN_VID] 	= coxmatch_ofb_vlan_vid(1000);
	m.oxmlist[OFPXMT_OFB_IP_DSCP] 	= coxmatch_ofb_ip_dscp(6);

	cmemory mem(m.length());

	m.pack((struct ofp_match*)mem.somem(), mem.memlen());
	fprintf(stderr, "match: %s\nmem:%s\n\n", m.c_str(), mem.c_str());

	cofmatch tm(m);

	fprintf(stderr, "tm: %s\n\n", tm.c_str());

	cofmatch cm;

	cm.unpack((struct ofp_match*)mem.somem(), mem.memlen());
	fprintf(stderr, "unpack: %s\n\n", cm.c_str());

	{
		cofmatch m;

		m.set_in_port(47);
		m.set_in_phy_port(47);
		m.set_eth_dst(cmacaddr("11:11:11:11:11:11"));
		m.set_eth_src(cmacaddr("22:22:22:22:22:22"));

		fprintf(stderr, "cofmatch: %s\n\n", m.c_str());
	}
}
#endif

