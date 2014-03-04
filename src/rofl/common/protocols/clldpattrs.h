/*
 * clldpattrs.h
 *
 *  Created on: 03.03.2014
 *      Author: andreas
 */

#ifndef CLLDPATTRS_H_
#define CLLDPATTRS_H_

#include <map>

#include "rofl/common/protocols/clldpattr.h"
#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"

namespace rofl {
namespace protocol {
namespace lldp {

class eLLDPAttrsBase		: public RoflException {};
class eLLDPAttrsInval		: public eLLDPAttrsBase {};
class eLLDPAttrsNotFound	: public eLLDPAttrsBase {};

class clldpattrs {

	std::map<uint8_t, clldpattr*> attrs;

public:

	/**
	 *
	 */
	clldpattrs();

	/**
	 *
	 */
	virtual
	~clldpattrs();

	/**
	 *
	 */
	clldpattrs(clldpattrs const& attrs);

	/**
	 *
	 */
	clldpattrs&
	operator= (clldpattrs const& attrs);

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	void
	insert(clldpattr const& attr);

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
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);

public:

	clldpattr_chassis_id&
	add_chassis_id();

	clldpattr_chassis_id&
	set_chassis_id();

	clldpattr_chassis_id&
	get_chassis_id();

	bool
	has_chassis_id();

	void
	drop_chassis_id();



	clldpattr_port_id&
	add_port_id();

	clldpattr_port_id&
	set_port_id();

	clldpattr_port_id&
	get_port_id();

	bool
	has_port_id();

	void
	drop_port_id();



	clldpattr_ttl&
	add_ttl();

	clldpattr_ttl&
	set_ttl();

	clldpattr_ttl&
	get_ttl();

	bool
	has_ttl();

	void
	drop_ttl();



	clldpattr_port_desc&
	add_port_desc();

	clldpattr_port_desc&
	set_port_desc();

	clldpattr_port_desc&
	get_port_desc();

	bool
	has_port_desc();

	void
	drop_port_desc();




	clldpattr_system_name&
	add_system_name();

	clldpattr_system_name&
	set_system_name();

	clldpattr_system_name&
	get_system_name();

	bool
	has_system_name();

	void
	drop_system_name();




	clldpattr_system_desc&
	add_system_desc();

	clldpattr_system_desc&
	set_system_desc();

	clldpattr_system_desc&
	get_system_desc();

	bool
	has_system_desc();

	void
	drop_system_desc();




	clldpattr_system_caps&
	add_system_caps();

	clldpattr_system_caps&
	set_system_caps();

	clldpattr_system_caps&
	get_system_caps();

	bool
	has_system_caps();

	void
	drop_system_caps();





public:

	friend std::ostream&
	operator<< (std::ostream& os, clldpattrs const& attrs) {

		return os;
	};
};

}; // end of namespace lldp
}; // end of namespace protocol
}; // end of namespace rofl




#endif /* CLLDPATTRS_H_ */
