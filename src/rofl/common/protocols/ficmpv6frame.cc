/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ficmpv6frame.h"

using namespace rofl;


ficmpv6frame::ficmpv6frame(
		uint8_t* data,
		size_t datalen) :
		fframe(data, datalen),
		data(0),
		datalen(0)
{
	icmpv6_hdr = (struct icmpv6_hdr_t*)soframe();
	initialize();
}



ficmpv6frame::ficmpv6frame(
		size_t len) :
		fframe(len),
		data(0),
		datalen(0)
{
	icmpv6_hdr = (struct icmpv6_hdr_t*)soframe();
	initialize();
}



ficmpv6frame::~ficmpv6frame()
{

}



ficmpv6opt&
ficmpv6frame::get_option(ficmpv6opt::icmpv6_option_type_t type) throw (eICMPv6FrameNotFound)
{
	if (icmpv6opts.find(type) == icmpv6opts.end()) {
		throw eICMPv6FrameNotFound();
	}
	return icmpv6opts[type];
}



void
ficmpv6frame::initialize()
{
	
	try {
		if (framelen() < sizeof(struct icmpv6_hdr_t)) {
			return;
		}

		icmpv6_hdr = (struct icmpv6_hdr_t*)soframe();
		
		switch (get_icmpv6_type()) {
		case ICMPV6_TYPE_DESTINATION_UNREACHABLE: {
			parse_icmpv6_dest_unreach();
		} break;
		case ICMPV6_TYPE_PACKET_TOO_BIG: {
			parse_icmpv6_pkt_too_big();
		} break;
		case ICMPV6_TYPE_TIME_EXCEEDED: {
			parse_icmpv6_time_exceeded();
		} break;
		case ICMPV6_TYPE_PARAMETER_PROBLEM: {
			parse_icmpv6_param_problem();
		} break;
		case ICMPV6_TYPE_ECHO_REQUEST: {
			parse_icmpv6_echo_request();
		} break;
		case ICMPV6_TYPE_ECHO_REPLY: {
			parse_icmpv6_echo_reply();
		} break;
		case ICMPV6_TYPE_ROUTER_SOLICATION: {
			parse_icmpv6_rtr_solicitation();
		} break;
		case ICMPV6_TYPE_ROUTER_ADVERTISEMENT: {
			parse_icmpv6_rtr_advertisement();
		} break;
		case ICMPV6_TYPE_NEIGHBOR_SOLICITATION: {
			parse_icmpv6_neighbor_solicitation();
		} break;
		case ICMPV6_TYPE_NEIGHBOR_ADVERTISEMENT: {
			parse_icmpv6_neighbor_advertisement();
		} break;
		case ICMPV6_TYPE_REDIRECT_MESSAGE: {
			parse_icmpv6_redirect();
		} break;
		default: {
			LOGGING_WARN << "[rofl][frame][icmpv6] unsupported ICMPv6 message type detected" << std::endl;
		} break;
		}

	} catch (eICMPv6FrameTooShort& e) {
		LOGGING_WARN << "[rofl][frame][icmpv6] ICMPv6 frame too short" << std::endl;
	}
}



void
ficmpv6frame::parse_icmpv6_options(
		struct ficmpv6opt::icmpv6_option_hdr_t *option, size_t optlen) throw (eICMPv6FrameTooShort)
{
	icmpv6opts.clear();
	if (((ficmpv6opt::icmpv6_option_hdr_t*)0 == option) || (0 == optlen)) {
		return;
	}
	int reslen = optlen;
	struct ficmpv6opt::icmpv6_option_hdr_t *nextopt = option;

	while (true) {
		if (reslen < (int)sizeof(struct ficmpv6opt::icmpv6_option_hdr_t)) {
			return;
		}
		if (0 == nextopt->len) {
			LOGGING_WARN << "[rofl][frame][icmpv6][parser] found invalid ICMPv6 option (len=0), ignoring" << std::endl;;
			return;
		}
		size_t optlen = 8 * nextopt->len; // length is measured in blocks of 8-octets
		if (reslen < (int)optlen) {
			LOGGING_WARN << "[rofl][frame][icmpv6][parser] found invalid ICMPv6 option (too short), ignoring" << std::endl;
			return;
		}

		icmpv6opts[(ficmpv6opt::icmpv6_option_type_t)(nextopt->type)] = ficmpv6opt(nextopt, optlen);
		nextopt = (struct ficmpv6opt::icmpv6_option_hdr_t*)(((uint8_t*)nextopt) + optlen);
		reslen -= optlen;
	}
}



