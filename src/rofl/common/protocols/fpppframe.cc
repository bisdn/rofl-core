/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fpppframe.h"

using namespace rofl;

/*
* FCS lookup table as calculated by the table generator.
*/
/*static*/ uint16_t fpppframe::fcstab[256] = {
  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
  0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
  0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
  0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
  0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
  0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
  0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
  0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
  0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
  0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
  0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
  0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
  0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
  0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
  0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
  0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
  0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

#define PPPINITFCS16    0xffff  /* Initial FCS value */
#define PPPGOODFCS16    0xf0b8  /* Good final FCS value */


fpppframe::fpppframe(
		uint8_t *data,
		size_t datalen) :
		fframe(data, datalen),
		ppp_hdr(0),
		ppp_lcp_hdr(0),
		ppp_ipcp_hdr(0)
{
	initialize();
}



fpppframe::~fpppframe()
{
	while (not lcp_options.empty()) {
		delete lcp_options.begin()->second;
		lcp_options.erase(lcp_options.begin());
	}

	while (not ipcp_options.empty()) {
		delete ipcp_options.begin()->second;
		ipcp_options.erase(ipcp_options.begin());
	}
}



void
fpppframe::unpack(uint8_t *frame, size_t framelen) throw (ePPPInval)
{
	reset(frame, framelen);

	initialize();

	if (!complete())
		throw ePPPInval();

	validate();
}


bool
fpppframe::complete() const
{
	//initialize();

	if (framelen() < sizeof(struct ppp_hdr_t))
		return false;

	switch (be16toh(ppp_hdr->prot)) {
	case PPP_PROT_CHAP: // TODO
		break;
	case PPP_PROT_LCP:
		if (framelen() < (sizeof(struct ppp_hdr_t) +
								sizeof(struct ppp_lcp_hdr_t)))
		{
			return false;
		}
		if (framelen() < (sizeof(struct ppp_hdr_t) +
								//sizeof(struct ppp_lcp_hdr_t) +
								be16toh(ppp_lcp_hdr->length)))
		{
			return false;
		}
		break;
	case PPP_PROT_LQR: // TODO
		break;
	case PPP_PROT_PADDING: // TODO
		break;
	case PPP_PROT_PAP: // TODO
		break;
	}

	return true;
}


size_t
fpppframe::need_bytes() const
{
	if (complete())
		return 0;

	if (framelen() < sizeof(struct ppp_hdr_t))
		return (sizeof(struct ppp_hdr_t) - framelen());

	switch (be16toh(ppp_hdr->prot)) {
	case PPP_PROT_CHAP: // TODO
		break;
	case PPP_PROT_LCP:
		if (framelen() < (sizeof(struct ppp_hdr_t) +
								sizeof(struct ppp_lcp_hdr_t)))
		{
			return ((sizeof(struct ppp_hdr_t) +
					sizeof(struct ppp_lcp_hdr_t)) - framelen());
		}
		if (framelen() < (sizeof(struct ppp_hdr_t) +
								//sizeof(struct ppp_lcp_hdr_t) +
								be16toh(ppp_lcp_hdr->length)))
		{
			return ((sizeof(struct ppp_hdr_t) +
					sizeof(struct ppp_lcp_hdr_t) +
					be16toh(ppp_lcp_hdr->length)) - framelen());
		}
		break;
	case PPP_PROT_LQR: // TODO
		break;
	case PPP_PROT_PADDING: // TODO
		break;
	case PPP_PROT_PAP: // TODO
		break;
	}

	return 0;
}


void
fpppframe::initialize()
{
	if (0 == framelen())
		return;

	ppp_hdr = (struct ppp_hdr_t*)soframe();

	switch (be16toh(ppp_hdr->prot)) {
	case PPP_PROT_LCP:
		ppp_lcp_hdr = (struct ppp_lcp_hdr_t*)(soframe() + sizeof(struct ppp_hdr_t));
		break;
	}
}


void
fpppframe::hdlc_decode(
		cmemory& decoded,
		uint32_t accm_recv) throw (ePPPFrameHdlcDecodeFailed)
{
#if 1
	try {
		decoded.resize(4 + framelen());
#if 0
		decoded.stored_bytes(4 + framelen());
#endif

		bool escape_it = false;
		size_t mem_idx = 0;

		for (size_t idx = 0; idx < framelen(); ++idx)
		{
			if (soframe()[idx] < 0x20) // check all values smaller than 0x20 with accm_recv
			{
				if (accm_recv & (1 << soframe()[idx])) // value is flagged, simply remove from decoded frame
				{
					continue;
				}
			}
			else // soframe()[idx] >= 0x20
			{
				if (soframe()[idx] == HDLC_ESCAPE)
				{
					escape_it = true;
					continue;
				}
				else
				{
					if (escape_it)
					{
						decoded[mem_idx] = (soframe()[idx] ^ 0x20); // flip bit 5
						escape_it = false;
					}
					else
					{
						decoded[mem_idx] = soframe()[idx];
					}

					mem_idx++;
				}
			}
		}

		decoded.remove(mem_idx, decoded.memlen() - (mem_idx));
		//decoded.stored_bytes(mem_idx);


		// remove flag sequences, if still present
		if (decoded[0] == HDLC_FRAME_DELIMITER)
		{
			decoded.remove((unsigned int)0, 1);
		}

		if (decoded[decoded.memlen()-1] == HDLC_FRAME_DELIMITER)
		{
			decoded.remove(decoded.memlen()-1, 1);
		}



		// now we have the (hopefully correct) decoded HDLC

		uint16_t f_crc = *((uint16_t*)&(decoded[(decoded.memlen() - 0) - 2]));

		decoded.remove(decoded.memlen() - 2, 2); // drop crc16 checksum at end of frame
		//decoded[decoded.memlen() - 1] = 0x00;
		//decoded[decoded.memlen() - 2] = 0x00;

	//	boost::crc_basic<16>  crc_ccitt1( 0x8404, 0xFFFF, 0xFFFF, false, false );
	//	crc_ccitt1.process_bytes( decoded.somem(), decoded.memlen() );

		uint16_t p_crc = 0xffff;
		p_crc = pppfcs16(p_crc, (unsigned char*)decoded.somem(), decoded.memlen());
		p_crc ^= 0xffff;

		if (f_crc != p_crc)
		{
			throw ePPPFrameHdlcDecodeFailed();
		}

		// remove HDLC-DST-ALL (0xff), if still present
		if (decoded[0] == HDLC_DST_ALL)
		{
			decoded.remove((unsigned int)0, 1);
		}

		// remove HDLC-DST-ALL (0x03), if still present
		if (decoded[0] == HDLC_PPP_CONTROL)
		{
			decoded.remove((unsigned int)0, 1);
		}

		//*dynamic_cast<cpacket*>( this ) = decoded;

		// parse PPP packet
		//validate();

	} catch (...) {
		logging::error << "[rofl][frame][ppp][hdlc-decode] ePacketOutOfRange" << std::endl;
	}
#endif
}


void
fpppframe::hdlc_encode(
		cmemory& encoded,
		uint32_t *accm_send)
{
	// 2 bytes HDLC_DST_ALL and HDLC_PPP_CONTROL
	// framelen() bytes payload
	// 2 bytes CRC16
	cmemory mem(4 + framelen());

	// set HDLC destination and PPP control field
	mem[0] = HDLC_DST_ALL;
	mem[1] = HDLC_PPP_CONTROL;

	// copy payload into mem
	memcpy(mem.somem() + 2, soframe(), framelen());

	// calculate CRC16
	uint16_t p_crc = 0xffff;
	p_crc = pppfcs16(p_crc, (unsigned char*)mem.somem(), 2 + framelen());
	p_crc ^= 0xffff;

	// append CRC16 to mem
	mem[2 /*HDLC dst + PPP ctrl*/+ framelen() + 0] =  p_crc & 0xff;
	mem[2 /*HDLC dst + PPP ctrl*/+ framelen() + 1] = (p_crc >> 8) & 0xff;

	encoded.resize(4 + 2 * mem.memlen());
#if 0
	encoded.stored_bytes(4 + 2 * mem.memlen());
#endif

	// insert HDLC flag sequence in encoded
	encoded[0] = HDLC_FRAME_DELIMITER;

	size_t mem_idx = 1;

	for (size_t idx = 0; idx < (4 + framelen()); ++idx) {

		if (accm_send[(mem[idx]/0x20)] & (1 << (mem[idx] % 0x20))) {
			encoded[mem_idx++] = HDLC_ESCAPE;
			encoded[mem_idx++] = (mem[idx] ^ 0x20);
		} else {
			encoded[mem_idx++] = mem[idx];
		}
	}

	encoded.remove(mem_idx + 1, (encoded.memlen() - (mem_idx + 1)));

	encoded[encoded.memlen() - 1] = HDLC_FRAME_DELIMITER;
}


uint16_t
fpppframe::pppfcs16(
		register uint16_t fcs,
		register unsigned char *cp,
		register int len)
{
    assert(sizeof (uint16_t) == 2);
    assert(((uint16_t) -1) > 0);
    while (len--)
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];

    return (fcs);
}



