/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fpppoeframe.h"

using namespace rofl;

/*
 * comment: use define -DSTRICT_MODE_RFC2516 in order to activate
 * checking of presence of PPPoE tags in discovery frames
 */
#ifndef STRICT_MODE_RFC2516
#define STRICT_MODE_RFC2516 1
#endif

fpppoeframe::fpppoeframe(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		pppoe_hdr(0),
		pppdata(0),
		pppdatalen(0)
{
	initialize();
}


fpppoeframe::fpppoeframe(
		size_t len) :
		fframe(len),
		pppoe_hdr(0),
		pppdata(0),
		pppdatalen(0)
{
	initialize();
	set_pppoe_vers(PPPOE_VERSION);
	set_pppoe_type(PPPOE_TYPE);
}



fpppoeframe::~fpppoeframe()
{

}


void
fpppoeframe::initialize()
{
	pppoe_hdr = (struct pppoe_hdr_t*)soframe();

	if (framelen() <= sizeof(struct fpppoeframe::pppoe_hdr_t))
	{
		return;
	}

	pppdata = pppoe_hdr->data;
	pppdatalen = framelen() - sizeof(struct fpppoeframe::pppoe_hdr_t);
}


bool
fpppoeframe::complete()
{
	initialize();

	if (framelen() < sizeof(struct pppoe_hdr_t))
		return false;

	return true;
}


size_t
fpppoeframe::need_bytes()
{
	if (complete())
		return 0;

	if (framelen() < sizeof(struct pppoe_hdr_t))
		return (sizeof(struct pppoe_hdr_t) - framelen());

	if (framelen() < be16toh(pppoe_hdr->length))
		return (be16toh(pppoe_hdr->length) - framelen());

	return 0; // just to make gcc happy
}


void
fpppoeframe::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct pppoe_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(pppoe_hdr->data, data, datalen);
	pppoe_hdr->length = htobe16(datalen);
}


uint8_t*
fpppoeframe::payload() const throw (eFrameNoPayload)
{
	if (!pppdata)
		throw eFrameNoPayload();
	return pppdata;
}


size_t
fpppoeframe::payloadlen() const throw (eFrameNoPayload)
{
	if (!pppdatalen)
		throw eFrameNoPayload();
	return pppdatalen;
}


void
fpppoeframe::validate(uint16_t total_len) throw (ePPPoEFrameTooShort,
									ePPPoEFrameInvalType,
									ePPPoEFrameInvalVersion,
									ePPPoEFrameInvalCode,
									ePPPoEPadsInvalSid,
									ePPPoEPadtInvalCode,
									ePPPoEPadtInvalSid)
{
	//initialize(); // commented out 2012-12-13

	if (!complete())
	{
		WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::validate(): "
							"invalid PPPoE frame rcvd: incomplete => %s", this, c_str());
		throw ePPPoEFrameTooShort();
	}

	if (PPPOE_TYPE != get_pppoe_type())
	{
		WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::validate(): "
							"invalid PPPoE frame rcvd: type => %s", this, c_str());
		throw ePPPoEFrameInvalType();
	}

	if (PPPOE_VERSION != get_pppoe_vers())
	{
		WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::validate(): "
							"invalid PPPoE frame rcvd: version => %s", this, c_str());
		throw ePPPoEFrameInvalVersion();
	}

	switch (pppoe_hdr->code) {
	case PPPOE_CODE_SESSION_DATA:
		validate_pppoe_session();
		break;
	case PPPOE_CODE_PADI:
		validate_pppoe_discovery_padi();
		break;
	case PPPOE_CODE_PADO:
		validate_pppoe_discovery_pado();
		break;
	case PPPOE_CODE_PADR:
		validate_pppoe_discovery_padr();
		break;
	case PPPOE_CODE_PADS:
		validate_pppoe_discovery_pads();
		break;
	case PPPOE_CODE_PADT:
		validate_pppoe_discovery_padt();
		break;
	default:
		throw ePPPoEFrameInvalCode();
		break;
	}
}


