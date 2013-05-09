#ifndef __OF12_MATCH_H__
#define __OF12_MATCH_H__

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "rofl.h"
#include "../../../common/ternary_fields.h"
//#include "../../../common/datapacket.h"
#include "of12_packet_matches.h"

//Useful ETH_TYPE values
#define OF12_ETH_TYPE_MPLS_UNICAST 0x8847
#define OF12_ETH_TYPE_MPLS_MULTICAST 0x8848
#define OF12_ETH_TYPE_IPV4 0x0800
#define OF12_ETH_TYPE_IPV6 0x86DD
#define OF12_ETH_TYPE_PPPOE_DISCOVERY 0x8863
#define OF12_ETH_TYPE_PPPOE_SESSION 0x8864

//Useful IP_PROTO values
#define OF12_IP_PROTO_TCP 6
#define OF12_IP_PROTO_UDP 17
#define OF12_IP_PROTO_ICMPV4 1

//PPP PROTO values
#define OF12_PPP_PROTO_IP4 0x0021 

/* Defines possible matchings. This is EXPLICITELY copied from openflow.h, to simplify names, avoid collisions and add extensions */
typedef enum{
   OF12_MATCH_IN_PORT = 0,            /* Switch input port. */                // required
   OF12_MATCH_IN_PHY_PORT = 1,        /* Switch physical input port. */
   OF12_MATCH_METADATA = 2,        /* Metadata passed between tables. */
   OF12_MATCH_ETH_DST = 3,            /* Ethernet destination address. */        // required
   OF12_MATCH_ETH_SRC = 4,            /* Ethernet source address. */            // required
   OF12_MATCH_ETH_TYPE = 5,        /* Ethernet frame type. */                // required
   OF12_MATCH_VLAN_VID = 6,        /* VLAN id. */
   OF12_MATCH_VLAN_PCP = 7,        /* VLAN priority. */
   OF12_MATCH_IP_DSCP = 8,            /* IP DSCP (6 bits in ToS field). */
   OF12_MATCH_IP_ECN = 9,            /* IP ECN (2 bits in ToS field). */
   OF12_MATCH_IP_PROTO = 10,        /* IP protocol. */                        // required
   OF12_MATCH_IPV4_SRC = 11,        /* IPv4 source address. */                // required
   OF12_MATCH_IPV4_DST = 12,        /* IPv4 destination address. */            // required
   OF12_MATCH_TCP_SRC = 13,        /* TCP source port. */                    // required
   OF12_MATCH_TCP_DST = 14,        /* TCP destination port. */                // required
   OF12_MATCH_UDP_SRC = 15,        /* UDP source port. */                    // required
   OF12_MATCH_UDP_DST = 16,        /* UDP destination port. */                // required
   OF12_MATCH_SCTP_SRC = 17,        /* SCTP source port. */
   OF12_MATCH_SCTP_DST = 18,        /* SCTP destination port. */
   OF12_MATCH_ICMPV4_TYPE = 19,    /* ICMP type. */
   OF12_MATCH_ICMPV4_CODE = 20,    /* ICMP code. */
   OF12_MATCH_ARP_OP = 21,            /* ARP opcode. */
   OF12_MATCH_ARP_SPA = 22,        /* ARP source IPv4 address. */
   OF12_MATCH_ARP_TPA = 23,        /* ARP target IPv4 address. */
   OF12_MATCH_ARP_SHA = 24,        /* ARP source hardware address. */
   OF12_MATCH_ARP_THA = 25,        /* ARP target hardware address. */
   OF12_MATCH_IPV6_SRC = 26,        /* IPv6 source address. */                // required
   OF12_MATCH_IPV6_DST = 27,        /* IPv6 destination address. */            // required
   OF12_MATCH_IPV6_FLABEL = 28,    /* IPv6 Flow Label */
   OF12_MATCH_ICMPV6_TYPE = 29,    /* ICMPv6 type. */
   OF12_MATCH_ICMPV6_CODE = 30,    /* ICMPv6 code. */
   OF12_MATCH_IPV6_ND_TARGET = 31,    /* Target address for ND. */
   OF12_MATCH_IPV6_ND_SLL = 32,    /* Source link-layer for ND. */
   OF12_MATCH_IPV6_ND_TLL = 33,    /* Target link-layer for ND. */
   OF12_MATCH_MPLS_LABEL = 34,        /* MPLS label. */
   OF12_MATCH_MPLS_TC = 35,        /* MPLS TC. */
 
  /* PPP/PPPoE related extensions */
   OF12_MATCH_PPPOE_CODE = 36,        /* PPPoE code */
   OF12_MATCH_PPPOE_TYPE = 37,        /* PPPoE type */
   OF12_MATCH_PPPOE_SID = 38,        /* PPPoE session id */
   OF12_MATCH_PPP_PROT = 39,        /* PPP protocol */

   /* max value */
   OF12_MATCH_MAX,
} of12_match_type_t;


struct of12_match{
	
	//Type
	of12_match_type_t type;

	//Ternary value
	utern_t* value;
	
	//Previous entry
	struct of12_match* prev;
	
	//Next entry
	struct of12_match* next;
};
typedef struct of12_match of12_match_t;


/* Match group, using a double-linked-list */
typedef struct{
	//Double linked list
	of12_match_t* head;
	of12_match_t* tail;
}of12_match_group_t;


/*
*
* Initializers per match 
*
*/

//C++ extern C
ROFL_BEGIN_DECLS

