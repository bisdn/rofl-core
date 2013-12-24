/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmlist.cc
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */


#include "coxmlist.h"

using namespace rofl;

coxmlist::coxmlist()
{

}


coxmlist::~coxmlist()
{
	clear();
}


coxmlist::coxmlist(
		coxmlist const& oxmlist)
{
	*this = oxmlist;
}


coxmlist&
coxmlist::operator= (
		coxmlist const& oxl)
{
	if (this == &oxl)
		return *this;

	clear();

	for (std::map<uint16_t, std::map<uint8_t, coxmatch*> >::const_iterator
			it = oxl.matches.begin(); it != oxl.matches.end(); ++it) {
		for (std::map<uint8_t, coxmatch*>::const_iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt) {
#if 0
			matches[it->first][jt->first] = new coxmatch(*(jt->second));
#endif
			map_and_insert(*(jt->second));
		}
	}

	return *this;
}




void
coxmlist::clear()
{
	for (std::map<uint16_t, std::map<uint8_t, coxmatch*> >::iterator
			it = matches.begin(); it != matches.end(); ++it) {
		for (std::map<uint8_t, coxmatch*>::iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt) {
			 delete jt->second;
		}
	}
	matches.clear();
}



bool
coxmlist::operator== (coxmlist const& oxmlist)
{
	return this->contains(oxmlist, true/*strict*/);
}





void
coxmlist::unpack(
		uint8_t* buf,
		size_t buflen)
{
	clear();

	// sanity check: oxm_len must be of size at least of ofp_oxm_hdr
	if (buflen < (int)sizeof(struct rofl::openflow::ofp_oxm_hdr)) {
		throw eBadMatchBadLen();
	}

	// first instruction
	struct openflow::ofp_oxm_hdr *hdr = (struct openflow::ofp_oxm_hdr*)buf;


	while (buflen > 0) {
		if ((buflen < sizeof(struct openflow::ofp_oxm_hdr)) || (0 == hdr->oxm_length)) {
			return; // not enough bytes to parse an entire ofp_oxm_hdr, possibly padding bytes found
		}

		if (hdr->oxm_length > (sizeof(struct openflow::ofp_oxm_hdr) + buflen))
			throw eBadMatchBadLen();

		coxmatch oxm(hdr, sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length);

		map_and_insert(oxm);

		buflen -= (sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length);
		hdr = (struct openflow::ofp_oxm_hdr*)(((uint8_t*)hdr) + sizeof(struct openflow::ofp_oxm_hdr) + hdr->oxm_length);
	}
}



void
coxmlist::pack(
		uint8_t* buf,
		size_t buflen)
{
	if (buflen < length()) {
		throw eBadMatchBadLen();
	}

	for (std::map<uint16_t, std::map<uint8_t, coxmatch*> >::iterator
			it = matches.begin(); it != matches.end(); ++it) {
		for (std::map<uint8_t, coxmatch*>::iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt) {

			coxmatch& match = *(matches[it->first][jt->first]);

			match.pack(buf, match.length());

			buf += match.length();
		}
	}
}



unsigned int
coxmlist::get_n_matches() const
{
	unsigned int n_matches = 0;
	for (std::map<uint16_t, std::map<uint8_t, coxmatch*> >::const_iterator
			it = matches.begin(); it != matches.end(); ++it) {
		n_matches += it->second.size();
	}
	return n_matches;
}



size_t
coxmlist::length() const
{
	size_t len = 0;
	for (std::map<uint16_t, std::map<uint8_t, coxmatch*> >::const_iterator
			it = matches.begin(); it != matches.end(); ++it) {
		for (std::map<uint8_t, coxmatch*>::const_iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt) {

			coxmatch const& match = *(jt->second);

			len += match.length();
		}
	}
	return len;
}



coxmatch&
coxmlist::get_match(uint16_t ofm_class, uint8_t ofm_field)
{
	if (matches[ofm_class].find(ofm_field) == matches[ofm_class].end())
		throw eOxmListNotFound();
	return *(matches[ofm_class][ofm_field]);
}



coxmatch const&
coxmlist::get_const_match(
		uint16_t ofm_class, uint8_t ofm_field) const
{
	for (std::map<uint16_t, std::map<uint8_t, coxmatch*> >::const_iterator
			it = matches.begin(); it != matches.end(); ++it) {
		for (std::map<uint8_t, coxmatch*>::const_iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt) {
			if ((ofm_class == it->first) && (ofm_field == jt->first)) {
				return *(jt->second);
			}
		}
	}
	throw eOxmListNotFound();
}



void
coxmlist::insert(
		coxmatch const& oxm)
{
	map_and_insert(oxm);
}



