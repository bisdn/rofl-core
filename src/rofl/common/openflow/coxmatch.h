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

#ifdef __cplusplus
extern "C" {
#endif
#include "openflow.h"
#include <endian.h>
#include <pthread.h>
#ifndef htobe16
#include "../endian_conversion.h"
#endif
#ifdef __cplusplus
}
#endif

#include "../cvastring.h"
#include "../cerror.h"
#include "../cmemory.h"
#include "../cmacaddr.h"
#include "../caddress.h"
#include "../coflist.h"
#include "../fframe.h"

namespace rofl
{

/* error classes */
class eOxmBase : public cerror {}; // error base class for class cofinstruction
class eOxmInval : public eOxmBase {}; // invalid parameter
class eOxmBadLen : public eOxmBase {}; // bad length
class eOxmInvalType : public eOxmBase {}; // invalid instruction type
class eOxmHeaderInval : public eOxmBase {}; // invalid instruction header
class eOxmActionNotFound : public eOxmBase {}; // action not found in instruction
class eOxmBadExperimenter : public eOxmBase {}; // unknown experimenter instruction


/**
 *
 */
class coxmatch :
	public cmemory
{
public: // static stuff, enums, constants

		typedef struct {
			uint16_t type;
			char desc[64];
		} oxm_typedesc_t;

		typedef struct {
			uint16_t oxm_class;
			char desc[64];
		} oxm_classdesc_t;

#define COXMATCH_DEFAULT_LEN		64


public: // data structures


	pthread_rwlock_t oxmlock; // mutex for this cofinst instance

	union { // for OpenFlow 1.2
		struct ofp_oxm_hdr				*oxmu_header;
		struct ofp_oxm_ofb_uint8_t		*oxmu_uint8t;
		struct ofp_oxm_ofb_uint16_t		*oxmu_uint16t;
		struct ofp_oxm_ofb_uint32_t		*oxmu_uint32t;
		struct ofp_oxm_ofb_uint48_t		*oxmu_uint48t;
		struct ofp_oxm_ofb_uint64_t 	*oxmu_uint64t;
		struct ofp_oxm_ofb_maddr 		*oxmu_maddr;
		struct ofp_oxm_ofb_ipv6_addr 	*oxmu_ipv6addr;
	} oxm_oxmu;

#define oxm_header oxm_oxmu.oxmu_header					// oxm: plain header
#define oxm_uint8t	oxm_oxmu.oxmu_uint8t				// oxm: uint8_t field
#define oxm_uint16t	 oxm_oxmu.oxmu_uint16t				// oxm: uint16_t field
#define oxm_uint32t	 oxm_oxmu.oxmu_uint32t				// oxm: uint32_t field
#define oxm_uint48t	 oxm_oxmu.oxmu_uint48t				// oxm: uint48_t field
#define oxm_uint64t	 oxm_oxmu.oxmu_uint64t				// oxm: uint64_t field
#define oxm_maddr	 oxm_oxmu.oxmu_maddr				// oxm: uint8_t[6] field
#define oxm_ipv6addr	 oxm_oxmu.oxmu_ipv6addr			// oxm: uint8_t[16] field


private: // data structures


	std::string info;


public: // methods


	/** constructor
	 *
	 */
	coxmatch(
			size_t size = COXMATCH_DEFAULT_LEN);


	/** constructor
	 *
	 */
	coxmatch(
			struct ofp_oxm_hdr *hdr,
			size_t oxm_len);


	/** copy constructor
	 *
	 */
	coxmatch(
			coxmatch const& oxm);


	/**
	 *
	 */
	coxmatch(
			struct ofp_action_set_field *ach,
			size_t achlen) throw (eOxmInval);


	/** destructor
	 *
	 */
	virtual
	~coxmatch();


	/** assignment operator
	 *
	 */
	coxmatch&
	operator= (
			coxmatch const& oxm);


	/** comparison operator
	 *
	 */
	bool
	operator== (
			coxmatch const& oxm);

	/**
	 *
	 */
	bool
	operator< (
			coxmatch const& oxm);


	/** reset (=clears all actions)
	 *
	 */
	void
	reset();


	/** return pointer to ofp_oxm_hdr start
	 *
	 */
	struct ofp_oxm_hdr*
	sooxm() const;


	/** return length of OXM TLV in bytes including header struct ofp_oxm_hdr
	 *
	 */
	size_t
	length() const;



	/** copy struct ofp_action_header
	 *
	 */
	void
	pack(
			uint8_t* buf,
			size_t buflen);



	/** unpack
	 *
	 */
	void
	unpack(
			uint8_t* buf,
			size_t buflen);



	/** dump info string for this action
	 *
	 */
	const char*
	c_str();


	/**
	 *
	 */
	void
	set_oxm_class(
			uint16_t oxm_class = OFPXMC_OPENFLOW_BASIC);


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
			bool oxm_hasmask = true);


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


