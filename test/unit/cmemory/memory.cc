/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <rofl/common/cmemory.h>
#include <stdlib.h>

using namespace rofl;

void check_index_operator();
void check_insert();
void check_add_operator();



int
main(int args, char** argv)
{
	check_index_operator();
	check_insert();
	//check_remove();
	check_add_operator();

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

	std::cerr << "a1: " << a1 << std::endl;
	std::cerr << "a2: " << a2 << std::endl;

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

	cmemory a1(size + 4);

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
	cmemory a2(size);
	memset(a2.somem(), 0x55, a2.memlen());
	std::cerr << "start" << std::endl << a2 << std::endl;

	std::cerr << "adding 2 bytes at offset 4" << std::endl;
	ptr = a2.insert((unsigned int)4, 2);
	memset(ptr, 0x33, 2);
	std::cerr << a2 << std::endl;

	std::cerr << "adding 4 bytes at offset 2" << std::endl;
	ptr = a2.insert((unsigned int)2, 4);
	memset(ptr, 0x77, 4);
	std::cerr << a2 << std::endl;

	std::cerr << "removing 2 bytes at offset 1" << std::endl;
	std::cerr << a2 << std::endl;
	a2.remove((unsigned int)1, 2);

	std::cerr << "a1: " << a1 << std::endl;
	std::cerr << "a2: " << a2 << std::endl;

	if (a1 != a2)
	{
		exit(EXIT_FAILURE);
	}


	a1.clear();
	a2.clear();

	std::cerr << "clearing memory" << std::endl;
	std::cerr << "a1: " << a1 << std::endl;
	std::cerr << "a2: " << a2 << std::endl;

	if (a1 != a2)
	{
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("cmemory::insert() success\n");
	}
}



void
check_add_operator()
{
	uint8_t result1[8];
	for (int i = 0; i < 4; i++)
	{
		result1[i] = 0x44;
	}
	for (int i = 4; i < 8; i++)
	{
		result1[i] = 0x66;
	}


	cmemory mem1(4);
	for (unsigned int i = 0; i < mem1.memlen(); ++i)
	{
		mem1[i] = 0x44;
	}

	cmemory mem2(4);
	for (unsigned int i = 0; i < mem2.memlen(); ++i)
	{
		mem2[i] = 0x66;
	}

	printf("test: cmemory::operator+=() ... ");
	mem1 += mem2;

	if (memcmp(result1, mem1.somem(), mem1.memlen()))
	{
		std::cerr << " failed." <<  mem1 << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cerr << " success." << std::endl;
	}
}


