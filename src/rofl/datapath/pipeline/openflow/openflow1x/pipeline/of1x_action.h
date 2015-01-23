/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_ACTION_H__
#define __OF1X_ACTION_H__

#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "rofl.h"
#include "of1x_utils.h"
#include "../../../common/ternary_fields.h"

/**
* @file of1x_action.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief OpenFlow v1.0, 1.2 and 1.3.2 actions
*
* Actions are a part of a of1x_flow_entry. The typical
* workflow for creation of actions is:
*
* Apply actions(pseudo-code):
* @code
*
* group = of1x_init_action_group()
*
* for(i in openflow_msg->apply_actions)
*   action = of1x_init_packet_action(i->type, i->field)
*   of1x_push_packet_action_to_group(group, action)
*
*   //Action can never be accessed or freed now!
*   //This is forbidded
*   //of1x_destroy_packet_action(action)
*   //action->type = something; //R/W access is also forbidden
*
* //To release resources
* of1x_destroy_action_group(group)
* @endcode
*
* Write actions(pseudo-code):
* @code
*
* write_actions = of1x_init_write_actions()
*
* for(i in openflow_msg->write_actions)
*   action = of1x_init_packet_action(i->type, i->field)
*   of1x_set_packet_action_on_write_actions(write_actions,action);
*   //Action must be freed(set only copies the action)
*   of1x_destroy_packet_action(action)
*
*
* //To release resources
* of1x_destroy_write_actions(group)
* @endcode
*
* Note regarding endianness:
* Conforming the convention that the pipeline works in Network Byte Order
* the matches need to to be initialized in NBO (Big Endian).
* This applies to the values comming from the packet (eth_src, eth_dst, ...)
* and NOT to the ones that are external to it:
*  - port_in
*  - port_phy_in
*  - metadata
*
* There is an special alignment for non complete values as
*  - mac addresses ( 6 bytes)
*  - vlan vid      (12 bits )
*  - mpls label    (20 bits )
*  - pbb isid      ( 3 bytes)
* More information on these alignments can be found in the
* pipeline general documentation
*
*/

