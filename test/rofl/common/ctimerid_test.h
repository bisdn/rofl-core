/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
	CPPUNIT_TEST( testGetterAndSetter );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testDefaultConstructor();
	void testCopyConstructor();
	void testAssignmentOperator();
	void testGetterAndSetter();
};

#endif /* CTIMERID_TEST_H_ */
