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

#include "openflow.h"
#include "../cmemory.h"
#include "../caddress.h"
#include "../cerror.h"
#include "../cmacaddr.h"
#include "../cvastring.h"
#include "../openflow/openflow_rofl_exceptions.h"

#include "rofl/platform/unix/csyslog.h"
#include "coxmlist.h"

#include <rofl/common/openflow/experimental/matches/gtp_matches.h>
#include <rofl/common/openflow/experimental/matches/pppoe_matches.h>

namespace rofl
{


class eOFmatchBase 			: public cerror {}; // error base class cofmatch
class eOFmatchType 			: public eOFmatchBase {};
class eOFmatchInval 		: public eOFmatchBase {};
class eOFmatchNotFound 		: public eOFmatchBase {};
class eOFmatchInvalBadValue	: public eOFmatchInval {};




class cofmatch :
	public csyslog
{
private: // data structures

	uint8_t 		of_version;		// OpenFlow version used for this cofmatch instance
	std::string 	info; 			// info string
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
	

#if 0
	/** less operator
	 *
	 */
	bool
	operator< (cofmatch const& m) const;
#endif

#if 0
	/** dump cofmatch instance
	 */
	const char*
	c_str();
#endif

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
	 */
	void
	set_type(uint16_t type);


	/**
	 *
	 */
	uint32_t
	get_in_port() const;


	/**
	 *
	 */
	void
	set_in_port(
			uint32_t in_port);


	/**
	 *
	 */
	uint32_t
	get_in_phy_port() const;


	/**
	 *
	 */
	void
	set_in_phy_port(
			uint32_t in_phy_port);


	/**
	 *
	 */
	uint64_t
	get_metadata() const;


	/**
	 *
	 */
	void
	set_metadata(uint64_t metadata);


	/**
	 *
	 */
	cmacaddr
	get_eth_dst() const;


	/**
	 *
	 */
	cmacaddr
	get_eth_dst_addr() const;


	/**
	 *
	 */
	cmacaddr
	get_eth_dst_mask() const;


	/**
	 *
	 */
	void
	set_eth_dst(
			cmacaddr const& maddr,
			cmacaddr const& mmask = cmacaddr("ff:ff:ff:ff:ff:ff"));


	/**
	 *
	 */
	cmacaddr
	get_eth_src() const;


	/**
	 *
	 */
	cmacaddr
	get_eth_src_addr() const;


	/**
	 *
	 */
	cmacaddr
	get_eth_src_mask() const;


	/**
	 *
	 */
	void
	set_eth_src(
			cmacaddr const& maddr,
			cmacaddr const& mmask = cmacaddr("ff:ff:ff:ff:ff:ff"));


	/**
	 *
	 */
	uint16_t
	get_eth_type() const;


	/**
	 *
	 */
	void
	set_eth_type(
			uint16_t dl_type);


	/**
	 *
	 */
	uint16_t
	get_vlan_vid() const;


	/**
	 *
	 */
	uint16_t
	get_vlan_vid_value() const;


	/**
	 *
	 */
	uint16_t
	get_vlan_vid_mask() const;


	/**
	 *
	 */
	void
	set_vlan_vid(
			coxmatch_ofb_vlan_vid::vlan_tag_mode_t tag_mode,
			uint16_t vid);


	/**
	 *
	 */
	uint8_t
	get_vlan_pcp() const;


	/**
	 *
	 */
	void
	set_vlan_pcp(
			uint8_t pcp);


	/**
	 *
	 */
	uint32_t
	get_mpls_label() const;


	/**
	 *
	 */
	void
	set_mpls_label(
			uint32_t label);


	/**
	 *
	 */
	uint8_t
	get_mpls_tc() const;


	/**
	 *
	 */
	void
	set_mpls_tc(
			uint8_t tc);

	/**
	 *
	 */
	caddress
	get_nw_src() const;


	/**
	 *
	 */
	caddress
	get_nw_src_value() const;


	/**
	 *
	 */
	caddress
	get_nw_src_mask() const;


	/**
	 *
	 */
	void
	set_nw_src(
			caddress const& src);


	/**
	 *
	 */
	void
	set_nw_src(
			caddress const& src,
			caddress const& mask);


	//OF1.0 only
	/**
	 *
	 */
	/**
	 *
	 */
	uint8_t
	get_nw_proto() const;


	/**
	 *
	 */
	void
	set_nw_proto(
			uint8_t proto);


	caddress
	get_nw_dst() const;


	/**
	 *
	 */
	caddress
	get_nw_dst_value() const;


	/**
	 *
	 */
	caddress
	get_nw_dst_mask() const;


	/**
	 *
	 */
	void
	set_nw_dst(
			caddress const& dst);


	/**
	 *
	 */
	void
	set_nw_dst(
			caddress const& dst,
			caddress const& mask);
	

	//End of OF1.0 only

	/**
	 *
	 */
	caddress
	get_ipv4_src() const;


	/**
	 *
	 */
	caddress
	get_ipv4_src_value() const;


	/**
	 *
	 */
	caddress
	get_ipv4_src_mask() const;


	/**
	 *
	 */
	void
	set_ipv4_src(
			caddress const& src);


	/**
	 *
	 */
	void
	set_ipv4_src(
			caddress const& src,
			caddress const& mask);


	/**
	 *
	 */
	caddress
	get_ipv4_dst() const;


	/**
	 *
	 */
	caddress
	get_ipv4_dst_value() const;


	/**
	 *
	 */
	caddress
	get_ipv4_dst_mask() const;


	/**
	 *
	 */
	void
	set_ipv4_dst(
			caddress const& dst);


	/**
	 *
	 */
	void
	set_ipv4_dst(
			caddress const& dst,
			caddress const& mask);


	/**
	 *
	 */
	uint16_t
	get_arp_opcode() const;


	/**
	 *
	 */
	void
	set_arp_opcode(
			uint16_t opcode);


	/**
	 *
	 */
	cmacaddr
	get_arp_sha() const;


	/**
	 *
	 */
	cmacaddr
	get_arp_sha_addr() const;


	/**
	 *
	 */
	cmacaddr
	get_arp_sha_mask() const;


	/**
	 *
	 */
	void
	set_arp_sha(
			cmacaddr const& sha,
			cmacaddr const& mmask = cmacaddr("ff:ff:ff:ff:ff:ff"));


	/**
	 *
	 */
	cmacaddr
	get_arp_tha() const;


	/**
	 *
	 */
	cmacaddr
	get_arp_tha_addr() const;


	/**
	 *
	 */
	cmacaddr
	get_arp_tha_mask() const;


	/**
	 *
	 */
	void
	set_arp_tha(
			cmacaddr const& tha,
			cmacaddr const& mmask = cmacaddr("ff:ff:ff:ff:ff:ff"));


	/**
	 *
	 */
	caddress
	get_arp_spa() const;


	/**
	 *
	 */
	caddress
	get_arp_spa_value() const;


	/**
	 *
	 */
	caddress
	get_arp_spa_mask() const;


	/**
	 *
	 */
	void
	set_arp_spa(
			caddress const& spa);



	/**
	 *
	 */
	caddress
	get_arp_tpa() const;


	/**
	 *
	 */
	caddress
	get_arp_tpa_value() const;


	/**
	 *
	 */
	caddress
	get_arp_tpa_mask() const;


	/**
	 *
	 */
	void
	set_arp_tpa(
			caddress const& tpa);


	/**
	 *
	 */
	caddress
	get_ipv6_src() const;

	/**
	 * 
	 */
	caddress
	get_ipv6_src_value() const;
	
	/**
	 * 
	 */
	caddress
	get_ipv6_src_mask() const;

	/**
	 *
	 */
	void
	set_ipv6_src(
			caddress const& addr);


	/**
	 *
	 */
	void
	set_ipv6_src(
			caddress const& addr,
			caddress const& mask);


	/**
	 *
	 */
	caddress
	get_ipv6_dst() const;

	/**
	 * 
	 */
	caddress
	get_ipv6_dst_value() const;
	
	/**
	 * 
	 */
	caddress
	get_ipv6_dst_mask() const;

	/**
	 *
	 */
	void
	set_ipv6_dst(
			caddress const& addr);


	/**
	 *
	 */
	void
	set_ipv6_dst(
			caddress const& addr,
			caddress const& mask);


	/**
	 *
	 */
	caddress
	get_ipv6_nd_target() const;


	/**
	 *
	 */
	void
	set_ipv6_nd_target(
			caddress const& addr);



	/**
	 *
	 */
	uint8_t
	get_ip_proto() const;


	/**
	 *
	 */
	void
	set_ip_proto(
			uint8_t proto);


	/**
	 *
	 */
	uint8_t
	get_ip_dscp() const;


	/**
	 *
	 */
	void
	set_ip_dscp(
			uint8_t dscp);


	/**
	 *
	 */
	uint8_t
	get_ip_ecn() const;


	/**
	 *
	 */
	void
	set_ip_ecn(
			uint8_t ecn);



	/**
	 *
	 */
	uint8_t
	get_icmpv4_type() const;



	/**
	 *
	 */
	void
	set_icmpv4_type(
			uint8_t type);


	/**
	 *
	 */
	uint8_t
	get_icmpv4_code() const;



	/**
	 *
	 */
	void
	set_icmpv4_code(
			uint8_t code);



	/**
	 *
	 */
	uint8_t
	get_icmpv6_type() const;



	/**
	 *
	 */
	void
	set_icmpv6_type(
			uint8_t type);


	/**
	 *
	 */
	uint8_t
	get_icmpv6_code() const;



	/**
	 *
	 */
	void
	set_icmpv6_code(
			uint8_t code);


	/**
	 *
	 */
	uint32_t
	get_ipv6_flabel() const;


	/**
	 *
	 */
	void
	set_ipv6_flabel(
			uint32_t flabel);


	/**
	 *
	 */
	void
	set_ipv6_flabel(
			uint32_t flabel,
			uint32_t mask);


	/**
	 *
	 */
	cmacaddr
	get_icmpv6_neighbor_source_lladdr() const;


	/**
	 *
	 */
	void
	set_icmpv6_neighbor_source_lladdr(
			cmacaddr const& maddr);


	/**
	 *
	 */
	cmacaddr
	get_icmpv6_neighbor_target_lladdr() const;


	/**
	 *
	 */
	void
	set_icmpv6_neighbor_target_lladdr(
			cmacaddr const& maddr);


	/**
	 *
	 */
	caddress
	get_icmpv6_neighbor_taddr() const;


	/**
	 *
	 */
	void
	set_icmpv6_neighbor_taddr(
			caddress const& addr);


	//OF1.0 only
	/**
	 *
	 */
	uint16_t
	get_tp_src() const;



	/**
	 *
	 */
	void
	set_tp_src(
			uint16_t src_port);


	/**
	 *
	 */
	uint16_t
	get_tp_dst() const;


	/**
	 *
	 */
	void
	set_tp_dst(
			uint16_t dst_port);

	//End of OF1.0 only



	/**
	 *
	 */
	uint16_t
	get_udp_src() const;



	/**
	 *
	 */
	void
	set_udp_src(
			uint16_t src_port);


	/**
	 *
	 */
	uint16_t
	get_udp_dst() const;


	/**
	 *
	 */
	void
	set_udp_dst(
			uint16_t dst_port);



	/**
	 *
	 */
	uint16_t
	get_tcp_src() const;



	/**
	 *
	 */
	void
	set_tcp_src(
			uint16_t src_port);


	/**
	 *
	 */
	uint16_t
	get_tcp_dst() const;


	/**
	 *
	 */
	void
	set_tcp_dst(
			uint16_t dst_port);


	/**
	 *
	 */
	uint16_t
	get_sctp_src() const;



	/**
	 *
	 */
	void
	set_sctp_src(
			uint16_t src_port);


	/**
	 *
	 */
	uint16_t
	get_sctp_dst() const;


	/**
	 *
	 */
	void
	set_sctp_dst(
			uint16_t dst_port);



#ifndef NDEBUG
	/**
	 *
	 */
	static void
	test();
#endif

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofmatch const& m)
	{
		os << "cofmatch<";
			switch (m.of_version) {
			case openflow10::OFP_VERSION: {
				//ofh10_match->
			} break;
			case openflow12::OFP_VERSION: {
				os << "type: " << m.ofh12_match->type << " ";
				os << "length: " << m.ofh12_match->length << " ";
			} break;
			case openflow13::OFP_VERSION: {
				os << "type: " << m.ofh13_match->type << " ";
				os << "length: " << m.ofh13_match->length << " ";
			} break;
			default: {
				os << "OF version " << m.of_version << " not supported " << std::endl;
			}
			}
			os << "OXMlist: " << m.oxmlist << std::endl;
		os << ">";
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
	//WRITELOG(COFMATCH, DBG, "cofmatch(%p)::cofmatch() [2]", this);

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