	/**
	 *
	 */
	uint8_t  u8value() const;
	uint16_t u16value() const;
	uint32_t u32value() const;
	uint64_t u64value() const;
	caddress u32addr() const;
	cmacaddr u48addr() const;
	caddress u128addr() const;

	/**
	 *
	 */
	uint8_t
	uint8_value() const throw (eOxmInval);


	/**
	 *
	 */
	uint8_t
	uint8_mask() const throw (eOxmInval);


	/**
	 *
	 */
	uint16_t
	uint16_value() const throw (eOxmInval);


	/**
	 *
	 */
	uint16_t
	uint16_mask() const throw (eOxmInval);


	/**
	 *
	 */
	uint32_t
	uint32_value() const throw (eOxmInval);


	/**
	 *
	 */
	uint32_t
	uint32_mask() const throw (eOxmInval);


	/**
	 *
	 */
	uint64_t
	uint64_value() const throw (eOxmInval);


	/**
	 *
	 */
	uint64_t
	uint64_mask() const throw (eOxmInval);


	/**
	 *
	 */
	static void
	test();


public:


	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch const& oxm)
	{
		os << "coxmatch{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
		os << "}";
		return os;
	};


private: // methods


	/**
	 *
	 */
	const char*
	class2desc(
			uint16_t oxm_class);


	/**
	 *
	 */
	const char*
	type2desc(
			uint16_t oxm_class,
			uint16_t oxm_field);

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
class coxmatch_ofb_in_port :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_in_port(
			uint32_t port_no) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IN_PORT);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(port_no);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_in_port() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_in_port const& oxm)
	{
		os << "coxmatch_ofb_in_port{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<INPUT port: " << oxm.u32value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_IN_PHY_PORT
 *
 */
class coxmatch_ofb_in_phy_port :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_in_phy_port(
			uint32_t port_no) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IN_PHY_PORT);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(port_no);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_in_phy_port() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_in_phy_port const& oxm)
	{
		os << "coxmatch_ofb_in_phy_port{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<INPUT phy-port: " << oxm.u32value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_METADATA
 *
 */
class coxmatch_ofb_metadata :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_metadata(
			uint64_t metadata) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint64_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_METADATA);
		set_oxm_length(sizeof(uint64_t));
		memcpy(oxm_uint64t->word, (uint8_t*)&metadata, sizeof(metadata));
		//oxm_uint64t->qword = htobe64(metadata);
	};
	/** constructor
	 */
	coxmatch_ofb_metadata(
			uint64_t metadata,
			uint64_t metadata_mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint64_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_METADATA);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint64_t));
		memcpy(oxm_uint64t->word, (uint8_t*)&metadata, sizeof(metadata));
		memcpy(oxm_uint64t->mask, (uint8_t*)&metadata_mask, sizeof(metadata_mask));
		//oxm_uint64t->qword = htobe64(metadata);
		//oxm_uint64t->mask  = htobe64(metadata_mask);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_metadata() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_metadata const& oxm)
	{
		os << "coxmatch_ofb_metadata{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<METADATA metadata: " << oxm.u64value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_ETH_DST
 *
 */
class coxmatch_ofb_eth_dst :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_eth_dst(
			cmacaddr const& maddr) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ETH_DST);
		set_oxm_length(OFP_ETH_ALEN);
		memcpy(oxm_maddr->addr, maddr.somem(), OFP_ETH_ALEN);
	};
	/** constructor
	 */
	coxmatch_ofb_eth_dst(
			cmacaddr const& maddr,
			cmacaddr const& mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ETH_DST);
		set_oxm_hasmask(true);
		set_oxm_length(2 * OFP_ETH_ALEN);
		memcpy(oxm_maddr->addr, maddr.somem(), OFP_ETH_ALEN);
		memcpy(oxm_maddr->mask, mask.somem(), OFP_ETH_ALEN);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_eth_dst() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_eth_dst const& oxm)
	{
		os << "coxmatch_ofb_eth_dst{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ETH-DST: " << oxm.u48addr().c_str() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_ETH_SRC
 *
 */
class coxmatch_ofb_eth_src :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_eth_src(
			cmacaddr const& maddr) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ETH_SRC);
		set_oxm_length(OFP_ETH_ALEN);
		memcpy(oxm_maddr->addr, maddr.somem(), OFP_ETH_ALEN);
	};
	/** constructor
	 */
	coxmatch_ofb_eth_src(
			cmacaddr const& maddr,
			cmacaddr const& mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ETH_SRC);
		set_oxm_hasmask(true);
		set_oxm_length(2 * OFP_ETH_ALEN);
		memcpy(oxm_maddr->addr, maddr.somem(), OFP_ETH_ALEN);
		memcpy(oxm_maddr->mask, mask.somem(), OFP_ETH_ALEN);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_eth_src() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_eth_src const& oxm)
	{
		os << "coxmatch_ofb_eth_src{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ETH-SRC: " << oxm.u48addr().c_str() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_ETH_TYPE
 *
 */
class coxmatch_ofb_eth_type :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_eth_type(
			uint16_t dl_type) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ETH_TYPE);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(dl_type);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_eth_type() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_eth_type const& oxm)
	{
		os << "coxmatch_ofb_eth_type{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ETH-TYPE: " << oxm.u16value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_VLAN_VID
 *
 */
class coxmatch_ofb_vlan_vid :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_vlan_vid(
			uint16_t vid) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_VLAN_VID);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(vid);
	};
	/** constructor
	 */
	coxmatch_ofb_vlan_vid(
			uint16_t vid,
			uint16_t mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_VLAN_VID);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint16_t));
		oxm_uint16t->word = htobe16(vid);
		oxm_uint16t->mask = htobe16(mask);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_vlan_vid() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_vlan_vid const& oxm)
	{
		os << "coxmatch_ofb_vlan_vid{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<VLAN-VID: " << oxm.u16value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_VLAN_PCP
 *
 */
class coxmatch_ofb_vlan_pcp :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_vlan_pcp(
			uint8_t pcp) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_VLAN_PCP);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = pcp;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_vlan_pcp() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_vlan_pcp const& oxm)
	{
		os << "coxmatch_ofb_vlan_pcp{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<VLAN-PCP: " << oxm.u8value() << ">";
		os << "}";
		return os;
	};
};