inline void
ficmpv6frame::parse_icmpv6_dest_unreach() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_dest_unreach_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	data = icmpv6_dest_unreach->data;
	datalen = framelen() - sizeof(struct icmpv6_dest_unreach_hdr_t);
}



inline void
ficmpv6frame::parse_icmpv6_pkt_too_big() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_pkt_too_big_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	data = icmpv6_pkt_too_big->data;
	datalen = framelen() - sizeof(struct icmpv6_pkt_too_big_hdr_t);
}



inline void
ficmpv6frame::parse_icmpv6_time_exceeded() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_time_exceeded_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	data = icmpv6_time_exceeded->data;
	datalen = framelen() - sizeof(struct icmpv6_time_exceeded_hdr_t);
}



inline void
ficmpv6frame::parse_icmpv6_param_problem() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_param_problem_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	data = icmpv6_param_problem->data;
	datalen = framelen() - sizeof(struct icmpv6_param_problem_hdr_t);
}



inline void
ficmpv6frame::parse_icmpv6_echo_request() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_echo_request_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	data = icmpv6_echo_request->data;
	datalen = framelen() - sizeof(struct icmpv6_echo_request_hdr_t);
}



inline void
ficmpv6frame::parse_icmpv6_echo_reply() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_echo_reply_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	data = icmpv6_echo_reply->data;
	datalen = framelen() - sizeof(struct icmpv6_echo_reply_hdr_t);
}



inline void
ficmpv6frame::parse_icmpv6_rtr_solicitation() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_router_solicitation_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	parse_icmpv6_options(icmpv6_rtr_solicitation->options, framelen() - sizeof(struct icmpv6_router_solicitation_hdr_t));
}



inline void
ficmpv6frame::parse_icmpv6_rtr_advertisement() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_router_advertisement_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	parse_icmpv6_options(icmpv6_rtr_advertisement->options, framelen() - sizeof(struct icmpv6_router_advertisement_hdr_t));
}



inline void
ficmpv6frame::parse_icmpv6_neighbor_solicitation() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_neighbor_solicitation_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	parse_icmpv6_options(icmpv6_neighbor_solicitation->options, framelen() - sizeof(struct icmpv6_neighbor_solicitation_hdr_t));
}



inline void
ficmpv6frame::parse_icmpv6_neighbor_advertisement() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_neighbor_advertisement_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	parse_icmpv6_options(icmpv6_neighbor_advertisement->options, framelen() - sizeof(struct icmpv6_neighbor_advertisement_hdr_t));
}



inline void
ficmpv6frame::parse_icmpv6_redirect() throw (eICMPv6FrameTooShort)
{
	if (framelen() < sizeof(struct icmpv6_redirect_hdr_t)) {
		throw eICMPv6FrameTooShort();
	}
	parse_icmpv6_options(icmpv6_redirect->options, framelen() - sizeof(struct icmpv6_redirect_hdr_t));
}



bool
ficmpv6frame::complete() const
{
	//initialize();

	if (framelen() < sizeof(struct icmpv6_hdr_t))
		return false;

	return true;
}


size_t
ficmpv6frame::need_bytes() const
{
	if (complete())
		return 0;

	if (framelen() < sizeof(struct icmpv6_hdr_t))
		return (sizeof(struct icmpv6_hdr_t) - framelen());

	return 0; // just to make gcc happy
}


void
ficmpv6frame::payload_insert(
		uint8_t *data,
		size_t datalen) throw (eFrameOutOfRange)
{
	if (datalen > (framelen() - sizeof(struct icmpv6_hdr_t)))
	{
		throw eFrameOutOfRange();
	}
	memcpy(icmpv6_hdr->data, data, datalen);
	this->datalen = datalen;
}


uint8_t*
ficmpv6frame::payload() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!data)
		throw eFrameNoPayload();
	return data;
}


size_t
ficmpv6frame::payloadlen() const throw (eFrameNoPayload)
{
	//initialize(); // commented out 2012-12-13
	if (!datalen)
		throw eFrameNoPayload();
	return datalen;
}