void
fpppframe::tryfcs16(
		register unsigned char *cp,
		register int len)
{
    uint16_t trialfcs;

    /* add on output */
    trialfcs = pppfcs16( PPPINITFCS16, cp, len );
    trialfcs ^= 0xffff;                 /* complement */
    cp[len] = (trialfcs & 0x00ff);      /* least significant byte first */
    cp[len+1] = ((trialfcs >> 8) & 0x00ff);


    /* check on input */
    trialfcs = pppfcs16( PPPINITFCS16, cp, len + 2 );
    if ( trialfcs == PPPGOODFCS16 )
        fprintf(stderr, "Good FCS\n");
}


void
fpppframe::validate(uint16_t total_len) const
{
#if 0
	if ((*this)[0] == HDLC_FRAME_DELIMITER)
	{
		hdlc_decode();
	}
#endif

	if (framelen() < sizeof(struct ppp_hdr_t))
		throw ePPPFrameInvalidSyntax();

	switch (be16toh(ppp_hdr->prot)) {
	case PPP_PROT_LCP:
		validate_lcp();
		break;
	case PPP_PROT_IPCP:
		validate_ipcp();
		break;
	default: // do nothing for other frame types, ... for now.
		logging::warn << "[rofl][frame][ppp][validate] unsuppported ppp-port:" << (int)be16toh(ppp_hdr->prot) << std::endl;
		break;
	}
}