/** OXM_OF_IP_DSCP
 *
 */
class coxmatch_ofb_ip_dscp :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ip_dscp(
			uint8_t dscp) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IP_DSCP);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = (0x3f & dscp); // lower 6 bits only
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ip_dscp() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ip_dscp const& oxm)
	{
		os << "coxmatch_ofb_ip_dscp{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<IP-DSCP: " << oxm.u8value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_IP_ECN
 *
 */
class coxmatch_ofb_ip_ecn :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ip_ecn(
			uint8_t ecn) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IP_ECN);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = (0x03 & ecn); // lower 2 bits only (will be moved up later)
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ip_ecn() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ip_ecn const& oxm)
	{
		os << "coxmatch_ofb_ip_ecn{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<IP-ECN: " << oxm.u8value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_IP_PROTO
 *
 */
class coxmatch_ofb_ip_proto :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ip_proto(
			uint8_t proto) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IP_PROTO);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = proto;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ip_proto() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ip_proto const& oxm)
	{
		os << "coxmatch_ofb_ip_proto{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<IP-PROTO: " << oxm.u8value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_IPV4_SRC
 *
 */
class coxmatch_ofb_ipv4_src :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ipv4_src(
			uint32_t src) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV4_SRC);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(src);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv4_src(
			uint32_t src,
			uint32_t mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV4_SRC);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(src);
		oxm_uint32t->mask  = htobe32(mask);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv4_src(
			caddress const& src) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		if (src.ca_saddr->sa_family != AF_INET)
		{
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV4_SRC);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = src.ca_s4addr->sin_addr.s_addr;
	};
	/** constructor
	 */
	coxmatch_ofb_ipv4_src(
			caddress const& src,
			caddress const& mask) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint32_t))
	{
		if ((src.ca_saddr->sa_family != AF_INET) || (mask.ca_saddr->sa_family != AF_INET))
		{
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV4_SRC);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint32_t));
		oxm_uint32t->dword = src.ca_s4addr->sin_addr.s_addr;
		oxm_uint32t->mask  = mask.ca_s4addr->sin_addr.s_addr;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ipv4_src() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv4_src const& oxm)
	{
		os << "coxmatch_ofb_ipv4_src{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<IPV4-SRC: " << oxm.u32addr().addr_c_str() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_IPV4_DST
 *
 */
class coxmatch_ofb_ipv4_dst :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ipv4_dst(
			uint32_t dst) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV4_DST);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(dst);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv4_dst(
			uint32_t dst,
			uint32_t mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV4_DST);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(dst);
		oxm_uint32t->mask  = htobe32(mask);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv4_dst(
			caddress const& dst) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		if (dst.ca_saddr->sa_family != AF_INET)
		{
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV4_DST);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = dst.ca_s4addr->sin_addr.s_addr;
	};
	/** constructor
	 */
	coxmatch_ofb_ipv4_dst(
			caddress const& dst,
			caddress const& mask) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint32_t))
	{
		if ((dst.ca_saddr->sa_family != AF_INET) || (mask.ca_saddr->sa_family != AF_INET))
		{
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV4_DST);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint32_t));
		oxm_uint32t->dword = dst.ca_s4addr->sin_addr.s_addr;
		oxm_uint32t->mask  = mask.ca_s4addr->sin_addr.s_addr;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ipv4_dst() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv4_dst const& oxm)
	{
		os << "coxmatch_ofb_ipv4_dst{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<IPV4-DST: " << oxm.u32addr().addr_c_str() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_IPV6_SRC
 *
 */
class coxmatch_ofb_ipv6_src :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ipv6_src(
			uint8_t *addr, size_t addr_len) throw (eOxmBadLen) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 1 * 16 * sizeof(uint8_t))
	{
		if (addr_len < 16) {
			throw eOxmBadLen();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_SRC);
		set_oxm_length(1 * 16 * sizeof(uint8_t));
		memcpy(oxm_ipv6addr->addr, addr, 16);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv6_src(
			uint8_t *addr, size_t addr_len ,
			uint8_t *mask, size_t mask_len) throw (eOxmBadLen) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * 16 * sizeof(uint8_t))
	{
		if ((addr_len < 16) || (mask_len < 16)) {
			throw eOxmBadLen();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_SRC);
		set_oxm_hasmask(true);
		set_oxm_length(2 * 16 * sizeof(uint8_t));
		memcpy(oxm_ipv6addr->addr, addr, 16);
		memcpy(oxm_ipv6addr->mask, mask, 16);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv6_src(
			caddress const& addr) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 1 * 16 * sizeof(uint8_t))
	{
		if (addr.ca_saddr->sa_family != AF_INET6) {
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_SRC);
		set_oxm_length(1 * 16 * sizeof(uint8_t));
		memcpy(oxm_ipv6addr->addr, addr.ca_s6addr->sin6_addr.s6_addr, 16);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv6_src(
			caddress const& addr,
			caddress const& mask) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * 16 * sizeof(uint8_t))
	{
		if ((addr.ca_saddr->sa_family != AF_INET6) || (mask.ca_saddr->sa_family != AF_INET6)) {
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_SRC);
		set_oxm_hasmask(true);
		set_oxm_length(2 * 16 * sizeof(uint8_t));
		memcpy(oxm_ipv6addr->addr, addr.ca_s6addr->sin6_addr.s6_addr, 16);
		memcpy(oxm_ipv6addr->mask, mask.ca_s6addr->sin6_addr.s6_addr, 16);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ipv6_src() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_src const& oxm)
	{
		os << "coxmatch_ofb_ipv6_src{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<IPV6-SRC: " << oxm.u128addr().addr_c_str() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_IPV6_DST
 *
 */
class coxmatch_ofb_ipv6_dst :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ipv6_dst(
			uint8_t *addr, size_t addr_len) throw (eOxmBadLen) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 1 * 16 * sizeof(uint8_t))
	{
		if (addr_len < 16) {
			throw eOxmBadLen();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_DST);
		set_oxm_length(1 * 16 * sizeof(uint8_t));
		memcpy(oxm_ipv6addr->addr, addr, 16);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv6_dst(
			uint8_t *addr, size_t addr_len ,
			uint8_t *mask, size_t mask_len) throw (eOxmBadLen) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * 16 * sizeof(uint8_t))
	{
		if ((addr_len < 16) || (mask_len < 16)) {
			throw eOxmBadLen();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_DST);
		set_oxm_hasmask(true);
		set_oxm_length(2 * 16 * sizeof(uint8_t));
		memcpy(oxm_ipv6addr->addr, addr, 16);
		memcpy(oxm_ipv6addr->mask, mask, 16);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv6_dst(
			caddress const& addr) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 1 * 16 * sizeof(uint8_t))
	{
		if (addr.ca_saddr->sa_family != AF_INET6) {
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_DST);
		set_oxm_length(1 * 16 * sizeof(uint8_t));
		memcpy(oxm_ipv6addr->addr, addr.ca_s6addr->sin6_addr.s6_addr, 16);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv6_dst(
			caddress const& addr,
			caddress const& mask) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * 16 * sizeof(uint8_t))
	{
		if ((addr.ca_saddr->sa_family != AF_INET6) || (mask.ca_saddr->sa_family != AF_INET6)) {
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_DST);
		set_oxm_hasmask(true);
		set_oxm_length(2 * 16 * sizeof(uint8_t));
		memcpy(oxm_ipv6addr->addr, addr.ca_s6addr->sin6_addr.s6_addr, 16);
		memcpy(oxm_ipv6addr->mask, mask.ca_s6addr->sin6_addr.s6_addr, 16);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ipv6_dst() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_dst const& oxm)
	{
		os << "coxmatch_ofb_ipv6_dst{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<IPV6-DST: " << oxm.u128addr().addr_c_str() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_IPV6_DST
 *
 */
class coxmatch_ofb_ipv6_nd_target :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ipv6_nd_target(
			uint8_t *addr, size_t addr_len) throw (eOxmBadLen) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 1 * 16 * sizeof(uint8_t))
	{
		if (addr_len < 16) {
			throw eOxmBadLen();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_ND_TARGET);
		set_oxm_length(1 * 16 * sizeof(uint8_t));
		memcpy(oxm_ipv6addr->addr, addr, 16);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv6_nd_target(
			caddress const& addr) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 1 * 16 * sizeof(uint8_t))
	{
		if (addr.ca_saddr->sa_family != AF_INET6) {
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_ND_TARGET);
		set_oxm_length(1 * 16 * sizeof(uint8_t));
		memcpy(oxm_ipv6addr->addr, addr.ca_s6addr->sin6_addr.s6_addr, 16);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ipv6_nd_target() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_nd_target const& oxm)
	{
		os << "coxmatch_ofb_ipv6_nd_target{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ND-TARGET: " << oxm.u128addr().addr_c_str() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_TCP_SRC
 *
 */
class coxmatch_ofb_tcp_src :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_tcp_src(
			uint16_t src) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_TCP_SRC);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(src);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_tcp_src() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_tcp_src const& oxm)
	{
		os << "coxmatch_ofb_tcp_src{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<TCP-SRC: " << (unsigned int)oxm.u16value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_TCP_DST
 *
 */
class coxmatch_ofb_tcp_dst :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_tcp_dst(
			uint16_t dst) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_TCP_DST);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(dst);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_tcp_dst() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_tcp_dst const& oxm)
	{
		os << "coxmatch_ofb_tcp_dst{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<TCP-DST: " << (unsigned int)oxm.u16value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_UDP_SRC
 *
 */
class coxmatch_ofb_udp_src :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_udp_src(
			uint16_t src) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_UDP_SRC);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(src);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_udp_src() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_udp_src const& oxm)
	{
		os << "coxmatch_ofb_udp_src{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<UDP-SRC: " << (unsigned int)oxm.u16value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_UDP_DST
 *
 */
class coxmatch_ofb_udp_dst :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_udp_dst(
			uint16_t dst) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_UDP_DST);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(dst);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_udp_dst() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_udp_dst const& oxm)
	{
		os << "coxmatch_ofb_udp_dst{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<UDP-DST: " << (unsigned int)oxm.u16value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_SCTP_SRC
 *
 */
class coxmatch_ofb_sctp_src :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_sctp_src(
			uint16_t src) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_SCTP_SRC);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(src);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_sctp_src() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_sctp_src const& oxm)
	{
		os << "coxmatch_ofb_sctp_src{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<SCTP-SRC: " << (unsigned int)oxm.u16value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_SCTP_DST
 *
 */
class coxmatch_ofb_sctp_dst :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_sctp_dst(
			uint16_t dst) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_SCTP_DST);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(dst);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_sctp_dst() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_sctp_dst const& oxm)
	{
		os << "coxmatch_ofb_sctp_dst{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<SCTP-DST: " << (unsigned int)oxm.u16value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_ICMPV4_TYPE
 *
 */
class coxmatch_ofb_icmpv4_type :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_icmpv4_type(
			uint8_t type) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ICMPV4_TYPE);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = type;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_icmpv4_type() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_icmpv4_type const& oxm)
	{
		os << "coxmatch_ofb_icmpv4_type{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ICMPv4-TYPE: " << (unsigned int)oxm.u8value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_ICMPV4_CODE
 *
 */
class coxmatch_ofb_icmpv4_code :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_icmpv4_code(
			uint8_t code) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ICMPV4_CODE);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = code;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_icmpv4_code() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_icmpv4_code const& oxm)
	{
		os << "coxmatch_ofb_icmpv4_code{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ICMPv4-CODE: " << (unsigned int)oxm.u8value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_ARP_OP
 *
 */
class coxmatch_ofb_arp_op :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_arp_op(
			uint16_t opcode) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_OP);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(opcode);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_arp_op() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_arp_op const& oxm)
	{
		os << "coxmatch_ofb_arp_op{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ARP-OP: " << (unsigned int)oxm.u16value() << ">";
		os << "}";
		return os;
	};
};



