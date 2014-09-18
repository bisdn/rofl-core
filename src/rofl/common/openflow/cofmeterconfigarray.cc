/*
 * cofmeterconfigarray.cc
 *
 *  Created on: 27.05.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/cofmeterconfigarray.h"

using namespace rofl::openflow;


cofmeterconfigarray::cofmeterconfigarray(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{

}


cofmeterconfigarray::~cofmeterconfigarray()
{

}


cofmeterconfigarray::cofmeterconfigarray(
		const cofmeterconfigarray& mconfig)
{
	*this = mconfig;
}


cofmeterconfigarray&
cofmeterconfigarray::operator= (
		const cofmeterconfigarray& mconfig)
{
	if (this == &mconfig)
		return *this;

	this->array.clear();

	ofp_version = mconfig.ofp_version;
	for (std::map<unsigned int, cofmeter_config_reply>::const_iterator
			it = mconfig.array.begin(); it != mconfig.array.end(); ++it) {
		this->array[it->first] = it->second;
	}

	return *this;
}



bool
cofmeterconfigarray::operator== (
		const cofmeterconfigarray& mconfig)
{
	if (ofp_version != mconfig.ofp_version)
		return false;

	if (array.size() != mconfig.array.size())
		return false;

	for (std::map<unsigned int, cofmeter_config_reply>::const_iterator
				it = array.begin(),jt = mconfig.array.begin();
					it != array.end(); ++it, ++jt) {
		if (not (const_cast<cofmeter_config_reply&>(it->second) == jt->second))
			return false;
	}

	return true;
}



cofmeterconfigarray&
cofmeterconfigarray::operator+= (
		const cofmeterconfigarray& mconfig)
{
	unsigned int index = 0;

	for (std::map<unsigned int, cofmeter_config_reply>::const_iterator
			it = mconfig.array.begin(); it != mconfig.array.end(); ++it) {
		while (array.find(index) != array.end()) {
			index++;
		}
		this->array[index] = it->second;
	}

	return *this;
}



size_t
cofmeterconfigarray::length() const
{
	size_t len = 0;
	for (std::map<unsigned int, cofmeter_config_reply>::const_iterator
			it = array.begin(); it != array.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
cofmeterconfigarray::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {

		for (std::map<unsigned int, cofmeter_config_reply>::iterator
				it = array.begin(); it != array.end(); ++it) {
			it->second.pack(buf, it->second.length());
			buf += it->second.length();
		}

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmeterconfigarray::unpack(uint8_t *buf, size_t buflen)
{
	array.clear();

	if ((0 == buf) || (0 == buflen))
		return;

	unsigned int offset = 0;
	unsigned int index = 0;

	switch (ofp_version) {
	case rofl::openflow13::OFP_VERSION: {

		while ((buflen - offset) >= sizeof(struct rofl::openflow13::ofp_meter_config)) {

			struct rofl::openflow13::ofp_meter_config* mconfig =
					(struct rofl::openflow13::ofp_meter_config*)(buf + offset);

			uint16_t mconfig_len = be16toh(mconfig->length);

			if (mconfig_len < sizeof(struct rofl::openflow13::ofp_meter_config)) {
				throw eInval();
			}

			add_meter_config(index++).unpack(buf, mconfig_len);

			offset += mconfig_len;
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofmeter_config_reply&
cofmeterconfigarray::add_meter_config(unsigned int index)
{
	if (array.find(index) != array.end()) {
		array.erase(index);
	}
	return (array[index] = cofmeter_config_reply(ofp_version));
}



void
cofmeterconfigarray::drop_meter_config(unsigned int index)
{
	if (array.find(index) == array.end()) {
		return;
	}
	array.erase(index);
}



cofmeter_config_reply&
cofmeterconfigarray::set_meter_config(unsigned int index)
{
	if (array.find(index) == array.end()) {
		array[index] = cofmeter_config_reply(ofp_version);
	}
	return array[index];
}



cofmeter_config_reply const&
cofmeterconfigarray::get_meter_config(unsigned int index) const
{
	if (array.find(index) == array.end()) {
		throw eRofMeterConfigNotFound();
	}
	return array.at(index);
}



bool
cofmeterconfigarray::has_meter_config(unsigned int index) const
{
	return (not (array.find(index) == array.end()));
}





