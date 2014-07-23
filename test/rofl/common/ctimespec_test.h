/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ctimespec_test.h
 *
 *  Created on: 07.04.2014
 *      Author: andreas
 */

#ifndef CTIMERID_TEST_H_
#define CTIMERID_TEST_H_

#include <map>
#include "rofl/common/ctimespec.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class ctimespec_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( ctimespec_test );
	CPPUNIT_TEST( testTimeSpec );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testTimeSpec();
};

#endif /* CTIMERID_TEST_H_ */