void
fpppframe::validate_lcp() const
{
	if (framelen() < (sizeof(struct ppp_hdr_t) + sizeof(struct ppp_lcp_hdr_t)))
		throw ePPPFrameInvalidSyntax();

	if (framelen() < (be16toh(ppp_lcp_hdr->length) + sizeof(struct ppp_hdr_t)))
		throw ePPPFrameInvalidSyntax();


	switch (ppp_lcp_hdr->code) {
	case PPP_LCP_CONF_REQ:
	case PPP_LCP_CONF_ACK:
	case PPP_LCP_CONF_NAK:
	case PPP_LCP_CODE_REJ:
		//parse_lcp_options();
		break;
	default:
		break;
	}
}

void
fpppframe::parse_lcp_options()
{
	try {
		ppp_lcp_hdr = (struct ppp_lcp_hdr_t*)ppp_hdr->data;

		struct ppp_lcp_opt_hdr_t *opt = (struct ppp_lcp_opt_hdr_t*)ppp_lcp_hdr->data;
		size_t res_len = be16toh(ppp_lcp_hdr->length) - sizeof(struct ppp_lcp_hdr_t);

		while (not lcp_options.empty()) {
			delete lcp_options.begin()->second;
			lcp_options.erase(lcp_options.begin());
		}

		if (res_len == 0) // no options available
			return;

		while (res_len > 0) {
			if (res_len < sizeof(struct ppp_lcp_opt_hdr_t)) {
				return; // throw exception instead?
			}

			size_t opt_len = opt->length; // includes option and length field
			if ((opt_len > res_len) || (opt_len < sizeof(struct ppp_lcp_opt_hdr_t))) {
				return; // throw exception instead?
			}

			lcp_options[(enum ppp_lcp_option_t)opt->option] = new fppp_lcp_option(opt, opt_len);

			lcp_options[(enum ppp_lcp_option_t)opt->option]->validate();

			res_len -= opt_len;
			opt = (struct ppp_lcp_opt_hdr_t*)((uint8_t*)opt + opt_len);
		}

	} catch (ePPPLcpOptionInvalid& e) {
		throw ePPPFrameInvalidSyntax();
	}
}


