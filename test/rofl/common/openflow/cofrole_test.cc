#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofrole_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofrole_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofrole_test::setUp()
{
}



void
cofrole_test::tearDown()
{
}



void
cofrole_test::testDefaultConstructor()
{
	rofl::openflow::cofrole role;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == role.get_version());
}



void
cofrole_test::testCopyConstructor()
{
	rofl::openflow::cofrole role(rofl::openflow13::OFP_VERSION);

	role.set_role(0x11121314);
	role.set_generation_id(0x2122232425262728);


	rofl::cmemory mrole(role.length());
	role.pack(mrole.somem(), mrole.memlen());

#ifdef DEBUG
	std::cerr << "role:" << std::endl << role;
	std::cerr << "mac:" << std::endl << mrole;
#endif

	rofl::openflow::cofrole clone(role);

	rofl::cmemory mclone(clone.length());
	clone.pack(mclone.somem(), mclone.memlen());

#ifdef DEBUG
	std::cerr << "clone:" << std::endl << clone;
	std::cerr << "mclone:" << std::endl << mclone;
#endif

	CPPUNIT_ASSERT(mrole == mclone);
}




void
cofrole_test::testPackUnpack()
{
	/*
	 * pack
	 */
	rofl::openflow::cofrole arole(rofl::openflow13::OFP_VERSION);

	arole.set_role(0x11121314);
	arole.set_generation_id(0x2122232425262728);

	rofl::cmemory mrole(arole.length());
	arole.pack(mrole.somem(), mrole.memlen());

	struct rofl::openflow::cofrole::role_t *r = (struct rofl::openflow::cofrole::role_t*)mrole.somem();

#ifdef DEBUG
	std::cerr << "arole:" << std::endl << arole;
	std::cerr << "mrole:" << std::endl << mrole;
#endif

	CPPUNIT_ASSERT(mrole.memlen() == sizeof(struct rofl::openflow::cofrole::role_t));
	CPPUNIT_ASSERT(be32toh(r->role) == 0x11121314);
	CPPUNIT_ASSERT(be64toh(r->generation_id) == 0x2122232425262728);

	/*
	 * unpack
	 */

	rofl::openflow::cofrole brole(rofl::openflow13::OFP_VERSION);

	brole.unpack(mrole.somem(), mrole.memlen());

#ifdef DEBUG
	std::cerr << "mrole:" << std::endl << mrole;
	std::cerr << "brole:" << std::endl << brole;
#endif

	CPPUNIT_ASSERT(brole.get_role() == 0x11121314);
	CPPUNIT_ASSERT(brole.get_generation_id() == 0x2122232425262728);
}