void
fpppoeframe::validate_pppoe_session() throw (ePPPoEFrameInvalCode)
{
	if (0x0000 != get_pppoe_code()) // code field must not be 0
	{
		WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::is_valid_pppoe_session(): "
						"invalid PPPoE code %d", this, get_pppoe_type());
		throw ePPPoEFrameInvalCode();
	}

	size_t res_len = framelen() - sizeof(struct pppoe_hdr_t); // effective PPPoE payload length

	pppdatalen = get_hdr_length(); // PPPoE payload length claimed in header

	return; // FIXME: see below

	if (pppdatalen == 0) // is this valid? set ppp_payload to NULL
	{
		pppdata = NULL;
	}
	else if ((pppdatalen > 0) && (pppdatalen <= res_len)) // acceptable length range
	{
		pppdata = (uint8_t*)(pppoe_hdr->data);// get pointer to ppp payload: immediately after PPPoE base header
	}
	else // pppdatalen > res_len, this is invalid, reject packet
	{
#if 0
		// FIXME
		if (pppdatalen < totalpayloadlen())
		{
			return;
		}
#endif

		WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::is_valid_pppoe_session(): "
				"PPPoE length field larger than PPP payload (%d > %d)", this, pppdatalen, res_len);
		pppdata = NULL;
		pppdatalen = 0;
		throw eFrameInvalidSyntax();
	}
}

uint8_t
fpppoeframe::get_pppoe_vers()
{
	return ((pppoe_hdr->verstype & 0xf0) >> 4);
}

void
fpppoeframe::set_pppoe_vers(uint8_t version)
{
	pppoe_hdr->verstype = (pppoe_hdr->verstype & 0x0f) + ((version & 0x0f) << 4);
}

uint8_t
fpppoeframe::get_pppoe_type()
{
	return (pppoe_hdr->verstype & 0x0f);
}

void
fpppoeframe::set_pppoe_type(uint8_t type)
{
	pppoe_hdr->verstype = (pppoe_hdr->verstype & 0xf0) + (type & 0x0f);
}

uint8_t
fpppoeframe::get_pppoe_code()
{
	return pppoe_hdr->code;
}

void
fpppoeframe::set_pppoe_code(uint8_t code)
{
	pppoe_hdr->code = code;
}

uint16_t
fpppoeframe::get_pppoe_sessid()
{
	return be16toh(pppoe_hdr->sessid);
}

void
fpppoeframe::set_pppoe_sessid(uint16_t sessid)
{
	pppoe_hdr->sessid = htobe16(sessid);
}

void
fpppoeframe::pppoe_calc_length()
{
	WRITELOG(CPACKET, DBG, "fpppoeframe(%p)::pppoe_calc_length() setting to length %d",
			this, payloadlen());
	pppoe_hdr->length = htobe16(payloadlen());
}


uint16_t
fpppoeframe::get_hdr_length()
{
	if (NULL == pppoe_hdr)
	{
		initialize();
	}
	return be16toh(pppoe_hdr->length);
}


void
fpppoeframe::set_hdr_length(uint16_t len)
{
	if (NULL == pppoe_hdr)
	{
		initialize();
	}
	pppoe_hdr->length = htobe16(len);
}


size_t
fpppoeframe::length()
{
	switch (get_pppoe_code()) {
	// discovery frames carry tags only, no ppp data
	case PPPOE_CODE_PADI:
	case PPPOE_CODE_PADO:
	case PPPOE_CODE_PADR:
	case PPPOE_CODE_PADS:
	case PPPOE_CODE_PADT:
		return (sizeof(struct pppoe_hdr_t) + tags.length());

	// session frames carry ppp payload only, no tags
	case PPPOE_CODE_SESSION_DATA:
		return (sizeof(struct pppoe_hdr_t) + pppdatalen);
	}

	return 0;
}



cpacket&
fpppoeframe::pack(cpacket& packet)
{
#if 0
	packet.resize(length());
	packet.stored_bytes(length());
	//fprintf(stderr, "UUU => packet: %s\n", packet.c_str());
	pack((uint8_t*)packet.soframe(), packet.framelen());
#endif
	return packet;
}


