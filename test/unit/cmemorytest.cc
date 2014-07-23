/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "cmemorytest.h"
#include <stdlib.h>

using namespace rofl;

CPPUNIT_TEST_SUITE_REGISTRATION( cmemoryTest );


void
cmemoryTest::setUp()
{
	size_t mlen = blocklen = 64;

	for (unsigned int i = 0; i < 5; ++i) {
		uint8_t* m = (uint8_t*)0;

		m = (uint8_t*)::calloc(1, mlen);
		assert(NULL != m);

		blocks.push_back(std::pair<uint8_t*, size_t>(m, mlen));
	}

	for (unsigned int i = 0; i < mlen; i++) {
		memset(blocks[0].first, 0x00, mlen);
	}
	for (unsigned int i = 0; i < mlen; i++) {
		memset(blocks[1].first, 0xa1, mlen);
	}
	for (unsigned int i = 0; i < mlen; i++) {
		memset(blocks[2].first, 0xb2, mlen);
	}
	for (unsigned int i = 0; i < mlen; i++) {
		memset(blocks[3].first, 0xc3, mlen);
	}
	for (unsigned int i = 0; i < mlen; i++) {
		memset(blocks[4].first, 0xd4, mlen);
	}

}



void
cmemoryTest::tearDown()
{
	for (std::vector<std::pair<uint8_t*, size_t> >::iterator
			it = blocks.begin(); it != blocks.end(); ++it) {
		::free((*it).first);
	}
}



void
cmemoryTest::testConstructors()
{
	cmemory mem1(blocklen);
	//std::cerr << "cmemory under test:" << std::endl << mem1;
	CPPUNIT_ASSERT(mem1.memlen() == blocklen); // check length field
	CPPUNIT_ASSERT(!memcmp(mem1.somem(), blocks[0].first, blocklen)); // should be initialized with all bytes set to 0

	cmemory mem2(blocks[1].first, blocks[1].second);
	//std::cerr << "cmemory under test:" << std::endl << mem2;
	CPPUNIT_ASSERT(mem2.memlen() == blocks[1].second); // check length field
	CPPUNIT_ASSERT(!memcmp(mem2.somem(), blocks[1].first, blocks[1].second)); // should be initialized with all bytes set to 0xa1
}



void
cmemoryTest::testAssignmentOperator()
{
	cmemory mem1(blocks[1].first, blocks[1].second);
	//std::cerr << "cmemory under test:" << std::endl << mem1;

	cmemory mem2;

	mem2 = mem1;
	CPPUNIT_ASSERT(mem1 == mem2);
	CPPUNIT_ASSERT(!memcmp(mem1.somem(), blocks[1].first, blocks[1].second)); // should be initialized with all bytes set to 0xa1
	CPPUNIT_ASSERT(!memcmp(mem2.somem(), blocks[1].first, blocks[1].second)); // should be initialized with all bytes set to 0xa1
	CPPUNIT_ASSERT(!memcmp(mem2.somem(), mem1.somem(), mem1.memlen())); // should be initialized with all bytes set to 0xa1
	CPPUNIT_ASSERT(!memcmp(mem1.somem(), mem2.somem(), mem2.memlen())); // should be initialized with all bytes set to 0xa1
}



void
cmemoryTest::testMethodAssign()
{
	cmemory mem;
	mem.assign(blocks[1].first, blocks[1].second);
	CPPUNIT_ASSERT(mem.memlen() == blocks[1].second); // check length field
	CPPUNIT_ASSERT(!memcmp(mem.somem(), blocks[1].first, blocks[1].second)); // should be initialized with all bytes set to 0xa1

	mem.assign(NULL, 0);
	CPPUNIT_ASSERT(mem.memlen() == 0); // check length field
	//std::cerr << "cmemory under test:" << std::endl << mem;
}


void
cmemoryTest::testIndexOperator()
{
	cmemory mem(blocklen);

	for (unsigned int i = 0; i < blocklen; i++) {
		mem[i] = 0xa1;
	}
	CPPUNIT_ASSERT(mem.memlen() == blocks[1].second); // check length field
	CPPUNIT_ASSERT(!memcmp(mem.somem(), blocks[1].first, blocks[1].second)); // should be initialized with all bytes set to 0xa1
}


void
cmemoryTest::testAddOperator()
{
	cmemory mem1(blocks[1].first, blocks[1].second);
	cmemory mem2(blocks[2].first, blocks[2].second);

	cmemory mem3(blocks[1].second + blocks[2].second);
	memcpy(mem3.somem(), blocks[1].first, blocks[1].second);
	memcpy(mem3.somem() + blocks[1].second, blocks[2].first, blocks[2].second);

	//std::cerr << "cmemory under test: mem1" << std::endl << mem1;
	//std::cerr << "cmemory under test: mem2" << std::endl << mem2;
	//std::cerr << "cmemory under test: mem3" << std::endl << mem3;
	CPPUNIT_ASSERT((mem1 + mem2).memlen() == mem3.memlen());
	CPPUNIT_ASSERT((mem1 + mem2).memlen() == (blocks[1].second + blocks[2].second));
	CPPUNIT_ASSERT((mem1 + mem2) == mem3);
	mem1 += mem2;
	//std::cerr << "cmemory under test: mem1" << std::endl << mem1;
	CPPUNIT_ASSERT(mem1 == mem3);
}



void
cmemoryTest::testMethodResize()
{
	cmemory mem;

	//std::cerr << "cmemory under test: mem" << std::endl << mem;
	CPPUNIT_ASSERT(mem.memlen() == 0);
	mem.resize(32);
	//std::cerr << "cmemory under test: mem" << std::endl << mem;
	CPPUNIT_ASSERT(mem.memlen() == 32);
	mem.resize(44);
	//std::cerr << "cmemory under test: mem" << std::endl << mem;
	CPPUNIT_ASSERT(mem.memlen() == 44);
	mem.resize(10);
	//std::cerr << "cmemory under test: mem" << std::endl << mem;
	CPPUNIT_ASSERT(mem.memlen() == 10);
}



void
cmemoryTest::testMethodPack()
{
	cmemory mem(blocks[1].first, blocks[1].second);

	cmemory packed(blocks[1].second);

	mem.pack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(mem == packed);
}



void
cmemoryTest::testMethodUnpack()
{
	cmemory packed(blocks[1].second);
	for (unsigned int i = 0; i < packed.memlen(); ++i) {
		packed[i] = 0xa1;
	}

	cmemory mem;

	mem.unpack(packed.somem(), packed.memlen());

	CPPUNIT_ASSERT(mem == packed);
	CPPUNIT_ASSERT(!memcmp(mem.somem(), blocks[1].first, blocks[1].second)); // should be initialized with all bytes set to 0xa1
}