void
ficmpv6frame::validate(uint16_t total_len) const
{
	//initialize();

	if (!complete())
	{
		throw eICMPv6FrameTooShort();
	}

	// TODO: verify checksum here?
}



void
ficmpv6frame::icmpv6_calc_checksum()
{
	initialize();

	throw eNotImplemented();
#if 0
	icmp_hdr->checksum = htobe16(0x0000);

	// ICMPv4 header
	//

	// sum
	uint32_t sum = 0;

	// pointer on 16bit words
	uint16_t *word16 = (uint16_t*)icmp_hdr;

	// number of 16bit words
	int wnum = (sizeof(struct icmpv4_hdr_t) / sizeof(uint16_t));

	// header loop
	for (int i = 0; i < wnum; i++)
	{
		uint32_t tmp = (uint32_t)(be16toh(word16[i]));
		sum += tmp;
		//fprintf(stderr, "word16[%d]=0x%08x sum()=0x%08x\n", i, tmp, sum);
	}

	// TODO: checksum must also cover data portion of ICMP message!

	uint16_t res16 = (sum & 0x0000ffff) + ((sum & 0xffff0000) >> 16);

	//fprintf(stderr, " res16(1)=0x%x\n", res16);

	icmp_hdr->checksum = htobe16(~res16);

	//fprintf(stderr, "~res16(1)=0x%x\n", be16toh(udp_hdr->checksum));
#endif
}


uint8_t
ficmpv6frame::get_icmpv6_code() const
{
	return icmpv6_hdr->code;
}


void
ficmpv6frame::set_icmpv6_code(uint8_t code)
{
	icmpv6_hdr->code = code;
}


uint8_t
ficmpv6frame::get_icmpv6_type() const
{
	return icmpv6_hdr->type;
}


void
ficmpv6frame::set_icmpv6_type(uint8_t type)
{
	icmpv6_hdr->type = type;
}




caddress_in6
ficmpv6frame::get_icmpv6_neighbor_taddr() const
{
	caddress_in6 addr;

	try {
		switch (get_icmpv6_type()) {
		case ICMPV6_TYPE_NEIGHBOR_SOLICITATION: {
			memcpy(addr.somem(), icmpv6_neighbor_solicitation->taddr, IPV6_ADDR_LEN);
		} break;
		case ICMPV6_TYPE_NEIGHBOR_ADVERTISEMENT: {
			memcpy(addr.somem(), icmpv6_neighbor_advertisement->taddr, IPV6_ADDR_LEN);
		} break;
		case ICMPV6_TYPE_REDIRECT_MESSAGE: {
			memcpy(addr.somem(), icmpv6_redirect->taddr, IPV6_ADDR_LEN);
		} break;
		default: {
			throw eICMPv6FrameInvalType();
		}
		}

	} catch (eICMPv6FrameInvalType& e) {

		LOGGING_WARN << "[rofl][frame][icmpv6][get-nb-taddr] invalid frame type" << std::endl;
	}

	return addr;
}



void
ficmpv6frame::set_icmpv6_neighbor_taddr(caddress_in6 const& addr)
{
	try {
		switch (get_icmpv6_type()) {
		case ICMPV6_TYPE_NEIGHBOR_SOLICITATION: {
			memcpy(icmpv6_neighbor_solicitation->taddr, addr.somem(), IPV6_ADDR_LEN);
		} break;
		case ICMPV6_TYPE_NEIGHBOR_ADVERTISEMENT: {
			memcpy(icmpv6_neighbor_advertisement->taddr, addr.somem(), IPV6_ADDR_LEN);
		} break;
		case ICMPV6_TYPE_REDIRECT_MESSAGE: {
			memcpy(icmpv6_redirect->taddr, addr.somem(), IPV6_ADDR_LEN);
		} break;
		default: {
			throw eICMPv6FrameInvalType();
		}
		}

	} catch (eICMPv6FrameInvalType& e) {

		LOGGING_WARN << "[rofl][frame][icmpv6][set-nb-taddr] invalid frame type" << std::endl;
	}
}










ficmpv6opt::ficmpv6opt(
		uint8_t *data,
		size_t datalen) :
				fframe(data, datalen)
{
	icmpv6_opt = (struct icmpv6_option_hdr_t*)data;
}



