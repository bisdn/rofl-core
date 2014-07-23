/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * fl2tpframe.cc
 *
 *  Created on: 21.11.2013
 *      Author: andreas
 */

#include "fl2tpv3frame.h"

using namespace rofl;

fl2tpv3frame::fl2tpv3frame(
			enum l2tp_encap_t encap_type,
			uint8_t *data,
			size_t datalen) :
					fframe(data, datalen)
{
	switch (encap_type) {
	case L2TP_ENCAP_IP:
	case L2TP_ENCAP_UDP: {
		this->encap_type = encap_type;
	} break;
	default:
		throw eL2tpFrameInval(); // invalid encapsulation mode
	}
	l2tpv3_hdr = soframe();
}


fl2tpv3frame::fl2tpv3frame(
		enum l2tp_encap_t encap_type,
		size_t len) :
				fframe(len)
{
	switch (encap_type) {
	case L2TP_ENCAP_IP:
	case L2TP_ENCAP_UDP: {
		this->encap_type = encap_type;
	} break;
	default:
		throw eL2tpFrameInval(); // invalid encapsulation mode
	}
	l2tpv3_hdr = soframe();
}


fl2tpv3frame::~fl2tpv3frame()
{
	// do _NOT_ delete or deallocate (data,datalen) here!
}


void
fl2tpv3frame::reset(
		enum l2tp_encap_t encap_type,
		uint8_t* data,
		size_t datalen)
{
	switch (encap_type) {
	case L2TP_ENCAP_IP:
	case L2TP_ENCAP_UDP: {
		this->encap_type = encap_type;
	} break;
	default:
		throw eL2tpFrameInval(); // invalid encapsulation mode
	}
	fframe::reset(data, datalen);
	initialize();
}


uint8_t*
fl2tpv3frame::sopdu() const
{
	return soframe();
}


size_t
fl2tpv3frame::pdulen() const
{
	return framelen();
}


uint8_t*
fl2tpv3frame::sosdu() const
{
	if (not complete())
		throw eL2tpFrameInval();

	// We have no notion of the surrounding L2TPv3 session, so we ignore potential cookie fields.
	// It is up to the calling entity to consider the session state.

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (is_ctl_msg()) {
			// control messages: return start of sequence of AVPs
			/* TODO */
		} else {
			// data messages: return start of data field (might be the cookie!)
			return l2tpv3_data_hdr_ip_encap->data;
		}
	} break;
	case L2TP_ENCAP_UDP: {
		if (is_ctl_msg()) {
			// control messages: return start of sequence of AVPs
			/* TODO */
		} else {
			// data messages: return start of data field (might be the cookie!)
			return l2tpv3_data_hdr_udp_encap->data;
		}
	} break;
	}
	return NULL;
}


size_t
fl2tpv3frame::sdulen() const
{
	if (not complete())
		throw eL2tpFrameInval();

	// We have no notion of the surrounding L2TPv3 session, so we ignore potential cookie fields.
	// It is up to the calling entity to consider the session state.

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (is_ctl_msg()) {
			// control messages: return length of sequence of AVPs
			return (get_length() - sizeof(struct l2tpv3_ctl_hdr_ip_encap_t));
		} else {
			// data messages: we estimate this value as overall frame length - data header
			return (framelen() - sizeof(struct l2tpv3_data_hdr_ip_encap_t));
		}
	} break;
	case L2TP_ENCAP_UDP: {
		if (is_ctl_msg()) {
			// control messages: return length of sequence of AVPs
			return (get_length() - sizeof(struct l2tpv3_ctl_hdr_udp_encap_t));
		} else {
			// data messages: we estimate this value as overall frame length - data header
			return (framelen() - sizeof(struct l2tpv3_data_hdr_udp_encap_t));
		}
	} break;
	}
	return 0;
}



bool
fl2tpv3frame::complete() const
{
	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		// minimum size: length of data header for IP encapsulation
		if (framelen() < sizeof(struct l2tpv3_data_hdr_ip_encap_t))
			return false;
		// control message: pseudo session id == 0
		uint32_t sid = be32toh(((struct l2tpv3_data_hdr_ip_encap_t*)(soframe()))->session_id);
		if (0 == sid) {
			if (framelen() < sizeof(struct l2tpv3_ctl_hdr_ip_encap_t))
				return false;
		}
	} break;
	case L2TP_ENCAP_UDP: {
		// minimum size: length of data header for UDP encapsulation (2*32bit)
		if (framelen() < sizeof(struct l2tpv3_data_hdr_udp_encap_t))
			return false;
		// control message: Tbit is true
		bool Tbit = (be16toh(((struct l2tpv3_data_hdr_udp_encap_t*)soframe())->ctlfield) & (1 << 15));
		if (true == Tbit) {
			if (framelen() < sizeof(struct l2tpv3_ctl_hdr_udp_encap_t))
				return false;
		}
	} break;
	}
	return true;
}



