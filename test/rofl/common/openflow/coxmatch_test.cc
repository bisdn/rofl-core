#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "coxmatch_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( coxmatch_test );

#if defined DEBUG
#undef DEBUG
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
	CPPUNIT_ASSERT(oxm.get_u8value() == 0xb1);
	CPPUNIT_ASSERT(oxm.get_u8mask() == 0xff);
	CPPUNIT_ASSERT(oxm.get_u8masked_value() == 0xb1);

	oxm.set_u8value(0xd1);

	CPPUNIT_ASSERT(oxm.get_u8value() == 0xd1);
}



void
coxmatch_test::test1ByteHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint8_t)0xb1, (uint8_t)0xc1);

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
	CPPUNIT_ASSERT(oxm.get_u8value() == 0xb1);
	CPPUNIT_ASSERT(oxm.get_u8mask() == 0xc1);
	CPPUNIT_ASSERT(oxm.get_u8masked_value() == (0xb1 & 0xc1));

	oxm.set_u8value(0xd1);
	oxm.set_u8mask(0xe1);

	CPPUNIT_ASSERT(oxm.get_u8value() == 0xd1);
	CPPUNIT_ASSERT(oxm.get_u8mask() == 0xe1);
	CPPUNIT_ASSERT(oxm.get_u8masked_value() == (0xd1 & 0xe1));
}



void
coxmatch_test::test2Bytes()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint16_t)0xb1b2);

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
	CPPUNIT_ASSERT(oxm.get_u16value() == 0xb1b2);
	CPPUNIT_ASSERT(oxm.get_u16mask() == 0xffff);
	CPPUNIT_ASSERT(oxm.get_u16masked_value() == 0xb1b2);

	oxm.set_u16value(0xd1d2);

	CPPUNIT_ASSERT(oxm.get_u16value() == 0xd1d2);
}



void
coxmatch_test::test2BytesHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint16_t)0xb1b2, (uint16_t)0xc1c2);

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
	CPPUNIT_ASSERT(oxm.get_u16value() == 0xb1b2);
	CPPUNIT_ASSERT(oxm.get_u16mask() == 0xc1c2);
	CPPUNIT_ASSERT(oxm.get_u16masked_value() == (0xb1b2 & 0xc1c2));

	oxm.set_u16value(0xd1d2);
	oxm.set_u16mask(0xe1e2);

	CPPUNIT_ASSERT(oxm.get_u16value() == 0xd1d2);
	CPPUNIT_ASSERT(oxm.get_u16mask() == 0xe1e2);
	CPPUNIT_ASSERT(oxm.get_u16masked_value() == (0xd1d2 & 0xe1e2));
}



void
coxmatch_test::test4Bytes()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint32_t)0xb1b2b3b4);

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
	CPPUNIT_ASSERT(oxm.get_u32value() == 0xb1b2b3b4);
	CPPUNIT_ASSERT(oxm.get_u32mask() == 0xffffffff);
	CPPUNIT_ASSERT(oxm.get_u32masked_value() == 0xb1b2b3b4);

	oxm.set_u32value(0xd1d2d3d4);

	CPPUNIT_ASSERT(oxm.get_u32value() == 0xd1d2d3d4);
}



void
coxmatch_test::test4BytesHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint32_t)0xb1b2b3b4, (uint32_t)0xc1c2c3c4);

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
	CPPUNIT_ASSERT(oxm.get_u32value() == 0xb1b2b3b4);
	CPPUNIT_ASSERT(oxm.get_u32mask() == 0xc1c2c3c4);
	CPPUNIT_ASSERT(oxm.get_u32masked_value() == (0xb1b2b3b4 & 0xc1c2c3c4));

	oxm.set_u32value(0xd1d2d3d4);
	oxm.set_u32mask(0xe1e2e3e4);

	CPPUNIT_ASSERT(oxm.get_u32value() == 0xd1d2d3d4);
	CPPUNIT_ASSERT(oxm.get_u32mask() == 0xe1e2e3e4);
	CPPUNIT_ASSERT(oxm.get_u32masked_value() == (0xd1d2d3d4 & 0xe1e2e3e4));
}



void
coxmatch_test::test6Bytes()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));

	CPPUNIT_ASSERT(oxm.length() == 10);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(10);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 & ~0x01;
	test[3] = 0xa4;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u48value() == rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
	CPPUNIT_ASSERT(oxm.get_u48mask() == rofl::cmacaddr("ff:ff:ff:ff:ff:ff"));
	CPPUNIT_ASSERT(oxm.get_u48masked_value() == rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));

	oxm.set_u48value(rofl::cmacaddr("d1:d2:d3:d4:d5:d6"));

	CPPUNIT_ASSERT(oxm.get_u48value() == rofl::cmacaddr("d1:d2:d3:d4:d5:d6"));
}



