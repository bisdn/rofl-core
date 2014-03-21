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

#include "rofl/common/openflow/coxmlist.h"

#include "rofl/common/openflow/experimental/matches/gtp_matches.h"
#include "rofl/common/openflow/experimental/matches/pppoe_matches.h"

namespace rofl
{


class eOFmatchBase 			: public RoflException {}; // error base class cofmatch
class eOFmatchType 			: public eOFmatchBase {};
class eOFmatchInval 		: public eOFmatchBase {};
class eOFmatchNotFound 		: public eOFmatchBase {};
class eOFmatchInvalBadValue	: public eOFmatchInval {};




class cofmatch
{
private: // data structures

	uint8_t 		of_version;		// OpenFlow version used for this cofmatch instance
	coxmlist 		oxmlist;		// list of all oxms
	cmemory 		memarea;

#define OFP10_MATCH_STATIC_LEN		(sizeof(struct openflow10::ofp_match))
#define OFP12_MATCH_STATIC_LEN  	(2*sizeof(uint16_t))
#define OFP13_MATCH_STATIC_LEN  	(2*sizeof(uint16_t))

public: // data structures

	union {
		uint8_t*							ofpu_match;
		struct openflow10::ofp_match*		ofpu10_match;
		struct openflow12::ofp_match*		ofpu12_match;
		struct openflow13::ofp_match*		ofpu13_match;
	} ofpu;

#define ofh_match  	ofpu.ofpu_match
#define ofh10_match ofpu.ofpu10_match
#define ofh12_match ofpu.ofpu12_match
#define ofh13_match ofpu.ofpu13_match



public: // methods

	/** constructor
	 *
	 */
	cofmatch(
			uint8_t of_version = openflow12::OFP_VERSION,
			uint16_t type = openflow::OFPMT_OXM);


	/** constructor
	 *
	 */
	template<class T>
	cofmatch(
			uint8_t of_version,
			T* match,
			size_t matchlen);


	/** copy constructor
	 *
	 */
	cofmatch(const cofmatch &m) 
	{
		*this = m;
	};


	/** destructor
	 *
	 */
	virtual 
	~cofmatch();


	/** assignment operator
	 */
	cofmatch&
	operator= (const cofmatch& m);

	/**
	 *
	 */
	bool
	operator== (const cofmatch& m);


#if 0
	/** less operator
	 *
	 */
	bool
	operator< (cofmatch const& m) const;
#endif

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) {
		this->of_version = ofp_version;

		switch (of_version) {
		case openflow::OFP_VERSION_UNKNOWN: {
			memarea.resize(openflow13::OFP_MATCH_STATIC_LEN);
			ofh_match = memarea.somem();
		} break;
		case openflow10::OFP_VERSION: {
			memarea.resize(openflow10::OFP_MATCH_STATIC_LEN);
			ofh10_match = (struct openflow10::ofp_match*)memarea.somem();
		} break;
		case openflow12::OFP_VERSION: {
			memarea.resize(openflow12::OFP_MATCH_STATIC_LEN);
			ofh12_match = (struct openflow12::ofp_match*)memarea.somem();
			ofh12_match->type 	= htobe16(rofl::openflow::OFPMT_OXM);
			ofh12_match->length = htobe16(length());
		} break;
		case openflow13::OFP_VERSION: {
			memarea.resize(openflow13::OFP_MATCH_STATIC_LEN);
			ofh13_match = (struct openflow13::ofp_match*)memarea.somem();
			ofh13_match->type 	= htobe16(rofl::openflow::OFPMT_OXM);
			ofh13_match->length = htobe16(length());
		} break;
		default: {
		};
		}
	};

	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };

	/** validate match structure
	 *
	 */
	void
	validate() throw (eOFmatchInval);


	/** return required length for packed cofmatch (includes padding to 64bit)
	 */
	size_t
	length() const;


private:


	size_t
	length_internal();


