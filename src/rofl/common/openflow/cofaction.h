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

#include "openflow.h"
#include "../cerror.h"
#include "../cmemory.h"
#include "../cmacaddr.h"
#include "../cvastring.h"
#include "../coflist.h"
#include "openflow_rofl_exceptions.h"
#include "rofl/platform/unix/csyslog.h"
#include "coxmatch.h"

#ifndef ORAN
#define ORAN 1
#endif

#define ROFL_EXPERIMENTER_ID	0x555501dd


namespace rofl
{

/* error classes */
class eActionBase 			: public cerror {};
class eActionInval 			: public eActionBase {}; // invalid parameter
class eActionInvalType 		: public eActionBase {}; // invalid action type



class cofaction : public csyslog {

	#define COFACTION_DEFAULT_SIZE	128  // default action size: 128 bytes

public: // static methods

public: // data structures

	union { // for OpenFlow 1.1
		struct ofp_action_header				*oacu_header;

		// OF1.0 actions
		struct ofp10_action_output				*oacu_10output;
		struct ofp10_action_enqueue				*oacu_10enqueue;
		struct ofp10_action_vlan_vid			*oacu_10vlanvid;
		struct ofp10_action_vlan_pcp			*oacu_10vlanpcp;
		struct ofp10_action_dl_addr				*oacu_10dladdr;
		struct ofp10_action_nw_addr				*oacu_10nwaddr;
		struct ofp10_action_nw_tos				*oacu_10nwtos;
		struct ofp10_action_tp_port				*oacu_10tpport;
		struct ofp10_action_vendor_header		*oacu_10vendor;

		// OF1.2 actions
		struct ofp12_action_output				*oacu_12output;
		struct ofp12_action_mpls_ttl			*oacu_12mpls_ttl;
		struct ofp12_action_push 				*oacu_12push;
		struct ofp12_action_pop_mpls 			*oacu_12pop_mpls;
		struct ofp12_action_group 				*oacu_12group;
		struct ofp12_action_nw_ttl 				*oacu_12nw_ttl;
		struct ofp12_action_experimenter_header *oacu_12experimenter;
		struct ofp12_action_set_queue 			*oacu_12set_queue;
		struct ofp12_action_set_field 			*oacu_12set_field;
	} oac_oacu;

#define oac_header oac_oacu.oacu_header				// action: plain header

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

protected: // data structures

	uint8_t ofp_version;
	cmemory action;
	std::string info;

public: // methods

	/** constructor
	 */
	cofaction(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN,
			size_t datalen = COFACTION_DEFAULT_SIZE);

	/** constructor
	 */
	cofaction(
			uint8_t ofp_version,
			struct ofp_action_header* action,
			size_t aclen) throw (eBadActionBadLen, eBadActionBadOutPort);

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
	struct ofp_action_header* 
	soaction() const;

	/** return length of action in bytes
	 */
	size_t
	length() const;

	/** dump info string for this action
	 */
	const char*
	c_str();

	/** copy struct ofp_action_header
	 */
	struct ofp_action_header*
	pack(
			struct ofp_action_header* achdr,
			size_t aclen) const throw (eActionInval);

	/** unpack
	 */
	void
	unpack(
			struct ofp_action_header *achdr,
			size_t aclen)
			throw (eBadActionBadLen, eBadActionBadOutPort, eBadActionBadType);

	/**
	 *
	 */
	uint16_t
	get_type() const;

	/** return oxm for OFPAT_SET_FIELD
	 *
	 */
	coxmatch
	get_oxm() throw (eActionInvalType);

protected: // methods

	/**
	 *
	 */
	void
	resize(size_t size);

	/** create info string
	 */
	void
	__make_info();

private:

	/** unpack OF1.0
	 */
	void
	unpack(
			struct ofp10_action_header *achdr, size_t aclen);

	/** unpack OF1.2
	 */
	void
	unpack(
			struct ofp12_action_header *achdr, size_t aclen);