/** OXM_OF_ARP_SPA
 *
 */
class coxmatch_ofb_arp_spa :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_arp_spa(
			uint32_t spa) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_SPA);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(spa);
	};
	/** constructor
	 */
	coxmatch_ofb_arp_spa(
			uint32_t spa,
			uint32_t mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_SPA);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(spa);
		oxm_uint32t->mask = htobe32(mask);
	};
	/** constructor
	 */
	coxmatch_ofb_arp_spa(
			caddress const& spa) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		if (spa.ca_saddr->sa_family != AF_INET)
		{
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_SPA);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = spa.ca_s4addr->sin_addr.s_addr;
	};
	/** constructor
	 */
	coxmatch_ofb_arp_spa(
			caddress const& spa,
			caddress const& mask) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint32_t))
	{
		if ((spa.ca_saddr->sa_family != AF_INET) || (mask.ca_saddr->sa_family != AF_INET))
		{
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_SPA);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint32_t));
		oxm_uint32t->dword = spa.ca_s4addr->sin_addr.s_addr;
		oxm_uint32t->mask  = mask.ca_s4addr->sin_addr.s_addr;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_arp_spa() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_arp_spa const& oxm)
	{
		os << "coxmatch_ofb_arp_spa{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ARP-SPA: " << (unsigned int)oxm.u32addr().addr_c_str() << ">";
		os << "}";
		return os;
	};
};



