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

#include "rofl/common/openflow/openflow.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/cmacaddr.h"
#include "rofl/common/cvastring.h"
#include "rofl/common/coflist.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/platform/unix/csyslog.h"
#include "rofl/common/openflow/coxmatch.h"
#include "rofl/common/logging.h"

#ifndef ORAN
#define ORAN 1
#endif

#define ROFL_EXPERIMENTER_ID	0x555501dd


namespace rofl
{

/* error classes */
class eActionBase 			: public RoflException {};
class eActionInval 			: public eActionBase {}; // invalid parameter
class eActionInvalType 		: public eActionBase {}; // invalid action type



class cofaction : public csyslog {

	#define COFACTION_DEFAULT_SIZE	128  // default action size: 128 bytes

public: // static methods

public: // data structures

	union { // for OpenFlow 1.1
		uint8_t											*oacu_generic;
		struct openflow::ofp_action_header				*oacu_header;

		// OF1.0 actions
		struct openflow10::ofp_action_output			*oacu_10output;
		struct openflow10::ofp_action_enqueue			*oacu_10enqueue;
		struct openflow10::ofp_action_vlan_vid			*oacu_10vlanvid;
		struct openflow10::ofp_action_vlan_pcp			*oacu_10vlanpcp;
		struct openflow10::ofp_action_dl_addr			*oacu_10dladdr;
		struct openflow10::ofp_action_nw_addr			*oacu_10nwaddr;
		struct openflow10::ofp_action_nw_tos			*oacu_10nwtos;
		struct openflow10::ofp_action_tp_port			*oacu_10tpport;
		struct openflow10::ofp_action_vendor_header		*oacu_10vendor;

		// OF1.2 actions
		struct openflow12::ofp_action_output			*oacu_12output;
		struct openflow12::ofp_action_mpls_ttl			*oacu_12mpls_ttl;
		struct openflow12::ofp_action_push 				*oacu_12push;
		struct openflow12::ofp_action_pop_mpls 			*oacu_12pop_mpls;
		struct openflow12::ofp_action_group 			*oacu_12group;
		struct openflow12::ofp_action_nw_ttl 			*oacu_12nw_ttl;
		struct openflow12::ofp_action_experimenter_header *oacu_12experimenter;
		struct openflow12::ofp_action_set_queue 		*oacu_12set_queue;
		struct openflow12::ofp_action_set_field 		*oacu_12set_field;

		// OF1.3 actions
		struct openflow13::ofp_action_output			*oacu_13output;
		struct openflow13::ofp_action_mpls_ttl			*oacu_13mpls_ttl;
		struct openflow13::ofp_action_push 				*oacu_13push;
		struct openflow13::ofp_action_pop_mpls 			*oacu_13pop_mpls;
		struct openflow13::ofp_action_group 			*oacu_13group;
		struct openflow13::ofp_action_nw_ttl 			*oacu_13nw_ttl;
		struct openflow13::ofp_action_experimenter_header *oacu_13experimenter;
		struct openflow13::ofp_action_set_queue 		*oacu_13set_queue;
		struct openflow13::ofp_action_set_field 		*oacu_13set_field;
	} oac_oacu;

#define oac_generic 	oac_oacu.oacu_generic		// action: generic pointer
#define oac_header 		oac_oacu.oacu_header		// action: plain header

#define oac_10output 	oac_oacu.oacu_10output		// action: output OF1.0
#define oac_10enqueue 	oac_oacu.oacu_10enqueue		// action: enqueue
#define oac_10vlanvid 	oac_oacu.oacu_10vlanvid		// action: vlan_vid
#define oac_10vlanpcp 	oac_oacu.oacu_10vlanpcp		// action: vlan_pcp
#define oac_10dladdr 	oac_oacu.oacu_10dladdr		// action: dl_addr
#define oac_10nwaddr 	oac_oacu.oacu_10nwaddr		// action: nw_addr
#define oac_10nwtos 	oac_oacu.oacu_10nwtos		// action: nw_tos
#define oac_10tpport 	oac_oacu.oacu_10tpport		// action: tp_port
#define oac_10vendor 	oac_oacu.oacu_10vendor		// action: vendor

#define oac_12output 	oac_oacu.oacu_12output		// action: output OF1.2/OF1.3
#define oac_12mpls_ttl 	oac_oacu.oacu_12mpls_ttl	// action: mpls_ttl
#define oac_12push 		oac_oacu.oacu_12push		// action: push
#define oac_12pop_mpls 	oac_oacu.oacu_12pop_mpls	// action: pop_mpls
#define oac_12group 	oac_oacu.oacu_12group		// action: group
#define oac_12nw_ttl 	oac_oacu.oacu_12nw_ttl		// action: nw_ttl
#define oac_12experimenter oac_oacu.oacu_12experimenter	// action: experimenter_header
#define oac_12set_queue oac_oacu.oacu_12set_queue	// action: set_queue
#define oac_12set_field oac_oacu.oacu_12set_field	// action: set field

#define oac_13output 	oac_oacu.oacu_13output		// action: output OF1.2/OF1.3
#define oac_13mpls_ttl 	oac_oacu.oacu_13mpls_ttl	// action: mpls_ttl
#define oac_13push 		oac_oacu.oacu_13push		// action: push
#define oac_13pop_mpls 	oac_oacu.oacu_13pop_mpls	// action: pop_mpls
#define oac_13group 	oac_oacu.oacu_13group		// action: group
#define oac_13nw_ttl 	oac_oacu.oacu_13nw_ttl		// action: nw_ttl
#define oac_13experimenter oac_oacu.oacu_13experimenter	// action: experimenter_header
#define oac_13set_queue oac_oacu.oacu_13set_queue	// action: set_queue
#define oac_13set_field oac_oacu.oacu_13set_field	// action: set field

protected: // data structures

