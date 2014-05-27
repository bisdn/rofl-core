#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cofmeterconfigarray_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( cofmeterconfigarray_test );

#if defined DEBUG
#undef DEBUG
#endif

void
cofmeterconfigarray_test::setUp()
{
}



void
cofmeterconfigarray_test::tearDown()
{
}



void
cofmeterconfigarray_test::testDefaultConstructor()
{
	rofl::openflow::cofmeterconfigarray mstats;
	CPPUNIT_ASSERT(rofl::openflow::OFP_VERSION_UNKNOWN == mstats.get_version());
	CPPUNIT_ASSERT(0 == mstats.get_mconfig().size());
}



void
cofmeterconfigarray_test::testCopyConstructor()
{
	unsigned int num_of_mconfigs	= 5;
	uint16_t flags					= 0xa1a2;
	uint32_t meter_id				= 0xb1b2b3b4;

	rofl::openflow::cofmeterconfigarray mconfig1(rofl::openflow13::OFP_VERSION);

	for (unsigned int i = 0; i < num_of_mconfigs; i++) {
		mconfig1.set_meter_config(i).set_flags(flags);
		mconfig1.set_meter_config(i).set_meter_id(meter_id);
	}


#ifdef DEBUG
	std::cerr << "mconfig1:" << std::endl << mconfig1;
#endif

	rofl::openflow::cofmeterconfigarray mconfig2(mconfig1);

#ifdef DEBUG
	std::cerr << "mstats2:" << std::endl << mconfig2;
#endif

	for (unsigned int i = 0; i < num_of_mconfigs; i++) {
		CPPUNIT_ASSERT(mconfig2.get_meter_config(i).get_flags() 		== flags);
		CPPUNIT_ASSERT(mconfig2.get_meter_config(i).get_meter_id() 		== meter_id);
	}

	CPPUNIT_ASSERT(mconfig1 == mconfig2);
}



void
cofmeterconfigarray_test::testPackUnpack()
{
	unsigned int num_of_mconfigs		= 2;
	unsigned int num_of_meter_configs	= 3;
	uint16_t flags						= 0xa1a2;
	uint32_t meter_id					= 0xb1b2b3b4;

	uint8_t prec_level					= 0xc1;

	rofl::openflow::cofmeterconfigarray mconfig1(rofl::openflow13::OFP_VERSION);

	for (unsigned int i = 0; i < num_of_mconfigs; i++) {
		mconfig1.set_meter_config(i).set_flags(flags);
		mconfig1.set_meter_config(i).set_meter_id(meter_id);
		for (unsigned int j = 0; j < num_of_meter_configs; j++) {
			mconfig1.set_meter_config(i).set_meter_bands().set_meter_band_drop(j);
			mconfig1.set_meter_config(i).set_meter_bands().set_meter_band_dscp_remark(j).set_prec_level(prec_level);
		}
	}

	rofl::cmemory packed(mconfig1.length());
	mconfig1.pack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mconfig1:" << std::endl << mconfig1;
	std::cerr << "packed:" << std::endl << packed;
#endif

	rofl::openflow::cofmeterconfigarray mconfig2(rofl::openflow13::OFP_VERSION);

	mconfig2.unpack(packed.somem(), packed.memlen());

#ifdef DEBUG
	std::cerr << "mconfig2:" << std::endl << mconfig2;
#endif

	for (unsigned int i = 0; i < num_of_mconfigs; i++) {
		CPPUNIT_ASSERT(mconfig2.get_meter_config(i).get_flags() 		== flags);
		CPPUNIT_ASSERT(mconfig2.get_meter_config(i).get_meter_id() 		== meter_id);
		CPPUNIT_ASSERT(mconfig2.get_meter_config(i).get_meter_bands().get_mbs_drop().size() == num_of_meter_configs);
		CPPUNIT_ASSERT(mconfig2.get_meter_config(i).get_meter_bands().get_mbs_dscp_remark().size() == num_of_meter_configs);
		for (unsigned int j = 0; j < num_of_meter_configs; j++) {
			CPPUNIT_ASSERT(mconfig2.get_meter_config(i).get_meter_bands().get_meter_band_dscp_remark(j).get_prec_level() == prec_level);
		}
	}

	CPPUNIT_ASSERT(mconfig1 == mconfig2);
}


