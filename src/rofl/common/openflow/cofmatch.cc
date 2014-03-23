/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofmatch.h"

using namespace rofl::openflow;

cofmatch::cofmatch(
		uint8_t of_version,
		uint16_t type) :
				of_version(of_version)
{
	switch (of_version) {
	case openflow::OFP_VERSION_UNKNOWN: {
		ofh_match = 0;
	} break;
	case openflow10::OFP_VERSION: {
		memarea.resize(openflow10::OFP_MATCH_STATIC_LEN);
		ofh10_match = (struct openflow10::ofp_match*)memarea.somem();
	} break;
	case openflow12::OFP_VERSION: {
		memarea.resize(openflow12::OFP_MATCH_STATIC_LEN);
		ofh12_match = (struct openflow12::ofp_match*)memarea.somem();
		ofh12_match->type 	= htobe16(type);
		ofh12_match->length = htobe16(length());
	} break;
	case openflow13::OFP_VERSION: {
		memarea.resize(openflow13::OFP_MATCH_STATIC_LEN);
		ofh13_match = (struct openflow13::ofp_match*)memarea.somem();
		ofh13_match->type 	= htobe16(type);
		ofh13_match->length = htobe16(length());
	} break;
	default:
		throw eBadVersion();
	}

	clear();

	validate();
}




cofmatch::~cofmatch()
{

}



cofmatch&
cofmatch::operator= (const cofmatch& m)
{
	if (this == &m)
		return *this;

	of_version		= m.of_version;
	memarea			= m.memarea;
	oxmtlvs			= m.oxmtlvs;

	ofh_match = memarea.somem();

	validate();

	return *this;
}



bool
cofmatch::operator== (const cofmatch& m)
{
	return (
		(of_version == m.of_version) &&
		(memarea 	== m.memarea) &&
		(oxmtlvs 	== m.oxmtlvs));
}



void
cofmatch::clear()
{
	switch (of_version) {
	case openflow::OFP_VERSION_UNKNOWN: {
		oxmtlvs.clear();
		memarea.clear();
		ofh_match = 0;
	} break;
	case openflow10::OFP_VERSION: {
		oxmtlvs.clear();
		memset(ofh10_match, 0, openflow10::OFP_MATCH_STATIC_LEN);
	} break;
	case openflow12::OFP_VERSION: {
		oxmtlvs.clear();
		ofh12_match->length = htobe16(length());
	} break;
	case openflow13::OFP_VERSION: {
		oxmtlvs.clear();
		ofh13_match->length = htobe16(length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmatch::validate() throw (eOFmatchInval)
{
	// TODO: apply OF1.2 prerequisites here
}



size_t
cofmatch::length_internal()
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return openflow10::OFP_MATCH_STATIC_LEN;
	} break;
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		/*
		 * returns length of struct ofp_match including padding
		 */
		size_t match_len = 2 * sizeof(uint16_t); // first two 16bit fields in struct ofp_match

		match_len += oxmtlvs.length();

		if (0 != (match_len % sizeof(uint64_t)))
		{
			match_len = ((match_len / sizeof(uint64_t)) + 1) * sizeof(uint64_t);
		}
		return match_len;
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



size_t
cofmatch::length() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return openflow10::OFP_MATCH_STATIC_LEN;
	} break;
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		size_t total_length = 2*sizeof(uint16_t) + oxmtlvs.length(); // type + length + list

		size_t pad = (0x7 & total_length);
		/* append padding if not a multiple of 8 */
		if (pad) {
			total_length += 8 - pad;
		}
		return total_length;
	}
	default:
		throw eBadVersion();
	}
	return 0;
}



uint8_t*
cofmatch::pack(uint8_t* m, size_t mlen)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: return pack_of10(m, mlen); break;
	case openflow12::OFP_VERSION: return pack_of12(m, mlen); break;
	case openflow13::OFP_VERSION: return pack_of13(m, mlen); break;
	default: throw eBadVersion();
	}
}



