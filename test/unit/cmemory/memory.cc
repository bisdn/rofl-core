/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <rofl/common/cmemory.h>
#include <stdlib.h>


void check_index_operator();
void check_insert();


int
main(int args, char** argv)
{
	check_index_operator();
	check_insert();
	//check_remove();

	return EXIT_SUCCESS;
}


void
check_index_operator()
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


void
check_insert()
{

	size_t size = 16;

	cmemory a1(size + 4, 0, 0);

	printf("cmemory::insert() check\n");

	/*
	 * test vector
	 */
	memset(a1.somem(), 0x55, a1.memlen());
	a1[1] = 0x77;
	a1[2] = 0x77;
	a1[3] = 0x77;
	a1[6] = 0x33;
	a1[7] = 0x33;





	/*
	 * check insert() method
	 */
	uint8_t *ptr;
	cmemory a2(size, 0, 0);
	memset(a2.somem(), 0x55, a2.memlen());
	printf("start\n");
	printf("%s\n", a2.c_str());


	printf("adding 2 bytes at offset 4\n");
	ptr = a2.insert((unsigned int)4, 2);
	memset(ptr, 0x33, 2);
	printf("%s\n", a2.c_str());


	printf("adding 4 bytes at offset 2\n");
	ptr = a2.insert((unsigned int)2, 4);
	memset(ptr, 0x77, 4);
	printf("%s\n", a2.c_str());


	printf("removing 2 bytes at offset 1\n");
	printf("%s\n", a2.c_str());
	a2.remove((unsigned int)1, 2);


	printf("a1: %s\n", a1.c_str());
	printf("a2: %s\n", a2.c_str());

	if (a1 != a2)
	{
		exit(EXIT_FAILURE);
	}


	a1.clear();
	a2.clear();

	printf("clearing memory\n");
	printf("a1: %s\n", a1.c_str());
	printf("a2: %s\n", a2.c_str());

	if (a1 != a2)
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("cmemory::insert() success\n");
	}

}



