/*
 * clldpattrs.cc
 *
 *  Created on: 03.03.2014
 *      Author: andreas
 */

#include "clldpattrs.h"

using namespace rofl::protocol::lldp;


clldpattrs::clldpattrs()
{

}


clldpattrs::~clldpattrs()
{

}


clldpattrs::clldpattrs(
		clldpattrs const& attrs)
{
	*this = attrs;
}


clldpattrs&
clldpattrs::operator= (
		clldpattrs const& attrs)
{
	if (this == &attrs)
		return *this;

	clear();

	for (std::map<uint8_t, clldpattr*>::const_iterator
			it = attrs.attrs.begin(); it != attrs.attrs.end(); ++it) {
		insert(*(it->second));
	}

	return *this;
}


size_t
clldpattrs::length() const
{
	size_t len = 0;
	for (std::map<uint8_t, clldpattr*>::const_iterator
			it = attrs.begin(); it != attrs.end(); ++it) {
		len += it->second->length();
	}
	return len;
}


void
clldpattrs::pack(uint8_t *buf, size_t buflen)
{

}


void
clldpattrs::unpack(uint8_t *buf, size_t buflen)
{

}


void
clldpattrs::clear()
{
	for (std::map<uint8_t, clldpattr*>::const_iterator
			it = attrs.begin(); it != attrs.end(); ++it) {
		delete it->second;
	}
	attrs.clear();
}


void
clldpattrs::insert(clldpattr const& attr)
{
	if (attrs.find(attr.get_type()) != attrs.end()) {
		delete attrs[attr.get_type()];
		attrs.erase(attr.get_type());
	}

	switch (attr.get_type()) {
	case LLDPTT_CHASSIS_ID: {
		attrs[attr.get_type()] = new clldpattr_chassis_id(attr);
	} break;
	case LLDPTT_PORT_ID: {
		attrs[attr.get_type()] = new clldpattr_port_id(attr);
	} break;
	case LLDPTT_TTL: {
		attrs[attr.get_type()] = new clldpattr_ttl(attr);
	} break;
	case LLDPTT_PORT_DESC: {
		attrs[attr.get_type()] = new clldpattr_port_desc(attr);
	} break;
	case LLDPTT_SYSTEM_NAME: {
		attrs[attr.get_type()] = new clldpattr_system_name(attr);
	} break;
	case LLDPTT_SYSTEM_DESC: {
		attrs[attr.get_type()] = new clldpattr_system_desc(attr);
	} break;
	case LLDPTT_SYSTEM_CAPS: {
		attrs[attr.get_type()] = new clldpattr_system_caps(attr);
	} break;
	case LLDPTT_MGMT_ADDR:
	case LLDPTT_END:
	default: {
		attrs[attr.get_type()] = new clldpattr(attr);
	};
	}
}



/*
 * Chassis-ID
 */
clldpattr_chassis_id&
clldpattrs::add_chassis_id()
{
	if (attrs.find(LLDPTT_CHASSIS_ID) != attrs.end()) {
		delete attrs[LLDPTT_CHASSIS_ID];
	}
	return *(dynamic_cast<clldpattr_chassis_id*>( attrs[LLDPTT_CHASSIS_ID] = new clldpattr_chassis_id() ));
}


clldpattr_chassis_id&
clldpattrs::set_chassis_id()
{
	if (attrs.find(LLDPTT_CHASSIS_ID) == attrs.end()) {
		attrs[LLDPTT_CHASSIS_ID] = new clldpattr_chassis_id();
	}
	return (dynamic_cast<clldpattr_chassis_id&>( *(attrs[LLDPTT_CHASSIS_ID])));
}


clldpattr_chassis_id&
clldpattrs::get_chassis_id()
{
	if (attrs.find(LLDPTT_CHASSIS_ID) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_chassis_id&>( *(attrs[LLDPTT_CHASSIS_ID])));
}


bool
clldpattrs::has_chassis_id()
{
	return (attrs.find(LLDPTT_CHASSIS_ID) != attrs.end());
}