void
fpppframe::validate_ipcp() const
{
	if (framelen() < (sizeof(struct ppp_hdr_t) + sizeof(struct ppp_lcp_hdr_t))) {
		throw ePPPFrameInvalidSyntax();
	}

	if (framelen() < (be16toh(ppp_ipcp_hdr->length) + sizeof(struct ppp_hdr_t))) {
		throw ePPPFrameInvalidSyntax();
	}


	switch (ppp_ipcp_hdr->code) {
	case PPP_IPCP_OPT_IPV4_DEP:
	case PPP_IPCP_OPT_IP_COMP:
	case PPP_IPCP_OPT_IPV4:
	case PPP_IPCP_OPT_MOB_IPV4:
	case PPP_IPCP_OPT_PRIM_DNS:
	case PPP_IPCP_OPT_PRIM_MBNS:
	case PPP_IPCP_OPT_SEC_DNS:
	case PPP_IPCP_OPT_SEC_MBNS: {
		//parse_ipcp_options();
	} break;
	default:
		break;
	}
}


void
fpppframe::parse_ipcp_options()
{
	try {
		ppp_ipcp_hdr = (struct ppp_lcp_hdr_t*)ppp_hdr->data;

		struct ppp_ipcp_opt_hdr_t *opt = (struct ppp_ipcp_opt_hdr_t*)ppp_ipcp_hdr->data;
		size_t res_len = be16toh(ppp_ipcp_hdr->length) - sizeof(struct ppp_lcp_hdr_t);

		while (not ipcp_options.empty()) {
			delete ipcp_options.begin()->second;
			ipcp_options.erase(ipcp_options.begin());
		}

		if (res_len == 0) // no options available
			return;

		while (res_len > 0) {

			if (res_len < sizeof(struct ppp_ipcp_opt_hdr_t)) {
				return; // throw exception instead?
			}

			size_t opt_len = opt->length; // includes option and length field
			if ((opt_len > res_len) || (opt_len < sizeof(struct ppp_lcp_opt_hdr_t))) {
				return; // throw exception instead?
			}

			ipcp_options[(enum ppp_ipcp_option_t)opt->option] = new fppp_ipcp_option(opt, opt_len);

			ipcp_options[(enum ppp_ipcp_option_t)opt->option]->validate();

			res_len -= opt_len;
			opt = (struct ppp_ipcp_opt_hdr_t*)((uint8_t*)opt + opt_len);
		}
	} catch (ePPPIpcpOptionInvalid& e) {

		logging::warn << "[rofl][frame][ppp][parse-ipcp-options] option validation failed" << std::endl;
		throw ePPPFrameInvalidSyntax();
	}
}


fppp_lcp_option*
fpppframe::lcp_option_find(
		enum ppp_lcp_option_t type) throw (ePPPFrameOptionNotFound)
{
	std::map<enum ppp_lcp_option_t, fppp_lcp_option*>::const_iterator it;
	if ((it = lcp_options.find(type)) == lcp_options.end())
		throw ePPPFrameOptionNotFound();
	return it->second;
}


