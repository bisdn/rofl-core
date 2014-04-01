#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cparam_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cparam_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cparam_test::setUp()
{
}



void
cparam_test::tearDown()
{
}



void
cparam_test::testAll()
{
	std::string s_test = std::string("a test string ...");

	rofl::cparam p;

	p.set_string() = s_test;

	rofl::cparam pp(s_test);

	rofl::cparam clone(p);

	CPPUNIT_ASSERT(p == clone);
	CPPUNIT_ASSERT(pp.get_string() == s_test);
	CPPUNIT_ASSERT(clone.get_string() == s_test);
}



void
cparam_test::testInt()
{
	rofl::cparam 	p;
	int 			value = 111111;
	std::string		s_value("111111");


	// forward
	p.set_int(value);

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_int() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_int() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);

	// backward
	p.set_string() = s_value;

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_int() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_int() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);
}



void
cparam_test::testUnsignedInt()
{
	rofl::cparam 	p;
	unsigned int 	value = 222222;
	std::string		s_value("222222");

	// forward
	p.set_uint(value);

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_uint() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_uint() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);

	// backward
	p.set_string() = s_value;

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_uint() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_uint() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);
}



void
cparam_test::testBool()
{
	rofl::cparam 	p;
	bool 			value = true;
	std::string		s_value("true");

	// forward
	p.set_bool(value);

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_bool() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_bool() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);

	// backward
	p.set_string() = s_value;

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_bool() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_bool() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);


	// backward ("yes")
	s_value = "yes";
	p.set_string() = s_value;

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_bool() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_bool() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);


	// backward ("on")
	s_value = "on";
	p.set_string() = s_value;

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_bool() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_bool() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);



	// backward ("1")
	s_value = "1";
	p.set_string() = s_value;

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_bool() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_bool() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);


	// backward ("TrUe")
	s_value = "TrUe";
	p.set_string() = s_value;

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_bool() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_bool() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);


	// backward ("")
	s_value = "";
	value = false;
	p.set_string() = s_value;

#ifdef DEBUG
	std::cerr << p;
	std::cerr << p.get_bool() << std::endl;
#endif

	CPPUNIT_ASSERT(p.get_bool() == value);
	CPPUNIT_ASSERT(p.get_string() == s_value);
}


