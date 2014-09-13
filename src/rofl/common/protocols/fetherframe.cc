/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fetherframe.h"

using namespace rofl;

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
fetherframe::reset(
		uint8_t *data, size_t datalen)
{
	fframe::reset(data, datalen);
	initialize();
}


void
fetherframe::initialize()
{
	eth_hdr = (struct eth_hdr_t*)soframe();
	eth_llc_hdr = (struct eth_llc_hdr_t*)soframe();
}

bool
fetherframe::is_llc_frame() const
{
	return ( be16toh(eth_hdr->dl_type) < 0x600 );
}

bool
fetherframe::complete() const
{
	// TODO: case of Ethernet 802.3 header not handled cause the function is not used
	if (framelen() < sizeof(struct eth_hdr_t))
		return false;

	//initialize();

	return true;
}



size_t
fetherframe::need_bytes() const
{
	// TODO: case of Ethernet 802.3 header not handled cause the function is not used
	if (framelen() < sizeof(struct eth_hdr_t))
		return (sizeof(struct eth_hdr_t) - framelen());

	//initialize();

	return 0;
}


void
fetherframe::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if( is_llc_frame() ){
			if (datalen > (framelen() - sizeof(struct eth_llc_hdr_t)))
			{
					throw eFrameOutOfRange();
			}
			memcpy(soframe() + sizeof(struct eth_llc_hdr_t), data, datalen);
	}else{
			if (datalen > (framelen() - sizeof(struct eth_hdr_t)))
			{
					throw eFrameOutOfRange();
			}
			memcpy(soframe() + sizeof(struct eth_hdr_t), data, datalen);
	}
}


uint8_t*
fetherframe::payload() const throw (eFrameNoPayload)
{
	if( is_llc_frame() ){
		if (framelen() <= sizeof(struct eth_llc_hdr_t))
			throw eFrameNoPayload();

		return (soframe() + sizeof(struct eth_llc_hdr_t));
	}else{
		if (framelen() <= sizeof(struct eth_hdr_t))
			throw eFrameNoPayload();

		//initialize(); // commented out 2012-12-13

		return (soframe() + sizeof(struct eth_hdr_t));
	}
}


size_t
fetherframe::payloadlen() const throw (eFrameNoPayload)
{
	if( is_llc_frame() ){
		if (framelen() <= sizeof(struct eth_llc_hdr_t))
			throw eFrameNoPayload();

		return (framelen() - sizeof(struct eth_llc_hdr_t));
	}else{
		if (framelen() <= sizeof(struct eth_hdr_t))
			throw eFrameNoPayload();

		//initialize(); // commented out 2012-12-13

		return (framelen() - sizeof(struct eth_hdr_t));
	}
}



void
fetherframe::validate(uint16_t total_len) const
{
	//initialize();

	if( is_llc_frame() ){
		if (framelen() < (sizeof(struct eth_llc_hdr_t)))
			throw eFrameInvalidSyntax();
	}else{
		if (framelen() < (sizeof(struct eth_hdr_t)))
			throw eFrameInvalidSyntax();
	}

	// TODO: check on minimum length of 64 bytes?
}


void
fetherframe::set_dl_src(cmacaddr const& dl_src) throw (eFrameInval)
{
	if (dl_src.memlen() < ETH_ADDR_LEN)
	{
		throw eFrameInval();
	}

	initialize();

	if (!complete())
	{
		throw eFrameInval();
	}

	memcpy(eth_hdr->dl_src, dl_src.somem(), ETH_ADDR_LEN);
}


cmacaddr
fetherframe::get_dl_src() const
{
	cmacaddr ma(eth_hdr->dl_src, ETH_ADDR_LEN);
	return ma;
}


void
fetherframe::set_dl_dst(cmacaddr const& dl_dst) throw (eFrameInval)
{
	if (dl_dst.memlen() < ETH_ADDR_LEN)
	{
		throw eFrameInval();
	}

	initialize();

	if (!eth_hdr)
	{
		throw eFrameInval();
	}

	memcpy(eth_hdr->dl_dst, dl_dst.somem(), ETH_ADDR_LEN);
}


cmacaddr
fetherframe::get_dl_dst() const
{
	cmacaddr ma(eth_hdr->dl_dst, ETH_ADDR_LEN);
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
	
	if( is_llc_frame() ){
		eth_llc_hdr->dl_type = htobe16(dl_type);
	}else{
		eth_hdr->dl_type = htobe16(dl_type);
	}
}


uint16_t
fetherframe::get_dl_type() const
{
	if( is_llc_frame() ){
		return be16toh(eth_llc_hdr->dl_type);
	}{
		return be16toh(eth_hdr->dl_type);
	}
}

