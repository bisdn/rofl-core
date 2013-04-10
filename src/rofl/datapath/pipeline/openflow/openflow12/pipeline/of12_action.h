/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF12_ACTION_H__
#define __OF12_ACTION_H__

#include <inttypes.h> 
#include <string.h> 
#include <stdbool.h>
#include "rofl.h"
#include "../../../util/rofl_pipeline_utils.h"
#include "../../../common/datapacket.h"

/**
* @file of12_action.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Openflow v1.2 actions 
*
* Actions are a part of a of12_flow_entry. The typical
* workflow for creation of actions is:
*
* Apply actions(pseudo-code):
* @code
*
* group = of12_init_action_group()
*
* for(i in openflow_msg->apply_actions)
*   action = of12_init_packet_action(i->type, i->field)
*   of12_push_packet_action_to_group(group, action)
*
*   //Action can never be accessed or freed now!
*   //This is forbidded 
*   //of12_destroy_packet_action(action)
*   //action->type = something; //R/W access is also forbidden
*
* //To release resources
* of12_destroy_action_group(group)
* @endcode
*
* Write actions(pseudo-code):
* @code
*
* write_actions = of12_init_write_actions()
*
* for(i in openflow_msg->write_actions)
*   action = of12_init_packet_action(i->type, i->field)
*   of12_set_packet_action_on_write_actions(write_actions,action);
*   //Action must be freed(set only copies the action)
*   of12_destroy_packet_action(action)
*   
*
* //To release resources
* of12_destroy_write_actions(group)
* @endcode
*
*/


/**
* @ingroup core_of12 
* Actions over a packet as per defined in OF12. Set operations are converted to actions
* for pipeline simplification. This is comming from of12_action_type enum. 
*
* @warning values are MODIFIED from OF specification and are reorder! Order matters,
* as when write actions are executed are done in order.
*/
typedef enum{
	//No action. This MUST always exist and the value MUST be 0	
	OF12_AT_NO_ACTION		= 0, 	/* NO action/EMPTY action. */

	//Copy TTL inwards
	OF12_AT_COPY_TTL_IN,			/* Copy TTL "inwards" -- from outermost to next-to-outermost */

	/*
	*Pop: first VLAN, MPLS, PPPoE, PPP
	*/
	OF12_AT_POP_VLAN,			/* Pop the outer VLAN tag */
	OF12_AT_POP_MPLS,			/* Pop the outer MPLS tag */
	OF12_AT_POP_PPPOE,			/* Pop the PPPoE tag */

	/*
	*Push: first PPP, PPPoE, MPLS, VLAN
	*/
	OF12_AT_PUSH_PPPOE,			/* Push a new PPPoE tag */
	OF12_AT_PUSH_MPLS,			/* Push a new MPLS tag */
	OF12_AT_PUSH_VLAN,			/* Push a new VLAN tag */

	//Copy ttl outwards
	OF12_AT_COPY_TTL_OUT, 			/* Copy TTL "outwards" -- from next-to-outermost to outermost */

	//Decrement TTL
	OF12_AT_DEC_NW_TTL,			/* Decrement IP TTL. */
	OF12_AT_DEC_MPLS_TTL,			/* Decrement MPLS TTL */

	//Set fields
	OF12_AT_SET_MPLS_TTL,			/* MPLS TTL */
	OF12_AT_SET_NW_TTL,			/* IP TTL. */
	OF12_AT_SET_QUEUE,			/* Set queue id when outputting to a port */

	//TODO 
	//OF12_AT_SET_FIELD_METADATA,		/* Ethernet destination address. */
	
	OF12_AT_SET_FIELD_ETH_DST,   		/* Ethernet destination address. */
	OF12_AT_SET_FIELD_ETH_SRC,   		/* Ethernet source address. */
	OF12_AT_SET_FIELD_ETH_TYPE,  		/* Ethernet frame type. */
	OF12_AT_SET_FIELD_VLAN_VID,  		/* VLAN id. */
	OF12_AT_SET_FIELD_VLAN_PCP,  		/* VLAN priority. */
	OF12_AT_SET_FIELD_IP_DSCP,   		/* IP DSCP (6 bits in ToS field). */
	OF12_AT_SET_FIELD_IP_ECN,    		/* IP ECN (2 bits in ToS field). */
	OF12_AT_SET_FIELD_IP_PROTO,  		/* IP protocol. */
	OF12_AT_SET_FIELD_IPV4_SRC,  		/* IPv4 source address. */
	OF12_AT_SET_FIELD_IPV4_DST,  		/* IPv4 destination address. */
	OF12_AT_SET_FIELD_TCP_SRC,   		/* TCP source port. */
	OF12_AT_SET_FIELD_TCP_DST,   		/* TCP destination port. */
	OF12_AT_SET_FIELD_UDP_SRC,   		/* UDP source port. */
	OF12_AT_SET_FIELD_UDP_DST,   		/* UDP destination port. */
	OF12_AT_SET_FIELD_ICMPV4_TYPE,		/* ICMP type. */
	OF12_AT_SET_FIELD_ICMPV4_CODE,		/* ICMP code. */
	OF12_AT_SET_FIELD_MPLS_LABEL,		/* MPLS label. */
	OF12_AT_SET_FIELD_MPLS_TC,   	   	/* MPLS TC. */

	OF12_AT_SET_FIELD_PPPOE_CODE,		/* PPPoE code */
	OF12_AT_SET_FIELD_PPPOE_TYPE,		/* PPPoE type */
	OF12_AT_SET_FIELD_PPPOE_SID, 	   	/* PPPoE session id */
	OF12_AT_SET_FIELD_PPP_PROT,  	   	/* PPP protocol */

	/* Add more set fields here... */

	//Groups		
	OF12_AT_GROUP,				/* Apply group. */

	//Experimenter
	OF12_AT_EXPERIMENTER,	

	OF12_AT_OUTPUT			 	/* Output to switch port. */
}of12_packet_action_type_t;

