/*
 * cofmeterbandset.cc
 *
 *  Created on: 16.06.2013
 *      Author: andreas
 */

#include "cofmeterbandset.h"

using namespace rofl;


cofmeter_band_set::cofmeter_band_set(
		uint8_t of_version) :
				of_version(of_version)
{

}



cofmeter_band_set::~cofmeter_band_set()
{

}



cofmeter_band_set::cofmeter_band_set(
		cofmeter_band_set const& mbset)
{
	*this = mbset;
}



cofmeter_band_set&
cofmeter_band_set::operator= (
		cofmeter_band_set const& mbs)
{
	if (this == &mbs)
		return *this;

	clean_up();

	for (std::map<enum openflow13::ofp_meter_band_type, cofmeter_band*>::const_iterator
			it = mbs.mbset.begin(); it != mbs.mbset.end(); ++it) {
		switch (it->first) {
		case OFPMBT_DROP: {
			mbset[OFPMBT_DROP] = new cofmeter_band_drop(*(it->second));
		} break;
		case OFPMBT_DSCP_REMARK: {
			mbset[OFPMBT_DSCP_REMARK] = new cofmeter_band_dscp_remark(*(it->second));
		} break;
		case OFPMBT_EXPERIMENTER: {
			mbset[OFPMBT_EXPERIMENTER] = new cofmeter_band_expr(*(it->second));
		} break;
		default: {
			throw eNotImplemented();
		}
		}
	}

	return *this;
}



void
cofmeter_band_set::clean_up()
{
	for (std::map<enum openflow13::ofp_meter_band_type, cofmeter_band*>::iterator
			it = mbset.begin(); it != mbset.end(); ++it) {
		delete (it->second);
	}
	mbset.clear();
}



cofmeter_band_drop&
cofmeter_band_set::get_meter_band_drop()
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		if (mbset.find(OFPMBT_DROP) == mbset.end())
			throw eOFMeterBandSetNotFound();
		return *(dynamic_cast<cofmeter_band_drop*>( mbset[OFPMBT_DROP] ));
	} break;
	default:
		throw eBadVersion();
	}
}



cofmeter_band_dscp_remark&
cofmeter_band_set::get_meter_band_dscp_remark()
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		if (mbset.find(OFPMBT_DSCP_REMARK) == mbset.end())
			throw eOFMeterBandSetNotFound();
		return *(dynamic_cast<cofmeter_band_dscp_remark*>( mbset[OFPMBT_DSCP_REMARK] ));
	} break;
	default:
		throw eBadVersion();
	}
}



cofmeter_band_expr&
cofmeter_band_set::get_meter_band_expr()
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		if (mbset.find(OFPMBT_EXPERIMENTER) == mbset.end())
			throw eOFMeterBandSetNotFound();
		return *(dynamic_cast<cofmeter_band_expr*>( mbset[OFPMBT_EXPERIMENTER] ));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_set::set_meter_band(
		cofmeter_band const& mb)
{
	if (mbset.find((enum openflow13::ofp_meter_band_type)(mb.get_type())) == mbset.end()) {
		switch (mb.get_type()) {
		case OFPMBT_DROP: {
			mbset[OFPMBT_DROP] = new cofmeter_band_drop(mb);
		} break;
		case OFPMBT_DSCP_REMARK: {
			mbset[OFPMBT_DSCP_REMARK] = new cofmeter_band_dscp_remark(mb);
		} break;
		case OFPMBT_EXPERIMENTER: {
			mbset[OFPMBT_EXPERIMENTER] = new cofmeter_band_expr(mb);
		} break;
		default: {
			throw eNotImplemented();
		}
		}
	} else {
		switch (mb.get_type()) {
		case OFPMBT_DROP: {
			*mbset[OFPMBT_DROP] = cofmeter_band_drop(mb);
		} break;
		case OFPMBT_DSCP_REMARK: {
			*mbset[OFPMBT_DSCP_REMARK] = cofmeter_band_dscp_remark(mb);
		} break;
		case OFPMBT_EXPERIMENTER: {
			*mbset[OFPMBT_EXPERIMENTER] = cofmeter_band_expr(mb);
		} break;
		default: {
			throw eNotImplemented();
		}
		}
	}
}



void
cofmeter_band_set::clear_meter_band(
		enum openflow13::ofp_meter_band_type mbt)
{
	if (mbset.find(mbt) == mbset.end())
		return;
	delete mbset[mbt];
	mbset.erase(mbt);
}



size_t
cofmeter_band_set::length() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		size_t len = 0;
		for (std::map<enum openflow13::ofp_meter_band_type, cofmeter_band*>::const_iterator
				it = mbset.begin(); it != mbset.end(); ++it) {
			len += (it->second)->length();
		}
		return len;
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_set::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	unsigned int offset = 0;

	switch (of_version) {
	case openflow13::OFP_VERSION: {
		for (std::map<enum openflow13::ofp_meter_band_type, cofmeter_band*>::iterator
				it = mbset.begin(); it != mbset.end(); ++it) {
			cofmeter_band& mb = *(it->second);
			mb.pack(buf + offset, mb.length());
			offset += mb.length();
		}
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_band_set::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	clean_up();

	switch (of_version) {
	case openflow13::OFP_VERSION: {

		unsigned int offset = 0;
		while (offset < buflen) {
			struct openflow13::ofp_meter_band_header *hdr = (struct openflow13::ofp_meter_band_header*)(buf + offset);
			if (((buflen - offset) < sizeof(struct openflow13::ofp_meter_band_header)) ||
					(be16toh(hdr->len) < sizeof(struct openflow13::ofp_meter_band_header)))
				throw eInval();
			switch (be16toh(hdr->type)) {
			case OFPMBT_DROP: {
				mbset[OFPMBT_DROP] = new cofmeter_band_drop(of_version);
				mbset[OFPMBT_DROP]->pack(buf, be16toh(hdr->len));
				offset += mbset[OFPMBT_DROP]->get_length();
			} break;
			case OFPMBT_DSCP_REMARK: {
				mbset[OFPMBT_DSCP_REMARK] = new cofmeter_band_dscp_remark(of_version);
				mbset[OFPMBT_DSCP_REMARK]->pack(buf, be16toh(hdr->len));
				offset += mbset[OFPMBT_DSCP_REMARK]->get_length();
			} break;
			case OFPMBT_EXPERIMENTER: {
				mbset[OFPMBT_EXPERIMENTER] = new cofmeter_band_expr(of_version);
				mbset[OFPMBT_EXPERIMENTER]->pack(buf, be16toh(hdr->len));
				offset += mbset[OFPMBT_EXPERIMENTER]->get_length();
			} break;
			default:
				throw eNotImplemented();
			}
		}

	} break;
	default:
		throw eBadVersion();
	}
}



