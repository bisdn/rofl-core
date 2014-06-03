/*
 * cofmeterconfigarray.h
 *
 *  Created on: 27.05.2014
 *      Author: andreas
 */

#ifndef COFMETERCONFIGARRAY_H_
#define COFMETERCONFIGARRAY_H_

#include <iostream>
#include <map>

#include "rofl/common/openflow/cofmeterconfig.h"

namespace rofl {
namespace openflow {

class cofmeterconfigarray
{
public:

	/**
	 *
	 */
	cofmeterconfigarray(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofmeterconfigarray();

	/**
	 *
	 */
	cofmeterconfigarray(
			const cofmeterconfigarray& mconfig);

	/**
	 *
	 */
	cofmeterconfigarray&
	operator= (
			const cofmeterconfigarray& mconfig);

	/**
	 *
	 */
	bool
	operator== (
			const cofmeterconfigarray& mconfig);

	/**
	 *
	 */
	cofmeterconfigarray&
	operator+= (
			const cofmeterconfigarray& mconfig);

public:

	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	size_t
	size() const { return array.size(); };

	/**
	 *
	 */
	void
	clear() { array.clear(); };

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) {
		this->ofp_version = ofp_version;
		for (std::map<unsigned int, cofmeter_config_reply>::iterator
				it = array.begin(); it != array.end(); ++it) {
			it->second.set_version(ofp_version);
		}
	};

	/**
	 *
	 */
	const std::map<unsigned int, cofmeter_config_reply>&
	get_mconfig() const { return array; };

	/**
	 *
	 */
	std::map<unsigned int, cofmeter_config_reply>&
	set_mconfig() { return array; };

public:

	/**
	 *
	 */
	cofmeter_config_reply&
	add_meter_config(
			unsigned int index = 0);

	/**
	 *
	 */
	void
	drop_meter_config(
			unsigned int index = 0);

	/**
	 *
	 */
	cofmeter_config_reply&
	set_meter_config(
			unsigned int index = 0);

	/**
	 *
	 */
	const cofmeter_config_reply&
	get_meter_config(
			unsigned int index = 0) const;

	/**
	 *
	 */
	bool
	has_meter_config(
			unsigned int index = 0) const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmeterconfigarray const& mconfig) {
		os << rofl::indent(0) << "<cofmeterconfigarray #meter-config:" << (int)mconfig.array.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<unsigned int, cofmeter_config_reply>::const_iterator
				it = mconfig.array.begin(); it != mconfig.array.end(); ++it) {
			os << it->second;
		}
		return os;
	}

private:

	uint8_t											ofp_version;
	std::map<unsigned int, cofmeter_config_reply>	array;
};

}; // end of openflow
}; // end of rofl



#endif /* COFMETERCONFIGARRAY_H_ */
