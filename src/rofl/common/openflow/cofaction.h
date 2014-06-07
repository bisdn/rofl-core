/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFACTION_H
#define COFACTION_H 1

#include <vector>
#include <string>
#include <algorithm>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif


#include "rofl/common/croflexception.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/cmacaddr.h"
#include "rofl/common/caddress.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/coxmatch.h"
#include "rofl/common/logging.h"


namespace rofl {
namespace openflow {

/* error classes */
class eActionBase 			: public RoflException {};
class eActionInval 			: public eActionBase {}; // invalid parameter
class eActionInvalType 		: public eActionBase {}; // invalid action type



class cofaction {
public:

	/**
	 *
	 */
	cofaction(
			uint8_t ofp_version = 0,
			uint16_t type = 0,
			const rofl::cmemory& body = rofl::cmemory((size_t)0)) :
				ofp_version(ofp_version),
				type(type),
				len(sizeof(struct rofl::openflow::ofp_action) + body.length()),
				body(body) {};

	/**
	 *
	 */
	virtual
	~cofaction() {};

	/**
	 *
	 */
	cofaction(
			const cofaction& action) { *this = action; };

	/**
	 *
	 */
	cofaction&
	operator= (
			const cofaction& action) {
		if (this == &action)
			return *this;
		ofp_version 	= action.ofp_version;
		type			= action.type;
		len				= action.len;
		body			= action.body;
		return *this;
	};

	/**
	 *
	 */
	virtual void
	check_prerequisites() const {};

public:

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

	/**
	 *
	 */
	uint16_t
	get_type() const { return type; };

	/**
	 *
	 */
	void
	set_type(uint16_t type) { this->type = type; };

	/**
	 *
	 */
	uint16_t
	get_length() const { return len; };

	/**
	 *
	 */
	void
	set_length(uint16_t len) { this->len = len; };

	/**
	 *
	 */
	rofl::cmemory&
	set_body() { return body; };

	/**
	 *
	 */
	const rofl::cmemory&
	get_body() const { return body; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction const& action) {
		os << rofl::indent(0) << "<cofaction ";
			os << "ofp-version:" << (int)action.get_version() << " ";
			os << "type:" << (int)action.get_type() << " ";
			os << "length:" << (int)action.get_length() << " ";
		os << " >" << std::endl;
		if (action.get_body().length() > 0) {
			rofl::indent i(2); os << action.get_body();
		}
		return os;
	}

	class cofaction_find_by_type {
		uint16_t type;
	public:
		cofaction_find_by_type(uint16_t type) :
			type(type) {};
		bool operator() (const cofaction& action) {
			return (action.get_type() == type);
		};
		bool operator() (const cofaction* action) {
			return (action->get_type() == type);
		};
	};

private:

	uint8_t 		ofp_version;
	uint16_t 		type;
	uint16_t 		len;
	rofl::cmemory	body;
};




class cofaction_output : public cofaction {
public:

	/**
	 *
	 */
	cofaction_output(
			uint8_t ofp_version = 0,
			uint32_t port_no = 0,
			uint16_t max_len = 128) :
				cofaction(ofp_version, rofl::openflow::OFPAT_OUTPUT),
				port_no(port_no),
				max_len(max_len) {};

	/**
	 *
	 */
	virtual
	~cofaction_output() {};

	/**
	 *
	 */
	cofaction_output(
			 const cofaction_output& action) { *this = action; };

	/**
	 *
	 */
	cofaction_output&
	operator= (
			const cofaction_output& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		port_no		= action.port_no;
		max_len		= action.max_len;
		return *this;
	};

	/**
	 *
	 */
	virtual void
	check_prerequisites() const;

public:

	/**
	 *
	 */
	void
	set_port_no(uint32_t port_no) { this->port_no = port_no; };

	/**
	 *
	 */
	uint32_t
	get_port_no() const { return port_no; };

	/**
	 *
	 */
	void
	set_max_len(uint16_t max_len) { this->max_len = max_len; };