void
clldpattrs::drop_chassis_id()
{
	if (attrs.find(LLDPTT_CHASSIS_ID) == attrs.end()) {
		return;
	}
	delete attrs[LLDPTT_CHASSIS_ID];
	attrs.erase(LLDPTT_CHASSIS_ID);
}



/*
 * Port-ID
 */
clldpattr_port_id&
clldpattrs::add_port_id()
{
	if (attrs.find(LLDPTT_PORT_ID) != attrs.end()) {
		delete attrs[LLDPTT_PORT_ID];
	}
	return *(dynamic_cast<clldpattr_port_id*>( attrs[LLDPTT_PORT_ID] = new clldpattr_port_id() ));
}


clldpattr_port_id&
clldpattrs::set_port_id()
{
	if (attrs.find(LLDPTT_PORT_ID) == attrs.end()) {
		attrs[LLDPTT_PORT_ID] = new clldpattr_port_id();
	}
	return (dynamic_cast<clldpattr_port_id&>( *(attrs[LLDPTT_PORT_ID])));
}


clldpattr_port_id&
clldpattrs::get_port_id()
{
	if (attrs.find(LLDPTT_PORT_ID) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_port_id&>( *(attrs[LLDPTT_PORT_ID])));
}


bool
clldpattrs::has_port_id()
{
	return (attrs.find(LLDPTT_PORT_ID) != attrs.end());
}


void
clldpattrs::drop_port_id()
{
	if (attrs.find(LLDPTT_PORT_ID) == attrs.end()) {
		return;
	}
	delete attrs[LLDPTT_PORT_ID];
	attrs.erase(LLDPTT_PORT_ID);
}



/*
 * TTL
 */
clldpattr_ttl&
clldpattrs::add_ttl()
{
	if (attrs.find(LLDPTT_TTL) != attrs.end()) {
		delete attrs[LLDPTT_TTL];
	}
	return *(dynamic_cast<clldpattr_ttl*>( attrs[LLDPTT_TTL] = new clldpattr_ttl() ));
}


clldpattr_ttl&
clldpattrs::set_ttl()
{
	if (attrs.find(LLDPTT_TTL) == attrs.end()) {
		attrs[LLDPTT_TTL] = new clldpattr_ttl();
	}
	return (dynamic_cast<clldpattr_ttl&>( *(attrs[LLDPTT_TTL])));
}


clldpattr_ttl&
clldpattrs::get_ttl()
{
	if (attrs.find(LLDPTT_TTL) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_ttl&>( *(attrs[LLDPTT_TTL])));
}


bool
clldpattrs::has_ttl()
{
	return (attrs.find(LLDPTT_TTL) != attrs.end());
}


void
clldpattrs::drop_ttl()
{
	if (attrs.find(LLDPTT_TTL) == attrs.end()) {
		return;
	}
	delete attrs[LLDPTT_TTL];
	attrs.erase(LLDPTT_TTL);
}




/*
 * PORT_DESC
 */
clldpattr_port_desc&
clldpattrs::add_port_desc()
{
	if (attrs.find(LLDPTT_PORT_DESC) != attrs.end()) {
		delete attrs[LLDPTT_PORT_DESC];
	}
	return *(dynamic_cast<clldpattr_port_desc*>( attrs[LLDPTT_PORT_DESC] = new clldpattr_port_desc() ));
}


clldpattr_port_desc&
clldpattrs::set_port_desc()
{
	if (attrs.find(LLDPTT_PORT_DESC) == attrs.end()) {
		attrs[LLDPTT_PORT_DESC] = new clldpattr_port_desc();
	}
	return (dynamic_cast<clldpattr_port_desc&>( *(attrs[LLDPTT_PORT_DESC])));
}


clldpattr_port_desc&
clldpattrs::get_port_desc()
{
	if (attrs.find(LLDPTT_PORT_DESC) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_port_desc&>( *(attrs[LLDPTT_PORT_DESC])));
}


bool
clldpattrs::has_port_desc()
{
	return (attrs.find(LLDPTT_PORT_DESC) != attrs.end());
}


void
clldpattrs::drop_port_desc()
{
	if (attrs.find(LLDPTT_PORT_DESC) == attrs.end()) {
		return;
	}
	delete attrs[LLDPTT_PORT_DESC];
	attrs.erase(LLDPTT_PORT_DESC);
}




