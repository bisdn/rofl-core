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
	ether[12] = 0x81;
	ether[13] = 0x00;

	cmemory ptest = ether + outer_vlan_tag + outer_mpls_tag + inner_vlan_tag + inner_mpls_tag + payload;

	printf("cpacket::classify() test packet: %s\n", ptest.c_str());

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