	/** unpack OF1.3
	 */
	void
	unpack(
			struct ofp13_action_header *achdr, size_t aclen);
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
				cofaction(ofp_version, sizeof(struct ofp10_action_output))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_output));
			oac_10output->type 		= htobe16(OFP10AT_OUTPUT);
			oac_10output->len 		= htobe16(sizeof(struct ofp10_action_output));
			oac_10output->port 		= htobe16(port);
			oac_10output->max_len 	= htobe16(max_len);
		} break;
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_output));
			oac_12output->type 		= htobe16(OFP12AT_OUTPUT);
			oac_12output->len 		= htobe16(sizeof(struct ofp12_action_output));
			oac_12output->port 		= htobe32(port);
			oac_12output->max_len 	= htobe16(max_len);
		} break;
		default: {
			throw eBadVersion();
		}
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_output() {};

	/**
	 *
	 */
	uint32_t
	get_port() throw (eActionInvalType);

	/**
	 *
	 */
	void
	set_max_len(uint16_t max_len) const throw (eActionInvalType);

	/**
	 *
	 */
	uint16_t
	get_max_len() const throw (eActionInvalType);
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
				cofaction(ofp_version, sizeof(struct ofp10_action_vlan_vid))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_vlan_vid));
			oac_10vlanvid->type 	= htobe16(OFP10AT_SET_VLAN_VID);
			oac_10vlanvid->len 		= htobe16(sizeof(struct ofp10_action_vlan_vid));
			oac_10vlanvid->vlan_vid = vlan_vid;
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_vlan_vid() {};
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
				cofaction(ofp_version, sizeof(struct ofp10_action_vlan_pcp))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_vlan_pcp));
			oac_10vlanpcp->type 	= htobe16(OFP10AT_SET_VLAN_PCP);
			oac_10vlanpcp->len 		= htobe16(sizeof(struct ofp10_action_vlan_pcp));
			oac_10vlanpcp->vlan_pcp = vlan_pcp;
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_vlan_pcp() {};
};

/** OFPAT_STRIP_VLAN
 *
 */
class cofaction_strip_vlan : public cofaction {
public:
	/** constructor
	 */
	cofaction_strip_vlan(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct ofp10_action_header))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_header));
			oac_header->type 	= htobe16(OFP10AT_STRIP_VLAN);
			oac_header->len 	= htobe16(sizeof(struct ofp10_action_header));
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_strip_vlan() {};
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
				cofaction(ofp_version, sizeof(struct ofp10_action_dl_addr))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_dl_addr));
			oac_10dladdr->type 	= htobe16(OFP10AT_SET_DL_SRC);
			oac_10dladdr->len 	= htobe16(sizeof(struct ofp10_action_dl_addr));
			memcpy(oac_10dladdr->dl_addr, maddr.somem(), OFP_ETH_ALEN);
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_dl_src() {};
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
				cofaction(ofp_version, sizeof(struct ofp10_action_dl_addr))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_dl_addr));
			oac_10dladdr->type 	= htobe16(OFP10AT_SET_DL_DST);
			oac_10dladdr->len 	= htobe16(sizeof(struct ofp10_action_dl_addr));
			memcpy(oac_10dladdr->dl_addr, maddr.somem(), OFP_ETH_ALEN);
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_dl_dst() {};
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
				cofaction(ofp_version, sizeof(struct ofp10_action_nw_addr))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_nw_addr));
			oac_10nwaddr->type 		= htobe16(OFP10AT_SET_NW_SRC);
			oac_10nwaddr->len 		= htobe16(sizeof(struct ofp10_action_nw_addr));
			oac_10nwaddr->nw_addr 	= addr.ca_s4addr->sin_addr.s_addr;
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_nw_src() {};
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
				cofaction(ofp_version, sizeof(struct ofp10_action_nw_addr))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_nw_addr));
			oac_10nwaddr->type 		= htobe16(OFP10AT_SET_NW_DST);
			oac_10nwaddr->len 		= htobe16(sizeof(struct ofp10_action_nw_addr));
			oac_10nwaddr->nw_addr 	= addr.ca_s4addr->sin_addr.s_addr;
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_nw_dst() {};
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
				cofaction(ofp_version, sizeof(struct ofp10_action_nw_tos))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_nw_tos));
			oac_10nwtos->type 	= htobe16(OFP10AT_SET_NW_TOS);
			oac_10nwtos->len 	= htobe16(sizeof(struct ofp10_action_nw_tos));
			oac_10nwtos->nw_tos = nw_tos;
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_nw_tos() {};
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
				cofaction(ofp_version, sizeof(struct ofp10_action_tp_port))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_tp_port));
			oac_10tpport->type 		= htobe16(OFP10AT_SET_TP_SRC);
			oac_10tpport->len 		= htobe16(sizeof(struct ofp10_action_tp_port));
			oac_10tpport->tp_port 	= tp_src;
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_tp_src() {};
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
				cofaction(ofp_version, sizeof(struct ofp10_action_tp_port))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_tp_port));
			oac_10tpport->type 		= htobe16(OFP10AT_SET_TP_DST);
			oac_10tpport->len 		= htobe16(sizeof(struct ofp10_action_tp_port));
			oac_10tpport->tp_port 	= tp_dst;
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_tp_dst() {};
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
				cofaction(ofp_version, sizeof(struct ofp10_action_enqueue))
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_enqueue));
			oac_10enqueue->type 	= htobe16(OFP10AT_ENQUEUE);
			oac_10enqueue->len 		= htobe16(sizeof(struct ofp10_action_enqueue));
			oac_10enqueue->port 	= htobe16(port);
			oac_10enqueue->queue_id = htobe32(queue_id);
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_enqueue() {};
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
				cofaction(ofp_version, sizeof(struct ofp10_action_vendor_header) + datalen)
	{
		switch (ofp_version) {
		case OFP10_VERSION: {
			cofaction::resize(sizeof(struct ofp10_action_vendor_header) + datalen);
			oac_10vendor->type 		= htobe16(OFP10AT_VENDOR);
			oac_10vendor->len 		= htobe16(sizeof(struct ofp10_action_vendor_header) + datalen);
			oac_10vendor->vendor 	= htobe32(vendor);

			if (data && datalen) {
				memcpy(oac_10vendor->data, data, datalen);
			}
		} break;
		default:
			throw eBadVersion();
		}
	};

	/**
	 * constructor
	 */
	cofaction_vendor(cofaction const& action) :
		cofaction(action)
	{
		if (OFP10AT_VENDOR != action.get_type())
			throw eActionInvalType();
	};

	/** destructor
	 */
	virtual
	~cofaction_vendor() {};

	/**
	 *
	 */
	uint32_t
	get_vendor() const { return be32toh(oac_10vendor->vendor); };
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
				cofaction(ofp_version, sizeof(struct ofp12_action_mpls_ttl))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_mpls_ttl));
			oac_12mpls_ttl->type 		= htobe16(OFP12AT_SET_MPLS_TTL);
			oac_12mpls_ttl->len 		= htobe16(sizeof(struct ofp12_action_mpls_ttl));
			oac_12mpls_ttl->mpls_ttl 	= mpls_ttl;
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_mpls_ttl() {};
};