/** OXM_OF_ARP_TPA
 *
 */
class coxmatch_ofb_arp_tpa :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_arp_tpa(
			uint32_t tpa) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_TPA);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(tpa);
	};
	/** constructor
	 */
	coxmatch_ofb_arp_tpa(
			uint32_t tpa,
			uint32_t mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_TPA);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(tpa);
		oxm_uint32t->mask = htobe32(mask);
	};
	/** constructor
	 */
	coxmatch_ofb_arp_tpa(
			caddress const& tpa) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		if (tpa.ca_saddr->sa_family != AF_INET)
		{
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_TPA);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = tpa.ca_s4addr->sin_addr.s_addr;
	};
	/** constructor
	 */
	coxmatch_ofb_arp_tpa(
			caddress const& tpa,
			caddress const& mask) throw (eOxmInval) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint32_t))
	{
		if ((tpa.ca_saddr->sa_family != AF_INET) || (mask.ca_saddr->sa_family != AF_INET))
		{
			throw eOxmInval();
		}
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_TPA);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint32_t));
		oxm_uint32t->dword = tpa.ca_s4addr->sin_addr.s_addr;
		oxm_uint32t->mask  = mask.ca_s4addr->sin_addr.s_addr;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_arp_tpa() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_arp_tpa const& oxm)
	{
		os << "coxmatch_ofb_arp_tpa{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ARP-TPA: " << (unsigned int)oxm.u32addr().addr_c_str() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_ARP_SHA
 *
 */
class coxmatch_ofb_arp_sha :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_arp_sha(
			cmacaddr const& maddr) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_SHA);
		set_oxm_length(OFP_ETH_ALEN);
		memcpy(oxm_uint48t->value, maddr.somem(), OFP_ETH_ALEN);
	};
	/** constructor
	 */
	coxmatch_ofb_arp_sha(
			cmacaddr const& maddr,
			cmacaddr const& mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_SHA);
		set_oxm_hasmask(true);
		set_oxm_length(2 * OFP_ETH_ALEN);
		memcpy(oxm_uint48t->value, maddr.somem(), OFP_ETH_ALEN);
		memcpy(oxm_uint48t->mask, mask.somem(), OFP_ETH_ALEN);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_arp_sha() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_arp_sha const& oxm)
	{
		os << "coxmatch_ofb_arp_sha{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ARP-SHA: " << (unsigned int)oxm.u48addr().c_str() << ">";
		os << "}";
		return os;
	};
};



