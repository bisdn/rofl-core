/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include <rofl/common/cmacaddr.h>
#include <rofl/common/cmemory.h>
#include <rofl/common/cpacket.h>
#include <stdlib.h>


void check_parser();
void check_push_pop_vlan();
//void check_push_pop_mpls();
//void check_push_pop_pppoe();

cmemory create_ether_header(
		cmacaddr const& dst,
		cmacaddr const& src,
		uint16_t eth_type);

cmemory create_vlan_tag(
		uint16_t vid,
		uint8_t pcp,
		uint16_t eth_type);

cmemory create_mpls_tag(
		uint32_t label,
		uint8_t tc,
		uint8_t ttl,
		bool bos);

cmemory create_ipv4_header(
		uint8_t tos,
		uint16_t total_len,
		uint16_t ident,
		uint8_t ttl,
		uint8_t proto,
		caddress const& src,
		caddress const& dst);

cmemory create_payload(
		size_t size,
		uint8_t value);

//cmemory create_ipv4_header();




int
main(int args, char** argv)
{
	//check_parser();
	check_push_pop_vlan();

	return EXIT_SUCCESS;
}



// 0x88a8

cmemory
create_ether_header(
		cmacaddr const& dst,
		cmacaddr const& src,
		uint16_t eth_type)
{
	cmemory ether(14); 							// empty ethernet header

	memcpy(ether.somem(), dst.somem(), OFP_ETH_ALEN);
	memcpy(ether.somem() + OFP_ETH_ALEN, src.somem(), OFP_ETH_ALEN);

	ether[12] = (eth_type & 0xff00) >> 8;
	ether[13] = (eth_type & 0x00ff);

	return ether;
}



cmemory
create_vlan_tag(
		uint16_t vid,
		uint8_t pcp,
		uint16_t eth_type)
{
	cmemory vlan(4);

	vlan[1] 	 = vid & 0x00ff;
	vlan[0] 	&= 0xf0;
	vlan[0]		 = ((pcp & 0x07) << 5) | ((vid & 0x0f00) >> 8);

	vlan[2] 	 = (eth_type & 0xff00) >> 8;
	vlan[3]		 = (eth_type & 0x00ff);

	return vlan;
}



cmemory
create_mpls_tag(
		uint32_t label,
		uint8_t tc,
		uint8_t ttl,
		bool bos)
{
	cmemory mpls(4);

	mpls[0] =  (label & 0x000ff000) >> 12;
	mpls[1] =  (label & 0x00000ff0) >>  4;
	mpls[2] = ((label & 0x0000000f) <<  4) + ((tc & 0x07) << 1) + (bos & 0x01);
	mpls[3] = ttl;

	return mpls;
}


cmemory
create_ipv4_header(
		uint8_t tos,
		uint16_t total_len,
		uint16_t ident,
		uint8_t ttl,
		uint8_t proto,
		caddress const& src,
		caddress const& dst)
{
	cmemory ipv4(20);

	ipv4[0] = (4 << 4) + 5;
	ipv4[1] = tos;
	ipv4[2] = (total_len & 0xff00) >> 8;
	ipv4[3] = (total_len & 0x00ff) >> 0;
	ipv4[4] = (ident & 0xff00) >> 8;
	ipv4[5] = (ident & 0x00ff) >> 0;
	ipv4[6] = 0x88; // flags + fragment offset
	ipv4[7] = 0x99; // fragment offset
	ipv4[8] = ttl;
	ipv4[9] = proto;
	ipv4[10] = 0xaf; // header checksum
	ipv4[11] = 0xfa; // header checksum
	memcpy(&ipv4[12], &(src.s4addr->sin_addr.s_addr), sizeof(uint32_t));
	memcpy(&ipv4[16], &(dst.s4addr->sin_addr.s_addr), sizeof(uint32_t));


	printf("%s\n", ipv4.c_str());

	//exit(EXIT_SUCCESS);

	return ipv4;
}



cmemory
create_payload(size_t size, uint8_t value)
{
	cmemory payload(size);

	for (unsigned int i = 0; i < size; i++)
	{
		payload[i] = value;
	}
	return payload;
}



void
check_push_pop_vlan()
{
	cmemory pack(0);

	pack += create_ether_header(
			cmacaddr("00:11:11:11:11:11"),
			cmacaddr("00:22:22:22:22:22"),
			0x0800 /*IPv4*/);
	pack += create_ipv4_header(0x00, 38, 0x4444, 0x10, 0x00, caddress(AF_INET, "10.1.1.1"), caddress(AF_INET, "10.2.2.2"));
	pack += create_payload(18, 0x80);




	/*
	 * sub-test-1 => push vlan and set fields
	 */
	{
		printf("cpacket::classify() test packet: %s\n", pack.c_str());

		cmemory result1(0);

		result1 += create_ether_header(
				cmacaddr("00:11:11:11:11:11"),
				cmacaddr("00:22:22:22:22:22"),
				0x0800 /*IPv4*/);
		result1 += create_vlan_tag(0xfff, 0x0, 0x8100);
		result1 += create_ipv4_header(0x00, 38, 0x4444, 0x10, 0x00, caddress(AF_INET, "10.1.1.1"), caddress(AF_INET, "10.2.2.2"));
		result1 += create_payload(18, 0x80);




		cpacket a1(pack.somem(), pack.memlen(), /*in_port=*/3, true);

		printf("cpacket: %s\n", a1.c_str());



		printf("push vlan => 0x8100, 0xfff, 0x00\n");
		a1.push_vlan(0x8100);
		a1.set_field(coxmatch_ofb_vlan_vid(0xfff));
		a1.set_field(coxmatch_ofb_vlan_pcp(0x00));
		printf("a1: %s\n", a1.c_str());

		if (a1.to_mem() != result1)
		{
			printf("push_vlan failed =>\nexpected: %s\nreceived: %s", result1.c_str(), a1.c_str());

			exit(EXIT_FAILURE);
		}
	}





	return;


#if 0
	if (a1 != a2)
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("cmemory::operator[] () success\n");
	}
#endif
}




void
check_parser()
{
	cmemory ptest(0);

	ptest += create_ether_header(
			cmacaddr("00:11:11:11:11:11"),
			cmacaddr("00:22:22:22:22:22"),
			0x88a8 /*IEEE802.1ad Q-in-Q*/);
	ptest += create_vlan_tag(0xfff, 0x0, 0x8100);
	ptest += create_vlan_tag(0x333, 0x7, 0x8848);
	ptest += create_mpls_tag(0x55555, 0x03, 0x10, false);
	ptest += create_mpls_tag(0x77777, 0x00, 0x20, true);
	ptest += create_ipv4_header(0x00, 38, 0x4444, 0x10, 0x00, caddress(AF_INET, "10.1.1.1"), caddress(AF_INET, "10.2.2.2"));
	ptest += create_payload(18, 0x80);


	printf("cpacket::classify() test packet: %s\n", ptest.c_str());


	cpacket a1(ptest.somem(), ptest.memlen(), /*in_port=*/3, true);

	printf("a1: %s\n", a1.c_str());


	printf("pop outer mpls => should be ignored\n");
	a1.pop_mpls(0x8847);
	printf("a1: %s\n", a1.c_str());


	printf("pop outer vlan =>\n");
	a1.pop_vlan();
	printf("a1: %s\n", a1.c_str());


	return;

#if 0
	if (a1 != a2)
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("cmemory::operator[] () success\n");
	}
#endif
}
