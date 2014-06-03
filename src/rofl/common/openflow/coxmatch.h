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
#include "rofl/common/cvastring.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/cmacaddr.h"
#include "rofl/common/caddress.h"
#include "rofl/common/fframe.h"
#include "rofl/common/logging.h"
#include "rofl/common/thread_helper.h"

namespace rofl {
namespace openflow {

/* error classes */
class eOxmBase 				: public RoflException {}; // error base class for class cofinstruction
class eOxmInval 			: public eOxmBase {}; // invalid parameter
class eOxmNotFound 			: public eOxmBase {}; // not found
class eOxmBadLen 			: public eOxmBase {}; // bad length
class eOxmInvalType 		: public eOxmBase {}; // invalid OXM TLV field value
class eOxmInvalClass 		: public eOxmBase {}; // invalid OXM TLV class value
class eOxmHeaderInval 		: public eOxmBase {}; // invalid instruction header
class eOxmActionNotFound 	: public eOxmBase {}; // action not found in instruction
class eOxmBadExperimenter 	: public eOxmBase {}; // unknown experimenter instruction


/**
 *
 */
class coxmatch :
	public rofl::cmemory
{
	PthreadRwLock		oxmlock;

public:

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
	 *
	 */
	coxmatch(
			uint32_t oxm_id, uint8_t value);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, uint8_t value, uint8_t mask);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, uint16_t value);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, uint16_t value, uint16_t mask);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, uint32_t value);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, uint32_t value, uint32_t mask);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, uint64_t value);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, uint64_t value, uint64_t mask);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, rofl::caddress const& value);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, rofl::caddress const& value, rofl::caddress const& mask);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, rofl::cmacaddr const& value);

	/**
	 *
	 */
	coxmatch(
			uint32_t oxm_id, rofl::cmacaddr const& value, rofl::cmacaddr const& mask);

	/**
	 *
	 */
	coxmatch(
			coxmatch const& oxm);

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
			coxmatch const& oxm);

	/**
	 *
	 */
	bool
	operator== (
			coxmatch const& oxm) const;

	/**
	 *
	 */
	bool
	operator!= (
			coxmatch const& oxm) const;

	/**
	 *
	 */
	bool
	operator< (
			coxmatch const& oxm);

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

	uint8_t get_u8value() const;
	uint8_t get_u8mask() const;
	uint8_t get_u8masked_value() const;

	uint16_t get_u16value() const;
	uint16_t get_u16mask() const;
	uint16_t get_u16masked_value() const;

	uint32_t get_u32value() const;
	uint32_t get_u32mask() const;
	uint32_t get_u32masked_value() const;

	rofl::caddress get_u32value_as_addr() const;
	rofl::caddress get_u32mask_as_addr() const;
	rofl::caddress get_u32masked_value_as_addr() const;

	rofl::cmacaddr get_u48value() const;
	rofl::cmacaddr get_u48mask() const;
	rofl::cmacaddr get_u48masked_value() const;

	uint64_t get_u64value() const;
	uint64_t get_u64mask() const;
	uint64_t get_u64masked_value() const;

	rofl::caddress get_u128value() const;
	rofl::caddress get_u128mask() const;
	rofl::caddress get_u128masked_value() const;

	void set_u8value(uint8_t value);
	void set_u8mask(uint8_t mask);

	void set_u16value(uint16_t value);
	void set_u16mask(uint16_t mask);

	void set_u32value(uint32_t value);
	void set_u32mask(uint32_t mask);

	void set_u32value(rofl::caddress const& addr);
	void set_u32mask(rofl::caddress const& mask);

	void set_u48value(rofl::cmacaddr const& addr);
	void set_u48mask(rofl::cmacaddr const& mask);

	void set_u64value(uint64_t value);
	void set_u64mask(uint64_t mask);

	void set_u128value(rofl::caddress const& addr);
	void set_u128mask(rofl::caddress const& mask);