/** OXM_OF_ARP_THA
 *
 */
class coxmatch_ofb_arp_tha :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_arp_tha(
			cmacaddr const& maddr) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_THA);
		set_oxm_length(OFP_ETH_ALEN);
		memcpy(oxm_uint48t->value, maddr.somem(), OFP_ETH_ALEN);
	};
	/** constructor
	 */
	coxmatch_ofb_arp_tha(
			cmacaddr const& maddr,
			cmacaddr const& mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ARP_THA);
		set_oxm_hasmask(true);
		set_oxm_length(2 * OFP_ETH_ALEN);
		memcpy(oxm_uint48t->value, maddr.somem(), OFP_ETH_ALEN);
		memcpy(oxm_uint48t->mask, mask.somem(), OFP_ETH_ALEN);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_arp_tha() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_arp_tha const& oxm)
	{
		os << "coxmatch_ofb_arp_tha{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ARP-THA: " << (unsigned int)oxm.u48addr().c_str() << ">";
		os << "}";
		return os;
	};
};





/** OXM_OF_IPV6_FLABEL
 *
 */
class coxmatch_ofb_ipv6_flabel :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ipv6_flabel(
			uint32_t flow_label) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_FLABEL);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(flow_label);
	};
	/** constructor
	 */
	coxmatch_ofb_ipv6_flabel(
			uint32_t flow_label,
			uint32_t mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_FLABEL);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(flow_label);
		oxm_uint32t->mask = htobe32(mask);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ipv6_flabel() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_flabel const& oxm)
	{
		os << "coxmatch_ofb_ipv6_flabel{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<IPv6-FLOWLABEL: " << (unsigned int)oxm.u32value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_ICMPV6_TYPE
 *
 */
class coxmatch_ofb_icmpv6_type :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_icmpv6_type(
			uint8_t type) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ICMPV6_TYPE);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = type;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_icmpv6_type() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_icmpv6_type const& oxm)
	{
		os << "coxmatch_ofb_icmpv6_type{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ICMPv6-TYPE: " << (unsigned int)oxm.u8value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_ICMPV6_CODE
 *
 */
class coxmatch_ofb_icmpv6_code :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_icmpv6_code(
			uint8_t code) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_ICMPV6_CODE);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = code;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_icmpv6_code() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_icmpv6_code const& oxm)
	{
		os << "coxmatch_ofb_icmpv6_code{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ICMPv6-CODE: " << (unsigned int)oxm.u8value() << ">";
		os << "}";
		return os;
	};
};