void
cofmatch::unpack(uint8_t *m, size_t mlen)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: unpack_of10(m, mlen); break;
	case openflow12::OFP_VERSION: unpack_of12(m, mlen); break;
	case openflow13::OFP_VERSION: unpack_of13(m, mlen); break;
	default: throw eBadVersion();
	}
}



uint8_t*
cofmatch::pack_of10(uint8_t* match, size_t matchlen)
{
	if (matchlen < length()) {
		throw eOFmatchInval();
	}

	uint32_t wildcards = 0;

	/*
	 * fill in real length, i.e. excluding padding
	 */
	memset(match, 0, matchlen);

	struct openflow10::ofp_match *m = (struct openflow10::ofp_match*)match;

	// in_port
	try {
		m->in_port = htobe16((uint16_t)(oxmtlvs.get_match(OXM_TLV_BASIC_IN_PORT).get_u32value() && 0x0000ffff));
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_IN_PORT;
	}

	// dl_src
	try {
		memcpy(m->dl_src, oxmtlvs.get_match(OXM_TLV_BASIC_ETH_SRC).get_u48value().somem(), OFP_ETH_ALEN);
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_DL_SRC;
	}

	// dl_dst
	try {
		memcpy(m->dl_dst, oxmtlvs.get_match(OXM_TLV_BASIC_ETH_DST).get_u48value().somem(), OFP_ETH_ALEN);
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_DL_DST;
	}

	// dl_vlan
	try {
		m->dl_vlan = htobe16(oxmtlvs.get_match(OXM_TLV_BASIC_VLAN_VID).get_u16value());
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_DL_VLAN;
	}

	// dl_vlan_pcp
	try {
		if(oxmtlvs.get_match(OXM_TLV_BASIC_VLAN_VID).get_u16value() != openflow10::OFP_VLAN_NONE)
			m->dl_vlan_pcp = oxmtlvs.get_match(OXM_TLV_BASIC_VLAN_PCP).get_u8value();
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_DL_VLAN_PCP;
	}

	// dl_type
	try {
		m->dl_type = htobe16(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value());
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_DL_TYPE;
	}

	// nw_tos
	try {
		m->nw_tos = oxmtlvs.get_match(experimental::OXM_TLV_EXPR_NW_TOS).get_u8value();
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_NW_TOS;
	}

	// nw_proto
	try {
		m->nw_proto = oxmtlvs.get_match(experimental::OXM_TLV_EXPR_NW_PROTO).get_u8value();
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_NW_PROTO;
	}

	// nw_src
	try {
		coxmatch const& oxm = oxmtlvs.get_match(experimental::OXM_TLV_EXPR_NW_SRC);
		m->nw_src = htobe32(oxm.get_u32value());
		if (oxm.get_oxm_hasmask()) {
			std::bitset<32> mask(oxm.get_u32mask());
			wildcards |= ((32 - mask.count()) << openflow10::OFPFW_NW_SRC_SHIFT) & openflow10::OFPFW_NW_SRC_MASK;
		}
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_NW_SRC_ALL;
	}


	// nw_dst
	try {
		coxmatch const& oxm = oxmtlvs.get_match(experimental::OXM_TLV_EXPR_NW_DST);
		m->nw_dst = htobe32(oxm.get_u32value());
		if (oxm.get_oxm_hasmask()) {
			std::bitset<32> mask(oxm.get_u32mask());
			wildcards |= ((32 - mask.count()) << openflow10::OFPFW_NW_DST_SHIFT) & openflow10::OFPFW_NW_DST_MASK;
		}
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_NW_DST_ALL;
	}

	// tp_src
	try {
		m->tp_src = htobe16(oxmtlvs.get_match(experimental::OXM_TLV_EXPR_TP_SRC).get_u16value());
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_TP_SRC;
	}

	// tp_dst
	try {
		m->tp_dst = htobe16(oxmtlvs.get_match(experimental::OXM_TLV_EXPR_TP_DST).get_u16value());
	} catch (eOxmListNotFound& e) {
		wildcards |= openflow10::OFPFW_TP_DST;
	}

	m->wildcards = htobe32(wildcards);

	return match;
}