void
coxmatch_test::test6BytesHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, rofl::cmacaddr("b1:b2:b3:b4:b5:b6"), rofl::cmacaddr("c1:c2:c3:c4:c5:c6"));

	CPPUNIT_ASSERT(oxm.length() == 16);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(16);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 | 0x01;
	test[3] = 0xa4;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	test[10] = 0xc1;
	test[11] = 0xc2;
	test[12] = 0xc3;
	test[13] = 0xc4;
	test[14] = 0xc5;
	test[15] = 0xc6;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u48value() == rofl::cmacaddr("b1:b2:b3:b4:b5:b6"));
	CPPUNIT_ASSERT(oxm.get_u48mask() == rofl::cmacaddr("c1:c2:c3:c4:c5:c6"));
	CPPUNIT_ASSERT(oxm.get_u48masked_value() == (rofl::cmacaddr("b1:b2:b3:b4:b5:b6") & rofl::cmacaddr("c1:c2:c3:c4:c5:c6")));

	oxm.set_u48value(rofl::cmacaddr("d1:d2:d3:d4:d5:d6"));
	oxm.set_u48mask(rofl::cmacaddr("e1:e2:e3:e4:e5:e6"));

	CPPUNIT_ASSERT(oxm.get_u48value() == rofl::cmacaddr("d1:d2:d3:d4:d5:d6"));
	CPPUNIT_ASSERT(oxm.get_u48mask() == rofl::cmacaddr("e1:e2:e3:e4:e5:e6"));
	CPPUNIT_ASSERT(oxm.get_u48masked_value() == (rofl::cmacaddr("d1:d2:d3:d4:d5:d6") & rofl::cmacaddr("e1:e2:e3:e4:e5:e6")));
}



void
coxmatch_test::test8Bytes()
{
	uint32_t oxm_id = 0xa1a2a3a4 & ~HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint64_t)0xb1b2b3b4b5b6b7b8);

	CPPUNIT_ASSERT(oxm.length() == 12);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 & ~0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(12);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 & ~0x01;
	test[3] = 0xa4;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	test[10] = 0xb7;
	test[11] = 0xb8;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u64value() == 0xb1b2b3b4b5b6b7b8);
	CPPUNIT_ASSERT(oxm.get_u64mask() == 0xffffffffffffffff);
	CPPUNIT_ASSERT(oxm.get_u64masked_value() == 0xb1b2b3b4b5b6b7b8);

	oxm.set_u64value(0xd1d2d3d4d5d6d7d8);

	CPPUNIT_ASSERT(oxm.get_u64value() == 0xd1d2d3d4d5d6d7d8);
}



void
coxmatch_test::test8BytesHasMask()
{
	uint32_t oxm_id = 0xa1a2a3a4 | HAS_MASK_FLAG;

	rofl::openflow::coxmatch oxm(oxm_id, (uint64_t)0xb1b2b3b4b5b6b7b8, (uint64_t)0xc1c2c3c4c5c6c7c8);

	CPPUNIT_ASSERT(oxm.length() == 20);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0xa1a2);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == ((0xa3 | 0x01) >> 1));
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 0xa4);

	rofl::cmemory mem(oxm.length());
	oxm.pack(mem.somem(), mem.memlen());

	rofl::cmemory test(20);
	test[0] = 0xa1;
	test[1] = 0xa2;
	test[2] = 0xa3 | 0x01;
	test[3] = 0xa4;
	test[4] = 0xb1;
	test[5] = 0xb2;
	test[6] = 0xb3;
	test[7] = 0xb4;
	test[8] = 0xb5;
	test[9] = 0xb6;
	test[10] = 0xb7;
	test[11] = 0xb8;
	test[12] = 0xc1;
	test[13] = 0xc2;
	test[14] = 0xc3;
	test[15] = 0xc4;
	test[16] = 0xc5;
	test[17] = 0xc6;
	test[18] = 0xc7;
	test[19] = 0xc8;

	CPPUNIT_ASSERT(mem == test);
	CPPUNIT_ASSERT(oxm.get_u64value() == 0xb1b2b3b4b5b6b7b8);
	CPPUNIT_ASSERT(oxm.get_u64mask() == 0xc1c2c3c4c5c6c7c8);
	CPPUNIT_ASSERT(oxm.get_u64masked_value() == (0xb1b2b3b4b5b6b7b8 & 0xc1c2c3c4c5c6c7c8));

	oxm.set_u64value(0xd1d2d3d4d5d6d7d8);
	oxm.set_u64mask(0xe1e2e3e4e5e6e7e8);

	CPPUNIT_ASSERT(oxm.get_u64value() == 0xd1d2d3d4d5d6d7d8);
	CPPUNIT_ASSERT(oxm.get_u64mask() == 0xe1e2e3e4e5e6e7e8);
	CPPUNIT_ASSERT(oxm.get_u64masked_value() == (0xd1d2d3d4d5d6d7d8 & 0xe1e2e3e4e5e6e7e8));
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





