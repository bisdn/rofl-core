/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_ACTION_H__
#define __OF1X_ACTION_H__

#include <inttypes.h> 
#include <string.h> 
#include <stdbool.h>
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
*/

//Fwd decl
struct datapacket;

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
	OF1X_AT_POP_PPPOE,			/* Pop the PPPoE header */
	OF1X_AT_POP_PBB,			/* Pop the PBB header */
	OF1X_AT_POP_CAPWAP,			/* Pop the CAPWAP Header */
	OF1X_AT_POP_IEEE80211,			/* Pop the IEEE80211 Header */
 
	/*
	* Push: first PBB, PPP, PPPoE, MPLS, VLAN
	*/
	OF1X_AT_PUSH_PBB,			/* Push a new PBB header */
	OF1X_AT_PUSH_PPPOE,			/* Push a new PPPoE header */
	OF1X_AT_PUSH_GTP,			/* Push a new GTP header */
	OF1X_AT_PUSH_MPLS,			/* Push a new MPLS tag */
	OF1X_AT_PUSH_VLAN,			/* Push a new VLAN tag */
	OF1X_AT_PUSH_CAPWAP,			/* Push a new CAPWAP Header */
	OF1X_AT_PUSH_IEEE80211,			/* Push a new IEEE80211 Header */
 
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
	OF1X_AT_SET_FIELD_GTP_TEID,		/* GTP TEID */

	OF1X_AT_SET_FIELD_CAPWAP_RID,
	OF1X_AT_SET_FIELD_CAPWAP_FLAGS,
	OF1X_AT_SET_FIELD_CAPWAP_WBID,

	OF1X_AT_SET_FIELD_IEEE80211_FC,
	OF1X_AT_SET_FIELD_IEEE80211_TYPE,
	OF1X_AT_SET_FIELD_IEEE80211_SUBTYPE,
	OF1X_AT_SET_FIELD_IEEE80211_DIRECTION,
	OF1X_AT_SET_FIELD_IEEE80211_ADDRESS_1,
	OF1X_AT_SET_FIELD_IEEE80211_ADDRESS_2,
	OF1X_AT_SET_FIELD_IEEE80211_ADDRESS_3,

	/* Add more set fields here... */

	//Groups		
	OF1X_AT_GROUP,				/* Apply group. */

	//Experimenter
	OF1X_AT_EXPERIMENTER,	

	OF1X_AT_OUTPUT,			 	/* Output to switch port. */
}of1x_packet_action_type_t;

#define OF1X_AT_NUMBER OF1X_AT_OUTPUT+1 

/**
* @ingroup core_of1x 
* Actions enumeration for bitmap usage, in the order defined in OF12 and OF13, plus extensions. This is ONLY
* used in the table capabilities bitmap to prevent overflow of the bitmap 
*/
enum of12p_action_type {
	OF12PAT_OUTPUT 		= 0, 	/* Output to switch port. */
	OF12PAT_COPY_TTL_OUT 	= 11, 	/* Copy TTL "outwards" -- from next-to-outermost to outermost */
	OF12PAT_COPY_TTL_IN 	= 12, 	/* Copy TTL "inwards" -- from outermost to next-to-outermost */
	OF12PAT_SET_MPLS_TTL 	= 15, 	/* MPLS TTL */
	OF12PAT_DEC_MPLS_TTL 	= 16, 	/* Decrement MPLS TTL */
	OF12PAT_PUSH_VLAN 	= 17, 	/* Push a new VLAN tag */
	OF12PAT_POP_VLAN 	= 18, 	/* Pop the outer VLAN tag */
	OF12PAT_PUSH_MPLS 	= 19, 	/* Push a new MPLS tag */
	OF12PAT_POP_MPLS 	= 20, 	/* Pop the outer MPLS tag */
	OF12PAT_SET_QUEUE 	= 21, 	/* Set queue id when outputting to a port */
	OF12PAT_GROUP 		= 22, 	/* Apply group. */
	OF12PAT_SET_NW_TTL 	= 23, 	/* IP TTL. */
	OF12PAT_DEC_NW_TTL 	= 24, 	/* Decrement IP TTL. */
	OF12PAT_SET_FIELD 	= 25, 	/* Set a header field using OXM TLV format. */
	OF12PAT_PUSH_PBB 	= 26, 	/* Push PBB service tag (I-TAG) */
	OF12PAT_POP_PBB 	= 27, 	/* Pop PBB service tag (I-TAG) */

	/* Extensions */
	OF12PAT_PUSH_PPPOE,		/* Push a new PPPoE tag */
	OF12PAT_POP_PPPOE,		/* Pop the PPPoE tag */
	OF12PAT_PUSH_PPP,		/* Push a new PPP tag */
	OF12PAT_POP_PPP,		/* Pop the PPP tag */
	OF12PAT_EXPERIMENTER	= 0xFFFF
};


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
	wrap_uint_t field;

	//group
	struct of1x_group* group;
	
	//DLL
	struct of1x_packet_action* prev;
	struct of1x_packet_action* next;

	/* Fast validation flags */
	//Bitmap of required OF versions
	of1x_ver_req_t ver_req; 
}of1x_packet_action_t;

/**
* @ingroup core_of1x 
* Action group (apply-actions) structure
*/
typedef struct{

	//Number of actions in the list
	unsigned int num_of_actions;	

	//Double linked list
	of1x_packet_action_t* head;
	of1x_packet_action_t* tail;
	
	//Number of outputs in the action list
	unsigned int num_of_output_actions;
	
	/* Fast validation flags */
	//Bitmap of required OF versions
	of1x_ver_req_t ver_req; 
}of1x_action_group_t;

/**
* @ingroup core_of1x 
* Write actions structure
*/
typedef struct{
	//Presence of action flag => type == 0
	of1x_packet_action_t write_actions[OF1X_AT_NUMBER];
	
	//Number of actions. Merely for dumping and to skip unnecessary loop iterations
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
* Each field set consitutes an action per-se. 
*/
of1x_packet_action_t* of1x_init_packet_action(/*const struct of1x_switch* sw,*/of1x_packet_action_type_t type, wrap_uint_t field, of1x_packet_action_t* prev, of1x_packet_action_t* next);

/**
* @ingroup core_of1x 
* Destroys packet action (OF action)
*/
void of1x_destroy_packet_action(of1x_packet_action_t* action);

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


//Apply actions
void __of1x_process_apply_actions(const struct of1x_switch* sw, const unsigned int table_id, struct datapacket* pkt, const of1x_action_group_t* apply_actions_group, bool replicate_pkts);

//Write actions data structure management
/*
* Init a write actions group
*/
void __of1x_init_packet_write_actions(struct datapacket *const pkt);


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
void of1x_set_packet_action_on_write_actions(of1x_write_actions_t* write_actions, of1x_packet_action_t* action);

void __of1x_update_packet_write_actions(struct datapacket* pkt, const of1x_write_actions_t* entry_write_actions);
void __of1x_clear_write_actions(struct datapacket* pkt);
void __of1x_process_write_actions(const struct of1x_switch* sw, const unsigned int table_id, struct datapacket* pkt, bool replicate_pkts);

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
void __of1x_dump_write_actions(of1x_write_actions_t* write_actions_group);
void __of1x_dump_action_group(of1x_action_group_t* action_group);

//validate actions
rofl_result_t __of1x_validate_action_group(of1x_action_group_t *ag, struct of1x_group_table *gt);
rofl_result_t __of1x_validate_write_actions(of1x_write_actions_t *wa, struct of1x_group_table *gt);


//C++ extern C
ROFL_END_DECLS

#endif //OF1X_ACTION
