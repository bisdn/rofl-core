#ifndef __OF12_ACTION_H__
#define __OF12_ACTION_H__

#include <inttypes.h> 
#include <string.h> 
#include "../../../util/rofl_pipeline_utils.h"
#include "../../../common/datapacket.h"

/* 
* Actions over a packet as per defined in OF12. Comming from of12_12_action_type enum. WARNING: values are MODIFIED and are reorder! Order matters, as when write actions are executed are done in order 
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
	OF12_AT_POP_PPP,			/* Pop the PPP tag */

	/*
	*Push: first PPP, PPPoE, MPLS, VLAN
	*/
	OF12_AT_PUSH_PPP,			/* Push a new PPP tag */
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

typedef void of12_group_t;		//FIXME: remove when appropiate of12_group implementation is available

/* Packet action abstraction data structure */
struct of12_packet_action{
	//Type and value(for set fields and push)
	of12_packet_action_type_t type;
	uint64_t field;			//TODO: substitute for a 128 value for IPv6 support

	//group
	of12_group_t* group;
	
	//DLL
	struct of12_packet_action* prev;
	struct of12_packet_action* next;
};
typedef struct of12_packet_action of12_packet_action_t;

/* Action group, using a double-linked-list */ 
typedef struct{
	//Number of actions in the list
	unsigned int num_of_actions;	

	//Double linked list
	of12_packet_action_t* head;
	of12_packet_action_t* tail;
}of12_action_group_t;

/* Write actions structure */
typedef struct{
	//Presence of action flag => type == 0
	of12_packet_action_t write_actions[OF12_AT_NUMBER];
	
	//Number of actions. Merely for dumping and to skip innecessary looip iterations
	unsigned int num_of_actions;	
	
}of12_write_actions_t;

/*
*
* Function prototypes
*
*/

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

//Action
of12_packet_action_t* of12_init_packet_action(of12_packet_action_type_t type, uint64_t field, of12_packet_action_t* prev, of12_packet_action_t* next);
void of12_destroy_packet_action(of12_packet_action_t* action);

//Action group
of12_action_group_t* of12_init_action_group(of12_packet_action_t* actions);
void of12_destroy_action_group(of12_action_group_t* group);

//Apply actions
void of12_process_apply_actions(datapacket_t* pkt, const of12_action_group_t* apply_actions_group);

//Write actions data structure management
void of12_init_packet_write_actions(datapacket_t *const pkt, of12_write_actions_t* write_actions);
of12_write_actions_t* of12_init_write_actions(void);
void of12_update_packet_write_actions(datapacket_t* pkt, const of12_write_actions_t* entry_write_actions);
void of12_clear_write_actions(of12_write_actions_t* write_actions);
void of12_destroy_write_actions(of12_write_actions_t* write_actions);
void of12_set_packet_action_on_write_actions(of12_write_actions_t* write_actions, of12_packet_action_t* action);

void of12_process_write_actions(datapacket_t* pkt);

//Push packet action
void of12_push_packet_action_to_group(of12_action_group_t* group, of12_packet_action_t* action);

//Dump
void of12_dump_write_actions(of12_write_actions_t* write_actions_group);
void of12_dump_action_group(of12_action_group_t* action_group);


/*
*
* Platform actions prototypes
*
*/

/* Copy ttl */
void platform_copy_ttl_in(datapacket_t* pkt);

/* POP */
void platform_pop_vlan(datapacket_t* pkt);
void platform_pop_mpls(datapacket_t* pkt);
void platform_pop_pppoe(datapacket_t* pkt);
void platform_pop_ppp(datapacket_t* pkt);

/* PUSH */
void platform_push_ppp(datapacket_t* pkt);
void platform_push_pppoe(datapacket_t* pkt);
void platform_push_mpls(datapacket_t* pkt);
void platform_push_vlan(datapacket_t* pkt);

/* Copy ttl out*/
void platform_copy_ttl_out(datapacket_t* pkt);

/* Decrement ttl */
void platform_dec_nw_ttl(datapacket_t* pkt);
void platform_dec_mpls_ttl(datapacket_t* pkt);

/* Set field */
void platform_set_mpls_ttl(datapacket_t* pktm, uint8_t new_ttl);
void platform_set_nw_ttl(datapacket_t* pkt, uint8_t new_ttl);
void platform_set_queue(datapacket_t* pkt, uint32_t queue);

//TODO:
//void platform_set_metadata(datapacket_t* pkt, uint64_t metadata);

//Ethernet
void platform_set_eth_dst(datapacket_t* pkt, uint64_t eth_dst);
void platform_set_eth_src(datapacket_t* pkt, uint64_t eth_src);
void platform_set_eth_type(datapacket_t* pkt, uint16_t eth_type);

//802.1q
void platform_set_vlan_vid(datapacket_t* pkt, uint16_t vlan_vid);
void platform_set_vlan_pcp(datapacket_t* pkt, uint8_t vlan_pcp);

//IP, IPv4
void platform_set_ip_dscp(datapacket_t* pkt, uint8_t ip_dscp);
void platform_set_ip_ecn(datapacket_t* pkt, uint8_t ip_ecn);
void platform_set_ip_proto(datapacket_t* pkt, uint8_t ip_proto);
void platform_set_ipv4_src(datapacket_t* pkt, uint32_t ip_src);
void platform_set_ipv4_dst(datapacket_t* pkt, uint32_t ip_dst);

//TCP
void platform_set_tcp_src(datapacket_t* pkt, uint16_t tcp_src);
void platform_set_tcp_dst(datapacket_t* pkt, uint16_t tcp_dst);

//UDP
void platform_set_udp_src(datapacket_t* pkt, uint16_t udp_src);
void platform_set_udp_dst(datapacket_t* pkt, uint16_t udp_dst);


//ICMPV4
void platform_set_icmpv4_type(datapacket_t* pkt, uint8_t type);
void platform_set_icmpv4_code(datapacket_t* pkt, uint8_t code);

//MPLS
void platform_set_mpls_label(datapacket_t* pkt, uint32_t label);
void platform_set_mpls_tc(datapacket_t* pkt, uint8_t tc);

//PPPOE
void platform_set_pppoe_type(datapacket_t* pkt, uint8_t type);
void platform_set_pppoe_code(datapacket_t* pkt, uint8_t code);
void platform_set_pppoe_sid(datapacket_t* pkt, uint16_t sid);

//PPP
void platform_set_ppp_proto(datapacket_t* pkt, uint16_t proto);

/* Output action */
void platform_output_packet(datapacket_t* pkt, uint32_t port_num);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //OF12_ACTION
