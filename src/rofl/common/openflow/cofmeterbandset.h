/*
 * cofmeterbandset.h
 *
 *  Created on: 16.06.2013
 *      Author: andreas
 */

#ifndef COFMETERBANDSET_H_
#define COFMETERBANDSET_H_ 1

#include <map>

#include "rofl/common/openflow/cofmeterband.h"

namespace rofl {
namespace openflow {

class eOFMeterBandSetBase : public RoflException {};
class eOFMeterBandSetNotFound : public eOFMeterBandSetBase {};

class cofmeter_band_set {
private:

	uint8_t		of_version;
	std::map<enum openflow13::ofp_meter_band_type, cofmeter_band*> 	mbset;

public:


	/**
	 *
	 */
	cofmeter_band_set(
			uint8_t of_version = openflow13::OFP_VERSION);


	/**
	 *
	 */
	virtual
	~cofmeter_band_set();


	/**
	 *
	 */
	cofmeter_band_set(
			cofmeter_band_set const& mbset);


	/**
	 *
	 */
	cofmeter_band_set&
	operator= (
			cofmeter_band_set const& mbset);


public:


	/**
	 *
	 */
	cofmeter_band_drop&
	get_meter_band_drop();


	/**
	 *
	 */
	cofmeter_band_dscp_remark&
	get_meter_band_dscp_remark();


	/**
	 *
	 */
	cofmeter_band_expr&
	get_meter_band_expr();


public:


	/**
	 *
	 */
	void
	set_meter_band(cofmeter_band const& mb);


	/**
	 *
	 */
	void
	clear_meter_band(enum openflow13::ofp_meter_band_type mbt);


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


private:


	/**
	 *
	 */
	void
	clean_up();
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFMETERBANDSET_H_ */