fppp_ipcp_option*
fpppframe::ipcp_option_find(
		enum ppp_ipcp_option_t type) throw (ePPPFrameOptionNotFound)
{
	std::map<enum ppp_ipcp_option_t, fppp_ipcp_option*>::const_iterator it;
	if ((it = ipcp_options.find(type)) == ipcp_options.end())
		throw ePPPFrameOptionNotFound();
	return it->second;
}

#if 0
const char*
fpppframe::c_str()
{
	cvastring vas;

	std::string protstr;
	std::string codestr;
	switch (be16toh(ppp_hdr->prot)) {
	case PPP_PROT_LCP:
		protstr.assign("LCP");
		{
			switch (ppp_lcp_hdr->code) {
			case PPP_LCP_CONF_REQ:
				codestr.assign("CONF-REQ");
				break;
			case PPP_LCP_CONF_ACK:
				codestr.assign("CONF-ACK");
				break;
			case PPP_LCP_CONF_NAK:
				codestr.assign("CONF-NAK");
				break;
			case PPP_LCP_CONF_REJ:
				codestr.assign("CONF-REJ");
				break;
			case PPP_LCP_TERM_REQ:
				codestr.assign("TERM-REQ");
				break;
			case PPP_LCP_TERM_ACK:
				codestr.assign("TERM-ACK");
				break;
			case PPP_LCP_CODE_REJ:
				codestr.assign("CODE-REJ");
				break;
			case PPP_LCP_PROT_REJ:
				codestr.assign("PROT-REJ");
				break;
			case PPP_LCP_ECHO_REQ:
				codestr.assign("ECHO-REQ");
				break;
			case PPP_LCP_ECHO_REP:
				codestr.assign("ECHO-REP");
				break;
			case PPP_LCP_DISC_REQ:
				codestr.assign("DISC-REQ");
				break;
			default:
				codestr.assign("(FIXME)");
				break;
			}
		}
		break;
	case PPP_PROT_IPCP:
		protstr.assign("IPCP");
		if (NULL != ppp_ipcp_hdr) {
			switch (ppp_ipcp_hdr->code) {
			case PPP_IPCP_VEND_SPC:
				codestr.assign("VEND_SPC");
				break;
			case PPP_IPCP_CONF_REQ:
				codestr.assign("CONF_REQ");
				break;
			case PPP_IPCP_CONF_ACK:
				codestr.assign("CONF_ACK");
				break;
			case PPP_IPCP_CONF_NAK:
				codestr.assign("CONF_NAK");
				break;
			case PPP_IPCP_CONF_REJ:
				codestr.assign("CONF_REJ");
				break;
			case PPP_IPCP_TERM_REQ:
				codestr.assign("TERM_REQ");
				break;
			case PPP_IPCP_TERM_ACK:
				codestr.assign("TERM_ACK");
				break;
			case PPP_IPCP_CODE_REJ:
				codestr.assign("CODE_REJ");
				break;
			default:
				codestr.assign("(FIXME)");
				break;
			}
		} else {
			codestr.assign("(code not parsed yet)");
		}
		break;
	case PPP_PROT_IPV4:
		protstr.assign("IPV4");
		break;
	case PPP_PROT_CCP:
		protstr.assign("CCP");
		break;
	default:
		protstr.assign("(FIXME)");
		break;
	}

	info.assign(vas("[fpppframe(%p) PPP-%s-%s]",
			this,
			protstr.c_str(),
			codestr.c_str()));

	return info.c_str();
}
#endif

uint8_t*
fpppframe::payload() const throw (eFrameNoPayload)
{
	switch (be16toh(ppp_hdr->prot)) {
	case PPP_PROT_IPV4:
		return ppp_hdr->data;
	}
	return NULL;
}


size_t
fpppframe::payloadlen() const throw (eFrameNoPayload)
{
	switch (be16toh(ppp_hdr->prot)) {
	case PPP_PROT_IPV4:
		return framelen() - sizeof(struct ppp_hdr_t);
	}
	return 0;
}