size_t
fl2tpv3frame::need_bytes() const
{
	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		// minimum size: length of data header for IP encapsulation
		if (framelen() < sizeof(struct l2tpv3_data_hdr_ip_encap_t))
			return sizeof(struct l2tpv3_data_hdr_ip_encap_t);
		// control message: pseudo session id == 0
		uint32_t sid = be32toh(((struct l2tpv3_data_hdr_ip_encap_t*)(soframe()))->session_id);
		if (0 == sid) {
			if (framelen() < sizeof(struct l2tpv3_ctl_hdr_ip_encap_t))
				return sizeof(struct l2tpv3_ctl_hdr_ip_encap_t);
			else
				if (framelen() < get_length())
					return (get_length() - framelen());
		// data message: session id != 0
		} else {
			if (framelen() < sizeof(struct l2tpv3_data_hdr_ip_encap_t))
				return sizeof(struct l2tpv3_data_hdr_ip_encap_t);
			else
				// we are unable to detect the real length, so return a pseudo value of 0
				return 0;
		}
	} break;
	case L2TP_ENCAP_UDP: {
		// minimum size: length of data header for UDP encapsulation (2*32bit)
		if (framelen() < sizeof(struct l2tpv3_data_hdr_udp_encap_t))
			return sizeof(struct l2tpv3_data_hdr_udp_encap_t);
		// control message: Tbit is true
		bool Tbit = (be16toh(((struct l2tpv3_data_hdr_udp_encap_t*)soframe())->ctlfield) & (1 << 15));
		if (true == Tbit) {
			if (framelen() < sizeof(struct l2tpv3_ctl_hdr_udp_encap_t))
				return sizeof(struct l2tpv3_ctl_hdr_udp_encap_t);
			else
				if (framelen() < get_length())
					return (get_length() - framelen());
		// data message: Tbit is false
		} else {
			if (framelen() < sizeof(struct l2tpv3_data_hdr_udp_encap_t))
				return sizeof(struct l2tpv3_data_hdr_udp_encap_t);
			else
				// we are unable to detect the real length, so return a pseudo value of 0
				return 0;
		}
	} break;
	}
	return 0;
}



void
fl2tpv3frame::validate(
		uint16_t total_len) const
{
	if (not complete())
		throw eFrameInvalidSyntax();
}


void
fl2tpv3frame::initialize()
{
	l2tpv3_hdr = soframe();
}


void
fl2tpv3frame::payload_insert(
		uint8_t *data, size_t datalen) throw (eFrameOutOfRange)
{
	throw eNotImplemented();
}


uint8_t*
fl2tpv3frame::payload() const throw (eFrameNoPayload)
{
	return sosdu();
}



size_t
fl2tpv3frame::payloadlen() const throw (eFrameNoPayload)
{
	return sdulen();
}


const char*
fl2tpv3frame::c_str()
{
	std::stringstream sstr;
	sstr << *this;
	info = sstr.str();
	return info.c_str();
}


std::string
fl2tpv3frame::get_encap() const
{
	switch (encap_type) {
	case L2TP_ENCAP_IP: 	return std::string("ip"); 		break;
	case L2TP_ENCAP_UDP: 	return std::string("udp"); 		break;
	default:				return std::string("unknown");	break;
	}
	return std::string(""); // just to make eclipse happy ...
}


bool
fl2tpv3frame::is_ctl_msg() const
{
	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		uint32_t sid = be32toh(((struct l2tpv3_data_hdr_ip_encap_t*)(soframe()))->session_id);
		return ((0 == sid) && (true == get_Tbit()));
	} break;
	case L2TP_ENCAP_UDP: {
		return (true == get_Tbit());
	} break;
	default:
		return false;
	}
}


