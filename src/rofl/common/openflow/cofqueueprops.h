/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofqueueprops.h
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */

#ifndef COFQUEUEPROPS_H_
#define COFQUEUEPROPS_H_ 1

#include <map>

#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofqueueprop.h"

namespace rofl {
namespace openflow {

class cofqueue_props {
public:

	/**
	 *
	 */
	cofqueue_props(
		uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofqueue_props();

	/**
	 *
	 */
	cofqueue_props(
		const cofqueue_props& props);

	/**
	 */
	cofqueue_props&
	operator= (
		const cofqueue_props& props);

public:

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) {
		this->ofp_version = ofp_version;
		for (std::map<uint16_t, cofqueue_prop*>::iterator
				it = properties.begin(); it != properties.end(); ++it) {
			it->second->set_version(ofp_version);
		}
	};

	/**
	 *
	 */
	const std::map<uint16_t, cofqueue_prop*>&
	get_properties() const { return properties; };

	/**
	 *
	 */
	std::map<uint16_t, cofqueue_prop*>&
	set_properties() { return properties; };

public:

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	cofqueue_prop_min_rate&
	add_queue_prop_min_rate();

	/**
	 *
	 */
	cofqueue_prop_min_rate&
	set_queue_prop_min_rate();

	/**
	 *
	 */
	const cofqueue_prop_min_rate&
	get_queue_prop_min_rate() const;

	/**
	 *
	 */
	void
	drop_queue_prop_min_rate();

	/**
	 *
	 */
	bool
	has_queue_prop_min_rate() const;

	/**
	 *
	 */
	cofqueue_prop_max_rate&
	add_queue_prop_max_rate();

	/**
	 *
	 */
	cofqueue_prop_max_rate&
	set_queue_prop_max_rate();

	/**
	 *
	 */
	const cofqueue_prop_max_rate&
	get_queue_prop_max_rate() const;

	/**
	 *
	 */
	void
	drop_queue_prop_max_rate();

	/**
	 *
	 */
	bool
	has_queue_prop_max_rate() const;

	/**
	 *
	 */
	cofqueue_prop_experimenter&
	add_queue_prop_experimenter();

	/**
	 *
	 */
	cofqueue_prop_experimenter&
	set_queue_prop_experimenter();

	/**
	 *
	 */
	const cofqueue_prop_experimenter&
	get_queue_prop_experimenter() const;

	/**
	 *
	 */
	void
	drop_queue_prop_experimenter();

	/**
	 *
	 */
	bool
	has_queue_prop_experimenter() const;

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
	unpack(
		uint8_t *buf, size_t buflen);



	/**
	 *
	 */
	virtual void
	pack(
		uint8_t *buf,
		size_t buflen);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofqueue_props const& props) {
		os << rofl::indent(0) << "<cofqueue_props version: " << (unsigned int)props.get_version()
				<< " #props: " << props.get_properties().size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint16_t, cofqueue_prop*>::const_iterator
				it = props.properties.begin(); it != props.properties.end(); ++it) {
			switch (it->second->get_property()) {
			case rofl::openflow13::OFPQT_MIN_RATE: {
				os << dynamic_cast<const cofqueue_prop_min_rate&>( *(it->second) );
			} break;
			case rofl::openflow13::OFPQT_MAX_RATE: {
				os << dynamic_cast<const cofqueue_prop_max_rate&>( *(it->second) );
			} break;
			case rofl::openflow13::OFPQT_EXPERIMENTER: {
				os << dynamic_cast<const cofqueue_prop_experimenter&>( *(it->second) );
			} break;
			default: {
				os << (*(it->second));
			}
			}
		}
		return os;
	};

private:

	uint8_t 								ofp_version;
	std::map<uint16_t, cofqueue_prop*>		properties;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFQUEUEPROPLIST_H_ */
