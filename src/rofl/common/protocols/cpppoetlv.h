/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CPPPOETLV_H
#define CPPPOETLV_H 1

#include <string>
#include <limits>

#include "../cmemory.h"
#include "../cerror.h"
#include "../cvastring.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <endian.h>
#ifndef htobe16
#include "../endian_conversion.h"
#endif
#ifdef __cplusplus
}
#endif

#include "../coflist.h"

class ePPPoEbase : public cerror {};
class ePPPoEInval : public ePPPoEbase {};
class ePPPoENotFound : public ePPPoEbase {};


class cpppoetlv {
public:

	struct pppoe_tag_hdr_t {
		uint16_t type;
		uint16_t length;
		uint8_t data[0];
	} __attribute__((packed));

	struct pppoe_tag_vendor_hdr_t {
		uint16_t type;
		uint16_t length;
		uint32_t vendor_id;
	} __attribute__((packed));



	// PPPoE tags
	enum pppoe_tag_t {
		PPPOE_TAG_END_OF_LIST = 0x0000,
		PPPOE_TAG_SERVICE_NAME = 0x0101,
		PPPOE_TAG_AC_NAME = 0x0102,
		PPPOE_TAG_HOST_UNIQ = 0x0103,
		PPPOE_TAG_AC_COOKIE = 0x0104,
		PPPOE_TAG_VENDOR_SPECIFIC = 0x0105,
		PPPOE_TAG_RELAY_SESSION_ID = 0x0110,
		PPPOE_TAG_SERVICE_NAME_ERROR = 0x0201,
		PPPOE_TAG_AC_SYSTEM_ERROR = 0x0202,
		PPPOE_TAG_GENERIC_ERROR = 0x0203,
	};

public: // data structures

#define DEFAULT_PPPOE_TLV_SIZE 128

	cmemory tlvmem;

	struct pppoe_tag_hdr_t 			*tlv_hdr;

public: // methods

	/**
	 *
	 */
	cpppoetlv(
			size_t tlvlen = DEFAULT_PPPOE_TLV_SIZE);

	/**
	 *
	 */
	cpppoetlv(
			uint8_t *tlv, size_t tlvlen);

	/**
	 *
	 */
	cpppoetlv(
			uint16_t type,
			std::string const& tag_value) throw (ePPPoEInval);

	/**
	 *
	 */
	cpppoetlv(
			uint16_t type,
			cmemory const& tag_value) throw (ePPPoEInval);

	/**
	 *
	 */
	cpppoetlv(
			cpppoetlv const& tlv);

	/**
	 *
	 */
	virtual
	~cpppoetlv();

	/**
	 *
	 */
	cpppoetlv&
	operator= (cpppoetlv const& tlv);

	/**
	 *
	 */
	size_t
	length();

	/**
	 *
	 */
	struct pppoe_tag_hdr_t*
	pack(
			struct pppoe_tag_hdr_t* tlv,
			size_t tlvlen) throw (ePPPoEInval);

	/**
	 *
	 */
	struct pppoe_tag_hdr_t*
	unpack(
			struct pppoe_tag_hdr_t *tlv,
			size_t tlvlen) throw (ePPPoEInval);

	/**
	 *
	 */
	virtual const char*
	c_str();

	/**
	 *
	 */
	void
	set_hdr_type(uint16_t type);

	/**
	 *
	 */
	uint16_t
	get_hdr_type() const;

	/**
	 *
	 */
	void
	set_hdr_length(uint16_t len);

	/**
	 *
	 */
	uint16_t
	get_hdr_length() const;

protected: // data structures

	std::string info;

};


class cpppoe_tlv_vendor_tag :
	public cmemory
{
public:

		struct pppoe_vendor_subtlv_t {
			uint8_t type;
			uint8_t length;
			uint8_t data[0];
		};

		struct pppoe_vendor_subtlv_t *tlv;

public:
		/**
		 *
		 */
		cpppoe_tlv_vendor_tag(
				uint8_t type = 0,
				cmemory const& body = cmemory(0)) :
					cmemory(2 * sizeof(struct pppoe_vendor_subtlv_t) + body.memlen()),
					tlv((struct pppoe_vendor_subtlv_t*)somem())
		{
			tlv->type = type;
			tlv->length = + 2 * sizeof(uint8_t) + body.memlen(); // RFC 3046
			memcpy(somem() + 2*sizeof(uint8_t), body.somem(), body.memlen());
		};

		/**
		 *
		 */
		virtual
		~cpppoe_tlv_vendor_tag()
		{ };
};


