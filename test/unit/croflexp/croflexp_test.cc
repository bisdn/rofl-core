/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <rofl/common/croflexp.h>
#include <stdlib.h>

using namespace rofl;

void check_rext_flowspace();



int
main(int args, char** argv)
{
	check_rext_flowspace();

	return EXIT_SUCCESS;
}

#if 0
0x16093d0 : 00 01 00 16   80 00 00 04   00 00 00 01   80 00 08 06
0x16093e0 : 00 11 11 11   11 11 00 00
#endif

void
check_rext_flowspace()
{
	printf("croflexp_flowspace::croflexp_flowspace() check\n");

	cofmatch m;

	m.set_in_port(1);
	m.set_eth_src(cmacaddr("00:11:11:11:11:11"));

	croflexp_flowspace *fsp = new croflexp_flowspace(croflexp::OFPRET_FSP_ADD, m);

	cmemory test(m.length()); m.pack((struct ofp_match*)test.somem(), test.memlen());

	cmemory result(sizeof(struct croflexp::ofp_rofl_ext_flowspace) + m.length());

	result[0] = croflexp::OFP_ROFL_EXT_VERSION1;
	result[1] = croflexp::OFPRET_FLOWSPACE;
	uint16_t p_len = sizeof(struct croflexp::ofp_rofl_ext_flowspace) + m.length();
	result[2] = (p_len & 0xff00) >> 8;
	result[3] = (p_len & 0x00ff) >> 0;
	result[4] = croflexp::OFPRET_FSP_ADD;
	result[5] = 0;
	result[6] = 0;
	result[7] = 0;

	m.pack((struct ofp_match*)(result.somem() +
					sizeof(struct croflexp::ofp_rofl_ext_flowspace)), m.length());


	cmemory fsp_packed(fsp->length());	fsp->pack(fsp_packed.somem(), fsp_packed.memlen());

	printf("croflexp_flowspace() construction and packing test ... ");

	if (fsp_packed != result)
	{
		printf("failed. =>\nexpected: %s\nreceived: %s\n", result.c_str(), fsp_packed.c_str());

		exit(EXIT_FAILURE);
	}
	else
	{
		printf(" successful.\n");
	}



	printf("croflexp_flowspace() parsing and unpacking test ... ");

	try {
		croflexp rexp(fsp_packed.somem(), fsp_packed.memlen());

		if (not (rexp.match == m))
		{
			printf("failed. =>\nexpected match: %s\nreceived match: %s\n", m.c_str(), rexp.match.c_str());

			exit(EXIT_FAILURE);
		}

		if (rexp.rext_header->version != croflexp::OFP_ROFL_EXT_VERSION1)
		{
			printf("failed. => ofp_rofl_ext_header::version\nexpected value: %d\nreceived value: %d\n",
					croflexp::OFP_ROFL_EXT_VERSION1, rexp.rext_header->version);

			exit(EXIT_FAILURE);
		}

		if (rexp.rext_header->type != croflexp::OFPRET_FLOWSPACE)
		{
			printf("failed. => ofp_rofl_ext_header::type\nexpected value: %d\nreceived value: %d\n",
					croflexp::OFPRET_FLOWSPACE, rexp.rext_header->type);

			exit(EXIT_FAILURE);
		}

		if (be16toh(rexp.rext_header->length) !=
				sizeof(struct croflexp::ofp_rofl_ext_flowspace) + m.length())
		{
			printf("failed. => ofp_rofl_ext_header::length\nexpected value: %lu\nreceived value: %d\n",
					sizeof(struct croflexp::ofp_rofl_ext_flowspace) + m.length(),
					be16toh(rexp.rext_header->length));

			exit(EXIT_FAILURE);
		}

		if (rexp.rext_fsp->command != croflexp::OFPRET_FSP_ADD)
		{
			printf("failed. => ofp_rofl_ext_flowspace::command\nexpected value: %d\nreceived value: %d\n",
					croflexp::OFPRET_FSP_ADD, rexp.rext_fsp->command);

			exit(EXIT_FAILURE);
		}

		printf(" successful.\n");

	} catch (eRoflExpInval& e) {

		printf("failed with exception indicating invalid frame syntax.\n");

		exit(EXIT_FAILURE);
	}
}



