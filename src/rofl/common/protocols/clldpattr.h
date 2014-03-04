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
	uint8_t 				dst[ETH_ALEN];
	uint8_t 				src[ETH_ALEN];
	uint16_t 				ethertype; // = 0x88-CC
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
class clldpattr :
		public rofl::cmemory
{
	union {
		uint8_t					*lldpu_generic;
		struct lldp_tlv_hdr_t	*lldpu_hdr;
	} lldp_lldpu;

#define lldp_generic 	lldp_lldpu.lldpu_generic
#define lldp_hdr		lldp_lldpu.lldpu_hdr

public:

	/**
	 *
	 */
	clldpattr(
			size_t len = sizeof(struct lldp_tlv_hdr_t));

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
	virtual
	~clldpattr();

public:

	/**
	 *
	 */
	virtual uint8_t*
	resize(size_t len);

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
	virtual rofl::cmemory
	get_body() const;

	/**
	 *
	 */
	virtual void
	set_body(rofl::cmemory const& body);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, clldpattr const& attr) {
		os << rofl::indent(0) << "<clldpattr type:" << (int)attr.get_type() << " len:" << (int)attr.get_length() << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<rofl::cmemory const&>( attr );
		return os;
	};
};




/**
 *
 */
class clldpattr_id :
		public rofl::protocol::lldp::clldpattr
{
	union {
		uint8_t						*lldpu_id_generic;
		struct lldp_tlv_id_hdr_t	*lldpu_id_hdr;
	} lldp_lldpu;

#define lldp_id_generic 	lldp_lldpu.lldpu_id_generic
#define lldp_id_hdr			lldp_lldpu.lldpu_id_hdr

public:

	/**
	 *
	 */
	clldpattr_id(
			size_t len = sizeof(struct lldp_tlv_id_hdr_t));

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
	virtual uint8_t*
	resize(size_t len);

public:

	/**
	 *
	 */
	virtual uint8_t
	get_sub_type() const;

	/**
	 *
	 */
	virtual void
	set_sub_type(uint8_t type);

	/**
	 *
	 */
	virtual rofl::cmemory
	get_body() const;

	/**
	 *
	 */
	virtual void
	set_body(rofl::cmemory const& body);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, clldpattr_id const& attr) {
		os << dynamic_cast<clldpattr const&>( attr );
		switch (attr.get_type()) {
		case rofl::protocol::lldp::LLDPTT_CHASSIS_ID: {
			os << rofl::indent(2) << "<clldpattr_chassis_id sub-type:" << attr.get_sub_type() << " >" << std::endl;
		} break;
		case rofl::protocol::lldp::LLDPTT_PORT_ID: {
			os << rofl::indent(2) << "<clldpattr_port_id sub-type:" << attr.get_sub_type() << " >" << std::endl;
		} break;
		default: {
			// do nothing
		};
		}
		rofl::indent i(4);
		os << attr.get_body();
		return os;
	};
};

typedef clldpattr_id clldpattr_chassis_id;
typedef clldpattr_id clldpattr_port_id;



/**
 *
 */
class clldpattr_ttl :
		public rofl::protocol::lldp::clldpattr
{
	union {
		uint8_t						*lldpu_ttl_generic;
		struct lldp_tlv_ttl_hdr_t	*lldpu_ttl_hdr;
	} lldp_lldpu;

#define lldp_ttl_generic 	lldp_lldpu.lldpu_ttl_generic
#define lldp_ttl_hdr		lldp_lldpu.lldpu_ttl_hdr

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
	virtual uint8_t*
	resize(size_t len);

public:

	/**
	 *
	 */
	virtual uint16_t
	get_ttl() const;

	/**
	 *
	 */
	virtual void
	set_ttl(uint16_t ttl);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, clldpattr_ttl const& attr) {
		os << dynamic_cast<clldpattr const&>( attr );
		os << rofl::indent(2) << "<clldpattr_ttl ttl:" << attr.get_ttl() << " >";
		return os;
	};
};


/**
 *
 */
class clldpattr_desc :
		public rofl::protocol::lldp::clldpattr
{
	union {
		uint8_t					*lldpu_desc_generic;
		struct lldp_tlv_hdr_t	*lldpu_desc_hdr;
	} lldp_lldpu;

#define lldp_desc_generic 	lldp_lldpu.lldpu_desc_generic
#define lldp_desc_hdr		lldp_lldpu.lldpu_desc_hdr

public:

	/**
	 *
	 */
	clldpattr_desc(
			size_t len = sizeof(struct lldp_tlv_hdr_t));

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
	virtual uint8_t*
	resize(size_t len);

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
			os << rofl::indent(2) << "<clldpattr_desc desc:" << attr.get_desc() << " >";
		} break;
		case rofl::protocol::lldp::LLDPTT_SYSTEM_NAME: {
			os << rofl::indent(2) << "<clldpattr_system_name desc:" << attr.get_desc() << " >";
		} break;
		case rofl::protocol::lldp::LLDPTT_SYSTEM_DESC: {
			os << rofl::indent(2) << "<clldpattr_system_desc desc:" << attr.get_desc() << " >";
		} break;
		default: {
			// do nothing
		};
		}

		return os;
	};
};

typedef clldpattr_desc clldpattr_port_desc;
typedef clldpattr_desc clldpattr_system_name;
typedef clldpattr_desc clldpattr_system_desc;



/**
 *
 */
class clldpattr_sys_caps :
		public rofl::protocol::lldp::clldpattr
{
	union {
		uint8_t							*lldpu_sys_caps_generic;
		struct lldp_tlv_sys_caps_hdr_t	*lldpu_sys_caps_hdr;
	} lldp_lldpu;

#define lldp_sys_caps_generic 	lldp_lldpu.lldpu_sys_caps_generic
#define lldp_sys_caps_hdr		lldp_lldpu.lldpu_sys_caps_hdr

public:

	/**
	 *
	 */
	clldpattr_sys_caps(
			size_t len = sizeof(struct lldp_tlv_sys_caps_hdr_t));

	/**
	 *
	 */
	clldpattr_sys_caps(
			clldpattr_sys_caps const& attr);

	/**
	 *
	 */
	clldpattr_sys_caps&
	operator= (clldpattr_sys_caps const& attr);

	/**
	 *
	 */
	virtual
	~clldpattr_sys_caps();

public:

	/**
	 *
	 */
	virtual uint8_t*
	resize(size_t len);

public:

	/**
	 *
	 */
	virtual uint8_t
	get_chassis_id() const;

	/**
	 *
	 */
	virtual void
	set_chassis_id(uint8_t chassis_id);

	/**
	 *
	 */
	virtual uint16_t
	get_available_caps() const;

	/**
	 *
	 */
	virtual void
	set_available_caps(uint16_t caps);

	/**
	 *
	 */
	virtual uint16_t
	get_enabled_caps() const;

	/**
	 *
	 */
	virtual void
	set_enabled_caps(uint16_t caps);


public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, clldpattr_sys_caps const& attr) {
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
