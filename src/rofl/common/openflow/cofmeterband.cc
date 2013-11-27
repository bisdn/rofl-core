/*
 * ofmeterband.cc
 *
 *  Created on: 15.06.2013
 *      Author: andi
 */

#include "cofmeterband.h"

using namespace rofl;

cofmeter_band::cofmeter_band(
		uint8_t of_version) :
				of_version(of_version)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		resize(sizeof(struct openflow13::ofp_meter_band_header));
	} break;
	default: {
		throw eBadVersion();
	}
	}
	ofm_header = somem();
	set_length(memlen());
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
cofmeter_band::operator =(
		cofmeter_band const& mb)
{
	if (this == &mb)
		return *this;

	cmemory::operator =(mb);

	ofm_header = somem();
	of_version = mb.of_version;

	return *this;
}



uint8_t*
cofmeter_band::resize(
		size_t len)
{
	cmemory::resize(len);
	ofm_header = somem();
	return somem();
}



size_t
cofmeter_band::length() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		return sizeof(struct openflow13::ofp_meter_band_header);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band::pack(uint8_t *buf, size_t buflen) const
{
	if (buflen < length())
		throw eInval();

	switch (of_version) {
	case openflow13::OFP_VERSION: {
		memcpy(buf, somem(), memlen());
	} break;
	default: {
		throw eBadVersion();
	}
	}
}



void
cofmeter_band::unpack(uint8_t *buf, size_t buflen)
{
	if (memlen() < buflen)
		resize(buflen);

	switch (of_version) {
	case openflow13::OFP_VERSION: {
		memcpy(somem(), buf, buflen);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}

	ofm_header = somem();
}



uint8_t
cofmeter_band::get_version() const
{
	return of_version;
}



void
cofmeter_band::set_version(
		uint8_t of_version)
{
	this->of_version = of_version;
}



uint16_t
cofmeter_band::get_type() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		return be16toh(ofm13_header->type);
	} break;
	default: {
		throw eBadVersion();
	}
	}
	return 0;
}


void
cofmeter_band::set_type(uint16_t type)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		ofm13_header->type = htobe16(type);
	} break;
	default: {
		throw eBadVersion();
	}
	}
}



uint16_t
cofmeter_band::get_length() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		return be16toh(ofm13_header->len);
	} break;
	default: {
		throw eBadVersion();
	}
	}
	return 0;
}



void
cofmeter_band::set_length(uint16_t len)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		ofm13_header->len = htobe16(len);
	} break;
	default: {
		throw eBadVersion();
	}
	}
}



uint32_t
cofmeter_band::get_rate() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		return be32toh(ofm13_header->rate);
	} break;
	default: {
		throw eBadVersion();
	}
	}
	return 0;
}



void
cofmeter_band::set_rate(uint32_t rate)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		ofm13_header->rate = htobe32(rate);
	} break;
	default: {
		throw eBadVersion();
	}
	}
}



uint32_t
cofmeter_band::get_burst_size() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		return be32toh(ofm13_header->burst_size);
	} break;
	default: {
		throw eBadVersion();
	}
	}
	return 0;
}



void
cofmeter_band::set_burst_size(uint32_t burst_size)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		ofm13_header->burst_size = htobe32(burst_size);
	} break;
	default: {
		throw eBadVersion();
	}
	}
}






cofmeter_band_drop::cofmeter_band_drop(
	uint8_t of_version) :
		cofmeter_band(of_version)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		resize(sizeof(struct openflow13::ofp_meter_band_drop));
		set_length(sizeof(struct openflow13::ofp_meter_band_drop));
	} break;
	default: {
		throw eBadVersion();
	}
	}
	ofm_drop = somem();
	set_type(OFPMBT_DROP);
}




cofmeter_band_drop::~cofmeter_band_drop()
{

}



cofmeter_band_drop::cofmeter_band_drop(
	cofmeter_band const& mb) :
		cofmeter_band(mb.get_version())
{
	*this = mb;
}



cofmeter_band_drop&
cofmeter_band_drop::operator= (
		cofmeter_band const& mb)
{
	if (this == &mb)
		return *this;

	if (OFPMBT_DROP != mb.get_type())
		throw eInval();

	unpack(mb.somem(), mb.memlen());

	return *this;
}



size_t
cofmeter_band_drop::length() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		return sizeof(struct openflow13::ofp_meter_band_drop);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_drop::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
	} break;
	default: {
		throw eBadVersion();
	}
	}

	cofmeter_band::pack(buf, buflen);
}



void
cofmeter_band_drop::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
	} break;
	default: {
		throw eBadVersion();
	}
	}

	cofmeter_band::unpack(buf, buflen);
	ofm_drop = somem();
}









cofmeter_band_dscp_remark::cofmeter_band_dscp_remark(
	uint8_t of_version,
	uint8_t prec_level) :
		cofmeter_band(of_version)
{
	switch (of_version) {
	case openflow12::OFP_VERSION: {
		resize(sizeof(struct openflow13::ofp_meter_band_dscp_remark));
		set_length(sizeof(struct openflow13::ofp_meter_band_dscp_remark));
	} break;
	default: {
		throw eBadVersion();
	}
	}
	ofm_dscp_remark = somem();
	set_type(OFPMBT_DSCP_REMARK);
	set_prec_level(prec_level);
}




