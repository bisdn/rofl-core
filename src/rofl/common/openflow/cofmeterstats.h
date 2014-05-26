/*
 * cofmeterstats.h
 *
 *  Created on: 26.05.2014
 *      Author: andi
 */

#ifndef COFMETERSTATS_H_
#define COFMETERSTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/cofmeterbandstatsarray.h"

namespace rofl {
namespace openflow {

class eRofMeterStatsBase		: public RoflException {};
class eRofMeterStatsInval		: public eRofMeterStatsBase {};
class eRofMeterStatsNotFound	: public eRofMeterStatsBase {};

class cofmeter_stats_request
{
public:
	/**
	 *
	 */
	cofmeter_stats_request(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);

	/**
	 *
	 */
	cofmeter_stats_request(
			uint8_t of_version,
			uint32_t port_no);


	/**
	 *
	 */
	virtual
	~cofmeter_stats_request();


	/**
	 *
	 */
	cofmeter_stats_request(
			const cofmeter_stats_request& meter_stats_request);

	/**
	 *
	 */
	cofmeter_stats_request&
	operator= (
			const cofmeter_stats_request& meter_stats_request);

	/**
	 *
	 */
	const bool
	operator== (
			const cofmeter_stats_request& meter_stats_request) const;

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
	operator<< (std::ostream& os, cofmeter_stats_request const& r) {
		os << indent(0) << "<cofmeter_stats_request >" << std::endl;
		os << indent(2) << "<meter-id: 0x" << std::hex << (int)r.get_meter_id() << std::dec << " >" << std::endl;
		return os;
	};

private: // data structures

	uint8_t 	of_version;
	uint32_t	meter_id;
};



class cofmeter_stats_reply
{
public:
	/**
	 *
	 */
	cofmeter_stats_reply(
			uint8_t of_version = 0,
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0);


	/**
	 *
	 */
	cofmeter_stats_reply(
			uint8_t of_version,
			uint32_t meter_id,
			uint32_t flow_count,
			uint64_t packet_in_count,
			uint64_t byte_in_count,
			uint32_t durarion_sec,
			uint32_t duration_nsec,
			const rofl::openflow::cofmeter_band_stats_array& mbstats);



	/**
	 *
	 */
	virtual
	~cofmeter_stats_reply();


	/**
	 *
	 */
	cofmeter_stats_reply(
			const cofmeter_stats_reply& meter_stats_reply);

	/**
	 *
	 */
	cofmeter_stats_reply&
	operator= (
			const cofmeter_stats_reply& meter_stats_reply);

	/**
	 *
	 */
	bool
	operator== (
			const cofmeter_stats_reply& meter_stats_reply);

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
	set_meter_id(uint32_t meter_id) { this->meter_id = meter_id; };

	/**
	 *
	 */
	uint32_t
	get_meter_id() const { return meter_id; };

	/**
	 *
	 */
	void
	set_flow_count(uint32_t flow_count) { this->flow_count = flow_count; };

	/**
	 *
	 */
	uint32_t
	get_flow_count() const { return flow_count; };

	/**
	 *
	 */
	void
	set_packet_in_count(uint64_t packet_in_count) { this->packet_in_count = packet_in_count; };

	/**
	 *
	 */
	uint64_t
	get_packet_in_count() const { return packet_in_count; };

	/**
	 *
	 */
	void
	set_byte_in_count(uint64_t byte_in_count) { this->byte_in_count = byte_in_count; };

	/**
	 *
	 */
	uint64_t
	get_byte_in_count() const { return byte_in_count; };

	/**
	 *
	 */
	void
	set_duration_sec(uint32_t duration_sec) { this->duration_sec = duration_sec; };

	/**
	 *
	 */
	uint32_t
	get_duration_sec() const { return duration_sec; };

	/**
	 *
	 */
	void
	set_duration_nsec(uint32_t duration_nsec) { this->duration_nsec = duration_nsec; };

	/**
	 *
	 */
	uint32_t
	get_duration_nsec() const { return duration_nsec; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmeter_stats_reply const& r) {
		os << indent(0) << "<cofmeter_stats_reply >" << std::endl;
		os << std::hex;
		os << indent(2) << "<meter-id: 0x" 			<< (unsigned int)r.get_meter_id() << " >" << std::endl;
		os << indent(2) << "<flow-count: 0x" 		<< (unsigned int)r.get_flow_count() << " >" << std::endl;
		os << indent(2) << "<packet-in-count: 0x" 	<< (unsigned long long)r.get_packet_in_count() << " >" << std::endl;
		os << indent(2) << "<byte-in-count: 0x" 	<< (unsigned long long)r.get_byte_in_count() << " >" << std::endl;
		os << indent(2) << "<duration-sec: 0x" 		<< (unsigned long long)r.get_duration_sec() 	<< " >" << std::endl;
		os << indent(2) << "<duration-nsec: 0x" 	<< (unsigned long long)r.get_duration_nsec() 	<< " >" << std::endl;
		os << std::dec;
		return os;
	};

private: // data structures

	uint8_t 	of_version;
	uint32_t 	meter_id;
	uint32_t	flow_count;
	uint64_t	packet_in_count;
	uint64_t	byte_in_count;
	uint32_t	duration_sec;
	uint32_t	duration_nsec;
	rofl::openflow::cofmeter_band_stats_array 	mbstats;
};

typedef cofmeter_stats_reply cofmeter_stats;

} /* end of namespace openflow */
} /* end of namespace rofl */

#endif /* COFMETERSTATS_H_ */