void
cofmatch::unpack_of10(uint8_t* match, size_t matchlen)
{
	of_version = openflow10::OFP_VERSION;
	oxmtlvs.clear();
	memarea.resize(openflow10::OFP_MATCH_STATIC_LEN);
	ofh10_match = (struct openflow10::ofp_match*)memarea.somem();

	if (matchlen < (unsigned int)openflow10::OFP_MATCH_STATIC_LEN) {
		throw eOFmatchInval();
	}

	struct openflow10::ofp_match *m = (struct openflow10::ofp_match*)match;

	uint32_t wildcards = be32toh(m->wildcards);

	// in_port
	if (!(wildcards & openflow10::OFPFW_IN_PORT)) {
		oxmtlvs.add_match(coxmatch_ofb_in_port(be16toh(m->in_port)));
	}

	// dl_src
	if (!(wildcards & openflow10::OFPFW_DL_SRC)) {
		oxmtlvs.add_match(coxmatch_ofb_eth_src(rofl::cmacaddr(m->dl_src, OFP_ETH_ALEN)));
	}

	// dl_dst
	if (!(wildcards & openflow10::OFPFW_DL_DST)) {
		oxmtlvs.add_match(coxmatch_ofb_eth_dst(rofl::cmacaddr(m->dl_dst, OFP_ETH_ALEN)));
	}

	// dl_vlan
	if (!(wildcards & openflow10::OFPFW_DL_VLAN) && m->dl_vlan != 0xffff) { //0xFFFF value is used to indicate that no VLAN id eas set.
		oxmtlvs.add_match(coxmatch_ofb_vlan_vid(be16toh(m->dl_vlan)));
	}

	// dl_vlan_pcp
	if (!(wildcards & openflow10::OFPFW_DL_VLAN_PCP) && m->dl_vlan != 0xffff) { //0xFFFF value is used to indicate that no VLAN id eas set.
		oxmtlvs.add_match(coxmatch_ofb_vlan_pcp(m->dl_vlan_pcp));
	}

	// dl_type
	if (!(wildcards & openflow10::OFPFW_DL_TYPE)) {
		oxmtlvs.add_match(coxmatch_ofb_eth_type(be16toh(m->dl_type)));
	}

	// nw_tos
	if (!(wildcards & openflow10::OFPFW_NW_TOS)) {
		oxmtlvs.add_match(coxmatch_ofx_nw_tos(m->nw_tos));
	}

	// nw_proto
	if (!(wildcards & openflow10::OFPFW_NW_PROTO)) {
		oxmtlvs.add_match(coxmatch_ofx_nw_proto(m->nw_proto));
	}

	// nw_src
	{
		uint64_t num_of_bits = (wildcards & openflow10::OFPFW_NW_SRC_MASK) >> openflow10::OFPFW_NW_SRC_SHIFT;
		if(num_of_bits > 32)
			num_of_bits = 32;
		uint64_t u_mask = ~((1UL << num_of_bits) - 1UL);
		rofl::caddress addr(AF_INET, "0.0.0.0");
		rofl::caddress mask(AF_INET, "0.0.0.0");
		addr.ca_s4addr->sin_addr.s_addr = m->nw_src;
		mask.ca_s4addr->sin_addr.s_addr = htobe32((uint32_t)u_mask);
		if (num_of_bits < 32) {
			oxmtlvs.add_match(coxmatch_ofx_nw_src(addr, mask));
		}
#ifdef FALSCH
		if (num_of_bits > 0) {
			set_nw_src(addr, mask);
		} else {
			set_nw_src(addr);
		}
#endif
	}

	// nw_dst
	{
		uint64_t num_of_bits = (wildcards & openflow10::OFPFW_NW_DST_MASK) >> openflow10::OFPFW_NW_DST_SHIFT;
		if(num_of_bits > 32)
			num_of_bits = 32;
		uint64_t u_mask = ~((1UL << num_of_bits) - 1UL);
		rofl::caddress addr(AF_INET, "0.0.0.0");
		rofl::caddress mask(AF_INET, "0.0.0.0");
		addr.ca_s4addr->sin_addr.s_addr = m->nw_dst;
		mask.ca_s4addr->sin_addr.s_addr = htobe32((uint32_t)u_mask);
		if (num_of_bits < 32) {
			oxmtlvs.add_match(coxmatch_ofx_nw_dst(addr, mask));
		}
#ifdef FALSCH
		if (num_of_bits > 0) {
			set_nw_dst(addr, mask);
		} else {
			set_nw_dst(addr);
		}
#endif
	}

	// tp_src
	if (!(wildcards & openflow10::OFPFW_TP_SRC)) {
		oxmtlvs.add_match(coxmatch_ofx_tp_src(be16toh(m->tp_src)));
	}

	// tp_dst
	if (!(wildcards & openflow10::OFPFW_TP_DST)) {
		oxmtlvs.add_match(coxmatch_ofx_tp_dst(be16toh(m->tp_dst)));
	}

	validate();
}