	/**
	 *
	 */
	uint16_t
	get_max_len() const { return max_len; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_output const& action) {
		os << rofl::indent(0) << "<cofaction_output ";
		os << std::hex;
		os << "port-no: 0x" << (unsigned int)action.get_port_no() << " ";
		os << "max-len: 0x" << (unsigned int)action.get_max_len() << " ";
		os << std::dec;
		os << ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint32_t	port_no;
	uint16_t	max_len;
};



/*
 * old OF1.0 actions
 */



class cofaction_set_vlan_vid : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_vlan_vid(
			uint8_t ofp_version = 0,
			uint16_t vlan_vid = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_VLAN_VID),
				vlan_vid(vlan_vid) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_vlan_vid() {};

	/**
	 *
	 */
	cofaction_set_vlan_vid(
			const cofaction_set_vlan_vid& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_vlan_vid&
	operator= (
			const cofaction_set_vlan_vid& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		vlan_vid = action.vlan_vid;
		return *this;
	};


public:

	/**
	 *
	 */
	void
	set_vlan_vid(uint16_t vlan_vid) { this->vlan_vid = vlan_vid; };

	/**
	 *
	 */
	uint16_t
	get_vlan_vid() const { return vlan_vid; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_vlan_vid const& action) {
		os << rofl::indent(2) << "<cofaction_set_vlan_vid ";
		os << "vid:" << (unsigned int)action.get_vlan_vid() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint16_t		vlan_vid;
};






class cofaction_set_vlan_pcp : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_vlan_pcp(
			uint8_t ofp_version = 0,
			uint8_t vlan_pcp = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_VLAN_PCP),
				vlan_pcp(vlan_pcp) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_vlan_pcp() {};

	/**
	 *
	 */
	cofaction_set_vlan_pcp(
			const cofaction_set_vlan_pcp& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_vlan_pcp&
	operator= (
			const cofaction_set_vlan_pcp& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		vlan_pcp = action.vlan_pcp;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_vlan_pcp(uint8_t vlan_pcp) { this->vlan_pcp = vlan_pcp; };

	/**
	 *
	 */
	uint8_t
	get_vlan_pcp() const { return vlan_pcp; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_vlan_pcp const& action) {
		os << rofl::indent(2) << "<cofaction_set_vlan_pcp ";
		os << "pcp:" << (unsigned int)action.get_vlan_pcp() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint8_t		vlan_pcp;
};






class cofaction_strip_vlan : public cofaction {
public:

	/**
	 *
	 */
	cofaction_strip_vlan(uint8_t ofp_version = 0) :
		cofaction(ofp_version, rofl::openflow::OFPAT_STRIP_VLAN) {};

	/**
	 *
	 */
	virtual
	~cofaction_strip_vlan() {};

	/**
	 *
	 */
	cofaction_strip_vlan(
			const cofaction_strip_vlan& action) { *this = action; };

	/**
	 *
	 */
	cofaction_strip_vlan&
	operator= (
			const cofaction_strip_vlan& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		return *this;
	};

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_strip_vlan& action) {
		os << rofl::indent(0) << "<cofaction_strip_vlan >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction&>( action );
		return os;
	};
};



class cofaction_set_dl_src : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_dl_src(
			uint8_t ofp_version = 0,
			const rofl::cmacaddr& macaddr = rofl::cmacaddr("00:00:00:00:00:00")) :
					cofaction(ofp_version, rofl::openflow::OFPAT_SET_DL_SRC),
					macaddr(macaddr) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_dl_src() {};

	/**
	 *
	 */
	cofaction_set_dl_src(
			const cofaction_set_dl_src& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_dl_src&
	operator= (
			const cofaction_set_dl_src& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		macaddr	= action.macaddr;
		return *this;
	};

public:

	/**
	 *
	 */
	const cmacaddr&
	get_dl_src() const { return macaddr; };

	/**
	 *
	 */
	cmacaddr&
	set_dl_src() { return macaddr; };

	/**
	 *
	 */
	void
	set_dl_src(const cmacaddr& macaddr) { this->macaddr = macaddr; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_set_dl_src& action) {
		os << rofl::indent(0) << "<cofaction_set_dl_src macaddr:";
		os << action.get_dl_src() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	rofl::cmacaddr		macaddr;
};



class cofaction_set_dl_dst : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_dl_dst(
			uint8_t ofp_version = 0,
			const rofl::cmacaddr& macaddr = rofl::cmacaddr("00:00:00:00:00:00")) :
					cofaction(ofp_version, rofl::openflow::OFPAT_SET_DL_DST),
					macaddr(macaddr) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_dl_dst() {};

	/**
	 *
	 */
	cofaction_set_dl_dst(
			const cofaction_set_dl_dst& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_dl_dst&
	operator= (
			const cofaction_set_dl_dst& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		macaddr	= action.macaddr;
		return *this;
	};

public:

	/**
	 *
	 */
	const cmacaddr&
	get_dl_dst() const { return macaddr; };

	/**
	 *
	 */
	cmacaddr&
	set_dl_dst() { return macaddr; };

	/**
	 *
	 */
	void
	set_dl_dst(const cmacaddr& macaddr) { this->macaddr = macaddr; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_set_dl_dst& action) {
		os << rofl::indent(0) << "<cofaction_set_dl_dst macaddr:";
		os << action.get_dl_dst() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	rofl::cmacaddr		macaddr;
};



class cofaction_set_nw_src : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_nw_src(
			uint8_t ofp_version = 0,
			const rofl::caddress& addr = rofl::caddress(AF_INET, "0.0.0.0")) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_NW_SRC),
				addr(addr) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_nw_src() {};

	/**
	 *
	 */
	cofaction_set_nw_src(
			const cofaction_set_nw_src& action) { *this = action };

	/**
	 *
	 */
	cofaction_set_nw_src&
	operator= (
			const cofaction_set_nw_src& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		addr = action.addr;
		return *this;
	};


public:

	/**
	 *
	 */
	const caddress&
	get_nw_src() const { return addr; };

	/**
	 *
	 */
	caddress&
	set_nw_src() { return addr; };

	/**
	 *
	 */
	void
	set_nw_src(const caddress& addr) { this->addr = addr; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_set_nw_src& action) {
		os << rofl::indent(0) << "<cofaction_set_nw_src ";
		os << "nw-src:" << action.get_nw_src() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	rofl::caddress		addr;
};



class cofaction_set_nw_dst : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_nw_dst(
			uint8_t ofp_version = 0,
			const rofl::caddress& addr = rofl::caddress(AF_INET, "0.0.0.0")) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_NW_DST),
				addr(addr) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_nw_dst() {};

	/**
	 *
	 */
	cofaction_set_nw_dst(
			const cofaction_set_nw_dst& action) { *this = action };

	/**
	 *
	 */
	cofaction_set_nw_dst&
	operator= (
			const cofaction_set_nw_dst& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		addr = action.addr;
		return *this;
	};


public:

	/**
	 *
	 */
	const caddress&
	get_nw_dst() const { return addr; };

	/**
	 *
	 */
	caddress&
	set_nw_dst() { return addr; };

	/**
	 *
	 */
	void
	set_nw_dst(const caddress& addr) { this->addr = addr; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_set_nw_dst& action) {
		os << rofl::indent(0) << "<cofaction_set_nw_dst ";
		os << "nw-dst:" << action.get_nw_dst() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	rofl::caddress		addr;
};




class cofaction_set_nw_tos : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_nw_tos(
			uint8_t ofp_version = 0,
			uint8_t nw_tos = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_NW_TOS),
				nw_tos(nw_tos) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_nw_tos() {};

	/**
	 *
	 */
	cofaction_set_nw_tos(
			const cofaction_set_nw_tos& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_nw_tos&
	operator= (
			const cofaction_set_nw_tos& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		nw_tos = action.nw_tos;
		return *this;
	}

public:

	/**
	 *
	 */
	void
	set_nw_tos(uint8_t nw_tos) { this->nw_tos = nw_tos; };

	/**
	 *
	 */
	uint8_t
	get_nw_tos() const { return nw_tos; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_nw_tos const& action) {
		os << rofl::indent(0) << "<cofaction_set_nw_tos ";
		os << "nw-tos:" << (unsigned int)action.get_nw_tos() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint8_t		nw_tos;
};



class cofaction_set_tp_src : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_tp_src(
			uint8_t ofp_version = 0,
			uint16_t tp_src = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_TP_SRC),
				tp_src(tp_src) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_tp_src() {};

	/**
	 *
	 */
	cofaction_set_tp_src(
			const cofaction_set_tp_src& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_tp_src&
	operator= (
			const cofaction_set_tp_src& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		tp_src = action.tp_src;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_tp_src(uint16_t tp_src) { this->tp_src = tp_src; };

	/**
	 *
	 */
	uint16_t
	get_tp_src() const { return tp_src; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_set_tp_src& action) {
		os << rofl::indent(0) << "<cofaction_set_tp_src ";
		os << "tp-src:" << (unsigned int)action.get_tp_src() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint16_t	tp_src;
};



class cofaction_set_tp_dst : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_tp_dst(
			uint8_t ofp_version = 0,
			uint16_t tp_dst = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_TP_DST),
				tp_dst(tp_dst) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_tp_dst() {};

	/**
	 *
	 */
	cofaction_set_tp_dst(
			const cofaction_set_tp_dst& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_tp_dst&
	operator= (
			const cofaction_set_tp_dst& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		tp_dst = action.tp_dst;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_tp_dst(uint16_t tp_dst) { this->tp_dst = tp_dst; };

	/**
	 *
	 */
	uint16_t
	get_tp_dst() const { return tp_dst; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_set_tp_dst& action) {
		os << rofl::indent(0) << "<cofaction_set_tp_dst ";
		os << "tp-dst:" << (unsigned int)action.get_tp_dst() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint16_t	tp_dst;
};



class cofaction_enqueue : public cofaction {
public:

	/**
	 *
	 */
	cofaction_enqueue(
			uint8_t ofp_version = 0,
			uint16_t port_no = 0,
			uint32_t queue_id = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_QUEUE),
				port_no(port_no),
				queue_id(queue_id) {};

	/**
	 *
	 */
	virtual
	~cofaction_enqueue() {};

	/**
	 *
	 */
	cofaction_enqueue(
			const cofaction_enqueue& action) { *this = action; };

	/**
	 *
	 */
	cofaction_enqueue&
	operator= (
			const cofaction_enqueue& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		port_no 	= action.port_no;
		queue_id 	= action.queue_id;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_port_no(uint16_t port_no) { this->port_no = port_no; };

	/**
	 *
	 */
	uint16_t
	get_port() const { return port_no; };

	/**
	 *
	 */
	void
	set_queue_id(uint32_t queue_id) { this->queue_id = queue_id; };

	/**
	 *
	 */
	uint32_t
	get_queue_id() const { return queue_id; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_enqueue& action) {
		os << rofl::indent(0) << "<cofaction_enqueue ";
		os << "port-no: " << (unsigned int)action.get_port() << " ";
		os << "queue-id: " << (unsigned int)action.get_queue_id() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint16_t	port_no;
	uint32_t	queue_id;
};



class cofaction_vendor : public cofaction {
public:

	/**
	 *
	 */
	cofaction_vendor(
			uint8_t ofp_version = 0,
			uint32_t exp_id = 0,
			rofl::cmemory& exp_body = rofl::cmemory((size_t)0)) :
				cofaction(ofp_version, rofl::openflow::OFPAT_EXPERIMENTER),
				exp_id(exp_id),
				exp_body(exp_body) {};

	/**
	 *
	 */
	virtual
	~cofaction_vendor() {};

	/**
	 *
	 */
	cofaction_vendor(
			const cofaction_vendor& action) { *this = action; };

	/**
	 *
	 */
	cofaction_vendor&
	operator= (
			const cofaction_vendor& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		exp_id		= action.exp_id;
		exp_body	= action.exp_body;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_exp_id(uint32_t exp_id) { this->exp_id = exp_id; };

	/**
	 *
	 */
	uint32_t
	get_exp_id() const { return exp_id; };

	/**
	 *
	 */
	const rofl::cmemory&
	get_exp_body() const { return exp_body; };

	/**
	 *
	 */
	rofl::cmemory&
	set_exp_body() { return exp_body; };

	/**
	 * @brief 	Shadow intentionally cofaction::get_body()
	 */
	const rofl::cmemory&
	get_body() const { return exp_body; };

	/**
	 * @brief 	Shadow intentionally cofaction::set_body()
	 */
	rofl::cmemory&
	set_body() { return exp_body; };

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_vendor& action) {
		os << rofl::indent(0) << "<cofaction_vendor ";
		os << "exp-id:" << (unsigned int)action.get_exp_id() << " >" << std::endl;
		rofl::indent i(2);
		os << action.get_exp_body();
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint32_t		exp_id;
	rofl::cmemory	exp_body;
};



#if 0


/*
 * new OF1.2 actions
 */

/** OFPAT_SET_MPLS_TTL
 *
 */
class cofaction_set_mpls_ttl : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_mpls_ttl(
			uint8_t ofp_version,
			uint8_t mpls_ttl) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_mpls_ttl));
			oac_12mpls_ttl->type 		= htobe16(openflow12::OFPAT_SET_MPLS_TTL);
			oac_12mpls_ttl->len 		= htobe16(sizeof(struct openflow12::ofp_action_mpls_ttl));
			oac_12mpls_ttl->mpls_ttl 	= mpls_ttl;
		} break;
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow13::ofp_action_mpls_ttl));
			oac_12mpls_ttl->type 		= htobe16(openflow13::OFPAT_SET_MPLS_TTL);
			oac_12mpls_ttl->len 		= htobe16(sizeof(struct openflow13::ofp_action_mpls_ttl));
			oac_12mpls_ttl->mpls_ttl 	= mpls_ttl;
		} break;
		default:
			logging::warn << "cofaction_set_mpls_ttl: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_set_mpls_ttl(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_mpls_ttl() {};

	/**
	 *
	 */
	void
	set_mpls_ttl(uint8_t mpls_ttl) {
		oac_12mpls_ttl->mpls_ttl = mpls_ttl;
	};

	/**
	 *
	 */
	uint8_t
	get_mpls_ttl() const {
		return oac_12mpls_ttl->mpls_ttl;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_mpls_ttl const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_set_mpls_ttl ";
			os << "mpls-ttl:" << (int)action.get_mpls_ttl() << " >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type SET-MPLS-TTL not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};


/** OFPAT_DEC_MPLS_TTL
 *
 */
class cofaction_dec_mpls_ttl : public cofaction {
public:
	/** constructor
	 */
	cofaction_dec_mpls_ttl(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_header));
			oac_header->type 	= htobe16(openflow12::OFPAT_DEC_MPLS_TTL);
			oac_header->len 	= htobe16(sizeof(struct openflow12::ofp_action_header));
		} break;
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow13::ofp_action_header));
			oac_header->type 	= htobe16(openflow13::OFPAT_DEC_MPLS_TTL);
			oac_header->len 	= htobe16(sizeof(struct openflow13::ofp_action_header));
		} break;
		default:
			logging::warn << "cofaction_dec_mpls_ttl: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_dec_mpls_ttl(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_dec_mpls_ttl() {};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_dec_mpls_ttl const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_dec_mpls_ttl >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type DEC-MPLS-TTL not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};


/** OFPAT_PUSH_VLAN
 *
 */
class cofaction_push_vlan : public cofaction {
public:
	/** constructor
	 */
	cofaction_push_vlan(
			uint8_t ofp_version,
			uint16_t ethertype) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_push));
			oac_12push->type 		= htobe16(openflow12::OFPAT_PUSH_VLAN);
			oac_12push->len 		= htobe16(sizeof(struct openflow12::ofp_action_push));
			oac_12push->ethertype 	= htobe16(ethertype);
		} break;
		default:
			logging::warn << "cofaction_push_vlan: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_push_vlan(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_push_vlan() {};

