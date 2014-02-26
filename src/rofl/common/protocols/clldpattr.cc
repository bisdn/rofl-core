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

	rofl::cmemory::operator =(attr);

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
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < rofl::cmemory::memlen()) {
		throw eLLDPInval();
	}

	set_length(rofl::cmemory::memlen());

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
	return ((lldp_hdr->tlen & 0xfe00) >> 9);
}



void
clldpattr::set_type(uint8_t type)
{
	lldp_hdr->tlen &= 0x01ff;
	lldp_hdr->tlen |= ((type & 0x7f) << 9);
}



uint16_t
clldpattr::get_length() const
{
	return ((lldp_hdr->tlen & 0x01ff));
}



void
clldpattr::set_length(uint16_t len)
{
	lldp_hdr->tlen &= 0xfe00;
	lldp_hdr->tlen |= (len & 0x01ff);
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


