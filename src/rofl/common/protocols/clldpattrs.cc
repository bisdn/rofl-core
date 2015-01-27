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
	clear();
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
	try {
		// add an End-TLV, if none is present
		if (not has_end()) {
			add_end();
		}

		if (buflen < length()) {
			throw eLLDPAttrsInval();
		}

		// chassis id must be present
		get_chassis_id().pack(buf, get_chassis_id().length());
		buf += get_chassis_id().length();

		// port id must be present
		get_port_id().pack(buf, get_port_id().length());
		buf += get_port_id().length();

		// ttl must be present
		get_ttl().pack(buf, get_ttl().length());
		buf += get_ttl().length();

		for (std::map<uint8_t, clldpattr*>::iterator
				it = attrs.begin(); it != attrs.end(); ++it) {
			switch (it->first) {
			case LLDPTT_CHASSIS_ID:
			case LLDPTT_PORT_ID:
			case LLDPTT_TTL:
			case LLDPTT_END:
				continue; // already done (or will be dealt with, see below)
			default: {
				it->second->pack(buf, it->second->length());
				buf += it->second->length();
			};
			}
		}

		get_end().pack(buf, get_end().length());
		buf += get_end().length();

	} catch (eLLDPAttrsNotFound& e) {

		LOGGING_ERROR << "clldpattrs::pack => mandatory TLV not found" << std::endl;
	}
}


void
clldpattrs::unpack(uint8_t *buf, size_t buflen)
{
	clear();

	while (buflen >= sizeof(struct lldp_tlv_hdr_t)) {
		struct lldp_tlv_hdr_t* tlv_hdr = (struct lldp_tlv_hdr_t*)buf;
		size_t len = be16toh(tlv_hdr->tlen) & 0x1ff;
		if ((len + sizeof(struct lldp_tlv_hdr_t)) > buflen) {
			throw eLLDPAttrsInval();
		}

		clldpattr attr;
		attr.unpack(buf, len + sizeof(struct lldp_tlv_hdr_t));
		insert(attr);

		buf += attr.length();
		buflen -= attr.length();
	}
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
	case LLDPTT_END: {
		attrs[attr.get_type()] = new clldpattr_end(attr);
	} break;
	case LLDPTT_CHASSIS_ID:
	case LLDPTT_PORT_ID: {
		attrs[attr.get_type()] = new clldpattr_id(attr);
	} break;
	case LLDPTT_TTL: {
		attrs[attr.get_type()] = new clldpattr_ttl(attr);
	} break;
	case LLDPTT_PORT_DESC:
	case LLDPTT_SYSTEM_NAME:
	case LLDPTT_SYSTEM_DESC: {
		attrs[attr.get_type()] = new clldpattr_desc(attr);
	} break;
	case LLDPTT_SYSTEM_CAPS: {
		attrs[attr.get_type()] = new clldpattr_system_caps(attr);
	} break;
	case LLDPTT_MGMT_ADDR:
	default: {
		attrs[attr.get_type()] = new clldpattr(attr);
	};
	}
}




/*
 * End
 */
clldpattr_end&
clldpattrs::add_end()
{
	if (attrs.find(LLDPTT_END) != attrs.end()) {
		delete attrs[LLDPTT_END];
	}
	return *(dynamic_cast<clldpattr_end*>( attrs[LLDPTT_END] = new clldpattr_end() ));
}


clldpattr_end&
clldpattrs::set_end()
{
	if (attrs.find(LLDPTT_END) == attrs.end()) {
		attrs[LLDPTT_END] = new clldpattr_end();
	}
	return (dynamic_cast<clldpattr_end&>( *(attrs[LLDPTT_END])));
}


clldpattr_end&
clldpattrs::get_end()
{
	if (attrs.find(LLDPTT_END) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_end&>( *(attrs[LLDPTT_END])));
}


bool
clldpattrs::has_end() const
{
	return (attrs.find(LLDPTT_END) != attrs.end());
}


void
clldpattrs::drop_end()
{
	if (attrs.find(LLDPTT_END) == attrs.end()) {
		return;
	}
	delete attrs[LLDPTT_END];
	attrs.erase(LLDPTT_END);
}





/*
 * Chassis-ID
 */
clldpattr_id&
clldpattrs::add_chassis_id()
{
	if (attrs.find(LLDPTT_CHASSIS_ID) != attrs.end()) {
		delete attrs[LLDPTT_CHASSIS_ID];
	}
	return *(dynamic_cast<clldpattr_id*>( attrs[LLDPTT_CHASSIS_ID] = new clldpattr_id(LLDPTT_CHASSIS_ID) ));
}


clldpattr_id&
clldpattrs::set_chassis_id()
{
	if (attrs.find(LLDPTT_CHASSIS_ID) == attrs.end()) {
		attrs[LLDPTT_CHASSIS_ID] = new clldpattr_id(LLDPTT_CHASSIS_ID);
	}
	return (dynamic_cast<clldpattr_id&>( *(attrs[LLDPTT_CHASSIS_ID])));
}


