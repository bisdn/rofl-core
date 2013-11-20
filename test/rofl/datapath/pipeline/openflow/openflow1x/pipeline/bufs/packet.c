#include <inttypes.h>
#include <rofl/datapath/pipeline/common/datapacket.h>
#include <rofl/datapath/pipeline/common/ternary_fields.h>
#include <rofl/datapath/pipeline/physical_switch.h>
#include <rofl/datapath/pipeline/openflow/of_switch.h>
#include <rofl/datapath/pipeline/platform/packet.h>
#include <assert.h>
#include <stdio.h>
#include <CUnit/Basic.h>
#include "io.h" 

/*
* Counters
*/

unsigned int replicas = 0;
unsigned int drops = 0;
unsigned int outputs = 0;
unsigned int allocated = 0;
unsigned int released = 0;

/*
* Buffer pool 
*/
#define FAKE_IO_POOL_SLOTS 16
datapacket_t* pool[FAKE_IO_POOL_SLOTS]={0};
bool pool_state[FAKE_IO_POOL_SLOTS]={false};

void init_io(){
	int i;
	for(i=0;i<FAKE_IO_POOL_SLOTS;i++){
		pool[i] = (datapacket_t*)malloc(sizeof(datapacket_t));	
	}
}

void destroy_io(){
	int i;

	for(i=0;i<FAKE_IO_POOL_SLOTS;i++){
		free(pool[i]);
	}	
}

void reset_io_state(){
	int i;

	replicas = drops = outputs = allocated = released = 0;

	for(i=0;i<FAKE_IO_POOL_SLOTS;i++){
		pool_state[i] = false;
	}
}

datapacket_t* allocate_buffer(){
	int i;
	for(i=0;i<FAKE_IO_POOL_SLOTS;i++){
		if(pool_state[i] == false){
			pool_state[i] = true;
			allocated++;
			fprintf(stderr,"[pool] allocated %p\n", pool[i]);
			return pool[i];
		}
	}

	CU_ASSERT(0);
	
	return NULL;
}
void release_buffer(datapacket_t* pkt){
	int i;
	
	for(i=0;i<FAKE_IO_POOL_SLOTS;i++){
		if(pool[i] == pkt){
			if(pool_state[i] != true){
				fprintf(stderr," Attempting to release an unallocated or previously released buffer\n");
			}
			CU_ASSERT(pool_state[i] == true);
			pool_state[i] = false;
			released++;
			fprintf(stderr,"[pool] released %p\n", pkt);
			return;
		}
	}
		
	CU_ASSERT(0);
}