cofmeter_band_dscp_remark::~cofmeter_band_dscp_remark()
{

}



cofmeter_band_dscp_remark::cofmeter_band_dscp_remark(
	cofmeter_band const& mb) :
		cofmeter_band(mb.get_version())
{
	*this = mb;
}



cofmeter_band_dscp_remark&
cofmeter_band_dscp_remark::operator= (
	cofmeter_band const& mb)
{
	if (this == &mb)
		return *this;

	if (OFPMBT_DSCP_REMARK != mb.get_type())
		throw eInval();

	unpack(mb.somem(), mb.memlen());

	return *this;
}



size_t
cofmeter_band_dscp_remark::length() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		return sizeof(struct openflow13::ofp_meter_band_dscp_remark);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_dscp_remark::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
	} break;
	default: {
		throw eBadVersion();
	}
	}

	cofmeter_band::pack(buf, buflen);
}



void
cofmeter_band_dscp_remark::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
	} break;
	default: {
		throw eBadVersion();
	}
	}

	cofmeter_band::unpack(buf, buflen);
	ofm_dscp_remark = somem();
}



uint8_t
cofmeter_band_dscp_remark::get_prec_level() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		return ofm13_dscp_remark->prec_level;
	} break;
	default: {
		throw eBadVersion();
	}
	}
	return 0;
}



void
cofmeter_band_dscp_remark::set_prec_level(uint8_t prec_level)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		ofm13_dscp_remark->prec_level = prec_level;
	} break;
	default: {
		throw eBadVersion();
	}
	}
}








cofmeter_band_expr::cofmeter_band_expr(
	uint8_t of_version,
	uint32_t exp_id,
	cmemory const& body) :
		cofmeter_band(of_version),
		body(body)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		resize(sizeof(struct openflow13::ofp_meter_band_experimenter) + body.memlen());
		set_length(sizeof(struct openflow13::ofp_meter_band_experimenter) + body.memlen());
	} break;
	default: {
		throw eBadVersion();
	}
	}
	ofm_expr = somem();
	set_type(OFPMBT_EXPERIMENTER);
}




cofmeter_band_expr::~cofmeter_band_expr()
{

}



cofmeter_band_expr::cofmeter_band_expr(
	cofmeter_band const& mb) :
		cofmeter_band(mb.get_version()),
		body(0)
{
	*this = mb;
}



cofmeter_band_expr&
cofmeter_band_expr::operator= (
	cofmeter_band_expr const& mb)
{
	if (this == &mb)
		return *this;

	cofmeter_band::operator=(mb);

	of_version = mb.of_version;
	ofm_expr = somem();
	body = mb.body;

	return *this;
}



cofmeter_band_expr&
cofmeter_band_expr::operator= (
	cofmeter_band const& mb)
{
	if (this == &mb)
		return *this;

	if (OFPMBT_EXPERIMENTER != mb.get_type())
		throw eInval();

	of_version = mb.get_version();
	unpack(mb.somem(), mb.memlen());

	return *this;
}


size_t
cofmeter_band_expr::length() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_meter_band_experimenter) + body.memlen());
	} break;
	default: {
		throw eBadVersion();
	}
	}
	return 0;
}



void
cofmeter_band_expr::pack(uint8_t *buf, size_t buflen) const
{
	if (buflen < length())
		throw eInval();

	switch (of_version) {
	case openflow13::OFP_VERSION: {
		memcpy(buf, somem(), memlen());
		memcpy(buf + memlen(), body.somem(), body.memlen());
		struct openflow13::ofp_meter_band_header* mb = (struct openflow13::ofp_meter_band_header*)buf;
		mb->len = htobe16(length());
	} break;
	default: {
		throw eBadVersion();
	}
	}
}



void
cofmeter_band_expr::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct openflow13::ofp_meter_band_experimenter))
			throw eInval();
		cofmeter_band::resize(sizeof(struct openflow13::ofp_meter_band_experimenter));
		ofm_expr = somem();
		memcpy(somem(), buf, sizeof(struct openflow13::ofp_meter_band_experimenter));
		if (buflen > sizeof(struct openflow13::ofp_meter_band_experimenter)) {
			body.resize(buflen - sizeof(struct openflow13::ofp_meter_band_experimenter));
			memcpy(body.somem(), buf + sizeof(struct openflow13::ofp_meter_band_experimenter),
					body.memlen());
		}
	} break;
	default: {
		throw eBadVersion();
	}
	}

	cofmeter_band::unpack(buf, buflen);
	ofm_expr = somem();
}



uint32_t
cofmeter_band_expr::get_expr() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		return be32toh(ofm13_expr->experimenter);
	} break;
	default: {
		throw eBadVersion();
	}
	}
	return 0;
}



void
cofmeter_band_expr::set_expr(uint32_t expr)
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		ofm13_expr->experimenter = htobe32(expr);
	} break;
	default: {
		throw eBadVersion();
	}
	}
}



cmemory&
cofmeter_band_expr::get_body()
{
	return body;
}


