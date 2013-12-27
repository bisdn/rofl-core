/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CLLDPTLV_H
#define CLLDPTLV_H 1

#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../../cmemory.h"
#include "../../croflexception.h"
#include "../../cvastring.h"
#include "../../coflist.h"

namespace rofl
{

class eLLDPbase : public RoflException {};
class eLLDPInval : public eLLDPbase {};

class clldptlv {
public:

	// LLDP common TLV header
	struct lldp_tlv_hdr_t {
		uint16_t hdr; // see set_hdr_type(), set_hdr_length() methods below
		uint8_t body[0];
	} __attribute__((packed));

	// LLDP maximum length values for all tlv types in octets
	#define LLDP_TLV_CHASSIS_ID_MAX_LEN			(256)
	#define LLDP_TLV_PORT_ID_MAX_LEN			(256)
	#define LLDP_TLV_TTL_MAX_LEN				(2)
	#define LLDP_TLV_PORT_DESC_MAX_LEN			(256)
	#define LLDP_TLV_SYS_NAME_MAX_LEN			(255)
	#define LLDP_TLV_SYS_DESC_MAX_LEN			(255)
	#define LLDP_TLV_SYS_CAPS_MAX_LEN			(4)
	#define LLDP_TLV_MGMT_ADDR_MAX_LEN			(167)
	#define LLDP_TLV_ORG_SPEC_MAX_LEN			(511)


	// LLDP TLV chassis ID header
	struct lldp_tlv_chassis_id_hdr_t {
		struct lldp_tlv_hdr_t header;
		uint8_t subtype;
		uint8_t body[0];
	} __attribute__((packed));


	// LLDP TLV port ID header
	struct lldp_tlv_port_id_hdr_t {
		struct lldp_tlv_hdr_t header;
		uint8_t subtype;
		uint8_t body[0];
	} __attribute__((packed));


	// LLDP TLV SYSTEM CAPS header
	struct lldp_tlv_sys_caps_hdr_t {
		struct lldp_tlv_hdr_t header;
		uint16_t available_caps;
		uint16_t enabled_caps;
	};


	// LLDP TLV TTL header
	struct lldp_tlv_ttl_hdr_t {
		struct lldp_tlv_hdr_t header;
		uint16_t ttl;
	} __attribute__((packed));


	/* LLDP TLV types */
	enum lldp_tlv_type {
		LLDPTT_END = 0,
		LLDPTT_CHASSIS_ID = 1,
		LLDPTT_PORT_ID = 2,
		LLDPTT_TTL = 3,
		LLDPTT_PORT_DESC = 4,
		LLDPTT_SYSTEM_NAME = 5,
		LLDPTT_SYSTEM_DESC = 6,
		LLDPTT_SYSTEM_CAPS = 7,
		LLDPTT_MGMT_ADDR = 8
	};

	/* LLDP chassis ID TLV subtypes */
	enum lldp_tlv_chassis_id_subtype {
		LLDPCHIDST_RESERVED = 0,
		LLDPCHIDST_CHASSIS = 1,
		LLDPCHIDST_IFALIAS = 2,
		LLDPCHIDST_PORT = 3,
		LLDPCHIDST_MAC_ADDR = 4,
		LLDPCHIDST_NET_ADDR = 5,
		LLDPCHIDST_IFNAME = 6,
		LLDPCHIDST_LOCAL = 7,
	};

	/* LLDP port ID TLV subtypes */
	enum lldp_tlv_port_id_subtype {
		LLDPPRTIDST_RESERVED = 0,
		LLDPPRTIDST_IFALIAS = 1,
		LLDPPRTIDST_PORT = 2,
		LLDPPRTIDST_MAC_ADDR = 3,
		LLDPPRTIDST_NET_ADDR = 4,
		LLDPPRTIDST_IFNAME = 5,
		LLDPPRTIDST_AGENT_CIRCUIT_ID = 6,
		LLDPPRTIDST_LOCAL = 7,
	};

