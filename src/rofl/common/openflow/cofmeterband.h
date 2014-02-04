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

namespace rofl
{

class eMeterBandBase : public RoflException {};
class eMeterBandNotFound : public eMeterBandBase {};


class cofmeter_band :
		public cmemory
{
private:

	union {
		uint8_t							*ofmu_hdr;
		struct openflow13::ofp_meter_band_header	*ofmu13_hdr;
	} ofm_ofmu;

#define ofm_header ofm_ofmu.ofmu_hdr
#define ofm13_header ofm_ofmu.ofmu13_hdr

protected:

	uint8_t of_version;

	/**
	 *
	 */
	virtual uint8_t*
	resize(
			size_t len);


public:

	/**
	 *
	 */
	cofmeter_band(
			uint8_t of_version = openflow13::OFP_VERSION);


	/**
	 *
	 */
	virtual
	~cofmeter_band();


	/**
	 *
	 */
	cofmeter_band(
			cofmeter_band const& mb);


	/**
	 *
	 */
	cofmeter_band&
	operator= (
			cofmeter_band const& mb);


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
	pack(uint8_t *buf, size_t buflen) const;


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
	get_version() const;


	/**
	 *
	 */
	void
	set_version(
			uint8_t of_version);


	/**
	 *
	 */
	uint16_t
	get_type() const;


	/**
	 *
	 */
	void
	set_type(
			uint16_t type);


	/**
	 *
	 */
	uint16_t
	get_length() const;


	/**
	 *
	 */
	void
	set_length(uint16_t len);


	/**
	 *
	 */
	uint32_t
	get_rate() const;


	/**
	 *
	 */
	void
	set_rate(
			uint32_t rate);


	/**
	 *
	 */
	uint32_t
	get_burst_size() const;


	/**
	 *
	 */
	void
	set_burst_size(
			uint32_t burst_size);


	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofmeter_band const& mb)
	{
		os << "MeterBand["
			<< "type: " << mb.get_type()
			<< "rate: " << mb.get_rate()
			<< "burst_size: " << mb.get_burst_size()
			<< "]";
		return os;
	};
};







class cofmeter_band_drop :
		public cofmeter_band
{
private:

	union {
		uint8_t							*ofmu_drop;
		struct openflow13::ofp_meter_band_drop	*ofmu13_drop;
	} ofm_ofmu;

#define ofm_drop ofm_ofmu.ofmu_drop
#define ofm13_drop ofm_ofmu.ofmu13_drop

public:

	/**
	 *
	 */
	cofmeter_band_drop(
			uint8_t of_version);


	/**
	 *
	 */
	virtual
	~cofmeter_band_drop();



	/**
	 *
	 */
	cofmeter_band_drop(
			cofmeter_band const& mb);



	/**
	 *
	 */
	cofmeter_band_drop&
	operator= (
			cofmeter_band const& mb);


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
	pack(uint8_t *buf, size_t buflen) const;


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);
};







class cofmeter_band_dscp_remark :
		public cofmeter_band
{
private:

	union {
		uint8_t								*ofmu_dscp_remark;
		struct openflow13::ofp_meter_band_dscp_remark	*ofmu13_dscp_remark;
	} ofm_ofmu;

#define ofm_dscp_remark ofm_ofmu.ofmu_dscp_remark
#define ofm13_dscp_remark ofm_ofmu.ofmu13_dscp_remark

public:

	/**
	 *
	 */
	cofmeter_band_dscp_remark(
			uint8_t of_version, uint8_t prec_level = 0);


	/**
	 *
	 */
	virtual
	~cofmeter_band_dscp_remark();



	/**
	 *
	 */
	cofmeter_band_dscp_remark(
			cofmeter_band const& mb);



	/**
	 *
	 */
	cofmeter_band_dscp_remark&
	operator= (
			cofmeter_band const& mb);



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
	pack(uint8_t *buf, size_t buflen) const;


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
	get_prec_level() const;


	/**
	 *
	 */
	void
	set_prec_level(uint8_t prec_level);

};





class cofmeter_band_expr :
		public cofmeter_band
{
private:

	union {
		uint8_t									*ofmu_expr;
		struct openflow13::ofp_meter_band_experimenter	*ofmu13_expr;
	} ofm_ofmu;

#define ofm_expr ofm_ofmu.ofmu_expr
#define ofm13_expr ofm_ofmu.ofmu13_expr

	cmemory body;

public:

	/**
	 *
	 */
	cofmeter_band_expr(
			uint8_t of_version,
			uint32_t exp_id = 0,
			cmemory const& body = cmemory(0));


	/**
	 *
	 */
	virtual
	~cofmeter_band_expr();


	/**
	 *
	 */
	cofmeter_band_expr(
			cofmeter_band const& mb);


	/**
	 *
	 */
	cofmeter_band_expr&
	operator= (
			cofmeter_band_expr const& mb);


	/**
	 *
	 */
	cofmeter_band_expr&
	operator= (
			cofmeter_band const& mb);



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
	pack(uint8_t *buf, size_t buflen) const;


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
	get_expr() const;


	/**
	 *
	 */
	void
	set_expr(uint32_t expr);


	/**
	 *
	 */
	cmemory&
	get_body();
};


}

#endif /* COFMETERBAND_H_ */