/*
 * SYSTEM_DESC
 */
clldpattr_system_name&
clldpattrs::add_system_name()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) != attrs.end()) {
		delete attrs[LLDPTT_SYSTEM_DESC];
	}
	return *(dynamic_cast<clldpattr_system_name*>( attrs[LLDPTT_SYSTEM_DESC] = new clldpattr_system_name() ));
}


clldpattr_system_name&
clldpattrs::set_system_name()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) == attrs.end()) {
		attrs[LLDPTT_SYSTEM_DESC] = new clldpattr_system_name();
	}
	return (dynamic_cast<clldpattr_system_name&>( *(attrs[LLDPTT_SYSTEM_DESC])));
}


clldpattr_system_name&
clldpattrs::get_system_name()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_system_name&>( *(attrs[LLDPTT_SYSTEM_DESC])));
}


bool
clldpattrs::has_system_name()
{
	return (attrs.find(LLDPTT_SYSTEM_DESC) != attrs.end());
}


void
clldpattrs::drop_system_name()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) == attrs.end()) {
		return;
	}
	delete attrs[LLDPTT_SYSTEM_DESC];
	attrs.erase(LLDPTT_SYSTEM_DESC);
}




/*
 * SYSTEM_DESC
 */
clldpattr_system_desc&
clldpattrs::add_system_desc()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) != attrs.end()) {
		delete attrs[LLDPTT_SYSTEM_DESC];
	}
	return *(dynamic_cast<clldpattr_system_desc*>( attrs[LLDPTT_SYSTEM_DESC] = new clldpattr_system_desc() ));
}


clldpattr_system_desc&
clldpattrs::set_system_desc()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) == attrs.end()) {
		attrs[LLDPTT_SYSTEM_DESC] = new clldpattr_system_desc();
	}
	return (dynamic_cast<clldpattr_system_desc&>( *(attrs[LLDPTT_SYSTEM_DESC])));
}


clldpattr_system_desc&
clldpattrs::get_system_desc()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_system_desc&>( *(attrs[LLDPTT_SYSTEM_DESC])));
}


bool
clldpattrs::has_system_desc()
{
	return (attrs.find(LLDPTT_SYSTEM_DESC) != attrs.end());
}


void
clldpattrs::drop_system_desc()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) == attrs.end()) {
		return;
	}
	delete attrs[LLDPTT_SYSTEM_DESC];
	attrs.erase(LLDPTT_SYSTEM_DESC);
}



/*
 * SYSTEM_CAPS
 */
clldpattr_system_caps&
clldpattrs::add_system_caps()
{
	if (attrs.find(LLDPTT_SYSTEM_CAPS) != attrs.end()) {
		delete attrs[LLDPTT_SYSTEM_CAPS];
	}
	return *(dynamic_cast<clldpattr_system_caps*>( attrs[LLDPTT_SYSTEM_CAPS] = new clldpattr_system_caps() ));
}


clldpattr_system_caps&
clldpattrs::set_system_caps()
{
	if (attrs.find(LLDPTT_SYSTEM_CAPS) == attrs.end()) {
		attrs[LLDPTT_SYSTEM_CAPS] = new clldpattr_system_caps();
	}
	return (dynamic_cast<clldpattr_system_caps&>( *(attrs[LLDPTT_SYSTEM_CAPS])));
}


clldpattr_system_caps&
clldpattrs::get_system_caps()
{
	if (attrs.find(LLDPTT_SYSTEM_CAPS) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_system_caps&>( *(attrs[LLDPTT_SYSTEM_CAPS])));
}


bool
clldpattrs::has_system_caps()
{
	return (attrs.find(LLDPTT_SYSTEM_CAPS) != attrs.end());
}


void
clldpattrs::drop_system_caps()
{
	if (attrs.find(LLDPTT_SYSTEM_CAPS) == attrs.end()) {
		return;
	}
	delete attrs[LLDPTT_SYSTEM_CAPS];
	attrs.erase(LLDPTT_SYSTEM_CAPS);
}