void
coxmlist::erase(
		uint16_t oxm_class,
		uint8_t oxm_field)
{
	if (matches[oxm_class].find(oxm_field) == matches[oxm_class].end()) {
		return;
	}

	delete matches[oxm_class][oxm_field];
	matches[oxm_class].erase(oxm_field);
}




bool
coxmlist::contains(
	coxmlist const& oxl,
	bool strict /* = false (default) */)
{
	/*
	 * strict:
	 * all elemens in *this and oxl must be identical (number of and value of elements)
	 *
	 * non-strict:
	 * all elements in oxl must be present in *this (value of elements), but *this may contain additional OXM TLVs
	 */

	std::ostringstream oxlos; oxlos << oxl;
	std::ostringstream os; os << *this;
	WRITELOG(COXMLIST, DBG, "coxmlist(%p)::contains() [%s]\nus:%s vs.\nthem:%s",
			this, (strict) ? "strict" : "non-strict", os.str().c_str(), oxlos.str().c_str());


	WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() [strict]", this);

	for (std::map<uint16_t, std::map<uint8_t, coxmatch*> >::const_iterator
			it = matches.begin(); it != matches.end(); ++it) {

		if (oxl.matches.find(it->first) == oxl.matches.end()) {
			return false;
		}

		std::map<uint8_t, coxmatch*>&       lmap = matches[it->first];
		std::map<uint8_t, coxmatch*> const& rmap = oxl.matches.find(it->first)->second;

		if (strict) {
			// strict: # of elems for an ofm_class must be the same in oxl for the specific ofm_class
			if (lmap.size() != rmap.size()) {
				return false;
			}
		} else  /* non-strict*/ {
			// non-strict: # of elems for a class must be larger (or equal) the number of elements in oxl
			if (lmap.size() > rmap.size()) {
				return false;
			}
		}

		// strict: check all TLVs for specific class in oxl.matches => must exist and have same value
		for (std::map<uint8_t, coxmatch*>::const_iterator
				jt = lmap.begin(); jt != lmap.end(); ++jt) {

			coxmatch&       lmatch = *(jt->second);

			// strict: all OXM TLVs must also exist in oxl
			if (rmap.find(lmatch.get_oxm_field()) == rmap.end()) {
				return false;
			}

			coxmatch const& rmatch = *(rmap.find(jt->first)->second);

			// strict: both OXM TLVs must have identical values
			if (lmatch != rmatch) {
				return false;
			}
		}
	}
	return true;
}




bool
coxmlist::is_part_of(
		coxmlist const& oxl,
		uint16_t& exact_hits,
		uint16_t& wildcard_hits,
		uint16_t& missed)
{
	bool result = true;

	std::ostringstream oxlos; oxlos << oxl;
	std::ostringstream os; os << *this;
	WRITELOG(COXMLIST, DBG, "coxmlist(%p)::is_part_of()\n\tus:%s\n\tthem:%s\n",
			this, os.str().c_str(), oxlos.str().c_str());

	for (std::map<uint16_t, std::map<uint8_t, coxmatch*> >::const_iterator
			it = oxl.matches.begin(); it != oxl.matches.end(); ++it) {

		std::map<uint8_t, coxmatch*>&       lmap = matches.find(it->first)->second;
		std::map<uint8_t, coxmatch*> const& rmap = it->second;

		for (std::map<uint8_t, coxmatch*>::const_iterator
				jt = rmap.begin(); jt != rmap.end(); ++jt) {

			coxmatch const& rmatch = *(jt->second);

			if (lmap.find(rmatch.get_oxm_field()) == lmap.end()) {
				wildcard_hits++; continue;
			}

			coxmatch& lmatch = *(lmap.find(jt->first)->second);

			if (lmatch != rmatch) {
				missed++; result = false;
			}

			exact_hits++;
		}
	}

	return result;
}