#define OF12_AT_NUMBER OF12_AT_OUTPUT+1 

/**
* @ingroup core_of12 
* Special port numbers, according to OF12 (of12p_port_no )
*/
enum of12_port_numbers {

	/* Maximum number of physical switch ports. */
	OF12_PORT_MAX = 0xffffff00,

	/* Fake output "ports". */
	OF12_PORT_IN_PORT = 0xfffffff8,  /* Send the packet out the input port.  This
	virtual port must be explicitly used
	in order to send back out of the input
	port. */

	OF12_PORT_TABLE      = 0xfffffff9,	/* Submit the packet to the first flow table
						NB: This destination port can only be
						used in packet-out messages. */
	OF12_PORT_NORMAL     = 0xfffffffa,	/* Process with normal L2/L3 switching. */
	OF12_PORT_FLOOD      = 0xfffffffb,	/* All physical ports in VLAN, except input
						port and those blocked or link down. */
	OF12_PORT_ALL        = 0xfffffffc,	/* All physical ports except input port. */
	OF12_PORT_CONTROLLER = 0xfffffffd,  	/* Send to controller. */
	OF12_PORT_LOCAL      = 0xfffffffe,  	/* Local openflow "port". */
	OF12_PORT_ANY        = 0xffffffff	/* Wildcard port used only for flow mod
						(delete) and flow stats requests. Selects
						all flows regardless of output port
						(including flows with no output port). */
};


//fwd declaration
struct of12_group;

/**
* @ingroup core_of12 
* Packet action abstraction data structure 
*/
typedef struct of12_packet_action{
	//Type and value(for set fields and push)
	of12_packet_action_type_t type;

	/** @TODO substitute for a 128 value for IPv6 support */
	uint64_t field;

	//group
	struct of12_group* group;
	
	//DLL
	struct of12_packet_action* prev;
	struct of12_packet_action* next;
}of12_packet_action_t;

