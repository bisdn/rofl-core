/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <rofl/common/cclock.h>
#include <stdlib.h>
#include <unistd.h>

using namespace rofl;

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

	printf("copying clk...");
	cclock clk1(clk);

	if (clk1 != clk)
	{
		printf("failed.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("success.\n");
	}

	printf("testing clk add operation...");
	cclock clk2(clk);

	clk2.ts.tv_sec += 4;

	printf("sleeping %d secs. ...", 2);
	sleep(2);

	if (clk2 > cclock::now())
	{
		printf("failed. clk2: %s  now: %s\n", clk2.c_str(), cclock::now().c_str());
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("success.\n");
	}

	printf("testing clk add operation..."); fflush(stdout);
	printf("sleeping %d secs. ...", 6);
	sleep(6);

	printf("BLUB => clk2: %s  now: %s\n", clk2.c_str(), cclock::now().c_str());

	if (clk2 > cclock::now())
	{
		printf("failed. clk2: %s  now: %s\n", clk2.c_str(), cclock::now().c_str());
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("success.\n");
	}



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