	uint8_t ofp_version;
	cmemory action;
	std::string info;

public: // methods

	/** constructor
	 */
	cofaction(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN,
			size_t datalen = COFACTION_DEFAULT_SIZE);

	/** constructor
	 */
	cofaction(
			uint8_t ofp_version,
			struct openflow::ofp_action_header* action,
			size_t aclen);

	/** copy constructor
	 */
	cofaction(cofaction const& action);

	/** assignment operator
	 */
	cofaction&
	operator= (const cofaction& ac);

	/** destructor
	 */
	virtual ~cofaction();



	/** return pointer to ofp_action_header start
	 */
	struct openflow::ofp_action_header*
	soaction() const;

	/** return length of action in bytes
	 */
	size_t
	length() const;

	/** copy struct openflow::ofp_action_header
	 */
	uint8_t*
	pack(uint8_t* achdr, size_t aclen);

	/** unpack
	 */
	void
	unpack(uint8_t* achdr, size_t aclen);

	/**
	 *
	 */
	uint8_t
	get_version() const;

	/**
	 *
	 */
	uint16_t
	get_type() const;

	/**
	 *
	 */
	void
	set_type(uint16_t type);

	/**
	 *
	 */
	uint16_t
	get_length() const;

	/**
	 *
	 */
	void
	set_length(uint16_t len);

	/** return oxm for OFPAT_SET_FIELD
	 *
	 */
	coxmatch
	get_oxm() const;

	/**
	 *
	 */
	virtual void
	check_prerequisites() const {};

protected: // methods

	/**
	 *
	 */
	void
	resize(size_t size);

private:

	/** unpack OF1.0
	 */
	void
	unpack_of10(
			uint8_t* achdr, size_t aclen);

	/** unpack OF1.2
	 */
	void
	unpack_of12(
			uint8_t* achdr, size_t aclen);


	/** unpack OF1.3
	 */
	void
	unpack_of13(
			uint8_t* achdr, size_t aclen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction const& action) {
		os << indent(0) << "<cofaction ";
			os << "ofp-version:" << (int)action.ofp_version << " ";
			os << "type:" << (int)action.get_type() << " ";
			os << "length:" << (int)action.length() << " ";
		os << " >" << std::endl;
		return os;
	};
};


/** predicate for finding cofaction instances of
 * a specific type
 */
class cofaction_find_type {
public:
	cofaction_find_type(uint16_t type) : 
		type(type) { };

	bool operator() (cofaction const& action) {
		return (be16toh(action.oac_header->type) == type);
	};

	bool operator() (cofaction const* action) {
		return (be16toh(action->oac_header->type) == type);
	};

