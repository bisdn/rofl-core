/*
 * ofmeterband.cc
 *
 *  Created on: 15.06.2013
 *      Author: andi
 */

#include "rofl/common/openflow/cofmeterband.h"

using namespace rofl::openflow;

cofmeter_band::cofmeter_band(
		uint8_t of_version,
		size_t len) :
				of_version(of_version),
				type(0),
				len(len),
				rate(0),
				burst_size(0),
				body(len - sizeof(struct rofl::openflow13::ofp_meter_band_header))
{

}



cofmeter_band::~cofmeter_band()
{

}



cofmeter_band::cofmeter_band(
		cofmeter_band const& mb)
{
	*this = mb;
}



cofmeter_band&
cofmeter_band::operator= (
		cofmeter_band const& mb)
{
	if (this == &mb)
		return *this;

	of_version 	= mb.of_version;
	type		= mb.type;
	len			= mb.len;
	rate		= mb.rate;
	burst_size	= mb.burst_size;
	body		= mb.body;

	return *this;
}



const bool
cofmeter_band::operator== (
		const cofmeter_band& mb) const
{
	return ((of_version == mb.of_version) &&
			(type == mb.type) &&
			(len == mb.len) &&
			(rate == mb.rate) &&
			(burst_size == mb.burst_size) &&
			(body == mb.body));
}



size_t
cofmeter_band::length() const
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_meter_band_header) + body.memlen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		len = length();

		struct rofl::openflow13::ofp_meter_band_header *mbh =
				(struct rofl::openflow13::ofp_meter_band_header*)buf;
		mbh->type 		= htobe16(type);
		mbh->len		= htobe16(len);
		mbh->rate		= htobe32(rate);
		mbh->burst_size	= htobe32(burst_size);
		memcpy(mbh->body, body.somem(), body.memlen());
	} break;
	default: {
		throw eBadVersion();
	}
	}
}



void
cofmeter_band::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (of_version) {
	case openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_meter_band_header)) {
			LOGGING_ERROR << "[rofl][cofmeterband][unpack] buffer too short" << std::endl;
			throw eBadRequestBadLen();
		}

		struct rofl::openflow13::ofp_meter_band_header *mbh =
				(struct rofl::openflow13::ofp_meter_band_header*)buf;

		type			= be16toh(mbh->type);
		len				= be16toh(mbh->len);
		rate			= be32toh(mbh->rate);
		burst_size		= be32toh(mbh->burst_size);

		if (len > buflen) {
			LOGGING_ERROR << "[rofl][cofmeterband][unpack] invalid length field" << std::endl;
			throw eMeterModBadBand();
		}

		body.assign(mbh->body, buflen - sizeof(struct rofl::openflow13::ofp_meter_band_header));

	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}





cofmeter_band_drop::cofmeter_band_drop(
	uint8_t of_version) :
		cofmeter_band(of_version, sizeof(struct rofl::openflow13::ofp_meter_band_drop))
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPMBT_DROP);
	} break;
	default: {
		// do nothing
	};
	}
}



cofmeter_band_drop::~cofmeter_band_drop()
{

}



cofmeter_band_drop::cofmeter_band_drop(
	const cofmeter_band_drop& mb) :
		cofmeter_band(mb.get_version())
{
	*this = mb;
}



cofmeter_band_drop&
cofmeter_band_drop::operator= (
		const cofmeter_band_drop& mb)
{
	if (this == &mb)
		return *this;

	cofmeter_band::operator= (mb);

	return *this;
}



const bool
cofmeter_band_drop::operator== (
		const cofmeter_band_drop& mb) const
{
	return (cofmeter_band::operator== (mb));
}








cofmeter_band_dscp_remark::cofmeter_band_dscp_remark(
	uint8_t of_version) :
		cofmeter_band(of_version, sizeof(struct rofl::openflow13::ofp_meter_band_header)),
		prec_level(0)
{
	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPMBT_DSCP_REMARK);
	} break;
	default: {
		// do nothing
	};
	}
}



cofmeter_band_dscp_remark::~cofmeter_band_dscp_remark()
{

}



cofmeter_band_dscp_remark::cofmeter_band_dscp_remark(
	const cofmeter_band_dscp_remark& mb)
{
	*this = mb;
}