void
cofmeterconfigarray_test::testOperatorPlus()
{
	unsigned int num_of_mconfigs		= 2;
	unsigned int num_of_meter_configs	= 3;
	uint16_t flags						= 0xa1a2;
	uint32_t meter_id					= 0xb1b2b3b4;
	uint8_t prec_level					= 0xc1;

	rofl::openflow::cofmeterconfigarray mconfig1(rofl::openflow13::OFP_VERSION);

	for (unsigned int i = 0; i < num_of_mconfigs; i++) {
		mconfig1.set_meter_config(i).set_flags(flags);
		mconfig1.set_meter_config(i).set_meter_id(meter_id);
		for (unsigned int j = 0; j < num_of_meter_configs; j++) {
			mconfig1.set_meter_config(i).set_meter_bands().set_meter_band_drop(j);
			mconfig1.set_meter_config(i).set_meter_bands().set_meter_band_dscp_remark(j).set_prec_level(prec_level);
		}
	}

	rofl::openflow::cofmeterconfigarray mconfig2(rofl::openflow13::OFP_VERSION);

	for (unsigned int i = 0; i < num_of_mconfigs; i++) {
		mconfig2.set_meter_config(i).set_flags(flags);
		mconfig2.set_meter_config(i).set_meter_id(meter_id);
		for (unsigned int j = 0; j < num_of_meter_configs; j++) {
			mconfig2.set_meter_config(i).set_meter_bands().set_meter_band_drop(j);
			mconfig2.set_meter_config(i).set_meter_bands().set_meter_band_dscp_remark(j).set_prec_level(prec_level);
		}
	}


#ifdef DEBUG
	std::cerr << "mconfig1:" << std::endl << mconfig1;
	std::cerr << "mconfig2:" << std::endl << mconfig2;
#endif

	rofl::openflow::cofmeterconfigarray mconfig3(rofl::openflow13::OFP_VERSION);

	mconfig3 += mconfig1;
	mconfig3 += mconfig2;

#ifdef DEBUG
	std::cerr << "mconfig3:" << std::endl << mconfig3;
#endif

	for (unsigned int i = 0; i < 2*num_of_mconfigs; i++) {
		CPPUNIT_ASSERT(mconfig3.get_meter_config(i).get_flags() 		== flags);
		CPPUNIT_ASSERT(mconfig3.get_meter_config(i).get_meter_id() 		== meter_id);
		CPPUNIT_ASSERT(mconfig3.get_meter_config(i).get_meter_bands().get_mbs_drop().size() == num_of_meter_configs);
		CPPUNIT_ASSERT(mconfig3.get_meter_config(i).get_meter_bands().get_mbs_dscp_remark().size() == num_of_meter_configs);
		for (unsigned int j = 0; j < num_of_meter_configs; j++) {
			CPPUNIT_ASSERT(mconfig3.get_meter_config(i).get_meter_bands().get_meter_band_dscp_remark(j).get_prec_level() == prec_level);
		}
	}
}



void
cofmeterconfigarray_test::testAddDropSetGetHas()
{
	rofl::openflow::cofmeterconfigarray mconfig(rofl::openflow13::OFP_VERSION);

	try {
		mconfig.get_meter_config();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eRofMeterConfigNotFound& e) {};

	if (mconfig.has_meter_config()) {
		CPPUNIT_ASSERT(false);
	}
	mconfig.set_meter_config();

	try {
		mconfig.get_meter_config();
	} catch (rofl::openflow::eRofMeterConfigNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	try {
		mconfig.set_meter_config();
	} catch (rofl::openflow::eRofMeterConfigNotFound& e) {
		CPPUNIT_ASSERT(false);
	}

	if (not mconfig.has_meter_config()) {
		CPPUNIT_ASSERT(false);
	}

	mconfig.drop_meter_config();

	try {
		mconfig.get_meter_config();
		CPPUNIT_ASSERT(false);
	} catch (rofl::openflow::eRofMeterConfigNotFound& e) {};

	if (mconfig.has_meter_config()) {
		CPPUNIT_ASSERT(false);
	}
	mconfig.set_meter_config();
}