//Fwd decl
struct datapacket;
struct of1x_flow_table;
/**
* @ingroup core_of1x
* Actions over a packet as per defined in OF10, OF12, OF13. Set operations are converted to actions
* for pipeline simplification. This is comming from of1x_action_type enum.
*
* @warning values are MODIFIED from OF specification and are reorder! Order matters,
* as when write actions are executed are done in order.
*/
typedef enum{
	//No action. This MUST always exist and the value MUST be 0
	OF1X_AT_NO_ACTION= 0,			/* NO action/EMPTY action. */

	//Copy TTL inwards
	OF1X_AT_COPY_TTL_IN,			/* Copy TTL "inwards" -- from outermost to next-to-outermost */

	/*
	* Pop: first VLAN, MPLS, PPPoE, PPP, PBB
	*/
	OF1X_AT_POP_VLAN,			/* Pop the outer VLAN tag */
	OF1X_AT_POP_MPLS,			/* Pop the outer MPLS tag */
	OF1X_AT_POP_GTP,			/* Pop the GTP header */
	OF1X_AT_POP_CAPWAP,			/* Pop the CAPWAP header */
	OF1X_AT_POP_PPPOE,			/* Pop the PPPoE header */
	OF1X_AT_POP_PBB,			/* Pop the PBB header */
	OF1X_AT_POP_WLAN,			/* Pop the WLAN header */
	OF1X_AT_POP_GRE,			/* Pop the GRE header */

	/*
	* Push: first PBB, PPP, PPPoE, MPLS, VLAN
	*/
	OF1X_AT_PUSH_GRE,			/* Push a new GRE header */
	OF1X_AT_PUSH_WLAN,			/* Push a new WLAN header */
	OF1X_AT_PUSH_PBB,			/* Push a new PBB header */
	OF1X_AT_PUSH_PPPOE,			/* Push a new PPPoE header */
	OF1X_AT_PUSH_CAPWAP,		/* Push a new CAPWAP header */
	OF1X_AT_PUSH_GTP,			/* Push a new GTP header */
	OF1X_AT_PUSH_MPLS,			/* Push a new MPLS tag */
	OF1X_AT_PUSH_VLAN,			/* Push a new VLAN tag */

	//Copy ttl outwards
	OF1X_AT_COPY_TTL_OUT, 			/* Copy TTL "outwards" -- from next-to-outermost to outermost */

	//Decrement TTL
	OF1X_AT_DEC_NW_TTL,			/* Decrement IP TTL. */
	OF1X_AT_DEC_MPLS_TTL,			/* Decrement MPLS TTL */

	//Set fields
	OF1X_AT_SET_MPLS_TTL,			/* MPLS TTL */
	OF1X_AT_SET_NW_TTL,			/* IP TTL. */
	OF1X_AT_SET_QUEUE,			/* Set queue id when outputting to a port */

	OF1X_AT_SET_FIELD_ETH_DST,   		/* Ethernet destination address. */
	OF1X_AT_SET_FIELD_ETH_SRC,   		/* Ethernet source address. */
	OF1X_AT_SET_FIELD_ETH_TYPE,  		/* Ethernet frame type. */

	OF1X_AT_SET_FIELD_MPLS_LABEL,		/* MPLS label. */
	OF1X_AT_SET_FIELD_MPLS_TC,   	   	/* MPLS TC. */
	OF1X_AT_SET_FIELD_MPLS_BOS,   	   	/* MPLS BoS flag */

	OF1X_AT_SET_FIELD_VLAN_VID,  		/* VLAN id. */
	OF1X_AT_SET_FIELD_VLAN_PCP,  		/* VLAN priority. */
	OF1X_AT_SET_FIELD_ARP_OPCODE,		/* ARP opcode */

	OF1X_AT_SET_FIELD_ARP_SHA,		/* ARP source hardware address */
	OF1X_AT_SET_FIELD_ARP_SPA,		/* ARP source protocol address */
	OF1X_AT_SET_FIELD_ARP_THA,		/* ARP target hardware address */
	OF1X_AT_SET_FIELD_ARP_TPA,		/* ARP target protocol address */

	/* OF10 only */
	OF1X_AT_SET_FIELD_NW_PROTO,		/* Network layer proto/arp code */
	OF1X_AT_SET_FIELD_NW_SRC,		/* Source network address */
	OF1X_AT_SET_FIELD_NW_DST,		/* Destination network address */
	/* End of OF10 only */


	OF1X_AT_SET_FIELD_IP_DSCP,   		/* IP DSCP (6 bits in ToS field). */
	OF1X_AT_SET_FIELD_IP_ECN,    		/* IP ECN (2 bits in ToS field). */
	OF1X_AT_SET_FIELD_IP_PROTO,  		/* IP protocol. */

	OF1X_AT_SET_FIELD_IPV4_SRC,  		/* IPv4 source address. */
	OF1X_AT_SET_FIELD_IPV4_DST,  		/* IPv4 destination address. */
	OF1X_AT_SET_FIELD_IPV6_SRC,		/* IPv6 source address */

	OF1X_AT_SET_FIELD_IPV6_DST,		/* IPv6 destination address */
	OF1X_AT_SET_FIELD_IPV6_FLABEL,		/* IPv6 flow label */
	OF1X_AT_SET_FIELD_IPV6_ND_TARGET,	/* IPv6 Neighbour Discovery target */
	OF1X_AT_SET_FIELD_IPV6_ND_SLL,		/* IPv6 ND source link level */
	OF1X_AT_SET_FIELD_IPV6_ND_TLL,		/* IPv6 ND target link level */
	OF1X_AT_SET_FIELD_IPV6_EXTHDR,		/* IPv6 Extension pseudo header */

	OF1X_AT_SET_FIELD_TCP_SRC,   		/* TCP source port. */
	OF1X_AT_SET_FIELD_TCP_DST,   		/* TCP destination port. */

	OF1X_AT_SET_FIELD_UDP_SRC,   		/* UDP source port. */
	OF1X_AT_SET_FIELD_UDP_DST,   		/* UDP destination port. */

	OF1X_AT_SET_FIELD_SCTP_SRC,   		/* SCTP source port. */
	OF1X_AT_SET_FIELD_SCTP_DST,   		/* SCTP destination port. */

	/* OF10 only */
	OF1X_AT_SET_FIELD_TP_SRC,		/* Trans. protocol (TCP/UDP) src port */
	OF1X_AT_SET_FIELD_TP_DST,		/* Trans. protocol (TCP/UDP) dst port */
	/* End of OF10 only */

	OF1X_AT_SET_FIELD_ICMPV4_TYPE,		/* ICMP type. */
	OF1X_AT_SET_FIELD_ICMPV4_CODE,		/* ICMP code. */

	OF1X_AT_SET_FIELD_ICMPV6_TYPE,		/* ICMPv6 type */
	OF1X_AT_SET_FIELD_ICMPV6_CODE,		/* ICMPv6 code */

	OF1X_AT_SET_FIELD_PBB_ISID,		/* PBB ISID field */
	OF1X_AT_SET_FIELD_TUNNEL_ID,		/* Tunnel id */

	/*
	* Extensions
	*/

	OF1X_AT_SET_FIELD_PPPOE_CODE,		/* PPPoE code */
	OF1X_AT_SET_FIELD_PPPOE_TYPE,		/* PPPoE type */
	OF1X_AT_SET_FIELD_PPPOE_SID, 	   	/* PPPoE session id */
	OF1X_AT_SET_FIELD_PPP_PROT,  	   	/* PPP protocol */

	OF1X_AT_SET_FIELD_GTP_MSG_TYPE,		/* GTP message type */
	OF1X_AT_SET_FIELD_GTP_TEID,			/* GTP TEID */

	OF1X_AT_SET_FIELD_CAPWAP_WBID,		/* CAPWAP wbid */
	OF1X_AT_SET_FIELD_CAPWAP_RID,		/* CAPWAP rid */
	OF1X_AT_SET_FIELD_CAPWAP_FLAGS,		/* CAPWAP flags */

	OF1X_AT_SET_FIELD_WLAN_FC,			/* WLAN fc */
	OF1X_AT_SET_FIELD_WLAN_TYPE,		/* WLAN type */
	OF1X_AT_SET_FIELD_WLAN_SUBTYPE,		/* WLAN subtype */
	OF1X_AT_SET_FIELD_WLAN_DIRECTION,	/* WLAN direction */
	OF1X_AT_SET_FIELD_WLAN_ADDRESS_1,	/* WLAN address 1 */
	OF1X_AT_SET_FIELD_WLAN_ADDRESS_2,	/* WLAN address 2 */
	OF1X_AT_SET_FIELD_WLAN_ADDRESS_3,	/* WLAN address 3 */

	OF1X_AT_SET_FIELD_GRE_VERSION,		/* GRE version */
	OF1X_AT_SET_FIELD_GRE_PROT_TYPE,	/* GRE protocol type */
	OF1X_AT_SET_FIELD_GRE_KEY,			/* GRE key */

	/* Add more set fields here... */

	//Groups
	OF1X_AT_GROUP,				/* Apply group. */

	//Experimenter
	OF1X_AT_EXPERIMENTER,

	OF1X_AT_OUTPUT,			 	/* Output to switch port. */
}of1x_packet_action_type_t;

