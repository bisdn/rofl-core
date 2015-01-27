/*
 * cofmeterbands.cc
 *
 *  Created on: 16.06.2013
 *      Author: andreas
 */

#include "rofl/common/openflow/cofmeterbands.h"

using namespace rofl::openflow;


cofmeter_bands::cofmeter_bands(
		uint8_t of_version) :
				of_version(of_version)
{

}



cofmeter_bands::~cofmeter_bands()
{
	clear();
}



cofmeter_bands::cofmeter_bands(
		cofmeter_bands const& mbs)
{
	*this = mbs;
}



cofmeter_bands&
cofmeter_bands::operator= (
		cofmeter_bands const& mbs)
{
	if (this == &mbs)
		return *this;

	clear();

	of_version = mbs.of_version;

	for (std::map<unsigned int, cofmeter_band_drop>::const_iterator
			it = mbs.mbs_drop.begin(); it != mbs.mbs_drop.end(); ++it) {
		mbs_drop[it->first] = it->second;
	}

	for (std::map<unsigned int, cofmeter_band_dscp_remark>::const_iterator
			it = mbs.mbs_dscp_remark.begin(); it != mbs.mbs_dscp_remark.end(); ++it) {
		mbs_dscp_remark[it->first] = it->second;
	}

	for (std::map<unsigned int, cofmeter_band_experimenter>::const_iterator
			it = mbs.mbs_experimenter.begin(); it != mbs.mbs_experimenter.end(); ++it) {
		mbs_experimenter[it->first] = it->second;
	}

	return *this;
}



const bool
cofmeter_bands::operator== (
		const cofmeter_bands& mbands)
{
	if ((mbs_drop.size() != mbands.mbs_drop.size()) ||
			(mbs_dscp_remark.size() != mbands.mbs_dscp_remark.size()) ||
				(mbs_experimenter.size() != mbands.mbs_experimenter.size())) {
		return false;
	}
	for (std::map<unsigned int, cofmeter_band_drop>::const_iterator
			it = mbs_drop.begin(), jt = mbands.mbs_drop.begin();
				it != mbs_drop.end(); ++it, ++jt) {
		if (not (it->second == jt->second))
			return false;
	}
	for (std::map<unsigned int, cofmeter_band_dscp_remark>::const_iterator
			it = mbs_dscp_remark.begin(), jt = mbands.mbs_dscp_remark.begin();
				it != mbs_dscp_remark.end(); ++it, ++jt) {
		if (not (it->second == jt->second))
			return false;
	}
	for (std::map<unsigned int, cofmeter_band_experimenter>::const_iterator
			it = mbs_experimenter.begin(), jt = mbands.mbs_experimenter.begin();
				it != mbs_experimenter.end(); ++it, ++jt) {
		if (not (it->second == jt->second))
			return false;
	}
	return true;
}



void
cofmeter_bands::clear()
{
	mbs_drop.clear();
	mbs_dscp_remark.clear();
	mbs_experimenter.clear();
}



unsigned int
cofmeter_bands::get_num_of_mbs() const
{
	return (mbs_drop.size() + mbs_dscp_remark.size() + mbs_experimenter.size());
}



cofmeter_band_drop&
cofmeter_bands::add_meter_band_drop(
		unsigned int index)
{
	if (mbs_drop.find(index) != mbs_drop.end()) {
		mbs_drop.erase(index);
	}
	return (mbs_drop[index] = cofmeter_band_drop(of_version));
}



void
cofmeter_bands::drop_meter_band_drop(
		unsigned int index)
{
	if (mbs_drop.find(index) == mbs_drop.end()) {
		return;
	}
	mbs_drop.erase(index);
}



cofmeter_band_drop&
cofmeter_bands::set_meter_band_drop(
		unsigned int index)
{
	if (mbs_drop.find(index) == mbs_drop.end()) {
		mbs_drop[index] = cofmeter_band_drop(of_version);
	}
	return mbs_drop[index];
}



const cofmeter_band_drop&
cofmeter_bands::get_meter_band_drop(
		unsigned int index) const
{
	if (mbs_drop.find(index) == mbs_drop.end()) {
		throw eRofMeterBandNotFound();
	}
	return mbs_drop.at(index);
}



bool
cofmeter_bands::has_meter_band_drop(
		unsigned int index) const
{
	return (not (mbs_drop.find(index) == mbs_drop.end()));
}



cofmeter_band_dscp_remark&
cofmeter_bands::add_meter_band_dscp_remark(
		unsigned int index)
{
	if (mbs_dscp_remark.find(index) != mbs_dscp_remark.end()) {
		mbs_dscp_remark.erase(index);
	}
	return (mbs_dscp_remark[index] = cofmeter_band_dscp_remark(of_version));
}



void
cofmeter_bands::drop_meter_band_dscp_remark(
		unsigned int index)
{
	if (mbs_dscp_remark.find(index) == mbs_dscp_remark.end()) {
		return;
	}
	mbs_dscp_remark.erase(index);
}



cofmeter_band_dscp_remark&
cofmeter_bands::set_meter_band_dscp_remark(
		unsigned int index)
{
	if (mbs_dscp_remark.find(index) == mbs_dscp_remark.end()) {
		mbs_dscp_remark[index] = cofmeter_band_dscp_remark(of_version);
	}
	return mbs_dscp_remark[index];
}



