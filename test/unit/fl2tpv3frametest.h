#include "rofl/common/protocols/fl2tpv3frame.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/ciosrv.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace rofl;

class fl2tpv3frameTest : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( fl2tpv3frameTest );
	CPPUNIT_TEST( testEncapIpVersion );
	CPPUNIT_TEST( testEncapIpSessionID );
	CPPUNIT_TEST( testEncapIpLength );
	CPPUNIT_TEST( testEncapIpTbit );
	CPPUNIT_TEST( testEncapIpLbit );
	CPPUNIT_TEST( testEncapIpSbit );
	CPPUNIT_TEST( testEncapIpCCID );
	CPPUNIT_TEST( testEncapIpNs );
	CPPUNIT_TEST( testEncapIpNr );
	CPPUNIT_TEST( testEncapUdpVersion );
	CPPUNIT_TEST( testEncapUdpSessionID );
	CPPUNIT_TEST( testEncapUdpLength );
	CPPUNIT_TEST( testEncapUdpTbit );
	CPPUNIT_TEST( testEncapUdpLbit );
	CPPUNIT_TEST( testEncapUdpSbit );
	CPPUNIT_TEST( testEncapUdpCCID );
	CPPUNIT_TEST( testEncapUdpNs );
	CPPUNIT_TEST( testEncapUdpNr );
	CPPUNIT_TEST_SUITE_END();

private:

	uint32_t		sid_ctl;
	uint32_t		sid_data;
	uint8_t			version;
	uint16_t 		ctlfield;
	uint32_t 		ccid;
	uint16_t		Ns;
	uint16_t		Nr;

	cmemory			*pattern_l2tpv3_encap_ip_ctl;
	cmemory			*pattern_l2tpv3_encap_ip_data;
	cmemory			*pattern_l2tpv3_encap_udp_ctl;
	cmemory			*pattern_l2tpv3_encap_udp_data;

public:
	void setUp();
	void tearDown();

	void testEncapIpVersion();
	void testEncapIpSessionID();
	void testEncapIpLength();
	void testEncapIpTbit();
	void testEncapIpLbit();
	void testEncapIpSbit();
	void testEncapIpCCID();
	void testEncapIpNs();
	void testEncapIpNr();
	void testEncapUdpVersion();
	void testEncapUdpSessionID();
	void testEncapUdpLength();
	void testEncapUdpTbit();
	void testEncapUdpLbit();
	void testEncapUdpSbit();
	void testEncapUdpCCID();
	void testEncapUdpNs();
	void testEncapUdpNr();
};

