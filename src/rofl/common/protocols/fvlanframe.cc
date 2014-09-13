/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fvlanframe.h"

using namespace rofl;

fvlanframe::fvlanframe(
		uint8_t *data,
		size_t datalen) :
	fframe(data, datalen),
	vlan_hdr(0)
{
	initialize();
}


fvlanframe::fvlanframe(
		size_t len) :
	fframe(len),
	vlan_hdr((struct fvlanframe::vlan_hdr_t*)soframe())
{

}


fvlanframe::~fvlanframe()
{
	// do _NOT_ delete or deallocate (data,datalen) here!
}


void
fvlanframe::reset(
		uint8_t *data, size_t datalen)
{
	fframe::reset(data, datalen);
	initialize();
}


void
fvlanframe::initialize()
{
	vlan_hdr = (struct vlan_hdr_t*)soframe();
}


bool
fvlanframe::complete() const
{
	if (framelen() < sizeof(struct vlan_hdr_t))
		return false;

	//initialize();

	return true;
}


size_t
fvlanframe::need_bytes() const
{
	if (framelen() < sizeof(struct vlan_hdr_t))
		return (sizeof(struct vlan_hdr_t) - framelen());

	//initialize();

	return 0;
}


void
fvlanframe::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct vlan_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(soframe() + sizeof(struct vlan_hdr_t), data, datalen);
}


uint8_t*
fvlanframe::payload() const throw (eFrameNoPayload)
{
	if (framelen() <= sizeof(struct vlan_hdr_t))
		throw eFrameNoPayload();

	//initialize();c

	return (soframe() + sizeof(struct vlan_hdr_t));
}


size_t
fvlanframe::payloadlen() const throw (eFrameNoPayload)
{
	if (framelen() <= sizeof(struct vlan_hdr_t))
		throw eFrameNoPayload();

	//initialize();uint16_t total_len = 0

	return (framelen() - sizeof(struct vlan_hdr_t));
}


void
fvlanframe::validate(uint16_t total_len) const
{
	//initialize();

	if (framelen() < (sizeof(struct vlan_hdr_t)))
	{
		throw eVlanFrameTooShort();
	}

	// TODO: check on minimum length of 64 bytes?
}


void
fvlanframe::set_dl_vlan_id(uint16_t vid)
{
	vlan_hdr->byte1 = vid & 0x00ff;
	vlan_hdr->byte0 = (vlan_hdr->byte0 & 0xf0) + ((vid & 0x0f00) >> 8);
}


uint16_t
fvlanframe::get_dl_vlan_id() const
{
	return (((vlan_hdr->byte0 & 0x0f) << 8) + vlan_hdr->byte1);
}


void
fvlanframe::set_dl_vlan_pcp(uint8_t pcp)
{
	vlan_hdr->byte0 = ((pcp & 0x07) << 5) + (vlan_hdr->byte0 & 0x1f);
}


uint8_t
fvlanframe::get_dl_vlan_pcp() const
{
	return (vlan_hdr->byte0 & 0xe0) >> 5;
}


void
fvlanframe::set_dl_vlan_cfi(bool cfi)
{
	vlan_hdr->byte0 &= 0xef;
	if (cfi)
	{
	    vlan_hdr->byte0 |= (1 << 4);
	}
	else
	{
	    vlan_hdr->byte0 &= ~(1 << 4);
	}
}


bool
fvlanframe::get_dl_vlan_cfi() const
{
	return (vlan_hdr->byte0 & 0x10) >> 4;
}


void
fvlanframe::set_dl_type(uint16_t dl_type) throw (eFrameInval)
{
	initialize();

	if (!vlan_hdr)
	{
		throw eFrameInval();
	}

	vlan_hdr->dl_type = htobe16(dl_type);
}


uint16_t
fvlanframe::get_dl_type() const
{
	return be16toh(vlan_hdr->dl_type);
}