/** OFPAT_DEC_MPLS_TTL
 *
 */
class cofaction_dec_mpls_ttl : public cofaction {
public:
	/** constructor
	 */
	cofaction_dec_mpls_ttl(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct ofp12_action_header))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_header));
			oac_header->type 	= htobe16(OFP12AT_DEC_MPLS_TTL);
			oac_header->len 	= htobe16(sizeof(struct ofp12_action_header));
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_dec_mpls_ttl() {};
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
				cofaction(ofp_version, sizeof(struct ofp12_action_push))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_push));
			oac_12push->type 		= htobe16(OFP12AT_PUSH_VLAN);
			oac_12push->len 		= htobe16(sizeof(struct ofp12_action_push));
			oac_12push->ethertype 	= htobe16(ethertype);
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_push_vlan() {};
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
				cofaction(ofp_version, sizeof(struct ofp12_action_push))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_push));
			oac_12push->type 		= htobe16(OFP12AT_PUSH_MPLS);
			oac_12push->len 		= htobe16(sizeof(struct ofp12_action_push));
			oac_12push->ethertype 	= htobe16(ethertype);
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_push_mpls() {};
};


/** OFPAT_POP_VLAN
 *
 */
class cofaction_pop_vlan : public cofaction {
public:
	/** constructor
	 */
	cofaction_pop_vlan(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct ofp12_action_header))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_header));
			oac_header->type 	= htobe16(OFP12AT_POP_VLAN);
			oac_header->len 	= htobe16(sizeof(struct ofp12_action_header));
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_pop_vlan() {};
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
				cofaction(ofp_version, sizeof(struct ofp12_action_pop_mpls))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_pop_mpls));
			oac_12pop_mpls->type 		= htobe16(OFP12AT_POP_MPLS);
			oac_12pop_mpls->len 		= htobe16(sizeof(struct ofp12_action_pop_mpls));
			oac_12pop_mpls->ethertype 	= htobe16(ethertype);
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_pop_mpls() {};
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
				cofaction(ofp_version, sizeof(struct ofp12_action_group))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_group));
			oac_12group->type 		= htobe16(OFP12AT_GROUP);
			oac_12group->len 		= htobe16(sizeof(struct ofp12_action_group));
			oac_12group->group_id 	= htobe32(group_id);
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_group() {};
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
				cofaction(ofp_version, sizeof(struct ofp12_action_nw_ttl))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_nw_ttl));
			oac_12nw_ttl->type 		= htobe16(OFP12AT_SET_NW_TTL);
			oac_12nw_ttl->len 		= htobe16(sizeof(struct ofp12_action_nw_ttl));
			oac_12nw_ttl->nw_ttl 	= nw_ttl;
		} break;
		default:
			throw eBadVersion();
		}

	};

	/** destructor
	 */
	virtual
	~cofaction_set_nw_ttl() {};
};