	uint16_t type;
};

/** OFPAT_OUTPUT
 *
 */
class cofaction_output : public cofaction {
public:
	/** constructor
	 */
	cofaction_output(
			uint8_t ofp_version,
			uint32_t port,
			uint16_t max_len = 128) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_output));
			oac_10output->type 		= htobe16(openflow10::OFPAT_OUTPUT);
			oac_10output->len 		= htobe16(sizeof(struct openflow10::ofp_action_output));
			oac_10output->port 		= htobe16(port);
			oac_10output->max_len 	= htobe16(max_len);
		} break;
		case openflow12::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow12::ofp_action_output));
			oac_12output->type 		= htobe16(openflow12::OFPAT_OUTPUT);
			oac_12output->len 		= htobe16(sizeof(struct openflow12::ofp_action_output));
			oac_12output->port 		= htobe32(port);
			oac_12output->max_len 	= htobe16(max_len);
		} break;
		case openflow13::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow13::ofp_action_output));
			oac_13output->type 		= htobe16(openflow13::OFPAT_OUTPUT);
			oac_13output->len 		= htobe16(sizeof(struct openflow13::ofp_action_output));
			oac_13output->port 		= htobe32(port);
			oac_13output->max_len 	= htobe16(max_len);
		} break;
		default: {
			throw eBadVersion();
		}
		}
	};

	/** constructor
	 */
	cofaction_output(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_output() {};

	/**
	 *
	 */
	virtual void
	check_prerequisites() const;

	/**
	 *
	 */
	uint32_t
	get_port() const;

	/**
	 *
	 */
	void
	set_max_len(uint16_t max_len);

	/**
	 *
	 */
	uint16_t
	get_max_len() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_output const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_output ";
		os << "port:0x" << std::hex << (int)action.get_port() << std::dec << " ";
		os << "max-len:" << (int)action.get_max_len() << " >" << std::endl;
		return os;
	};
};


/*
 * old OF1.0 actions
 */

/** OFPAT_SET_VLAN_VID
 *
 */
class cofaction_set_vlan_vid : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_vlan_vid(
			uint8_t ofp_version,
			uint16_t vlan_vid) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_vlan_vid));
			oac_10vlanvid->type 	= htobe16(openflow10::OFPAT_SET_VLAN_VID);
			oac_10vlanvid->len 		= htobe16(sizeof(struct openflow10::ofp_action_vlan_vid));
			oac_10vlanvid->vlan_vid = htobe16(vlan_vid);
		} break;
		default:
			logging::warn << "cofaction_set_vlan_vid: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_set_vlan_vid(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_vlan_vid() {};

	/**
	 *
	 */
	void
	set_vlan_vid(uint16_t vlan_vid) {
		oac_10vlanvid->vlan_vid = htobe16(vlan_vid);
	};

	/**
	 *
	 */
	uint16_t
	get_vlan_vid() const {
		return be16toh(oac_10vlanvid->vlan_vid);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_vlan_vid const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_vlan_vid ";
		os << "vid:" << (int)action.get_vlan_vid() << " >" << std::endl;
		return os;
	};
};

/** OFPAT_SET_VLAN_PCP
 *
 */
class cofaction_set_vlan_pcp : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_vlan_pcp(
			uint8_t ofp_version,
			uint8_t vlan_pcp) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_vlan_pcp));
			oac_10vlanpcp->type 	= htobe16(openflow10::OFPAT_SET_VLAN_PCP);
			oac_10vlanpcp->len 		= htobe16(sizeof(struct openflow10::ofp_action_vlan_pcp));
			oac_10vlanpcp->vlan_pcp = vlan_pcp;
		} break;
		default:
			logging::warn << "cofaction_set_vlan_pcp: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_set_vlan_pcp(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_vlan_pcp() {};

	/**
	 *
	 */
	void
	set_vlan_pcp(uint8_t vlan_pcp) {
		oac_10vlanpcp->vlan_pcp = vlan_pcp;
	};

	/**
	 *
	 */
	uint8_t
	get_vlan_pcp() const {
		return oac_10vlanpcp->vlan_pcp;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_vlan_pcp const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_vlan_pcp ";
		os << "pcp:" << (int)action.get_vlan_pcp() << " >" << std::endl;
		return os;
	};
};

/** OFPAT_STRIP_VLAN
 *
 */
class cofaction_strip_vlan : public cofaction {
public:
	/** constructor
	 */
	cofaction_strip_vlan(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_header));
			oac_header->type 	= htobe16(openflow10::OFPAT_STRIP_VLAN);
			oac_header->len 	= htobe16(sizeof(struct openflow10::ofp_action_header));
		} break;
		default:
			logging::warn << "cofaction_set_strip_vlan: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_strip_vlan(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_strip_vlan() {};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_strip_vlan const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_strip_vlan >" << std::endl;
		return os;
	};
};

