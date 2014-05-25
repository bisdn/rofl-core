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
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN);


	/**
	 *
	 */
	virtual
	~cofmeter_bands();


	/**
	 *
	 */
	cofmeter_bands(
			const cofmeter_bands& mbset);


	/**
	 *
	 */
	cofmeter_bands&
	operator= (
			const cofmeter_bands& mbset);


public:


	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	void
	set_version(uint8_t of_version) {
		this->of_version = of_version;
		for (std::map<unsigned int, cofmeter_band_drop>::iterator
				it = mbs_drop.begin(); it != mbs_drop.end(); ++it) {
			it->second.set_version(of_version);
		}
		for (std::map<unsigned int, cofmeter_band_dscp_remark>::iterator
				it = mbs_dscp_remark.begin(); it != mbs_dscp_remark.end(); ++it) {
			it->second.set_version(of_version);
		}
		for (std::map<unsigned int, cofmeter_band_experimenter>::iterator
				it = mbs_experimenter.begin(); it != mbs_experimenter.end(); ++it) {
			it->second.set_version(of_version);
		}
	};

	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };


	/**
	 *
	 */
	unsigned int
	get_num_of_mbs() const;


	/**
	 *
	 */
	std::map<unsigned int, cofmeter_band_drop>&
	set_mbs_drop() { return mbs_drop; };


	/**
	 *
	 */
	const std::map<unsigned int, cofmeter_band_drop>&
	get_mbs_drop() const { return mbs_drop; };


	/**
	 *
	 */
	std::map<unsigned int, cofmeter_band_dscp_remark>&
	set_mbs_dscp_remark() { return mbs_dscp_remark; };


	/**
	 *
	 */
	const std::map<unsigned int, cofmeter_band_dscp_remark>&
	get_mbs_dscp_remark() const { return mbs_dscp_remark; };


	/**
	 *
	 */
	std::map<unsigned int, cofmeter_band_experimenter>&
	set_mbs_experimenter() { return mbs_experimenter; };


	/**
	 *
	 */
	const std::map<unsigned int, cofmeter_band_experimenter>&
	get_mbs_experimenter() const { return mbs_experimenter; };


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
		os << rofl::indent(0) << "<cofmeterbands version: " << (int)mbs.get_version()
				<< " #mbs: " << mbs.get_num_of_mbs() << " >" << std::endl;
		rofl::indent i(2);
		// drop
		for (std::map<unsigned int, cofmeter_band_drop>::const_iterator
				it = mbs.get_mbs_drop().begin(); it != mbs.get_mbs_drop().end(); ++it) {
			os << it->second;
		}
		// dscp_remark
		for (std::map<unsigned int, cofmeter_band_dscp_remark>::const_iterator
				it = mbs.get_mbs_dscp_remark().begin(); it != mbs.get_mbs_dscp_remark().end(); ++it) {
			os << it->second;
		}
		// experimenter
		for (std::map<unsigned int, cofmeter_band_experimenter>::const_iterator
				it = mbs.get_mbs_experimenter().begin(); it != mbs.get_mbs_experimenter().end(); ++it) {
			os << it->second;
		}
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