	/* LLDP system capabilities */
	enum lldp_sys_caps_t {
		LLDP_SYSCAPS_OTHER = (1 << 0),
		LLDP_SYSCAPS_REPEATER = (1 << 1),
		LLDP_SYSCAPS_BRIDGE = (1 << 2),
		LLDP_SYSCAPS_WLANAP = (1 << 3),
		LLDP_SYSCAPS_ROUTER = (1 << 4),
		LLDP_SYSCAPS_PHONE = (1 << 5),
		LLDP_SYSCAPS_DOCSIS = (1 << 6),
		LLDP_SYSCAPS_STA_ONLY = (1 << 7),
	};

public: // data structures

	#define DEFAULT_LLDP_TLV_SIZE 128

	cmemory tlvmem;

	struct lldp_tlv_hdr_t					*tlv_hdr;  // pointer to TLV header, always set
	union {
		struct lldp_tlv_hdr_t 				*lldpu_tlv_hdr;
		struct lldp_tlv_chassis_id_hdr_t  	*lldpu_chassis_id_hdr;
		struct lldp_tlv_port_id_hdr_t		*lldpu_port_id_hdr;
		struct lldp_tlv_sys_caps_hdr_t		*lldpu_sys_caps_hdr;
		struct lldp_tlv_ttl_hdr_t			*lldpu_ttl_hdr;
	} lldp_lldpu;

	#define lldp_tlv						lldp_lldpu.lldpu_tlv_hdr
	#define lldp_chassis_id					lldp_lldpu.lldpu_chassis_id_hdr
	#define lldp_port_id					lldp_lldpu.lldpu_port_id_hdr
	#define lldp_sys_caps					lldp_lldpu.lldpu_sys_caps_hdr
	#define lldp_ttl						lldp_lldpu.lldpu_ttl_hdr

public: // methods

	/**
	 *
	 */
	clldptlv(
			size_t tlvlen = DEFAULT_LLDP_TLV_SIZE);

	/**
	 *
	 */
	clldptlv(
			uint8_t* tlv,
			size_t tlvlen);

	/**
	 *
	 */
	virtual
	~clldptlv();

	/**
	 *
	 */
	clldptlv&
	operator= (clldptlv const& tlv);

	/** returns length required for storing this tlv
	 * includes size of common tlv header
	 *
	 */
	size_t
	length() const;

	/** packs LLDP TLV to (tlv, tlvlen)
	 *
	 */
	struct lldp_tlv_hdr_t*
	pack(
			struct lldp_tlv_hdr_t *tlv,
			size_t tlvlen) throw (eLLDPInval);

	/** unpacks LLDP TLV at (tlv,tlvlen)
	 *
	 */
	struct lldp_tlv_hdr_t*
	unpack(
			struct lldp_tlv_hdr_t *tlv,
			size_t tlvlen) throw (eLLDPInval);

	/** return info string
	 *
	 */
	virtual const char*
	c_str();

	/** 7bits type field in hdr
	 *
	 */
	void
	set_hdr_type(uint8_t type);

	/** 7bits type field in hdr
	 *
	 */
	uint8_t
	get_hdr_type() const;

	/** 9bits length field in hdr
	 *
	 */
	void
	set_hdr_length(uint16_t len);

	/** 9bits length field in hdr
	 *
	 */
	uint16_t
	get_hdr_length() const;

protected: // data structures

	std::string info;

};


class clldptlv_end : public clldptlv {
public:
	clldptlv_end() :
		clldptlv(sizeof(struct lldp_tlv_hdr_t))
	{
		set_hdr_type(LLDPTT_END);
		set_hdr_length(0);
	};
	clldptlv_end(clldptlv const& tlv) :
		clldptlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		info.append(vas("clldptlv(%p) LLDP -END- TLV", this));
		return info.c_str();
	};
};


