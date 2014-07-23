/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/protocols/ficmpv6frame.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace rofl;

class ficmpv6frameTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( ficmpv6frameTest );
	CPPUNIT_TEST( testICMPv6Type );
	CPPUNIT_TEST( testICMPv6Code );
	CPPUNIT_TEST( testICMPv6ShortFrame );
	CPPUNIT_TEST( testICMPv6OptionWithLengthNull );
	CPPUNIT_TEST_SUITE_END();

private:

	cmemory 		*mem;
	cmemory			*mem_t;
	cmemory			*mem_n;
	ficmpv6frame 	*icmpv6;
	ficmpv6frame	*icmpv6_too_short;
	ficmpv6frame	*icmpv6_option_len_null;

public:
	void setUp();
	void tearDown();

	void testICMPv6Type();
	void testICMPv6Code();
	void testICMPv6ShortFrame();
	void testICMPv6OptionWithLengthNull();
};