size_t
fpppoeframe::pack(uint8_t *frame, size_t framelen) throw (ePPPoEInval)
{
	size_t len = length();

	if (framelen < len)
		throw ePPPoEInval();

	switch (get_pppoe_code()) {
	// discovery frames carry tags only, no ppp data
	case PPPOE_CODE_PADI:
	case PPPOE_CODE_PADO:
	case PPPOE_CODE_PADR:
	case PPPOE_CODE_PADS:
	case PPPOE_CODE_PADT:
	{
		set_hdr_length(tags.length());
		memcpy(frame, (uint8_t*)pppoe_hdr, sizeof(struct pppoe_hdr_t));
		tags.pack(frame + sizeof(struct pppoe_hdr_t), len - sizeof(struct pppoe_hdr_t));
		break;
	}

	// session frames carry ppp payload only, no tags
	case PPPOE_CODE_SESSION_DATA:
	{
		set_hdr_length(len);
		memcpy(frame, (uint8_t*)pppoe_hdr, len);
		break;
	}

	} // end switch(get_pppoe_code()) { ...

	return len;
}


void
fpppoeframe::unpack(uint8_t *frame, size_t framelen) throw (ePPPoEInval)
{
	reset(frame, framelen);

	if (!complete())
		throw ePPPoEInval();

	validate();
}

#if 0
void
fpppoeframe::parse_pppoe_tags() throw (eFrameInvalidSyntax)
{
	struct pppoe_tag_hdr_t *tag = (struct pppoe_tag_hdr_t*)(pppoe_hdr->data);
	size_t res_len = be16toh(pppoe_hdr->length); // remaining length after pppoe header

	tags.unpack(pppoe_hdr->data, res_len);

	pppoe_tags.clear(); // initialize vector of pppoe tag pointers

	if (res_len == 0) // no tags at all
		return;

	while (res_len > 0)
	{
		WRITELOG(CPACKET, DBG, "fpppoeframe(%p)::parse_pppoe_tags() tag=%p tag->type=%d tag->length=%d res_len=%d",
				this, tag, be16toh(tag->type), be16toh(tag->length), res_len);

		if (res_len < sizeof(struct pppoe_tag_hdr_t))
		{
			WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::parse_pppoe_tags(): "
							"remaining length insufficient for tag (%d > %d)",
							this, res_len, sizeof(struct pppoe_tag_hdr_t));
			return; // throw exception instead?
		}

		size_t tag_len = be16toh(tag->length); // excludes the pppoe tag header fields type and value!
		if ((sizeof(struct pppoe_tag_hdr_t) +  tag_len) > res_len)
		{
			WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::parse_pppoe_tags(): "
							"invalid tag length field (%d > %d)",
							this, (sizeof(struct pppoe_tag_hdr_t) + tag_len), res_len);
			return; // throw exception instead?
		}

		switch (be16toh(tag->type)) {
		case PPPOE_TAG_END_OF_LIST:
			if (be16toh(tag->length) != 0x0000)
			{
				WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::parse_pppoe_tags(): PPPoE tag -End-of-List- with "
						"invalid length field (%d)", this, be16toh(tag->length));
				throw eFrameInvalidSyntax();
			}
			break;
		case PPPOE_TAG_SERVICE_NAME:
			if (be16toh(tag->length) == 0)
			{
				WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::parse_pppoe_tags(): PPPoE tag -Service-Name- with "
						"invalid length field (%d)", this, be16toh(tag->length));
				throw eFrameInvalidSyntax();
			}
			break;
		case PPPOE_TAG_AC_NAME:
			if (be16toh(tag->length) == 0)
			{
				WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::parse_pppoe_tags(): PPPoE tag -AC-Name- with "
						"invalid length field (%d)", this, be16toh(tag->length));
				throw eFrameInvalidSyntax();
			}
			break;
		case PPPOE_TAG_HOST_UNIQ:
			if (be16toh(tag->length) == 0)
			{
				WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::parse_pppoe_tags(): PPPoE tag -Host-Uniq- with "
						"invalid length field (%d)", this, be16toh(tag->length));
				throw eFrameInvalidSyntax();
			}
			break;
		case PPPOE_TAG_AC_COOKIE:
			if (be16toh(tag->length) == 0)
			{
				WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::parse_pppoe_tags(): PPPoE tag -AC-Cookie- with "
						"invalid length field (%d)", this, be16toh(tag->length));
				throw eFrameInvalidSyntax();
			}
			break;
		case PPPOE_TAG_VENDOR_SPECIFIC:
			if (be16toh(tag->length) < 4)
			{
				WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::parse_pppoe_tags(): PPPoE tag -Vendor-Specific- with "
						"invalid length field (%d)", this, be16toh(tag->length));
				throw eFrameInvalidSyntax();
			}
			break;
		case PPPOE_TAG_RELAY_SESSION_ID:
			// nothing to do, opaque tag value, so may be zero in length
			break;
		case PPPOE_TAG_SERVICE_NAME_ERROR:
			// nothing to do, may be zero or not
			break;
		case PPPOE_TAG_AC_SYSTEM_ERROR:
			// nothing to do, may be zero or not
			break;
		case PPPOE_TAG_GENERIC_ERROR:
			// nothing to do, may be zero or not
			break;
		default:
			WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::parse_pppoe_tags(): "
										"unknown tag type (%d)", this, be16toh(tag->type));
			break;
		}

		pppoe_tags[(enum pppoe_tag_t)be16toh(tag->type)] = tag;

		res_len -= (sizeof(struct pppoe_tag_hdr_t) + tag_len);
		tag = (struct pppoe_tag_hdr_t*)((uint8_t*)tag + (sizeof(struct pppoe_tag_hdr_t) + tag_len));
	}

}
#endif

