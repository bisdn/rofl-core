/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class cofhelloelemversionbitmapTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( cofhelloelemversionbitmapTest );
	CPPUNIT_TEST( testPack );
	CPPUNIT_TEST( testUnPack );
	CPPUNIT_TEST_SUITE_END();

private:

	rofl::openflow::cofhello_elem_versionbitmap	*elem;
	rofl::cmemory *mem;

public:
	void setUp();
	void tearDown();

	void testPack();
	void testUnPack();
};