public:

	/*
	 * TODO: introduce a template
	 */

	uint8_t*
	pack(uint8_t* m, size_t mlen);

	void
	unpack(uint8_t* m, size_t mlen);

	/** copy internal struct ofp_match into specified ofp_match ptr 'm'
	 * @return pointer 'm'
	 *
	 */
	uint8_t*
	pack_of10(
			uint8_t* m,
			size_t mlen);


	/** copy ofp_match structure pointed to by 'm' into internal struct ofp_match
	 *
	 */
	void
	unpack_of10(
			uint8_t* m,
			size_t mlen);


	/** copy internal struct ofp_match into specified ofp_match ptr 'm'
	 * @return pointer 'm'
	 *
	 */
	uint8_t*
	pack_of12(
			uint8_t* m,
			size_t mlen);


	/** copy ofp_match structure pointed to by 'm' into internal struct ofp_match
	 *
	 */
	void
	unpack_of12(
			uint8_t* m,
			size_t mlen);


	/** copy internal struct ofp_match into specified ofp_match ptr 'm'
	 * @return pointer 'm'
	 *
	 */
	uint8_t*
	pack_of13(
			uint8_t* m,
			size_t mlen);


	/** copy ofp_match structure pointed to by 'm' into internal struct ofp_match
	 *
	 */
	void
	unpack_of13(
			uint8_t* m,
			size_t mlen);



	/**
	 * @brief	checks prerequisites for OF1.2 and beyond OXM TLV lists
	 */
	void
	check_prerequisites() const;


	
	/** check for an identical match between two ofp_match structures
	 */
	bool 
	operator== (
		cofmatch& m);


	/** reset structure
	 *
	 */
	void
	clear();


	/**
	 *
	 */
	bool
	contains(
			cofmatch const& ofm,
			bool strict = false);



	/**
	 *
	 */
	bool
	is_part_of(
			cofmatch const& ofm,
			uint16_t& exact_hits,
			uint16_t& wildcard_hits,
			uint16_t& missed);


	/**
	 *
	 */
	void
	insert(
			coxmatch const& oxm);



	/**
	 *
	 */
	void
	remove(
			uint16_t oxm_class,
			uint8_t oxm_field);



	/**
	 *
	 */
	coxmatch&
	get_match(
			uint16_t ofm_class, uint8_t ofm_field);




	/**
	 *
	 */
	coxmatch const&
	get_const_match(
			uint16_t ofm_class, uint8_t ofm_field) const;


public:

	/**
	 *
	 * @param type	type of struct ofp_match
	 */
	void
	set_type(
			uint16_t type);