class cpppoetlv_end : public cpppoetlv {
public:
	cpppoetlv_end() :
		cpppoetlv(sizeof(struct pppoe_tag_hdr_t))
	{
		set_hdr_type(PPPOE_TAG_END_OF_LIST);
		set_hdr_length(0);
	};
	cpppoetlv_end(cpppoetlv const& tlv) :
		cpppoetlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		info.append(vas("cpppoetlv(%p) PPPoE -END-OF-LIST- TLV", this));
		return info.c_str();
	};
};


class cpppoetlv_service_name : public cpppoetlv {
public:
	cpppoetlv_service_name(std::string const& svcname) :
		cpppoetlv(PPPOE_TAG_SERVICE_NAME, svcname) {};
	cpppoetlv_service_name(cpppoetlv const& tlv) :
		cpppoetlv(tlv) {};
	std::string
	get_svc_name()
	{
		std::string svcname((const char*)tlv_hdr->data, get_hdr_length());
		return svcname;
	};
	virtual const char*
	c_str()
	{
		cvastring vas;
		info.append(vas("cpppoetlv_service_name(%p) PPPoE -Service-Name- TLV [%s]", this, get_svc_name().c_str()));
		return info.c_str();
	};
};


class cpppoetlv_ac_name : public cpppoetlv {
public:
	cpppoetlv_ac_name(std::string const& acname) :
		cpppoetlv(PPPOE_TAG_AC_NAME, acname) {};
	cpppoetlv_ac_name(cpppoetlv const& tlv) :
		cpppoetlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string acname((const char*)tlv_hdr->data, get_hdr_length());
		info.append(vas("cpppoetlv_ac_name(%p) PPPoE -AC-Name- TLV [%s]", this, acname.c_str()));
		return info.c_str();
	};
};


class cpppoetlv_host_uniq : public cpppoetlv {
public:
	cpppoetlv_host_uniq(cmemory const& opaque) :
		cpppoetlv(PPPOE_TAG_HOST_UNIQ, opaque) {};
	cpppoetlv_host_uniq(cpppoetlv const& tlv) :
		cpppoetlv(tlv) {};
	cmemory
	get_host_uniq()
	{
		cmemory hostuniq((uint8_t*)tlv_hdr->data, get_hdr_length());
		return hostuniq;
	};
	virtual const char*
	c_str()
	{
		cvastring vas;
		cmemory opaque(tlv_hdr->data, get_hdr_length());
		info.append(vas("cpppoetlv_host_uniq(%p) PPPoE -Host-Uniq- TLV [%s]", this, opaque.c_str()));
		return info.c_str();
	};
};


class cpppoetlv_ac_cookie : public cpppoetlv {
public:
	cpppoetlv_ac_cookie(cmemory const& opaque) :
		cpppoetlv(PPPOE_TAG_AC_COOKIE, opaque) {};
	cpppoetlv_ac_cookie(cpppoetlv const& tlv) :
		cpppoetlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		cmemory opaque(tlv_hdr->data, get_hdr_length());
		info.append(vas("cpppoetlv_ac_cookie(%p) PPPoE -AC-Cookie- TLV [%s]", this, opaque.c_str()));
		return info.c_str();
	};
};


// TODO: vendor specific


class cpppoetlv_relay_session_id : public cpppoetlv {
public:
	cpppoetlv_relay_session_id(cmemory const& opaque) :
		cpppoetlv(PPPOE_TAG_RELAY_SESSION_ID, opaque) {};
	cpppoetlv_relay_session_id(cpppoetlv const& tlv) :
		cpppoetlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		cmemory opaque(tlv_hdr->data, get_hdr_length());
		info.append(vas("cpppoetlv_relay_session_id(%p) PPPoE -Relay-Session-ID- TLV [%s]", this, opaque.c_str()));
		return info.c_str();
	};
};