/** OFPAT_DEC_NW_TTL
 *
 */
class cofaction_dec_nw_ttl : public cofaction {
public:
	/** constructor
	 */
	cofaction_dec_nw_ttl(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct ofp12_action_header))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_header));
			oac_header->type 	= htobe16(OFP12AT_DEC_NW_TTL);
			oac_header->len 	= htobe16(sizeof(struct ofp12_action_header));
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_dec_nw_ttl() {};
};


/** OFPAT_COPY_TTL_OUT
 *
 */
class cofaction_copy_ttl_out : public cofaction {
public:
	/** constructor
	 */
	cofaction_copy_ttl_out(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct ofp12_action_header))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_header));
			oac_header->type 	= htobe16(OFP12AT_COPY_TTL_OUT);
			oac_header->len 	= htobe16(sizeof(struct ofp12_action_header));
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_copy_ttl_out() {};
};


/** OFPAT_COPY_TTL_IN
 *
 */
class cofaction_copy_ttl_in : public cofaction {
public:
	/** constructor
	 */
	cofaction_copy_ttl_in(uint8_t ofp_version) :
				cofaction(ofp_version, sizeof(struct ofp12_action_header))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_header));
			oac_header->type	= htobe16(OFP12AT_COPY_TTL_IN);
			oac_header->len 	= htobe16(sizeof(struct ofp12_action_header));
		} break;
		default:
			throw eBadVersion();
		}

	};

	/** destructor
	 */
	virtual
	~cofaction_copy_ttl_in() {};
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
			uint8_t queue_id) :
				cofaction(ofp_version, sizeof(struct ofp12_action_set_queue))
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_set_queue));
			oac_12set_queue->type 		= htobe16(OFP12AT_SET_QUEUE);
			oac_12set_queue->len 		= htobe16(sizeof(struct ofp12_action_set_queue));
			oac_12set_queue->queue_id 	= htobe32(queue_id);
		} break;
		default:
			throw eBadVersion();
		}
	};

	/** destructor
	 */
	virtual
	~cofaction_set_queue() {};
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
		case OFP12_VERSION:
		case OFP13_VERSION: {
			size_t total_length = 2 * sizeof(uint16_t) + oxm.length();

			size_t pad = (0x7 & total_length);

			/* append padding if not a multiple of 8 */
			if (pad) {
				total_length += 8 - pad;
			}

			action.resize(total_length);

			oac_12set_field 		= (struct ofp12_action_set_field*)action.somem();
			oac_12set_field->type 	= htobe16(OFP12AT_SET_FIELD);
			oac_12set_field->len 	= htobe16(total_length);

			memcpy(oac_12set_field->field, (void*)oxm.sooxm(), oxm.length());
		} break;
		default:
			throw eBadVersion();
		}
	};
	/** destructor
	 */
	virtual
	~cofaction_set_field() {};
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
				cofaction(ofp_version, sizeof(struct ofp12_action_experimenter_header) + datalen)
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_experimenter_header) + datalen);
			oac_12experimenter->type 			= htobe16(OFP12AT_EXPERIMENTER);
			oac_12experimenter->len 			= htobe16(sizeof(struct ofp12_action_experimenter_header) + datalen);
			oac_12experimenter->experimenter 	= htobe32(exp_id);
			oac_12experimenter->type			= htobe32(exp_type);
		} break;
		default:
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
				cofaction(ofp_version, sizeof(struct ofp12_action_experimenter_header) + datalen)
	{
		switch (ofp_version) {
		case OFP12_VERSION:
		case OFP13_VERSION: {
			cofaction::resize(sizeof(struct ofp12_action_experimenter_header) + datalen);
			oac_12experimenter->type 			= htobe16(OFP12AT_EXPERIMENTER);
			oac_12experimenter->len 			= htobe16(sizeof(struct ofp12_action_experimenter_header) + datalen);
			oac_12experimenter->experimenter 	= htobe32(exp_id);
			oac_12experimenter->exp_type		= htobe32(exp_type);

			if (data && datalen) {
				memcpy(oac_12experimenter->data, data, datalen);
			}
		} break;
		default:
			throw eBadVersion();
		}
	};

	/**
	 * constructor
	 */
	cofaction_experimenter(cofaction const& action) :
		cofaction(action)
	{
		if (OFP12AT_EXPERIMENTER != action.get_type())
			throw eActionInvalType();
	};

	/** destructor
	 */
	virtual
	~cofaction_experimenter() {};

	/**
	 *
	 */
	uint32_t
	get_exp_id() const { return be32toh(oac_12experimenter->experimenter); };
};




}; // end of namespace

#endif // COFACTION_H
