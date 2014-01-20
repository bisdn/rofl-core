/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmlist.h
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#ifndef COXMLIST_H_
#define COXMLIST_H_

#include <ostream>
#include <string>
#include <deque>
#include <map>

#include <algorithm>

#include "../cmemory.h"
#include "../croflexception.h"
#include "../cvastring.h"
#include "rofl/platform/unix/csyslog.h"
#include "../openflow/openflow_rofl_exceptions.h"

#include "coxmatch.h"
#include "../openflow/experimental/matches/pppoe_matches.h"
#include "../openflow/experimental/matches/gtp_matches.h"

namespace rofl
{

class eOxmListBase 			: public RoflException {};
class eOxmListInval 		: public eOxmListBase {}; // invalid parameter
class eOxmListBadLen 		: public eOxmListBase {}; // bad length
class eOxmListNotFound 		: public eOxmListBase {}; // element not found
class eOxmListOutOfRange 	: public eOxmListBase {}; // out of range


/** this class contains a list of Openflow eXtensible Matches (OXM)
 * it does not contain a full struct ofp_match, see class cofmatch for this
 *
 */
class coxmlist :
	public csyslog
{
private: // data structures

		std::string 										info;
		std::map<uint16_t, std::map<uint8_t, coxmatch*> >	matches;
							// key1: OXM TLV class, key2: OXM TLV field, value: ptr to coxmatch instance on heap


public: // methods


		/** constructor
		 *
		 */
		coxmlist();


		/** destructor
		 *
		 */
		virtual
		~coxmlist();


		/**
		 *
		 */
		coxmlist(
				coxmlist const& oxmlist);


		/**
		 *
		 */
		coxmlist&
		operator= (
				coxmlist const& oxmlist);


		/**
		 *
		 */
		bool
		operator== (
				coxmlist const& oxmlist);


		/**
		 * @brief	Returns number of OXM TLVs stored in this coxmlist instance.
		 */
		unsigned int
		get_n_matches() const;


public:


		/** stores cofinst instances in this->invec from a packed array struct ofp_instruction (e.g. in struct ofp_flow_mod)
		 *
		 */
		void
		unpack(
				uint8_t* buf,
				size_t buflen);


		/** builds an array of struct ofp_instruction from this->oxmvec
		 *
		 */
		void
		pack(
				uint8_t* buf,
				size_t buflen);


		/**
		 * @brief	Returns length of this coxmlist instance when begin packed.
		 */
		size_t
		length() const;



		/** erase oxmlist
		 *
		 */
		void
		erase(
				uint16_t oxm_class,
				uint8_t oxm_field);


		/** insert coxmatch instance
		 *
		 */
		void
		insert(
				coxmatch const& oxm);

		/**
		 *
		 */
		bool
		has_match(
				uint16_t ofm_class, uint8_t ofm_field) const;

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

		/**
		 *
		 */
		void
		clear();




		/**
		 *
		 */
		bool
		contains(
				coxmlist const& oxmlist,
				bool strict = false);



		/**
		 *
		 */
		bool
		is_part_of(
				coxmlist const& oxmlist,
				uint16_t& exact_hits,
				uint16_t& wildcard_hits,
				uint16_t& missed);

private:

		void
		map_and_insert(
				coxmatch const& oxm);

public:

		friend std::ostream&
		operator<< (std::ostream& os, coxmlist const& oxl) {
			for (std::map<uint16_t, std::map<uint8_t, coxmatch*> >::const_iterator
					it = oxl.matches.begin(); it != oxl.matches.end(); ++it) {
				for (std::map<uint8_t, coxmatch*>::const_iterator
						jt = it->second.begin(); jt != it->second.end(); ++jt) {
						switch (jt->second->get_oxm_class()) {
						case openflow::OFPXMC_OPENFLOW_BASIC: {
							switch (jt->second->get_oxm_field()) {
							case openflow::OFPXMT_OFB_IN_PORT:
								if (dynamic_cast<coxmatch_ofb_in_port*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_in_port*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IN_PHY_PORT:
								if (dynamic_cast<coxmatch_ofb_in_phy_port*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_in_phy_port*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_METADATA:
								if (dynamic_cast<coxmatch_ofb_metadata*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_metadata*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ETH_DST:
								if (dynamic_cast<coxmatch_ofb_eth_dst*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_eth_dst*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ETH_SRC:
								if (dynamic_cast<coxmatch_ofb_eth_src*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_eth_src*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ETH_TYPE:
								if (dynamic_cast<coxmatch_ofb_eth_type*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_eth_type*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_VLAN_VID:
								if (dynamic_cast<coxmatch_ofb_vlan_vid*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_vlan_vid*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_VLAN_PCP:
								if (dynamic_cast<coxmatch_ofb_vlan_pcp*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_vlan_pcp*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IP_DSCP:
								if (dynamic_cast<coxmatch_ofb_ip_dscp*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ip_dscp*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IP_ECN:
								if (dynamic_cast<coxmatch_ofb_ip_ecn*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ip_ecn*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IP_PROTO:
								if (dynamic_cast<coxmatch_ofb_ip_proto*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ip_proto*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IPV4_SRC:
								if (dynamic_cast<coxmatch_ofb_ipv4_src*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ipv4_src*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IPV4_DST:
								if (dynamic_cast<coxmatch_ofb_ipv4_dst*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ipv4_dst*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_TCP_SRC:
								if (dynamic_cast<coxmatch_ofb_tcp_src*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_tcp_src*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_TCP_DST:
								if (dynamic_cast<coxmatch_ofb_tcp_dst*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_tcp_dst*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_UDP_SRC:
								if (dynamic_cast<coxmatch_ofb_udp_src*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_udp_src*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_UDP_DST:
								if (dynamic_cast<coxmatch_ofb_udp_dst*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_udp_dst*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_SCTP_SRC:
								if (dynamic_cast<coxmatch_ofb_sctp_src*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_sctp_src*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_SCTP_DST:
								if (dynamic_cast<coxmatch_ofb_sctp_dst*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_sctp_dst*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ICMPV4_TYPE:
								if (dynamic_cast<coxmatch_ofb_icmpv4_type*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_icmpv4_type*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ICMPV4_CODE:
								if (dynamic_cast<coxmatch_ofb_icmpv4_code*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_icmpv4_code*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ARP_OP:
								if (dynamic_cast<coxmatch_ofb_arp_opcode*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_arp_opcode*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ARP_SPA:
								if (dynamic_cast<coxmatch_ofb_arp_spa*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_arp_spa*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ARP_TPA:
								if (dynamic_cast<coxmatch_ofb_arp_tpa*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_arp_tpa*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ARP_SHA:
								if (dynamic_cast<coxmatch_ofb_arp_sha*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_arp_sha*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ARP_THA:
								if (dynamic_cast<coxmatch_ofb_arp_tpa*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_arp_tpa*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IPV6_SRC:
								if (dynamic_cast<coxmatch_ofb_ipv6_src*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ipv6_src*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IPV6_DST:
								if (dynamic_cast<coxmatch_ofb_ipv6_dst*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ipv6_dst*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IPV6_FLABEL:
								if (dynamic_cast<coxmatch_ofb_ipv6_flabel*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ipv6_flabel*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ICMPV6_TYPE:
								if (dynamic_cast<coxmatch_ofb_icmpv6_type*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_icmpv6_type*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_ICMPV6_CODE:
								if (dynamic_cast<coxmatch_ofb_icmpv6_code*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_icmpv6_code*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IPV6_ND_TARGET:
								if (dynamic_cast<coxmatch_ofb_ipv6_nd_target*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ipv6_nd_target*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IPV6_ND_SLL:
								if (dynamic_cast<coxmatch_ofb_ipv6_nd_sll*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ipv6_nd_sll*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_IPV6_ND_TLL:
								if (dynamic_cast<coxmatch_ofb_ipv6_nd_tll*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_ipv6_nd_tll*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_MPLS_LABEL:
								if (dynamic_cast<coxmatch_ofb_mpls_label*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_mpls_label*>(jt->second);
								break;
							case openflow::OFPXMT_OFB_MPLS_TC:
								if (dynamic_cast<coxmatch_ofb_mpls_tc*>(jt->second))
									os << *dynamic_cast<coxmatch_ofb_mpls_tc*>(jt->second);
								break;
							default:
								os << *(jt->second);
								break;
							}

						} break;
						case openflow::OFPXMC_EXPERIMENTER: {
							switch (jt->second->get_oxm_field()) {
							case openflow::experimental::OFPXMT_OFX_NW_SRC:
								if (dynamic_cast<coxmatch_ofx_nw_src*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_nw_src*>(jt->second);
								break;
							case openflow::experimental::OFPXMT_OFX_NW_DST:
								if (dynamic_cast<coxmatch_ofx_nw_dst*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_nw_dst*>(jt->second);
								break;
							case openflow::experimental::OFPXMT_OFX_NW_PROTO:
								if (dynamic_cast<coxmatch_ofx_nw_proto*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_nw_proto*>(jt->second);
								break;
							case openflow::experimental::OFPXMT_OFX_TP_SRC:
								if (dynamic_cast<coxmatch_ofx_tp_src*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_tp_src*>(jt->second);
								break;
							case openflow::experimental::OFPXMT_OFX_TP_DST:
								if (dynamic_cast<coxmatch_ofx_tp_dst*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_tp_dst*>(jt->second);
								break;
							case openflow::experimental::OFPXMT_OFX_PPPOE_CODE:
								if (dynamic_cast<coxmatch_ofx_pppoe_code*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_pppoe_code*>(jt->second);
								break;
							case openflow::experimental::OFPXMT_OFX_PPPOE_TYPE:
								if (dynamic_cast<coxmatch_ofx_pppoe_type*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_pppoe_type*>(jt->second);
								break;
							case openflow::experimental::OFPXMT_OFX_PPPOE_SID:
								if (dynamic_cast<coxmatch_ofx_pppoe_sid*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_pppoe_sid*>(jt->second);
								break;
							case openflow::experimental::OFPXMT_OFX_PPP_PROT:
								if (dynamic_cast<coxmatch_ofx_ppp_prot*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_ppp_prot*>(jt->second);
								break;
							case openflow::experimental::OFPXMT_OFX_GTP_MSG_TYPE:
								if (dynamic_cast<coxmatch_ofx_gtp_msg_type*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_gtp_msg_type*>(jt->second);
								break;
							case openflow::experimental::OFPXMT_OFX_GTP_TEID:
								if (dynamic_cast<coxmatch_ofx_gtp_teid*>(jt->second))
									os << *dynamic_cast<coxmatch_ofx_gtp_teid*>(jt->second);
								break;
							default:
								os << *(jt->second);
							}
						} break;
						case openflow::OFPXMC_NXM_0:
						case openflow::OFPXMC_NXM_1:
						default:
							os << *(jt->second);
						}
				}
			}
			return os;
		};
};

}; // end of namespace

#endif /* COXMLIST_H_ */
