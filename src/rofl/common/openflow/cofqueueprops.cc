/*
 * cofqueueprops.cc
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */

#include "rofl/common/openflow/cofqueueprops.h"

using namespace rofl::openflow;

cofqueue_props::cofqueue_props(
		uint8_t ofp_version) :
			ofp_version(ofp_version)
{

}



cofqueue_props::~cofqueue_props()
{
	clear();
}



cofqueue_props::cofqueue_props(
		const cofqueue_props& props)
{
	*this = props;
}



cofqueue_props&
cofqueue_props::operator= (
		const cofqueue_props& props)
{
	if (this == &props)
		return *this;

	cofqueue_props::clear();

	ofp_version = props.ofp_version;

	for (std::map<uint16_t, cofqueue_prop*>::const_iterator
			it = props.get_properties().begin(); it != props.get_properties().end(); ++it) {
		switch (it->second->get_property()) {
		case rofl::openflow13::OFPQT_MIN_RATE: {
			add_queue_prop_min_rate() = props.get_queue_prop_min_rate();
		} break;
		case rofl::openflow13::OFPQT_MAX_RATE: {
			add_queue_prop_max_rate() = props.get_queue_prop_max_rate();
		} break;
		case rofl::openflow13::OFPQT_EXPERIMENTER: {
			add_queue_prop_experimenter() = props.get_queue_prop_experimenter();
		} break;
		default:
			LOGGING_WARN << "[rofl][cofqueue-props] ignoring unknown queue property, property-type:"
						<< it->second->get_property() << std::endl;
		}
	}

	return *this;
}



void
cofqueue_props::clear()
{
	for (std::map<uint16_t, cofqueue_prop*>::iterator
			it = properties.begin(); it != properties.end(); ++it) {
		delete it->second;
	}
	properties.clear();
}



cofqueue_prop_min_rate&
cofqueue_props::add_queue_prop_min_rate()
{
	if (properties.find(rofl::openflow13::OFPQT_MIN_RATE) != properties.end()) {
		delete properties[rofl::openflow13::OFPQT_MIN_RATE];
	}
	properties[rofl::openflow13::OFPQT_MIN_RATE] = new cofqueue_prop_min_rate(ofp_version);
	return (dynamic_cast<cofqueue_prop_min_rate&>( *properties[rofl::openflow13::OFPQT_MIN_RATE] ));
}



cofqueue_prop_min_rate&
cofqueue_props::set_queue_prop_min_rate()
{
	if (properties.find(rofl::openflow13::OFPQT_MIN_RATE) == properties.end()) {
		properties[rofl::openflow13::OFPQT_MIN_RATE] = new cofqueue_prop_min_rate(ofp_version);
	}
	return (dynamic_cast<cofqueue_prop_min_rate&>( *properties[rofl::openflow13::OFPQT_MIN_RATE] ));
}



const cofqueue_prop_min_rate&
cofqueue_props::get_queue_prop_min_rate() const
{
	if (properties.find(rofl::openflow13::OFPQT_MIN_RATE) == properties.end()) {
		throw eQueuePropNotFound();
	}
	return (dynamic_cast<const cofqueue_prop_min_rate&>( *properties.at(rofl::openflow13::OFPQT_MIN_RATE) ));
}



void
cofqueue_props::drop_queue_prop_min_rate()
{
	if (properties.find(rofl::openflow13::OFPQT_MIN_RATE) == properties.end()) {
		return;
	}
	delete properties[rofl::openflow13::OFPQT_MIN_RATE];
	properties.erase(rofl::openflow13::OFPQT_MIN_RATE);
}



bool
cofqueue_props::has_queue_prop_min_rate() const
{
	return (not (properties.find(rofl::openflow13::OFPQT_MIN_RATE) == properties.end()));
}



cofqueue_prop_max_rate&
cofqueue_props::add_queue_prop_max_rate()
{
	if (properties.find(rofl::openflow13::OFPQT_MAX_RATE) != properties.end()) {
		delete properties[rofl::openflow13::OFPQT_MAX_RATE];
	}
	properties[rofl::openflow13::OFPQT_MAX_RATE] = new cofqueue_prop_max_rate(ofp_version);
	return (dynamic_cast<cofqueue_prop_max_rate&>( *properties[rofl::openflow13::OFPQT_MAX_RATE] ));
}



cofqueue_prop_max_rate&
cofqueue_props::set_queue_prop_max_rate()
{
	if (properties.find(rofl::openflow13::OFPQT_MAX_RATE) == properties.end()) {
		properties[rofl::openflow13::OFPQT_MAX_RATE] = new cofqueue_prop_max_rate(ofp_version);
	}
	return (dynamic_cast<cofqueue_prop_max_rate&>( *properties[rofl::openflow13::OFPQT_MAX_RATE] ));
}