bool
fl2tpv3frame::get_Tbit() const
{
	if (sizeof(uint16_t) > framelen())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		return (be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) & TBIT_FLAG);
	} break;
	case L2TP_ENCAP_UDP: {
		return (be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) & TBIT_FLAG);
	} break;
	}
	return false;
}


void
fl2tpv3frame::set_Tbit(bool Tbit)
{
	if (sizeof(uint16_t) > framelen())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (true == Tbit)
			l2tpv3_ctl_hdr_ip_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) | TBIT_FLAG);
		else
			l2tpv3_ctl_hdr_ip_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) & ~TBIT_FLAG);
		l2tpv3_ctl_hdr_ip_encap->pseudo_session_id = htobe32(0);
	} break;
	case L2TP_ENCAP_UDP: {
		if (true == Tbit)
			l2tpv3_ctl_hdr_udp_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) | TBIT_FLAG);
		else
			l2tpv3_ctl_hdr_udp_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) & ~TBIT_FLAG);
	} break;
	}
}


bool
fl2tpv3frame::get_Lbit() const
{
	if (sizeof(uint16_t) > framelen())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		return (be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) & LBIT_FLAG);
	} break;
	case L2TP_ENCAP_UDP: {
		return (be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) & LBIT_FLAG);
	} break;
	}
	return false;
}


void
fl2tpv3frame::set_Lbit(bool Lbit)
{
	if (sizeof(uint16_t) > framelen())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (true == Lbit)
			l2tpv3_ctl_hdr_ip_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) | LBIT_FLAG);
		else
			l2tpv3_ctl_hdr_ip_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) & ~LBIT_FLAG);
	} break;
	case L2TP_ENCAP_UDP: {
		if (true == Lbit)
			l2tpv3_ctl_hdr_udp_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) | LBIT_FLAG);
		else
			l2tpv3_ctl_hdr_udp_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) & ~LBIT_FLAG);
	} break;
	}
}


bool
fl2tpv3frame::get_Sbit() const
{
	if (sizeof(uint16_t) > framelen())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		return (be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) & SBIT_FLAG);
	} break;
	case L2TP_ENCAP_UDP: {
		return (be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) & SBIT_FLAG);
	} break;
	}
	return false;
}


void
fl2tpv3frame::set_Sbit(bool Sbit)
{
	if (sizeof(uint16_t) > framelen())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (true == Sbit)
			l2tpv3_ctl_hdr_ip_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) | SBIT_FLAG);
		else
			l2tpv3_ctl_hdr_ip_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) & ~SBIT_FLAG);
	} break;
	case L2TP_ENCAP_UDP: {
		if (true == Sbit)
			l2tpv3_ctl_hdr_udp_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) | SBIT_FLAG);
		else
			l2tpv3_ctl_hdr_udp_encap->ctlfield = htobe16(be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) & ~SBIT_FLAG);
	} break;
	}
}


uint8_t
fl2tpv3frame::get_version() const
{
	if (sizeof(uint16_t) > framelen())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (is_ctl_msg()) {
			return be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) & VERS_MASK;
		} else {
			return L2TP_VERSION_3; // IP encapsulation only supported by L2TPv3
		}
	} break;
	case L2TP_ENCAP_UDP: {
		if (is_ctl_msg()) {
			return be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) & VERS_MASK;
		} else {
			return be16toh(l2tpv3_data_hdr_udp_encap->ctlfield) & VERS_MASK;
		}
	} break;
	}
	return 0;
}


void
fl2tpv3frame::set_version(uint8_t version)
{
	if (sizeof(uint16_t) > framelen())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (is_ctl_msg()) {
			l2tpv3_ctl_hdr_ip_encap->ctlfield = htobe16((be16toh(l2tpv3_ctl_hdr_ip_encap->ctlfield) & ~VERS_MASK) | version);
		} else {
			return; // we cannot set the version in data messages for IP encapsulation
		}
	} break;
	case L2TP_ENCAP_UDP: {
		if (is_ctl_msg()) {
			l2tpv3_ctl_hdr_udp_encap->ctlfield = htobe16((be16toh(l2tpv3_ctl_hdr_udp_encap->ctlfield) & ~VERS_MASK) | version);
		} else {
			l2tpv3_data_hdr_udp_encap->ctlfield = htobe16((be16toh(l2tpv3_data_hdr_udp_encap->ctlfield) & ~VERS_MASK) | version);
		}
	} break;
	}
}


