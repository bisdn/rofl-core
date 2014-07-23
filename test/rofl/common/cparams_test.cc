/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cparams_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cparams_test );

#if defined DEBUG
//#undef DEBUG
#endif

void
cparams_test::setUp()
{
}



void
cparams_test::tearDown()
{
}



void
cparams_test::testAll()
{
	std::string s_test1 = std::string("a test string [1] ...");
	std::string s_test2 = std::string("a test string [2] ...");

	rofl::cparams p;

	p.add_param("key1") = s_test1;
	p.add_param("key2") = s_test2;

	CPPUNIT_ASSERT(p.get_params().size() == 2);

	try {
		p.get_param("key1");
	} catch (rofl::eParamNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		p.get_param("key2");
	} catch (rofl::eParamNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	p.drop_param("key1");

	try {
		p.get_param("key1");
		CPPUNIT_ASSERT(false);
	} catch (rofl::eParamNotFound& e) {
		// correct, key1 was removed before
	}

	rofl::cparam* p1 = &(p.add_param("key2"));
	rofl::cparam* p2 = &(p.set_param("key2"));
	CPPUNIT_ASSERT(p1 == p2);

	if (p.has_param("key1")) {
		CPPUNIT_ASSERT(false);
	}

	if (not p.has_param("key2")) {
		CPPUNIT_ASSERT(false);
	}
}


