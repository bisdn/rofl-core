/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "flldpframe.h"

flldpframe::flldpframe(
		uint8_t *_data,
		size_t _datalen,
		uint16_t _totallen) :
	fframe(_data, _datalen, _totallen),
	lldp_hdr(NULL)
{
	initialize();
	validate();
}


flldpframe::flldpframe(
		size_t len) :
		fframe(len),
		lldp_hdr(NULL)
{
	initialize();
}


flldpframe::~flldpframe()
{
	// do _NOT_ delete or deallocate (data,datalen) here!
}


void
flldpframe::initialize()
{
	lldp_hdr = (struct clldptlv::lldp_tlv_hdr_t*)soframe();
}


bool
flldpframe::complete()
{
	return true;
}


size_t
flldpframe::need_bytes()
{
	initialize();

	// TODO: use length field from Ethernet 802.3 header, for now just Ethernet II

	throw eNotImplemented();

	return 0;
}


void
flldpframe::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{

}


uint8_t*
flldpframe::payload() throw (eFrameNoPayload)
{
	return NULL;
}


size_t
flldpframe::payloadlen() throw (eFrameNoPayload)
{
	return tlvs.length();
}


void
flldpframe::validate() throw (eFrameInvalidSyntax)
{
	initialize();

	try {
		//fprintf(stderr, "fframe: %s\n", fframe::c_str());
		tlvs.unpack(soframe(), framelen());

	} catch (eLLDPbase& e) {

	}
}


const char*
flldpframe::c_str()
{
	cvastring vas(1024);

	info.assign(vas("[flldpframe(%p)\ntlvs =>\n%s\nfframe =>\n%s]",
			this,
			tlvs.c_str(),
			fframe::c_str()));

	return info.c_str();
}


size_t
flldpframe::length()
{
	return (sizeof(struct lldp_hdr_t) + tlvs.length());
}


#if 0
size_t
flldpframe::pack(cpacket& packet)
{
	if (packet.length() < length())
	{
		packet.resize(length());
		packet.stored_bytes(length());
	}
	return pack((uint8_t*)packet.soframe(), packet.framelen());
}
#endif


size_t
flldpframe::pack(uint8_t *mem, size_t memlen) throw (eLLDPInval)
{
	if (memlen < length())
		throw eLLDPInval();

	memcpy(mem, (uint8_t*)lldp_hdr, sizeof(struct lldp_hdr_t));
	tlvs.pack(mem + sizeof(struct lldp_hdr_t), memlen - sizeof(struct lldp_hdr_t));

	return length();
}


/*static*/
void
flldpframe::test()
{
	flldpframe l;

	l.tlvs.next() = clldptlv_chassis_id(clldptlv::LLDPCHIDST_PORT, std::string("eth0"));
	l.tlvs.next() = clldptlv_sys_caps(clldptlv::LLDP_SYSCAPS_BRIDGE |
										clldptlv::LLDP_SYSCAPS_ROUTER,
										clldptlv::LLDP_SYSCAPS_BRIDGE);
	l.tlvs.next() = clldptlv_port_desc(std::string("port eth0"));
	l.tlvs.next() = clldptlv_sys_name(std::string("this is a longer system name for testing the lldp code"));
	l.tlvs.next() = clldptlv_end();

	fprintf(stderr, "l: %s\n", l.c_str());

#if 0
	cpacket pack;
	l.pack(pack);

	fprintf(stderr, "lmem: %s\n", pack.c_str());

	flldpframe lframe(pack.soframe(), pack.framelen(), pack.framelen());

	fprintf(stderr, "lframe: %s\n", lframe.c_str());
#endif
}
