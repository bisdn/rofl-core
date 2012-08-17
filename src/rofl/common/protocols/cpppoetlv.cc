/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cpppoetlv.h"

cpppoetlv::cpppoetlv(size_t tlvlen) :
	tlvmem(tlvlen),
	tlv_hdr((struct pppoe_tag_hdr_t*)tlvmem.somem())
{

}



cpppoetlv::cpppoetlv
(uint8_t *tlv, size_t tlvlen) :
	tlvmem(tlv, tlvlen),
	tlv_hdr((struct pppoe_tag_hdr_t*)tlvmem.somem())
{

}



cpppoetlv::cpppoetlv(
		uint16_t type,
		const std::string & tag_value) throw (ePPPoEInval):
	tlvmem(sizeof(struct pppoe_tag_hdr_t) + tag_value.length()),
	tlv_hdr((struct pppoe_tag_hdr_t*)tlvmem.somem())
{
	if (tag_value.length() > std::numeric_limits<uint16_t>::max())
	{
		throw ePPPoEInval();
	}
	size_t len = tag_value.length();

	set_hdr_type(type);
	set_hdr_length(len);

	memcpy(tlv_hdr->data, tag_value.c_str(), len);
}



cpppoetlv::cpppoetlv(
		uint16_t type,
		cmemory const& tag_value) throw (ePPPoEInval):
	tlvmem(sizeof(struct pppoe_tag_hdr_t) + tag_value.memlen()),
	tlv_hdr((struct pppoe_tag_hdr_t*)tlvmem.somem())
{
	if (tag_value.memlen() > std::numeric_limits<uint16_t>::max())
	{
		throw ePPPoEInval();
	}
	size_t len = tag_value.memlen();

	set_hdr_type(type);
	set_hdr_length(len);

	memcpy(tlv_hdr->data, tag_value.somem(), len);
}



cpppoetlv::cpppoetlv(
		const cpppoetlv & tlv)
{
	*this = tlv;
}



cpppoetlv::~cpppoetlv()
{
}



cpppoetlv&
cpppoetlv::operator =(const cpppoetlv& tlv)
{
	if (this == &tlv)
		return *this;

	tlvmem = tlv.tlvmem;
	tlv_hdr = (struct pppoe_tag_hdr_t*)tlvmem.somem();

	return *this;
}



size_t
cpppoetlv::length()
{
	return (sizeof(struct pppoe_tag_hdr_t) + get_hdr_length());
}



struct cpppoetlv::pppoe_tag_hdr_t*
cpppoetlv::pack(struct pppoe_tag_hdr_t *tlv, size_t tlvlen) throw (ePPPoEInval)
{
	if (tlvlen < this->length())
	{
		throw ePPPoEInval();
	}

	size_t len = (tlvmem.memlen() < length()) ? tlvmem.memlen() : length();

	memcpy((uint8_t*)tlv, (uint8_t*)tlvmem.somem(), len);

	return tlv;
}



struct cpppoetlv::pppoe_tag_hdr_t*
cpppoetlv::unpack(struct pppoe_tag_hdr_t *tlv, size_t tlvlen) throw (ePPPoEInval)
{
	if (tlvlen < sizeof(struct pppoe_tag_hdr_t))
	{
		throw ePPPoEInval();
	}

	tlvmem.resize(tlvlen);

	memcpy(tlvmem.somem(), (uint8_t*)tlv, tlvlen);

	tlv_hdr = (struct pppoe_tag_hdr_t*)tlvmem.somem();

	return tlv;
}



const char*
cpppoetlv::c_str()
{
	cvastring vas;
	info.assign(vas("cpppoetlv(%p) type:%d length:%d mem:%s",
			this,
			get_hdr_type(),
			get_hdr_length(),
			tlvmem.c_str()));
	return info.c_str();
}



void
cpppoetlv::set_hdr_type(uint16_t type)
{
	tlv_hdr->type = htobe16(type);
}



uint16_t
cpppoetlv::get_hdr_type() const
{
	return be16toh(tlv_hdr->type);
}



void
cpppoetlv::set_hdr_length(uint16_t len)
{
	tlv_hdr->length = htobe16(len);
}



uint16_t
cpppoetlv::get_hdr_length() const
{
	return be16toh(tlv_hdr->length);
}


