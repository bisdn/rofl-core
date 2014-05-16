/*
 * ctimerid_test.h
 *
 *  Created on: 07.04.2014
 *      Author: andreas
 */

#ifndef CTIMERID_TEST_H_
#define CTIMERID_TEST_H_

#include <map>
#include "rofl/common/ctimerid.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class ctimerid_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( ctimerid_test );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testCopyConstructor );
	CPPUNIT_TEST( testAssignmentOperator );
	CPPUNIT_TEST( testComparisonOperator );
	CPPUNIT_TEST( testGetterAndSetter );
	CPPUNIT_TEST( testTimerIdWithMap );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testCopyConstructor();
	void testAssignmentOperator();
	void testComparisonOperator();
	void testGetterAndSetter();
	void testTimerIdWithMap();
};

#endif /* CTIMERID_TEST_H_ */
