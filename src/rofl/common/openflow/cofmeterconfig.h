/*
 * cofmeterconfig.h
 *
 *  Created on: 26.05.2014
 *      Author: andi
 */

#ifndef COFMETERCONFIG_H_
#define COFMETERCONFIG_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/cofmeterbands.h"

namespace rofl {
namespace openflow {

class eRofMeterConfigBase		: public RoflException {};
class eRofMeterConfigInval		: public eRofMeterConfigBase {};
class eRofMeterConfigNotFound	: public eRofMeterConfigBase {};

class cofmeter_config_request
{
public:
	/**
	 *
	 */
	cofmeter_config_request(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);

	/**
	 *
	 */
	cofmeter_config_request(
			uint8_t of_version,
			uint32_t port_no);


	/**
	 *
	 */
	virtual
	~cofmeter_config_request();


	/**
	 *
	 */
	cofmeter_config_request(
			const cofmeter_config_request& meter_config_request);

	/**
	 *
	 */
	cofmeter_config_request&
	operator= (
			const cofmeter_config_request& meter_config_request);

	/**
	 *
	 */
	const bool
	operator== (
			const cofmeter_config_request& meter_config_request) const;

public:

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


	/**
	 *
	 */
	size_t
	length() const;

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
	set_meter_id(uint32_t meter_id) { this->meter_id = meter_id; };

	/**
	 *
	 */
	uint32_t
	get_meter_id() const { return meter_id; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmeter_config_request const& r) {
		os << indent(0) << "<cofmeter_config_request >" << std::endl;
		os << indent(2) << "<meter-id: 0x" << std::hex << (int)r.get_meter_id() << std::dec << " >" << std::endl;
		return os;
	};

private: // data structures

	uint8_t 	of_version;
	uint32_t	meter_id;
};



class cofmeter_config_reply
{
public:
	/**
	 *
	 */
	cofmeter_config_reply(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);


	/**
	 *
	 */
	cofmeter_config_reply(
			uint8_t of_version,
			uint16_t flags,
			uint32_t meter_id,
			const rofl::openflow::cofmeter_bands& mbands);



	/**
	 *
	 */
	virtual
	~cofmeter_config_reply();


	/**
	 *
	 */
	cofmeter_config_reply(
			const cofmeter_config_reply& meter_config_reply);

	/**
	 *
	 */
	cofmeter_config_reply&
	operator= (
			const cofmeter_config_reply& meter_config_reply);

	/**
	 *
	 */
	bool
	operator== (
			const cofmeter_config_reply& meter_config_reply);

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
	set_flags(uint16_t flags) { this->flags = flags; };

	/**
	 *
	 */
	uint16_t
	get_flags() const { return flags; };

	/**
	 *
	 */
	void
	set_meter_id(uint32_t meter_id) { this->meter_id = meter_id; };

	/**
	 *
	 */
	uint32_t
	get_meter_id() const { return meter_id; };

	/**
	 *
	 */
	rofl::openflow::cofmeter_bands&
	set_meter_bands() { return mbands; };

	/**
	 *
	 */
	const rofl::openflow::cofmeter_bands&
	get_meter_bands() const { return mbands; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmeter_config_reply const& r) {
		os << indent(0) << "<cofmeter_config_reply >" << std::endl;
		os << std::hex;
		os << indent(2) << "<flags: 0x"		 		<< (unsigned int)r.get_flags() << " >" << std::endl;
		os << indent(2) << "<meter-id: 0x" 			<< (unsigned int)r.get_meter_id() << " >" << std::endl;
		os << std::dec;
		{ rofl::indent i(2); os << r.get_meter_bands(); }
		return os;
	};

private: // data structures

	uint8_t 						of_version;
	uint16_t						flags;
	uint32_t 						meter_id;
	rofl::openflow::cofmeter_bands 	mbands;
};

typedef cofmeter_config_reply cofmeter_config;

} /* end of namespace openflow */
} /* end of namespace rofl */

#endif /* COFMETERCONFIG_H_ */
