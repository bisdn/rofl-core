#include "rofl/common/openflow/cofinstruction.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofinstruction_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofinstruction_test );
	CPPUNIT_TEST( test_cofinstruction );
	CPPUNIT_TEST( test_cofinstruction_goto_table );
	CPPUNIT_TEST( test_cofinstruction_apply_actions );
	CPPUNIT_TEST( test_cofinstruction_write_actions );
	CPPUNIT_TEST( test_cofinstruction_clear_actions );
	CPPUNIT_TEST( test_cofinstruction_write_metadata );
	CPPUNIT_TEST( test_cofinstruction_meter );
	CPPUNIT_TEST( test_cofinstruction_experimenter );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void test_cofinstruction();
	void test_cofinstruction_goto_table();
	void test_cofinstruction_apply_actions();
	void test_cofinstruction_write_actions();
	void test_cofinstruction_clear_actions();
	void test_cofinstruction_write_metadata();
	void test_cofinstruction_meter();
	void test_cofinstruction_experimenter();
};