class clldptlv_chassis_id : public clldptlv {
public:
	clldptlv_chassis_id(
			uint8_t chassis_id_subtype,
			std::string chassis_id = std::string("")) :
		clldptlv(sizeof(struct lldp_tlv_hdr_t) + LLDP_TLV_CHASSIS_ID_MAX_LEN)
	{
		set_hdr_type(LLDPTT_CHASSIS_ID);
		set_hdr_length(sizeof(uint8_t) /* chassis id subtype */ + chassis_id.length());
		lldp_chassis_id->subtype = chassis_id_subtype;
		if (not chassis_id.empty())
		{
			size_t len = (chassis_id.length() < (LLDP_TLV_CHASSIS_ID_MAX_LEN - 1)) ?
					chassis_id.length() : (LLDP_TLV_CHASSIS_ID_MAX_LEN - 1);
			memcpy(lldp_chassis_id->body, chassis_id.c_str(), len);
		}
	};
	clldptlv_chassis_id(clldptlv const& tlv) :
		clldptlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string s_subtype;
		switch (lldp_chassis_id->subtype) {
		case LLDPCHIDST_CHASSIS:
			s_subtype.assign("chassis component");
			break;
		case LLDPCHIDST_IFALIAS:
			s_subtype.assign("interface alias");
			break;
		case LLDPCHIDST_PORT:
			s_subtype.assign("port component");
			break;
		case LLDPCHIDST_MAC_ADDR:
			s_subtype.assign("mac address");
			break;
		case LLDPCHIDST_NET_ADDR:
			s_subtype.assign("network address");
			break;
		case LLDPCHIDST_IFNAME:
			s_subtype.assign("interface name");
			break;
		case LLDPCHIDST_LOCAL:
			s_subtype.assign("locally assigned");
			break;
		case LLDPCHIDST_RESERVED:
		default:
			s_subtype.assign("reserved");
			break;
		}

		std::string s_idstr((const char*)lldp_chassis_id->body, (get_hdr_length() - 1));
		info.append(vas("clldptlv(%p) LLDP -CHASSID-ID- TLV: -%s- %s ",
				this,
				s_subtype.c_str(),
				s_idstr.c_str()));
		return info.c_str();
	};
};


class clldptlv_port_id : public clldptlv {
public:
	clldptlv_port_id(
			uint8_t port_id_subtype,
			std::string port_id = std::string("")) :
		clldptlv(sizeof(struct lldp_tlv_hdr_t) + LLDP_TLV_PORT_ID_MAX_LEN)
	{
		set_hdr_type(LLDPTT_PORT_ID);
		set_hdr_length(sizeof(uint8_t) /* chassis id subtype */ + port_id.length());
		lldp_port_id->subtype = port_id_subtype;
		if (not port_id.empty())
		{
			size_t len = (port_id.length() < (LLDP_TLV_PORT_ID_MAX_LEN - 1)) ?
					port_id.length() : (LLDP_TLV_PORT_ID_MAX_LEN - 1);
			memcpy(lldp_port_id->body, port_id.c_str(), len);
		}
	};
	clldptlv_port_id(clldptlv const& tlv) :
		clldptlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string s_subtype;
		switch (lldp_chassis_id->subtype) {
		case LLDPPRTIDST_IFALIAS:
			s_subtype.assign("interface alias");
			break;
		case LLDPPRTIDST_PORT:
			s_subtype.assign("port component");
			break;
		case LLDPPRTIDST_MAC_ADDR:
			s_subtype.assign("mac address");
			break;
		case LLDPPRTIDST_NET_ADDR:
			s_subtype.assign("network address");
			break;
		case LLDPPRTIDST_IFNAME:
			s_subtype.assign("interface name");
			break;
		case LLDPPRTIDST_AGENT_CIRCUIT_ID:
			s_subtype.assign("agent circuit id");
			break;
		case LLDPPRTIDST_LOCAL:
			s_subtype.assign("locally assigned");
			break;
		case LLDPPRTIDST_RESERVED:
		default:
			s_subtype.assign("reserved");
			break;
		}

		std::string s_idstr((const char*)lldp_chassis_id->body, (get_hdr_length() - 1));
		info.append(vas("clldptlv(%p) LLDP -PORT-ID- TLV: -%s- %s ",
				this,
				s_subtype.c_str(),
				s_idstr.c_str()));
		return info.c_str();
	};
};