/** OXM_OF_IPV6_ND_SLL
 *
 */
class coxmatch_ofb_ipv6_nd_sll :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ipv6_nd_sll(
			cmacaddr const& addr) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_ND_SLL);
		set_oxm_length(16);
		memcpy(oxm_maddr->addr, addr.somem(), OFP_ETH_ALEN);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ipv6_nd_sll() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofb_ipv6_nd_sll const& oxm)
	{
		os << "coxmatch_ofb_icmpv6_code{";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<ICMPv6-CODE: " << (unsigned int)oxm.u8value() << ">";
		os << "}";
		return os;
	};
};


/** OXM_OF_IPV6_ND_TLL
 *
 */
class coxmatch_ofb_ipv6_nd_tll :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_ipv6_nd_tll(
			cmacaddr const& addr) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_IPV6_ND_TLL);
		set_oxm_length(16);
		memcpy(oxm_maddr->addr, addr.somem(), OFP_ETH_ALEN);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_ipv6_nd_tll() {};
};


/** OXM_OF_MPLS_LABEL
 *
 */
class coxmatch_ofb_mpls_label :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_mpls_label(
			uint32_t mpls_label) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_MPLS_LABEL);
		set_oxm_length(sizeof(uint32_t));
		oxm_uint32t->dword = htobe32(mpls_label);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_mpls_label() {};
};