clldpattr_id&
clldpattrs::get_chassis_id()
{
	if (attrs.find(LLDPTT_CHASSIS_ID) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_id&>( *(attrs[LLDPTT_CHASSIS_ID])));
}


bool
clldpattrs::has_chassis_id() const
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
clldpattr_id&
clldpattrs::add_port_id()
{
	if (attrs.find(LLDPTT_PORT_ID) != attrs.end()) {
		delete attrs[LLDPTT_PORT_ID];
	}
	return *(dynamic_cast<clldpattr_id*>( attrs[LLDPTT_PORT_ID] = new clldpattr_id(LLDPTT_PORT_ID) ));
}


clldpattr_id&
clldpattrs::set_port_id()
{
	if (attrs.find(LLDPTT_PORT_ID) == attrs.end()) {
		attrs[LLDPTT_PORT_ID] = new clldpattr_id(LLDPTT_PORT_ID);
	}
	return (dynamic_cast<clldpattr_id&>( *(attrs[LLDPTT_PORT_ID])));
}


clldpattr_id&
clldpattrs::get_port_id()
{
	if (attrs.find(LLDPTT_PORT_ID) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_id&>( *(attrs[LLDPTT_PORT_ID])));
}


bool
clldpattrs::has_port_id() const
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
clldpattrs::has_ttl() const
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
clldpattr_desc&
clldpattrs::add_port_desc()
{
	if (attrs.find(LLDPTT_PORT_DESC) != attrs.end()) {
		delete attrs[LLDPTT_PORT_DESC];
	}
	return *(dynamic_cast<clldpattr_desc*>( attrs[LLDPTT_PORT_DESC] = new clldpattr_desc(LLDPTT_PORT_DESC) ));
}


clldpattr_desc&
clldpattrs::set_port_desc()
{
	if (attrs.find(LLDPTT_PORT_DESC) == attrs.end()) {
		attrs[LLDPTT_PORT_DESC] = new clldpattr_desc(LLDPTT_PORT_DESC);
	}
	return (dynamic_cast<clldpattr_desc&>( *(attrs[LLDPTT_PORT_DESC])));
}


clldpattr_desc&
clldpattrs::get_port_desc()
{
	if (attrs.find(LLDPTT_PORT_DESC) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_desc&>( *(attrs[LLDPTT_PORT_DESC])));
}


bool
clldpattrs::has_port_desc() const
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
 * SYSTEM_NAME
 */
clldpattr_desc&
clldpattrs::add_system_name()
{
	if (attrs.find(LLDPTT_SYSTEM_NAME) != attrs.end()) {
		delete attrs[LLDPTT_SYSTEM_NAME];
	}
	return *(dynamic_cast<clldpattr_desc*>( attrs[LLDPTT_SYSTEM_NAME] = new clldpattr_desc(LLDPTT_SYSTEM_NAME) ));
}


clldpattr_desc&
clldpattrs::set_system_name()
{
	if (attrs.find(LLDPTT_SYSTEM_NAME) == attrs.end()) {
		attrs[LLDPTT_SYSTEM_NAME] = new clldpattr_desc(LLDPTT_SYSTEM_NAME);
	}
	return (dynamic_cast<clldpattr_desc&>( *(attrs[LLDPTT_SYSTEM_DESC])));
}


clldpattr_desc&
clldpattrs::get_system_name()
{
	if (attrs.find(LLDPTT_SYSTEM_NAME) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_desc&>( *(attrs[LLDPTT_SYSTEM_NAME])));
}


bool
clldpattrs::has_system_name() const
{
	return (attrs.find(LLDPTT_SYSTEM_NAME) != attrs.end());
}


void
clldpattrs::drop_system_name()
{
	if (attrs.find(LLDPTT_SYSTEM_NAME) == attrs.end()) {
		return;
	}
	delete attrs[LLDPTT_SYSTEM_NAME];
	attrs.erase(LLDPTT_SYSTEM_NAME);
}




/*
 * SYSTEM_DESC
 */
clldpattr_desc&
clldpattrs::add_system_desc()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) != attrs.end()) {
		delete attrs[LLDPTT_SYSTEM_DESC];
	}
	return *(dynamic_cast<clldpattr_desc*>( attrs[LLDPTT_SYSTEM_DESC] = new clldpattr_desc(LLDPTT_SYSTEM_DESC) ));
}


clldpattr_desc&
clldpattrs::set_system_desc()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) == attrs.end()) {
		attrs[LLDPTT_SYSTEM_DESC] = new clldpattr_desc(LLDPTT_SYSTEM_DESC);
	}
	return (dynamic_cast<clldpattr_desc&>( *(attrs[LLDPTT_SYSTEM_DESC])));
}


clldpattr_desc&
clldpattrs::get_system_desc()
{
	if (attrs.find(LLDPTT_SYSTEM_DESC) == attrs.end()) {
		throw eLLDPAttrsNotFound();
	}
	return (dynamic_cast<clldpattr_desc&>( *(attrs[LLDPTT_SYSTEM_DESC])));
}


bool
clldpattrs::has_system_desc() const
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
clldpattrs::has_system_caps() const
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