cofmeter_band_dscp_remark&
cofmeter_band_dscp_remark::operator= (
	const cofmeter_band_dscp_remark& mb)
{
	if (this == &mb)
		return *this;

	cofmeter_band::operator= (mb);
	prec_level = mb.prec_level;

	return *this;
}



const bool
cofmeter_band_dscp_remark::operator== (
		const cofmeter_band_dscp_remark& mb) const
{
	return (cofmeter_band::operator== (mb) && (prec_level == mb.prec_level));
}



size_t
cofmeter_band_dscp_remark::length() const
{
	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_meter_band_dscp_remark));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_dscp_remark::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < length())
			throw eInval();

		cofmeter_band::pack(buf, buflen);

		struct rofl::openflow13::ofp_meter_band_dscp_remark* mbh =
				(struct rofl::openflow13::ofp_meter_band_dscp_remark*)buf;
		mbh->prec_level = prec_level;
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_dscp_remark::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	cofmeter_band::unpack(buf, buflen);

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < length())
			throw eInval();

		set_body() = rofl::cmemory(0);

		struct rofl::openflow13::ofp_meter_band_dscp_remark* mbh =
				(struct rofl::openflow13::ofp_meter_band_dscp_remark*)buf;
		prec_level = mbh->prec_level;

	} break;
	default: {
		throw eBadVersion();
	}
	}
}










cofmeter_band_experimenter::cofmeter_band_experimenter(
	uint8_t of_version) :
		cofmeter_band(of_version, sizeof(struct rofl::openflow13::ofp_meter_band_header)), // yes, do not store exp_id here
		exp_id(0)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPMBT_EXPERIMENTER);
	} break;
	default: {
		throw eBadVersion();
	}
	}
}



cofmeter_band_experimenter::~cofmeter_band_experimenter()
{

}



cofmeter_band_experimenter::cofmeter_band_experimenter(
	const cofmeter_band_experimenter& mb)
{
	*this = mb;
}



cofmeter_band_experimenter&
cofmeter_band_experimenter::operator= (
	cofmeter_band_experimenter const& mb)
{
	if (this == &mb)
		return *this;

	cofmeter_band::operator=(mb);

	exp_id		= mb.exp_id;
	exp_body	= mb.exp_body;

	return *this;
}



const bool
cofmeter_band_experimenter::operator== (
		const cofmeter_band_experimenter& mb) const
{
	return (cofmeter_band::operator== (mb) && (exp_id == mb.exp_id) && (exp_body == mb.exp_body));
}



size_t
cofmeter_band_experimenter::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_meter_band_experimenter) + exp_body.memlen());
	} break;
	default: {
		throw eBadVersion();
	}
	}
	return 0;
}



void
cofmeter_band_experimenter::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < length()) {
			LOGGING_ERROR << "[rofl][cofmeterband_experimenter][pack] invalid length" << std::endl;
			throw eMeterModBadBand();
		}

		// pack common header
		cofmeter_band::pack(buf, buflen);

		struct rofl::openflow13::ofp_meter_band_experimenter* mbh =
				(struct rofl::openflow13::ofp_meter_band_experimenter*)buf;
		mbh->experimenter 	= htobe32(exp_id);
		memcpy(mbh->body, exp_body.somem(), exp_body.memlen());
	} break;
	default: {
		throw eBadVersion();
	}
	}
}



void
cofmeter_band_experimenter::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < sizeof(struct openflow13::ofp_meter_band_experimenter)) {
			LOGGING_ERROR << "[rofl][cofmeterband_experimenter][unpack] invalid length" << std::endl;
			throw eMeterModBadBand();
		}

		// unpack common header
		cofmeter_band::unpack(buf, buflen);

		set_body() = rofl::cmemory(0);

		struct rofl::openflow13::ofp_meter_band_experimenter* mbh =
				(struct rofl::openflow13::ofp_meter_band_experimenter*)buf;

		exp_id = be32toh(mbh->experimenter);

		if (buflen > sizeof(struct openflow13::ofp_meter_band_experimenter)) {
			exp_body.assign(mbh->body, buflen -
					sizeof(struct openflow13::ofp_meter_band_experimenter));
		}
	} break;
	default: {
		throw eBadVersion();
	}
	}
}




