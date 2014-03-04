/*
 * clldpattr.cc
 *
 *  Created on: 26.02.2014
 *      Author: andi
 */

#include "rofl/common/protocols/clldpattr.h"

using namespace rofl::protocol::lldp;

clldpattr::clldpattr(size_t len) :
		rofl::cmemory(len)
{
	lldp_generic = rofl::cmemory::somem();
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

	rofl::cmemory::operator= (attr);

	lldp_generic = rofl::cmemory::somem();

	return *this;
}


clldpattr::~clldpattr()
{

}


uint8_t*
clldpattr::resize(size_t len)
{
	return (lldp_generic = rofl::cmemory::resize(len));
}


size_t
clldpattr::length() const
{
	return rofl::cmemory::memlen();
}


void
clldpattr::pack(uint8_t *buf, size_t buflen)
{
	set_length(rofl::cmemory::memlen());

	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < rofl::cmemory::memlen()) {
		throw eLLDPInval();
	}

	rofl::cmemory::pack(buf, buflen);
}


void
clldpattr::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < sizeof(struct lldp_tlv_hdr_t)) {
		throw eLLDPInval();
	}

	rofl::cmemory::unpack(buf, buflen);

	lldp_generic = rofl::cmemory::somem();
}


uint8_t
clldpattr::get_type() const
{
	return ((be16toh(lldp_hdr->tlen) & 0xfe00) >> 9);
}



void
clldpattr::set_type(uint8_t type)
{
	lldp_hdr->tlen &= htobe16(0x01ff);
	lldp_hdr->tlen |= htobe16(((type & 0x7f) << 9));
}



uint16_t
clldpattr::get_length() const
{
	return ((be16toh(lldp_hdr->tlen) & 0x01ff));
}



void
clldpattr::set_length(uint16_t len)
{
	lldp_hdr->tlen &= htobe16(0xfe00);
	lldp_hdr->tlen |= htobe16((len & 0x01ff));
}



rofl::cmemory
clldpattr::get_body() const
{
	if (sizeof(struct lldp_tlv_hdr_t) >= rofl::cmemory::memlen()) {
		throw eLLDPNotFound();
	}

	return rofl::cmemory(lldp_hdr->body, rofl::cmemory::memlen() - sizeof(struct lldp_tlv_hdr_t));
}



void
clldpattr::set_body(rofl::cmemory const& body)
{
	if (rofl::cmemory::memlen() < (sizeof(struct lldp_tlv_hdr_t) + body.memlen())) {
		resize(sizeof(struct lldp_tlv_hdr_t) + body.memlen());
	}

	memcpy(lldp_hdr->body, body.somem(), body.memlen());
}






/*
 * chassis-id
 */
clldpattr_id::clldpattr_id(
			size_t len) :
					clldpattr(len)
{
	lldp_id_generic = rofl::cmemory::somem();
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

	lldp_id_generic = somem();

	return *this;
}


clldpattr_id::~clldpattr_id()
{

}


uint8_t*
clldpattr_id::resize(size_t len)
{
	return (lldp_id_generic = clldpattr::resize(len));
}


uint8_t
clldpattr_id::get_sub_type() const
{
	return lldp_id_hdr->subtype;
}


void
clldpattr_id::set_sub_type(uint8_t type)
{
	lldp_id_hdr->subtype = type;
}


rofl::cmemory
clldpattr_id::get_body() const
{
	if (sizeof(struct lldp_tlv_id_hdr_t) >= rofl::cmemory::memlen()) {
		throw eLLDPNotFound();
	}

	return rofl::cmemory(lldp_id_hdr->body, rofl::cmemory::memlen() - sizeof(struct lldp_tlv_id_hdr_t));
}


void
clldpattr_id::set_body(rofl::cmemory const& body)
{
	if (rofl::cmemory::memlen() < (sizeof(struct lldp_tlv_id_hdr_t) + body.memlen())) {
		resize(sizeof(struct lldp_tlv_id_hdr_t) + body.memlen());
	}

	memcpy(lldp_id_hdr->body, body.somem(), body.memlen());
}







/*
 * ttl
 */
clldpattr_ttl::clldpattr_ttl(
			size_t len) :
					clldpattr(len)
{
	lldp_ttl_generic = rofl::cmemory::somem();
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

	lldp_ttl_generic = somem();

	return *this;
}


clldpattr_ttl::~clldpattr_ttl()
{

}


uint8_t*
clldpattr_ttl::resize(size_t len)
{
	return (lldp_ttl_generic = clldpattr::resize(len));
}


uint16_t
clldpattr_ttl::get_ttl() const
{
	return be16toh(lldp_ttl_hdr->ttl);
}


void
clldpattr_ttl::set_ttl(uint16_t ttl)
{
	lldp_ttl_hdr->ttl = htobe16(ttl);
}








/*
 * desc
 */
clldpattr_desc::clldpattr_desc(
			size_t len) :
					clldpattr(len)
{
	lldp_desc_generic = rofl::cmemory::somem();
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

	lldp_desc_generic = somem();

	return *this;
}


clldpattr_desc::~clldpattr_desc()
{

}


uint8_t*
clldpattr_desc::resize(size_t len)
{
	return (lldp_desc_generic = clldpattr::resize(len));
}


std::string
clldpattr_desc::get_desc() const
{
	return std::string((const char*)lldp_desc_hdr->body, rofl::cmemory::memlen() - sizeof(struct lldp_tlv_hdr_t));
}


void
clldpattr_desc::set_desc(std::string const& desc)
{
	if (rofl::cmemory::memlen() < (sizeof(struct lldp_tlv_hdr_t) + desc.length())) {
		resize(sizeof(struct lldp_tlv_hdr_t) + desc.length());
	}

	memcpy(lldp_desc_hdr->body, desc.c_str(), desc.length());
}






/*
 * sys_caps
 */
clldpattr_sys_caps::clldpattr_sys_caps(
			size_t len) :
					clldpattr(len)
{
	lldp_sys_caps_generic = rofl::cmemory::somem();
}


clldpattr_sys_caps::clldpattr_sys_caps(
			clldpattr_sys_caps const& attr)
{
	*this = attr;
}


clldpattr_sys_caps&
clldpattr_sys_caps::operator= (clldpattr_sys_caps const& attr)
{
	if (this == &attr)
		return *this;

	rofl::protocol::lldp::clldpattr::operator= (attr);

	lldp_sys_caps_generic = somem();

	return *this;
}


clldpattr_sys_caps::~clldpattr_sys_caps()
{

}


uint8_t*
clldpattr_sys_caps::resize(size_t len)
{
	return (lldp_sys_caps_generic = clldpattr::resize(len));
}


uint8_t
clldpattr_sys_caps::get_chassis_id() const
{
	return lldp_sys_caps_hdr->chassis_id;
}


void
clldpattr_sys_caps::set_chassis_id(uint8_t chassis_id)
{
	lldp_sys_caps_hdr->chassis_id = chassis_id;
}


uint16_t
clldpattr_sys_caps::get_available_caps() const
{
	return be16toh(lldp_sys_caps_hdr->available_caps);
}


void
clldpattr_sys_caps::set_available_caps(uint16_t caps)
{
	lldp_sys_caps_hdr->available_caps = htobe16(caps);
}


uint16_t
clldpattr_sys_caps::get_enabled_caps() const
{
	return be16toh(lldp_sys_caps_hdr->enabled_caps);
}


void
clldpattr_sys_caps::set_enabled_caps(uint16_t caps)
{
	lldp_sys_caps_hdr->enabled_caps = htobe16(caps);
}


