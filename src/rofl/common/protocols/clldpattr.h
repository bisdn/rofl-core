/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * clldpattr.h
 *
 *  Created on: 26.02.2014
 *      Author: andi
 */

#ifndef CLLDPATTR_H_
#define CLLDPATTR_H_

#include <string.h>

#include <string>

#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/logging.h"

namespace rofl {
namespace protocol {
namespace lldp {

class eLLDPException		: public RoflException {};
class eLLDPNotFound			: public eLLDPException {};
class eLLDPInval			: public eLLDPException {};


#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

/*
 * LLDP common TLV header
 */
struct lldp_tlv_hdr_t {
	uint16_t 		tlen; // type:7 + length:9
	uint8_t 		body[0];
} __attribute__((packed));


// Ethernet + LLDP common header
struct lldp_hdr_t {
	uint8_t 				eth_dst[ETH_ALEN];
	uint8_t 				eth_src[ETH_ALEN];
	uint16_t 				eth_type; // = 0x88-CC
	struct lldp_tlv_hdr_t 	body[0];
} __attribute__((packed));

// LLDP TLV ID header (chassis-id, port-id)
struct lldp_tlv_id_hdr_t {
	struct lldp_tlv_hdr_t 	hdr;
	uint8_t 				subtype;
	uint8_t 				body[0];
} __attribute__((packed));


// LLDP TLV SYSTEM CAPS header
struct lldp_tlv_sys_caps_hdr_t {
	struct lldp_tlv_hdr_t 	hdr;
	uint8_t					chassis_id;
	uint16_t 				available_caps;
	uint16_t 				enabled_caps;
} __attribute__((packed));


// LLDP TLV TTL header
struct lldp_tlv_ttl_hdr_t {
	struct lldp_tlv_hdr_t 	hdr;
	uint16_t 				ttl;
} __attribute__((packed));


/* LLDP TLV types */
enum lldp_tlv_type {
	LLDPTT_END 			= 0,
	LLDPTT_CHASSIS_ID 	= 1,
	LLDPTT_PORT_ID 		= 2,
	LLDPTT_TTL 			= 3,
	LLDPTT_PORT_DESC 	= 4,
	LLDPTT_SYSTEM_NAME 	= 5,
	LLDPTT_SYSTEM_DESC 	= 6,
	LLDPTT_SYSTEM_CAPS 	= 7,
	LLDPTT_MGMT_ADDR 	= 8,
};

/* LLDP chassis ID TLV subtypes */
enum lldp_tlv_chassis_id_subtype {
	LLDPCHIDST_RESERVED = 0,
	LLDPCHIDST_CHASSIS 	= 1,
	LLDPCHIDST_IFALIAS 	= 2,
	LLDPCHIDST_PORT 	= 3,
	LLDPCHIDST_MAC_ADDR = 4,
	LLDPCHIDST_NET_ADDR = 5,
	LLDPCHIDST_IFNAME 	= 6,
	LLDPCHIDST_LOCAL 	= 7,
};

/* LLDP port ID TLV subtypes */
enum lldp_tlv_port_id_subtype {
	LLDPPRTIDST_RESERVED 	= 0,
	LLDPPRTIDST_IFALIAS 	= 1,
	LLDPPRTIDST_PORT 		= 2,
	LLDPPRTIDST_MAC_ADDR 	= 3,
	LLDPPRTIDST_NET_ADDR 	= 4,
	LLDPPRTIDST_IFNAME 		= 5,
	LLDPPRTIDST_AGENT_CIRCUIT_ID = 6,
	LLDPPRTIDST_LOCAL 		= 7,
};

/* LLDP system capabilities */
enum lldp_sys_caps {
	LLDP_SYSCAPS_OTHER 		= 0,
	LLDP_SYSCAPS_REPEATER 	= 1,
	LLDP_SYSCAPS_BRIDGE 	= 2,
	LLDP_SYSCAPS_WLANAP 	= 3,
	LLDP_SYSCAPS_ROUTER 	= 4,
	LLDP_SYSCAPS_PHONE 		= 5,
	LLDP_SYSCAPS_DOCSIS 	= 6,
	LLDP_SYSCAPS_STA_ONLY 	= 7,
};




/**
 *
 */
class clldpattr
{
protected:

	rofl::cmemory				hdr;	// LLDP header
	rofl::cmemory 				body;	// LLDP payload

public:

	/**
	 *
	 */
	clldpattr(
			size_t bodylen = 0);

	/**
	 *
	 */
	clldpattr(
			clldpattr const& attr);

