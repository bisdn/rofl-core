#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "coxmatchtest.h"
#include <stdlib.h>

using namespace rofl;

CPPUNIT_TEST_SUITE_REGISTRATION( coxmatchTest );


void
coxmatchTest::setUp()
{

}



void
coxmatchTest::tearDown()
{

}



void
coxmatchTest::testInPort()
{
	uint32_t port_no = 0xddccbbaa;
	coxmatch_ofb_in_port oxm = coxmatch_ofb_in_port(port_no);

	cmemory m(2*sizeof(uint32_t));
	m[0] = 0x80;	// class: here => basic (0x8000)
	m[1] = 0x00;	// class: here => basic (0x8000)
	m[2] = 0x00;  	// type + has-mask flag
	m[3] = 0x04; 	// length
	m[4] = ((uint8_t*)&port_no)[3];
	m[5] = ((uint8_t*)&port_no)[2];
	m[6] = ((uint8_t*)&port_no)[1];
	m[7] = ((uint8_t*)&port_no)[0];

	printf("oxm: %s\nm:    %s\n", oxm.c_str(), m.c_str());
	printf("oxm.u32value() 0x%x port_no 0x%x\n", oxm.u32value(), port_no);

	CPPUNIT_ASSERT(oxm.memlen() == m.memlen());
	CPPUNIT_ASSERT(!memcmp(oxm.somem(), m.somem(), oxm.memlen()));
	CPPUNIT_ASSERT(oxm.u32value() == port_no);
	CPPUNIT_ASSERT(oxm.uint32_value() == port_no);
	CPPUNIT_ASSERT(oxm.get_oxm_hasmask() == false);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0x8000);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == 0);
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 1*sizeof(uint32_t));
}



void
coxmatchTest::testIPv4Src()
{
	caddress addr(AF_INET, "192.168.88.99");
	coxmatch_ofb_ipv4_src oxm = coxmatch_ofb_ipv4_src(addr);

	cmemory m(2*sizeof(uint32_t));
	m[0] = 0x80;	// class: here => basic (0x8000)
	m[1] = 0x00;	// class: here => basic (0x8000)
	m[2] = 0x16;  	// type + has-mask flag (=(dez:11) << 1 + flag:0)
	m[3] = 0x04; 	// length
	m[4] = ((uint8_t*)&(addr.ca_s4addr->sin_addr.s_addr))[0];
	m[5] = ((uint8_t*)&(addr.ca_s4addr->sin_addr.s_addr))[1];
	m[6] = ((uint8_t*)&(addr.ca_s4addr->sin_addr.s_addr))[2];
	m[7] = ((uint8_t*)&(addr.ca_s4addr->sin_addr.s_addr))[3];

	printf("oxm: %s\nm:    %s\n", oxm.c_str(), m.c_str());
	printf("oxm.u32addr() %s addr %s\n", oxm.u32addr().c_str(), addr.c_str());

	CPPUNIT_ASSERT(oxm.memlen() == m.memlen());
	CPPUNIT_ASSERT(!memcmp(oxm.somem(), m.somem(), oxm.memlen()));
	CPPUNIT_ASSERT(oxm.u32addr() == addr);
	CPPUNIT_ASSERT(oxm.get_oxm_hasmask() == false);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0x8000);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == 11);
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 1*sizeof(uint32_t));
}



void
coxmatchTest::testIPv4SrcWithMask()
{
	caddress addr(AF_INET, "192.168.88.99");
	caddress mask(AF_INET, "255.255.255.0");
	coxmatch_ofb_ipv4_src oxm = coxmatch_ofb_ipv4_src(addr, mask);

	cmemory m(3*sizeof(uint32_t));
	m[0] = 0x80;	// class: here => basic (0x8000)
	m[1] = 0x00;	// class: here => basic (0x8000)
	m[2] = 0x17;  	// type + has-mask flag (=(dez:11) << 1 + flag:0)
	m[3] = 0x08; 	// length
	m[4] = ((uint8_t*)&(addr.ca_s4addr->sin_addr.s_addr))[0];
	m[5] = ((uint8_t*)&(addr.ca_s4addr->sin_addr.s_addr))[1];
	m[6] = ((uint8_t*)&(addr.ca_s4addr->sin_addr.s_addr))[2];
	m[7] = ((uint8_t*)&(addr.ca_s4addr->sin_addr.s_addr))[3];
	m[8] = ((uint8_t*)&(mask.ca_s4addr->sin_addr.s_addr))[0];
	m[9] = ((uint8_t*)&(mask.ca_s4addr->sin_addr.s_addr))[1];
	m[10] = ((uint8_t*)&(mask.ca_s4addr->sin_addr.s_addr))[2];
	m[11] = ((uint8_t*)&(mask.ca_s4addr->sin_addr.s_addr))[3];

	printf("oxm: %s\nm:    %s\n", oxm.c_str(), m.c_str());
	printf("oxm.u32addr() %s addr %s mask %s\n", oxm.u32addr().c_str(), addr.c_str(), mask.c_str());

	CPPUNIT_ASSERT(oxm.memlen() == m.memlen());
	CPPUNIT_ASSERT(!memcmp(oxm.somem(), m.somem(), oxm.memlen()));
	CPPUNIT_ASSERT(oxm.u32addr() == (addr & mask));
	CPPUNIT_ASSERT(oxm.get_oxm_hasmask() == true);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0x8000);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == 11);
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 2*sizeof(uint32_t));
}



void
coxmatchTest::testEthType()
{
	uint16_t eth_type = 0x0800;
	coxmatch_ofb_eth_type oxm = coxmatch_ofb_eth_type(eth_type);

	cmemory m(6);
	m[0] = 0x80;	// class: here => basic (0x8000)
	m[1] = 0x00;	// class: here => basic (0x8000)
	m[2] = 0x0a;  	// type + has-mask flag (=(dez:5) << 1 + flag:0)
	m[3] = 0x02; 	// length
	m[4] = 0x08;
	m[5] = 0x00;

	printf("oxm: %s\nm:    %s\n", oxm.c_str(), m.c_str());
	printf("oxm.u16value() 0x%x \n", oxm.u16value());

	CPPUNIT_ASSERT(oxm.memlen() == m.memlen());
	CPPUNIT_ASSERT(!memcmp(oxm.somem(), m.somem(), oxm.memlen()));
	CPPUNIT_ASSERT(oxm.u16value() == eth_type);
	CPPUNIT_ASSERT(oxm.get_oxm_hasmask() == false);
	CPPUNIT_ASSERT(oxm.get_oxm_class() == 0x8000);
	CPPUNIT_ASSERT(oxm.get_oxm_field() == 5);
	CPPUNIT_ASSERT(oxm.get_oxm_length() == 1*sizeof(uint16_t));
}