void
fpppoeframe::validate_pppoe_discovery_padi() throw (eFrameInvalidSyntax)
{
	try {

		//fprintf(stderr, "YYY => pppoe: %s\n", c_str());

		if (PPPOE_CODE_PADI != get_pppoe_code())
			throw ePPPoEPadiInvalCode();

		if (0x0000 != get_pppoe_sessid()) // session id must be 0x0000
			throw ePPPoEPadiInvalSid();

		if (get_hdr_length() > (framelen() - sizeof(struct fpppoeframe::pppoe_hdr_t)))
		{
			throw ePPPoEFrameTooShort();
		}

		tags.unpack(pppoe_hdr->data, (framelen() - sizeof(struct fpppoeframe::pppoe_hdr_t)));

		//fprintf(stderr, "validate_pppoe_discovery_padi() tags: %s\n", tags.c_str());

		tags.find_pppoe_tlv(PPPOE_TAG_SERVICE_NAME);

	} catch (ePPPoElistNotFound& e) {

#ifdef STRICT_MODE_RFC2516
		throw ePPPoEPadiNoSvcTag();
#else
		WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::validate_pppoe_discovery_padi() "
		    "invalid PPPoE PADI frame rcvd => no SVCname tag found", this);
#endif
	}
}

void
fpppoeframe::validate_pppoe_discovery_pado() throw (eFrameInvalidSyntax)
{
	try {

		if (PPPOE_CODE_PADO != get_pppoe_code())
			throw eFrameInvalidSyntax();

		if (0x0000 != get_pppoe_sessid()) // session id must be 0x0000
			throw eFrameInvalidSyntax();

		tags.unpack(pppoe_hdr->data, get_hdr_length());

		tags.find_pppoe_tlv(PPPOE_TAG_AC_NAME); // tag -AC-Name- must be present

		tags.find_pppoe_tlv(PPPOE_TAG_SERVICE_NAME); // tag -Service-Name- must be present

	} catch (ePPPoElistNotFound& e) {

#ifdef STRICT_MODE_RFC2516
             throw eFrameInvalidSyntax();
#else
             WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::validate_pppoe_discovery_pado() "
                    "invalid PPPoE PADO frame rcvd => no SVCname tag or no ACname tag found", this);
#endif
	}
}