const cofmeter_band_dscp_remark&
cofmeter_bands::get_meter_band_dscp_remark(
		unsigned int index) const
{
	if (mbs_dscp_remark.find(index) == mbs_dscp_remark.end()) {
		throw eRofMeterBandNotFound();
	}
	return mbs_dscp_remark.at(index);
}



bool
cofmeter_bands::has_meter_band_dscp_remark(
		unsigned int index) const
{
	return (not (mbs_dscp_remark.find(index) == mbs_dscp_remark.end()));
}



cofmeter_band_experimenter&
cofmeter_bands::add_meter_band_experimenter(
		unsigned int index)
{
	if (mbs_experimenter.find(index) != mbs_experimenter.end()) {
		mbs_experimenter.erase(index);
	}
	return (mbs_experimenter[index] = cofmeter_band_experimenter(of_version));
}



void
cofmeter_bands::drop_meter_band_experimenter(
		unsigned int index)
{
	if (mbs_experimenter.find(index) == mbs_experimenter.end()) {
		return;
	}
	mbs_experimenter.erase(index);
}



cofmeter_band_experimenter&
cofmeter_bands::set_meter_band_experimenter(
		unsigned int index)
{
	if (mbs_experimenter.find(index) == mbs_experimenter.end()) {
		mbs_experimenter[index] = cofmeter_band_experimenter(of_version);
	}
	return mbs_experimenter[index];
}



const cofmeter_band_experimenter&
cofmeter_bands::get_meter_band_experimenter(
		unsigned int index) const
{
	if (mbs_experimenter.find(index) == mbs_experimenter.end()) {
		throw eRofMeterBandNotFound();
	}
	return mbs_experimenter.at(index);
}



bool
cofmeter_bands::has_meter_band_experimenter(
		unsigned int index) const
{
	return (not (mbs_experimenter.find(index) == mbs_experimenter.end()));
}








size_t
cofmeter_bands::length() const
{
	switch (of_version) {
	case openflow13::OFP_VERSION: {
		size_t len = 0;
		for (std::map<unsigned int, cofmeter_band_drop>::const_iterator
				it = mbs_drop.begin(); it != mbs_drop.end(); ++it) {
			len += it->second.length();
		}
		for (std::map<unsigned int, cofmeter_band_dscp_remark>::const_iterator
				it = mbs_dscp_remark.begin(); it != mbs_dscp_remark.end(); ++it) {
			len += it->second.length();
		}
		for (std::map<unsigned int, cofmeter_band_experimenter>::const_iterator
				it = mbs_experimenter.begin(); it != mbs_experimenter.end(); ++it) {
			len += it->second.length();
		}
		return len;
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_bands::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	unsigned int offset = 0;

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		for (std::map<unsigned int, cofmeter_band_drop>::iterator
				it = mbs_drop.begin(); it != mbs_drop.end(); ++it) {
			cofmeter_band& mb = it->second;
			mb.pack(buf + offset, mb.length());
			offset += mb.length();
		}
		for (std::map<unsigned int, cofmeter_band_dscp_remark>::iterator
				it = mbs_dscp_remark.begin(); it != mbs_dscp_remark.end(); ++it) {
			cofmeter_band& mb = it->second;
			mb.pack(buf + offset, mb.length());
			offset += mb.length();
		}
		for (std::map<unsigned int, cofmeter_band_experimenter>::iterator
				it = mbs_experimenter.begin(); it != mbs_experimenter.end(); ++it) {
			cofmeter_band& mb = it->second;
			mb.pack(buf + offset, mb.length());
			offset += mb.length();
		}
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeter_bands::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	clear();

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		unsigned int offset = 0;
		unsigned int index_drop = 0;
		unsigned int index_dscp_remark = 0;
		unsigned int index_experimenter = 0;

		while (offset < buflen) {
			struct rofl::openflow13::ofp_meter_band_header *mbh =
					(struct rofl::openflow13::ofp_meter_band_header*)(buf + offset);

			if (((buflen - offset) < sizeof(struct rofl::openflow13::ofp_meter_band_header)) ||
					(be16toh(mbh->len) < sizeof(struct rofl::openflow13::ofp_meter_band_header)))
				throw eMeterModBadBand();

			uint16_t type 	= be16toh(mbh->type);
			uint16_t len 	= be16toh(mbh->len);

			switch (type) {
			case rofl::openflow13::OFPMBT_DROP: {
				add_meter_band_drop(index_drop).unpack(buf + offset, len);
				offset += len;
				index_drop++;
			} break;
			case rofl::openflow13::OFPMBT_DSCP_REMARK: {
				add_meter_band_dscp_remark(index_dscp_remark).unpack(buf + offset, len);
				offset += len;
				index_dscp_remark++;
			} break;
			case rofl::openflow13::OFPMBT_EXPERIMENTER: {
				add_meter_band_experimenter(index_experimenter).unpack(buf + offset, len);
				offset += len;
				index_experimenter++;
			} break;
			default:
				LOGGING_WARN << "[rofl][cofmeterbands][unpack] unknown meter band type:" << (int)type << std::endl;
				offset += len;
			}
		}

	} break;
	default:
		throw eBadVersion();
	}
}