public:

	/*
	 * old API
	 */

	/*
	 * PLEASE NOTE: IPV6-FLOWLABEL uses a mask in OF12, and none in OF13
	 * we follow the OF13 convention and do not use a masked flow-label for IPv6
	 */


	// OF10
	caddress get_nw_src() const;
	caddress get_nw_src_value() const;
	caddress get_nw_src_mask() const;
	uint8_t  get_nw_proto() const;
	uint8_t  get_nw_tos() const;
	caddress get_nw_dst() const;
	caddress get_nw_dst_value() const;
	caddress get_nw_dst_mask() const;
	uint16_t get_tp_src() const;
	uint16_t get_tp_dst() const;

	// OF12
	uint32_t get_in_port() const;
	uint32_t get_in_phy_port() const;
	uint64_t get_metadata() const;
	uint64_t get_metadata_value() const;
	uint64_t get_metadata_mask() const;
	cmacaddr get_eth_dst() const;
	cmacaddr get_eth_dst_addr() const;
	cmacaddr get_eth_dst_mask() const;
	cmacaddr get_eth_src() const;
	cmacaddr get_eth_src_addr() const;
	cmacaddr get_eth_src_mask() const;
	uint16_t get_eth_type() const;
	uint16_t get_vlan_vid() const;
	uint16_t get_vlan_vid_value() const;
	uint16_t get_vlan_vid_mask() const;
	uint8_t  get_vlan_pcp() const;
	uint32_t get_mpls_label() const;
	uint8_t  get_mpls_tc() const;
	caddress get_ipv4_src() const;
	caddress get_ipv4_src_value() const;
	caddress get_ipv4_src_mask() const;
	caddress get_ipv4_dst() const;
	caddress get_ipv4_dst_value() const;
	caddress get_ipv4_dst_mask() const;
	uint16_t get_arp_opcode() const;
	cmacaddr get_arp_sha() const;
	cmacaddr get_arp_sha_addr() const;
	cmacaddr get_arp_sha_mask() const;
	cmacaddr get_arp_tha() const;
	cmacaddr get_arp_tha_addr() const;
	cmacaddr get_arp_tha_mask() const;
	caddress get_arp_spa() const;
	caddress get_arp_spa_value() const;
	caddress get_arp_spa_mask() const;
	caddress get_arp_tpa() const;
	caddress get_arp_tpa_value() const;
	caddress get_arp_tpa_mask() const;
	caddress get_ipv6_src() const;
	caddress get_ipv6_src_value() const;
	caddress get_ipv6_src_mask() const;
	caddress get_ipv6_dst() const;
	caddress get_ipv6_dst_value() const;
	caddress get_ipv6_dst_mask() const;
	uint8_t  get_ip_proto() const;
	uint8_t  get_ip_dscp() const;
	uint8_t  get_ip_ecn() const;
	uint8_t  get_icmpv4_type() const;
	uint8_t  get_icmpv4_code() const;
	uint8_t  get_icmpv6_type() const;
	uint8_t  get_icmpv6_code() const;
	uint32_t get_ipv6_flabel() const;
	cmacaddr get_ipv6_nd_sll() const;
	cmacaddr get_ipv6_nd_tll() const;
	caddress get_ipv6_nd_target() const;
	uint16_t get_udp_src() const;
	uint16_t get_udp_dst() const;
	uint16_t get_tcp_src() const;
	uint16_t get_tcp_dst() const;
	uint16_t get_sctp_src() const;
	uint16_t get_sctp_dst() const;

	// OF13
	bool     get_mpls_bos() const;
	uint64_t get_tunnel_id() const;
	uint64_t get_tunnel_id_value() const;
	uint64_t get_tunnel_id_mask() const;
	uint32_t get_pbb_isid() const;
	uint32_t get_pbb_isid_value() const;
	uint32_t get_pbb_isid_mask() const;
	uint16_t get_ipv6_exthdr() const;
	uint16_t get_ipv6_exthdr_value() const;
	uint16_t get_ipv6_exthdr_mask() const;


	// OF10
	void set_nw_src(caddress const& src, caddress const& mask = caddress(AF_INET, "255.255.255.255"));
	void set_nw_proto(uint8_t proto);
	void set_nw_tos(uint8_t tos);
	void set_nw_dst(caddress const& dst, caddress const& mask = caddress(AF_INET, "255.255.255.255"));
	void set_tp_src(uint16_t src_port);
	void set_tp_dst(uint16_t dst_port);

	// OF12
	void set_in_port(uint32_t in_port);
	void set_in_phy_port(uint32_t in_phy_port);
	void set_metadata(uint64_t metadata, uint64_t mask = 0xffffffffffffffff);
	void set_eth_dst(cmacaddr const& maddr, cmacaddr const& mmask = cmacaddr("ff:ff:ff:ff:ff:ff"));
	void set_eth_src(cmacaddr const& maddr, cmacaddr const& mmask = cmacaddr("ff:ff:ff:ff:ff:ff"));
	void set_eth_type( uint16_t dl_type);
	void set_vlan_vid(uint16_t vid, uint16_t mask = 0xffff);
	void set_vlan_present();
	void set_vlan_untagged();
	void set_vlan_pcp(uint8_t pcp);
	void set_ip_dscp(uint8_t dscp);
	void set_ip_ecn(uint8_t ecn);
	void set_ip_proto(uint8_t proto);
	void set_ipv4_src(caddress const& src, caddress const& mask = caddress(AF_INET, "255.255.255.255"));
	void set_ipv4_dst(caddress const& dst, caddress const& mask = caddress(AF_INET, "255.255.255.255"));
	void set_tcp_src(uint16_t src_port);
	void set_tcp_dst(uint16_t dst_port);
	void set_udp_src(uint16_t src_port);
	void set_udp_dst(uint16_t dst_port);
	void set_sctp_src(uint16_t src_port);
	void set_sctp_dst(uint16_t dst_port);
	void set_icmpv4_type(uint8_t type);
	void set_icmpv4_code(uint8_t code);
	void set_arp_opcode(uint16_t opcode);
	void set_arp_spa(caddress const& spa, caddress const& mask = caddress(AF_INET, "255.255.255.255"));
	void set_arp_tpa(caddress const& tpa, caddress const& mask = caddress(AF_INET, "255.255.255.255"));
	void set_arp_sha(cmacaddr const& sha, cmacaddr const& mmask = cmacaddr("ff:ff:ff:ff:ff:ff"));
	void set_arp_tha(cmacaddr const& tha, cmacaddr const& mmask = cmacaddr("ff:ff:ff:ff:ff:ff"));
	void set_ipv6_src(caddress const& addr, caddress const& mask = caddress(AF_INET, "255.255.255.255"));
	void set_ipv6_dst(caddress const& addr, caddress const& mask = caddress(AF_INET, "255.255.255.255"));
	void set_ipv6_flabel(uint32_t flabel);
	void set_icmpv6_type(uint8_t type);
	void set_icmpv6_code(uint8_t code);
	void set_ipv6_nd_target(caddress const& addr);
	void set_ipv6_nd_sll(cmacaddr const& maddr);
	void set_ipv6_nd_tll(cmacaddr const& maddr);
	void set_mpls_label(uint32_t label);
	void set_mpls_tc(uint8_t tc);

	// OF13
	void set_mpls_bos(bool bos);
	void set_tunnel_id(uint64_t tunnel_id, uint64_t mask = 0xffffffffffffffff);
	void set_pbb_isid(uint32_t pbb_isid, uint32_t mask = 0xffffffff);
	void set_ipv6_exthdr(uint16_t ipv6_exthdr, uint16_t mask = 0xffff);


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmatch const& m) {
		os << indent(0) << "<cofmatch ";
			switch (m.of_version) {
			case openflow10::OFP_VERSION: {
				os << "OF1.0 ";
			} break;
			case openflow12::OFP_VERSION: {
				os << "OF1.2 type: " << be16toh(m.ofh12_match->type) << " ";
				os << "length: " << be16toh(m.ofh12_match->length) << " ";
			} break;
			case openflow13::OFP_VERSION: {
				os << "OF1.3 type: " << be16toh(m.ofh13_match->type) << " ";
				os << "length: " << be16toh(m.ofh13_match->length) << " ";
			} break;
			default: {
				os << "OF version " << m.of_version << " not supported";
			}
			}
		os << ">" << std::endl;
		indent i(2);
		os << m.oxmlist;
		return os;
	};
};


template<class T>
cofmatch::cofmatch(
		uint8_t of_version,
		T* match,
		size_t matchlen) :
			of_version(of_version)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		if (OFP10_MATCH_STATIC_LEN != matchlen) {
			throw eBadVersion();
		}
		unpack(match, matchlen);
	} break;
	case openflow12::OFP_VERSION: {
		if (OFP12_MATCH_STATIC_LEN != matchlen) {
			throw eBadVersion();
		}
		unpack(match, matchlen);
	} break;
	case openflow13::OFP_VERSION: {
		if (OFP13_MATCH_STATIC_LEN != matchlen) {
			throw eBadVersion();
		}
		unpack(match, matchlen);
	} break;
	default:
		throw eBadVersion();
	}

	validate();
}

}; // end of namespace

#endif
