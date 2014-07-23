/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "fl2tpv3frametest.h"
#include <stdlib.h>

using namespace rofl;

CPPUNIT_TEST_SUITE_REGISTRATION( fl2tpv3frameTest );


void
fl2tpv3frameTest::setUp()
{
	sid_ctl 	= 0;
	sid_data	= 0xeeeeeeee;
	version 	= 3;
	ctlfield 	= /*Tbit:*/(1 << 15) | /*Lbit:*/(1 << 14) | /*Sbit:*/(1 << 11) | /*version:*/(version << 0);
	ccid 		= 0xcacacaca;
	Ns 			= 0x1000;
	Nr 			= 0x2000;

	pattern_l2tpv3_encap_ip_ctl = new cmemory(sizeof(struct fl2tpv3frame::l2tpv3_ctl_hdr_ip_encap_t));
	struct fl2tpv3frame::l2tpv3_ctl_hdr_ip_encap_t* hdr1 = (struct fl2tpv3frame::l2tpv3_ctl_hdr_ip_encap_t*)(pattern_l2tpv3_encap_ip_ctl->somem());

	hdr1->pseudo_session_id = htobe32(sid_ctl);
	hdr1->ctlfield 			= htobe16(ctlfield);
	hdr1->len 				= htobe16(sizeof(struct fl2tpv3frame::l2tpv3_ctl_hdr_ip_encap_t));
	hdr1->ccid 				= htobe32(ccid);
	hdr1->ns 				= htobe16(Ns);
	hdr1->nr 				= htobe16(Nr);

	pattern_l2tpv3_encap_ip_data = new cmemory(sizeof(struct fl2tpv3frame::l2tpv3_data_hdr_ip_encap_t));
	struct fl2tpv3frame::l2tpv3_data_hdr_ip_encap_t* hdr2 = (struct fl2tpv3frame::l2tpv3_data_hdr_ip_encap_t*)(pattern_l2tpv3_encap_ip_data->somem());

	hdr2->session_id		= htobe32(sid_data);

	pattern_l2tpv3_encap_udp_ctl = new cmemory(sizeof(struct fl2tpv3frame::l2tpv3_ctl_hdr_udp_encap_t));
	struct fl2tpv3frame::l2tpv3_ctl_hdr_udp_encap_t* hdr3 = (struct fl2tpv3frame::l2tpv3_ctl_hdr_udp_encap_t*)(pattern_l2tpv3_encap_udp_ctl->somem());

	hdr3->ctlfield			= htobe16(ctlfield);
	hdr3->len				= htobe16(sizeof(struct fl2tpv3frame::l2tpv3_ctl_hdr_udp_encap_t));
	hdr3->ccid				= htobe32(ccid);
	hdr3->nr				= htobe16(Nr);
	hdr3->ns				= htobe16(Ns);

	pattern_l2tpv3_encap_udp_data = new cmemory(sizeof(struct fl2tpv3frame::l2tpv3_data_hdr_udp_encap_t));
	struct fl2tpv3frame::l2tpv3_data_hdr_udp_encap_t* hdr4 = (struct fl2tpv3frame::l2tpv3_data_hdr_udp_encap_t*)(pattern_l2tpv3_encap_udp_data->somem());

	uint16_t ctlfield_data  = version;
	hdr4->ctlfield			= htobe16(ctlfield_data);
	hdr4->reserved			= htobe16(0);
	hdr4->session_id		= htobe32(sid_data);
}



void
fl2tpv3frameTest::tearDown()
{
	delete pattern_l2tpv3_encap_udp_data;
	delete pattern_l2tpv3_encap_udp_ctl;
	delete pattern_l2tpv3_encap_ip_data;
	delete pattern_l2tpv3_encap_ip_ctl;
}