ficmpv6opt::ficmpv6opt(
		struct icmpv6_option_hdr_t *data,
		size_t datalen) :
				fframe((uint8_t*)data, datalen)
{
	icmpv6_opt = (struct icmpv6_option_hdr_t*)data;
}



ficmpv6opt::~ficmpv6opt()
{

}



ficmpv6opt::ficmpv6opt(
		ficmpv6opt const& opt) :
				fframe((size_t)opt.framelen())
{
	*this = opt;
}



ficmpv6opt&
ficmpv6opt::operator= (ficmpv6opt const& opt)
{
	if (this == &opt)
		return *this;

	fframe::operator= (opt);

	icmpv6_opt = (struct icmpv6_option_hdr_t*)soframe();

	return *this;
}




uint8_t
ficmpv6opt::get_opt_type() const
{
	return icmpv6_opt->type;
}



void
ficmpv6opt::set_opt_type(uint8_t type)
{
	icmpv6_opt->type = type;
}



cmacaddr
ficmpv6opt::get_ll_taddr() const
{
	if (ICMPV6_OPT_LLADDR_TARGET != icmpv6_opt->type) {
		throw eICMPv6FrameInvalType();
	}
	if (framelen() < sizeof(struct icmpv6_lla_option_t)) {
		throw eICMPv6FrameTooShort();
	}
	return cmacaddr(icmpv6_opt_lla->addr, ETHER_ADDR_LEN);
}



void
ficmpv6opt::set_ll_taddr(cmacaddr const& addr)
{
	if (framelen() < sizeof(struct icmpv6_lla_option_t)) {
		throw eICMPv6FrameTooShort();
	}
	memcpy(icmpv6_opt_lla->addr, addr.somem(), ETHER_ADDR_LEN);
}



cmacaddr
ficmpv6opt::get_ll_saddr() const
{
	if (ICMPV6_OPT_LLADDR_SOURCE != icmpv6_opt->type) {
		throw eICMPv6FrameInvalType();
	}
	if (framelen() < sizeof(struct icmpv6_lla_option_t)) {
		throw eICMPv6FrameTooShort();
	}
	return cmacaddr(icmpv6_opt_lla->addr, ETHER_ADDR_LEN);
}



void
ficmpv6opt::set_ll_saddr(cmacaddr const& addr)
{
	if (framelen() < sizeof(struct icmpv6_lla_option_t)) {
		throw eICMPv6FrameTooShort();
	}
	memcpy(icmpv6_opt_lla->addr, addr.somem(), ETHER_ADDR_LEN);
}



uint8_t
ficmpv6opt::get_pfx_on_link_flag() const
{
	if (ICMPV6_OPT_PREFIX_INFO != icmpv6_opt->type) {
		throw eICMPv6FrameInvalType();
	}
	if (framelen() < sizeof(struct icmpv6_prefix_info_t)) {
		throw eICMPv6FrameTooShort();
	}
	return ((icmpv6_opt_pfx->flags & 0x80) >> 7);
}



void
ficmpv6opt::set_pfx_on_link_flag(uint8_t flag)
{
	if (framelen() < sizeof(struct icmpv6_prefix_info_t)) {
		throw eICMPv6FrameTooShort();
	}
	icmpv6_opt_pfx->flags = (icmpv6_opt_pfx->flags & 0x7F) | ((flag & 0x01) << 7);
}



uint8_t
ficmpv6opt::get_pfx_aac_flag() const
{
	if (ICMPV6_OPT_PREFIX_INFO != icmpv6_opt->type) {
		throw eICMPv6FrameInvalType();
	}
	if (framelen() < sizeof(struct icmpv6_prefix_info_t)) {
		throw eICMPv6FrameTooShort();
	}
	return ((icmpv6_opt_pfx->flags & 0x40) >> 6);
}



void
ficmpv6opt::set_pfx_aac_flag(uint8_t flag)
{
	if (framelen() < sizeof(struct icmpv6_prefix_info_t)) {
		throw eICMPv6FrameTooShort();
	}
	icmpv6_opt_pfx->flags = (icmpv6_opt_pfx->flags & 0xBF) | ((flag & 0x01) << 6);
}