/** OFPAT_SET_DL_SRC
 *
 */
class cofaction_set_dl_src : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_dl_src(
			uint8_t ofp_version,
			rofl::cmacaddr const& maddr) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_dl_addr));
			oac_10dladdr->type 	= htobe16(openflow10::OFPAT_SET_DL_SRC);
			oac_10dladdr->len 	= htobe16(sizeof(struct openflow10::ofp_action_dl_addr));
			memcpy(oac_10dladdr->dl_addr, maddr.somem(), OFP_ETH_ALEN);
		} break;
		default:
			logging::warn << "cofaction_set_dl_src: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/**
	 *
	 */
	cmacaddr
	get_dl_src() const {
		return cmacaddr(oac_10dladdr->dl_addr, OFP_ETH_ALEN);
	};

	/**
	 *
	 */
	void
	set_dl_src(cmacaddr maddr) {
		memcpy(oac_10dladdr->dl_addr, maddr.somem(), OFP_ETH_ALEN);
	};

	/** constructor
	 */
	cofaction_set_dl_src(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_dl_src() {};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_dl_src const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_dl_src ";
		os << "dl-src:" << action.get_dl_src() << " >" << std::endl;
		return os;
	};
};

/** OFPAT_SET_DL_DST
 *
 */
class cofaction_set_dl_dst : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_dl_dst(
			uint8_t ofp_version,
			rofl::cmacaddr const& maddr) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_dl_addr));
			oac_10dladdr->type 	= htobe16(openflow10::OFPAT_SET_DL_DST);
			oac_10dladdr->len 	= htobe16(sizeof(struct openflow10::ofp_action_dl_addr));
			memcpy(oac_10dladdr->dl_addr, maddr.somem(), OFP_ETH_ALEN);
		} break;
		default:
			logging::warn << "cofaction_set_dl_dst: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/**
	 *
	 */
	cmacaddr
	get_dl_dst() const {
		return cmacaddr(oac_10dladdr->dl_addr, OFP_ETH_ALEN);
	};

	/**
	 *
	 */
	void
	set_dl_dst(cmacaddr maddr) {
		memcpy(oac_10dladdr->dl_addr, maddr.somem(), OFP_ETH_ALEN);
	};

	/** constructor
	 */
	cofaction_set_dl_dst(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_dl_dst() {};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_dl_dst const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_dl_dst ";
		os << "dl-dst:" << action.get_dl_dst() << " >" << std::endl;
		return os;
	};
};

/** OFPAT_SET_NW_SRC
 *
 */
class cofaction_set_nw_src : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_nw_src(
			uint8_t ofp_version,
			rofl::caddress const& addr) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_nw_addr));
			oac_10nwaddr->type 		= htobe16(openflow10::OFPAT_SET_NW_SRC);
			oac_10nwaddr->len 		= htobe16(sizeof(struct openflow10::ofp_action_nw_addr));
			oac_10nwaddr->nw_addr 	= addr.ca_s4addr->sin_addr.s_addr;
		} break;
		default:
			logging::warn << "cofaction_set_nw_src: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_set_nw_src(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_nw_src() {};

	/**
	 *
	 */
	caddress
	get_nw_src() const {
		caddress addr(AF_INET, "0.0.0.0");
		addr.ca_s4addr->sin_addr.s_addr = oac_10nwaddr->nw_addr;
		return addr;
	};

	/**
	 *
	 */
	void
	set_nw_src(caddress addr) {
		oac_10nwaddr->nw_addr 	= addr.ca_s4addr->sin_addr.s_addr;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_nw_src const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_nw_src ";
		os << "nw-src:" << action.get_nw_src() << " >" << std::endl;
		return os;
	};
};

/** OFPAT_SET_NW_DST
 *
 */
class cofaction_set_nw_dst : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_nw_dst(
			uint8_t ofp_version,
			rofl::caddress const& addr) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_nw_addr));
			oac_10nwaddr->type 		= htobe16(openflow10::OFPAT_SET_NW_DST);
			oac_10nwaddr->len 		= htobe16(sizeof(struct openflow10::ofp_action_nw_addr));
			oac_10nwaddr->nw_addr 	= addr.ca_s4addr->sin_addr.s_addr;
		} break;
		default:
			logging::warn << "cofaction_set_nw_dst: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_set_nw_dst(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_nw_dst() {};

	/**
	 *
	 */
	caddress
	get_nw_dst() const {
		caddress addr(AF_INET, "0.0.0.0");
		addr.ca_s4addr->sin_addr.s_addr = oac_10nwaddr->nw_addr;
		return addr;
	};

	/**
	 *
	 */
	void
	set_nw_dst(caddress addr) {
		oac_10nwaddr->nw_addr 	= addr.ca_s4addr->sin_addr.s_addr;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_nw_dst const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_nw_dst ";
		os << "nw-dst:" << action.get_nw_dst() << " >" << std::endl;
		return os;
	};
};

