/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <rofl/common/cclock.h>
#include <stdlib.h>


void check_cclock();



int
main(int args, char** argv)
{
	check_cclock();

	return EXIT_SUCCESS;
}


void
check_cclock()
{
	printf("cclock check\n");

	cclock clk;

	printf("clk: %s\n", clk.c_str());

#if 0
	if (fsp_packed != result)
	{
		printf("failed. =>\nexpected: %s\nreceived: %s\n", result.c_str(), fsp_packed.c_str());

		exit(EXIT_FAILURE);
	}
	else
	{
		printf(" successful.\n");
	}
#endif
}



