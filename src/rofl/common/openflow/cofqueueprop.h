/*
 * cofqueueprop.h
 *
 *  Created on: 29.04.2013
 *      Author: andi
 */

#ifndef COFQUEUEPROP_H_
#define COFQUEUEPROP_H_ 1

#include <assert.h>
#include <ostream>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"

namespace rofl {
namespace openflow {

class eQueuePropBase 		: public RoflException {};
class eQueuePropNotFound 	: public eQueuePropBase {};


class cofqueue_prop {
public:

	/**
	 *
	 */
	cofqueue_prop(
			uint8_t of_version = openflow::OFP_VERSION_UNKNOWN,
			uint16_t property = 0) :
				ofp_version(of_version),
				property(property),
				len(sizeof(struct rofl::openflow12::ofp_queue_prop_header)) {};


	/**
	 *
	 */
	virtual
	~cofqueue_prop() {};


	/**
	 *
	 */
	cofqueue_prop(
			const cofqueue_prop& prop) {
		*this = prop;
	};


	/**
	 *
	 */
	cofqueue_prop&
	operator= (
			const cofqueue_prop& qp) {
		if (this == &qp)
			return *this;

		ofp_version = qp.ofp_version;
		property	= qp.property;
		len			= qp.len;

		return *this;
	};


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
	pack(
			uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);


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
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

	/**
	 *
	 */
	uint16_t
	get_property() const { return property; };


	/**
	 *
	 */
	void
	set_property(
			uint16_t property) { this->property = property; };


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

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofqueue_prop const& prop) {
		os << rofl::indent(0) << "<cofqueue_prop "
				<< "version: " << (int)prop.get_version() << " "
				<< std::hex
				<< "property: 0x" << (int)prop.get_property() << " "
				<< std::dec
				<< "length: " << (int)prop.get_length() << " "
				<< ">" << std::endl;
		return os;
	};


private:

	uint8_t 			ofp_version;
	uint16_t			property;
	mutable uint16_t 	len;
};



class cofqueue_prop_min_rate : public cofqueue_prop {
public:

	/**
	 *
	 */
	cofqueue_prop_min_rate(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t min_rate = OFPQ_MIN_RATE_UNCFG) :
				cofqueue_prop(ofp_version, rofl::openflow13::OFPQT_MIN_RATE),
				min_rate(min_rate) {};

	/**
	 *
	 */
	virtual
	~cofqueue_prop_min_rate() {};

	/**
	 *
	 */
	cofqueue_prop_min_rate(
			const cofqueue_prop_min_rate& prop) {
		*this = prop;
	};

	/**
	 *
	 */
	cofqueue_prop_min_rate&
	operator= (
			const cofqueue_prop_min_rate& prop) {
		if (this == &prop)
			return *this;
		cofqueue_prop::operator= (prop);
		min_rate	= prop.min_rate;
		return *this;
	};

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
	pack(
			uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	uint16_t
	get_min_rate() const { return min_rate; };


	/**
	 *
	 */
	void
	set_min_rate(uint16_t min_rate) { this->min_rate = min_rate; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofqueue_prop_min_rate& prop) {
		os << rofl::indent(0) << "<cofqueue_prop_min_rate version: " << (unsigned int)prop.get_version() << " >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofqueue_prop&>( prop ); }
		os << rofl::indent(4) << "<min-rate: 0x" << std::hex << (unsigned int)prop.get_min_rate() << std::dec << " >" << std::endl;
		return os;
	};

private:

	uint16_t min_rate;
};



class cofqueue_prop_max_rate : public cofqueue_prop {
public:

	/**
	 *
	 */
	cofqueue_prop_max_rate(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t max_rate = OFPQ_MAX_RATE_UNCFG) :
				cofqueue_prop(ofp_version, rofl::openflow13::OFPQT_MAX_RATE),
				max_rate(max_rate) {};

	/**
	 *
	 */
	virtual
	~cofqueue_prop_max_rate() {};

	/**
	 *
	 */
	cofqueue_prop_max_rate(
			const cofqueue_prop_max_rate& prop) {
		*this = prop;
	};

	/**
	 *
	 */
	cofqueue_prop_max_rate&
	operator= (
			const cofqueue_prop_max_rate& prop) {
		if (this == &prop)
			return *this;
		cofqueue_prop::operator= (prop);
		max_rate	= prop.max_rate;
		return *this;
	};

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
	pack(
			uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	uint16_t
	get_max_rate() const { return max_rate; };


	/**
	 *
	 */
	void
	set_max_rate(uint16_t max_rate) { this->max_rate = max_rate; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofqueue_prop_max_rate& prop) {
		os << rofl::indent(0) << "<cofqueue_prop_max_rate version: " << (unsigned int)prop.get_version() << " >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofqueue_prop&>( prop ); }
		os << rofl::indent(4) << "<max-rate: 0x" << std::hex << (unsigned int)prop.get_max_rate() << std::dec << " >" << std::endl;
		return os;
	};

private:

	uint16_t max_rate;
};



class cofqueue_prop_experimenter : public cofqueue_prop {
public:

	/**
	 *
	 */
	cofqueue_prop_experimenter(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t exp_id = 0,
			const rofl::cmemory& exp_body = rofl::cmemory((size_t)0)) :
				cofqueue_prop(ofp_version, rofl::openflow13::OFPQT_EXPERIMENTER),
				exp_id(exp_id),
				exp_body(exp_body) {};

	/**
	 *
	 */
	virtual
	~cofqueue_prop_experimenter() {};

	/**
	 *
	 */
	cofqueue_prop_experimenter(
			const cofqueue_prop_experimenter& prop) {
		*this = prop;
	};

	/**
	 *
	 */
	cofqueue_prop_experimenter&
	operator= (
			const cofqueue_prop_experimenter& prop) {
		if (this == &prop)
			return *this;
		cofqueue_prop::operator= (prop);
		exp_id		= prop.exp_id;
		exp_body 	= prop.exp_body;
		return *this;
	};

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
	pack(
			uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);


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
	const rofl::cmemory&
	get_exp_body() const { return exp_body; };

	/**
	 *
	 */
	rofl::cmemory&
	set_exp_body() { return exp_body; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofqueue_prop_experimenter& prop) {
		os << rofl::indent(0) << "<cofqueue_prop_experimenter version: " << (unsigned int)prop.get_version() << " >" << std::endl;
		{ rofl::indent i(2); os << dynamic_cast<const cofqueue_prop&>( prop ); }
		os << rofl::indent(4) << "<exp-id: 0x" << std::hex << (unsigned int)prop.get_exp_id() << " >" << std::dec << std::endl;
		{ rofl::indent i(4); os << prop.get_exp_body(); }
		return os;
	};

private:

	uint32_t 		exp_id;
	rofl::cmemory 	exp_body;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFQUEUEPROP_H_ */