	/**
	 *
	 */
	void
	set_eth_type(uint16_t ethertype) {
		oac_12push->ethertype = htobe16(ethertype);
	};

	/**
	 *
	 */
	uint16_t
	get_eth_type() const {
		return be16toh(oac_12push->ethertype);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_push_vlan const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_push_vlan ";
			os << "eth-type:0x" << std::hex << (int)action.get_eth_type() << std::dec << " >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type PUSH-VLAN not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};


/** OFPAT_PUSH_MPLS
 *
 */
class cofaction_push_mpls : public cofaction {
public:
	/** constructor
	 */
	cofaction_push_mpls(
			uint8_t ofp_version,
			uint16_t ethertype) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_push));
			oac_12push->type 		= htobe16(openflow12::OFPAT_PUSH_MPLS);
			oac_12push->len 		= htobe16(sizeof(struct openflow12::ofp_action_push));
			oac_12push->ethertype 	= htobe16(ethertype);
		} break;
		default:
			logging::warn << "cofaction_push_mpls: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_push_mpls(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_push_mpls() {};

	/**
	 *
	 */
	void
	set_eth_type(uint16_t ethertype) {
		oac_12push->ethertype = htobe16(ethertype);
	};

	/**
	 *
	 */
	uint16_t
	get_eth_type() const {
		return be16toh(oac_12push->ethertype);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_push_mpls const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_push_mpls ";
			os << "eth-type:0x" << std::hex << (int)action.get_eth_type() << std::dec << " >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type PUSH-MPLS not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};


/** OFPAT_POP_VLAN
 *
 */
class cofaction_pop_vlan : public cofaction {
public:
	/** constructor
	 */
	cofaction_pop_vlan(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_header));
			oac_header->type 	= htobe16(openflow12::OFPAT_POP_VLAN);
			oac_header->len 	= htobe16(sizeof(struct openflow12::ofp_action_header));
		} break;
		default:
			logging::warn << "cofaction_pop_vlan: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_pop_vlan(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_pop_vlan() {};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_pop_vlan const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_pop_vlan >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type POP-VLAN not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};


/** OFPAT_POP_MPLS
 *
 */
class cofaction_pop_mpls : public cofaction {
public:
	/** constructor
	 */
	cofaction_pop_mpls(
			uint8_t ofp_version,
			uint16_t ethertype) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_pop_mpls));
			oac_12pop_mpls->type 		= htobe16(openflow12::OFPAT_POP_MPLS);
			oac_12pop_mpls->len 		= htobe16(sizeof(struct openflow12::ofp_action_pop_mpls));
			oac_12pop_mpls->ethertype 	= htobe16(ethertype);
		} break;
		default:
			logging::warn << "cofaction_pop_mpls: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_pop_mpls(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_pop_mpls() {};

	/**
	 *
	 */
	void
	set_eth_type(uint16_t ethertype) {
		oac_12pop_mpls->ethertype = htobe16(ethertype);
	};

	/**
	 *
	 */
	uint16_t
	get_eth_type() const {
		return be16toh(oac_12pop_mpls->ethertype);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_pop_mpls const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_pop_mpls ";
			os << "eth-type:0x" << std::hex << (int)action.get_eth_type() << std::dec << " >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type POP-MPLS not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};



/** OFPAT_GROUP
 *
 */
class cofaction_group : public cofaction {
public:
	/** constructor
	 */
	cofaction_group(
			uint8_t ofp_version,
			uint32_t group_id) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_group));
			oac_12group->type 		= htobe16(openflow12::OFPAT_GROUP);
			oac_12group->len 		= htobe16(sizeof(struct openflow12::ofp_action_group));
			oac_12group->group_id 	= htobe32(group_id);
		} break;
		default:
			logging::warn << "cofaction_group: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_group(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_group() {};

	/**
	 *
	 */
	void
	set_group_id(uint32_t group_id) {
		oac_12group->group_id = htobe32(group_id);
	};

	/**
	 *
	 */
	uint32_t
	get_group_id() const {
		return be32toh(oac_12group->group_id);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_group const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_group ";
			os << "group-id:" << (int)action.get_group_id() << " >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type GROUP not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};


/** OFPAT_SET_NW_TTL
 *
 */
class cofaction_set_nw_ttl : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_nw_ttl(
			uint8_t ofp_version,
			uint8_t nw_ttl) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_nw_ttl));
			oac_12nw_ttl->type 		= htobe16(openflow12::OFPAT_SET_NW_TTL);
			oac_12nw_ttl->len 		= htobe16(sizeof(struct openflow12::ofp_action_nw_ttl));
			oac_12nw_ttl->nw_ttl 	= nw_ttl;
		} break;
		default:
			logging::warn << "cofaction_set_nw_ttl: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}

	};

	/** constructor
	 */
	cofaction_set_nw_ttl(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_nw_ttl() {};

	/**
	 *
	 */
	void
	set_nw_ttl(uint8_t nw_ttl) {
		oac_12nw_ttl->nw_ttl = nw_ttl;
	};

	/**
	 *
	 */
	uint8_t
	get_nw_ttl() const {
		return oac_12nw_ttl->nw_ttl;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_nw_ttl const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_set_nw_ttl ";
			os << "nw-ttl:" << (int)action.get_nw_ttl() << " >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type SET-NW-TTL not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};


/** OFPAT_DEC_NW_TTL
 *
 */
class cofaction_dec_nw_ttl : public cofaction {
public:
	/** constructor
	 */
	cofaction_dec_nw_ttl(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_header));
			oac_header->type 	= htobe16(openflow12::OFPAT_DEC_NW_TTL);
			oac_header->len 	= htobe16(sizeof(struct openflow12::ofp_action_header));
		} break;
		default:
			logging::warn << "cofaction_dec_nw_ttl: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_dec_nw_ttl(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_dec_nw_ttl() {};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_dec_nw_ttl const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_dec_nw_ttl >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type DEC-NW-TTL not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};


/** OFPAT_COPY_TTL_OUT
 *
 */
class cofaction_copy_ttl_out : public cofaction {
public:
	/** constructor
	 */
	cofaction_copy_ttl_out(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_header));
			oac_header->type 	= htobe16(openflow12::OFPAT_COPY_TTL_OUT);
			oac_header->len 	= htobe16(sizeof(struct openflow12::ofp_action_header));
		} break;
		default:
			logging::warn << "cofaction_copy_ttl_out: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_copy_ttl_out(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_copy_ttl_out() {};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_copy_ttl_out const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_copy_ttl_out >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type COPY-TTL-OUT not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};