/**
* @ingroup core_of12 
* Action group (apply-actions) structure
*/
typedef struct{

	//Number of actions in the list
	unsigned int num_of_actions;	

	//Double linked list
	of12_packet_action_t* head;
	of12_packet_action_t* tail;
	
	//Number of outputs in the action list
	unsigned int num_of_output_actions;
	
}of12_action_group_t;

/**
* @ingroup core_of12 
* Write actions structure
*/
typedef struct{
	//Presence of action flag => type == 0
	of12_packet_action_t write_actions[OF12_AT_NUMBER];
	
	//Number of actions. Merely for dumping and to skip innecessary looip iterations
	unsigned int num_of_actions;	
	
}of12_write_actions_t;

//Fwd declaration
struct of12_switch;

/*
*
* Function prototypes
*
*/

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

//Action
/**
* @ingroup core_of12 
* Initializes a packet action (OF action)
*
* Each field set consitutes an action per-se. 
*/
of12_packet_action_t* of12_init_packet_action(/*const struct of12_switch* sw,*/of12_packet_action_type_t type, uint64_t field, of12_packet_action_t* prev, of12_packet_action_t* next);

/**
* @ingroup core_of12 
* Destroys packet action (OF action)
*/
void of12_destroy_packet_action(of12_packet_action_t* action);

//Action group
/**
* @ingroup core_of12 
* Create an action group (apply actions) 
*/
of12_action_group_t* of12_init_action_group(of12_packet_action_t* actions);

/**
* @ingroup core_of12 
* Destroy an action group. This also destroys actions contained
*/
void of12_destroy_action_group(of12_action_group_t* group);

//Push packet action
/**
* @ingroup core_of12 
* Push an action to the group. The action can no longer be used or freed
* from outside of the library. of12_destroy_action_group() will destroy it. 
*/
void of12_push_packet_action_to_group(of12_action_group_t* group, of12_packet_action_t* action);


//Apply actions
void of12_process_apply_actions(const struct of12_switch* sw, const unsigned int table_id, datapacket_t* pkt, const of12_action_group_t* apply_actions_group, bool replicate_pkts);

//Write actions data structure management
/**
* @ingroup core_of12 
* Init a write actions group
*/
void of12_init_packet_write_actions(datapacket_t *const pkt, of12_write_actions_t* write_actions);
of12_write_actions_t* of12_init_write_actions(void);

/**
* @ingroup core_of12 
* Destroy a write_actions instance. This also destroys actions contained
*/
void of12_destroy_write_actions(of12_write_actions_t* write_actions);

/**
* @ingroup core_of12 
* Set (copy) the action to the write actions. The action pointer can safely used
* outside the library, and released. 
*/
void of12_set_packet_action_on_write_actions(of12_write_actions_t* write_actions, of12_packet_action_t* action);

void of12_update_packet_write_actions(datapacket_t* pkt, const of12_write_actions_t* entry_write_actions);
void of12_clear_write_actions(of12_write_actions_t* write_actions);
void of12_process_write_actions(const struct of12_switch* sw, const unsigned int table_id, datapacket_t* pkt, bool replicate_pkts);

//Update apply/write
rofl_result_t of12_update_apply_actions(of12_action_group_t** group, of12_action_group_t* new_group);
rofl_result_t of12_update_write_actions(of12_write_actions_t** group, of12_write_actions_t* new_group);


//Checking functions
bool of12_write_actions_has(of12_write_actions_t* write_actions, of12_packet_action_type_t type, uint64_t value);
bool of12_apply_actions_has(const of12_action_group_t* apply_actions_group, of12_packet_action_type_t type, uint64_t value);

//Copy (cloning) methods
of12_action_group_t* of12_copy_action_group(of12_action_group_t* origin);
of12_write_actions_t* of12_copy_write_actions(of12_write_actions_t* origin);

//Dump
void of12_dump_write_actions(of12_write_actions_t* write_actions_group);
void of12_dump_action_group(of12_action_group_t* action_group);

//validate actions
bool of12_validate_action_group(of12_action_group_t *ag);
bool of12_validate_write_actions(of12_write_actions_t *wa);


//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_ACTION