#define OF1X_AT_NUMBER OF1X_AT_OUTPUT+1

//Make sure we are not exceeding the bitmap size
#if OF1X_AT_NUMBER >= 128
	#error Number of actions beyond 128 not supported by bitmap128_t. Implement bitmap256_t.
#endif

/**
* @ingroup core_of1x
* Special port numbers, according to OF1X (of1xp_port_no )
*/
enum of1x_port_numbers {

	/* Maximum number of physical switch ports. */
	OF1X_PORT_MAX = 0xffffff00,

	/* Fake output "ports". */
	OF1X_PORT_IN_PORT = 0xfffffff8,  /* Send the packet out the input port.  This
	virtual port must be explicitly used
	in order to send back out of the input
	port. */

	OF1X_PORT_TABLE      = 0xfffffff9,	/* Submit the packet to the first flow table
						NB: This destination port can only be
						used in packet-out messages. */
	OF1X_PORT_NORMAL     = 0xfffffffa,	/* Process with normal L2/L3 switching. */
	OF1X_PORT_FLOOD      = 0xfffffffb,	/* All physical ports in VLAN, except input
						port and those blocked or link down. */
	OF1X_PORT_ALL        = 0xfffffffc,	/* All physical ports except input port. */
	OF1X_PORT_CONTROLLER = 0xfffffffd,  	/* Send to controller. */
	OF1X_PORT_LOCAL      = 0xfffffffe,  	/* Local OpenFlow "port". */
	OF1X_PORT_ANY        = 0xffffffff	/* Wildcard port used only for flow mod
						(delete) and flow stats requests. Selects
						all flows regardless of output port
						(including flows with no output port). */
};


//fwd declaration
struct of1x_group;