const cofqueue_prop_max_rate&
cofqueue_props::get_queue_prop_max_rate() const
{
	if (properties.find(rofl::openflow13::OFPQT_MAX_RATE) == properties.end()) {
		throw eQueuePropNotFound();
	}
	return (dynamic_cast<const cofqueue_prop_max_rate&>( *properties.at(rofl::openflow13::OFPQT_MAX_RATE) ));
}



void
cofqueue_props::drop_queue_prop_max_rate()
{
	if (properties.find(rofl::openflow13::OFPQT_MAX_RATE) == properties.end()) {
		return;
	}
	delete properties[rofl::openflow13::OFPQT_MAX_RATE];
	properties.erase(rofl::openflow13::OFPQT_MAX_RATE);
}



bool
cofqueue_props::has_queue_prop_max_rate() const
{
	return (not (properties.find(rofl::openflow13::OFPQT_MAX_RATE) == properties.end()));
}



cofqueue_prop_experimenter&
cofqueue_props::add_queue_prop_experimenter()
{
	if (properties.find(rofl::openflow13::OFPQT_EXPERIMENTER) != properties.end()) {
		delete properties[rofl::openflow13::OFPQT_EXPERIMENTER];
	}
	properties[rofl::openflow13::OFPQT_EXPERIMENTER] = new cofqueue_prop_experimenter(ofp_version);
	return (dynamic_cast<cofqueue_prop_experimenter&>( *properties[rofl::openflow13::OFPQT_EXPERIMENTER] ));
}



cofqueue_prop_experimenter&
cofqueue_props::set_queue_prop_experimenter()
{
	if (properties.find(rofl::openflow13::OFPQT_EXPERIMENTER) == properties.end()) {
		properties[rofl::openflow13::OFPQT_EXPERIMENTER] = new cofqueue_prop_experimenter(ofp_version);
	}
	return (dynamic_cast<cofqueue_prop_experimenter&>( *properties[rofl::openflow13::OFPQT_EXPERIMENTER] ));
}



const cofqueue_prop_experimenter&
cofqueue_props::get_queue_prop_experimenter() const
{
	if (properties.find(rofl::openflow13::OFPQT_EXPERIMENTER) == properties.end()) {
		throw eQueuePropNotFound();
	}
	return (dynamic_cast<const cofqueue_prop_experimenter&>( *properties.at(rofl::openflow13::OFPQT_EXPERIMENTER) ));
}



void
cofqueue_props::drop_queue_prop_experimenter()
{
	if (properties.find(rofl::openflow13::OFPQT_EXPERIMENTER) == properties.end()) {
		return;
	}
	delete properties[rofl::openflow13::OFPQT_EXPERIMENTER];
	properties.erase(rofl::openflow13::OFPQT_EXPERIMENTER);
}



bool
cofqueue_props::has_queue_prop_experimenter() const
{
	return (not (properties.find(rofl::openflow13::OFPQT_EXPERIMENTER) == properties.end()));
}



size_t
cofqueue_props::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION:
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		size_t len = 0;
		for (std::map<uint16_t, cofqueue_prop*>::const_iterator
				it = properties.begin(); it != properties.end(); ++it) {
			len += it->second->length();
		}
		return len;
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_props::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < cofqueue_props::length())
		throw eInval();

	switch (ofp_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		for (std::map<uint16_t, cofqueue_prop*>::iterator
				it = properties.begin(); it != properties.end(); ++it) {
			cofqueue_prop& prop = *(it->second);
			prop.pack(buf, prop.length());
			buf += prop.length();
			buflen -= prop.length();
		}
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofqueue_props::unpack(
		uint8_t *buf, size_t buflen)
{
	clear();

	switch (ofp_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {

		while (buflen >= sizeof(struct rofl::openflow13::ofp_queue_prop_header)) {

			struct rofl::openflow13::ofp_queue_prop_header* hdr =
					(struct rofl::openflow13::ofp_queue_prop_header*)buf;

			uint16_t property 	= be16toh(hdr->property);
			uint16_t len		= be16toh(hdr->len);

			if ((buflen < len) || (0 == len))
				throw eInval();

			switch (property) {
			case rofl::openflow13::OFPQT_MIN_RATE: {
				add_queue_prop_min_rate().unpack(buf, len);
				buf += set_queue_prop_min_rate().length();
				buflen -= set_queue_prop_min_rate().length();
			} break;
			case rofl::openflow13::OFPQT_MAX_RATE: {
				add_queue_prop_max_rate().unpack(buf, len);
				buf += set_queue_prop_max_rate().length();
				buflen -= set_queue_prop_max_rate().length();
			} break;
			case rofl::openflow13::OFPQT_EXPERIMENTER: {
				add_queue_prop_experimenter().unpack(buf, len);
				buf += set_queue_prop_experimenter().length();
				buflen -= set_queue_prop_experimenter().length();
			} break;
			default: {
				LOGGING_WARN << "[rofl][cofqueue-props] ignoring unknown queue property, property-type:"
							<<(unsigned int)property << std::endl;
				buf += len;
				buflen -= len;
			};
			}
		}

	} break;
	default:
		throw eBadVersion();
	}
}





