#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofbucketcounter_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofbucketcounter_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofbucketcounter_test::setUp()
{
}



void
cofbucketcounter_test::tearDown()
{
}



void
cofbucketcounter_test::testDefaultConstructor()
{
	rofl::openflow::cofbucket_counter bc;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == bc.get_version());
}



void
cofbucketcounter_test::testCopyConstructor()
{
	rofl::openflow::cofbucket_counter bc(rofl::openflow13::OFP_VERSION);

	bc.set_packet_count(0xa1a2a3a4a5a6a7a8);
	bc.set_byte_count(0xb1b2b3b4b5b6b7b8);


	rofl::cmemory mbc(bc.length());
	bc.pack(mbc.somem(), mbc.memlen());

#ifdef DEBUG
	std::cerr << "bc:" << std::endl << bc;
	std::cerr << "mbc:" << std::endl << mbc;
#endif

	rofl::openflow::cofbucket_counter clone(bc);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(mbc == mclone);
}




void
cofbucketcounter_test::testPackUnpack()
{
	rofl::openflow::cofbucket_counter bc(rofl::openflow13::OFP_VERSION);

	bc.set_packet_count(0xa1a2a3a4a5a6a7a8);
	bc.set_byte_count(0xb1b2b3b4b5b6b7b8);


	rofl::cmemory mbc(bc.length());
	bc.pack(mbc.somem(), mbc.memlen());

#ifdef DEBUG
	std::cerr << "bc:" << std::endl << bc;
	std::cerr << "mbc:" << std::endl << mbc;
#endif

	rofl::openflow::cofbucket_counter clone(bc);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(mbc == mclone);
}

