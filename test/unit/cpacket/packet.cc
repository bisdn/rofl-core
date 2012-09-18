/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include <rofl/common/cmacaddr.h>
#include <rofl/common/cmemory.h>
#include <rofl/common/cpacket.h>
#include <stdlib.h>


void check_parser();



int
main(int args, char** argv)
{
	check_parser();

	return EXIT_SUCCESS;
}


void
check_parser()
{
	cmemory ptest(0);

	cmemory ether(14); 			// ethernet header
	cmemory outer_vlan_tag(4); 	// outer vlan tag
	cmemory inner_vlan_tag(4); 	// inner vlan tag
	cmemory outer_mpls_tag(4); 	// outer mpls tag
	cmemory inner_mpls_tag(4); 	// inner mpls tag
	cmemory payload(18); 		// payload

	cmacaddr dl_src("00:11:11:11:11:11");
	cmacaddr dl_dst("00:22:22:22:22:22");
	memcpy(ether.somem(), dl_dst.somem(), OFP_ETH_ALEN);
	memcpy(ether.somem() + OFP_ETH_ALEN, dl_src.somem(), OFP_ETH_ALEN);
	ether[12] = 0x88; // IEEE 802.1ad-q-in-q
	ether[13] = 0xa8; // IEEE 802.1ad-q-in-q



	outer_vlan_tag[0] = 0x0f;
	outer_vlan_tag[1] = 0xff;
	outer_vlan_tag[2] = 0x81;
	outer_vlan_tag[3] = 0x00;


	inner_vlan_tag[0] = 0x03;
	inner_vlan_tag[1] = 0x33;
	inner_vlan_tag[2] = 0x88;
	inner_vlan_tag[3] = 0x48;


	outer_mpls_tag[0] = 0x55; // label
	outer_mpls_tag[1] = 0x55; // label
	outer_mpls_tag[2] = 0x50; // label, TC = 0, BoS = 0
	outer_mpls_tag[3] = 0x10; // TTL = 16


	inner_mpls_tag[0] = 0x77; // label
	inner_mpls_tag[1] = 0x77; // label
	inner_mpls_tag[2] = 0x71; // label, TC = 0, BoS = 1
	inner_mpls_tag[3] = 0x20; // TTL = 32


	for (int i = 0; i < 18; i++)
	{
		payload[i] = 0x80;
	}



	ptest += ether;
	ptest += outer_vlan_tag;
	ptest += inner_vlan_tag;
	ptest += outer_mpls_tag;
	ptest += inner_mpls_tag;
	ptest += payload;

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

