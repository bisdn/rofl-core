/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fieee80211frame.h"

using namespace rofl;

fieee80211frame::fieee80211frame(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		ieee80211data(0),
		ieee80211datalen(0)
{
	initialize();
}



fieee80211frame::~fieee80211frame()
{

}


void
fieee80211frame::initialize()
{
	ieee80211data = NULL;
	ieee80211datalen = 0;
	ieee80211_hdr = soframe();

	if (framelen() < IEEE80211_HDR_LEN)
		return;

	if (get_ieee80211_hlen() > framelen())
		return;

	ieee80211data = ((uint8_t*)soframe()) + get_ieee80211_hlen();
	ieee80211datalen = framelen() - get_ieee80211_hlen();
}


bool
fieee80211frame::complete()
{
	return true;
}


size_t
fieee80211frame::need_bytes()
{
	return 0; // just to make gcc happy
}

#if 0
void
fieee80211frame::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct ieee80211_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(ieee80211data, data, datalen);
	ieee80211datalen = datalen;
}
#endif


uint8_t*
fieee80211frame::payload() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!ieee80211data)
		throw eFrameNoPayload();
	return ieee80211data;
}


size_t
fieee80211frame::payloadlen() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!ieee80211datalen)
		throw eFrameNoPayload();
	return ieee80211datalen;
}


void
fieee80211frame::validate(uint16_t total_len) throw (eFrameInvalidSyntax)
{
	if (!complete())
		throw eFrameInvalidSyntax();
}


const char*
fieee80211frame::c_str()
{
	cvastring vas;
#if 0
	uint8_t ihlvers;        // IP header length in 32bit words,
							// TODO: check for options and padding
	uint8_t tos;
	uint16_t length;
	uint16_t ident;
	uint16_t offset_flags;
	uint8_t ttl;
	uint8_t proto;
	uint16_t checksum;
	uint32_t src;
	uint32_t dst;

	info.assign(vas("[fieee80211frame(%p) dst:%s src:%s length:%d vers:%d dscp:%d ecn:%d ihl:%d proto:%d ttl:%d ]",
			this,
			get_ieee80211_dst().addr_c_str(),
			get_ieee80211_src().addr_c_str(),
			be16toh(ieee80211_hdr->length),
			get_ieee80211_version(),
			get_ieee80211_dscp(),
			get_ieee80211_ecn(),
			get_ieee80211_ihl(),
			get_ieee80211_proto(),
			get_ieee80211_ttl()
			));
#endif
	info.assign("not implementd");

	return info.c_str();
}

uint8_t
fieee80211frame::get_ieee80211_order()
{
	return get_ieee80211_fc() & WLAN_FC_ORDER;
}

void
fieee80211frame::set_ieee80211_fc(uint16_t fc)
{
	*((uint16_t* )ieee80211_hdr) = htole16(fc);
}

uint16_t
fieee80211frame::get_ieee80211_fc()
{
	return le16toh(*(uint16_t* )ieee80211_hdr);
}

void
fieee80211frame::set_ieee80211_type(uint8_t type)
{
	set_ieee80211_fc((get_ieee80211_fc() & ~WLAN_FC_TYPE) |
			 ((type << 2) & WLAN_FC_TYPE));
}

uint8_t
fieee80211frame::get_ieee80211_type()
{
	return (get_ieee80211_fc() & WLAN_FC_TYPE) >> 2;
}

void
fieee80211frame::set_ieee80211_subtype(uint8_t subtype)
{
	set_ieee80211_fc((get_ieee80211_fc() & ~WLAN_FC_SUBTYPE) |
			 ((subtype << 4) & WLAN_FC_SUBTYPE));
}

uint8_t
fieee80211frame::get_ieee80211_subtype()
{
	return (get_ieee80211_fc() & WLAN_FC_SUBTYPE) >> 4;
}

void
fieee80211frame::set_ieee80211_direction(uint8_t direction)
{
	set_ieee80211_fc((get_ieee80211_fc() & ~(WLAN_FC_TODS | WLAN_FC_FROMDS)) |
			 ((direction << 8) & (WLAN_FC_TODS | WLAN_FC_FROMDS)));
}

uint8_t
fieee80211frame::get_ieee80211_direction()
{
	return (get_ieee80211_fc() & (WLAN_FC_TODS | WLAN_FC_FROMDS)) >> 8;
}

void
fieee80211frame::set_ieee80211_address_1(cmacaddr const& address) throw (eFrameInval)
{
	if (address.memlen() < OFP_ETH_ALEN)
	{
		throw eFrameInval();
	}

	initialize();

	if (!complete())
	{
		throw eFrameInval();
	}

	memcpy(ieee80211_hdr + 4, address.somem(), OFP_ETH_ALEN);
}


cmacaddr
fieee80211frame::get_ieee80211_address_1() const
{
	cmacaddr ma(ieee80211_hdr + 4, OFP_ETH_ALEN);
	return ma;
}

void
fieee80211frame::set_ieee80211_address_2(cmacaddr const& address) throw (eFrameInval)
{
	if (address.memlen() < OFP_ETH_ALEN)
	{
		throw eFrameInval();
	}

	initialize();

	if (!complete())
	{
		throw eFrameInval();
	}

	memcpy(ieee80211_hdr + 10, address.somem(), OFP_ETH_ALEN);
}


cmacaddr
fieee80211frame::get_ieee80211_address_2() const
{
	cmacaddr ma(ieee80211_hdr + 10, OFP_ETH_ALEN);
	return ma;
}

void
fieee80211frame::set_ieee80211_address_3(cmacaddr const& address) throw (eFrameInval)
{
	if (address.memlen() < OFP_ETH_ALEN)
	{
		throw eFrameInval();
	}

	initialize();

	if (!complete())
	{
		throw eFrameInval();
	}

	memcpy(ieee80211_hdr + 16, address.somem(), OFP_ETH_ALEN);
}


cmacaddr
fieee80211frame::get_ieee80211_address_3() const
{
	cmacaddr ma(ieee80211_hdr + 16, OFP_ETH_ALEN);
	return ma;
}

uint16_t
fieee80211frame::get_ieee80211_length()
{
    return ieee80211datalen;
}


uint8_t
fieee80211frame::get_ieee80211_hlen()
{
	int hlen = IEEE80211_HDR_LEN;

	if (get_ieee80211_direction() == 3)
		hlen += 6;

	switch (get_ieee80211_type()) {
	case WLAN_FC_TYPE_DATA:
		if (get_ieee80211_subtype() & WLAN_FC_STYPE_QOS) {
			hlen += 2;
			if (get_ieee80211_order())
				hlen += 4;
		}
		break;

	case WLAN_FC_TYPE_CTRL:
		if (get_ieee80211_subtype() == WLAN_FC_STYPE_CONTROL_WRAPPER)
			hlen += 4;
		break;

	case WLAN_FC_TYPE_MGMT:
		if (get_ieee80211_order())
			hlen += 4;
		break;

	default:
		break;
	}

	return hlen;
}
