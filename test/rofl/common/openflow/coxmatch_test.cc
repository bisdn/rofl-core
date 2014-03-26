#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coxmatch_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( coxmatch_test );

#if defined DEBUG
//#undef DEBUG
#endif

void
coxmatch_test::setUp()
{
}



void
coxmatch_test::tearDown()
{
}



void
coxmatch_test::test1Byte()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint8_t)0xb1);

	std::cerr << oxm;

	CPPUNIT_ASSERT(oxm.length() == 5);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(5);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 & ~0x01;
	test[3] = 0xa4;
	test[4] = 0xb1;

	CPPUNIT_ASSERT(mem == test);
}



void
coxmatch_test::test1ByteHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint8_t)0xb1, (uint8_t)0xc1);

	std::cerr << oxm;

	CPPUNIT_ASSERT(oxm.length() == 6);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 | 0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(6);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 | 0x01;
	test[3] = 0xa4;
	test[4] = 0xb1;
	test[5] = 0xc1;

	CPPUNIT_ASSERT(mem == test);
}



void
coxmatch_test::test2Bytes()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint16_t)0xb1b2);

	std::cerr << oxm;

	CPPUNIT_ASSERT(oxm.length() == 6);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(6);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 & ~0x01;
	test[3] = 0xa4;
	test[4] = 0xb1;
	test[5] = 0xb2;

	CPPUNIT_ASSERT(mem == test);
}



void
coxmatch_test::test2BytesHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint16_t)0xb1b2, (uint16_t)0xc1c2);

	std::cerr << oxm;

	CPPUNIT_ASSERT(oxm.length() == 8);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 | 0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(8);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 | 0x01;
	test[3] = 0xa4;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xc1;
	test[7] = 0xc2;

	CPPUNIT_ASSERT(mem == test);
}



void
coxmatch_test::test4Bytes()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint32_t)0xb1b2b3b4);

	std::cerr << oxm;

	CPPUNIT_ASSERT(oxm.length() == 8);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(8);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 & ~0x01;
	test[3] = 0xa4;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;

	CPPUNIT_ASSERT(mem == test);
}



void
coxmatch_test::test4BytesHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint32_t)0xb1b2b3b4, (uint32_t)0xc1c2c3c4);

	std::cerr << oxm;

	CPPUNIT_ASSERT(oxm.length() == 12);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 | 0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(12);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 | 0x01;
	test[3] = 0xa4;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xc1;
	test[9] = 0xc2;
	test[10] = 0xc3;
	test[11] = 0xc4;

	CPPUNIT_ASSERT(mem == test);
}



void
coxmatch_test::test6Bytes()
{

}



void
coxmatch_test::test6BytesHasMask()
{

}



void
coxmatch_test::test8Bytes()
{

}



void
coxmatch_test::test8BytesHasMask()
{
	exit(0);
}



void
coxmatch_test::testCopyConstructor()
{
#if 0
	rofl::openflow::coxmatch role(rofl::openflow13::OFP_VERSION);

	role.set_role(0x11121314);
	role.set_generation_id(0x2122232425262728);


	rofl::cmemory mrole(role.length());
	role.pack(mrole.somem(), mrole.memlen());

#ifdef DEBUG
	std::cerr << "role:" << std::endl << role;
	std::cerr << "mac:" << std::endl << mrole;
#endif

	rofl::openflow::coxmatch clone(role);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(mrole == mclone);
#endif
}





