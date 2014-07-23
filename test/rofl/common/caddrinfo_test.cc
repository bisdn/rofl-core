/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "caddrinfo_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( caddrinfo_test );

#if defined DEBUG
//#undef DEBUG
#endif

void
caddrinfo_test::setUp()
{
}



void
caddrinfo_test::tearDown()
{
}



void
caddrinfo_test::testAddrInfo()
{
	rofl::caddrinfo ainfo;

	std::cerr << "ainfo:" << std::endl << ainfo;
}


