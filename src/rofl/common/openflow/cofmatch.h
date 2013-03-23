/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFMATCH_H
#define COFMATCH_H 1

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <endian.h>
#include <stdio.h>
#include <string.h>
#include "openflow.h"
#include <endian.h>
#ifndef be32toh
#include "../endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "../cmemory.h"
#include "../caddress.h"
#include "../cerror.h"
#include "../cmacaddr.h"
#include "../cvastring.h"
#include "../openflow/openflow_rofl_exceptions.h"

#include "rofl/platform/unix/csyslog.h"
#include "coxmlist.h"

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

#define OFP10_MATCH_STATIC_LEN	(sizeof(struct ofp10_match))
#define OFP12_MATCH_STATIC_LEN  (2*sizeof(uint16_t))
#define OFP13_MATCH_STATIC_LEN  (2*sizeof(uint16_t))

public: // data structures

	union {
		uint8_t*				ofpu_match;
		struct ofp10_match*		ofp10u_match;
		struct ofp12_match*		ofp12u_match;
		struct ofp13_match*		ofp13u_match;
	} ofpu;

#define ofh_match  	ofpu.ofpu_match
#define ofh10_match ofpu.ofp10u_match
#define ofh12_match ofpu.ofp12u_match
#define ofh13_match ofpu.ofp13u_match