public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch const& oxm) {
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


/** OXM_OF_IN_PORT
 *
 */
class coxmatch_ofb_in_port : public coxmatch {
public:
	coxmatch_ofb_in_port(
			uint32_t port_no) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IN_PORT, port_no) {};
	coxmatch_ofb_in_port(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_in_port() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_in_port const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<in-port: " << std::hex << (int)oxm.get_u32value() << std::dec << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IN_PHY_PORT
 *
 */
class coxmatch_ofb_in_phy_port : public coxmatch {
public:
	coxmatch_ofb_in_phy_port(
			uint32_t port_no) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT, port_no) {};
	coxmatch_ofb_in_phy_port(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_in_phy_port() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_in_phy_port const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<in-phy-port: " << std::hex << (int)oxm.get_u32value() << std::dec << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_METADATA
 *
 */
class coxmatch_ofb_metadata : public coxmatch {
public:
	coxmatch_ofb_metadata(
			uint64_t metadata) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_METADATA, metadata) {};
	coxmatch_ofb_metadata(
			uint64_t metadata, uint64_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_METADATA_MASK, metadata, mask) {};
	coxmatch_ofb_metadata(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_metadata() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_metadata const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << std::hex;
		os << indent(2) << "<metadata: "
		<< (unsigned long long)oxm.get_u64value() << "/" << (unsigned long long)oxm.get_u64mask()
		<< std::dec
		<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_ETH_DST
 *
 */
class coxmatch_ofb_eth_dst : public coxmatch {
public:
	coxmatch_ofb_eth_dst(
			cmacaddr const& maddr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_DST, maddr) {};
	coxmatch_ofb_eth_dst(
			cmacaddr const& maddr, cmacaddr const& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK, maddr, mask) {};
	coxmatch_ofb_eth_dst(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_eth_dst() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_eth_dst const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<eth-dst: "
						<< oxm.get_u48value() << "/" << oxm.get_u48mask()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_ETH_SRC
 *
 */
class coxmatch_ofb_eth_src : public coxmatch {
public:
	coxmatch_ofb_eth_src(
			cmacaddr const& maddr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_SRC, maddr) {};
	coxmatch_ofb_eth_src(
			cmacaddr const& maddr, cmacaddr const& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_SRC_MASK, maddr, mask) {};
	coxmatch_ofb_eth_src(
			coxmatch const& oxm) : coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_eth_src() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_eth_src const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<eth-src: "
						<< oxm.get_u48value() << "/" << oxm.get_u48mask()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_ETH_TYPE
 *
 */
class coxmatch_ofb_eth_type : public coxmatch {
public:
	coxmatch_ofb_eth_type(
			uint16_t dl_type) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE, dl_type) {};
	coxmatch_ofb_eth_type(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_eth_type() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_eth_type const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<eth-type: 0x" << std::hex << (int)oxm.get_u16value() << std::dec << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_VLAN_VID
 *
 */
class coxmatch_ofb_vlan_vid : public coxmatch {
public:
	coxmatch_ofb_vlan_vid(
			uint16_t vid) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_VLAN_VID, vid) {};
	coxmatch_ofb_vlan_vid(
			uint16_t vid, uint16_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK, vid, mask) {};
	coxmatch_ofb_vlan_vid(
			coxmatch const& oxm) : coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_vlan_vid() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_vlan_vid const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << rofl::indent(2) << "<coxmatch_ofb_vlan_vid >" << std::endl;
		os << rofl::indent(4) << "<vlan-vid: 0x" << std::hex << (int)oxm.get_u16value() << "/0x" << (int)oxm.get_u16mask() << std::dec << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_VLAN_VID
 *
 */
class coxmatch_ofb_vlan_untagged : public coxmatch {
public:
	coxmatch_ofb_vlan_untagged() :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_VLAN_VID,
										(uint16_t)rofl::openflow::OFPVID_NONE) {};
	coxmatch_ofb_vlan_untagged(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_vlan_untagged() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_vlan_untagged const& oxm) {
		os << dynamic_cast<coxmatch_ofb_vlan_vid const&>(oxm);
		os << rofl::indent(2) << "<vlan-untagged >" << std::endl;
		return os;
	};
};


/** OXM_OF_VLAN_VID
 *
 */
class coxmatch_ofb_vlan_present : public coxmatch {
public:
	coxmatch_ofb_vlan_present() :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK,
										(uint16_t)rofl::openflow::OFPVID_PRESENT,
										(uint16_t)rofl::openflow::OFPVID_PRESENT) {};
	coxmatch_ofb_vlan_present(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_vlan_present() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_vlan_present const& oxm) {
		os << dynamic_cast<coxmatch_ofb_vlan_vid const&>(oxm);
		os << rofl::indent(2) << "<vlan-present >" << std::endl;
		return os;
	};
};


/** OXM_OF_VLAN_PCP
 *
 */
class coxmatch_ofb_vlan_pcp : public coxmatch {
public:
	coxmatch_ofb_vlan_pcp(
			uint8_t pcp) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP, pcp) {};
	coxmatch_ofb_vlan_pcp(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_vlan_pcp() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_vlan_pcp const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<vlan-pcp: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IP_DSCP
 *
 */
class coxmatch_ofb_ip_dscp : public coxmatch {
public:
	coxmatch_ofb_ip_dscp(
			uint8_t dscp) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IP_DSCP, dscp) {};
	coxmatch_ofb_ip_dscp(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ip_dscp() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ip_dscp const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<ip-dscp: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IP_ECN
 *
 */
class coxmatch_ofb_ip_ecn : public coxmatch {
public:
	coxmatch_ofb_ip_ecn(
			uint8_t ecn) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IP_ECN, ecn) {};
	coxmatch_ofb_ip_ecn(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ip_ecn() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ip_ecn const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<ip-ecn: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IP_PROTO
 *
 */
class coxmatch_ofb_ip_proto : public coxmatch {
public:
	coxmatch_ofb_ip_proto(
			uint8_t proto) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IP_PROTO, proto) {};
	coxmatch_ofb_ip_proto(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ip_proto() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ip_proto const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<ip-proto: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IPV4_SRC
 *
 */
class coxmatch_ofb_ipv4_src : public coxmatch {
public:
	coxmatch_ofb_ipv4_src(
			uint32_t src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC, src) {};
	coxmatch_ofb_ipv4_src(
			uint32_t src, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC_MASK, src, mask) {};
	coxmatch_ofb_ipv4_src(
			rofl::caddress const& src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC, src) {};
	coxmatch_ofb_ipv4_src(
			rofl::caddress const& src, rofl::caddress const& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC_MASK, src, mask) {};
	coxmatch_ofb_ipv4_src(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ipv4_src() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv4_src const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
			os << indent(2) << "<ipv4-src: "
			<< oxm.get_u32value_as_addr() << "/" << oxm.get_u32mask_as_addr()
			<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IPV4_DST
 *
 */
class coxmatch_ofb_ipv4_dst : public coxmatch {
public:
	coxmatch_ofb_ipv4_dst(
			uint32_t dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_DST, dst) {};
	coxmatch_ofb_ipv4_dst(
			uint32_t dst, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK, dst, mask) {};
	coxmatch_ofb_ipv4_dst(
			caddress const& dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_DST, dst) {};
	coxmatch_ofb_ipv4_dst(
			caddress const& dst, caddress const& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK, dst, mask) {};
	coxmatch_ofb_ipv4_dst(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ipv4_dst() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv4_dst const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
			os << indent(2) << "<ipv4-dst: "
			<< oxm.get_u32value_as_addr() << "/" << oxm.get_u32mask_as_addr()
			<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IPV6_SRC
 *
 */
class coxmatch_ofb_ipv6_src : public coxmatch {
public:
	coxmatch_ofb_ipv6_src(
			rofl::caddress const& src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC, src) {};
	coxmatch_ofb_ipv6_src(
			rofl::caddress const& src, rofl::caddress const& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC_MASK, src, mask) {};
	coxmatch_ofb_ipv6_src(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ipv6_src() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_src const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		rofl::indent i(2);
		os << rofl::indent(0) << "<ipv6-src: >" << std::endl;
		rofl::indent j(2);
		os << "<" << oxm.get_u128value() << "/" << oxm.get_u128mask() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IPV6_DST
 *
 */
class coxmatch_ofb_ipv6_dst : public coxmatch {
public:
	coxmatch_ofb_ipv6_dst(
			rofl::caddress const& dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_DST, dst) {};
	coxmatch_ofb_ipv6_dst(
			rofl::caddress const& dst, rofl::caddress const& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_DST_MASK, dst, mask) {};
	coxmatch_ofb_ipv6_dst(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ipv6_dst() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_dst const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		rofl::indent i(2);
		os << rofl::indent(0) << "<ipv6-dst: >" << std::endl;
		rofl::indent j(2);
		os << "<" << oxm.get_u128value() << "/" << oxm.get_u128mask() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IPV6_ND_TARGET
 *
 */
class coxmatch_ofb_ipv6_nd_target : public coxmatch {
public:
	coxmatch_ofb_ipv6_nd_target(
			rofl::caddress const& nd_target) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET, nd_target) {};
	coxmatch_ofb_ipv6_nd_target(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ipv6_nd_target() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_nd_target const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		rofl::indent i(2);
		os << rofl::indent(0) << "<ipv6-nd-target: >" << std::endl;
		rofl::indent j(2);
		os << "<" << oxm.get_u128value() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_TCP_SRC
 *
 */
class coxmatch_ofb_tcp_src : public coxmatch {
public:
	coxmatch_ofb_tcp_src(
			uint16_t src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_TCP_SRC, src) {};
	coxmatch_ofb_tcp_src(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_tcp_src() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_tcp_src const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<tcp-src: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_TCP_DST
 *
 */
class coxmatch_ofb_tcp_dst : public coxmatch {
public:
	coxmatch_ofb_tcp_dst(
			uint16_t dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_TCP_DST, dst) {};
	coxmatch_ofb_tcp_dst(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_tcp_dst() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_tcp_dst const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<tcp-dst: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_UDP_SRC
 *
 */
class coxmatch_ofb_udp_src : public coxmatch {
public:
	coxmatch_ofb_udp_src(
			uint16_t src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_UDP_SRC, src) {};
	coxmatch_ofb_udp_src(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_udp_src() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_udp_src const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<udp-src: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_UDP_DST
 *
 */
class coxmatch_ofb_udp_dst : public coxmatch {
public:
	coxmatch_ofb_udp_dst(
			uint16_t dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_UDP_DST, dst) {};
	coxmatch_ofb_udp_dst(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_udp_dst() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_udp_dst const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<udp-dst: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_SCTP_SRC
 *
 */
class coxmatch_ofb_sctp_src : public coxmatch {
public:
	coxmatch_ofb_sctp_src(
			uint16_t src) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC, src) {};
	coxmatch_ofb_sctp_src(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_sctp_src() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_sctp_src const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<sctp-src: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_SCTP_DST
 *
 */
class coxmatch_ofb_sctp_dst : public coxmatch {
public:
	coxmatch_ofb_sctp_dst(
			uint16_t dst) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_SCTP_DST, dst) {};
	coxmatch_ofb_sctp_dst(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_sctp_dst() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_sctp_dst const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<sctp-dst: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_ICMPV4_TYPE
 *
 */
class coxmatch_ofb_icmpv4_type : public coxmatch {
public:
	coxmatch_ofb_icmpv4_type(
			uint8_t type) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE, type) {};
	coxmatch_ofb_icmpv4_type(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_icmpv4_type() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_icmpv4_type const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<icmpv4-type: "
						<< (int)oxm.get_u8value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_ICMPV4_CODE
 *
 */
class coxmatch_ofb_icmpv4_code : public coxmatch {
public:
	coxmatch_ofb_icmpv4_code(
			uint8_t code) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE, code) {};
	coxmatch_ofb_icmpv4_code(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_icmpv4_code() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_icmpv4_code const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<icmpv4-code: "
						<< (int)oxm.get_u8value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_ARP_OP
 *
 */
class coxmatch_ofb_arp_opcode : public coxmatch {
public:
	coxmatch_ofb_arp_opcode(
			uint16_t opcode) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_OP, opcode) {};
	coxmatch_ofb_arp_opcode(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_arp_opcode() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_arp_opcode const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<arp-opcode: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};



/** OXM_OF_ARP_SPA
 *
 */
class coxmatch_ofb_arp_spa : public coxmatch {
public:
	coxmatch_ofb_arp_spa(
			uint32_t spa) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SPA, spa) {};
	coxmatch_ofb_arp_spa(
			uint32_t spa, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SPA, spa, mask) {};
	coxmatch_ofb_arp_spa(
			caddress const& spa) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SPA, spa) {};
	coxmatch_ofb_arp_spa(
			caddress const& spa, caddress const& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SPA, spa, mask) {};
	coxmatch_ofb_arp_spa(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_arp_spa() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_arp_spa const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<arp-spa: "
						<< oxm.get_u32value_as_addr() << "/" << oxm.get_u32mask_as_addr()
						<< " >" << std::endl;
		return os;
	};
};



/** OXM_OF_ARP_TPA
 *
 */
class coxmatch_ofb_arp_tpa : public coxmatch {
public:
	coxmatch_ofb_arp_tpa(
			uint32_t tpa) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_TPA, tpa) {};
	coxmatch_ofb_arp_tpa(
			uint32_t tpa, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_TPA, tpa, mask) {};
	coxmatch_ofb_arp_tpa(
			caddress const& tpa) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_TPA, tpa) {};
	coxmatch_ofb_arp_tpa(
			caddress const& tpa, caddress const& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_TPA, tpa, mask) {};
	coxmatch_ofb_arp_tpa(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_arp_tpa() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_arp_tpa const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<arp-tpa: "
						<< oxm.get_u32value_as_addr() << "/" << oxm.get_u32mask_as_addr()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_ARP_SHA
 *
 */
class coxmatch_ofb_arp_sha : public coxmatch {
public:
	coxmatch_ofb_arp_sha(
			cmacaddr const& maddr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SHA, maddr) {};
	coxmatch_ofb_arp_sha(
			cmacaddr const& maddr, cmacaddr const& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SHA, maddr, mask) {};
	coxmatch_ofb_arp_sha(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_arp_sha() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_arp_sha const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<arp-sha: "
						<< oxm.get_u48value() << "/" << oxm.get_u48mask()
						<< " >" << std::endl;
		return os;
	};
};



/** OXM_OF_ARP_THA
 *
 */
class coxmatch_ofb_arp_tha : public coxmatch {
public:
	coxmatch_ofb_arp_tha(
			cmacaddr const& maddr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SHA, maddr) {};
	coxmatch_ofb_arp_tha(
			cmacaddr const& maddr, cmacaddr const& mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ARP_SHA, maddr, mask) {};
	coxmatch_ofb_arp_tha(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_arp_tha() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_arp_tha const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<arp-tha: "
						<< oxm.get_u48value() << "/" << oxm.get_u48mask()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IPV6_FLABEL
 *
 */
class coxmatch_ofb_ipv6_flabel : public coxmatch {
public:
	/** constructor
	 */
	coxmatch_ofb_ipv6_flabel(
			uint32_t flow_label) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL, flow_label) {};
	coxmatch_ofb_ipv6_flabel(
			uint32_t flow_label, uint32_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL_MASK, flow_label, mask) {};
	coxmatch_ofb_ipv6_flabel(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ipv6_flabel() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_flabel const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<ipv6-flabel: "
						<< (int)oxm.get_u32value() << "/" << (int)oxm.get_u32mask()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_ICMPV6_TYPE
 *
 */
class coxmatch_ofb_icmpv6_type : public coxmatch {
public:
	coxmatch_ofb_icmpv6_type(
			uint8_t type) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE, type) {};
	coxmatch_ofb_icmpv6_type(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_icmpv6_type() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_icmpv6_type const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<icmpv6-type: "
						<< (int)oxm.get_u8value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_ICMPV6_CODE
 *
 */
class coxmatch_ofb_icmpv6_code : public coxmatch {
public:
	coxmatch_ofb_icmpv6_code(
			uint8_t code) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE, code) {};
	coxmatch_ofb_icmpv6_code(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_icmpv6_code() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_icmpv6_code const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<icmpv6-code: "
						<< (int)oxm.get_u8value()
						<< " >" << std::endl;
		return os;
	};
};



/** OXM_OF_IPV6_ND_SLL
 *
 */
class coxmatch_ofb_ipv6_nd_sll : public coxmatch {
public:
	coxmatch_ofb_ipv6_nd_sll(
			cmacaddr const& addr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL, addr) {};
	coxmatch_ofb_ipv6_nd_sll(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ipv6_nd_sll() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_nd_sll const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<ipv6-nd-sll: "
						<< oxm.get_u48value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IPV6_ND_TLL
 *
 */
class coxmatch_ofb_ipv6_nd_tll : public coxmatch {
public:
	coxmatch_ofb_ipv6_nd_tll(
			cmacaddr const& addr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL, addr) {};
	coxmatch_ofb_ipv6_nd_tll(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ipv6_nd_tll() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_nd_tll const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<ipv6-nd-tll: "
						<< oxm.get_u48value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_MPLS_LABEL
 *
 */
class coxmatch_ofb_mpls_label : public coxmatch {
public:
	coxmatch_ofb_mpls_label(
			uint32_t mpls_label) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL, mpls_label) {};
	coxmatch_ofb_mpls_label(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_mpls_label() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_mpls_label const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<mpls-label: 0x"
						<< std::hex << (int)oxm.get_u32value() << std::dec
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_MPLS_TC
 *
 */
class coxmatch_ofb_mpls_tc : public coxmatch {
public:
	coxmatch_ofb_mpls_tc(
			uint8_t mpls_tc) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_MPLS_TC, mpls_tc) {};
	coxmatch_ofb_mpls_tc(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_mpls_tc() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_mpls_tc const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<mpls-tc: "
						<< (int)oxm.get_u8value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_MPLS_BOS
 *
 */
class coxmatch_ofb_mpls_bos : public coxmatch {
public:
	coxmatch_ofb_mpls_bos(
			uint8_t mpls_bos) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_MPLS_BOS, mpls_bos) {};
	coxmatch_ofb_mpls_bos(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_mpls_bos() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_mpls_bos const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<mpls-bos: "
						<< (int)oxm.get_u8value()
						<< " >" << std::endl;
		return os;
	};
};



/** OXM_OF_TUNNEL_ID
 *
 */
class coxmatch_ofb_tunnel_id : public coxmatch {
public:
	coxmatch_ofb_tunnel_id(
			uint64_t tunnel_id) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID, tunnel_id) {};
	coxmatch_ofb_tunnel_id(
			uint64_t tunnel_id, uint64_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID_MASK, tunnel_id, mask) {};
	coxmatch_ofb_tunnel_id(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_tunnel_id() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_tunnel_id const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<tunnel_id: "
						<< std::hex
						<< (unsigned long long)oxm.get_u64value() << "/" << (unsigned long long)oxm.get_u64mask()
						<< std::dec
						<< " >" << std::endl;
		return os;
	};
};




/** OXM_OF_PBB_ISID: TODO: uint24_t
 *
 */
class coxmatch_ofb_pbb_isid : public coxmatch {
public:
	coxmatch_ofb_pbb_isid(
			uint32_t pbb_isid) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_PBB_ISID, pbb_isid) {};
	coxmatch_ofb_pbb_isid(
			uint64_t pbb_isid, uint64_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_PBB_ISID_MASK, pbb_isid, mask) {};
	coxmatch_ofb_pbb_isid(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_pbb_isid() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_pbb_isid const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<pbb_isid: "
						<< std::hex
						<< (unsigned int)oxm.get_u32value() << "/" << (unsigned int)oxm.get_u32mask()
						<< std::dec
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_IPV6_EXTHDR
 *
 */
class coxmatch_ofb_ipv6_exthdr : public coxmatch {
public:
	coxmatch_ofb_ipv6_exthdr(
			uint16_t ipv6_exthdr) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR, ipv6_exthdr) {};
	coxmatch_ofb_ipv6_exthdr(
			uint16_t ipv6_exthdr, uint16_t mask) :
				coxmatch(rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR_MASK, ipv6_exthdr, mask) {};
	coxmatch_ofb_ipv6_exthdr(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofb_ipv6_exthdr() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_exthdr const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<ipv6-exthdr: "
						<< (int)oxm.get_u16value() << "/" << oxm.get_u16mask()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_NW_PROTO (pseudo OXM-TLV for OF1.0 backwards compatibility)
 *
 */
class coxmatch_ofx_nw_proto : public coxmatch {
public:
	coxmatch_ofx_nw_proto(
			uint8_t proto) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_PROTO, proto) {};
	coxmatch_ofx_nw_proto(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_nw_proto() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_nw_proto const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<nw-proto: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_NW_TOS (pseudo OXM-TLV for OF1.0 backwards compatibility)
 *
 */
class coxmatch_ofx_nw_tos : public coxmatch {
public:
	coxmatch_ofx_nw_tos(
			uint8_t tos) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_TOS, tos) {};
	coxmatch_ofx_nw_tos(
			coxmatch const& oxm) : coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_nw_tos() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_nw_tos const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<nw-tos: " << (int)oxm.get_u8value() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_NW_SRC (pseudo OXM-TLV for OF1.0 backwards compatibility)
 *
 */
class coxmatch_ofx_nw_src : public coxmatch {
public:
	coxmatch_ofx_nw_src(
			uint32_t src) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC, src) {};
	coxmatch_ofx_nw_src(
			uint32_t src, uint32_t mask) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK, src, mask) {};
	coxmatch_ofx_nw_src(
			rofl::caddress const& src) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC, src) {};
	coxmatch_ofx_nw_src(
			rofl::caddress const& src, rofl::caddress const& mask) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_SRC_MASK, src, mask) {};
	coxmatch_ofx_nw_src(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_nw_src() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_nw_src const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<nw-src: " << oxm.get_u32value_as_addr() << "/" << oxm.get_u32mask_as_addr() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_NW_DST (pseudo OXM-TLV for OF1.0 backwards compatibility)
 *
 */
class coxmatch_ofx_nw_dst : public coxmatch {
public:
	coxmatch_ofx_nw_dst(
			uint32_t dst) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST, dst) {};
	coxmatch_ofx_nw_dst(
			uint32_t dst, uint32_t mask) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK, dst, mask) {};
	coxmatch_ofx_nw_dst(
			rofl::caddress const& dst) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST, dst) {};
	coxmatch_ofx_nw_dst(
			rofl::caddress const& dst, rofl::caddress const& mask) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_NW_DST_MASK, dst, mask) {};
	coxmatch_ofx_nw_dst(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_nw_dst() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_nw_dst const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<nw-dst: " << oxm.get_u32value_as_addr() << "/" << oxm.get_u32mask_as_addr() << " >" << std::endl;
		return os;
	};
};


/** OXM_OF_TP_SRC (pseudo OXM-TLV for OF1.0 backwards compatibility)
 *
 */
class coxmatch_ofx_tp_src : public coxmatch {
public:
	coxmatch_ofx_tp_src(
			uint16_t src) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_TP_SRC, src) {};
	coxmatch_ofx_tp_src(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_tp_src() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_tp_src const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<tp-src: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_TP_DST (pseudo OXM-TLV for OF1.0 backwards compatibility)
 *
 */
class coxmatch_ofx_tp_dst : public coxmatch {
public:
	coxmatch_ofx_tp_dst(
			uint16_t dst) :
				coxmatch(rofl::openflow::experimental::OXM_TLV_EXPR_TP_DST, dst) {};
	coxmatch_ofx_tp_dst(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_tp_dst() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_tp_dst const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<tp-dst: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};












class coxmatch_output {
	coxmatch oxm;
public:
	coxmatch_output(coxmatch const& oxm) : oxm(oxm) {};

	friend std::ostream&
	operator<<(std::ostream& os, coxmatch_output const& oxm_output) {
		coxmatch const& oxm = oxm_output.oxm;
		switch (oxm.get_oxm_class()) {
		case openflow::OFPXMC_OPENFLOW_BASIC: {
			switch (oxm.get_oxm_field()) {
			case openflow::OFPXMT_OFB_IN_PORT:
				os << coxmatch_ofb_in_port(oxm); return os;
			case openflow::OFPXMT_OFB_IN_PHY_PORT:
				os << coxmatch_ofb_in_phy_port(oxm); return os;
			case openflow::OFPXMT_OFB_METADATA:
				os << coxmatch_ofb_metadata(oxm); return os;
			case openflow::OFPXMT_OFB_ETH_DST:
				os << coxmatch_ofb_eth_dst(oxm); return os;
			case openflow::OFPXMT_OFB_ETH_SRC:
				os << coxmatch_ofb_eth_src(oxm); return os;
			case openflow::OFPXMT_OFB_ETH_TYPE:
				os << coxmatch_ofb_eth_type(oxm); return os;
			case openflow::OFPXMT_OFB_VLAN_VID:
				os << coxmatch_ofb_vlan_vid(oxm); return os;
			case openflow::OFPXMT_OFB_VLAN_PCP:
				os << coxmatch_ofb_vlan_pcp(oxm); return os;
			case openflow::OFPXMT_OFB_IP_DSCP:
				os << coxmatch_ofb_ip_dscp(oxm); return os;
			case openflow::OFPXMT_OFB_IP_ECN:
				os << coxmatch_ofb_ip_ecn(oxm); return os;
			case openflow::OFPXMT_OFB_IP_PROTO:
				os << coxmatch_ofb_ip_proto(oxm); return os;
			case openflow::OFPXMT_OFB_IPV4_SRC:
				os << coxmatch_ofb_ipv4_src(oxm); return os;
			case openflow::OFPXMT_OFB_IPV4_DST:
				os << coxmatch_ofb_ipv4_dst(oxm); return os;
			case openflow::OFPXMT_OFB_TCP_SRC:
				os << coxmatch_ofb_tcp_src(oxm); return os;
			case openflow::OFPXMT_OFB_TCP_DST:
				os << coxmatch_ofb_tcp_dst(oxm); return os;
			case openflow::OFPXMT_OFB_UDP_SRC:
				os << coxmatch_ofb_udp_src(oxm); return os;
			case openflow::OFPXMT_OFB_UDP_DST:
				os << coxmatch_ofb_udp_dst(oxm); return os;
			case openflow::OFPXMT_OFB_SCTP_SRC:
				os << coxmatch_ofb_sctp_src(oxm); return os;
			case openflow::OFPXMT_OFB_SCTP_DST:
				os << coxmatch_ofb_sctp_dst(oxm); return os;
			case openflow::OFPXMT_OFB_ICMPV4_TYPE:
				os << coxmatch_ofb_icmpv4_type(oxm); return os;
			case openflow::OFPXMT_OFB_ICMPV4_CODE:
				os << coxmatch_ofb_icmpv4_code(oxm); return os;
			case openflow::OFPXMT_OFB_ARP_OP:
				os << coxmatch_ofb_arp_opcode(oxm); return os;
			case openflow::OFPXMT_OFB_ARP_SPA:
				os << coxmatch_ofb_arp_spa(oxm); return os;
			case openflow::OFPXMT_OFB_ARP_TPA:
				os << coxmatch_ofb_arp_tpa(oxm); return os;
			case openflow::OFPXMT_OFB_ARP_SHA:
				os << coxmatch_ofb_arp_sha(oxm); return os;
			case openflow::OFPXMT_OFB_ARP_THA:
				os << coxmatch_ofb_arp_tha(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_SRC:
				os << coxmatch_ofb_ipv6_src(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_DST:
				os << coxmatch_ofb_ipv6_dst(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_FLABEL:
				os << coxmatch_ofb_ipv6_flabel(oxm); return os;
			case openflow::OFPXMT_OFB_ICMPV6_TYPE:
				os << coxmatch_ofb_icmpv6_type(oxm); return os;
			case openflow::OFPXMT_OFB_ICMPV6_CODE:
				os << coxmatch_ofb_icmpv6_code(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_ND_TARGET:
				os << coxmatch_ofb_ipv6_nd_target(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_ND_SLL:
				os << coxmatch_ofb_ipv6_nd_sll(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_ND_TLL:
				os << coxmatch_ofb_ipv6_nd_tll(oxm); return os;
			case openflow::OFPXMT_OFB_MPLS_LABEL:
				os << coxmatch_ofb_mpls_label(oxm); return os;
			case openflow::OFPXMT_OFB_MPLS_TC:
				os << coxmatch_ofb_mpls_tc(oxm); return os;
			case openflow::OFPXMT_OFB_MPLS_BOS:
				os << coxmatch_ofb_mpls_bos(oxm); return os;
			case openflow::OFPXMT_OFB_TUNNEL_ID:
				os << coxmatch_ofb_tunnel_id(oxm); return os;
			case openflow::OFPXMT_OFB_PBB_ISID:
				os << coxmatch_ofb_pbb_isid(oxm); return os;
			case openflow::OFPXMT_OFB_IPV6_EXTHDR:
				os << coxmatch_ofb_ipv6_exthdr(oxm); return os;
			default:
				os << oxm; return os;
			}
		} break;
		case openflow::OFPXMC_EXPERIMENTER: {
			switch (oxm.get_oxm_field()) {
			case openflow::experimental::OFPXMT_OFX_NW_SRC:
				os << coxmatch_ofx_nw_src(oxm); return os;
			case openflow::experimental::OFPXMT_OFX_NW_DST:
				os << coxmatch_ofx_nw_dst(oxm); return os;
			case openflow::experimental::OFPXMT_OFX_NW_PROTO:
				os << coxmatch_ofx_nw_proto(oxm); return os;
			case openflow::experimental::OFPXMT_OFX_NW_TOS:
				os << coxmatch_ofx_nw_tos(oxm); return os;
			case openflow::experimental::OFPXMT_OFX_TP_SRC:
				os << coxmatch_ofx_tp_src(oxm); return os;
			case openflow::experimental::OFPXMT_OFX_TP_DST:
				os << coxmatch_ofx_tp_dst(oxm); return os;
			default:
				os << oxm; return os;
			}
		} break;
		default:
			os << oxm; return os;
		}

		return os;
	};
};





}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COXMATCH_H_ */
