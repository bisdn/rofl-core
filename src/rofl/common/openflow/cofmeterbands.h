/*
 * cofmeterbands.h
 *
 *  Created on: 16.06.2013
 *      Author: andreas
 */

#ifndef COFMETERBANDS_H_
#define COFMETERBANDS_H_ 1

#include <inttypes.h>

#include <map>
#include <iostream>

#include "rofl/common/openflow/cofmeterband.h"

namespace rofl {
namespace openflow {

class eRofMeterBandsBase 		: public RoflException {};
class eRofMeterBandsNotFound 	: public eRofMeterBandsBase {};

class cofmeter_bands {
public:


	/**
	 *
	 */
	cofmeter_bands(
			uint8_t of_version = openflow13::OFP_VERSION);


	/**
	 *
	 */
	virtual
	~cofmeter_bands();


	/**
	 *
	 */
	cofmeter_bands(
			cofmeter_bands const& mbset);


	/**
	 *
	 */
	cofmeter_bands&
	operator= (
			cofmeter_bands const& mbset);


public:


	/**
	 *
	 */
	void
	clear();


	// cofmeter_band_drop

	/**
	 *
	 */
	cofmeter_band_drop&
	add_meter_band_drop(
			unsigned int index = 0);


	/**
	 *
	 */
	void
	drop_meter_band_drop(
			unsigned int index = 0);


	/**
	 *
	 */
	cofmeter_band_drop&
	set_meter_band_drop(
			unsigned int index = 0);


	/**
	 *
	 */
	const cofmeter_band_drop&
	get_meter_band_drop(
			unsigned int index = 0) const;


	/**
	 *
	 */
	bool
	has_meter_band_drop(
			unsigned int index = 0) const;


	// cofmeter_band_dscp_remark

	/**
	 *
	 */
	cofmeter_band_dscp_remark&
	add_meter_band_dscp_remark(
			unsigned int index = 0);


	/**
	 *
	 */
	void
	drop_meter_band_dscp_remark(
			unsigned int index = 0);


	/**
	 *
	 */
	cofmeter_band_dscp_remark&
	set_meter_band_dscp_remark(
			unsigned int index = 0);


	/**
	 *
	 */
	const cofmeter_band_dscp_remark&
	get_meter_band_dscp_remark(
			unsigned int index = 0) const;


	/**
	 *
	 */
	bool
	has_meter_band_dscp_remark(
			unsigned int index = 0) const;



	// cofmeter_band_experimenter

	/**
	 *
	 */
	cofmeter_band_experimenter&
	add_meter_band_experimenter(
			unsigned int index = 0);


	/**
	 *
	 */
	void
	drop_meter_band_experimenter(
			unsigned int index = 0);


	/**
	 *
	 */
	cofmeter_band_experimenter&
	set_meter_band_experimenter(
			unsigned int index = 0);


	/**
	 *
	 */
	const cofmeter_band_experimenter&
	get_meter_band_experimenter(
			unsigned int index = 0) const;


	/**
	 *
	 */
	bool
	has_meter_band_experimenter(
			unsigned int index = 0) const;



public:


	/**
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(
			uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);


public:


	friend std::ostream&
	operator<< (std::ostream& os, const cofmeter_bands& mbs) {

		return os;
	};


private:

	uint8_t												of_version;
	std::map<unsigned int, cofmeter_band_drop> 			mbs_drop;
	std::map<unsigned int, cofmeter_band_dscp_remark> 	mbs_dscp_remark;
	std::map<unsigned int, cofmeter_band_experimenter> 	mbs_experimenter;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFMETERBANDS_H_ */
