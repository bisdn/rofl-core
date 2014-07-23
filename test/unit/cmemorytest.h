/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <assert.h>

#include <set>

#include "rofl/common/cmemory.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace rofl;

class cmemoryTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cmemoryTest );
	CPPUNIT_TEST( testConstructors );
	CPPUNIT_TEST( testAssignmentOperator );
	CPPUNIT_TEST( testMethodAssign );
	CPPUNIT_TEST( testMethodResize );
	CPPUNIT_TEST( testIndexOperator );
	CPPUNIT_TEST( testAddOperator );
	CPPUNIT_TEST( testMethodPack );
	CPPUNIT_TEST( testMethodUnpack );
	CPPUNIT_TEST_SUITE_END();

private:

	size_t blocklen;
	std::vector<std::pair<uint8_t*, size_t> > blocks;


public:
	void setUp();
	void tearDown();

	void testConstructors();
	void testAssignmentOperator();
	void testMethodAssign();
	void testMethodResize();
	void testIndexOperator();
	void testAddOperator();
	void testMethodPack();
	void testMethodUnpack();
};

