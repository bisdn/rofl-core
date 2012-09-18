/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fetherframe.h"


fetherframe::fetherframe(
		uint8_t *data,
		size_t datalen) :
	fframe(data, datalen),
	eth_hdr(0)
{
	initialize();
}



fetherframe::fetherframe(
		size_t len) :
		fframe(len),
		eth_hdr(0)
{
	initialize();
}



fetherframe::~fetherframe()
{
	// do _NOT_ delete or deallocate (data,datalen) here!
}



void
fetherframe::initialize()
{
	eth_hdr = (struct eth_hdr_t*)soframe();
}



bool
fetherframe::complete()
{
	if (framelen() < sizeof(struct eth_hdr_t))
		return false;

	initialize();

	return true;
}



size_t
fetherframe::need_bytes()
{
	if (framelen() < sizeof(struct eth_hdr_t))
		return (sizeof(struct eth_hdr_t) - framelen());

	initialize();

	// TODO: use length field from Ethernet 802.3 header, for now just Ethernet II

	return 0;
}


void
fetherframe::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct eth_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(soframe() + sizeof(struct eth_hdr_t), data, datalen);
}


uint8_t*
fetherframe::payload() throw (eFrameNoPayload)
{
	if (framelen() <= sizeof(struct eth_hdr_t))
		throw eFrameNoPayload();

	initialize();

	return (soframe() + sizeof(struct eth_hdr_t));
}


size_t
fetherframe::payloadlen() throw (eFrameNoPayload)
{
	if (framelen() <= sizeof(struct eth_hdr_t))
		throw eFrameNoPayload();

	initialize();

	return (framelen() - sizeof(struct eth_hdr_t));
}



void
fetherframe::validate() throw (eFrameInvalidSyntax)
{
	initialize();

	if (framelen() < (sizeof(struct eth_hdr_t)))
		throw eFrameInvalidSyntax();

	// TODO: check on minimum length of 64 bytes?
}


void
fetherframe::set_dl_src(cmacaddr const& dl_src) throw (eFrameInval)
{
	if (dl_src.memlen() < OFP_ETH_ALEN)
	{
		throw eFrameInval();
	}

	initialize();

	if (!complete())
	{
		throw eFrameInval();
	}

	memcpy(eth_hdr->dl_src, dl_src.somem(), OFP_ETH_ALEN);
}


cmacaddr
fetherframe::get_dl_src() const
{
	cmacaddr ma(eth_hdr->dl_src, OFP_ETH_ALEN);
	return ma;
}


void
fetherframe::set_dl_dst(cmacaddr const& dl_dst) throw (eFrameInval)
{
	if (dl_dst.memlen() < OFP_ETH_ALEN)
	{
		throw eFrameInval();
	}

	initialize();

	if (!eth_hdr)
	{
		throw eFrameInval();
	}

	memcpy(eth_hdr->dl_dst, dl_dst.somem(), OFP_ETH_ALEN);
}


cmacaddr
fetherframe::get_dl_dst() const
{
	cmacaddr ma(eth_hdr->dl_dst, OFP_ETH_ALEN);
	return ma;
}


void
fetherframe::set_dl_type(uint16_t dl_type) throw (eFrameInval)
{
	initialize();

	if (!eth_hdr)
	{
		throw eFrameInval();
	}

	eth_hdr->dl_type = htobe16(dl_type);
}


uint16_t
fetherframe::get_dl_type() const
{
	return be16toh(eth_hdr->dl_type);
}




const char*
fetherframe::c_str()
{
	cvastring vas;

	info.assign(vas("[fetherframe(%p) dst:%02x:%02x:%02x:%02x:%02x:%02x "
			"src:%02x:%02x:%02x:%02x:%02x:%02x type:0x%04x ]",
			this,
			eth_hdr->dl_dst[0],
			eth_hdr->dl_dst[1],
			eth_hdr->dl_dst[2],
			eth_hdr->dl_dst[3],
			eth_hdr->dl_dst[4],
			eth_hdr->dl_dst[5],
			eth_hdr->dl_src[0],
			eth_hdr->dl_src[1],
			eth_hdr->dl_src[2],
			eth_hdr->dl_src[3],
			eth_hdr->dl_src[4],
			eth_hdr->dl_src[5],
			be16toh(eth_hdr->dl_type) ));

	return info.c_str();
}
