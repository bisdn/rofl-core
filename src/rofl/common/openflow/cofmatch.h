/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFMATCH_H
#define COFMATCH_H 1

#include <string>
#include <endian.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>
#ifndef be32toh
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/cmacaddr.h"
#include "rofl/common/cvastring.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/coxmatches.h"

namespace rofl {
namespace openflow {

class eOFmatchBase 			: public RoflException {};
class eOFmatchType 			: public eOFmatchBase {};
class eOFmatchInval 		: public eOFmatchBase {};
class eOFmatchNotFound 		: public eOFmatchBase {};
class eOFmatchInvalBadValue	: public eOFmatchInval {};


class cofmatch
{
	uint8_t 				of_version;
	coxmatches 				matches;
	uint16_t				type;

public:

	/**
	 *
	 */
	cofmatch(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t type = rofl::openflow::OFPMT_OXM);

	/**
	 *
	 */
	cofmatch(
			const cofmatch &match);

	/**
	 *
	 */
	virtual 
	~cofmatch();

	/**
	 *
	 */
	cofmatch&
	operator= (
			const cofmatch& match);

	/**
	 *
	 */
	bool
	operator== (const cofmatch& m);



public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	/**
	 *
	 */
	void
	check_prerequisites() const;

	/**
	 *
	 */
	void
	clear() { matches.clear(); };

	/**
	 *
	 */
	bool
	contains(
			cofmatch const& match,
			bool strict = false) {
		return matches.contains(match.get_matches(), strict);
	};

	/**
	 *
	 */
	bool
	is_part_of(
			cofmatch const& match,
			uint16_t& exact_hits,
			uint16_t& wildcard_hits,
			uint16_t& missed) {
		return matches.is_part_of(match.get_matches(), exact_hits, wildcard_hits, missed);
	};

public:

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->of_version = ofp_version; };

	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };

	/**
	 *
	 */
	void
	set_type(uint16_t type) { this->type = type; };

	/**
	 *
	 */
	uint16_t
	get_type() const { return type; };

	/**
	 *
	 */
	coxmatches&
	set_matches() { return matches; };

	/**
	 *
	 */
	coxmatches const&
	get_matches() const { return matches; };

private:

	/**
	 *
	 */
	void
	pack_of10(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack_of10(
			uint8_t* buf, size_t buflen);


	/**
	 *
	 */
	void
	pack_of13(
			uint8_t* buf, size_t buflen);


	/**
	 *
	 */
	void
	unpack_of13(
			uint8_t* m,
			size_t mlen);


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmatch const& m) {
		os << rofl::indent(0) << "<cofmatch ofp-version:" << (int)m.get_version() << " >" << std::endl;
			switch (m.of_version) {
			case rofl::openflow12::OFP_VERSION:
			case rofl::openflow13::OFP_VERSION: {
				os << rofl::indent(2) << "<type: " << m.type << " >" << std::endl;
			} break;
			}
		rofl::indent i(2);
		os << m.matches;
		return os;
	};


public:

	/*
  	 * old API: to be or not to be ... deprecated (???)
	 */

	// OF10
	uint8_t  get_nw_proto() const {
	 return matches.get_match(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO).get_u8value();
	};
	uint8_t  get_nw_tos() const {
	 return matches.get_match(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS).get_u8value();
	};
	caddress get_nw_src() const {
	 return matches.get_match(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK).get_u32masked_value_as_addr();
	};
	caddress get_nw_src_value() const {
	 return matches.get_match(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK).get_u32value_as_addr();
	};
	caddress get_nw_src_mask() const {
	 return matches.get_match(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK).get_u32mask_as_addr();
	};
	caddress get_nw_dst() const {
	 return matches.get_match(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK).get_u32masked_value_as_addr();
	};
	caddress get_nw_dst_value() const {
	 return matches.get_match(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK).get_u32value_as_addr();
	};
	caddress get_nw_dst_mask() const {
	 return matches.get_match(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK).get_u32mask_as_addr();
	};
	uint16_t get_tp_src() const {
	 return matches.get_match(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC).get_u16value();
	};
	uint16_t get_tp_dst() const {
	 return matches.get_match(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST).get_u16value();
	};

