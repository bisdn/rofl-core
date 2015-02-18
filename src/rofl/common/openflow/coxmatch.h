/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmatch.h
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#ifndef COXMATCH_H_
#define COXMATCH_H_

#include <string>
#include <ostream>
#include <endian.h>
#include <pthread.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/caddress.h"
#include "rofl/common/fframe.h"
#include "rofl/common/logging.h"
#include "rofl/common/thread_helper.h"

namespace rofl {
namespace openflow {

/* error classes */
class eOxmBase 				: public RoflException {
public:
	eOxmBase(const std::string& __arg) : RoflException(__arg) {};
};
class eOxmInval 			: public eOxmBase {
public:
	eOxmInval(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmNotFound 			: public eOxmBase {
public:
	eOxmNotFound(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmBadLen 			: public eOxmBase {
public:
	eOxmBadLen(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmInvalType 		: public eOxmBase {
public:
	eOxmInvalType(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmInvalClass 		: public eOxmBase {
public:
	eOxmInvalClass(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmHeaderInval 		: public eOxmBase {
public:
	eOxmHeaderInval(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmActionNotFound 	: public eOxmBase {
public:
	eOxmActionNotFound(const std::string& __arg) : eOxmBase(__arg) {};
};
class eOxmBadExperimenter 	: public eOxmBase {
public:
	eOxmBadExperimenter(const std::string& __arg) : eOxmBase(__arg) {};
};


/**
 *
 */
class coxmatch :
	public rofl::cmemory
{
	PthreadRwLock		oxmlock;

public:

	enum coxmatch_bit_t {
		COXMATCH_8BIT	= (1 << 0),
		COXMATCH_16BIT	= (1 << 1),
		COXMATCH_24BIT	= (1 << 2),
		COXMATCH_32BIT	= (1 << 3),
		COXMATCH_48BIT	= (1 << 4),
		COXMATCH_64BIT	= (1 << 5),
	};

	/**
	 *
	 */
	coxmatch();

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id);

	/**
	 *
	 */
	coxmatch(
			uint8_t* oxm_hdr, size_t oxm_len);

	/**
	 * @brief	coxmatch base class for single 8bit values
	 */
	coxmatch(
			uint32_t oxm_id, uint8_t value, enum coxmatch_bit_t bits);

	/**
	 * @brief	coxmatch base class for masked 8bit values
	 */
	coxmatch(
			uint32_t oxm_id, uint8_t value, uint8_t mask, enum coxmatch_bit_t bits);

	/**
	 * @brief	coxmatch base class for single 16bit values
	 */
	coxmatch(
			uint32_t oxm_id, uint16_t value, enum coxmatch_bit_t bits);

	/**
	 * @brief	coxmatch base class for masked 16bit values
	 */
	coxmatch(
			uint32_t oxm_id, uint16_t value, uint16_t mask, enum coxmatch_bit_t bits);

	/**
	 * @brief	coxmatch base class for single 24bit and 32bit values
	 */
	coxmatch(
			uint32_t oxm_id, uint32_t value, enum coxmatch_bit_t bits);

	/**
	 * @brief	coxmatch base class for masked 24bit and 32bit values
	 */
	coxmatch(
			uint32_t oxm_id, uint32_t value, uint32_t mask, enum coxmatch_bit_t bits);

	/**
	 * @brief	coxmatch base class for single 48bit and 64bit values
	 */
	coxmatch(
			uint32_t oxm_id, uint64_t value, enum coxmatch_bit_t bits);

	/**
	 * @brief	coxmatch base class for masked 48bit and 64bit values
	 */
	coxmatch(
			uint32_t oxm_id, uint64_t value, uint64_t mask, enum coxmatch_bit_t bits);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, const caddress_in4& value);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, const caddress_in4& value, const caddress_in4& mask);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, rofl::caddress_in6 const& value);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, rofl::caddress_in6 const& value, rofl::caddress_in6 const& mask);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, const cmacaddr& value);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, const cmacaddr& value, const cmacaddr& mask);

	/**
	 *
	 */
	coxmatch(
			const coxmatch& oxm);

	/**
	 *
	 */
	virtual
	~coxmatch();

	/**
	 *
	 */
	coxmatch&
	operator= (
			const coxmatch& oxm);

	/**
	 *
	 */
	bool
	operator== (
			const coxmatch& oxm) const;

	/**
	 *
	 */
	bool
	operator!= (
			const coxmatch& oxm) const;

	/**
	 *
	 */
	bool
	operator< (
			const coxmatch& oxm);

public:

	/**
	 *
	 */
	virtual void
	clear() { rofl::cmemory::clear(); };

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

	/**
	 *
	 */
	void
	set_oxm_id(
			uint32_t oxm_id);


	/**
	 *
	 */
	uint32_t
	get_oxm_id() const;

	/**
	 *
	 */
	void
	set_oxm_class(
			uint16_t oxm_class);

	/**
	 *
	 */
	uint16_t
	get_oxm_class() const;

	/**
	 *
	 */
	void
	set_oxm_field(
			uint8_t oxm_field);

	/**
	 *
	 */
	uint8_t
	get_oxm_field() const;

	/**
	 *
	 */
	void
	set_oxm_hasmask(
			bool oxm_hasmask);

	/**
	 *
	 */
	bool
	get_oxm_hasmask() const;

	/**
	 *
	 */
	void
	set_oxm_length(
			uint8_t oxm_len);

	/**
	 *
	 */
	uint8_t
	get_oxm_length();

public:

	uint8_t
	get_u8value() const;

	uint8_t
	get_u8mask() const;

	uint8_t
	get_u8masked_value() const;

	uint16_t
	get_u16value() const;

	uint16_t
	get_u16mask() const;

	uint16_t
	get_u16masked_value() const;

	uint32_t
	get_u24value() const;

	uint32_t
	get_u24mask() const;

	uint32_t
	get_u24masked_value() const;

	uint32_t
	get_u32value() const;

	uint32_t
	get_u32mask() const;

	uint32_t
	get_u32masked_value() const;

	rofl::caddress_in4
	get_u32value_as_addr() const;

	rofl::caddress_in4
	get_u32mask_as_addr() const;

	rofl::caddress_in4
	get_u32masked_value_as_addr() const;

	rofl::cmacaddr
	get_u48value_as_lladdr() const;

	rofl::cmacaddr
	get_u48mask_as_lladdr() const;

	rofl::cmacaddr
	get_u48masked_value_as_lladdr() const;

	uint64_t
	get_u48value() const;

	uint64_t
	get_u48mask() const;

	uint64_t
	get_u48masked_value() const;

	uint64_t
	get_u64value() const;

	uint64_t
	get_u64mask() const;

	uint64_t
	get_u64masked_value() const;

	rofl::caddress_in6
	get_u128value() const;

	rofl::caddress_in6
	get_u128mask() const;

	rofl::caddress_in6
	get_u128masked_value() const;

	void
	set_u8value(
			uint8_t value);

	void
	set_u8mask(
			uint8_t mask);

	void
	set_u16value(
			uint16_t value);

	void
	set_u16mask(
			uint16_t mask);

	void
	set_u24value(
			uint32_t value);

	void
	set_u24mask(
			uint32_t mask);

	void
	set_u32value(
			uint32_t value);

	void
	set_u32mask(
			uint32_t mask);

	void
	set_u32value(
			const rofl::caddress_in4& addr);

	void
	set_u32mask(
			const rofl::caddress_in4& mask);

	void
	set_u48value(
			const rofl::cmacaddr& addr);

	void
	set_u48mask(
			const rofl::cmacaddr& mask);

	void
	set_u48value(
			uint64_t value);

	void
	set_u48mask(
			uint64_t mask);

	void
	set_u64value(
			uint64_t value);

	void
	set_u64mask(
			uint64_t mask);

	void
	set_u128value(
			const rofl::caddress_in6& addr);

	void
	set_u128mask(
			const rofl::caddress_in6& mask);


public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch& oxm) {
		os << rofl::indent(0) << "<coxmatch oxm-id: 0x" << std::hex << oxm.get_oxm_id() << std::dec << " >" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<rofl::cmemory const&>( oxm );
		return os;
	};

};


class coxmatch_find_type
{
		uint16_t oxm_class;
		uint8_t  oxm_field;
public:
		/**
		 *
		 */
		coxmatch_find_type(
				uint16_t oxm_class,
				uint8_t oxm_field) :
					oxm_class(oxm_class), oxm_field(oxm_field) {};
		/**
		 *
		 */
		bool
		operator() (
				uint16_t __oxm_class,
				uint8_t __oxm_field) const
		{
			return ((oxm_class == __oxm_class) && (oxm_field == __oxm_field));
		};

		/**
		 *
		 */
		bool
		operator() (
				coxmatch const& match) const
		{
			return ((oxm_class == match.get_oxm_class()) &&
					 (oxm_field == match.get_oxm_field()));
		};
};


/**
 * @brief	OXM_OF_IN_PORT
 */
class coxmatch_ofb_in_port : public coxmatch {
public:
	coxmatch_ofb_in_port(
			uint32_t port_no) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IN_PORT, port_no, COXMATCH_32BIT)
	{};
	coxmatch_ofb_in_port(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_in_port()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_in_port& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<in-port: " << std::hex << (int)oxm.get_u32value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IN_PHY_PORT
 */
class coxmatch_ofb_in_phy_port : public coxmatch {
public:
	coxmatch_ofb_in_phy_port(
			uint32_t port_no) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT, port_no, COXMATCH_32BIT)
	{};
	coxmatch_ofb_in_phy_port(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_in_phy_port()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_in_phy_port& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<in-phy-port: " << std::hex << (int)oxm.get_u32value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_METADATA
 */
class coxmatch_ofb_metadata : public coxmatch {
public:
	coxmatch_ofb_metadata(
			uint64_t metadata) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_METADATA, metadata, COXMATCH_64BIT)
	{};
	coxmatch_ofb_metadata(
			uint64_t metadata, uint64_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_METADATA_MASK, metadata, mask, COXMATCH_64BIT)
	{};
	coxmatch_ofb_metadata(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_metadata()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_metadata& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << std::hex;
		os << indent(2) << "<metadata: "
		<< (unsigned long long)oxm.get_u64value() << "/" << (unsigned long long)oxm.get_u64mask()
		<< std::dec
		<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ETH_DST
 */
class coxmatch_ofb_eth_dst : public coxmatch {
public:
	coxmatch_ofb_eth_dst(
			const cmacaddr& maddr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_DST, maddr)
	{};
	coxmatch_ofb_eth_dst(
			const cmacaddr& maddr, const cmacaddr& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK, maddr, mask)
	{};
	coxmatch_ofb_eth_dst(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_dst& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<eth-dst: "
				<< oxm.get_u48value_as_lladdr().str() << "/"
				<< oxm.get_u48mask_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ETH_SRC
 */
class coxmatch_ofb_eth_src : public coxmatch {
public:
	coxmatch_ofb_eth_src(
			const cmacaddr& maddr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_SRC, maddr)
	{};
	coxmatch_ofb_eth_src(
			const cmacaddr& maddr, const cmacaddr& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_SRC_MASK, maddr, mask)
	{};
	coxmatch_ofb_eth_src(
			const coxmatch& oxm) : coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_src& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<eth-src: "
				<< oxm.get_u48value_as_lladdr().str() << "/"
				<< oxm.get_u48mask_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ETH_TYPE
 */
class coxmatch_ofb_eth_type : public coxmatch {
public:
	coxmatch_ofb_eth_type(
			uint16_t dl_type) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE, dl_type, COXMATCH_16BIT)
	{};
	coxmatch_ofb_eth_type(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_eth_type()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_eth_type& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<eth-type: 0x" << std::hex << (int)oxm.get_u16value() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_VID
 */
class coxmatch_ofb_vlan_vid : public coxmatch {
public:
	coxmatch_ofb_vlan_vid(
			uint16_t vid) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_VLAN_VID, vid, COXMATCH_16BIT)
	{};
	coxmatch_ofb_vlan_vid(
			uint16_t vid, uint16_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK, vid, mask, COXMATCH_16BIT)
	{};
	coxmatch_ofb_vlan_vid(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_vid()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_vid& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << rofl::indent(2) << "<coxmatch_ofb_vlan_vid >" << std::endl;
		os << rofl::indent(4) << "<vlan-vid: 0x" << std::hex << (int)oxm.get_u16value() << "/0x" << (int)oxm.get_u16mask() << std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_VID
 */
class coxmatch_ofb_vlan_untagged : public coxmatch {
public:
	coxmatch_ofb_vlan_untagged() :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_VLAN_VID,
										(uint16_t)rofl::openflow::OFPVID_NONE, COXMATCH_16BIT)
	{};
	coxmatch_ofb_vlan_untagged(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_untagged()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_untagged& oxm) {
		os << dynamic_cast<coxmatch_ofb_vlan_vid const&>(oxm);
		os << rofl::indent(2) << "<vlan-untagged >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_VID
 */
class coxmatch_ofb_vlan_present : public coxmatch {
public:
	coxmatch_ofb_vlan_present() :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK,
										(uint16_t)rofl::openflow::OFPVID_PRESENT,
										(uint16_t)rofl::openflow::OFPVID_PRESENT, COXMATCH_16BIT)
	{};
	coxmatch_ofb_vlan_present(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_present()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_present& oxm) {
		os << dynamic_cast<coxmatch_ofb_vlan_vid const&>(oxm);
		os << rofl::indent(2) << "<vlan-present >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_VLAN_PCP
 */
class coxmatch_ofb_vlan_pcp : public coxmatch {
public:
	coxmatch_ofb_vlan_pcp(
			uint8_t pcp) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP, pcp, COXMATCH_8BIT)
	{};
	coxmatch_ofb_vlan_pcp(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_vlan_pcp()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_vlan_pcp& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<vlan-pcp: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IP_DSCP
 */
class coxmatch_ofb_ip_dscp : public coxmatch {
public:
	coxmatch_ofb_ip_dscp(
			uint8_t dscp) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IP_DSCP, dscp, COXMATCH_8BIT)
	{};
	coxmatch_ofb_ip_dscp(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ip_dscp()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ip_dscp& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ip-dscp: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IP_ECN
 */
class coxmatch_ofb_ip_ecn : public coxmatch {
public:
	coxmatch_ofb_ip_ecn(
			uint8_t ecn) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IP_ECN, ecn, COXMATCH_8BIT)
	{};
	coxmatch_ofb_ip_ecn(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ip_ecn()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ip_ecn& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ip-ecn: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IP_PROTO
 */
class coxmatch_ofb_ip_proto : public coxmatch {
public:
	coxmatch_ofb_ip_proto(
			uint8_t proto) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IP_PROTO, proto, COXMATCH_8BIT)
	{};
	coxmatch_ofb_ip_proto(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ip_proto()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ip_proto& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ip-proto: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV4_SRC
 */
class coxmatch_ofb_ipv4_src : public coxmatch {
public:
	coxmatch_ofb_ipv4_src(
			uint32_t src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC, src, COXMATCH_32BIT)
	{};
	coxmatch_ofb_ipv4_src(
			uint32_t src, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC_MASK, src, mask, COXMATCH_32BIT)
	{};
	coxmatch_ofb_ipv4_src(
			const rofl::caddress_in4& src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC, src)
	{};
	coxmatch_ofb_ipv4_src(
			const rofl::caddress_in4& src, const rofl::caddress_in4& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC_MASK, src, mask)
	{};
	coxmatch_ofb_ipv4_src(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv4_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv4_src& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ipv4-src: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV4_DST
 */
class coxmatch_ofb_ipv4_dst : public coxmatch {
public:
	coxmatch_ofb_ipv4_dst(
			uint32_t dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_DST, dst, COXMATCH_32BIT)
	{};
	coxmatch_ofb_ipv4_dst(
			uint32_t dst, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK, dst, mask, COXMATCH_32BIT)
	{};
	coxmatch_ofb_ipv4_dst(
			const caddress_in4& dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_DST, dst)
	{};
	coxmatch_ofb_ipv4_dst(
			const caddress_in4& dst, const caddress_in4& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK, dst, mask)
	{};
	coxmatch_ofb_ipv4_dst(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv4_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv4_dst& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ipv4-dst: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_SRC
 */
class coxmatch_ofb_ipv6_src : public coxmatch {
public:
	coxmatch_ofb_ipv6_src(
			const rofl::caddress_in6& src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC, src)
	{};
	coxmatch_ofb_ipv6_src(
			const rofl::caddress_in6& src, const rofl::caddress_in6& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC_MASK, src, mask)
	{};
	coxmatch_ofb_ipv6_src(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_src& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ipv6-src: " << oxm.get_u128value().str() << "/";
		os << oxm.get_u128mask().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_DST
 */
class coxmatch_ofb_ipv6_dst : public coxmatch {
public:
	coxmatch_ofb_ipv6_dst(
			const rofl::caddress_in6& dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_DST, dst)
	{};
	coxmatch_ofb_ipv6_dst(
			const rofl::caddress_in6& dst, const rofl::caddress_in6& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_DST_MASK, dst, mask)
	{};
	coxmatch_ofb_ipv6_dst(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_dst& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ipv6-dst: " << oxm.get_u128value().str() << "/";
		os << oxm.get_u128mask().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_ND_TARGET
 */
class coxmatch_ofb_ipv6_nd_target : public coxmatch {
public:
	coxmatch_ofb_ipv6_nd_target(
			const rofl::caddress_in6& nd_target) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET, nd_target)
	{};
	coxmatch_ofb_ipv6_nd_target(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_nd_target()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_nd_target& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << rofl::indent(2) << "<ipv6-nd-target: " << oxm.get_u128value().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_TCP_SRC
 */
class coxmatch_ofb_tcp_src : public coxmatch {
public:
	coxmatch_ofb_tcp_src(
			uint16_t src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_TCP_SRC, src, COXMATCH_16BIT)
	{};
	coxmatch_ofb_tcp_src(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_tcp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_tcp_src& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<tcp-src: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_TCP_DST
 */
class coxmatch_ofb_tcp_dst : public coxmatch {
public:
	coxmatch_ofb_tcp_dst(
			uint16_t dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_TCP_DST, dst, COXMATCH_16BIT)
	{};
	coxmatch_ofb_tcp_dst(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_tcp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_tcp_dst& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<tcp-dst: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_UDP_SRC
 */
class coxmatch_ofb_udp_src : public coxmatch {
public:
	coxmatch_ofb_udp_src(
			uint16_t src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_UDP_SRC, src, COXMATCH_16BIT)
	{};
	coxmatch_ofb_udp_src(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_udp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_udp_src& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<udp-src: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_UDP_DST
 */
class coxmatch_ofb_udp_dst : public coxmatch {
public:
	coxmatch_ofb_udp_dst(
			uint16_t dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_UDP_DST, dst, COXMATCH_16BIT)
	{};
	coxmatch_ofb_udp_dst(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_udp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_udp_dst& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<udp-dst: "	<< (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_SCTP_SRC
 */
class coxmatch_ofb_sctp_src : public coxmatch {
public:
	coxmatch_ofb_sctp_src(
			uint16_t src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC, src, COXMATCH_16BIT)
	{};
	coxmatch_ofb_sctp_src(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_sctp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_sctp_src& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<sctp-src: " << (int)oxm.get_u16value()	<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_SCTP_DST
 */
class coxmatch_ofb_sctp_dst : public coxmatch {
public:
	coxmatch_ofb_sctp_dst(
			uint16_t dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_SCTP_DST, dst, COXMATCH_16BIT)
	{};
	coxmatch_ofb_sctp_dst(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_sctp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_sctp_dst& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<sctp-dst: " << (int)oxm.get_u16value()	<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ICMPV4_TYPE
 */
class coxmatch_ofb_icmpv4_type : public coxmatch {
public:
	coxmatch_ofb_icmpv4_type(
			uint8_t type) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE, type, COXMATCH_8BIT)
	{};
	coxmatch_ofb_icmpv4_type(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv4_type()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv4_type& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<icmpv4-type: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ICMPV4_CODE
 */
class coxmatch_ofb_icmpv4_code : public coxmatch {
public:
	coxmatch_ofb_icmpv4_code(
			uint8_t code) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE, code, COXMATCH_8BIT)
	{};
	coxmatch_ofb_icmpv4_code(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv4_code()
	{};
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv4_code& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<icmpv4-code: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ARP_OP
 */
class coxmatch_ofb_arp_opcode : public coxmatch {
public:
	coxmatch_ofb_arp_opcode(
			uint16_t opcode) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_OP, opcode, COXMATCH_16BIT)
	{};
	coxmatch_ofb_arp_opcode(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_opcode()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_opcode& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<arp-opcode: " << (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_SPA
 */
class coxmatch_ofb_arp_spa : public coxmatch {
public:
	coxmatch_ofb_arp_spa(
			uint32_t spa) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SPA, spa, COXMATCH_32BIT)
	{};
	coxmatch_ofb_arp_spa(
			uint32_t spa, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SPA_MASK, spa, mask, COXMATCH_32BIT)
	{};
	coxmatch_ofb_arp_spa(
			const caddress_in4& spa) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SPA, spa)
	{};
	coxmatch_ofb_arp_spa(
			const caddress_in4& spa, const caddress_in4& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SPA_MASK, spa, mask)
	{};
	coxmatch_ofb_arp_spa(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_spa()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_spa& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<arp-spa: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_TPA
 */
class coxmatch_ofb_arp_tpa : public coxmatch {
public:
	coxmatch_ofb_arp_tpa(
			uint32_t tpa) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_TPA, tpa, COXMATCH_32BIT)
	{};
	coxmatch_ofb_arp_tpa(
			uint32_t tpa, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_TPA_MASK, tpa, mask, COXMATCH_32BIT)
	{};
	coxmatch_ofb_arp_tpa(
			const caddress_in4& tpa) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_TPA, tpa)
	{};
	coxmatch_ofb_arp_tpa(
			const caddress_in4& tpa, const caddress_in4& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_TPA_MASK, tpa, mask)
	{};
	coxmatch_ofb_arp_tpa(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_tpa()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_tpa& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<arp-tpa: " << oxm.get_u32value_as_addr().str() << "/";
		os << oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ARP_SHA
 */
class coxmatch_ofb_arp_sha : public coxmatch {
public:
	coxmatch_ofb_arp_sha(
			uint64_t lladdr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SHA, lladdr, COXMATCH_48BIT)
	{};
	coxmatch_ofb_arp_sha(
			uint64_t lladdr, uint64_t llmask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SHA_MASK, lladdr, llmask, COXMATCH_48BIT)
	{};
	coxmatch_ofb_arp_sha(
			const cmacaddr& lladdr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SHA, lladdr)
	{};
	coxmatch_ofb_arp_sha(
			const cmacaddr& lladdr, const cmacaddr& llmask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SHA_MASK, lladdr, llmask)
	{};
	coxmatch_ofb_arp_sha(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_sha()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_sha& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<arp-sha: "
				<< oxm.get_u48value_as_lladdr().str() << "/"
				<< oxm.get_u48mask_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_ARP_THA
 */
class coxmatch_ofb_arp_tha : public coxmatch {
public:
	coxmatch_ofb_arp_tha(
			uint64_t lladdr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_THA, lladdr, COXMATCH_48BIT)
	{};
	coxmatch_ofb_arp_tha(
			uint64_t lladdr, uint64_t llmask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_THA_MASK, lladdr, llmask, COXMATCH_48BIT)
	{};
	coxmatch_ofb_arp_tha(
			const cmacaddr& lladdr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_THA, lladdr)
	{};
	coxmatch_ofb_arp_tha(
			const cmacaddr& lladdr, const cmacaddr& llmask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_THA_MASK, lladdr, llmask)
	{};
	coxmatch_ofb_arp_tha(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_arp_tha()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_arp_tha& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<arp-tha: "
				<< oxm.get_u48value_as_lladdr().str() << "/"
				<< oxm.get_u48mask_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_FLABEL
 */
class coxmatch_ofb_ipv6_flabel : public coxmatch {
public:
	/** constructor
	 */
	coxmatch_ofb_ipv6_flabel(
			uint32_t flow_label) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL, flow_label, COXMATCH_32BIT)
	{};
	coxmatch_ofb_ipv6_flabel(
			uint32_t flow_label, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL_MASK, flow_label, mask, COXMATCH_32BIT)
	{};
	coxmatch_ofb_ipv6_flabel(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_flabel()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_flabel& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ipv6-flabel: 0x" << std::hex
						<< (int)oxm.get_u32value() << "/0x" << (int)oxm.get_u32mask()
						<< std::dec << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ICMPV6_TYPE
 */
class coxmatch_ofb_icmpv6_type : public coxmatch {
public:
	coxmatch_ofb_icmpv6_type(
			uint8_t type) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE, type, COXMATCH_8BIT)
	{};
	coxmatch_ofb_icmpv6_type(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv6_type()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv6_type& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<icmpv6-type: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_ICMPV6_CODE
 */
class coxmatch_ofb_icmpv6_code : public coxmatch {
public:
	coxmatch_ofb_icmpv6_code(
			uint8_t code) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE, code, COXMATCH_8BIT)
	{};
	coxmatch_ofb_icmpv6_code(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_icmpv6_code()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_icmpv6_code& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<icmpv6-code: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_IPV6_ND_SLL
 */
class coxmatch_ofb_ipv6_nd_sll : public coxmatch {
public:
	coxmatch_ofb_ipv6_nd_sll(
			uint64_t lladdr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL, lladdr, COXMATCH_48BIT)
	{};
	coxmatch_ofb_ipv6_nd_sll(
			const cmacaddr& addr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL, addr)
	{};
	coxmatch_ofb_ipv6_nd_sll(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_nd_sll()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_nd_sll& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ipv6-nd-sll: "
				<< oxm.get_u48value_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_ND_TLL
 */
class coxmatch_ofb_ipv6_nd_tll : public coxmatch {
public:
	coxmatch_ofb_ipv6_nd_tll(
			uint64_t lladdr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL, lladdr, COXMATCH_48BIT)
	{};
	coxmatch_ofb_ipv6_nd_tll(
			const cmacaddr& addr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL, addr)
	{};
	coxmatch_ofb_ipv6_nd_tll(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_nd_tll()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_nd_tll& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ipv6-nd-tll: "
				<< oxm.get_u48value_as_lladdr().str()
				<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_MPLS_LABEL
 */
class coxmatch_ofb_mpls_label : public coxmatch {
public:
	coxmatch_ofb_mpls_label(
			uint32_t mpls_label) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL, mpls_label, COXMATCH_32BIT)
	{};
	coxmatch_ofb_mpls_label(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_mpls_label()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_mpls_label& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<mpls-label: 0x" << std::hex << (int)oxm.get_u32value() << std::dec	<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_MPLS_TC
 */
class coxmatch_ofb_mpls_tc : public coxmatch {
public:
	coxmatch_ofb_mpls_tc(
			uint8_t mpls_tc) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_MPLS_TC, mpls_tc, COXMATCH_8BIT)
	{};
	coxmatch_ofb_mpls_tc(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_mpls_tc()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_mpls_tc& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<mpls-tc: "	<< (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_MPLS_BOS
 */
class coxmatch_ofb_mpls_bos : public coxmatch {
public:
	coxmatch_ofb_mpls_bos(
			uint8_t mpls_bos) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS, mpls_bos, COXMATCH_8BIT)
	{};
	coxmatch_ofb_mpls_bos(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_mpls_bos()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_mpls_bos& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<mpls-bos: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};



/**
 * @brief	OXM_OF_TUNNEL_ID
 */
class coxmatch_ofb_tunnel_id : public coxmatch {
public:
	coxmatch_ofb_tunnel_id(
			uint64_t tunnel_id) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID, tunnel_id, COXMATCH_64BIT)
	{};
	coxmatch_ofb_tunnel_id(
			uint64_t tunnel_id, uint64_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID_MASK, tunnel_id, mask, COXMATCH_64BIT)
	{};
	coxmatch_ofb_tunnel_id(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_tunnel_id()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_tunnel_id& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<tunnel-id: "
						<< std::hex
						<< (unsigned long long)oxm.get_u64value() << "/" << (unsigned long long)oxm.get_u64mask()
						<< std::dec
						<< " >" << std::endl;
		return os;
	};
};




/**
 * @brief	OXM_OF_PBB_ISID
 */
class coxmatch_ofb_pbb_isid : public coxmatch {
public:
	coxmatch_ofb_pbb_isid(
			uint32_t pbb_isid) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_PBB_ISID, pbb_isid, COXMATCH_24BIT)
	{};
	coxmatch_ofb_pbb_isid(
			uint32_t pbb_isid, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_PBB_ISID_MASK, pbb_isid, mask, COXMATCH_24BIT)
	{};
	coxmatch_ofb_pbb_isid(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_pbb_isid()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_pbb_isid& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<pbb-isid: 0x"
						<< std::hex
						<< (unsigned int)oxm.get_u24value() << "/0x" << (unsigned int)oxm.get_u24mask()
						<< std::dec
						<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_IPV6_EXTHDR
 */
class coxmatch_ofb_ipv6_exthdr : public coxmatch {
public:
	coxmatch_ofb_ipv6_exthdr(
			uint16_t ipv6_exthdr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR, ipv6_exthdr, COXMATCH_16BIT)
	{};
	coxmatch_ofb_ipv6_exthdr(
			uint16_t ipv6_exthdr, uint16_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR_MASK, ipv6_exthdr, mask, COXMATCH_16BIT)
	{};
	coxmatch_ofb_ipv6_exthdr(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofb_ipv6_exthdr()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofb_ipv6_exthdr& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<ipv6-exthdr: "
						<< (int)oxm.get_u16value() << "/" << oxm.get_u16mask()
						<< " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_NW_PROTO (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_proto : public coxmatch {
public:
	coxmatch_ofx_nw_proto(
			uint8_t proto) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO, proto, COXMATCH_8BIT)
	{};
	coxmatch_ofx_nw_proto(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_proto()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_proto& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<nw-proto: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_NW_TOS (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_tos : public coxmatch {
public:
	coxmatch_ofx_nw_tos(
			uint8_t tos) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS, tos, COXMATCH_8BIT)
	{};
	coxmatch_ofx_nw_tos(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_tos()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_tos& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<nw-tos: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_NW_SRC (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_src : public coxmatch {
public:
	coxmatch_ofx_nw_src(
			uint32_t src) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC, src, COXMATCH_32BIT)
	{};
	coxmatch_ofx_nw_src(
			uint32_t src, uint32_t mask) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK, src, mask, COXMATCH_32BIT)
	{};
	coxmatch_ofx_nw_src(
			const caddress_in4& src) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC, src)
	{};
	coxmatch_ofx_nw_src(
			const caddress_in4& src, const caddress_in4& mask) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK, src, mask)
	{};
	coxmatch_ofx_nw_src(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_src& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<nw-src: "
				<< oxm.get_u32value_as_addr().str() << "/"
				<< oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_NW_DST (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_nw_dst : public coxmatch {
public:
	coxmatch_ofx_nw_dst(
			uint32_t dst) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST, dst, COXMATCH_32BIT)
	{};
	coxmatch_ofx_nw_dst(
			uint32_t dst, uint32_t mask) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK, dst, mask, COXMATCH_32BIT)
	{};
	coxmatch_ofx_nw_dst(
			const caddress_in4& dst) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST, dst)
	{};
	coxmatch_ofx_nw_dst(
			const caddress_in4& dst, const caddress_in4& mask) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK, dst, mask)
	{};
	coxmatch_ofx_nw_dst(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofx_nw_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_nw_dst& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<nw-dst: "
				<< oxm.get_u32value_as_addr().str() << "/"
				<< oxm.get_u32mask_as_addr().str() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_TP_SRC (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_tp_src : public coxmatch {
public:
	coxmatch_ofx_tp_src(
			uint16_t src) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC, src, COXMATCH_16BIT)
	{};
	coxmatch_ofx_tp_src(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofx_tp_src()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_tp_src& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<tp-src: " << (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


/**
 * @brief	OXM_OF_TP_DST (pseudo OXM-TLV for OF1.0 backwards compatibility)
 */
class coxmatch_ofx_tp_dst : public coxmatch {
public:
	coxmatch_ofx_tp_dst(
			uint16_t dst) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST, dst, COXMATCH_16BIT)
	{};
	coxmatch_ofx_tp_dst(
			const coxmatch& oxm) :
				coxmatch(oxm)
	{};
	virtual
	~coxmatch_ofx_tp_dst()
	{};
public:
	friend std::ostream&
	operator<< (std::ostream& os, const coxmatch_ofx_tp_dst& oxm) {
		os << dynamic_cast<const coxmatch&>(oxm);
		os << indent(2) << "<tp-dst: " << (int)oxm.get_u16value() << " >" << std::endl;
		return os;
	};
};


}; // end of namespace openflow
}; // end of namespace rofl




#endif /* COXMATCH_H_ */