	/**
	 *
	 */
	clldpattr&
	operator= (clldpattr const& attr);

	/**
	 *
	 */
	bool
	operator== (clldpattr const& attr);

	/**
	 *
	 */
	virtual
	~clldpattr();

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
	pack(uint8_t *buf = NULL, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	virtual uint8_t
	get_type() const;

	/**
	 *
	 */
	virtual void
	set_type(uint8_t type);

	/**
	 *
	 */
	virtual uint16_t
	get_length() const;

	/**
	 *
	 */
	virtual void
	set_length(uint16_t len);

	/**
	 *
	 */
	virtual rofl::cmemory const&
	get_body() const { return body; };

	/**
	 *
	 */
	virtual rofl::cmemory&
	set_body() { return body; };

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, clldpattr const& attr) {
		os << rofl::indent(0) << "<clldpattr type:" << (int)attr.get_type()
				<< " len:" << (int)attr.get_length()
				<< " mem-len:" << (int)attr.length()
				<< " >" << std::endl;
		return os;
	};
};



/**
 *
 */
class clldpattr_end :
		public rofl::protocol::lldp::clldpattr
{
public:

	/**
	 *
	 */
	clldpattr_end();

	/**
	 *
	 */
	clldpattr_end(
			clldpattr const& attr) : clldpattr(attr) {};

	/**
	 *
	 */
	clldpattr_end(
			clldpattr_end const& attr);

	/**
	 *
	 */
	clldpattr_end&
	operator= (clldpattr_end const& attr);

	/**
	 *
	 */
	virtual
	~clldpattr_end();


public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, clldpattr_end const& attr) {
		os << dynamic_cast<clldpattr const&>( attr );
		os << rofl::indent(2) << "<clldpattr_end >" << std::endl;
		return os;
	};
};




/**
 *
 */
class clldpattr_id :
		public rofl::protocol::lldp::clldpattr
{
	uint8_t 		sub_type;

public:

	/**
	 *
	 */
	clldpattr_id(
			uint8_t type,
			uint8_t sub_type = 0,
			size_t len = sizeof(struct lldp_tlv_id_hdr_t));

	/**
	 *
	 */
	clldpattr_id(
			clldpattr const& attr) : clldpattr(attr), sub_type(0) {};

	/**
	 *
	 */
	clldpattr_id(
			clldpattr_id const& attr);

	/**
	 *
	 */
	clldpattr_id&
	operator= (clldpattr_id const& attr);

	/**
	 *
	 */
	virtual
	~clldpattr_id();

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
	pack(uint8_t *buf = NULL, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	virtual uint8_t const&
	get_sub_type() const { return sub_type; };

	/**
	 *
	 */
	virtual uint8_t&
	set_sub_type() { return sub_type; };

	/**
	 *
	 */
	virtual std::string
	get_string() const;

	/**
	 *
	 */
	virtual void
	set_string(std::string const str);


public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, clldpattr_id const& attr) {
		os << dynamic_cast<clldpattr const&>( attr );
		switch (attr.get_type()) {
		case LLDPTT_CHASSIS_ID: {
			os << rofl::indent(2) << "<clldpattr_chassis_id sub-type:" << (int)attr.get_sub_type() << " >" << std::endl;
		} break;
		case LLDPTT_PORT_ID: {
			os << rofl::indent(2) << "<clldpattr_port_id sub-type:" << (int)attr.get_sub_type() << " >" << std::endl;
		} break;
		default: {
			os << rofl::indent(2) << "<clldpattr_id type:" << (int)attr.get_type() << " sub-type:" << (int)attr.get_sub_type() << " >" << std::endl;
		};
		}
		rofl::indent i(4);
		os << attr.get_body();
		return os;
	};
};




/**
 *
 */
class clldpattr_ttl :
		public rofl::protocol::lldp::clldpattr
{
	uint16_t ttl;

public:

	/**
	 *
	 */
	clldpattr_ttl(
			size_t len = sizeof(struct lldp_tlv_ttl_hdr_t));

	/**
	 *
	 */
	clldpattr_ttl(
			clldpattr const& attr) : clldpattr(attr), ttl(0) {};

	/**
	 *
	 */
	clldpattr_ttl(
			clldpattr_ttl const& attr);

	/**
	 *
	 */
	clldpattr_ttl&
	operator= (clldpattr_ttl const& attr);

	/**
	 *
	 */
	virtual
	~clldpattr_ttl();

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
	pack(uint8_t *buf = NULL, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	virtual uint16_t const&
	get_ttl() const { return ttl; };

	/**
	 *
	 */
	virtual uint16_t&
	set_ttl() { return ttl; };

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, clldpattr_ttl const& attr) {
		os << dynamic_cast<clldpattr const&>( attr );
		os << rofl::indent(2) << "<clldpattr_ttl ttl:0x" << std::hex << (int)attr.get_ttl() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 *
 */
class clldpattr_desc :
		public rofl::protocol::lldp::clldpattr
{
public:

	/**
	 *
	 */
	clldpattr_desc(
			uint8_t type,
			size_t len = sizeof(struct lldp_tlv_hdr_t));

	/**
	 *
	 */
	clldpattr_desc(
			clldpattr const& attr) : clldpattr(attr) {
		//lldp_desc_generic = somem();
	};

	/**
	 *
	 */
	clldpattr_desc(
			clldpattr_desc const& attr);

	/**
	 *
	 */
	clldpattr_desc&
	operator= (clldpattr_desc const& attr);

	/**
	 *
	 */
	virtual
	~clldpattr_desc();


public:

	/**
	 *
	 */
	virtual std::string
	get_desc() const;

	/**
	 *
	 */
	virtual void
	set_desc(std::string const& desc);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, clldpattr_desc const& attr) {
		os << dynamic_cast<clldpattr const&>( attr );
		switch (attr.get_type()) {
		case rofl::protocol::lldp::LLDPTT_PORT_DESC: {
			os << rofl::indent(2) << "<clldpattr_port_desc desc:" << attr.get_desc() << " >";
		} break;
		case rofl::protocol::lldp::LLDPTT_SYSTEM_NAME: {
			os << rofl::indent(2) << "<clldpattr_system_name desc:" << attr.get_desc() << " >";
		} break;
		case rofl::protocol::lldp::LLDPTT_SYSTEM_DESC: {
			os << rofl::indent(2) << "<clldpattr_system_desc desc:" << attr.get_desc() << " >";
		} break;
		default: {
			os << rofl::indent(2) << "<clldpattr_desc type:" << (int)attr.get_type() << " desc:" << attr.get_desc() << " >";
		};
		}
		rofl::indent i(4);
		os << attr.get_body();
		return os;
	};
};











/**
 *
 */
class clldpattr_system_caps :
		public rofl::protocol::lldp::clldpattr
{
	uint8_t			chassis_id;
	uint16_t		available_caps;
	uint16_t		enabled_caps;

public:

	/**
	 *
	 */
	clldpattr_system_caps(
			size_t len = sizeof(struct lldp_tlv_sys_caps_hdr_t));

	/**
	 *
	 */
	clldpattr_system_caps(
			clldpattr const& attr) : clldpattr(attr), chassis_id(0), available_caps(0), enabled_caps(0) {};

	/**
	 *
	 */
	clldpattr_system_caps(
			clldpattr_system_caps const& attr);

	/**
	 *
	 */
	clldpattr_system_caps&
	operator= (clldpattr_system_caps const& attr);

	/**
	 *
	 */
	virtual
	~clldpattr_system_caps();

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
	pack(uint8_t *buf = NULL, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	virtual uint8_t const&
	get_chassis_id() const { return chassis_id; };

	/**
	 *
	 */
	virtual uint8_t&
	set_chassis_id() { return chassis_id; };

	/**
	 *
	 */
	virtual uint16_t const&
	get_available_caps() const { return available_caps; };

	/**
	 *
	 */
	virtual uint16_t&
	set_available_caps() { return available_caps; };

	/**
	 *
	 */
	virtual uint16_t const&
	get_enabled_caps() const { return enabled_caps; };

	/**
	 *
	 */
	virtual uint16_t&
	set_enabled_caps() { return enabled_caps; };


public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, clldpattr_system_caps const& attr) {
		os << dynamic_cast<clldpattr const&>( attr );
		os << rofl::indent(2) << "<clldpattr_sys_caps >" << std::endl;
		os << rofl::indent(4) << "<chassis-id: " 		<< (int)attr.get_chassis_id() << " >" << std::endl;
		os << rofl::indent(4) << "<available-caps: 0x" 	<< std::hex << (int)attr.get_available_caps() << std::dec << " >" << std::endl;
		os << rofl::indent(4) << "<enabled-caps: 0x" 	<< std::hex << (int)attr.get_enabled_caps() << std::dec << " >" << std::endl;
		return os;
	};
};

// TODO: mgmt-addr

}; // end of namespace lldp
}; // end of namespace protocol
}; // end of namespace rofl

#endif /* CLLDPATTR_H_ */
