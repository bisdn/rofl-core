/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <rofl/common/croflexp.h>
#include <stdlib.h>


void check_rext_flowspace();



int
main(int args, char** argv)
{
	check_rext_flowspace();

	return EXIT_SUCCESS;
}



void
check_rext_flowspace()
{




	size_t size = 32;

	cmemory a1(size);

	printf("cmemory::operator[] () check\n");

	/*
	 * fill entire memory with value 0x55 for comparison
	 */
	memset(a1.somem(), 0x55, a1.memlen());


	/*
	 * check index operator
	 */
	cmemory a2(size);
	for (unsigned int i = 0; i < size; i++)
	{
		a2[i] = 0x55;
	}

	printf("a1: %s\n", a1.c_str());
	printf("a2: %s\n", a2.c_str());

	if (a1 != a2)
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("cmemory::operator[] () success\n");
	}
}