void platform_packet_copy_ttl_in(datapacket_t* pkt){}
void platform_packet_pop_vlan(datapacket_t* pkt){}
void platform_packet_pop_mpls(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_pop_pppoe(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_pop_ppp(datapacket_t* pkt){}
void platform_packet_push_ppp(datapacket_t* pkt){}
void platform_packet_push_pppoe(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_push_mpls(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_push_vlan(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_copy_ttl_out(datapacket_t* pkt){}
void platform_packet_dec_nw_ttl(datapacket_t* pkt){}
void platform_packet_dec_mpls_ttl(datapacket_t* pkt){}
void platform_packet_set_mpls_ttl(datapacket_t* pkt, uint8_t new_ttl){}
void platform_packet_set_nw_ttl(datapacket_t* pkt, uint8_t new_ttl){}
void platform_packet_set_queue(datapacket_t* pkt, uint32_t queue){}
//TODO:
//void platform_packet_set_metadata(datapacket_t* pkt, uint64_t metadata){ }
void platform_packet_set_eth_dst(datapacket_t* pkt, uint64_t eth_dst){}
void platform_packet_set_eth_src(datapacket_t* pkt, uint64_t eth_src){}
void platform_packet_set_eth_type(datapacket_t* pkt, uint16_t eth_type){}
void platform_packet_set_vlan_vid(datapacket_t* pkt, uint16_t vlan_vid){}
void platform_packet_set_vlan_pcp(datapacket_t* pkt, uint8_t vlan_pcp){}
void platform_packet_set_arp_opcode(datapacket_t* pkt, uint16_t arp_opcode){}
void platform_packet_set_arp_sha(datapacket_t* pkt, uint64_t arp_sha){}
void platform_packet_set_arp_spa(datapacket_t* pkt, uint32_t arp_spa){}
void platform_packet_set_arp_tha(datapacket_t* pkt, uint64_t arp_tha){}
void platform_packet_set_arp_tpa(datapacket_t* pkt, uint32_t arp_tpa){}
void platform_packet_set_ip_dscp(datapacket_t* pkt, uint8_t ip_dscp){}
void platform_packet_set_ip_ecn(datapacket_t* pkt, uint8_t ip_ecn){}
void platform_packet_set_ip_proto(datapacket_t* pkt, uint8_t ip_proto){}
void platform_packet_set_ipv4_src(datapacket_t* pkt, uint32_t ip_src){}
void platform_packet_set_ipv4_dst(datapacket_t* pkt, uint32_t ip_dst){}
void platform_packet_set_tcp_src(datapacket_t* pkt, uint16_t tcp_src){}
void platform_packet_set_tcp_dst(datapacket_t* pkt, uint16_t tcp_dst){}
void platform_packet_set_udp_src(datapacket_t* pkt, uint16_t udp_src){}
void platform_packet_set_udp_dst(datapacket_t* pkt, uint16_t udp_dst){}
void platform_packet_set_sctp_src(datapacket_t* pkt, uint16_t sctp_src){}
void platform_packet_set_sctp_dst(datapacket_t* pkt, uint16_t sctp_dst){}



void platform_packet_set_icmpv4_type(datapacket_t* pkt, uint8_t type){}
void platform_packet_set_icmpv4_code(datapacket_t* pkt, uint8_t code){}
void platform_packet_set_mpls_label(datapacket_t* pkt, uint32_t label){}
void platform_packet_set_mpls_tc(datapacket_t* pkt, uint8_t tc){}
void platform_packet_set_mpls_bos(datapacket_t* pkt, bool bos){}
void platform_packet_set_pppoe_type(datapacket_t* pkt, uint8_t type){}
void platform_packet_set_pppoe_code(datapacket_t* pkt, uint8_t code){}
void platform_packet_set_pppoe_sid(datapacket_t* pkt, uint16_t sid){}
void platform_packet_set_ppp_proto(datapacket_t* pkt, uint16_t proto){}
void platform_packet_set_gtp_msg_type(datapacket_t* pkt, uint8_t msg_type){}
void platform_packet_set_gtp_teid(datapacket_t* pkt, uint32_t teid){}
void platform_packet_output(datapacket_t* pkt, switch_port_t* port){
	fprintf(stderr,"Output packet %p\n", pkt);
	release_buffer(pkt);
	outputs++;
}
datapacket_t* platform_packet_replicate(datapacket_t* pkt){
	datapacket_t* replica = allocate_buffer(); 
	if(replica){
		replicas++;
	}
	fprintf(stderr,"Pkt: %p cloned into %p\n", pkt, replica);
	return replica;
}
void platform_packet_drop(datapacket_t* pkt){
	fprintf(stderr,"Drop packet %p\n", pkt);
	release_buffer(pkt);
	drops++;
}
void platform_packet_set_ipv6_src(datapacket_t * pkt, uint128__t ipv6_src){}
void platform_packet_set_ipv6_dst(datapacket_t * pkt, uint128__t ipv6_dst){}
void platform_packet_set_ipv6_flabel(datapacket_t * pkt, uint64_t ipv6_flabel){}
void platform_packet_set_ipv6_nd_target(datapacket_t * pkt, uint128__t ipv6_nd_target){}
void platform_packet_set_ipv6_nd_sll(datapacket_t * pkt, uint64_t ipv6_nd_sll){}
void platform_packet_set_ipv6_nd_tll(datapacket_t * pkt, uint64_t ipv6_nd_tll){}
void platform_packet_set_ipv6_exthdr(datapacket_t * pkt, uint16_t ipv6_exthdr){}
void platform_packet_set_icmpv6_type(datapacket_t * pkt, uint8_t icmpv6_type){}
void platform_packet_set_icmpv6_code(datapacket_t * pkt, uint8_t icmpv6_code){}

void platform_packet_set_pbb_isid(datapacket_t*pkt, uint32_t pbb_isid){}
void platform_packet_pop_pbb(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_push_pbb(datapacket_t* pkt, uint16_t ether_type){}
void platform_packet_pop_gtp(datapacket_t* pkt){}
void platform_packet_push_gtp(datapacket_t* pkt){}



void platform_packet_set_tunnel_id(datapacket_t*pkt, uint64_t tunnel_id){}

uint32_t
platform_packet_get_size_bytes(datapacket_t * const pkt){
	return 0; 
}
uint32_t platform_packet_get_port_in(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_phy_port_in(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_eth_dst(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_eth_src(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_eth_type(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_vlan_vid(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_vlan_pcp(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_arp_opcode(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_arp_sha(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_arp_spa(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_arp_tha(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_arp_tpa(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_ip_proto(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_ip_ecn(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_ip_dscp(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_ipv4_src(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_ipv4_dst(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_tcp_dst(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_tcp_src(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_udp_dst(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_udp_src(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_sctp_dst(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_sctp_src(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_icmpv4_type(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_icmpv4_code(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_mpls_label(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_mpls_tc(datapacket_t *const pkt){
	return 0;
}
bool platform_packet_get_mpls_bos(datapacket_t *const pkt){
	return false;
}
uint8_t platform_packet_get_pppoe_code(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_pppoe_type(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_pppoe_sid(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_ppp_proto(datapacket_t *const pkt){
	return 0;
}
uint128__t platform_packet_get_ipv6_src(datapacket_t *const pkt){
	uint128__t ret;
	return ret;
}
uint128__t platform_packet_get_ipv6_dst(datapacket_t *const pkt){
	uint128__t ret;
	return ret;
}
uint64_t platform_packet_get_ipv6_flabel(datapacket_t *const pkt){
	return 0;
}
uint128__t platform_packet_get_ipv6_nd_target(datapacket_t *const pkt){
	uint128__t ret;
	return ret;
}
uint64_t platform_packet_get_ipv6_nd_sll(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_ipv6_nd_tll(datapacket_t *const pkt){
	return 0;
}
uint16_t platform_packet_get_ipv6_exthdr(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_icmpv6_type(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_icmpv6_code(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_pbb_isid(datapacket_t *const pkt){
	return 0;
}
uint64_t platform_packet_get_tunnel_id(datapacket_t *const pkt){
	return 0;
}
uint8_t platform_packet_get_gtp_msg_type(datapacket_t *const pkt){
	return 0;
}
uint32_t platform_packet_get_gtp_teid(datapacket_t *const pkt){
	return 0;
}
bool platform_packet_has_vlan(datapacket_t *const pkt){
       return false;
}
