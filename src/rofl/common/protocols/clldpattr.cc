/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * clldpattr.cc
 *
 *  Created on: 26.02.2014
 *      Author: andi
 */

#include "rofl/common/protocols/clldpattr.h"

using namespace rofl::protocol::lldp;

clldpattr::clldpattr(size_t bodylen) :
		hdr(sizeof(struct lldp_tlv_hdr_t)),
		body(bodylen)
{

}


clldpattr::clldpattr(clldpattr const& attr)
{
	*this = attr;
}


clldpattr&
clldpattr::operator= (clldpattr const& attr)
{
	if (this == &attr)
		return *this;

	hdr 	= attr.hdr;
	body 	= attr.body;

	return *this;
}


bool
clldpattr::operator== (clldpattr const& attr)
{
	return ((hdr == attr.hdr) && (body == attr.body));
}


clldpattr::~clldpattr()
{

}



size_t
clldpattr::length() const
{
	return (hdr.memlen() + body.memlen());
}


void
clldpattr::pack(uint8_t *buf, size_t buflen)
{
	set_length(body.memlen());

	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length()) {
		throw eLLDPInval();
	}

	hdr.pack(buf, hdr.memlen());

	body.pack(buf + hdr.memlen(), body.memlen());
}


void
clldpattr::unpack(uint8_t *buf, size_t buflen)
{
	body.clear();

	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < sizeof(struct lldp_tlv_hdr_t)) {
		throw eLLDPInval();
	}

	hdr.unpack(buf, sizeof(struct lldp_tlv_hdr_t));

	if (buflen > sizeof(struct lldp_tlv_hdr_t)) {
		body.assign(buf + sizeof(struct lldp_tlv_hdr_t), buflen - sizeof(struct lldp_tlv_hdr_t));
	}
}


uint8_t
clldpattr::get_type() const
{
	struct lldp_tlv_hdr_t *lldp_hdr = (struct lldp_tlv_hdr_t*)hdr.somem();
	return ((be16toh(lldp_hdr->tlen) & 0xfe00) >> 9);
}



void
clldpattr::set_type(uint8_t type)
{
	struct lldp_tlv_hdr_t *lldp_hdr = (struct lldp_tlv_hdr_t*)hdr.somem();
	lldp_hdr->tlen &= htobe16(0x01ff);
	lldp_hdr->tlen |= htobe16(((type & 0x7f) << 9));
}



uint16_t
clldpattr::get_length() const
{
	struct lldp_tlv_hdr_t *lldp_hdr = (struct lldp_tlv_hdr_t*)hdr.somem();
	return ((be16toh(lldp_hdr->tlen) & 0x01ff));
}



void
clldpattr::set_length(uint16_t len)
{
	struct lldp_tlv_hdr_t *lldp_hdr = (struct lldp_tlv_hdr_t*)hdr.somem();
	lldp_hdr->tlen &= htobe16(0xfe00);
	lldp_hdr->tlen |= htobe16((len & 0x01ff));
}






clldpattr_end::clldpattr_end()
{
	set_type(LLDPTT_END);
	set_length(0);
}


clldpattr_end::clldpattr_end(
		clldpattr_end const& attr)
{
	*this = attr;
}


clldpattr_end&
clldpattr_end::operator= (clldpattr_end const& attr)
{
	if (this == &attr)
		return *this;

	clldpattr::operator= (attr);

	return *this;
}


clldpattr_end::~clldpattr_end()
{

}






/*
 * chassis-id
 */
clldpattr_id::clldpattr_id(
		uint8_t type,
		uint8_t sub_type,
		size_t bodylen) :
					clldpattr(bodylen),
					sub_type(sub_type)
{
	set_type(type);
}


clldpattr_id::clldpattr_id(
			clldpattr_id const& attr)
{
	*this = attr;
}


clldpattr_id&
clldpattr_id::operator= (clldpattr_id const& attr)
{
	if (this == &attr)
		return *this;

	rofl::protocol::lldp::clldpattr::operator= (attr);

	sub_type = attr.sub_type;

	return *this;
}


clldpattr_id::~clldpattr_id()
{

}


size_t
clldpattr_id::length() const
{
	return (hdr.memlen() + sizeof(sub_type) + body.memlen());
}


void
clldpattr_id::pack(uint8_t *buf, size_t buflen)
{
	set_length(sizeof(sub_type) + body.memlen());

	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length()) {
		throw eLLDPInval();
	}

	hdr.pack(buf, sizeof(struct lldp_tlv_hdr_t));
	((struct lldp_tlv_id_hdr_t*)buf)->subtype = sub_type;
	body.pack(buf + sizeof(struct lldp_tlv_id_hdr_t), body.memlen());
}


void
clldpattr_id::unpack(uint8_t *buf, size_t buflen)
{
	body.clear();

	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < sizeof(struct lldp_tlv_id_hdr_t)) {
		throw eLLDPInval();
	}

	hdr.unpack(buf, sizeof(struct lldp_tlv_hdr_t));

	if (buflen >= sizeof(struct lldp_tlv_id_hdr_t)) {
		sub_type = ((struct lldp_tlv_id_hdr_t*)buf)->subtype;
	}

	if (buflen > sizeof(struct lldp_tlv_id_hdr_t)) {
		body.assign(buf + sizeof(struct lldp_tlv_id_hdr_t), buflen - sizeof(struct lldp_tlv_id_hdr_t));
	}
}