class cpppoetlv_service_name_error : public cpppoetlv {
public:
	cpppoetlv_service_name_error(std::string const& s_err) :
		cpppoetlv(PPPOE_TAG_SERVICE_NAME_ERROR, s_err) {};
	cpppoetlv_service_name_error(cpppoetlv const& tlv) :
		cpppoetlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string value((const char*)tlv_hdr->data, get_hdr_length());
		info.append(vas("cpppoetlv_service_name_error(%p) PPPoE -Service-Name-Error- TLV [%s]", this, value.c_str()));
		return info.c_str();
	};
};


class cpppoetlv_ac_system_error : public cpppoetlv {
public:
	cpppoetlv_ac_system_error(std::string const& s_err) :
		cpppoetlv(PPPOE_TAG_AC_SYSTEM_ERROR, s_err) {};
	cpppoetlv_ac_system_error(cpppoetlv const& tlv) :
		cpppoetlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string value((const char*)tlv_hdr->data, get_hdr_length());
		info.append(vas("cpppoetlv_ac_system_error(%p) PPPoE -AC-System-Error- TLV [%s]", this, value.c_str()));
		return info.c_str();
	};
};


class cpppoetlv_generic_error : public cpppoetlv {
public:
	cpppoetlv_generic_error(std::string const& s_err) :
		cpppoetlv(PPPOE_TAG_GENERIC_ERROR, s_err) {};
	cpppoetlv_generic_error(cpppoetlv const& tlv) :
		cpppoetlv(tlv) {};
	virtual const char*
	c_str()
	{
		cvastring vas;
		std::string value((const char*)tlv_hdr->data, get_hdr_length());
		info.append(vas("cpppoetlv_generic_error(%p) PPPoE -Generic-Error- TLV [%s]", this, value.c_str()));
		return info.c_str();
	};
};


class cpppoetlv_access_loop_id : public cpppoetlv {

	struct pppoe_tag_vendor_hdr_t *tlv;

#define PPPOE_TAG_VENDORID_BBF_IANA 0x00000DE9

public:
	cpppoetlv_access_loop_id(
			cmemory const& circuit_id,
			cmemory const& remote_id) :
				cpppoetlv(sizeof(struct pppoe_tag_vendor_hdr_t))
	{
		tlv = (struct pppoe_tag_vendor_hdr_t*)tlvmem.memlen();

		cpppoe_tlv_vendor_tag circuit(0x01, circuit_id);
		tlvmem += circuit;

		cpppoe_tlv_vendor_tag remote(0x02, remote_id);
		tlvmem += remote;

		set_hdr_type(PPPOE_TAG_VENDOR_SPECIFIC);
		set_hdr_length(2*sizeof(uint16_t) + circuit.memlen() + remote.memlen());
		set_hdr_vend_id(PPPOE_TAG_VENDORID_BBF_IANA);
	};
	cpppoetlv_access_loop_id(cpppoetlv const& tlv) :
		cpppoetlv(tlv) {};
	/**
	 *
	 */
	void
	set_hdr_vend_id(uint32_t vend_id)
	{
		tlv = (struct pppoe_tag_vendor_hdr_t*)tlvmem.somem();

		tlv->vendor_id = htobe32(vend_id);
	};
};

#if 0
+--------------+--------------+--------------+--------------+
| 0x0105 (Vendor-Specific)
|
TAG_LENGTH
|
+--------------+--------------+--------------+--------------+
| 0x00000DE9 (3561 decimal, i.e. “BBF” IANA entry)
|
+--------------+--------------+--------------+--------------+
| 0x01
| length
| Agent Circuit ID value...
|
+--------------+--------------+--------------+--------------+
| Agent Circuit ID value (con’t) ...
|
+--------------+--------------+--------------+--------------+
| 0x02
| length
| Agent Remote ID value...
|
+--------------+--------------+--------------+--------------+
| Agent Remote ID value (con’t) ...
|
+--------------+--------------+--------------+--------------+
#endif




#endif