//Phy
/**
* @brief Create an PORT_IN match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_port_in_match(of12_match_t* prev, of12_match_t* next, uint32_t value);
/**
* @brief Create an PHY_PORT_IN match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_port_in_phy_match(of12_match_t* prev, of12_match_t* next, uint32_t value);

/* TODO: add metadata */

//ETHERNET
/**
* @brief Create an ETH_DST match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_eth_dst_match(of12_match_t* prev, of12_match_t* next, uint64_t value, uint64_t mask);
/**
* @brief Create an ETH_SRC match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_eth_src_match(of12_match_t* prev, of12_match_t* next, uint64_t value, uint64_t mask);
/**
* @brief Create an ETH_TYPE match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_eth_type_match(of12_match_t* prev, of12_match_t* next, uint16_t value);

//8021.q
/**
* @brief Create an VLAN_VID match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_vlan_vid_match(of12_match_t* prev, of12_match_t* next, uint16_t value, uint16_t mask);
/**
* @brief Create an VLAN_PCP match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_vlan_pcp_match(of12_match_t* prev, of12_match_t* next, uint8_t value);

//MPLS
/**
* @brief Create an MPLS_LABEL match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_mpls_label_match(of12_match_t* prev, of12_match_t* next, uint32_t value);
/**
* @brief Create an MPLS_TC match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_mpls_tc_match(of12_match_t* prev, of12_match_t* next, uint8_t value);

//PPPoE
/**
* @brief Create an PPPOE_CODE match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_pppoe_code_match(of12_match_t* prev, of12_match_t* next, uint8_t value);
/**
* @brief Create an PPPOE_TYPE match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_pppoe_type_match(of12_match_t* prev, of12_match_t* next, uint8_t value);
/**
* @brief Create an PPPOE_SESSION match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_pppoe_session_match(of12_match_t* prev, of12_match_t* next, uint16_t value);

//PPP
/**
* @brief Create an PPP_PROTO match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_ppp_prot_match(of12_match_t* prev, of12_match_t* next, uint16_t value);


//IP
/**
* @brief Create an IP_PROTO match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_ip_proto_match(of12_match_t* prev, of12_match_t* next, uint8_t value);
/**
* @brief Create an IPC_ECN match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_ip_ecn_match(of12_match_t* prev, of12_match_t* next, uint8_t value);
/**
* @brief Create an IP_DSCP match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_ip_dscp_match(of12_match_t* prev, of12_match_t* next, uint8_t value);

//IPv4
/**
* @brief Create an IP4_SRC match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_ip4_src_match(of12_match_t* prev, of12_match_t* next, uint32_t value, uint32_t mask);
/**
* @brief Create an IP4_DST match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_ip4_dst_match(of12_match_t* prev, of12_match_t* next, uint32_t value, uint32_t mask);

//TCP
/**
* @brief Create an TCP_SRC match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_tcp_src_match(of12_match_t* prev, of12_match_t* next, uint16_t value);
/**
* @brief Create an TCP_DST match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_tcp_dst_match(of12_match_t* prev, of12_match_t* next, uint16_t value);

//UDP
/**
* @brief Create an UDP_SRC match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_udp_src_match(of12_match_t* prev, of12_match_t* next, uint16_t value);
/**
* @brief Create an UDP_DST match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_udp_dst_match(of12_match_t* prev, of12_match_t* next, uint16_t value);

//ICMPv4
/**
* @brief Create an ICMPv4_TYPE match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_icmpv4_type_match(of12_match_t* prev, of12_match_t* next, uint8_t value);
/**
* @brief Create an ICMPv4_CODE match 
* @ingroup core_of12 
*/
of12_match_t* of12_init_icmpv4_code_match(of12_match_t* prev, of12_match_t* next, uint8_t value);

//IPv6 && ICMv6
//TODO
//Add more here...

/** 
* @brief Destroys whichever match previously created using of12_init_match_*() 
* @ingroup core_of12 
*/
void of12_destroy_match(of12_match_t* match);





#if 0
/* match group */
void of12_init_match_group(of12_match_group_t* group);
void of12_destroy_match_group(of12_match_group_t* group);
void of12_match_group_push_match(of12_match_group_t* group, of12_match_t* match);
#endif

/* match group */
void __of12_init_match_group(of12_match_group_t* group);
void __of12_destroy_match_group(of12_match_group_t* group);
void __of12_match_group_push_back(of12_match_group_t* group, of12_match_t* match);

/* Push match at the end of the match */
rofl_result_t __of12_add_match(of12_match_t* root_match, of12_match_t* add_match);

/* 
* Single Match copy
* TODO: deprecate this in favour of match_group
*/
of12_match_t* __of12_copy_match(of12_match_t* match);

/* 
* Whole (linked list) Match copy
* TODO: deprecate this in favour of match_group
*/
of12_match_t* __of12_copy_matches(of12_match_t* matches);


/* 
* Get alike match 
*/ 
of12_match_t* __of12_get_alike_match(of12_match_t* match1, of12_match_t* match2);

/*
* Matching 
*/
bool __of12_equal_matches(of12_match_t* match1, of12_match_t* match2);
bool __of12_is_submatch(of12_match_t* sub_match, of12_match_t* match);
bool __of12_check_match(const of12_packet_matches_t* pkt, of12_match_t* it);

/*
* Dumping
*/
void __of12_dump_packet_matches(of12_packet_matches_t *const pkt);

void __of12_dump_matches(of12_match_t* matches);
void __of12_full_dump_matches(of12_match_t* matches);

//C++ extern C
ROFL_END_DECLS

#endif //OF12_MATCH
