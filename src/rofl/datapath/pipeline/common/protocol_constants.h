/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __PROTOCOL_CONSTANTS_H__
#define __PROTOCOL_CONSTANTS_H__

#include <stdint.h>
#include "endianness.h"

/*
 * This header file defines some network protocol header constants 
 */

/*
* Useful constants (endianness dependant)
*/
#if defined(BIG_ENDIAN_DETECTED)
	//Useful ETH_TYPE values
	#define ETH_TYPE_MPLS_UNICAST		0x8847
	#define ETH_TYPE_MPLS_MULTICAST		0x8848
	#define ETH_TYPE_8021Q			0x8100 
	#define ETH_TYPE_IPV4			0x0800
	#define ETH_TYPE_IPV6			0x86DD
	#define ETH_TYPE_PPPOE_DISCOVERY	0x8863
	#define ETH_TYPE_PPPOE_SESSION		0x8864
	#define ETH_TYPE_ARP			0x0806
	#define ETH_TYPE_PBB			0x88E7
	
	//VLAN values
	#define VLAN_CTAG_ETHER 0x8100
 	#define VLAN_STAG_ETHER 0x88a8
 	#define VLAN_ITAG_ETHER 0x88e7
	//MPLS values

	//PPP values
	#define PPP_PROT_PADDING 	0x0001 // 0x00 0x01 in network byte order
	#define PPP_PROT_LCP 		0xc021 // 0xc0 0x21 in network byte order
	#define PPP_PROT_PAP 		0xc023 // 0xc0 0x23 in network byte order
	#define PPP_PROT_LQR 		0xc025 // 0xc0 0x25 in network byte order
	#define PPP_PROT_CHAP 		0xc223 // 0xc2 0x23 in network byte order
	#define PPP_PROT_EAP 		0xc227 // 0xc2 0x27 in network byte order
	#define PPP_PROT_IPCP 		0x8021 // 0x80 0x21 in network byte order
	#define PPP_PROT_IPV4 		0x0021 // 0x00 0x21 in network byte order
	#define PPP_PROT_IPV6CP 	0x8057 // 0x80 0x57 in network byte order
	#define PPP_PROT_IPV6 		0x0057 // 0x00 0x57 in network byte order
	#define PPP_PROT_CCP 		0x80fd // 0x80 0xfd in network byte order
	
	//Useful UDP destination port values
	#define UDP_DST_PORT_GTPC		0x084B //2123
	#define UDP_DST_PORT_GTPU		0x0868 //2152

#elif defined(LITTLE_ENDIAN_DETECTED)
	//Useful ETH_TYPE values
	#define ETH_TYPE_MPLS_UNICAST		0x4788
	#define ETH_TYPE_MPLS_MULTICAST		0x4888
	#define ETH_TYPE_8021Q			0x0081 
	#define ETH_TYPE_IPV4			0x0008
	#define ETH_TYPE_IPV6			0xDD86
	#define ETH_TYPE_PPPOE_DISCOVERY	0x6388
	#define ETH_TYPE_PPPOE_SESSION		0x6488
	#define ETH_TYPE_ARP			0x0608
	#define ETH_TYPE_PBB			0xE788
	
	//VLAN values
	#define VLAN_CTAG_ETHER 0x0081
 	#define VLAN_STAG_ETHER 0xa888
 	#define VLAN_ITAG_ETHER 0xe788
	//MPLS values
	
	//PPP values
	#define PPP_PROT_PADDING 	0x0100 // 0x00 0x01 in network byte order
	#define PPP_PROT_LCP 		0x21c0 // 0xc0 0x21 in network byte order
	#define PPP_PROT_PAP 		0x23c0 // 0xc0 0x23 in network byte order
	#define PPP_PROT_LQR 		0x25c0 // 0xc0 0x25 in network byte order
	#define PPP_PROT_CHAP 		0x23c2 // 0xc2 0x23 in network byte order
	#define PPP_PROT_EAP 		0x27c2 // 0xc2 0x27 in network byte order
	#define PPP_PROT_IPCP 		0x2180 // 0x80 0x21 in network byte order
	#define PPP_PROT_IPV4 		0x2100 // 0x00 0x21 in network byte order
	#define PPP_PROT_IPV6CP 	0x5780 // 0x80 0x57 in network byte order
	#define PPP_PROT_IPV6 		0x5700 // 0x00 0x57 in network byte order
	#define PPP_PROT_CCP 		0xfd80 // 0x80 0xfd in network byte order

	//Useful UDP destination port values
	#define UDP_DST_PORT_GTPC		0x4B08 //2123
	#define UDP_DST_PORT_GTPU		0x6808 //2152
#else
	#error Unknwon endianness
#endif

/*
* Endianness agnostic (8bit or less)
*/
// we use this value to compare (grater or smaller)
// because of that we must do the comparison in HBO
// and therefore we don't need to define it twice
#define LLC_DELIMITER_HBO 0x0600

//Useful IP_PROTO values
#define IP_PROTO_TCP		6
#define IP_PROTO_UDP		17
#define IP_PROTO_SCTP		132
#define IP_PROTO_ICMPV4		1
#define IP_PROTO_ICMPV6		58

//PPP PROTO values
#define PPP_PROTO_IP4		0x21 
#define PPP_PROTO_IP6		0x57


#endif //__PROTOCOL_CONSTANTS_H__