/**
* @ingroup core_of1x
* Packet action abstraction data structure
*/
typedef struct of1x_packet_action{
	//Type and value(for set fields and push)
	of1x_packet_action_type_t type;

	//Field (set field)
	wrap_uint_t __field;

	//miss-send-len for OUTPUT actions only
	uint16_t send_len;

	//group
	struct of1x_group* group;

	//DLL
	struct of1x_packet_action* next;

	/* Fast validation flags */
	//Bitmap of required OF versions
	of1x_ver_req_t ver_req;
}of1x_packet_action_t;

/**
* @ingroup core_of1x
* Action group (apply-actions) structure
*/
typedef struct of1x_action_group{

	//bitmap of actions
	bitmap128_t bitmap;

	//Number of actions in the list
	unsigned int num_of_actions;

	//Double linked list
	of1x_packet_action_t* head;
	of1x_packet_action_t* tail;

	//Number of outputs in the action list
	unsigned int num_of_output_actions;

	//Contains output to TABLE
	bool has_output_table;

	/* Fast validation flags */
	//Bitmap of required OF versions
	of1x_ver_req_t ver_req;
}of1x_action_group_t;

/**
* @ingroup core_of1x
* Write actions structure
*/
typedef struct{

	//bitmap of actions
	bitmap128_t bitmap;

	//Write actions 0...OF1X_AT_NUMBER-1 at the very beginning of the array
	of1x_packet_action_t actions[OF1X_AT_NUMBER];

	//Number of output actions.
	unsigned int num_of_actions;
	unsigned int num_of_output_actions;

	/* Fast validation flags */
	//Bitmap of required OF versions
	of1x_ver_req_t ver_req;
}of1x_write_actions_t;

//Fwd declaration
struct of1x_switch;
struct of1x_group_table;

/*
*
* Function prototypes
*
*/

//C++ extern C
ROFL_BEGIN_DECLS

//Action
/**
* @ingroup core_of1x
* Initializes a packet action (OF action)
*
* @param field union containing 8, 16, 32, 64 and 128 bit action field (e.g. port_num in output actions, header value in set field actions). Put if not used in this action type.
* @param output_send_len Optional send to controller send len; leave to 0x0 if not used.
* @warning The value inside the action field must be in Host Byte Order
*/
of1x_packet_action_t* of1x_init_packet_action(of1x_packet_action_type_t type, wrap_uint_t field, uint16_t output_send_len);

/**
* @ingroup core_of1x
* Destroys packet action (OF action)
*/
void of1x_destroy_packet_action(of1x_packet_action_t* action);

//Getters for the values of the