/** OFPAT_COPY_TTL_IN
 *
 */
class cofaction_copy_ttl_in : public cofaction {
public:
	/** constructor
	 */
	cofaction_copy_ttl_in(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_header));
			oac_header->type	= htobe16(openflow12::OFPAT_COPY_TTL_IN);
			oac_header->len 	= htobe16(sizeof(struct openflow12::ofp_action_header));
		} break;
		default:
			logging::warn << "cofaction_copy_ttl_in: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}

	};

	/** constructor
	 */
	cofaction_copy_ttl_in(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_copy_ttl_in() {};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_copy_ttl_in const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_copy_ttl_in >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type COPY-TTL-IN not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};



/** OFPAT_SET_QUEUE
 *
 */
class cofaction_set_queue : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_queue(
			uint8_t ofp_version,
			uint32_t queue_id) :
				cofaction(ofp_version, sizeof(struct openflow12::ofp_action_set_queue))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_set_queue));
			oac_12set_queue->type 		= htobe16(openflow12::OFPAT_SET_QUEUE);
			oac_12set_queue->len 		= htobe16(sizeof(struct openflow12::ofp_action_set_queue));
			oac_12set_queue->queue_id 	= htobe32(queue_id);
		} break;
		default:
			logging::warn << "cofaction_set_queue: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_set_queue(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_queue() {};

	/**
	 *
	 */
	void
	set_queue_id(uint32_t queue_id) {
		oac_12set_queue->queue_id = htobe32(queue_id);
	};

	/**
	 *
	 */
	uint32_t
	get_queue_id() const {
		return be32toh(oac_12set_queue->queue_id);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_queue const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_set_queue ";
			os << "queue-id:" << (int)action.get_queue_id() << " >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type SET-QUEUE not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};


/** OFPAT_SET_FIELD
 *
 */
class cofaction_set_field : public cofaction {
public:
	/** constructor
	 *
	 */
	cofaction_set_field(
			uint8_t ofp_version,
			coxmatch const& oxm) :
				cofaction(ofp_version, 0)
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			size_t total_length = 2 * sizeof(uint16_t) + oxm.length();

			size_t pad = (0x7 & total_length);

			/* append padding if not a multiple of 8 */
			if (pad) {
				total_length += 8 - pad;
			}

			action.resize(total_length);

			oac_12set_field 		= (struct openflow12::ofp_action_set_field*)action.somem();
			oac_12set_field->type 	= htobe16(openflow12::OFPAT_SET_FIELD);
			oac_12set_field->len 	= htobe16(total_length);

			memcpy(oac_12set_field->field, (void*)oxm.somem(), oxm.memlen());
		} break;
		default:
			logging::warn << "cofaction_set_field: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_set_field(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_field() {};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_field const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_set_field >" << std::endl;
#if 0
			coxmatch oxm((struct openflow::ofp_oxm_hdr*)(action.oac_12set_field->field),
					be16toh(action.oac_12set_field->len) - 4*sizeof(uint8_t));
#endif
			indent i(4);
			os << coxmatch_output(action.get_oxm());
		} break;
		default: {
			os << indent(2) << "<action type SET-FIELD not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};




/** OFPAT_EXPERIMENTER
 *
 */
class cofaction_experimenter : public cofaction {
public:
	/** constructor
	 */
	cofaction_experimenter(
			uint8_t ofp_version,
			uint32_t exp_id,
			uint32_t exp_type,
			size_t datalen) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_experimenter_header) + datalen);
			oac_12experimenter->type 			= htobe16(openflow12::OFPAT_EXPERIMENTER);
			oac_12experimenter->len 			= htobe16(sizeof(struct openflow12::ofp_action_experimenter_header) + datalen);
			oac_12experimenter->experimenter 	= htobe32(exp_id);
			oac_12experimenter->type			= htobe32(exp_type);
		} break;
		default:
			logging::warn << "cofaction_experimenter: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_experimenter(
			uint8_t ofp_version,
			uint32_t exp_id,
			uint32_t exp_type,
			uint8_t *data = (uint8_t*)0, size_t datalen = 0) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_experimenter_header) + datalen);
			oac_12experimenter->type 			= htobe16(openflow12::OFPAT_EXPERIMENTER);
			oac_12experimenter->len 			= htobe16(sizeof(struct openflow12::ofp_action_experimenter_header) + datalen);
			oac_12experimenter->experimenter 	= htobe32(exp_id);
			oac_12experimenter->exp_type		= htobe32(exp_type);

			if (data && datalen) {
				memcpy(oac_12experimenter->data, data, datalen);
			}
		} break;
		default:
			logging::warn << "cofaction_experimenter: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/**
	 * constructor
	 */
	cofaction_experimenter(cofaction const& action) :
		cofaction(action)
	{
		switch (action.get_version()) {
		case openflow12::OFP_VERSION: {
			if (openflow12::OFPAT_EXPERIMENTER != action.get_type())
				throw eActionInvalType();
		} break;
		case openflow13::OFP_VERSION: {
			if (openflow13::OFPAT_EXPERIMENTER != action.get_type())
				throw eActionInvalType();
		} break;
		default:
			logging::warn << "cofaction_experimenter: 'copy' constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}

	};

	/** destructor
	 */
	virtual
	~cofaction_experimenter() {};

	/**
	 *
	 */
	void
	set_exp_id(uint32_t exp_id) const {
		oac_12experimenter->experimenter = htobe32(exp_id);
	};

	/**
	 *
	 */
	uint32_t
	get_exp_id() const {
		return be32toh(oac_12experimenter->experimenter);
	};

	/**
	 *
	 */
	void
	set_exp_type(uint32_t exp_type) {
		oac_12experimenter->exp_type = htobe32(exp_type);
	};

	/**
	 *
	 */
	uint32_t
	get_exp_type() const {
		return be32toh(oac_12experimenter->exp_type);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_experimenter const& action) {
		switch (action.get_version()) {
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << dynamic_cast<cofaction const&>( action );
			os << indent(2) << "<cofaction_experimenter ";
			os << "exp-id:" << (int)action.get_exp_id() << " ";
			os << "exp-type:" << (int)action.get_exp_type() << " >" << std::endl;
		} break;
		default: {
			os << indent(2) << "<action type EXPERIMENTER not supported by OF version:"
					<< action.get_version() << " >" << std::endl;
		};
		}
		return os;
	};
};

#endif

}; // end of namespace openflow
}; // end of namespace rofl

#endif // COFACTION_H
