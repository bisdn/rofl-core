#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "clldpattr_test.h"

using namespace rofl::protocol::lldp;

CPPUNIT_TEST_SUITE_REGISTRATION( clldpattrTest );

#if defined DEBUG
//#undef DEBUG
#endif

void
clldpattrTest::setUp()
{
}



void
clldpattrTest::tearDown()
{
}



void
clldpattrTest::testTypeField()
{
	clldpattr attr;
	attr.set_type(0x7f);

	std::cerr << "attr:" << attr << std::endl;

	CPPUNIT_ASSERT(attr[0] == 0xfe);
	CPPUNIT_ASSERT(attr.get_type() == 0x7f);
	CPPUNIT_ASSERT(attr[1] == 0x00);
	CPPUNIT_ASSERT(attr.get_length() == 0x00);
}



void
clldpattrTest::testLengthField()
{
	clldpattr attr;
	attr.set_length(0x1ff);

	std::cerr << "attr:" << attr << std::endl;

	CPPUNIT_ASSERT(attr[0] == 0x01);
	CPPUNIT_ASSERT(attr.get_type() == 0x00);
	CPPUNIT_ASSERT(attr[1] == 0xff);
	CPPUNIT_ASSERT(attr.get_length() == 0x1ff);
}



void
clldpattrTest::testDefaultConstructor()
{
	clldpattr attr;

	CPPUNIT_ASSERT(attr.length() == sizeof(struct lldp_tlv_hdr_t));
	CPPUNIT_ASSERT(attr.get_type() == 0);
	CPPUNIT_ASSERT(attr.get_length() == 0);
	try {
		attr.get_body();
		CPPUNIT_ASSERT(false);
	} catch (eLLDPNotFound& e) {}
}



void
clldpattrTest::testCopyConstructor()
{
	clldpattr attr;

	attr.set_type(0x58);
	rofl::cmemory body(7);
	for (unsigned int i = 0; i < 7; i++) {
		body[i] = i;
	}
	attr.set_body(body);
	attr.pack();

	clldpattr clone(attr);

	std::cerr << "attr:" << attr;
	std::cerr << "clone:" << clone;

	CPPUNIT_ASSERT(clone.get_type() == 0x58);
	CPPUNIT_ASSERT(clone.get_length() == 9);
	CPPUNIT_ASSERT(clone.get_body() == body);
}



void
clldpattrTest::testPackUnpack()
{

}


void
clldpattrTest::testId()
{

}



void
clldpattrTest::testTTL()
{

}


void
clldpattrTest::testDesc()
{

}


void
clldpattrTest::testSysCaps()
{

}