void
fpppoeframe::validate_pppoe_discovery_padr() throw (eFrameInvalidSyntax)
{
	try {

		if (PPPOE_CODE_PADR != get_pppoe_code())
			throw ePPPoEPadrInvalCode();

		if (0x0000 != get_pppoe_sessid())
			throw ePPPoEPadrInvalSid();

		tags.unpack(pppoe_hdr->data, get_hdr_length());

		tags.find_pppoe_tlv(PPPOE_TAG_SERVICE_NAME);

	} catch (ePPPoElistNotFound& e) {

#ifdef STRICT_MODE_RFC2516
	     throw ePPPoEPadrNoSvcTag();
#else
             WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::validate_pppoe_discovery_padr() "
                    "invalid PPPoE PADR frame rcvd => no SVCname tag found", this);
#endif
	}
}

void
fpppoeframe::validate_pppoe_discovery_pads() throw (ePPPoEFrameInvalCode, ePPPoEPadsInvalSid)
{
	try {

		if (PPPOE_CODE_PADS != get_pppoe_code())
		{
			throw ePPPoEFrameInvalCode();
		}

		tags.unpack(pppoe_hdr->data, get_hdr_length());

		tags.find_pppoe_tlv(PPPOE_TAG_SERVICE_NAME);

		// FIXME: this behavior is invalid compared to RFC 2516, check section 5.4

	} catch (ePPPoElistNotFound& e) {

#ifdef STRICT_MODE_RFC2516
             throw eFrameInvalidSyntax();
#else
             WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::validate_pppoe_discovery_pads() "
                    "invalid PPPoE PADS frame rcvd => no SVCname tag found", this);
#endif
	}
}


void
fpppoeframe::validate_pppoe_discovery_padt() throw (ePPPoEPadtInvalCode, ePPPoEPadtInvalSid)
{
#if 0
	parse_pppoe_tags(); // fill in pppoe_tags vector
#endif

	if (get_pppoe_code() != PPPOE_CODE_PADT)
	{
		throw ePPPoEPadtInvalCode();
	}

	if (0x0000 == get_pppoe_sessid()) // session id should not be 0x0000 for a PADT (ongoing session)
	{
		// some devices set sessid to 0x0000 in PADT, although it is forbidden in RFC 2516.
		// Be liberal what you accept ... including all crappy stuff from other people.
		//throw ePPPoEPadtInvalSid();
	}
}


const char*
fpppoeframe::c_str()
{
	cvastring vas(1024);

	info.assign(vas("[fpppoeframe(%p) code:0x%02x type:0x%02x length:%d sess:0x%04x [tags:%s] ]",
			this,
			get_pppoe_code(),
			get_pppoe_type(),
			get_hdr_length(),
			get_pppoe_sessid(),
			tags.c_str() ));

	return info.c_str();
}


/*static*/ void
fpppoeframe::test()
{
#if 0
	cpacket packet;

	cmemory cookie(4);
	cookie[0] = 0xde;
	cookie[1] = 0xad;
	cookie[2] = 0xbe;
	cookie[3] = 0xef;

	fpppoeframe padi(128);

	padi.set_pppoe_code(PPPOE_CODE_PADI);
	padi.tags.next() = cpppoetlv_service_name(std::string("service name"));
	padi.tags.next() = cpppoetlv_ac_name(std::string("ac name"));
	padi.tags.next() = cpppoetlv_ac_cookie(cookie);
	padi.tags.next() = cpppoetlv_end();

	padi.pack(packet);

	//fprintf(stderr, "padi: %s\n", padi.c_str());

	//fprintf(stderr, "pack: %s\n", packet.c_str());

	fpppoeframe pppoe2(packet.soframe(), packet.framelen(), packet.framelen());
	pppoe2.validate();

	//fprintf(stderr, "pppoe2: %s\n", pppoe2.c_str());
#endif
}

