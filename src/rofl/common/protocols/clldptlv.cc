/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "clldptlv.h"

clldptlv::clldptlv(size_t tlvlen) :
	tlvmem(tlvlen),
	tlv_hdr((struct lldp_tlv_hdr_t*)tlvmem.somem())
{
	lldp_tlv = tlv_hdr;
}


clldptlv::clldptlv(
		uint8_t* tlv,
		size_t tlvlen) :
	tlvmem(tlv, tlvlen),
	tlv_hdr((struct lldp_tlv_hdr_t*)tlvmem.somem())
{
	lldp_tlv = tlv_hdr;
}


clldptlv::~clldptlv()
{

}


clldptlv&
clldptlv::operator= (clldptlv const& tlv)
{
	if (this == &tlv)
		return *this;

	tlvmem = tlv.tlvmem;
	tlv_hdr = lldp_tlv = (struct lldp_tlv_hdr_t*)tlvmem.somem();

	return *this;
}


void
clldptlv::set_hdr_type(uint8_t type)
{
	uint16_t hdr = be16toh(tlv_hdr->hdr);
	hdr = (0x01ff & hdr) | (type << 9);
	tlv_hdr->hdr = htobe16(hdr);
}


uint8_t
clldptlv::get_hdr_type() const
{
	uint16_t hdr = be16toh(tlv_hdr->hdr);
	return ((hdr & 0xfe00) >> 9);
}


void
clldptlv::set_hdr_length(uint16_t len)
{
	uint16_t hdr = be16toh(tlv_hdr->hdr);
	hdr = (0xfe00 & hdr) | (len);
	tlv_hdr->hdr = htobe16(hdr);
}


uint16_t
clldptlv::get_hdr_length() const
{
	uint16_t hdr = be16toh(tlv_hdr->hdr);
	return (hdr & 0x01ff);
}


size_t
clldptlv::length() const
{
	return (sizeof(struct lldp_tlv_hdr_t) + get_hdr_length());
}


struct clldptlv::lldp_tlv_hdr_t*
clldptlv::pack(
		struct lldp_tlv_hdr_t *tlv,
		size_t tlvlen)
throw (eLLDPInval)
{
	if (tlvlen < this->length())
	{
		throw eLLDPInval();
	}

	size_t len = (tlvmem.memlen() < length()) ? tlvmem.memlen() : length();

	memcpy((uint8_t*)tlv, (uint8_t*)tlvmem.somem(), len);

	return tlv;
}


struct clldptlv::lldp_tlv_hdr_t*
clldptlv::unpack(
		struct lldp_tlv_hdr_t *tlv,
		size_t tlvlen)
throw (eLLDPInval)
{
	if (tlvlen < sizeof(struct lldp_tlv_hdr_t))
	{
		throw eLLDPInval();
	}

	tlvmem.resize(tlvlen);

	memcpy(tlvmem.somem(), (uint8_t*)tlv, tlvlen);

	tlv_hdr = lldp_tlv = (struct lldp_tlv_hdr_t*)tlvmem.somem();

	return tlv;
}


const char*
clldptlv::c_str()
{
	cvastring vas;
	info.assign(vas("clldptlv(%p)", this));
	return info.c_str();
}

