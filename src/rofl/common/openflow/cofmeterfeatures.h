/*
 * cofmeterfeatures.h
 *
 *  Created on: 26.05.2014
 *      Author: andi
 */

#ifndef COFMETERFEATURES_H_
#define COFMETERFEATURES_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class eRofMeterFeaturesBase		: public RoflException {};
class eRofMeterFeaturesInval	: public eRofMeterFeaturesBase {};
class eRofMeterFeaturesNotFound	: public eRofMeterFeaturesBase {};

class cofmeter_features_reply
{
public:
	/**
	 *
	 */
	cofmeter_features_reply(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);


	/**
	 *
	 */
	cofmeter_features_reply(
			uint8_t of_version,
			uint32_t max_meter,
			uint32_t band_types,
			uint32_t capabilities,
			uint8_t max_bands,
			uint8_t max_color);



	/**
	 *
	 */
	virtual
	~cofmeter_features_reply();


	/**
	 *
	 */
	cofmeter_features_reply(
			const cofmeter_features_reply& meter_features_reply);

	/**
	 *
	 */
	cofmeter_features_reply&
	operator= (
			const cofmeter_features_reply& meter_features_reply);

	/**
	 *
	 */
	bool
	operator== (
			const cofmeter_features_reply& meter_features_reply);

public:

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	pack(uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	void
	set_version(uint8_t of_version) { this->of_version = of_version; };

	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };

	/**
	 *
	 */
	void
	set_max_meter(uint32_t max_meter) { this->max_meter = max_meter; };

	/**
	 *
	 */
	uint32_t
	get_max_meter() const { return max_meter; };

	/**
	 *
	 */
	void
	set_band_types(uint32_t band_types) { this->band_types = band_types; };

	/**
	 *
	 */
	uint32_t
	get_band_types() const { return band_types; };

	/**
	 *
	 */
	void
	set_capabilities(uint32_t capabilities) { this->capabilities = capabilities; };

	/**
	 *
	 */
	uint32_t
	get_capabilities() const { return capabilities; };

	/**
	 *
	 */
	void
	set_max_bands(uint8_t max_bands) { this->max_bands = max_bands; };

	/**
	 *
	 */
	uint8_t
	get_max_bands() const { return max_bands; };

	/**
	 *
	 */
	void
	set_max_color(uint8_t max_color) { this->max_color = max_color; };

	/**
	 *
	 */
	uint8_t
	get_max_color() const { return max_color; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmeter_features_reply const& r) {
		os << indent(0) << "<cofmeter_features_reply ofp-version: " << (int)r.get_version() << " >" << std::endl;
		os << std::hex;
		os << indent(2) << "<max-meter: 0x"			<< (unsigned int)r.get_max_meter() << " >" << std::endl;
		os << indent(2) << "<band-types: 0x" 		<< (unsigned int)r.get_band_types() << " >" << std::endl;
		os << indent(2) << "<capabilities: 0x" 		<< (unsigned int)r.get_capabilities() << " >" << std::endl;
		os << indent(2) << "<max-bands: 0x" 		<< (unsigned int)r.get_max_bands() << " >" << std::endl;
		os << indent(2) << "<max-color: 0x" 		<< (unsigned int)r.get_max_color() << " >" << std::endl;
		os << std::dec;
		return os;
	};

private: // data structures

	uint8_t 						of_version;
	uint32_t 						max_meter;
	uint32_t						band_types;
	uint32_t						capabilities;
	uint8_t							max_bands;
	uint8_t							max_color;
};

typedef cofmeter_features_reply cofmeter_features;

} /* end of namespace openflow */
} /* end of namespace rofl */

#endif /* COFMETERFEATURES_H_ */
