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
			const cofaction_set_nw_src& action) { *this = action; };

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
			const cofaction_set_nw_dst& action) { *this = action; };

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
			const rofl::cmemory& exp_body = rofl::cmemory((size_t)0)) :
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



class cofaction_set_mpls_ttl : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_mpls_ttl(
			uint8_t ofp_version = 0,
			uint8_t mpls_ttl = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_MPLS_TTL),
				mpls_ttl(mpls_ttl) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_mpls_ttl() {};

	/**
	 *
	 */
	cofaction_set_mpls_ttl(
			const cofaction_set_mpls_ttl& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_mpls_ttl&
	operator= (
			const cofaction_set_mpls_ttl& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		mpls_ttl = action.mpls_ttl;
		return *this;
	}

public:

	/**
	 *
	 */
	void
	set_mpls_ttl(uint8_t mpls_ttl) { this->mpls_ttl = mpls_ttl; };

	/**
	 *
	 */
	uint8_t
	get_mpls_ttl() const { return mpls_ttl; };

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
	operator<< (std::ostream& os, const cofaction_set_mpls_ttl& action) {
		os << rofl::indent(0) << "<cofaction_set_mpls_ttl ";
		os << "mpls-ttl:" << (unsigned int)action.get_mpls_ttl() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint8_t		mpls_ttl;
};



class cofaction_dec_mpls_ttl : public cofaction {
public:

	/**
	 *
	 */
	cofaction_dec_mpls_ttl(
			uint8_t ofp_version = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_DEC_MPLS_TTL) {};

	/**
	 *
	 */
	virtual
	~cofaction_dec_mpls_ttl() {};

	/**
	 *
	 */
	cofaction_dec_mpls_ttl(
			const cofaction_dec_mpls_ttl& action) { *this = action; };

	/**
	 *
	 */
	cofaction_dec_mpls_ttl&
	operator= (
			const cofaction_dec_mpls_ttl& action) {
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
	operator<< (std::ostream& os, cofaction_dec_mpls_ttl const& action) {
		os << rofl::indent(0) << "<cofaction_dec_mpls_ttl >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};
};



class cofaction_push_vlan : public cofaction {
public:

	/**
	 *
	 */
	cofaction_push_vlan(
			uint8_t ofp_version = 0,
			uint16_t eth_type = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_PUSH_VLAN),
				eth_type(eth_type) {};

	/**
	 *
	 */
	virtual
	~cofaction_push_vlan() {};

	/**
	 *
	 */
	cofaction_push_vlan(
			const cofaction_push_vlan& action) { *this = action; };

	/**
	 *
	 */
	cofaction_push_vlan&
	operator= (
			const cofaction_push_vlan& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		eth_type = action.eth_type;
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

	/**
	 *
	 */
	void
	set_eth_type(uint16_t eth_type) { this->eth_type = eth_type; };

	/**
	 *
	 */
	uint16_t
	get_eth_type() const { return eth_type; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_push_vlan& action) {
		os << rofl::indent(0) << "<cofaction_push_vlan ";
		os << "eth-type:0x" << std::hex << (unsigned int)action.get_eth_type() << std::dec << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint16_t	eth_type;
};



class cofaction_pop_vlan : public cofaction {
public:

	/**
	 *
	 */
	cofaction_pop_vlan(uint8_t ofp_version = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_POP_VLAN) {};

	/**
	 *
	 */
	virtual
	~cofaction_pop_vlan() {};

	/**
	 *
	 */
	cofaction_pop_vlan(
			const cofaction_pop_vlan& action) { *this = action; };

	/**
	 *
	 */
	cofaction_pop_vlan&
	operator= (
			const cofaction_pop_vlan& action) {
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
	operator<< (std::ostream& os, const cofaction_pop_vlan& action) {
		os << rofl::indent(0) << "<cofaction_pop_vlan >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};
};



class cofaction_push_mpls : public cofaction {
public:

	/**
	 *
	 */
	cofaction_push_mpls(
			uint8_t ofp_version = 0,
			uint16_t eth_type = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_PUSH_VLAN),
				eth_type(eth_type) {};

	/**
	 *
	 */
	virtual
	~cofaction_push_mpls() {};

	/**
	 *
	 */
	cofaction_push_mpls(
			const cofaction_push_mpls& action) { *this = action; };

	/**
	 *
	 */
	cofaction_push_mpls&
	operator= (
			const cofaction_push_mpls& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		eth_type = action.eth_type;
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

	/**
	 *
	 */
	void
	set_eth_type(uint16_t eth_type) { this->eth_type = eth_type; };

	/**
	 *
	 */
	uint16_t
	get_eth_type() const { return eth_type; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_push_mpls& action) {
		os << rofl::indent(0) << "<cofaction_push_mpls ";
		os << "eth-type:0x" << std::hex << (unsigned int)action.get_eth_type() << std::dec << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint16_t	eth_type;
};



class cofaction_pop_mpls : public cofaction {
public:

	/**
	 *
	 */
	cofaction_pop_mpls(
			uint8_t ofp_version = 0,
			uint16_t eth_type = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_PUSH_VLAN),
				eth_type(eth_type) {};

	/**
	 *
	 */
	virtual
	~cofaction_pop_mpls() {};

	/**
	 *
	 */
	cofaction_pop_mpls(
			const cofaction_pop_mpls& action) { *this = action; };

	/**
	 *
	 */
	cofaction_pop_mpls&
	operator= (
			const cofaction_pop_mpls& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		eth_type = action.eth_type;
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

	/**
	 *
	 */
	void
	set_eth_type(uint16_t eth_type) { this->eth_type = eth_type; };

	/**
	 *
	 */
	uint16_t
	get_eth_type() const { return eth_type; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_pop_mpls& action) {
		os << rofl::indent(0) << "<cofaction_pop_mpls ";
		os << "eth-type:0x" << std::hex << (unsigned int)action.get_eth_type() << std::dec << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint16_t	eth_type;
};



class cofaction_group : public cofaction {
public:

	/**
	 *
	 */
	cofaction_group(
			uint8_t ofp_version = 0,
			uint32_t group_id = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_GROUP),
				group_id(group_id) {};

	/**
	 *
	 */
	virtual
	~cofaction_group() {};

	/**
	 *
	 */
	cofaction_group(
			const cofaction_group& action) { *this = action; };

	/**
	 *
	 */
	cofaction_group&
	operator= (
			const cofaction_group& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		group_id = action.group_id;
		return *this;
	};

public:
	/**
	 *
	 */
	void
	set_group_id(uint32_t group_id) { this->group_id = group_id; };

	/**
	 *
	 */
	uint32_t
	get_group_id() const { return group_id; };

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
	operator<< (std::ostream& os, const cofaction_group& action) {
		os << rofl::indent(0) << "<cofaction_group ";
		os << "group-id:" << (int)action.get_group_id() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint32_t	group_id;
};



class cofaction_set_nw_ttl : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_nw_ttl(
			uint8_t ofp_version = 0,
			uint8_t nw_ttl = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_NW_TTL),
				nw_ttl(nw_ttl) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_nw_ttl() {};

	/**
	 *
	 */
	cofaction_set_nw_ttl(
			const cofaction_set_nw_ttl& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_nw_ttl&
	operator= (
			const cofaction_set_nw_ttl& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		nw_ttl = action.nw_ttl;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_nw_ttl(uint8_t nw_ttl) { this->nw_ttl = nw_ttl; };

	/**
	 *
	 */
	uint8_t
	get_nw_ttl() const { return nw_ttl; };

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
	operator<< (std::ostream& os, const cofaction_set_nw_ttl& action) {
		os << rofl::indent(0) << "<cofaction_set_nw_ttl ";
		os << "nw-ttl:" << (unsigned int)action.get_nw_ttl() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint8_t nw_ttl;
};



class cofaction_dec_nw_ttl : public cofaction {
public:

	/**
	 *
	 */
	cofaction_dec_nw_ttl(uint8_t ofp_version = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_DEC_NW_TTL) {};

	/**
	 *
	 */
	virtual
	~cofaction_dec_nw_ttl() {};

	/**
	 *
	 */
	cofaction_dec_nw_ttl(
			const cofaction_dec_nw_ttl& action) { *this = action; };

	/**
	 *
	 */
	cofaction_dec_nw_ttl&
	operator= (
			const cofaction_dec_nw_ttl& action) {
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
	operator<< (std::ostream& os, cofaction_dec_nw_ttl const& action) {
		os << rofl::indent(0) << "<cofaction_dec_nw_ttl >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};
};



class cofaction_copy_ttl_out : public cofaction {
public:

	/**
	 *
	 */
	cofaction_copy_ttl_out(uint8_t ofp_version = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_COPY_TTL_OUT) {};

	/**
	 *
	 */
	virtual
	~cofaction_copy_ttl_out() {};

	/**
	 *
	 */
	cofaction_copy_ttl_out(
			const cofaction_copy_ttl_out& action) { *this = action; };

	/**
	 *
	 */
	cofaction_copy_ttl_out&
	operator= (
			const cofaction_copy_ttl_out& action) {
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
	operator<< (std::ostream& os, cofaction_copy_ttl_out const& action) {
		os << rofl::indent(0) << "<cofaction_copy_ttl_out >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};
};



class cofaction_copy_ttl_in : public cofaction {
public:

	/**
	 *
	 */
	cofaction_copy_ttl_in(uint8_t ofp_version = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_COPY_TTL_OUT) {};

	/**
	 *
	 */
	virtual
	~cofaction_copy_ttl_in() {};

	/**
	 *
	 */
	cofaction_copy_ttl_in(
			const cofaction_copy_ttl_in& action) { *this = action; };

	/**
	 *
	 */
	cofaction_copy_ttl_in&
	operator= (
			const cofaction_copy_ttl_in& action) {
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
	operator<< (std::ostream& os, cofaction_copy_ttl_in const& action) {
		os << rofl::indent(0) << "<cofaction_copy_ttl_in >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};
};



class cofaction_set_queue : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_queue(
			uint8_t ofp_version = 0,
			uint32_t queue_id = 0) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_QUEUE),
				queue_id(queue_id) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_queue() {};

	/**
	 *
	 */
	cofaction_set_queue(
			const cofaction_set_queue& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_queue&
	operator= (
			const cofaction_set_queue& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		queue_id = action.queue_id;
		return *this;
	};

public:

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
	operator<< (std::ostream& os, const cofaction_set_queue& action) {
		os << rofl::indent(0) << "<cofaction_set_queue ";
		os << "queue-id:" << (unsigned int)action.get_queue_id() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	uint32_t queue_id;
};



class cofaction_set_field : public cofaction {
public:

	/**
	 *
	 */
	cofaction_set_field(
			uint8_t ofp_version = 0,
			const rofl::openflow::coxmatch& oxm = rofl::openflow::coxmatch()) :
				cofaction(ofp_version, rofl::openflow::OFPAT_SET_FIELD),
				oxm(oxm) {};

	/**
	 *
	 */
	virtual
	~cofaction_set_field() {};

	/**
	 *
	 */
	cofaction_set_field(
			const cofaction_set_field& action) { *this = action; };

	/**
	 *
	 */
	cofaction_set_field&
	operator= (
			const cofaction_set_field& action) {
		if (this == &action)
			return *this;
		cofaction::operator= (action);
		oxm = action.oxm;
		return *this;
	};

public:

	/**
	 *
	 */
	rofl::openflow::coxmatch&
	set_oxm() { return oxm; };

	/**
	 *
	 */
	const rofl::openflow::coxmatch&
	get_oxm() const { return oxm; };

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
	operator<< (std::ostream& os, cofaction_set_field const& action) {
		os << rofl::indent(0) << "<cofaction_set_field >" << std::endl;
		rofl::indent i(2);
		os << coxmatch_output(action.get_oxm());
		rofl::indent j(2);
		os << dynamic_cast<cofaction const&>( action );
		return os;
	};

private:

	rofl::openflow::coxmatch oxm;
};



class cofaction_experimenter : public cofaction {
public:

	/**
	 *
	 */
	cofaction_experimenter(
			uint8_t ofp_version = 0,
			uint32_t exp_id = 0,
			const rofl::cmemory& exp_body = rofl::cmemory((size_t)0)) :
				cofaction(ofp_version, rofl::openflow::OFPAT_EXPERIMENTER),
				exp_id(exp_id),
				exp_body(exp_body) {};

	/**
	 *
	 */
	virtual
	~cofaction_experimenter() {};

	/**
	 *
	 */
	cofaction_experimenter(
			const cofaction_experimenter& action) { *this = action; };

	/**
	 *
	 */
	cofaction_experimenter&
	operator= (
			const cofaction_experimenter& action) {
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
	rofl::cmemory&
	set_exp_body() { return exp_body; };

	/**
	 *
	 */
	const rofl::cmemory&
	get_exp_body() const { return exp_body; };

	/**
	 *
	 */
	rofl::cmemory&
	set_body() { return exp_body; };

	/**
	 *
	 */
	const rofl::cmemory&
	get_body() const { return exp_body; };

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
	operator<< (std::ostream& os, cofaction_experimenter const& action) {
		os << rofl::indent(0) << "<cofaction_experimenter ";
		os << std::hex;
		os << "exp-id:" 	<< (unsigned int)action.get_exp_id() 	<< " ";
		os << std::dec;
		os << ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<cofaction const&>( action );
		os << action.get_exp_body();
		return os;
	};

private:

	uint32_t		exp_id;
	rofl::cmemory	exp_body;
};


}; // end of namespace openflow
}; // end of namespace rofl

#endif // COFACTION_H