public: // methods

	/** constructor
	 *
	 */
	cofmatch(
			uint8_t of_version = OFP12_VERSION,
			uint16_t type = OFPMT_OXM);


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
	

	/** less operator
	 *
	 */
	bool
	operator< (cofmatch const& m) const;


	/** dump cofmatch instance
	 */
	const char*
	c_str();


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


	/** copy internal struct ofp_match into specified ofp_match ptr 'm'
	 * @return pointer 'm'
	 *
	 */
	struct ofp10_match*
	pack(
			struct ofp10_match* m,
			size_t mlen) const throw (eOFmatchInval);


	/** copy ofp_match structure pointed to by 'm' into internal struct ofp_match
	 *
	 */
	void
	unpack(
			struct ofp10_match* m,
			size_t mlen) throw (eOFmatchInval);


	/** copy internal struct ofp_match into specified ofp_match ptr 'm'
	 * @return pointer 'm'
	 *
	 */
	struct ofp12_match*
	pack(
			struct ofp12_match* m,
			size_t mlen) const throw (eOFmatchInval);


	/** copy ofp_match structure pointed to by 'm' into internal struct ofp_match
	 *
	 */
	void
	unpack(
			struct ofp12_match* m,
			size_t mlen) throw (eOFmatchInval);


	/** copy internal struct ofp_match into specified ofp_match ptr 'm'
	 * @return pointer 'm'
	 *
	 */
	struct ofp13_match*
	pack(
			struct ofp13_match* m,
			size_t mlen) const throw (eOFmatchInval);


	/** copy ofp_match structure pointed to by 'm' into internal struct ofp_match
	 *
	 */
	void
	unpack(
			struct ofp13_match* m,
			size_t mlen) throw (eOFmatchInval);


	/** check for an overlap between two ofp_match structures 
	 * (strict and non-strict)
	 */
	bool 
	overlaps(
		cofmatch const& m,
		bool strict = false);

	
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
	is_matching(
			cofmatch& other,
			uint16_t& exact_hits,
			uint16_t& wildcard_hits,
			uint16_t& missed);


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
	void
	set_type(uint16_t type);


	/**
	 *
	 */
	uint32_t
	get_in_port() const
		throw (eOFmatchNotFound);


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
	get_in_phy_port() const
		throw (eOFmatchNotFound);


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
	get_metadata() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	void
	set_metadata(uint64_t metadata);


	/**
	 *
	 */
	cmacaddr
	get_eth_dst() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	cmacaddr
	get_eth_dst_addr() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	cmacaddr
	get_eth_dst_mask() const
		throw (eOFmatchNotFound);


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
	get_eth_src() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	cmacaddr
	get_eth_src_addr() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	cmacaddr
	get_eth_src_mask() const
		throw (eOFmatchNotFound);


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
	get_eth_type() const
		throw (eOFmatchNotFound);


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
	get_vlan_vid() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	uint16_t
	get_vlan_vid_value() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	uint16_t
	get_vlan_vid_mask() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	void
	set_vlan_vid(
			uint16_t vid);


	/**
	 *
	 */
	uint8_t
	get_vlan_pcp() const
		throw (eOFmatchNotFound);


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
	get_mpls_label() const
		throw (eOFmatchNotFound);


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
	get_mpls_tc() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	void
	set_mpls_tc(
			uint8_t tc);


	/**
	 *
	 */
	uint8_t
	get_pppoe_type() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	void
	set_pppoe_type(
			uint8_t type);


	/**
	 *
	 */
	uint8_t
	get_pppoe_code() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	void
	set_pppoe_code(
			uint8_t code);


	/**
	 *
	 */
	uint16_t
	get_pppoe_sessid() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	void
	set_pppoe_sessid(
			uint16_t sid);


	/**
	 *
	 */
	uint16_t
	get_ppp_prot() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	void
	set_ppp_prot(
			uint16_t prot);


	/**
	 *
	 */
	caddress
	get_ipv4_src() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	caddress
	get_ipv4_src_value() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	caddress
	get_ipv4_src_mask() const
		throw (eOFmatchNotFound);


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
	get_ipv4_dst() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	caddress
	get_ipv4_dst_value() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	caddress
	get_ipv4_dst_mask() const
		throw (eOFmatchNotFound);


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
	get_arp_opcode() const
		throw (eOFmatchNotFound);


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
	get_arp_sha() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	void
	set_arp_sha(
			cmacaddr const& sha);


	/**
	 *
	 */
	cmacaddr
	get_arp_tha() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	void
	set_arp_tha(
			cmacaddr const& tha);


	/**
	 *
	 */
	caddress
	get_arp_spa() const
		throw (eOFmatchNotFound);


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
	get_arp_tpa() const
		throw (eOFmatchNotFound);


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
	get_ipv6_src() const
		throw (eOFmatchNotFound);


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
	get_ipv6_dst() const
		throw (eOFmatchNotFound);


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
	get_ipv6_nd_target() const
		throw (eOFmatchNotFound);


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
	get_ip_proto() const
		throw (eOFmatchNotFound);


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
	get_ip_dscp() const
		throw (eOFmatchNotFound);


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
	get_ip_ecn() const
		throw (eOFmatchNotFound);


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
	get_icmpv4_type() const
		throw (eOFmatchNotFound);



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
	get_icmpv4_code() const
		throw (eOFmatchNotFound);



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
	get_icmpv6_type() const
		throw (eOFmatchNotFound);



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
	get_icmpv6_code() const
		throw (eOFmatchNotFound);



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
	get_ipv6_flabel() const
		throw (eOFmatchNotFound);


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
	get_icmpv6_neighbor_source_lladdr() const
		throw (eOFmatchNotFound);


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
	get_icmpv6_neighbor_target_lladdr() const
		throw (eOFmatchNotFound);


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
	get_icmpv6_neighbor_taddr() const
		throw (eOFmatchNotFound);


	/**
	 *
	 */
	void
	set_icmpv6_neighbor_taddr(
			caddress const& addr);


	/**
	 *
	 */
	uint16_t
	get_udp_src() const
		throw (eOFmatchNotFound);



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
	get_udp_dst() const
		throw (eOFmatchNotFound);


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
	get_tcp_src() const
		throw (eOFmatchNotFound);



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
	get_tcp_dst() const
		throw (eOFmatchNotFound);


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
	get_sctp_src() const
		throw (eOFmatchNotFound);



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
	get_sctp_dst() const
		throw (eOFmatchNotFound);


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
};

}; // end of namespace

#endif
