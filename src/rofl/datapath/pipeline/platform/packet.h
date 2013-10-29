/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PLATFORM_PACKET_HOOKS
#define PLATFORM_PACKET_HOOKS

#include <stdint.h>

#include "rofl.h"
#include "../common/datapacket.h"
#include "../common/ternary_fields.h"
#include "../switch_port.h"

/**
* @file packet.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Defines the interface for packet mangling. 
* The user of the library MUST provide an implementation for his/her platform.
*
* The packet mangling platform API is the set of calls used by library
* to perform operations over the packet. These operations are:
* 
* - Get header field
* - Get port in, phy port in, packet size, queue, metadata
* - Set field in header or in the metadata (including port queue id)
* - Push header
* - Pop header
* - Drop packet
* - Replicate (clone) packet
* - Output the packet to a port
*
* If the platform does NOT support some of the operations contained
* in this API, getters MUST always return 0x0 and actions must return immediately. If
* this is the case, the bitmap of the supported actions and matches shall be
* set appropiately in the platform_post_init_of1x_switch() hook, in ALL of the switch tables, to 
* prevent invalid flow mods to be installed 
*/

//C++ extern C
ROFL_BEGIN_DECLS

//////////////////////
// Packet mangling //
////////////////////

/**
* @ingroup platform_packet
* Gets the complete packet size in bytes.
*/
uint32_t platform_packet_get_size_bytes(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Output packet to the port(s)
* The action HAS to implement the destruction/release of the pkt
* (including if this pkt is a replica).
*
* If a flooding output actions needs to be done, the function
* has itself to deal with packet replication.
*/
void platform_packet_output(datapacket_t* pkt, switch_port_t* port);
/**
* @ingroup platform_packet
* Creates a copy (in heap) of the datapacket_t structure including any
* platform specific state (->platform_state). The following behaviour
* is expected from this hook:
* 
* - All data fields and pointers of datapacket_t struct must be memseted to 0, except:
* - datapacket_t flag is_replica must be set to true
* - platform_state, if used, must be replicated (copied) otherwise NULL
*
*/
void platform_packet_drop(datapacket_t* pkt);
/**
* @ingroup platform_packet
* Creates a copy (in heap) of the datapacket_t structure including any
* platform specific state (->platform_state). The following behaviour
* is expected from this hook:
* 
* - All data fields and pointers of datapacket_t struct must be memseted to 0, except:
* - datapacket_t flag is_replica must be set to true
* - platform_state, if used, must be replicated (copied) otherwise NULL
* - Real packet buffer may or may not be copied, depending on the platform ability
*   to handle copies (lazy copying)
*/
datapacket_t* platform_packet_replicate(datapacket_t* pkt);


////////////
// Ports //
//////////

/**
* @ingroup platform_packet
* Retrieves the port in identifier (position in the of1x_switch_t->logical_ports array) where the packet got in
*/
uint32_t platform_packet_get_port_in(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Retrieves the PHY port in identifier where the packet got in
* @return id if existing, 0x0 otherwise.
*/
uint32_t platform_packet_get_phy_port_in(datapacket_t *const pkt);	

/**
* @ingroup platform_packet
* Set output queue of the packet 
*/
void platform_packet_set_queue(datapacket_t* pkt, uint32_t queue);


///////////////
// Ethernet //
/////////////

/**
* @ingroup platform_packet
* Retrieve the Ethernet src MAC address of the packet 
*/
uint64_t platform_packet_get_eth_src(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Retrieve the Ethernet dst MAC address of the packet 
*/
uint64_t platform_packet_get_eth_dst(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Retrieve the Ethernet ETH_TYPE of the packet 
*/
uint16_t platform_packet_get_eth_type(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new Ethernet src MAC address to the packet 
*/
void platform_packet_set_eth_src(datapacket_t* pkt, uint64_t eth_src);
/**
* @ingroup platform_packet
* Set a new Ethernet dst MAC address to the packet 
*/
void platform_packet_set_eth_dst(datapacket_t* pkt, uint64_t eth_dst);
/**
* @ingroup platform_packet
* Set a new ETH_TYPE to the packet if it is 802.X 
*/
void platform_packet_set_eth_type(datapacket_t* pkt, uint16_t eth_type);


//////////////////
// 802.1q VLAN //
////////////////
/**
* @ingroup platform_packet
* Retrieves boolean if the packet contains a 802.1q VLAN tag
*/
bool platform_packet_has_vlan(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Retrieves the VLAN id of the outer-most 802.1q VLAN tag
* @return id if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_vlan_vid(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Retrieves the VLAN PCP of the outer-most 802.1q VLAN tag
* @return id if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_vlan_pcp(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Set a new VLAN VID value to the outer-most VLAN of the packet
*/
void platform_packet_set_vlan_vid(datapacket_t* pkt, uint16_t vlan_vid);
/**
* @ingroup platform_packet
* Set a new VLAN PCP value to the outer-most VLAN of the packet
*/
void platform_packet_set_vlan_pcp(datapacket_t* pkt, uint8_t vlan_pcp);
/**
* @ingroup platform_packet
* Pop the outer-most 802.1q VLAN tag
*/
void platform_packet_pop_vlan(datapacket_t* pkt);
/**
* @ingroup platform_packet
* Push a new 802.1q VLAN tag (outer-most)
*/
void platform_packet_push_vlan(datapacket_t* pkt, uint16_t ether_type);


///////////
// MPLS //
/////////

/**
* @ingroup platform_packet
* Retrieves the outer-most MPLS tag label
* @return label if existing, 0x0 otherwise.
*/
uint32_t platform_packet_get_mpls_label(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Retrieves the outer-most MPLS tag tc flag 
* @return flag if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_mpls_tc(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Retrieves the outer-most MPLS tag BoS flag 
* @return flag if existing, 0x0 otherwise.
*/
bool platform_packet_get_mpls_bos(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new label to the outer-most MPLS tag of the packet
*/
void platform_packet_set_mpls_label(datapacket_t* pkt, uint32_t label);
/**
* @ingroup platform_packet
* Set a new TC value to the outer-most MPLS MPLS tag of the packet
*/
void platform_packet_set_mpls_tc(datapacket_t* pkt, uint8_t tc);
/**
* @ingroup platform_packet
* Set a new MPLS BoS value to the outer-most MPLS tag of the packet
*/
void platform_packet_set_mpls_bos(datapacket_t* pkt, bool bos);
/**
* @ingroup platform_packet
* Set a new TTL value to the outer-most MPLS tag of the packet
*/
void platform_packet_set_mpls_ttl(datapacket_t* pkt, uint8_t new_ttl);

/**
* @ingroup platform_packet
* Decrement the ttl from the outer-most MPLS tag of the packet
*/
void platform_packet_dec_mpls_ttl(datapacket_t* pkt);
/**
* @ingroup platform_packet
* Pop outer-most MPLS tag, setting the BoS accordingly
*/
void platform_packet_pop_mpls(datapacket_t* pkt, uint16_t ether_type);
/**
* @ingroup platform_packet
* Push a new MPLS tag (outer-most), and set inner and outer tag BoS accordingly 
*/
void platform_packet_push_mpls(datapacket_t* pkt, uint16_t ether_type);


////////////////
// Tunnel ID //
//////////////
/**
* @ingroup platform_packet
* Get Tunnel ID
*/
uint64_t platform_packet_get_tunnel_id(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Set Tunnel ID
*/
void platform_packet_set_tunnel_id(datapacket_t*pkt, uint64_t tunnel_id);

//////////
// PBB //
////////
/**
* @ingroup platform_packet
* Get PBB I-SID value of the packet
* @return value if existing, 0x0 otherwise.
*/
uint32_t platform_packet_get_pbb_isid(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the PBB I-SID of the packet
*/
void platform_packet_set_pbb_isid(datapacket_t*pkt, uint32_t pbb_isid);
/**
* @ingroup platform_packet
* Pop outer-most PBB header
*/
void platform_packet_pop_pbb(datapacket_t* pkt, uint16_t ether_type);
/**
* @ingroup platform_packet
* Push PBB header (outer-most)
*/
void platform_packet_push_pbb(datapacket_t* pkt, uint16_t ether_type);

/////////////////
// TTL common //
///////////////
/**
* @ingroup platform_packet
* Copy ttl in
*/
void platform_packet_copy_ttl_in(datapacket_t* pkt);
/**
* @ingroup platform_packet
* Copy ttl out 
*/
void platform_packet_copy_ttl_out(datapacket_t* pkt);
/**
* @ingroup platform_packet
* Decrement ttl
*/
void platform_packet_dec_nw_ttl(datapacket_t* pkt);
/**
* @ingroup platform_packet
* Set new ttl value
*/
void platform_packet_set_nw_ttl(datapacket_t* pkt, uint8_t new_ttl);


//////////////////
// IP (common) //
////////////////
/**
* @ingroup platform_packet
* Get the ip proto value of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_ip_proto(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the ip ECN value of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_ip_ecn(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the ip DSCP value of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_ip_dscp(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the IP PROTO header field
*/
void platform_packet_set_ip_proto(datapacket_t* pkt, uint8_t ip_proto);
/**
* @ingroup platform_packet
* Set a new value for the IP DSCP header field
*/
void platform_packet_set_ip_dscp(datapacket_t* pkt, uint8_t ip_dscp);
/**
* @ingroup platform_packet
* Set a new value for the IP ECN header field
*/
void platform_packet_set_ip_ecn(datapacket_t* pkt, uint8_t ip_ecn);

//////////
// ARP //
////////
/**
* @ingroup platform_packet
* Get the ARP OPCODE value of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_arp_opcode(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the ARP SHA value of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint64_t platform_packet_get_arp_sha(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the ARP SPA value of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint32_t platform_packet_get_arp_spa(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the ARP THA value of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint64_t platform_packet_get_arp_tha(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the ARP TPA value of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint32_t platform_packet_get_arp_tpa(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the ARP OPCODE header field
*/
void platform_packet_set_arp_opcode(datapacket_t* pkt, uint16_t arp_opcode);
/**
* @ingroup platform_packet
* Set a new value for the ARP SHA header field
*/
void platform_packet_set_arp_sha(datapacket_t* pkt, uint64_t arp_sha);
/**
* @ingroup platform_packet
* Set a new value for the ARP SPA header field
*/
void platform_packet_set_arp_spa(datapacket_t* pkt, uint32_t arp_spa);
/**
* @ingroup platform_packet
* Set a new value for the ARP THA header field
*/
void platform_packet_set_arp_tha(datapacket_t* pkt, uint64_t arp_tha);
/**
* @ingroup platform_packet
* Set a new value for the ARP TPA header field
*/
void platform_packet_set_arp_tpa(datapacket_t* pkt, uint32_t arp_tpa);


///////////
// IPv4 //
/////////
/**
* @ingroup platform_packet
* Get the IPv4 src address of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint32_t platform_packet_get_ipv4_src(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the IPv4 dst address of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint32_t platform_packet_get_ipv4_dst(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the IPv4 src address
*/
void platform_packet_set_ipv4_src(datapacket_t* pkt, uint32_t ip_src);

/**
* @ingroup platform_packet
* Set a new value for the IPv4 dst address
*/
void platform_packet_set_ipv4_dst(datapacket_t* pkt, uint32_t ip_dst);


///////////
// IPv6 //
/////////
/**
* @ingroup platform_packet
* Get the IPv6 src address of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint128__t platform_packet_get_ipv6_src(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the IPv6 dst address of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint128__t platform_packet_get_ipv6_dst(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the IPv6 label of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint64_t platform_packet_get_ipv6_flabel(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the IPv6 nd target of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint128__t platform_packet_get_ipv6_nd_target(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the IPv6 nd sll of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint64_t platform_packet_get_ipv6_nd_sll(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the IPv6 nd tll of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint64_t platform_packet_get_ipv6_nd_tll(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the IPv6 exthdr of the packet 
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_ipv6_exthdr(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the IPv6 src address
*/
void platform_packet_set_ipv6_src(datapacket_t*pkt, uint128__t ipv6_src);
/**
* @ingroup platform_packet
* Set a new value for the IPv6 dst address
*/
void platform_packet_set_ipv6_dst(datapacket_t*pkt, uint128__t ipv6_dst);
/**
* @ingroup platform_packet
* Set a new value for the IPv6 label
*/
void platform_packet_set_ipv6_flabel(datapacket_t*pkt, uint64_t ipv6_flabel);
/**
* @ingroup platform_packet
* Set a new value for the IPv6 nd target
*/
void platform_packet_set_ipv6_nd_target(datapacket_t*pkt, uint128__t ipv6_nd_target);
/**
* @ingroup platform_packet
* Set a new value for the IPv6 nd sll
*/
void platform_packet_set_ipv6_nd_sll(datapacket_t*pkt, uint64_t ipv6_nd_sll);
/**
* @ingroup platform_packet
* Set a new value for the IPv6 nd tll
*/
void platform_packet_set_ipv6_nd_tll(datapacket_t*pkt, uint64_t ipv6_nd_tll);
/**
* @ingroup platform_packet
* Set a new value for the IPv6 exthdr
*/
void platform_packet_set_ipv6_exthdr(datapacket_t*pkt, uint16_t ipv6_exthdr);

//////////
// TCP //
////////
/**
* @ingroup platform_packet
* Get the TCP src port 
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_tcp_src(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the TCP dst port 
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_tcp_dst(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the TCP src port
*/
void platform_packet_set_tcp_src(datapacket_t* pkt, uint16_t tcp_src);
/**
* @ingroup platform_packet
* Set a new value for the TCP dst port
*/
void platform_packet_set_tcp_dst(datapacket_t* pkt, uint16_t tcp_dst);


//////////
// UDP //
////////
/**
* @ingroup platform_packet
* Get the UDP src port 
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_udp_src(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the UDP dst port 
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_udp_dst(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the UDP src port
*/
void platform_packet_set_udp_src(datapacket_t* pkt, uint16_t udp_src);
/**
* @ingroup platform_packet
* Set a new value for the UDP dst port
*/
void platform_packet_set_udp_dst(datapacket_t* pkt, uint16_t udp_dst);


///////////
// SCTP //
/////////
/**
* @ingroup platform_packet
* Get the SCTP src port 
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_sctp_src(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the SCTP dst port 
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_sctp_dst(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the SCTP src port
*/
void platform_packet_set_sctp_src(datapacket_t* pkt, uint16_t sctp_src);
/**
* @ingroup platform_packet
* Set a new value for the SCTP dst port
*/
void platform_packet_set_sctp_dst(datapacket_t* pkt, uint16_t sctp_dst);


/////////////
// ICMPv4 //
///////////
/**
* @ingroup platform_packet
* Get the ICMPv4 type
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_icmpv4_type(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the ICMPv4 code
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_icmpv4_code(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the ICMPv4 type
*/
void platform_packet_set_icmpv4_type(datapacket_t* pkt, uint8_t type);
/**
* @ingroup platform_packet
* Set a new value for the ICMPv4 code 
*/
void platform_packet_set_icmpv4_code(datapacket_t* pkt, uint8_t code);


/////////////
// ICMPv6 //
///////////
/**
* @ingroup platform_packet
* Get the ICMPv6 type
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_icmpv6_type(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the ICMPv6 code
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_icmpv6_code(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the ICMPv6 type
*/
void platform_packet_set_icmpv6_type(datapacket_t*pkt, uint8_t icmpv6_type);
/**
* @ingroup platform_packet
* Set a new value for the ICMPv6 code
*/
void platform_packet_set_icmpv6_code(datapacket_t*pkt, uint8_t icmpv6_code);


/*******************/
/*** Extensions ***/
/*****************/

////////////
// PPPoE //
//////////
/**
* @ingroup platform_packet
* Get the PPPoE code 
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_pppoe_code(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the PPPoE type 
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_pppoe_type(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the PPPoE session id
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_pppoe_sid(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the PPPoE type
*/
void platform_packet_set_pppoe_type(datapacket_t* pkt, uint8_t type);
/**
* @ingroup platform_packet
* Set a new value for the PPPoE code 
*/
void platform_packet_set_pppoe_code(datapacket_t* pkt, uint8_t code);
/**
* @ingroup platform_packet
* Set a new value for the PPPoE session id
*/
void platform_packet_set_pppoe_sid(datapacket_t* pkt, uint16_t sid);

/**
* @ingroup platform_packet
* Pop PPPoE and PPP frame 
*/
void platform_packet_pop_pppoe(datapacket_t* pkt, uint16_t ether_type);
/**
* @ingroup platform_packet
* Push a new PPPoE and PPP frame 
*/
void platform_packet_push_pppoe(datapacket_t* pkt, uint16_t ether_type);

//////////
// PPP //
////////
/**
* @ingroup platform_packet
* Get the PPP proto
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_ppp_proto(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the PPP proto 
*/
void platform_packet_set_ppp_proto(datapacket_t* pkt, uint16_t proto);

//////////
// GTP //
////////
/**
* @ingroup platform_packet
* Get the GTP msg type 
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_gtp_msg_type(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get the GTP teid
* @return value if existing, 0x0 otherwise.
*/
uint32_t platform_packet_get_gtp_teid(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the GTP msg type
*/
void platform_packet_set_gtp_msg_type(datapacket_t* pkt, uint8_t msg_type);
/**
* @ingroup platform_packet
* Set a new value for the GTP teid 
*/
void platform_packet_set_gtp_teid(datapacket_t* pkt, uint32_t teid);

/**
* @ingroup platform_packet
* Pop the GTP frame 
*/
void platform_packet_pop_gtp(datapacket_t* pkt);
/**
* @ingroup platform_packet
* Push a new GTP frame 
*/
void platform_packet_push_gtp(datapacket_t* pkt);

/////////////
// CAPWAP //
///////////
/**
* @ingroup platform_packet
* Get the CAPWAP RID 
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_capwap_rid(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get CAPWAP flags 
* @return value if existing, 0x0 otherwise.
*/
uint16_t platform_packet_get_capwap_flags(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get CAPWAP WBID 
* @return value if existing, 0x0 otherwise.
*/
uint8_t platform_packet_get_capwap_wbid(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the CAPWAP RID
*/
void platform_packet_set_capwap_rid(datapacket_t* pkt, uint8_t rid);
/**
* @ingroup platform_packet
* Set a new value for the CAPWAP FLAGS
*/
void platform_packet_set_capwap_flags(datapacket_t* pkt, uint16_t flags);
/**
* @ingroup platform_packet
* Set a new value for the CAPWAP WBID
*/
void platform_packet_set_capwap_wbid(datapacket_t* pkt, uint8_t wbid);

/**
* @ingroup platform_packet
* Pop CAPWAP header
*/
void platform_packet_pop_capwap(datapacket_t* pkt);
/**
* @ingroup platform_packet
* Push CAPWAP header
*/
void platform_packet_push_capwap(datapacket_t* pkt);

/////////////////
// IEEE802.11 //
///////////////
/**
* @ingroup platform_packet
* Get IEEE802.11 fc field
*/
uint16_t platform_packet_get_ieee80211_fc(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get IEEE802.11 type field
*/
uint8_t platform_packet_get_ieee80211_type(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get IEEE802.11 subtype field
*/
uint8_t platform_packet_get_ieee80211_subtype(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get IEEE802.11 direction field
*/
uint8_t platform_packet_get_ieee80211_direction(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get IEEE802.11 address 1 field
*/
uint64_t platform_packet_get_ieee80211_address_1(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get IEEE802.11 address 2 field
*/
uint64_t platform_packet_get_ieee80211_address_2(datapacket_t *const pkt);
/**
* @ingroup platform_packet
* Get IEEE802.11 address 3 field
*/
uint64_t platform_packet_get_ieee80211_address_3(datapacket_t *const pkt);

/**
* @ingroup platform_packet
* Set a new value for the IEEE802.11 fc 
*/
void platform_packet_set_ieee80211_fc(datapacket_t* pkt, uint16_t fc);
/**
* @ingroup platform_packet
* Set a new value for the IEEE802.11 type
*/
void platform_packet_set_ieee80211_type(datapacket_t* pkt, uint8_t type);
/**
* @ingroup platform_packet
* Set a new value for the IEEE802.11 subtype
*/
void platform_packet_set_ieee80211_subtype(datapacket_t* pkt, uint8_t subtype);
/**
* @ingroup platform_packet
* Set a new value for the IEEE802.11 direction
*/
void platform_packet_set_ieee80211_direction(datapacket_t* pkt, uint8_t direction);
/**
* @ingroup platform_packet
* Set a new value for the IEEE802.11 address 1
*/
void platform_packet_set_ieee80211_address_1(datapacket_t* pkt, uint64_t address_1);
/**
* @ingroup platform_packet
* Set a new value for the IEEE802.11 address 2
*/
void platform_packet_set_ieee80211_address_2(datapacket_t* pkt, uint64_t address_2);
/**
* @ingroup platform_packet
* Set a new value for the IEEE802.11 address 3 
*/
void platform_packet_set_ieee80211_address_3(datapacket_t* pkt, uint64_t address_3);

/**
* @ingroup platform_packet
* Pop IEEE802.11 header
*/
void platform_packet_pop_ieee80211(datapacket_t* pkt);
/**
* @ingroup platform_packet
* Push IEEE802.11 header
*/
void platform_packet_push_ieee80211(datapacket_t* pkt);

//C++ extern C
ROFL_END_DECLS

#endif //PLATFORM_PACKET_HOOKS