/** OFPAT_SET_NW_TOS
 *
 */
class cofaction_set_nw_tos : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_nw_tos(
			uint8_t ofp_version,
			uint8_t nw_tos) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_nw_tos));
			oac_10nwtos->type 	= htobe16(openflow10::OFPAT_SET_NW_TOS);
			oac_10nwtos->len 	= htobe16(sizeof(struct openflow10::ofp_action_nw_tos));
			oac_10nwtos->nw_tos = nw_tos;
		} break;
		default:
			logging::warn << "cofaction_set_nw_tos: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_set_nw_tos(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_nw_tos() {};

	/**
	 *
	 */
	void
	set_nw_tos(uint8_t nw_tos) {
		oac_10nwtos->nw_tos = nw_tos;
	};

	/**
	 *
	 */
	uint8_t
	get_nw_tos() const {
		return oac_10nwtos->nw_tos;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_nw_tos const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_nw_tos ";
		os << "nw-tos:" << (int)action.get_nw_tos() << " >" << std::endl;
		return os;
	};
};

/** OFPAT_SET_TP_SRC
 *
 */
class cofaction_set_tp_src : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_tp_src(
			uint8_t ofp_version,
			uint16_t tp_src) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_tp_port));
			oac_10tpport->type 		= htobe16(openflow10::OFPAT_SET_TP_SRC);
			oac_10tpport->len 		= htobe16(sizeof(struct openflow10::ofp_action_tp_port));
			oac_10tpport->tp_port 	= htobe16(tp_src);
		} break;
		default:
			logging::warn << "cofaction_set_tp_src: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_set_tp_src(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_tp_src() {};

	/**
	 *
	 */
	void
	set_tp_src(uint16_t tp_src) {
		oac_10tpport->tp_port 	= htobe16(tp_src);
	};

	/**
	 *
	 */
	uint16_t
	get_tp_src() const {
		return be16toh(oac_10tpport->tp_port);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_tp_src const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_tp_src ";
		os << "tp-src:" << (int)action.get_tp_src() << " >" << std::endl;
		return os;
	};
};

/** OFPAT_SET_TP_DST
 *
 */
class cofaction_set_tp_dst : public cofaction {
public:
	/** constructor
	 */
	cofaction_set_tp_dst(
			uint8_t ofp_version,
			uint16_t tp_dst) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_tp_port));
			oac_10tpport->type 		= htobe16(openflow10::OFPAT_SET_TP_DST);
			oac_10tpport->len 		= htobe16(sizeof(struct openflow10::ofp_action_tp_port));
			oac_10tpport->tp_port 	= htobe16(tp_dst);
		} break;
		default:
			logging::warn << "cofaction_set_tp_dst: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_set_tp_dst(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_set_tp_dst() {};

	/**
	 *
	 */
	void
	set_tp_dst(uint16_t tp_dst) {
		oac_10tpport->tp_port 	= htobe16(tp_dst);
	};

	/**
	 *
	 */
	uint16_t
	get_tp_dst() const {
		return be16toh(oac_10tpport->tp_port);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_set_tp_dst const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_tp_dst ";
		os << "tp-dst:" << (int)action.get_tp_dst() << " >" << std::endl;
		return os;
	};
};

/** OFPAT_ENQUEUE
 *
 */
