/*
 * cofrole.h
 *
 *  Created on: 18.03.2014
 *      Author: andreas
 */

#ifndef COFROLE_H_
#define COFROLE_H_

#include <inttypes.h>

#include <iostream>

#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl {
namespace openflow {

class cofrole
{
	uint8_t				ofp_version;

	uint32_t			role;
	uint64_t			generation_id;

public:

	struct role_t {
		uint32_t role;
		uint8_t  pad[4];
		uint64_t generation_id;
	};

public:

	/**
	 *
	 */
	cofrole(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofrole();

	/**
	 *
	 */
	cofrole(
			cofrole const& role);

	/**
	 *
	 */
	cofrole&
	operator= (
			cofrole const& role);

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
	uint32_t const&
	get_role() const { return role; };

	/**
	 *
	 */
	void
	set_role(uint32_t role) { this->role = role; };

	/**
	 *
	 */
	uint32_t&
	set_role() { return role; };

	/**
	 *
	 */
	uint64_t const&
	get_generation_id() const { return generation_id; };

	/**
	 *
	 */
	void
	set_generation_id(uint64_t generation_id) { this->generation_id = generation_id; };

	/**
	 *
	 */
	uint64_t&
	set_generation_id() { return generation_id; };

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofrole const& role) {
		os << rofl::indent(0) << "<cofrole ofp-version:" << (int)role.get_version() << " >" << std::endl;
		os << std::hex;
		os << rofl::indent(2) << "<role: 0x" << (unsigned int)role.get_role() << " >" << std::endl;
		os << rofl::indent(2) << "<generation-id: 0x" << (unsigned long long)role.get_generation_id() << " >" << std::endl;
		os << std::dec;
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFROLE_H_ */
