/*
 * coxmatch_output.h
 *
 *  Created on: 20.08.2014
 *      Author: andreas
 */

#ifndef COXMATCH_OUTPUT_H_
#define COXMATCH_OUTPUT_H_

#include "rofl.h"
#include "rofl/common/openflow/coxmatch.h"

#ifdef ROFL_EXPERIMENTAL
	#include "rofl/common/openflow/experimental/matches/gtp_matches.h"
	#include "rofl/common/openflow/experimental/matches/pppoe_matches.h"
#endif


namespace rofl {
namespace openflow {


class coxmatch_output {
	coxmatch oxm;
public:
	coxmatch_output(coxmatch const& oxm) : oxm(oxm) {};

	friend std::ostream&
	operator<<(std::ostream& os, coxmatch_output const& oxm_output) {
		coxmatch const& oxm = oxm_output.oxm;
		switch (oxm.get_oxm_class()) {
		case openflow::OFPXMC_OPENFLOW_BASIC: {
			switch (oxm.get_oxm_field()) {
			case openflow::OFPXMT_OFB_IN_PORT:
				os << coxmatch_ofb_in_port(oxm); return os;
			case openflow::OFPXMT_OFB_IN_PHY_PORT:
				os << coxmatch_ofb_in_phy_port(oxm); return os;
			case openflow::OFPXMT_OFB_METADATA:
				os << coxmatch_ofb_metadata(oxm); return os;
			case openflow::OFPXMT_OFB_ETH_DST:
				os << coxmatch_ofb_eth_dst(oxm); return os;
			case openflow::OFPXMT_OFB_ETH_SRC:
				os << coxmatch_ofb_eth_src(oxm); return os;
			case openflow::OFPXMT_OFB_ETH_TYPE:
				os << coxmatch_ofb_eth_type(oxm); return os;
			case openflow::OFPXMT_OFB_VLAN_VID:
				os << coxmatch_ofb_vlan_vid(oxm); return os;
			case openflow::OFPXMT_OFB_VLAN_PCP:
				os << coxmatch_ofb_vlan_pcp(oxm); return os;
			case openflow::OFPXMT_OFB_IP_DSCP:
				os << coxmatch_ofb_ip_dscp(oxm); return os;
			case openflow::OFPXMT_OFB_IP_ECN:
				os << coxmatch_ofb_ip_ecn(oxm); return os;
			case openflow::OFPXMT_OFB_IP_PROTO:
				os << coxmatch_ofb_ip_proto(oxm); return os;
			case openflow::OFPXMT_OFB_IPV4_SRC:
				os << coxmatch_ofb_ipv4_src(oxm); return os;
			case openflow::OFPXMT_OFB_IPV4_DST:
				os << coxmatch_ofb_ipv4_dst(oxm); return os;
			case openflow::OFPXMT_OFB_TCP_SRC:
				os << coxmatch_ofb_tcp_src(oxm); return os;
			case openflow::OFPXMT_OFB_TCP_DST:
				os << coxmatch_ofb_tcp_dst(oxm); return os;
			case openflow::OFPXMT_OFB_UDP_SRC:
				os << coxmatch_ofb_udp_src(oxm); return os;
			case openflow::OFPXMT_OFB_UDP_DST:
				os << coxmatch_ofb_udp_dst(oxm); return os;
			case openflow::OFPXMT_OFB_SCTP_SRC:
				os << coxmatch_ofb_sctp_src(oxm); return os;
			case openflow::OFPXMT_OFB_SCTP_DST:
				os << coxmatch_ofb_sctp_dst(oxm); return os;
			case openflow::OFPXMT_OFB_ICMPV4_TYPE:
				os << coxmatch_ofb_icmpv4_type(oxm); return os;
			case openflow::OFPXMT_OFB_ICMPV4_CODE:
				os << coxmatch_ofb_icmpv4_code(oxm); return os;
			case openflow::OFPXMT_OFB_ARP_OP:
				os << coxmatch_ofb_arp_opcode(oxm); return os;
			case openflow::OFPXMT_OFB_ARP_SPA:
				os << coxmatch_ofb_arp_spa(oxm); return os;
			case openflow::OFPXMT_OFB_ARP_TPA:
				os << coxmatch_ofb_arp_tpa(oxm); return os;
			case openflow::OFPXMT_OFB_ARP_SHA:
				os << coxmatch_ofb_arp_sha(oxm); return os;
			case openflow::OFPXMT_OFB_ARP_THA:
				os << coxmatch_ofb_arp_tha(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_SRC:
				os << coxmatch_ofb_ipv6_src(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_DST:
				os << coxmatch_ofb_ipv6_dst(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_FLABEL:
				os << coxmatch_ofb_ipv6_flabel(oxm); return os;
			case openflow::OFPXMT_OFB_ICMPV6_TYPE:
				os << coxmatch_ofb_icmpv6_type(oxm); return os;
			case openflow::OFPXMT_OFB_ICMPV6_CODE:
				os << coxmatch_ofb_icmpv6_code(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_ND_TARGET:
				os << coxmatch_ofb_ipv6_nd_target(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_ND_SLL:
				os << coxmatch_ofb_ipv6_nd_sll(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_ND_TLL:
				os << coxmatch_ofb_ipv6_nd_tll(oxm); return os;
			case openflow::OFPXMT_OFB_MPLS_LABEL:
				os << coxmatch_ofb_mpls_label(oxm); return os;
			case openflow::OFPXMT_OFB_MPLS_TC:
				os << coxmatch_ofb_mpls_tc(oxm); return os;
			case openflow::OFPXMT_OFB_MPLS_BOS:
				os << coxmatch_ofb_mpls_bos(oxm); return os;
			case openflow::OFPXMT_OFB_TUNNEL_ID:
				os << coxmatch_ofb_tunnel_id(oxm); return os;
			case openflow::OFPXMT_OFB_PBB_ISID:
				os << coxmatch_ofb_pbb_isid(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_EXTHDR:
				os << coxmatch_ofb_ipv6_exthdr(oxm); return os;
			default:
				os << oxm; return os;
			}
		} break;
		case openflow::OFPXMC_EXPERIMENTER: {
			switch (oxm.get_oxm_field()) {
			case openflow::experimental::OFPXMT_OFX_NW_SRC:
				os << coxmatch_ofx_nw_src(oxm); return os;
			case openflow::experimental::OFPXMT_OFX_NW_DST:
				os << coxmatch_ofx_nw_dst(oxm); return os;
			case openflow::experimental::OFPXMT_OFX_NW_PROTO:
				os << coxmatch_ofx_nw_proto(oxm); return os;
			case openflow::experimental::OFPXMT_OFX_NW_TOS:
				os << coxmatch_ofx_nw_tos(oxm); return os;
			case openflow::experimental::OFPXMT_OFX_TP_SRC:
				os << coxmatch_ofx_tp_src(oxm); return os;
			case openflow::experimental::OFPXMT_OFX_TP_DST:
				os << coxmatch_ofx_tp_dst(oxm); return os;
#ifdef ROFL_EXPERIMENTAL
			case openflow::experimental::gtp::OFPXMT_OFX_GTP_MSGTYPE:
				os << openflow::experimental::gtp::coxmatch_ofx_gtp_msg_type(oxm); return os;
			case openflow::experimental::gtp::OFPXMT_OFX_GTP_TEID:
				os << openflow::experimental::gtp::coxmatch_ofx_gtp_teid(oxm); return os;
			case openflow::experimental::pppoe::OFPXMT_OFX_PPPOE_SID:
				os << openflow::experimental::pppoe::coxmatch_ofx_pppoe_sid(oxm); return os;
			case openflow::experimental::pppoe::OFPXMT_OFX_PPPOE_CODE:
				os << openflow::experimental::pppoe::coxmatch_ofx_pppoe_code(oxm); return os;
			case openflow::experimental::pppoe::OFPXMT_OFX_PPPOE_TYPE:
				os << openflow::experimental::pppoe::coxmatch_ofx_pppoe_type(oxm); return os;
			case openflow::experimental::pppoe::OFPXMT_OFX_PPP_PROT:
				os << openflow::experimental::pppoe::coxmatch_ofx_ppp_prot(oxm); return os;
#endif
			default:
				os << oxm; return os;
			}
		} break;
		default:
			os << oxm; return os;
		}

		return os;
	};
	
	
	
	void
	str_sl(std::ostream& os) const {
		coxmatch const& oxm = this->oxm;
		switch (oxm.get_oxm_class()) {
		case openflow::OFPXMC_OPENFLOW_BASIC: {
			switch (oxm.get_oxm_field()) {
			case openflow::OFPXMT_OFB_IN_PORT:
				coxmatch_ofb_in_port(oxm).str_sl(os);
			case openflow::OFPXMT_OFB_IN_PHY_PORT:
				os << coxmatch_ofb_in_phy_port(oxm);
			case openflow::OFPXMT_OFB_METADATA:
				os << coxmatch_ofb_metadata(oxm);
			case openflow::OFPXMT_OFB_ETH_DST:
				os << coxmatch_ofb_eth_dst(oxm);
			case openflow::OFPXMT_OFB_ETH_SRC:
				os << coxmatch_ofb_eth_src(oxm);
			case openflow::OFPXMT_OFB_ETH_TYPE:
				os << coxmatch_ofb_eth_type(oxm);
			case openflow::OFPXMT_OFB_VLAN_VID:
				os << coxmatch_ofb_vlan_vid(oxm);
			case openflow::OFPXMT_OFB_VLAN_PCP:
				os << coxmatch_ofb_vlan_pcp(oxm);
			case openflow::OFPXMT_OFB_IP_DSCP:
				os << coxmatch_ofb_ip_dscp(oxm);
			case openflow::OFPXMT_OFB_IP_ECN:
				os << coxmatch_ofb_ip_ecn(oxm);
			case openflow::OFPXMT_OFB_IP_PROTO:
				os << coxmatch_ofb_ip_proto(oxm);
			case openflow::OFPXMT_OFB_IPV4_SRC:
				os << coxmatch_ofb_ipv4_src(oxm);
			case openflow::OFPXMT_OFB_IPV4_DST:
				os << coxmatch_ofb_ipv4_dst(oxm);
			case openflow::OFPXMT_OFB_TCP_SRC:
				os << coxmatch_ofb_tcp_src(oxm);
			case openflow::OFPXMT_OFB_TCP_DST:
				os << coxmatch_ofb_tcp_dst(oxm);
			case openflow::OFPXMT_OFB_UDP_SRC:
				os << coxmatch_ofb_udp_src(oxm);
			case openflow::OFPXMT_OFB_UDP_DST:
				os << coxmatch_ofb_udp_dst(oxm);
			case openflow::OFPXMT_OFB_SCTP_SRC:
				os << coxmatch_ofb_sctp_src(oxm);
			case openflow::OFPXMT_OFB_SCTP_DST:
				os << coxmatch_ofb_sctp_dst(oxm);
			case openflow::OFPXMT_OFB_ICMPV4_TYPE:
				os << coxmatch_ofb_icmpv4_type(oxm);
			case openflow::OFPXMT_OFB_ICMPV4_CODE:
				os << coxmatch_ofb_icmpv4_code(oxm);
			case openflow::OFPXMT_OFB_ARP_OP:
				os << coxmatch_ofb_arp_opcode(oxm);
			case openflow::OFPXMT_OFB_ARP_SPA:
				os << coxmatch_ofb_arp_spa(oxm);
			case openflow::OFPXMT_OFB_ARP_TPA:
				os << coxmatch_ofb_arp_tpa(oxm);
			case openflow::OFPXMT_OFB_ARP_SHA:
				os << coxmatch_ofb_arp_sha(oxm);
			case openflow::OFPXMT_OFB_ARP_THA:
				os << coxmatch_ofb_arp_tha(oxm);
			case openflow::OFPXMT_OFB_IPV6_SRC:
				os << coxmatch_ofb_ipv6_src(oxm);
			case openflow::OFPXMT_OFB_IPV6_DST:
				os << coxmatch_ofb_ipv6_dst(oxm);
			case openflow::OFPXMT_OFB_IPV6_FLABEL:
				os << coxmatch_ofb_ipv6_flabel(oxm);
			case openflow::OFPXMT_OFB_ICMPV6_TYPE:
				os << coxmatch_ofb_icmpv6_type(oxm);
			case openflow::OFPXMT_OFB_ICMPV6_CODE:
				os << coxmatch_ofb_icmpv6_code(oxm);
			case openflow::OFPXMT_OFB_IPV6_ND_TARGET:
				os << coxmatch_ofb_ipv6_nd_target(oxm);
			case openflow::OFPXMT_OFB_IPV6_ND_SLL:
				os << coxmatch_ofb_ipv6_nd_sll(oxm);
			case openflow::OFPXMT_OFB_IPV6_ND_TLL:
				os << coxmatch_ofb_ipv6_nd_tll(oxm);
			case openflow::OFPXMT_OFB_MPLS_LABEL:
				os << coxmatch_ofb_mpls_label(oxm);
			case openflow::OFPXMT_OFB_MPLS_TC:
				os << coxmatch_ofb_mpls_tc(oxm);
			case openflow::OFPXMT_OFB_MPLS_BOS:
				os << coxmatch_ofb_mpls_bos(oxm);
			case openflow::OFPXMT_OFB_TUNNEL_ID:
				os << coxmatch_ofb_tunnel_id(oxm);
			case openflow::OFPXMT_OFB_PBB_ISID:
				os << coxmatch_ofb_pbb_isid(oxm);
			case openflow::OFPXMT_OFB_IPV6_EXTHDR:
				os << coxmatch_ofb_ipv6_exthdr(oxm);
			default:
				os << oxm;
			}
		} break;
		case openflow::OFPXMC_EXPERIMENTER: {
			switch (oxm.get_oxm_field()) {
			case openflow::experimental::OFPXMT_OFX_NW_SRC:
				os << coxmatch_ofx_nw_src(oxm);
			case openflow::experimental::OFPXMT_OFX_NW_DST:
				os << coxmatch_ofx_nw_dst(oxm);
			case openflow::experimental::OFPXMT_OFX_NW_PROTO:
				os << coxmatch_ofx_nw_proto(oxm);
			case openflow::experimental::OFPXMT_OFX_NW_TOS:
				os << coxmatch_ofx_nw_tos(oxm);
			case openflow::experimental::OFPXMT_OFX_TP_SRC:
				os << coxmatch_ofx_tp_src(oxm);
			case openflow::experimental::OFPXMT_OFX_TP_DST:
				os << coxmatch_ofx_tp_dst(oxm);
#ifdef ROFL_EXPERIMENTAL
			case openflow::experimental::gtp::OFPXMT_OFX_GTP_MSGTYPE:
				os << openflow::experimental::gtp::coxmatch_ofx_gtp_msg_type(oxm);
			case openflow::experimental::gtp::OFPXMT_OFX_GTP_TEID:
				os << openflow::experimental::gtp::coxmatch_ofx_gtp_teid(oxm);
			case openflow::experimental::pppoe::OFPXMT_OFX_PPPOE_SID:
				os << openflow::experimental::pppoe::coxmatch_ofx_pppoe_sid(oxm);
			case openflow::experimental::pppoe::OFPXMT_OFX_PPPOE_CODE:
				os << openflow::experimental::pppoe::coxmatch_ofx_pppoe_code(oxm);
			case openflow::experimental::pppoe::OFPXMT_OFX_PPPOE_TYPE:
				os << openflow::experimental::pppoe::coxmatch_ofx_pppoe_type(oxm);
			case openflow::experimental::pppoe::OFPXMT_OFX_PPP_PROT:
				os << openflow::experimental::pppoe::coxmatch_ofx_ppp_prot(oxm);
#endif
			default:
				os << oxm;
			}
		} break;
		default:
			os << oxm;
		}

	};
};


}; // end of namespace openflow
}; // end of namespace rofl




#endif /* COXMATCH_OUTPUT_H_ */
