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
#define STRICT_MODE_RFC2516 0
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
fpppoeframe::validate(uint16_t total_len)
      throw (eFrameInvalidSyntax)
{
    try {
          //initialize(); // commented out 2012-12-13

          if (!complete()) {
        	  throw ePPPoEFrameTooShort();
          }

          if (PPPOE_TYPE != get_pppoe_type()) {
        	  throw ePPPoEFrameInvalType();
          }

          if (PPPOE_VERSION != get_pppoe_vers()) {
              throw ePPPoEFrameInvalVersion();
          }

#if 0
          // FIXME: set total_len appropriately
          if (get_hdr_length() > (total_len - sizeof(struct fpppoeframe::pppoe_hdr_t)))
          {
              //WRITELOG(CPACKET, WARN, "fpppoeframe(%p)::validate(): "
                                                      "invalid PPPoE frame rcvd: invalid header length => %s", this, c_str());

              throw ePPPoEFrameTooShort();
          }
#endif

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

          return;

    } catch (ePPPoEFrameInvalCode& e) {

        logging::warn << "[rofl][frame][pppoe][validate] unsupported pppoe code:" << (int)pppoe_hdr->code << std::endl;

    } catch (ePPPoEFrameInvalType& e) {

        logging::warn << "[rofl][frame][pppoe][validate] unsupported pppoe type:" << (int)get_pppoe_type() << std::endl;

    } catch (ePPPoEFrameInvalVersion& e) {

        logging::warn << "[rofl][frame][pppoe][validate] unsupported pppoe type:" << (int)get_pppoe_vers() << std::endl;

    } catch (ePPPoElistNotFound& e) {

    	logging::warn << "[rofl][frame][pppoe][validate] no SVCname tag or no ACname tag found" << std::endl;

    } catch (ePPPoEFrameInvalSid& e) {

    	logging::warn << "[rofl][frame][pppoe][validate] invalid session id" << std::endl;

    } catch (ePPPoEFrameTooShort& e) {

    	logging::warn << "[rofl][frame][pppoe][validate] invalid header length specified" << std::endl;

    } catch (ePPPoEBadLen& e) {

    	logging::warn << "[rofl][frame][pppoe][validate] unable to parse tags" << std::endl;

    } catch (...) {

    	logging::warn << "[rofl][frame][pppoe][validate] generic error" << std::endl;
    }

    throw eFrameInvalidSyntax();
}


void
fpppoeframe::validate_pppoe_discovery_padi()
      throw (ePPPoEFrameInvalSid, ePPPoEBadLen, ePPPoElistNotFound)
{
#ifdef STRICT_MODE_RFC2516
    if (0x0000 != get_pppoe_sessid()) // session id must be 0x0000
    {
        throw ePPPoEFrameInvalSid();
    }
#endif

    tags.unpack(pppoe_hdr->data, (framelen() - sizeof(struct fpppoeframe::pppoe_hdr_t)));

#ifdef STRICT_MODE_RFC2516
    tags.find_pppoe_tlv(PPPOE_TAG_SERVICE_NAME);
#endif
}


void
fpppoeframe::validate_pppoe_discovery_pado()
      throw (ePPPoEFrameInvalSid, ePPPoEBadLen, ePPPoElistNotFound)
{
#ifdef STRICT_MODE_RFC2516
    if (0x0000 != get_pppoe_sessid()) // session id must be 0x0000
    {
        throw ePPPoEFrameInvalSid();
    }
#endif

    tags.unpack(pppoe_hdr->data, get_hdr_length());

#ifdef STRICT_MODE_RFC2516
    tags.find_pppoe_tlv(PPPOE_TAG_AC_NAME); // tag -AC-Name- must be present

    tags.find_pppoe_tlv(PPPOE_TAG_SERVICE_NAME); // tag -Service-Name- must be present
#endif

}

void
fpppoeframe::validate_pppoe_discovery_padr()
      throw (ePPPoEFrameInvalSid, ePPPoEBadLen, ePPPoElistNotFound)
{
#ifdef STRICT_MODE_RFC2516
    if (0x0000 != get_pppoe_sessid())
    {
        throw ePPPoEFrameInvalSid();
    }
#endif

    tags.unpack(pppoe_hdr->data, get_hdr_length());

#ifdef STRICT_MODE_RFC2516
    tags.find_pppoe_tlv(PPPOE_TAG_SERVICE_NAME);
#endif
}


void
fpppoeframe::validate_pppoe_discovery_pads()
      throw (ePPPoEFrameInvalSid, ePPPoEBadLen, ePPPoElistNotFound)
{
    tags.unpack(pppoe_hdr->data, get_hdr_length());

#ifdef STRICT_MODE_RFC2516
    tags.find_pppoe_tlv(PPPOE_TAG_SERVICE_NAME);
#endif

    // FIXME: this behavior is invalid compared to RFC 2516, check section 5.4
}



void
fpppoeframe::validate_pppoe_discovery_padt()
      throw (ePPPoEFrameInvalSid, ePPPoEBadLen, ePPPoElistNotFound)
{
#ifdef STRICT_MODE_RFC2516
    if (0x0000 == get_pppoe_sessid()) // session id should not be 0x0000 for a PADT (ongoing session)
    {
        // some devices set sessid to 0x0000 in PADT, although it is forbidden in RFC 2516.
        // Be liberal what you accept ... including all crappy stuff from other people.
        throw ePPPoEFrameInvalSid();
    }
#endif
}


void
fpppoeframe::validate_pppoe_session()
      throw (ePPPoEFrameInvalSid, eFrameInvalidSyntax)
{
#ifdef STRICT_MODE_RFC2516
    if (0x0000 == get_pppoe_sessid()) // sessid must not be 0
    {
            throw ePPPoEFrameInvalSid();
    }
#endif

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

		logging::warn << "[rofl][frame][pppoe][validate-pppoe-session] "
				"PPPoE length field larger than PPP payload" << std::endl;
		pppdata = NULL;
		pppdatalen = 0;
		throw eFrameInvalidSyntax();
	}
}

uint8_t
fpppoeframe::get_pppoe_vers() const
{
	return ((pppoe_hdr->verstype & 0xf0) >> 4);
}

void
fpppoeframe::set_pppoe_vers(uint8_t version)
{
	pppoe_hdr->verstype = (pppoe_hdr->verstype & 0x0f) + ((version & 0x0f) << 4);
}

uint8_t
fpppoeframe::get_pppoe_type() const
{
	return (pppoe_hdr->verstype & 0x0f);
}

void
fpppoeframe::set_pppoe_type(uint8_t type)
{
	pppoe_hdr->verstype = (pppoe_hdr->verstype & 0xf0) + (type & 0x0f);
}

uint8_t
fpppoeframe::get_pppoe_code() const
{
	return pppoe_hdr->code;
}

void
fpppoeframe::set_pppoe_code(uint8_t code)
{
	pppoe_hdr->code = code;
}

uint16_t
fpppoeframe::get_pppoe_sessid() const
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
fpppoeframe::unpack(uint8_t *frame, size_t framelen) throw (ePPPoEInval, eFrameInvalidSyntax)
{
	reset(frame, framelen);

	if (!complete())
		throw ePPPoEInval();

	validate();
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