uint8_t*
cofmatch::pack_of12(uint8_t* match, size_t matchlen)
{
	if (matchlen < length()) {
		throw eOFmatchInval();
	}

	struct openflow12::ofp_match* m = (struct openflow12::ofp_match*)match;

	/*
	 * fill in real length, i.e. excluding padding
	 */
	ofh12_match->length = htobe16(2 * sizeof(uint16_t) + oxmtlvs.length());

	m->type 	= ofh12_match->type;
	m->length 	= ofh12_match->length;

	oxmtlvs.pack(m->oxm_fields, oxmtlvs.length());

	return match;
}



void
cofmatch::unpack_of12(uint8_t* match, size_t matchlen)
{
	of_version = openflow12::OFP_VERSION;
	oxmtlvs.clear();
	memarea.resize(openflow12::OFP_MATCH_STATIC_LEN);
	ofh12_match = (struct openflow12::ofp_match*)memarea.somem();
	ofh12_match->type 	= htobe16(openflow12::OFPMT_OXM);
	ofh12_match->length = htobe16(length());

	if (matchlen < (sizeof(uint16_t) + sizeof(uint16_t))) {
		throw eOFmatchInval();
	}

	struct openflow12::ofp_match* m = (struct openflow12::ofp_match*)match;

	ofh12_match->type	= (m->type);
	ofh12_match->length	= (m->length);

	if (openflow12::OFPMT_OXM != be16toh(ofh12_match->type)) {
		throw eBadMatchBadType();
	}

	matchlen -= 2 * sizeof(uint16_t);

	if (matchlen > 0) {
		oxmtlvs.unpack(m->oxm_fields, matchlen);
	}

	//check_prerequisites();
}



uint8_t*
cofmatch::pack_of13(uint8_t* match, size_t matchlen)
{
	if (matchlen < length()) {
		throw eOFmatchInval();
	}

	/*
	 * fill in real length, i.e. excluding padding
	 */
	ofh13_match->length = htobe16(2 * sizeof(uint16_t) + oxmtlvs.length());

	struct openflow13::ofp_match* m = (struct openflow13::ofp_match*)match;

	m->type 	= ofh13_match->type;
	m->length 	= ofh13_match->length;

	oxmtlvs.pack(m->oxm_fields, oxmtlvs.length());

	return match;
}



void
cofmatch::unpack_of13(uint8_t* match, size_t matchlen)
{
	of_version = openflow13::OFP_VERSION;
	oxmtlvs.clear();
	memarea.resize(openflow13::OFP_MATCH_STATIC_LEN);
	ofh13_match = (struct openflow13::ofp_match*)memarea.somem();
	ofh13_match->type 	= htobe16(openflow13::OFPMT_OXM);
	ofh13_match->length = htobe16(length());

	if (matchlen < (sizeof(uint16_t) + sizeof(uint16_t))) {
		throw eOFmatchInval();
	}

	struct openflow13::ofp_match* m = (struct openflow13::ofp_match*)match;

	ofh13_match->type	= (m->type);
	ofh13_match->length	= (m->length);

	if (openflow13::OFPMT_OXM != be16toh(ofh13_match->type)) {
		throw eBadMatchBadType();
	}

	matchlen -= 2 * sizeof(uint16_t);

	if (matchlen > 0) {
		oxmtlvs.unpack(m->oxm_fields, matchlen);
	}

	//check_prerequisites();
}