std::string
clldpattr_id::get_string() const
{
	return body.toString();
}


void
clldpattr_id::set_string(std::string const str)
{
	body.assign((uint8_t*)str.c_str(), str.length());
}





/*
 * ttl
 */
clldpattr_ttl::clldpattr_ttl(
			size_t len) :
					clldpattr(len), ttl(0)
{
	set_type(LLDPTT_TTL);
}


clldpattr_ttl::clldpattr_ttl(
			clldpattr_ttl const& attr)
{
	*this = attr;
}


clldpattr_ttl&
clldpattr_ttl::operator= (clldpattr_ttl const& attr)
{
	if (this == &attr)
		return *this;

	rofl::protocol::lldp::clldpattr::operator= (attr);

	ttl = attr.ttl;

	return *this;
}


clldpattr_ttl::~clldpattr_ttl()
{

}


size_t
clldpattr_ttl::length() const
{
	return (hdr.memlen() + sizeof(ttl));
}


void
clldpattr_ttl::pack(uint8_t *buf, size_t buflen)
{
	set_length(sizeof(ttl));

	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length()) {
		throw eLLDPInval();
	}

	hdr.pack(buf, sizeof(struct lldp_tlv_hdr_t));
	((struct lldp_tlv_ttl_hdr_t*)buf)->ttl = htobe16(ttl);
}


void
clldpattr_ttl::unpack(uint8_t *buf, size_t buflen)
{
	body.clear();

	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < sizeof(struct lldp_tlv_ttl_hdr_t)) {
		throw eLLDPInval();
	}

	hdr.unpack(buf, sizeof(struct lldp_tlv_hdr_t));

	if (buflen >= sizeof(struct lldp_tlv_id_hdr_t)) {
		ttl = be16toh(((struct lldp_tlv_ttl_hdr_t*)buf)->ttl);
	}
}








/*
 * desc
 */
clldpattr_desc::clldpattr_desc(
		uint8_t type,
		size_t len) :
					clldpattr(len)
{
	set_type(type);
}


clldpattr_desc::clldpattr_desc(
			clldpattr_desc const& attr)
{
	*this = attr;
}


clldpattr_desc&
clldpattr_desc::operator= (clldpattr_desc const& attr)
{
	if (this == &attr)
		return *this;

	rofl::protocol::lldp::clldpattr::operator= (attr);

	return *this;
}


clldpattr_desc::~clldpattr_desc()
{

}


std::string
clldpattr_desc::get_desc() const
{
	return body.toString();
}


void
clldpattr_desc::set_desc(std::string const& desc)
{
	body.assign((uint8_t*)desc.c_str(), desc.length());
}





/*
 * sys_caps
 */
clldpattr_system_caps::clldpattr_system_caps(
			size_t len) :
					clldpattr(len), chassis_id(0), available_caps(0), enabled_caps(0)
{
	set_type(LLDPTT_SYSTEM_CAPS);
}


clldpattr_system_caps::clldpattr_system_caps(
			clldpattr_system_caps const& attr)
{
	*this = attr;
}


clldpattr_system_caps&
clldpattr_system_caps::operator= (clldpattr_system_caps const& attr)
{
	if (this == &attr)
		return *this;

	rofl::protocol::lldp::clldpattr::operator= (attr);

	chassis_id 		= attr.chassis_id;
	available_caps 	= attr.available_caps;
	enabled_caps 	= attr.enabled_caps;

	return *this;
}


clldpattr_system_caps::~clldpattr_system_caps()
{

}




size_t
clldpattr_system_caps::length() const
{
	return (hdr.memlen() + sizeof(chassis_id) + sizeof(available_caps) + sizeof(enabled_caps));
}


void
clldpattr_system_caps::pack(uint8_t *buf, size_t buflen)
{
	set_length(sizeof(chassis_id) + sizeof(available_caps) + sizeof(enabled_caps));

	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length()) {
		throw eLLDPInval();
	}

	hdr.pack(buf, sizeof(struct lldp_tlv_hdr_t));
	((struct lldp_tlv_sys_caps_hdr_t*)buf)->chassis_id 		= chassis_id;
	((struct lldp_tlv_sys_caps_hdr_t*)buf)->available_caps 	= htobe16(available_caps);
	((struct lldp_tlv_sys_caps_hdr_t*)buf)->enabled_caps 	= htobe16(enabled_caps);
}


void
clldpattr_system_caps::unpack(uint8_t *buf, size_t buflen)
{
	body.clear();

	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < sizeof(struct lldp_tlv_sys_caps_hdr_t)) {
		throw eLLDPInval();
	}

	hdr.unpack(buf, sizeof(struct lldp_tlv_hdr_t));
	chassis_id 		= ((struct lldp_tlv_sys_caps_hdr_t*)buf)->chassis_id;
	available_caps 	= be16toh(((struct lldp_tlv_sys_caps_hdr_t*)buf)->available_caps);
	enabled_caps 	= be16toh(((struct lldp_tlv_sys_caps_hdr_t*)buf)->enabled_caps);
}