void
fl2tpv3frameTest::testEncapIpVersion()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_IP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_ctl->somem(), pattern_l2tpv3_encap_ip_ctl->memlen());
	CPPUNIT_ASSERT(3 == frame.get_version());
	CPPUNIT_ASSERT(3 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + sizeof(uint32_t))) & 0x000f));

	frame.set_version(5);
	CPPUNIT_ASSERT(5 == frame.get_version());
	CPPUNIT_ASSERT(5 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + sizeof(uint32_t))) & 0x000f));

	frame.set_version(fl2tpv3frame::L2TP_VERSION_3);
	CPPUNIT_ASSERT(3 == frame.get_version());
	CPPUNIT_ASSERT(3 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + sizeof(uint32_t))) & 0x000f));


	/* test data message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_data->somem(), pattern_l2tpv3_encap_ip_data->memlen());

	// there is no version field in IP encapsulation, so get_version() and set_version() should always result in value 3
	CPPUNIT_ASSERT(3 == frame.get_version());

	frame.set_version(5);
	CPPUNIT_ASSERT(3 == frame.get_version());

	frame.set_version(fl2tpv3frame::L2TP_VERSION_3);
	CPPUNIT_ASSERT(3 == frame.get_version());
}


void
fl2tpv3frameTest::testEncapIpSessionID()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_IP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_ctl->somem(), pattern_l2tpv3_encap_ip_ctl->memlen());
	CPPUNIT_ASSERT(sid_ctl == frame.get_session_id());
	CPPUNIT_ASSERT(sid_ctl == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_ip_ctl->somem())));

	uint32_t nsid = 0xb0b0b0b0;
	frame.set_session_id(nsid);
	CPPUNIT_ASSERT(nsid == frame.get_session_id());
	CPPUNIT_ASSERT(nsid == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_ip_ctl->somem())));

	frame.set_session_id(sid_ctl);
	CPPUNIT_ASSERT(sid_ctl == frame.get_session_id());
	CPPUNIT_ASSERT(sid_ctl == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_ip_ctl->somem())));


	/* test data message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_data->somem(), pattern_l2tpv3_encap_ip_data->memlen());
	CPPUNIT_ASSERT(sid_data == frame.get_session_id());
	CPPUNIT_ASSERT(sid_data == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_ip_data->somem())));

	nsid = 0xb0b0b0b0;
	frame.set_session_id(nsid);
	CPPUNIT_ASSERT(nsid == frame.get_session_id());
	CPPUNIT_ASSERT(nsid == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_ip_data->somem())));

	frame.set_session_id(sid_data);
	CPPUNIT_ASSERT(sid_data == frame.get_session_id());
	CPPUNIT_ASSERT(sid_data == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_ip_data->somem())));
}


void
fl2tpv3frameTest::testEncapIpLength()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_IP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_ctl->somem(), pattern_l2tpv3_encap_ip_ctl->memlen());
	CPPUNIT_ASSERT(sizeof(struct fl2tpv3frame::l2tpv3_ctl_hdr_ip_encap_t) == frame.get_length());
	CPPUNIT_ASSERT(sizeof(struct fl2tpv3frame::l2tpv3_ctl_hdr_ip_encap_t) == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 6*sizeof(uint8_t))));

	uint16_t len = 0x20; // any arbitrary value
	frame.set_length(len);
	CPPUNIT_ASSERT(len == frame.get_length());
	CPPUNIT_ASSERT(len == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 6*sizeof(uint8_t))));

	frame.set_length(sizeof(struct fl2tpv3frame::l2tpv3_ctl_hdr_ip_encap_t));
	CPPUNIT_ASSERT(sizeof(struct fl2tpv3frame::l2tpv3_ctl_hdr_ip_encap_t) == frame.get_length());
	CPPUNIT_ASSERT(sizeof(struct fl2tpv3frame::l2tpv3_ctl_hdr_ip_encap_t) == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 6*sizeof(uint8_t))));
}


void
fl2tpv3frameTest::testEncapIpTbit()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_IP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_ctl->somem(), pattern_l2tpv3_encap_ip_ctl->memlen());
	CPPUNIT_ASSERT(true == frame.get_Tbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 4*sizeof(uint8_t))) & (1 << 15)));

	frame.set_Tbit(false);
	CPPUNIT_ASSERT(false == frame.get_Tbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 4*sizeof(uint8_t))) & (1 << 15)));

	frame.set_Tbit(true);
	CPPUNIT_ASSERT(true == frame.get_Tbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 4*sizeof(uint8_t))) & (1 << 15)));
}


void
fl2tpv3frameTest::testEncapIpLbit()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_IP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_ctl->somem(), pattern_l2tpv3_encap_ip_ctl->memlen());
	CPPUNIT_ASSERT(true == frame.get_Lbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 4*sizeof(uint8_t))) & (1 << 14)));

	frame.set_Lbit(false);
	CPPUNIT_ASSERT(false == frame.get_Lbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 4*sizeof(uint8_t))) & (1 << 14)));

	frame.set_Lbit(true);
	CPPUNIT_ASSERT(true == frame.get_Lbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 4*sizeof(uint8_t))) & (1 << 14)));
}


void
fl2tpv3frameTest::testEncapIpSbit()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_IP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_ctl->somem(), pattern_l2tpv3_encap_ip_ctl->memlen());
	CPPUNIT_ASSERT(true == frame.get_Sbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 4*sizeof(uint8_t))) & (1 << 11)));

	frame.set_Sbit(false);
	CPPUNIT_ASSERT(false == frame.get_Sbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 4*sizeof(uint8_t))) & (1 << 11)));

	frame.set_Sbit(true);
	CPPUNIT_ASSERT(true == frame.get_Sbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 4*sizeof(uint8_t))) & (1 << 11)));
}


void
fl2tpv3frameTest::testEncapIpCCID()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_IP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_ctl->somem(), pattern_l2tpv3_encap_ip_ctl->memlen());
	CPPUNIT_ASSERT(ccid == frame.get_ccid());
	CPPUNIT_ASSERT(ccid == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 8*sizeof(uint8_t))));

	uint32_t nccid = 0x48485335;
	frame.set_ccid(nccid);
	CPPUNIT_ASSERT(nccid == frame.get_ccid());
	CPPUNIT_ASSERT(nccid == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 8*sizeof(uint8_t))));

	frame.set_ccid(ccid);
	CPPUNIT_ASSERT(ccid == frame.get_ccid());
	CPPUNIT_ASSERT(ccid == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 8*sizeof(uint8_t))));
}


void
fl2tpv3frameTest::testEncapIpNs()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_IP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_ctl->somem(), pattern_l2tpv3_encap_ip_ctl->memlen());
	CPPUNIT_ASSERT(Ns == frame.get_Ns());
	CPPUNIT_ASSERT(Ns == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 12*sizeof(uint8_t))));

	uint16_t nns = 0x4853;
	frame.set_Ns(nns);
	CPPUNIT_ASSERT(nns == frame.get_Ns());
	CPPUNIT_ASSERT(nns == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 12*sizeof(uint8_t))));

	frame.set_Ns(Ns);
	CPPUNIT_ASSERT(Ns == frame.get_Ns());
	CPPUNIT_ASSERT(Ns == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 12*sizeof(uint8_t))));
}


void
fl2tpv3frameTest::testEncapIpNr()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_IP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_IP, pattern_l2tpv3_encap_ip_ctl->somem(), pattern_l2tpv3_encap_ip_ctl->memlen());
	CPPUNIT_ASSERT(Nr == frame.get_Nr());
	CPPUNIT_ASSERT(Nr == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 14*sizeof(uint8_t))));

	uint16_t nnr = 0x4853;
	frame.set_Nr(nnr);
	CPPUNIT_ASSERT(nnr == frame.get_Nr());
	CPPUNIT_ASSERT(nnr == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 14*sizeof(uint8_t))));

	frame.set_Nr(Nr);
	CPPUNIT_ASSERT(Nr == frame.get_Nr());
	CPPUNIT_ASSERT(Nr == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_ip_ctl->somem() + 14*sizeof(uint8_t))));
}


void
fl2tpv3frameTest::testEncapUdpVersion()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_UDP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_ctl->somem(), pattern_l2tpv3_encap_udp_ctl->memlen());
	CPPUNIT_ASSERT(3 == frame.get_version());
	CPPUNIT_ASSERT(3 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & 0x000f));

	frame.set_version(5);
	CPPUNIT_ASSERT(5 == frame.get_version());
	CPPUNIT_ASSERT(5 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & 0x000f));

	frame.set_version(fl2tpv3frame::L2TP_VERSION_3);
	CPPUNIT_ASSERT(3 == frame.get_version());
	CPPUNIT_ASSERT(3 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & 0x000f));


	/* test data message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_data->somem(), pattern_l2tpv3_encap_udp_data->memlen());
	CPPUNIT_ASSERT(3 == frame.get_version());
	CPPUNIT_ASSERT(3 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & 0x000f));

	frame.set_version(5);
	CPPUNIT_ASSERT(5 == frame.get_version());
	CPPUNIT_ASSERT(5 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & 0x000f));

	frame.set_version(fl2tpv3frame::L2TP_VERSION_3);
	CPPUNIT_ASSERT(3 == frame.get_version());
	CPPUNIT_ASSERT(3 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & 0x000f));
}


void
fl2tpv3frameTest::testEncapUdpSessionID()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_UDP, NULL, 0);


	/* test data message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_data->somem(), pattern_l2tpv3_encap_udp_data->memlen());
	CPPUNIT_ASSERT(sid_data == frame.get_session_id());
	CPPUNIT_ASSERT(sid_data == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_udp_data->somem() + 4*sizeof(uint8_t))));

	uint32_t nsid = 0x51514994;
	frame.set_session_id(nsid);
	CPPUNIT_ASSERT(nsid == frame.get_session_id());
	CPPUNIT_ASSERT(nsid == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_udp_data->somem() + 4*sizeof(uint8_t))));

	frame.set_session_id(sid_data);
	CPPUNIT_ASSERT(sid_data == frame.get_session_id());
	CPPUNIT_ASSERT(sid_data == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_udp_data->somem() + 4*sizeof(uint8_t))));
}


void
fl2tpv3frameTest::testEncapUdpLength()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_UDP, NULL, 0);


	uint16_t len = sizeof(struct fl2tpv3frame::l2tpv3_ctl_hdr_udp_encap_t);

	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_ctl->somem(), pattern_l2tpv3_encap_udp_ctl->memlen());
	CPPUNIT_ASSERT(len == frame.get_length());
	CPPUNIT_ASSERT(len == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 2*sizeof(uint8_t))));

	uint16_t nlen = 0x20;
	frame.set_length(nlen);
	CPPUNIT_ASSERT(nlen == frame.get_length());
	CPPUNIT_ASSERT(nlen == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 2*sizeof(uint8_t))));

	frame.set_length(len);
	CPPUNIT_ASSERT(len == frame.get_length());
	CPPUNIT_ASSERT(len == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 2*sizeof(uint8_t))));
}


void
fl2tpv3frameTest::testEncapUdpTbit()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_UDP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_ctl->somem(), pattern_l2tpv3_encap_udp_ctl->memlen());
	CPPUNIT_ASSERT(true == frame.get_Tbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & (1 << 15)));

	frame.set_Tbit(false);
	CPPUNIT_ASSERT(false == frame.get_Tbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & (1 << 15)));

	frame.set_Tbit(true);
	CPPUNIT_ASSERT(true == frame.get_Tbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & (1 << 15)));


	/* test data message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_data->somem(), pattern_l2tpv3_encap_udp_data->memlen());
	CPPUNIT_ASSERT(false == frame.get_Tbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & (1 << 15)));

	frame.set_Tbit(true);
	CPPUNIT_ASSERT(true == frame.get_Tbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & (1 << 15)));

	frame.set_Tbit(false);
	CPPUNIT_ASSERT(false == frame.get_Tbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & (1 << 15)));
}


void
fl2tpv3frameTest::testEncapUdpLbit()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_UDP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_ctl->somem(), pattern_l2tpv3_encap_udp_ctl->memlen());
	CPPUNIT_ASSERT(true == frame.get_Lbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & (1 << 14)));

	frame.set_Lbit(false);
	CPPUNIT_ASSERT(false == frame.get_Lbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & (1 << 14)));

	frame.set_Lbit(true);
	CPPUNIT_ASSERT(true == frame.get_Lbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & (1 << 14)));


	/* test data message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_data->somem(), pattern_l2tpv3_encap_udp_data->memlen());
	CPPUNIT_ASSERT(false == frame.get_Lbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & (1 << 14)));

	frame.set_Lbit(true);
	CPPUNIT_ASSERT(true == frame.get_Lbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & (1 << 14)));

	frame.set_Lbit(false);
	CPPUNIT_ASSERT(false == frame.get_Lbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & (1 << 14)));
}


void
fl2tpv3frameTest::testEncapUdpSbit()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_UDP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_ctl->somem(), pattern_l2tpv3_encap_udp_ctl->memlen());
	CPPUNIT_ASSERT(true == frame.get_Sbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & (1 << 11)));

	frame.set_Sbit(false);
	CPPUNIT_ASSERT(false == frame.get_Sbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & (1 << 11)));

	frame.set_Sbit(true);
	CPPUNIT_ASSERT(true == frame.get_Sbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem())) & (1 << 11)));


	/* test data message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_data->somem(), pattern_l2tpv3_encap_udp_data->memlen());
	CPPUNIT_ASSERT(false == frame.get_Sbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & (1 << 11)));

	frame.set_Sbit(true);
	CPPUNIT_ASSERT(true == frame.get_Sbit());
	CPPUNIT_ASSERT(0 != (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & (1 << 11)));

	frame.set_Sbit(false);
	CPPUNIT_ASSERT(false == frame.get_Sbit());
	CPPUNIT_ASSERT(0 == (be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_data->somem())) & (1 << 11)));
}


void
fl2tpv3frameTest::testEncapUdpCCID()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_UDP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_ctl->somem(), pattern_l2tpv3_encap_udp_ctl->memlen());
	CPPUNIT_ASSERT(ccid == frame.get_ccid());
	CPPUNIT_ASSERT(ccid == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 4*sizeof(uint8_t))));

	uint32_t nccid = 0x48485335;
	frame.set_ccid(nccid);
	CPPUNIT_ASSERT(nccid == frame.get_ccid());
	CPPUNIT_ASSERT(nccid == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 4*sizeof(uint8_t))));

	frame.set_ccid(ccid);
	CPPUNIT_ASSERT(ccid == frame.get_ccid());
	CPPUNIT_ASSERT(ccid == be32toh(*(uint32_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 4*sizeof(uint8_t))));
}


void
fl2tpv3frameTest::testEncapUdpNs()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_UDP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_ctl->somem(), pattern_l2tpv3_encap_udp_ctl->memlen());
	CPPUNIT_ASSERT(Ns == frame.get_Ns());
	CPPUNIT_ASSERT(Ns == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 8*sizeof(uint8_t))));

	uint16_t nns = 0x4853;
	frame.set_Ns(nns);
	CPPUNIT_ASSERT(nns == frame.get_Ns());
	CPPUNIT_ASSERT(nns == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 8*sizeof(uint8_t))));

	frame.set_Ns(Ns);
	CPPUNIT_ASSERT(Ns == frame.get_Ns());
	CPPUNIT_ASSERT(Ns == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 8*sizeof(uint8_t))));
}


void
fl2tpv3frameTest::testEncapUdpNr()
{
	// empty L2TPv3 frame
	fl2tpv3frame frame(fl2tpv3frame::L2TP_ENCAP_UDP, NULL, 0);


	/* test control message */
	frame.reset(fl2tpv3frame::L2TP_ENCAP_UDP, pattern_l2tpv3_encap_udp_ctl->somem(), pattern_l2tpv3_encap_udp_ctl->memlen());
	CPPUNIT_ASSERT(Nr == frame.get_Nr());
	CPPUNIT_ASSERT(Nr == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 10*sizeof(uint8_t))));

	uint16_t nnr = 0x4853;
	frame.set_Nr(nnr);
	CPPUNIT_ASSERT(nnr == frame.get_Nr());
	CPPUNIT_ASSERT(nnr == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 10*sizeof(uint8_t))));

	frame.set_Nr(Nr);
	CPPUNIT_ASSERT(Nr == frame.get_Nr());
	CPPUNIT_ASSERT(Nr == be16toh(*(uint16_t*)(pattern_l2tpv3_encap_udp_ctl->somem() + 10*sizeof(uint8_t))));
}