//8 bit
static inline
uint8_t __of1x_get_packet_action_field8(const of1x_packet_action_t* action, bool raw_nbo){

	if(raw_nbo)
		return action->__field.u8;

	switch(action->type){
		case OF1X_AT_SET_FIELD_IP_DSCP:
			return OF1X_IP_DSCP_VALUE(action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_VLAN_PCP:
			return OF1X_VLAN_PCP_VALUE(action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_MPLS_TC:
			return OF1X_MPLS_TC_VALUE(action->__field.u8);
			break;
		case OF1X_AT_SET_FIELD_IP_ECN:
		case OF1X_AT_SET_FIELD_NW_PROTO:
		case OF1X_AT_SET_FIELD_ICMPV6_TYPE:
		case OF1X_AT_SET_FIELD_ICMPV6_CODE:
		case OF1X_AT_SET_FIELD_PPPOE_CODE:
		case OF1X_AT_SET_FIELD_PPPOE_TYPE:
		case OF1X_AT_SET_MPLS_TTL:
		case OF1X_AT_SET_NW_TTL:
		case OF1X_AT_SET_FIELD_IP_PROTO:
		case OF1X_AT_SET_FIELD_ICMPV4_TYPE:
		case OF1X_AT_SET_FIELD_ICMPV4_CODE:
		case OF1X_AT_SET_FIELD_GTP_MSG_TYPE:
		case OF1X_AT_SET_FIELD_MPLS_BOS:
			return action->__field.u8;
			break;
		default:
			return 0x0;
			break;
	}
	return 0x0;
}

/**
* @ingroup core_of1x
* Retrieve the action field for 8 bit values (or less) in HOST BYTE ORDER
*
* @retval The value of the field in host byte order
*/
static inline
uint8_t of1x_get_packet_action_field8(const of1x_packet_action_t* action){
	return __of1x_get_packet_action_field8(action, false);
}


//16 bit
static inline
uint16_t __of1x_get_packet_action_field16(const of1x_packet_action_t* action, bool raw_nbo){

	if(raw_nbo)
		return action->__field.u16;

	switch(action->type){
		case OF1X_AT_SET_FIELD_ETH_TYPE:
		case OF1X_AT_SET_FIELD_ARP_OPCODE:
		case OF1X_AT_SET_FIELD_TP_SRC:
		case OF1X_AT_SET_FIELD_TP_DST:
		case OF1X_AT_SET_FIELD_TCP_SRC:
		case OF1X_AT_SET_FIELD_TCP_DST:
		case OF1X_AT_SET_FIELD_UDP_SRC:
		case OF1X_AT_SET_FIELD_UDP_DST:
		case OF1X_AT_SET_FIELD_SCTP_SRC:
		case OF1X_AT_SET_FIELD_SCTP_DST:
		case OF1X_AT_SET_FIELD_PPPOE_SID:
		case OF1X_AT_SET_FIELD_PPP_PROT:
		case OF1X_AT_POP_MPLS:
		case OF1X_AT_POP_PPPOE:
		case OF1X_AT_POP_PBB:
		case OF1X_AT_PUSH_PPPOE:
		case OF1X_AT_PUSH_MPLS:
		case OF1X_AT_PUSH_VLAN:
		case OF1X_AT_PUSH_PBB:
		case OF1X_AT_SET_FIELD_VLAN_VID:
		case OF1X_AT_SET_FIELD_GRE_VERSION:
		case OF1X_AT_SET_FIELD_GRE_PROT_TYPE:
			return NTOHB16(action->__field.u16);
			break;
		case OF1X_AT_SET_FIELD_IPV6_EXTHDR:
		default:
			return 0x0;
			break;
	}
	return 0x0;
}

/**
* @ingroup core_of1x
* Retrieve the action field for 16 bit values (or less) in HOST BYTE ORDER
*
* @retval The value of the field in host byte order
*/
static inline
uint16_t of1x_get_packet_action_field16(const of1x_packet_action_t* action){
	return __of1x_get_packet_action_field16(action, false);
}

//32 bit
static inline
uint32_t __of1x_get_packet_action_field32(const of1x_packet_action_t* action, bool raw_nbo){

	if(raw_nbo)
		return action->__field.u32;

	switch(action->type){
		case OF1X_AT_SET_FIELD_MPLS_LABEL:
			return OF1X_MPLS_LABEL_VALUE(NTOHB32(action->__field.u32));
		case OF1X_AT_SET_FIELD_IPV6_FLABEL:
			return OF1X_IP6_FLABEL_VALUE(NTOHB32(action->__field.u32));
			break;
		case OF1X_AT_SET_FIELD_IPV4_SRC:
		case OF1X_AT_SET_FIELD_IPV4_DST:
		case OF1X_AT_SET_FIELD_NW_SRC:
		case OF1X_AT_SET_FIELD_NW_DST:
		case OF1X_AT_SET_FIELD_ARP_SPA:
		case OF1X_AT_SET_FIELD_ARP_TPA:
		case OF1X_AT_SET_FIELD_GTP_TEID:
		case OF1X_AT_SET_FIELD_PBB_ISID:
		case OF1X_AT_SET_FIELD_GRE_KEY:
			return NTOHB32(action->__field.u32);
			break;
		case OF1X_AT_SET_QUEUE:
		case OF1X_AT_OUTPUT:
		case OF1X_AT_GROUP:
			return action->__field.u32;
			break;
		default:
			return 0x0;
			break;
	}
	return 0x0;
}

/**
* @ingroup core_of1x
* Retrieve the action field for 32 bit values (or less) in HOST BYTE ORDER
*
* @retval The value of the field in host byte order
*/
static inline
uint32_t of1x_get_packet_action_field32(const of1x_packet_action_t* action){
	return __of1x_get_packet_action_field32(action, false);
}

//64 bit
static inline
uint64_t __of1x_get_packet_action_field64(const of1x_packet_action_t* action, bool raw_nbo){

	if(raw_nbo)
		return action->__field.u64;

	switch(action->type){
		case OF1X_AT_SET_FIELD_PBB_ISID:
		case OF1X_AT_SET_FIELD_TUNNEL_ID:
			return action->__field.u64;
			break;
		case OF1X_AT_SET_FIELD_IPV6_ND_SLL:
		case OF1X_AT_SET_FIELD_IPV6_ND_TLL:
		case OF1X_AT_SET_FIELD_ETH_DST:
		case OF1X_AT_SET_FIELD_ETH_SRC:
		case OF1X_AT_SET_FIELD_ARP_SHA:
		case OF1X_AT_SET_FIELD_ARP_THA:
			return OF1X_MAC_VALUE(NTOHB64(action->__field.u64));
			break;
		default:
			return 0x0;
			break;
	}
	return 0x0;
}

/**
* @ingroup core_of1x
* Retrieve the action field for 64 bit values (or less) in HOST BYTE ORDER
*
* @retval The value of the field in host byte order
*/
static inline
uint64_t of1x_get_packet_action_field64(const of1x_packet_action_t* action){
	return __of1x_get_packet_action_field64(action, false);
}

//128 bit
static inline
uint128__t __of1x_get_packet_action_field128(const of1x_packet_action_t* action, bool raw_nbo){
	uint128__t tmp= {{0x00}};

	if(raw_nbo)
		return action->__field.u128;

	switch(action->type){
		case OF1X_AT_SET_FIELD_IPV6_ND_TARGET:
		case OF1X_AT_SET_FIELD_IPV6_SRC:
		case OF1X_AT_SET_FIELD_IPV6_DST:
			tmp = action->__field.u128;
			NTOHB128(tmp);
			return tmp;
			break;
		default:
			return tmp;
			break;
	}
	return tmp;
}

/**
* @ingroup core_of1x
* Retrieve the action field for 128 bit values (or less) in HOST BYTE ORDER
*
* @retval The value of the field in host byte order
*/
static inline
uint128__t of1x_get_packet_action_field128(const of1x_packet_action_t* action){
	return __of1x_get_packet_action_field128(action, false);
}

//Action group
/**
* @ingroup core_of1x
* Create an action group (apply actions)
*/
of1x_action_group_t* of1x_init_action_group(of1x_packet_action_t* actions);

/**
* @ingroup core_of1x
* Destroy an action group. This also destroys actions contained
*/
void of1x_destroy_action_group(of1x_action_group_t* group);

//Push packet action
/**
* @ingroup core_of1x
* Push an action to the group. The action can no longer be used or freed
* from outside of the library. of1x_destroy_action_group() will destroy it.
*/
void of1x_push_packet_action_to_group(of1x_action_group_t* group, of1x_packet_action_t* action);


/**
* @ingroup core_of1x
* Create a write actions group
*/
of1x_write_actions_t* of1x_init_write_actions(void);


/*
* Destroy a write_actions instance. This also destroys actions contained
*/
void __of1x_destroy_write_actions(of1x_write_actions_t* write_actions);

/**
* @ingroup core_of1x
* Set (copy) the action to the write actions. The action pointer can safely used
* outside the library, and released.
*/
rofl_result_t of1x_set_packet_action_on_write_actions(of1x_write_actions_t* write_actions, of1x_packet_action_t* action);

//Update apply/write
rofl_result_t __of1x_update_apply_actions(of1x_action_group_t** group, of1x_action_group_t* new_group);
rofl_result_t __of1x_update_write_actions(of1x_write_actions_t** group, of1x_write_actions_t* new_group);

//Checking functions
bool __of1x_write_actions_has(of1x_write_actions_t* write_actions, of1x_packet_action_type_t type, uint64_t value);
bool __of1x_apply_actions_has(const of1x_action_group_t* apply_actions_group, of1x_packet_action_type_t type, uint64_t value);

//Copy (cloning) methodssource
of1x_action_group_t* __of1x_copy_action_group(of1x_action_group_t* origin);
of1x_write_actions_t* __of1x_copy_write_actions(of1x_write_actions_t* origin);

//Dump
void __of1x_dump_write_actions(of1x_write_actions_t* write_actions_group, bool raw_nbo);
void __of1x_dump_action_group(of1x_action_group_t* action_group, bool raw_nbo);

//validate actions
rofl_result_t __of1x_validate_action_group(bitmap128_t* supported, of1x_action_group_t *ag, struct of1x_group_table *gt, bool is_pkt_out_ag);
rofl_result_t __of1x_validate_write_actions(bitmap128_t* supported, of1x_write_actions_t *wa, struct of1x_group_table *gt);


//C++ extern C
ROFL_END_DECLS

#endif //OF1X_ACTION