uint32_t
fl2tpv3frame::get_session_id() const
{
	if (not complete())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		return be32toh(l2tpv3_data_hdr_ip_encap->session_id);
	} break;
	case L2TP_ENCAP_UDP: {
		if (is_ctl_msg())
			throw eL2tpFrameNoDataMsg();
		return be32toh(l2tpv3_data_hdr_udp_encap->session_id);
	} break;
	}
	return 0;
}


void
fl2tpv3frame::set_session_id(uint32_t sid)
{
	if (not complete())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		l2tpv3_data_hdr_ip_encap->session_id = htobe32(sid);
	} break;
	case L2TP_ENCAP_UDP: {
		if (is_ctl_msg())
			throw eL2tpFrameNoDataMsg();

		l2tpv3_data_hdr_udp_encap->session_id = htobe32(sid);
	} break;
	}
}


uint16_t
fl2tpv3frame::get_length() const
{
	if (not complete())
		throw eL2tpFrameInComplete();


	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (not is_ctl_msg())
			throw eL2tpFrameNoCtlMsg();

		return be16toh(l2tpv3_ctl_hdr_ip_encap->len);
	} break;
	case L2TP_ENCAP_UDP: {
		return be16toh(l2tpv3_ctl_hdr_udp_encap->len);
	} break;
	}
	return 0;
}


void
fl2tpv3frame::set_length(uint16_t len)
{
	if (not complete())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (not is_ctl_msg())
			throw eL2tpFrameNoCtlMsg();

		l2tpv3_ctl_hdr_ip_encap->len = htobe16(len);
	} break;
	case L2TP_ENCAP_UDP: {
		l2tpv3_ctl_hdr_udp_encap->len = htobe16(len);
	} break;
	}
}


uint32_t
fl2tpv3frame::get_ccid() const
{
	if (not complete())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (not is_ctl_msg())
			throw eL2tpFrameNoCtlMsg();

		return be32toh(l2tpv3_ctl_hdr_ip_encap->ccid);
	} break;
	case L2TP_ENCAP_UDP: {
		return be32toh(l2tpv3_ctl_hdr_udp_encap->ccid);
	} break;
	}
	return 0;
}


void
fl2tpv3frame::set_ccid(uint32_t ccid)
{
	if (not complete())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (not is_ctl_msg())
			throw eL2tpFrameNoDataMsg();

		l2tpv3_ctl_hdr_ip_encap->ccid = htobe32(ccid);
	} break;
	case L2TP_ENCAP_UDP: {
		l2tpv3_ctl_hdr_udp_encap->ccid = htobe32(ccid);
	} break;
	}
}


uint16_t
fl2tpv3frame::get_Ns() const
{
	if (not complete())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (not is_ctl_msg())
			throw eL2tpFrameNoCtlMsg();

		return be16toh(l2tpv3_ctl_hdr_ip_encap->ns);
	} break;
	case L2TP_ENCAP_UDP: {
		return be16toh(l2tpv3_ctl_hdr_udp_encap->ns);
	} break;
	}
	return 0;
}


void
fl2tpv3frame::set_Ns(uint16_t Ns)
{
	if (not complete())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (not is_ctl_msg())
			throw eL2tpFrameNoCtlMsg();

		l2tpv3_ctl_hdr_ip_encap->ns = htobe16(Ns);
	} break;
	case L2TP_ENCAP_UDP: {
		l2tpv3_ctl_hdr_udp_encap->ns = htobe16(Ns);
	} break;
	}
}


uint16_t
fl2tpv3frame::get_Nr() const
{
	if (not complete())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (not is_ctl_msg())
			throw eL2tpFrameNoCtlMsg();

		return be16toh(l2tpv3_ctl_hdr_ip_encap->nr);
	} break;
	case L2TP_ENCAP_UDP: {
		return be16toh(l2tpv3_ctl_hdr_udp_encap->nr);
	} break;
	}
	return 0;
}


void
fl2tpv3frame::set_Nr(uint16_t Nr)
{
	if (not complete())
		throw eL2tpFrameInComplete();

	switch (encap_type) {
	case L2TP_ENCAP_IP: {
		if (not is_ctl_msg())
			throw eL2tpFrameNoCtlMsg();

		l2tpv3_ctl_hdr_ip_encap->nr = htobe16(Nr);
	} break;
	case L2TP_ENCAP_UDP: {
		l2tpv3_ctl_hdr_udp_encap->nr = htobe16(Nr);
	} break;
	}
}