	// OF12
	uint32_t get_in_port() const {
	 return matches.get_match(OXM_TLV_BASIC_IN_PORT).get_u32value();
	};
	uint32_t get_in_phy_port() const {
	 return matches.get_match(OXM_TLV_BASIC_IN_PHY_PORT).get_u32value();
	};
	uint64_t get_metadata() const {
	 return matches.get_match(OXM_TLV_BASIC_METADATA).get_u64masked_value();
	};
	uint64_t get_metadata_value() const {
	 return matches.get_match(OXM_TLV_BASIC_METADATA).get_u64value();
	};
	uint64_t get_metadata_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_METADATA).get_u64mask();
	};
	cmacaddr get_eth_dst() const {
	 return matches.get_match(OXM_TLV_BASIC_ETH_DST).get_u48masked_value();
	};
	cmacaddr get_eth_dst_addr() const {
	 return matches.get_match(OXM_TLV_BASIC_ETH_DST).get_u48value();
	};
	cmacaddr get_eth_dst_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_ETH_DST).get_u48mask();
	};
	cmacaddr get_eth_src() const {
	 return matches.get_match(OXM_TLV_BASIC_ETH_SRC).get_u48masked_value();
	};
	cmacaddr get_eth_src_addr() const {
	 return matches.get_match(OXM_TLV_BASIC_ETH_SRC).get_u48value();
	};
	cmacaddr get_eth_src_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_ETH_SRC).get_u48mask();
	};
	uint16_t get_eth_type() const {
	 return matches.get_match(OXM_TLV_BASIC_ETH_TYPE).get_u16value();
	};
	uint16_t get_vlan_vid() const {
	 return matches.get_match(OXM_TLV_BASIC_VLAN_VID).get_u16masked_value();
	};
	uint16_t get_vlan_vid_value() const {
	 return matches.get_match(OXM_TLV_BASIC_VLAN_VID).get_u16value();
	};
	uint16_t get_vlan_vid_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_VLAN_VID).get_u16mask();
	};
	uint8_t  get_vlan_pcp() const {
	 return matches.get_match(OXM_TLV_BASIC_VLAN_PCP).get_u8value();
	};
	uint32_t get_mpls_label() const {
	 return matches.get_match(OXM_TLV_BASIC_MPLS_LABEL).get_u32value();
	};
	uint8_t  get_mpls_tc() const {
	 return matches.get_match(OXM_TLV_BASIC_MPLS_TC).get_u8value();
	};
	caddress get_ipv4_src() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV4_SRC).get_u32masked_value_as_addr();
	};
	caddress get_ipv4_src_value() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV4_SRC).get_u32value_as_addr();
	};
	caddress get_ipv4_src_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV4_SRC).get_u32mask_as_addr();
	};
	caddress get_ipv4_dst() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV4_DST).get_u32masked_value_as_addr();
	};
	caddress get_ipv4_dst_value() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV4_DST).get_u32value_as_addr();
	};
	caddress get_ipv4_dst_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV4_DST).get_u32mask_as_addr();
	};
	uint16_t get_arp_opcode() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_OP).get_u16value();
	};
	cmacaddr get_arp_sha() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_SHA).get_u48masked_value();
	};
	cmacaddr get_arp_sha_addr() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_SHA).get_u48value();
	};
	cmacaddr get_arp_sha_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_SHA).get_u48mask();
	};
	cmacaddr get_arp_tha() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_THA).get_u48masked_value();
	};
	cmacaddr get_arp_tha_addr() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_THA).get_u48value();
	};
	cmacaddr get_arp_tha_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_THA).get_u48mask();
	};
	caddress get_arp_spa() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_SPA).get_u32masked_value_as_addr();
	};
	caddress get_arp_spa_value() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_SPA).get_u32value_as_addr();
	};
	caddress get_arp_spa_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_SPA).get_u32mask_as_addr();
	};
	caddress get_arp_tpa() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_TPA).get_u32masked_value_as_addr();
	};
	caddress get_arp_tpa_value() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_TPA).get_u32value_as_addr();
	};
	caddress get_arp_tpa_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_ARP_TPA).get_u32mask_as_addr();
	};
	caddress get_ipv6_src() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_SRC).get_u128masked_value();
	};
	caddress get_ipv6_src_value() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_SRC).get_u128value();
	};
	caddress get_ipv6_src_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_SRC).get_u128mask();
	};
	caddress get_ipv6_dst() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_DST).get_u128masked_value();
	};
	caddress get_ipv6_dst_value() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_DST).get_u128value();
	};
	caddress get_ipv6_dst_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_DST).get_u128mask();
	};
	uint8_t  get_ip_proto() const {
	 return matches.get_match(OXM_TLV_BASIC_IP_PROTO).get_u8value();
	};
	uint8_t  get_ip_dscp() const {
	 return matches.get_match(OXM_TLV_BASIC_IP_DSCP).get_u8value();
	};
	uint8_t  get_ip_ecn() const {
	 return matches.get_match(OXM_TLV_BASIC_IP_ECN).get_u8value();
	};
	uint8_t  get_icmpv4_type() const {
	 return matches.get_match(OXM_TLV_BASIC_ICMPV4_TYPE).get_u8value();
	};
	uint8_t  get_icmpv4_code() const {
	 return matches.get_match(OXM_TLV_BASIC_ICMPV4_CODE).get_u8value();
	};
	uint8_t  get_icmpv6_type() const {
	 return matches.get_match(OXM_TLV_BASIC_ICMPV6_TYPE).get_u8value();
	};
	uint8_t  get_icmpv6_code() const {
	 return matches.get_match(OXM_TLV_BASIC_ICMPV6_CODE).get_u8value();
	};
	uint32_t get_ipv6_flabel() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_FLABEL).get_u32masked_value();
	};
	uint32_t get_ipv6_flabel_value() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_FLABEL).get_u32value();
	};
	uint32_t get_ipv6_flabel_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_FLABEL).get_u32mask();
	};
	cmacaddr get_ipv6_nd_sll() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_ND_SLL).get_u48masked_value();
	};
	cmacaddr get_ipv6_nd_tll() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_ND_TLL).get_u48masked_value();
	};
	caddress get_ipv6_nd_target() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_ND_TARGET).get_u128masked_value();
	};
	uint16_t get_udp_src() const {
	 return matches.get_match(OXM_TLV_BASIC_UDP_SRC).get_u16value();
	};
	uint16_t get_udp_dst() const {
	 return matches.get_match(OXM_TLV_BASIC_UDP_DST).get_u16value();
	};
	uint16_t get_tcp_src() const {
	 return matches.get_match(OXM_TLV_BASIC_TCP_SRC).get_u16value();
	};
	uint16_t get_tcp_dst() const {
	 return matches.get_match(OXM_TLV_BASIC_TCP_DST).get_u16value();
	};
	uint16_t get_sctp_src() const {
	 return matches.get_match(OXM_TLV_BASIC_SCTP_SRC).get_u16value();
	};
	uint16_t get_sctp_dst() const {
	 return matches.get_match(OXM_TLV_BASIC_SCTP_DST).get_u16value();
	};

	// OF13
	bool     get_mpls_bos() const {
	 return matches.get_match(OXM_TLV_BASIC_MPLS_BOS).get_u8value();
	};
	uint64_t get_tunnel_id() const {
	 return matches.get_match(OXM_TLV_BASIC_TUNNEL_ID).get_u64masked_value();
	};
	uint64_t get_tunnel_id_value() const {
	 return matches.get_match(OXM_TLV_BASIC_TUNNEL_ID).get_u64value();
	};
	uint64_t get_tunnel_id_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_TUNNEL_ID).get_u64mask();
	};
	uint32_t get_pbb_isid() const {
	 return matches.get_match(OXM_TLV_BASIC_PBB_ISID).get_u32masked_value();
	};
	uint32_t get_pbb_isid_value() const {
	 return matches.get_match(OXM_TLV_BASIC_PBB_ISID).get_u32value();
	};
	uint32_t get_pbb_isid_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_PBB_ISID).get_u32mask();
	};
	uint16_t get_ipv6_exthdr() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_EXTHDR).get_u16masked_value();
	};
	uint16_t get_ipv6_exthdr_value() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_EXTHDR).get_u16value();
	};
	uint16_t get_ipv6_exthdr_mask() const {
	 return matches.get_match(OXM_TLV_BASIC_IPV6_EXTHDR).get_u16mask();
	};

     // OF10
	void set_nw_proto(uint8_t proto) {
		matches.add_match(coxmatch_ofx_nw_proto(proto));
	};
	void set_nw_tos(uint8_t tos) {
		matches.add_match(coxmatch_ofx_nw_tos(tos));
	};
	void set_nw_src(caddress const& src) {
		matches.add_match(coxmatch_ofx_nw_src(src));
	};
	void set_nw_src(caddress const& src, caddress const& mask) {
		matches.add_match(coxmatch_ofx_nw_src(src, mask));
	};
	void set_nw_dst(caddress const& dst) {
		matches.add_match(coxmatch_ofx_nw_dst(dst));
	};
	void set_nw_dst(caddress const& dst, caddress const& mask) {
		matches.add_match(coxmatch_ofx_nw_dst(dst, mask));
	};
	void set_tp_src(uint16_t src_port) {
		matches.add_match(coxmatch_ofx_tp_src(src_port));
	};
	void set_tp_dst(uint16_t dst_port) {
		matches.add_match(coxmatch_ofx_tp_dst(dst_port));
	};

	// OF12
	void set_in_port(uint32_t in_port) {
		matches.add_match(coxmatch_ofb_in_port(in_port));
	};
	void set_in_phy_port(uint32_t in_phy_port) {
		matches.add_match(coxmatch_ofb_in_phy_port(in_phy_port));
	};
	void set_metadata(uint64_t metadata) {
		matches.add_match(coxmatch_ofb_metadata(metadata));
	};
	void set_metadata(uint64_t metadata, uint64_t mask) {
		matches.add_match(coxmatch_ofb_metadata(metadata, mask));
	};
	void set_eth_dst(cmacaddr const& maddr) {
		matches.add_match(coxmatch_ofb_eth_dst(maddr));
	};
	void set_eth_dst(cmacaddr const& maddr, cmacaddr const& mmask) {
		matches.add_match(coxmatch_ofb_eth_dst(maddr, mmask));
	};
	void set_eth_src(cmacaddr const& maddr) {
		matches.add_match(coxmatch_ofb_eth_src(maddr));
	};
	void set_eth_src(cmacaddr const& maddr, cmacaddr const& mmask) {
		matches.add_match(coxmatch_ofb_eth_src(maddr, mmask));
	};
	void set_eth_type( uint16_t dl_type) {
		matches.add_match(coxmatch_ofb_eth_type(dl_type));
	};
	void set_vlan_vid(uint16_t vid) {
		matches.add_match(coxmatch_ofb_vlan_vid(vid));
	};
	void set_vlan_vid(uint16_t vid, uint16_t mask) {
		matches.add_match(coxmatch_ofb_vlan_vid(vid, mask));
	};
	void set_vlan_present() {
		matches.add_match(coxmatch_ofb_vlan_present());
	};
	void set_vlan_untagged() {
		matches.add_match(coxmatch_ofb_vlan_untagged());
	};
	void set_vlan_pcp(uint8_t pcp) {
		matches.add_match(coxmatch_ofb_vlan_pcp(pcp));
	};
	void set_ip_dscp(uint8_t dscp) {
		matches.add_match(coxmatch_ofb_ip_dscp(dscp));
	};
	void set_ip_ecn(uint8_t ecn) {
		matches.add_match(coxmatch_ofb_ip_ecn(ecn));
	};
	void set_ip_proto(uint8_t proto) {
		matches.add_match(coxmatch_ofb_ip_proto(proto));
	};
	void set_ipv4_src(caddress const& src) {
		matches.add_match(coxmatch_ofb_ipv4_src(src));
	};
	void set_ipv4_src(caddress const& src, caddress const& mask) {
		matches.add_match(coxmatch_ofb_ipv4_src(src, mask));
	};
	void set_ipv4_dst(caddress const& dst) {
		matches.add_match(coxmatch_ofb_ipv4_dst(dst));
	};
	void set_ipv4_dst(caddress const& dst, caddress const& mask) {
		matches.add_match(coxmatch_ofb_ipv4_dst(dst, mask));
	};
	void set_tcp_src(uint16_t src_port) {
		matches.add_match(coxmatch_ofb_tcp_src(src_port));
	};
	void set_tcp_dst(uint16_t dst_port) {
		matches.add_match(coxmatch_ofb_tcp_dst(dst_port));
	};
	void set_udp_src(uint16_t src_port) {
		matches.add_match(coxmatch_ofb_udp_src(src_port));
	};
	void set_udp_dst(uint16_t dst_port) {
		matches.add_match(coxmatch_ofb_udp_dst(dst_port));
	};
	void set_sctp_src(uint16_t src_port) {
		matches.add_match(coxmatch_ofb_sctp_src(src_port));
	};
	void set_sctp_dst(uint16_t dst_port) {
		matches.add_match(coxmatch_ofb_sctp_dst(dst_port));
	};
	void set_icmpv4_type(uint8_t type) {
		matches.add_match(coxmatch_ofb_icmpv4_type(type));
	};
	void set_icmpv4_code(uint8_t code) {
		matches.add_match(coxmatch_ofb_icmpv4_code(code));
	};
	void set_arp_opcode(uint16_t opcode) {
		matches.add_match(coxmatch_ofb_arp_opcode(opcode));
	};
	void set_arp_spa(caddress const& spa) {
		matches.add_match(coxmatch_ofb_arp_spa(spa));
	};
	void set_arp_spa(caddress const& spa, caddress const& mask) {
		matches.add_match(coxmatch_ofb_arp_spa(spa, mask));
	};
	void set_arp_tpa(caddress const& tpa) {
		matches.add_match(coxmatch_ofb_arp_tpa(tpa));
	};
	void set_arp_tpa(caddress const& tpa, caddress const& mask) {
		matches.add_match(coxmatch_ofb_arp_tpa(tpa, mask));
	};
	void set_arp_sha(cmacaddr const& sha) {
		matches.add_match(coxmatch_ofb_arp_sha(sha));
	};
	void set_arp_sha(cmacaddr const& sha, cmacaddr const& mmask) {
		matches.add_match(coxmatch_ofb_arp_sha(sha, mmask));
	};
	void set_arp_tha(cmacaddr const& tha) {
		matches.add_match(coxmatch_ofb_arp_tha(tha));
	};
	void set_arp_tha(cmacaddr const& tha, cmacaddr const& mmask) {
		matches.add_match(coxmatch_ofb_arp_tha(tha, mmask));
	};
	void set_ipv6_src(caddress const& addr) {
		matches.add_match(coxmatch_ofb_ipv6_src(addr));
	};
	void set_ipv6_src(caddress const& addr, caddress const& mask) {
		matches.add_match(coxmatch_ofb_ipv6_src(addr, mask));
	};
	void set_ipv6_dst(caddress const& addr) {
		matches.add_match(coxmatch_ofb_ipv6_dst(addr));
	};
	void set_ipv6_dst(caddress const& addr, caddress const& mask) {
		matches.add_match(coxmatch_ofb_ipv6_dst(addr, mask));
	};
	void set_ipv6_flabel(uint32_t flabel) {
		matches.add_match(coxmatch_ofb_ipv6_flabel(flabel));
	};
	void set_icmpv6_type(uint8_t type) {
		matches.add_match(coxmatch_ofb_icmpv6_type(type));
	};
	void set_icmpv6_code(uint8_t code) {
		matches.add_match(coxmatch_ofb_icmpv6_code(code));
	};
	void set_ipv6_nd_target(caddress const& addr) {
		matches.add_match(coxmatch_ofb_ipv6_nd_target(addr));
	};
	void set_ipv6_nd_sll(cmacaddr const& maddr) {
		matches.add_match(coxmatch_ofb_ipv6_nd_sll(maddr));
	};
	void set_ipv6_nd_tll(cmacaddr const& maddr) {
		matches.add_match(coxmatch_ofb_ipv6_nd_tll(maddr));
	};
	void set_mpls_label(uint32_t label) {
		matches.add_match(coxmatch_ofb_mpls_label(label));
	};
	void set_mpls_tc(uint8_t tc) {
		matches.add_match(coxmatch_ofb_mpls_tc(tc));
	};

	// OF13
	void set_mpls_bos(bool bos) {
		matches.add_match(coxmatch_ofb_mpls_bos(bos));
	};
	void set_tunnel_id(uint64_t tunnel_id) {
		matches.add_match(coxmatch_ofb_tunnel_id(tunnel_id));
	};
	void set_tunnel_id(uint64_t tunnel_id, uint64_t mask) {
		matches.add_match(coxmatch_ofb_tunnel_id(tunnel_id, mask));
	};
	void set_pbb_isid(uint32_t pbb_isid) {
		matches.add_match(coxmatch_ofb_pbb_isid(pbb_isid));
	};
	void set_pbb_isid(uint32_t pbb_isid, uint32_t mask) {
		matches.add_match(coxmatch_ofb_pbb_isid(pbb_isid, mask));
	};
	void set_ipv6_exthdr(uint16_t ipv6_exthdr) {
		matches.add_match(coxmatch_ofb_ipv6_exthdr(ipv6_exthdr));
	};
	void set_ipv6_exthdr(uint16_t ipv6_exthdr, uint16_t mask) {
		matches.add_match(coxmatch_ofb_ipv6_exthdr(ipv6_exthdr, mask));
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
