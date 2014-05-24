/*
 * cofqueueprop.h
 *
 *  Created on: 29.04.2013
 *      Author: andi
 */

#ifndef COFMETERBAND_H_
#define COFMETERBAND_H_ 1

#include <ostream>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class eRofMeterBandBase 		: public RoflException {};
class eRofMeterBandNotFound 	: public eRofMeterBandBase {};


class cofmeter_band {
public:

	/**
	 *
	 */
	cofmeter_band(
			uint8_t of_version = rofl::openflow13::OFP_VERSION,
			size_t len = sizeof(struct rofl::openflow13::ofp_meter_band_header));


	/**
	 *
	 */
	virtual
	~cofmeter_band();


	/**
	 *
	 */
	cofmeter_band(
			const cofmeter_band& mb);


	/**
	 *
	 */
	cofmeter_band&
	operator= (
			const cofmeter_band& mb);


public:


	/**
	 *
	 */
	uint8_t*
	resize(size_t len) { return body.resize(len); }


	/**
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	uint8_t
	get_version() const { return of_version; };


	/**
	 *
	 */
	void
	set_version(
			uint8_t of_version) { this->of_version = of_version; };


	/**
	 *
	 */
	uint16_t
	get_type() const { return type; }


	/**
	 *
	 */
	void
	set_type(
			uint16_t type) { this->type = type; };


	/**
	 *
	 */
	uint16_t
	get_length() const { return len; };


	/**
	 *
	 */
	void
	set_length(uint16_t len) { this->len = len; };


	/**
	 *
	 */
	uint32_t
	get_rate() const { return rate; };


	/**
	 *
	 */
	void
	set_rate(
			uint32_t rate) { this->rate = rate; };


	/**
	 *
	 */
	uint32_t
	get_burst_size() const { return burst_size; };


	/**
	 *
	 */
	void
	set_burst_size(
			uint32_t burst_size) { this->burst_size = burst_size; };


	/**
	 *
	 */
	rofl::cmemory&
	set_body() { return body; };


	/**
	 *
	 */
	rofl::cmemory const&
	get_body() const { return body; };


public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofmeter_band const& mb) {
		os << rofl::indent(0) << "<cofmeter_band "
			<< "type: " << mb.get_type()
			<< "len: " << mb.get_length()
			<< "rate: " << mb.get_rate()
			<< "burst_size: " << mb.get_burst_size()
			<< " >" << std::endl;
		{ rofl::indent i(2); os << mb.get_body(); }
		return os;
	};

private:

	uint8_t 		of_version;
	uint16_t		type;
	uint16_t 		len;
	uint32_t		rate;
	uint32_t 		burst_size;
	rofl::cmemory	body;
};







class cofmeter_band_drop :
		public cofmeter_band
{
public:

	/**
	 *
	 */
	cofmeter_band_drop(
			uint8_t of_version = rofl::openflow13::OFP_VERSION);


	/**
	 *
	 */
	virtual
	~cofmeter_band_drop();



	/**
	 *
	 */
	cofmeter_band_drop(
			const cofmeter_band_drop& mb);



	/**
	 *
	 */
	cofmeter_band_drop&
	operator= (
			const cofmeter_band_drop& mb);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmeter_band_drop& mb) {
		os << dynamic_cast<const cofmeter_band&>( mb );
		os << rofl::indent(2) << "<cofmeter_band_drop >" << std::endl;
		return os;
	};
};







class cofmeter_band_dscp_remark :
		public cofmeter_band
{
public:

	/**
	 *
	 */
	cofmeter_band_dscp_remark(
			uint8_t of_version = rofl::openflow13::OFP_VERSION);


	/**
	 *
	 */
	virtual
	~cofmeter_band_dscp_remark();



	/**
	 *
	 */
	cofmeter_band_dscp_remark(
			const cofmeter_band_dscp_remark& mb);



	/**
	 *
	 */
	cofmeter_band_dscp_remark&
	operator= (
			const cofmeter_band_dscp_remark& mb);



public:


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	uint8_t
	get_prec_level() const { return prec_level; };


	/**
	 *
	 */
	void
	set_prec_level(uint8_t prec_level) { this->prec_level = prec_level; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmeter_band_dscp_remark& mb) {
		os << dynamic_cast<const cofmeter_band&>( mb );
		os << rofl::indent(2) << "<cofmeter_band_dscp_remark prec-level:"
				<< (int)mb.get_prec_level() << " >" << std::endl;
		return os;
	};

private:

	uint8_t		prec_level;
};





class cofmeter_band_experimenter :
		public cofmeter_band
{
public:

	/**
	 *
	 */
	cofmeter_band_experimenter(
			uint8_t of_version = rofl::openflow13::OFP_VERSION);


	/**
	 *
	 */
	virtual
	~cofmeter_band_experimenter();


	/**
	 *
	 */
	cofmeter_band_experimenter(
			const cofmeter_band_experimenter& mb);


	/**
	 *
	 */
	cofmeter_band_experimenter&
	operator= (
			const cofmeter_band_experimenter& mb);



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
	pack(uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	uint32_t
	get_exp_id() const { return exp_id; };


	/**
	 *
	 */
	void
	set_exp_id(uint32_t exp_id) { this->exp_id = exp_id; };


	/**
	 *
	 */
	cmemory const&
	get_exp_body() const { return exp_body; };


	/**
	 *
	 */
	cmemory&
	set_exp_body() { return exp_body; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofmeter_band_experimenter& mb) {
		os << dynamic_cast<const cofmeter_band&>( mb );
		os << rofl::indent(2) << "<cofmeter_band_experimenter exp_id:"
				<< (int)mb.get_exp_id() << " >" << std::endl;
		{ rofl::indent i(4); os << mb.get_body(); }
		return os;
	};

private:

	uint32_t 		exp_id;
	rofl::cmemory	exp_body;
};

}
}

#endif /* COFMETERBAND_H_ */