/** OXM_OF_MPLS_TC
 *
 */
class coxmatch_ofb_mpls_tc :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofb_mpls_tc(
			uint8_t mpls_tc) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_OPENFLOW_BASIC);
		set_oxm_field(OFPXMT_OFB_MPLS_TC);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = mpls_tc;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofb_mpls_tc() {};
};


/** OXM_OF_PPPOE_CODE
 *
 */
class coxmatch_ofx_pppoe_code :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_pppoe_code(
			uint8_t code) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_PPPOE_CODE);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = code;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_pppoe_code() {};
};


/** OXM_OF_PPPOE_TYPE
 *
 */
class coxmatch_ofx_pppoe_type :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_pppoe_type(
			uint8_t type) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_PPPOE_TYPE);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = type;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_pppoe_type() {};
};


/** OXM_OF_PPPOE_SID
 *
 */
class coxmatch_ofx_pppoe_sid :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_pppoe_sid(
			uint16_t sessid) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_PPPOE_SID);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(sessid);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_pppoe_sid() {};
};


/** OXM_OF_PPP_PROT
 *
 */
class coxmatch_ofx_ppp_prot :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_ppp_prot(
			uint16_t prot) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_PPP_PROT);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(prot);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_ppp_prot() {};
};

}; // end of namespace

#endif /* COXMATCH_H_ */