void
cofmatch::check_prerequisites() const
{
	switch (of_version) {
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		/*
		 * these are generic prerequisites as defined in OF 1.3, section 7.2.3.6, page 53 ff.
		 */
		if (oxmtlvs.has_match(OXM_TLV_BASIC_IN_PHY_PORT)) {
			if (not oxmtlvs.has_match(OXM_TLV_BASIC_IN_PORT)) {
				logging::warn << "[rofl][match] rejecting ofp_match: IN-PHY-PORT defined while no IN-PORT is present" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_VLAN_PCP)) {
			if (openflow::OFPVID_NONE == oxmtlvs.get_match(OXM_TLV_BASIC_VLAN_VID).get_u16value()) {
				logging::warn << "[rofl][match] rejecting ofp_match: VLAN-PCP defined while VID is set to OFPVID-NONE" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IP_DSCP)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					((oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x0800) && (oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x86dd))) {
				logging::warn << "[rofl][match] rejecting ofp_match: IP-DSCP defined while ETH-TYPE is not IPv4/IPv6" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IP_ECN)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					((oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x0800) && (oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x86dd))) {
				logging::warn << "[rofl][match] rejecting ofp_match: IP-ECN defined while ETH-TYPE is not IPv4/IPv6" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					((oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x0800) && (oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x86dd))) {
				logging::warn << "[rofl][match] rejecting ofp_match: IP-PROTO defined while ETH-TYPE is not IPv4/IPv6" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IPV4_SRC)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x0800)) {
				logging::warn << "[rofl][match] rejecting ofp_match: IPV4-SRC defined while ETH-TYPE is not IPv4" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IPV4_DST)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x0800)) {
				logging::warn << "[rofl][match] rejecting ofp_match: IPV4-DST defined while ETH-TYPE is not IPv4" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_TCP_SRC)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value() != 6)) {
				logging::warn << "[rofl][match] rejecting ofp_match: TCP-SRC defined while IP-PROTO is not TCP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_TCP_DST)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value() != 6)) {
				logging::warn << "[rofl][match] rejecting ofp_match: TCP-DST defined while IP-PROTO is not TCP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_UDP_SRC)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value() != 17)) {
				logging::warn << "[rofl][match] rejecting ofp_match: UDP-SRC defined while IP-PROTO is not UDP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_UDP_DST)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value() != 17)) {
				logging::warn << "[rofl][match] rejecting ofp_match: UDP-DST defined while IP-PROTO is not UDP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_SCTP_SRC)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value() != 132)) {
				logging::warn << "[rofl][match] rejecting ofp_match: SCTP-SRC defined while IP-PROTO is not SCTP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_SCTP_DST)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value() != 132)) {
				logging::warn << "[rofl][match] rejecting ofp_match: SCTP-DST defined while IP-PROTO is not SCTP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_ICMPV4_TYPE)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value() != 1)) {
				logging::warn << "[rofl][match] rejecting ofp_match: ICMPV4-TYPE defined while IP-PROTO is not ICMPV4" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_ICMPV4_CODE)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value() != 1)) {
				logging::warn << "[rofl][match] rejecting ofp_match: ICMPV4-CODE defined while IP-PROTO is not ICMPV4" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_ARP_OP)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x0806)) {
				logging::warn << "[rofl][match] rejecting ofp_match: ARP-OPCODE defined while ETH-TYPE is not ARP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_ARP_SPA)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x0806)) {
				logging::warn << "[rofl][match] rejecting ofp_match: ARP-SPA defined while ETH-TYPE is not ARP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_ARP_TPA)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x0806)) {
				logging::warn << "[rofl][match] rejecting ofp_match: ARP-TPA defined while ETH-TYPE is not ARP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_ARP_SHA)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x0806)) {
				logging::warn << "[rofl][match] rejecting ofp_match: ARP-SHA defined while ETH-TYPE is not ARP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_ARP_THA)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x0806)) {
				logging::warn << "[rofl][match] rejecting ofp_match: ARP-THA defined while ETH-TYPE is not ARP" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IPV6_SRC)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x86dd)) {
				logging::warn << "[rofl][match] rejecting ofp_match: IPV6-SRC defined while ETH-TYPE is not IPv6" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IPV6_DST)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x86dd)) {
				logging::warn << "[rofl][match] rejecting ofp_match: IPV6-DST defined while ETH-TYPE is not IPv6" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IPV6_FLABEL)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x86dd)) {
				logging::warn << "[rofl][match] rejecting ofp_match: IPV6-FLABEL defined while ETH-TYPE is not IPv6" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_ICMPV6_TYPE)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value() != 58)) {
				logging::warn << "[rofl][match] rejecting ofp_match: ICMPV6-TYPE defined while IP-PROTO is not ICMPV6" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_ICMPV6_CODE)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_IP_PROTO)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value() != 58)) {
				logging::warn << "[rofl][match] rejecting ofp_match: ICMPV6-CODE defined while IP-PROTO is not ICMPV6" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IPV6_ND_TARGET)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ICMPV6_TYPE)) ||
					((oxmtlvs.get_match(OXM_TLV_BASIC_ICMPV6_TYPE).get_u8value() != 135) && (oxmtlvs.get_match(OXM_TLV_BASIC_ICMPV6_TYPE).get_u8value() != 136))) {
				logging::warn << "[rofl][match] rejecting ofp_match: IPv6-ND-TARGET defined while ICMPV6-TYPE is not ND-SOLICITATION or ND-ADVERTISEMENT" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IPV6_ND_SLL)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ICMPV6_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ICMPV6_TYPE).get_u8value() != 135)) {
				logging::warn << "[rofl][match] rejecting ofp_match: IPv6-ND-SLL defined while ICMPV6-TYPE is not ND-SOLICITATION" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IPV6_ND_TLL)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ICMPV6_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ICMPV6_TYPE).get_u8value() != 136)) {
				logging::warn << "[rofl][match] rejecting ofp_match: IPv6-ND-TLL defined while ICMPV6-TYPE is not ND-ADVERTISEMENT" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_MPLS_LABEL)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					((oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x8847) && (oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x8848))) {
				logging::warn << "[rofl][match] rejecting ofp_match: MPLS-LABEL defined while ETH-TYPE is not MPLS/MPLS-UPSTREAM" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_MPLS_TC)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					((oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x8847) && (oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x8848))) {
				logging::warn << "[rofl][match] rejecting ofp_match: MPLS-TC defined while ETH-TYPE is not MPLS/MPLS-UPSTREAM" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_MPLS_BOS)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					((oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x8847) && (oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x8848))) {
				logging::warn << "[rofl][match] rejecting ofp_match: MPLS-BOS defined while ETH-TYPE is not MPLS/MPLS-UPSTREAM" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_PBB_ISID)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x88e7)) {
				logging::warn << "[rofl][match] rejecting ofp_match: PBB-ISID defined while ETH-TYPE is not PBB" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

		if (oxmtlvs.has_match(OXM_TLV_BASIC_IPV6_EXTHDR)) {
			if ((not oxmtlvs.has_match(OXM_TLV_BASIC_ETH_TYPE)) ||
					(oxmtlvs.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value() != 0x86dd)) {
				logging::warn << "[rofl][match] rejecting ofp_match: IPV6-EXTHDR defined while ETH-TYPE is not IPv6" << std::endl << oxmtlvs;
				throw eBadMatchBadPrereq();
			}
		}

	} break;
	default: {
		// do nothing
	};
	}
}

	

bool 
cofmatch::operator== (
	cofmatch& m)
{
	if (of_version != m.of_version) {
		return false;
	}
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return (oxmtlvs == m.oxmtlvs);
	} break;
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		if (ofh12_match->type != m.ofh12_match->type) {
			return false;
		}
		return (oxmtlvs == m.oxmtlvs);
	} break;
	default:
		throw eBadVersion();
	}
	return false;
}


void
cofmatch::set_type(uint16_t type)
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		ofh12_match->type = htobe16(type);
	} break;
	default:
		throw eBadVersion();
	}
}