class clldptlv_ttl : public clldptlv {
public:
	clldptlv_ttl(uint16_t ttl) :
		clldptlv(sizeof(struct lldp_tlv_hdr_t) + LLDP_TLV_TTL_MAX_LEN)
	{
		set_hdr_type(LLDPTT_TTL);
		set_hdr_length(sizeof(uint16_t));
		lldp_ttl->ttl = htobe16(ttl);
	};
	clldptlv_ttl(clldptlv const& tlv) :
		clldptlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		info.append(vas("clldptlv(%p) LLDP -TTL- TLV: %d",
				this,
				be16toh(lldp_ttl->ttl)));
		return info.c_str();
	};
};


class clldptlv_port_desc : public clldptlv {
public:
	clldptlv_port_desc(
			std::string port_desc = std::string("")) :
		clldptlv(sizeof(struct lldp_tlv_hdr_t) + LLDP_TLV_PORT_DESC_MAX_LEN)
	{
		set_hdr_type(LLDPTT_PORT_DESC);
		set_hdr_length(port_desc.length());
		if (not port_desc.empty())
		{
			size_t len = (port_desc.length() < (LLDP_TLV_PORT_DESC_MAX_LEN)) ?
					port_desc.length() : (LLDP_TLV_PORT_DESC_MAX_LEN);
			memcpy(lldp_tlv->body, port_desc.c_str(), len);
		}
	};
	clldptlv_port_desc(clldptlv const& tlv) :
		clldptlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string s_idstr((const char*)lldp_tlv->body, (get_hdr_length()));
		info.append(vas("clldptlv(%p) LLDP -PORT-DESC- TLV: %s ",
				this,
				s_idstr.c_str()));
		return info.c_str();
	};
};


class clldptlv_sys_name : public clldptlv {
public:
	clldptlv_sys_name(
			std::string sys_name = std::string("")) :
		clldptlv(sizeof(struct lldp_tlv_hdr_t) + LLDP_TLV_SYS_NAME_MAX_LEN)
	{
		set_hdr_type(LLDPTT_SYSTEM_NAME);
		set_hdr_length(sys_name.length());
		if (not sys_name.empty())
		{
			size_t len = (sys_name.length() < (LLDP_TLV_SYS_NAME_MAX_LEN)) ?
					sys_name.length() : (LLDP_TLV_SYS_NAME_MAX_LEN);
			memcpy(lldp_tlv->body, sys_name.c_str(), len);
		}
	};
	clldptlv_sys_name(clldptlv const& tlv) :
		clldptlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string s_idstr((const char*)lldp_tlv->body, (get_hdr_length()));
		info.append(vas("clldptlv(%p) LLDP -SYS-NAME- TLV: %s ",
				this,
				s_idstr.c_str()));
		return info.c_str();
	};
};


class clldptlv_sys_desc : public clldptlv {
public:
	clldptlv_sys_desc(
			std::string sys_desc = std::string("")) :
		clldptlv(sizeof(struct lldp_tlv_hdr_t) + LLDP_TLV_SYS_DESC_MAX_LEN)
	{
		set_hdr_type(LLDPTT_SYSTEM_DESC);
		set_hdr_length(sys_desc.length());
		if (not sys_desc.empty())
		{
			size_t len = (sys_desc.length() < (LLDP_TLV_SYS_DESC_MAX_LEN)) ?
					sys_desc.length() : (LLDP_TLV_SYS_DESC_MAX_LEN);
			memcpy(lldp_tlv->body, sys_desc.c_str(), len);
		}
	};
	clldptlv_sys_desc(clldptlv const& tlv) :
		clldptlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string s_idstr((const char*)lldp_tlv->body, (get_hdr_length()));
		info.append(vas("clldptlv(%p) LLDP -SYS-DESC- TLV: %s ",
				this,
				s_idstr.c_str()));
		return info.c_str();
	};
};