void
coxmlist::map_and_insert(coxmatch const& oxm)
{
	// avoid memory leaks
	if (matches[oxm.get_oxm_class()].find(oxm.get_oxm_field()) != matches[oxm.get_oxm_class()].end()) {
		delete matches[oxm.get_oxm_class()][oxm.get_oxm_field()];
		matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = (coxmatch*)0;
	}

	switch (oxm.get_oxm_class()) {
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		switch (oxm.get_oxm_field()) {
		case openflow::OFPXMT_OFB_IN_PORT:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_in_port(oxm); break;
		case openflow::OFPXMT_OFB_IN_PHY_PORT:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_in_phy_port(oxm); break;
		case openflow::OFPXMT_OFB_METADATA:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_metadata(oxm); break;
		case openflow::OFPXMT_OFB_ETH_DST:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_eth_dst(oxm); break;
		case openflow::OFPXMT_OFB_ETH_SRC:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_eth_src(oxm); break;
		case openflow::OFPXMT_OFB_ETH_TYPE:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_eth_type(oxm); break;
		case openflow::OFPXMT_OFB_VLAN_VID:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_vlan_vid(oxm); break;
		case openflow::OFPXMT_OFB_VLAN_PCP:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_vlan_pcp(oxm); break;
		case openflow::OFPXMT_OFB_IP_DSCP:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ip_dscp(oxm); break;
		case openflow::OFPXMT_OFB_IP_ECN:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ip_ecn(oxm); break;
		case openflow::OFPXMT_OFB_IP_PROTO:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ip_proto(oxm); break;
		case openflow::OFPXMT_OFB_IPV4_SRC:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ipv4_src(oxm); break;
		case openflow::OFPXMT_OFB_IPV4_DST:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ipv4_dst(oxm); break;
		case openflow::OFPXMT_OFB_TCP_SRC:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_tcp_src(oxm); break;
		case openflow::OFPXMT_OFB_TCP_DST:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_tcp_dst(oxm); break;
		case openflow::OFPXMT_OFB_UDP_SRC:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_udp_src(oxm); break;
		case openflow::OFPXMT_OFB_UDP_DST:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_udp_dst(oxm); break;
		case openflow::OFPXMT_OFB_SCTP_SRC:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_sctp_src(oxm); break;
		case openflow::OFPXMT_OFB_SCTP_DST:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_sctp_dst(oxm); break;
		case openflow::OFPXMT_OFB_ICMPV4_TYPE:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_icmpv4_type(oxm); break;
		case openflow::OFPXMT_OFB_ICMPV4_CODE:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_icmpv4_code(oxm); break;
		case openflow::OFPXMT_OFB_ARP_OP:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_arp_opcode(oxm); break;
		case openflow::OFPXMT_OFB_ARP_SPA:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_arp_spa(oxm); break;
		case openflow::OFPXMT_OFB_ARP_TPA:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_arp_tpa(oxm); break;
		case openflow::OFPXMT_OFB_ARP_SHA:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_arp_sha(oxm); break;
		case openflow::OFPXMT_OFB_ARP_THA:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_arp_tha(oxm); break;
		case openflow::OFPXMT_OFB_IPV6_SRC:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ipv6_src(oxm); break;
		case openflow::OFPXMT_OFB_IPV6_DST:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ipv6_dst(oxm); break;
		case openflow::OFPXMT_OFB_IPV6_FLABEL:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ipv6_flabel(oxm); break;
		case openflow::OFPXMT_OFB_ICMPV6_TYPE:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_icmpv6_type(oxm); break;
		case openflow::OFPXMT_OFB_ICMPV6_CODE:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_icmpv6_code(oxm); break;
		case openflow::OFPXMT_OFB_IPV6_ND_TARGET:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ipv6_nd_target(oxm); break;
		case openflow::OFPXMT_OFB_IPV6_ND_SLL:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ipv6_nd_sll(oxm); break;
		case openflow::OFPXMT_OFB_IPV6_ND_TLL:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_ipv6_nd_tll(oxm); break;
		case openflow::OFPXMT_OFB_MPLS_LABEL:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_mpls_label(oxm); break;
		case openflow::OFPXMT_OFB_MPLS_TC:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofb_mpls_tc(oxm); break;
		default:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch(oxm);
		}
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		switch (oxm.get_oxm_field()) {
		case openflow::experimental::OFPXMT_OFX_NW_SRC:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_nw_src(oxm); break;
		case openflow::experimental::OFPXMT_OFX_NW_DST:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_nw_dst(oxm); break;
		case openflow::experimental::OFPXMT_OFX_NW_PROTO:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_nw_proto(oxm); break;
		case openflow::experimental::OFPXMT_OFX_TP_SRC:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_tp_src(oxm); break;
		case openflow::experimental::OFPXMT_OFX_TP_DST:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_tp_dst(oxm); break;
		case openflow::experimental::OFPXMT_OFX_PPPOE_CODE:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_pppoe_code(oxm); break;
		case openflow::experimental::OFPXMT_OFX_PPPOE_TYPE:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_pppoe_type(oxm); break;
		case openflow::experimental::OFPXMT_OFX_PPPOE_SID:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_pppoe_sid(oxm); break;
		case openflow::experimental::OFPXMT_OFX_PPP_PROT:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_ppp_prot(oxm); break;
		case openflow::experimental::OFPXMT_OFX_GTP_MSG_TYPE:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_gtp_msg_type(oxm); break;
		case openflow::experimental::OFPXMT_OFX_GTP_TEID:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch_ofx_gtp_teid(oxm); break;
		default:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch(oxm);
		}
	} break;
	case openflow::OFPXMC_NXM_0:
	case openflow::OFPXMC_NXM_1:
	default: {
		switch (oxm.get_oxm_field()) {
		default:
			matches[oxm.get_oxm_class()][oxm.get_oxm_field()] = new coxmatch(oxm);
		}
	} break;
	}

}
