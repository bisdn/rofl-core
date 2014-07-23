/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofflowmod.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofflowmod_test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofflowmod_test );
	CPPUNIT_TEST( testFlowMod10 );
	CPPUNIT_TEST( testFlowMod13 );
	CPPUNIT_TEST_SUITE_END();

private:


public:
	void setUp();
	void tearDown();

	void testFlowMod10();
	void testFlowMod13();
};

