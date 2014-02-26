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

// LLDP TLV chassis ID header
struct lldp_tlv_chassis_id_hdr_t {
	struct lldp_tlv_hdr_t 	hdr;
	uint8_t 				subtype;
	uint8_t 				body[0];
} __attribute__((packed));


// LLDP TLV port ID header
struct lldp_tlv_port_id_hdr_t {
	struct lldp_tlv_hdr_t 	hdr;
	uint8_t 				subtype;
	uint8_t 				body[0];
} __attribute__((packed));


// LLDP TLV SYSTEM CAPS header
struct lldp_tlv_caps_hdr_t {
	struct lldp_tlv_hdr_t 	hdr;
	uint16_t 				available_caps;
	uint16_t 				enabled_caps;
};


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
	void
	pack(uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	void
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
		os << rofl::indent(0) << "<clldpattr type:" << attr.get_type() << " len:" << attr.get_length() << " >";
		return os;
	};
};




/**
 *
 */
class clldpattr_chassis_id :
		public rofl::protocol::lldp::clldpattr
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
	clldpattr_chassis_id(
			size_t len = sizeof(struct lldp_tlv_chassis_id_hdr_t));

	/**
	 *
	 */
	clldpattr_chassis_id(
			clldpattr_chassis_id const& attr);

	/**
	 *
	 */
	clldpattr_chassis_id&
	operator= (clldpattr_chassis_id const& attr);

	/**
	 *
	 */
	virtual
	~clldpattr_chassis_id();

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
	operator<< (std::ostream& os, clldpattr_chassis_id const& attr) {
		os << dynamic_cast<clldpattr const&>( attr );
		os << rofl::indent(2) << "<clldpattr_chassis_id sub-type:" << attr.get_sub_type() << " body:" << attr.get_body() << " >";
		return os;
	};
};


}; // end of namespace
}; // end of namespace
}; // end of namespace

#endif /* CLLDPATTR_H_ */