class cofaction_enqueue : public cofaction {
public:
	/** constructor
	 */
	cofaction_enqueue(
			uint8_t ofp_version,
			uint16_t port, uint32_t queue_id) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_enqueue));
			oac_10enqueue->type 	= htobe16(openflow10::OFPAT_ENQUEUE);
			oac_10enqueue->len 		= htobe16(sizeof(struct openflow10::ofp_action_enqueue));
			oac_10enqueue->port 	= htobe16(port);
			oac_10enqueue->queue_id = htobe32(queue_id);
		} break;
		default:
			logging::warn << "cofaction_enqueue: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/** constructor
	 */
	cofaction_enqueue(
			cofaction const& action) :
				cofaction(action) {};

	/** destructor
	 */
	virtual
	~cofaction_enqueue() {};

	/**
	 *
	 */
	void
	set_port_no(uint16_t port_no) {
		oac_10enqueue->port	= htobe16(port_no);
	};

	/**
	 *
	 */
	uint16_t
	get_port() const {
		return be16toh(oac_10enqueue->port);
	};

	/**
	 *
	 */
	void
	set_queue_id(uint32_t queue_id) {
		oac_10enqueue->queue_id = htobe32(queue_id);
	};

	/**
	 *
	 */
	uint32_t
	get_queue_id() const {
		return be32toh(oac_10enqueue->queue_id);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_enqueue const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_enqueue ";
		os << "port:0x" << std::hex << (int)action.get_port() << std::dec << " ";
		os << "queue-id:" << (int)action.get_queue_id() << " >" << std::endl;
		return os;
	};
};



/** OFPAT_VENDOR
 *
 */
class cofaction_vendor : public cofaction {
public:
	/** constructor
	 */
	cofaction_vendor(
			uint8_t ofp_version,
			uint32_t vendor,
			uint8_t *data = (uint8_t*)0, size_t datalen = 0) :
				cofaction(ofp_version, sizeof(struct openflow::ofp_action_header))
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			cofaction::resize(sizeof(struct openflow10::ofp_action_vendor_header) + datalen);
			oac_10vendor->type 		= htobe16(openflow10::OFPAT_VENDOR);
			oac_10vendor->len 		= htobe16(sizeof(struct openflow10::ofp_action_vendor_header) + datalen);
			oac_10vendor->vendor 	= htobe32(vendor);

			if (data && datalen) {
				memcpy(oac_10vendor->data, data, datalen);
			}
		} break;
		default:
			logging::warn << "cofaction_vendor: constructor called for invalid OFP version" << std::endl;
			throw eBadVersion();
		}
	};

	/**
	 * constructor
	 */
	cofaction_vendor(cofaction const& action) :
		cofaction(action)
	{
		if (openflow10::OFPAT_VENDOR != action.get_type())
			throw eActionInvalType();
	};

	/** destructor
	 */
	virtual
	~cofaction_vendor() {};

	/**
	 *
	 */
	void
	set_vendor(uint32_t vendor_id) {
		oac_10vendor->vendor = htobe32(vendor_id);
	};

	/**
	 *
	 */
	uint32_t
	get_vendor() const {
		return be32toh(oac_10vendor->vendor);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofaction_vendor const& action) {
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_vendor ";
		os << "vendor-id:" << (int)action.get_vendor() << " >" << std::endl;
		os << action.action;
		return os;
	};
};



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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_mpls_ttl ";
		os << "mpls-ttl:" << (int)action.get_mpls_ttl() << " >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_dec_mpls_ttl >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_push_vlan ";
		os << "eth-type:0x" << std::hex << (int)action.get_eth_type() << std::dec << " >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_push_mpls ";
		os << "eth-type:0x" << std::hex << (int)action.get_eth_type() << std::dec << " >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_pop_vlan >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_pop_mpls ";
		os << "eth-type:0x" << std::hex << (int)action.get_eth_type() << std::dec << " >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_group ";
		os << "group-id:" << (int)action.get_group_id() << " >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_nw_ttl ";
		os << "nw-ttl:" << (int)action.get_nw_ttl() << " >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_dec_nw_ttl >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_copy_ttl_out >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_copy_ttl_in >" << std::endl;
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_queue ";
		os << "queue-id:" << (int)action.get_queue_id() << " >" << std::endl;
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

			memcpy(oac_12set_field->field, (void*)oxm.sooxm(), oxm.length());
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_set_field >" << std::endl;
#if 0
		coxmatch oxm((struct openflow::ofp_oxm_hdr*)(action.oac_12set_field->field),
				be16toh(action.oac_12set_field->len) - 4*sizeof(uint8_t));
#endif
		indent i(4);
		os << coxmatch_output(action.get_oxm());
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
		os << dynamic_cast<cofaction const&>( action );
		os << indent(2) << "<cofaction_experimenter ";
		os << "exp-id:" << (int)action.get_exp_id() << " ";
		os << "exp-type:" << (int)action.get_exp_type() << " >" << std::endl;
		return os;
	};
};




}; // end of namespace

#endif // COFACTION_H