class clldptlv_sys_caps : public clldptlv {
public:
	clldptlv_sys_caps(
			uint16_t available_caps,
			uint16_t enabled_caps) :
		clldptlv(sizeof(struct lldp_tlv_hdr_t) + LLDP_TLV_SYS_CAPS_MAX_LEN)
	{
		set_hdr_type(LLDPTT_SYSTEM_CAPS);
		set_hdr_length(2 * sizeof(uint16_t));
		lldp_sys_caps->available_caps = htobe16(available_caps);
		lldp_sys_caps->enabled_caps = htobe16(enabled_caps);
	};
	clldptlv_sys_caps(clldptlv const& tlv) :
		clldptlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string s_caps;

		if (be16toh(lldp_sys_caps->available_caps) & LLDP_SYSCAPS_OTHER)
		{
			s_caps.assign(vas("other%s ",
					(be16toh(lldp_sys_caps->enabled_caps) & LLDP_SYSCAPS_OTHER) ? "(+)" : ""));
		}
		if (be16toh(lldp_sys_caps->available_caps) & LLDP_SYSCAPS_REPEATER)
		{
			s_caps.assign(vas("repeater%s ",
					(be16toh(lldp_sys_caps->enabled_caps) & LLDP_SYSCAPS_REPEATER) ? "(+)" : ""));
		}
		if (be16toh(lldp_sys_caps->available_caps) & LLDP_SYSCAPS_BRIDGE)
		{
			s_caps.assign(vas("bridge%s ",
					(be16toh(lldp_sys_caps->enabled_caps) & LLDP_SYSCAPS_BRIDGE) ? "(+)" : ""));
		}
		if (be16toh(lldp_sys_caps->available_caps) & LLDP_SYSCAPS_WLANAP)
		{
			s_caps.assign(vas("wlanap%s ",
					(be16toh(lldp_sys_caps->enabled_caps) & LLDP_SYSCAPS_WLANAP) ? "(+)" : ""));
		}
		if (be16toh(lldp_sys_caps->available_caps) & LLDP_SYSCAPS_ROUTER)
		{
			s_caps.assign(vas("router%s ",
					(be16toh(lldp_sys_caps->enabled_caps) & LLDP_SYSCAPS_ROUTER) ? "(+)" : ""));
		}
		if (be16toh(lldp_sys_caps->available_caps) & LLDP_SYSCAPS_PHONE)
		{
			s_caps.assign(vas("phone%s ",
					(be16toh(lldp_sys_caps->enabled_caps) & LLDP_SYSCAPS_PHONE) ? "(+)" : ""));
		}
		if (be16toh(lldp_sys_caps->available_caps) & LLDP_SYSCAPS_DOCSIS)
		{
			s_caps.assign(vas("docsis%s ",
					(be16toh(lldp_sys_caps->enabled_caps) & LLDP_SYSCAPS_DOCSIS) ? "(+)" : ""));
		}
		if (be16toh(lldp_sys_caps->available_caps) & LLDP_SYSCAPS_STA_ONLY)
		{
			s_caps.assign(vas("sta-only%s ",
					(be16toh(lldp_sys_caps->enabled_caps) & LLDP_SYSCAPS_STA_ONLY) ? "(+)" : ""));
		}

		info.append(vas("clldptlv(%p) LLDP -SYS-CAPS- TLV: %s ",
				this,
				s_caps.c_str()));
		return info.c_str();
	};
};


class clldptlv_mgmt_addr : public clldptlv {
public:
	clldptlv_mgmt_addr(
			uint8_t mgmt_addr_subtype,
			const char* mgmt_addr,
			size_t mgmt_addr_len,
			uint8_t iface_numbering_subtype,
			uint32_t iface_number,
			const char *oid,
			size_t oid_len) :
		clldptlv(sizeof(struct lldp_tlv_hdr_t) + LLDP_TLV_MGMT_ADDR_MAX_LEN)
	{
		throw eNotImplemented();
	};
	clldptlv_mgmt_addr(clldptlv const& tlv) :
		clldptlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string s_idstr((const char*)lldp_tlv->body, (get_hdr_length() - 1));
		info.append(vas("clldptlv(%p) LLDP -MGMT-ADDR- TLV: %s ",
				this));
		return info.c_str();
	};
};

}; // end of namespace

#endif
