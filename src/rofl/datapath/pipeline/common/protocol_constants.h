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

	//Useful UDP destination port values
	#define UDP_DST_PORT_GTPC		0x4B08 //2123
	#define UDP_DST_PORT_GTPU		0x6808 //2152
#else
	#error Unknwon endianness
#endif

/*
* Endianness agnostic (8bit or less)
*/

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