void
fpppframe::payload_insert(
		uint8_t *data, size_t datalen) throw (eFrameOutOfRange)
{
	throw eNotImplemented();
}


uint16_t
fpppframe::get_ppp_prot() const
{
	return be16toh(ppp_hdr->prot);
}


void
fpppframe::set_ppp_prot(uint16_t prot)
{
	ppp_hdr->prot = htobe16(prot);
}


uint8_t
fpppframe::get_lcp_code() throw (ePPPLcpNotFound)
{
	if (0 == ppp_lcp_hdr) throw ePPPLcpNotFound();

	return ppp_lcp_hdr->code;
}


void
fpppframe::set_lcp_code(uint8_t code) throw (ePPPLcpNotFound)
{
	if (0 == ppp_lcp_hdr) throw ePPPLcpNotFound();

	ppp_lcp_hdr->code = code;
}


uint8_t
fpppframe::get_lcp_ident() throw (ePPPLcpNotFound)
{
	if (0 == ppp_lcp_hdr) throw ePPPLcpNotFound();

	return ppp_lcp_hdr->ident;
}


void
fpppframe::set_lcp_ident(uint8_t ident) throw (ePPPLcpNotFound)
{
	if (0 == ppp_lcp_hdr) throw ePPPLcpNotFound();

	ppp_lcp_hdr->ident = ident;
}


uint16_t
fpppframe::get_lcp_length() throw (ePPPLcpNotFound)
{
	if (0 == ppp_lcp_hdr) throw ePPPLcpNotFound();

	return be16toh(ppp_lcp_hdr->length);
}


void
fpppframe::set_lcp_length(uint16_t len) throw (ePPPLcpNotFound)
{
	if (0 == ppp_lcp_hdr) throw ePPPLcpNotFound();

	ppp_lcp_hdr->length = htobe16(len);
}


fppp_lcp_option*
fpppframe::get_lcp_option(enum ppp_lcp_option_t option) throw (ePPPLcpOptionNotFound)
{
	if (lcp_options.find(option) == lcp_options.end()) throw ePPPLcpOptionNotFound();

	return lcp_options[option];
}


uint8_t
fpppframe::get_ipcp_code() throw (ePPPIpcpNotFound)
{
	if (0 == ppp_ipcp_hdr) throw ePPPIpcpNotFound();

	return ppp_ipcp_hdr->code;
}


void
fpppframe::set_ipcp_code(uint8_t code) throw (ePPPIpcpNotFound)
{
	if (0 == ppp_ipcp_hdr) throw ePPPIpcpNotFound();

	ppp_ipcp_hdr->code = code;
}


uint8_t
fpppframe::get_ipcp_ident() throw (ePPPIpcpNotFound)
{
	if (0 == ppp_ipcp_hdr) throw ePPPIpcpNotFound();

	return ppp_ipcp_hdr->ident;
}


void
fpppframe::set_ipcp_ident(uint8_t ident) throw (ePPPIpcpNotFound)
{
	if (0 == ppp_ipcp_hdr) throw ePPPIpcpNotFound();

	ppp_ipcp_hdr->ident = ident;
}


uint16_t
fpppframe::get_ipcp_length() throw (ePPPIpcpNotFound)
{
	if (0 == ppp_ipcp_hdr) throw ePPPIpcpNotFound();

	return be16toh(ppp_ipcp_hdr->length);
}


void
fpppframe::set_ipcp_length(uint16_t len) throw (ePPPIpcpNotFound)
{
	if (0 == ppp_ipcp_hdr) throw ePPPIpcpNotFound();

	ppp_ipcp_hdr->length = htobe16(len);
}


fppp_ipcp_option*
fpppframe::get_ipcp_option(enum ppp_ipcp_option_t option) throw (ePPPIpcpOptionNotFound)
{
	if (ipcp_options.find(option) == ipcp_options.end()) throw ePPPIpcpOptionNotFound();

	return ipcp_options[option];
}


