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
		struct ofp_action_output				*oacu_output;
		// OF1.0 actions
		struct ofp10_action_enqueue				*oacu_10enqueue;
		struct ofp10_action_vlan_vid			*oacu_10vlanvid;
		struct ofp10_action_vlan_pcp			*oacu_10vlanpcp;
		struct ofp10_action_dl_addr				*oacu_10dladdr;
		struct ofp10_action_nw_addr				*oacu_10nwaddr;
		struct ofp10_action_nw_tos				*oacu_10nwtos;
		struct ofp10_action_tp_port				*oacu_10tpport;
		struct ofp10_action_vendor_header		*oacu_10vendor;
		// OF1.0 actions -done-

		struct ofp_action_mpls_ttl				*oacu_mpls_ttl;
		struct ofp_action_push 					*oacu_push;
		struct ofp_action_pop_mpls 				*oacu_pop_mpls;
		struct ofp_action_group 				*oacu_group;
		struct ofp_action_nw_ttl 				*oacu_nw_ttl;
		struct ofp_action_experimenter_header 	*oacu_experimenter_header;
		struct ofp_action_set_queue 			*oacu_set_queue;
		struct ofp_action_set_field 			*oacu_set_field;
	} oac_oacu;

#define oac_header oac_oacu.oacu_header			// action: plain header
#define oac_output oac_oacu.oacu_output			// action: output
#define oac_enqueue oac_oacu.oacu_10enqueue		// action: enqueue
#define oac_vlanvid oac_oacu.oacu_10vlanvid		// action: vlan_vid
#define oac_vlanpcp oac_oacu.oacu_10vlanpcp		// action: vlan_pcp
#define oac_dladdr oac_oacu.oacu_10dladdr		// action: dl_addr
#define oac_nwaddr oac_oacu.oacu_10nwaddr		// action: nw_addr
#define oac_nwtos oac_oacu.oacu_10nwtos			// action: nw_tos
#define oac_tpport oac_oacu.oacu_10tpport		// action: tp_port
#define oac_vendor oac_oacu.oacu_10vendor		// action: vendor

#define oac_mpls_ttl oac_oacu.oacu_mpls_ttl		// action: mpls_ttl
#define oac_push oac_oacu.oacu_push				// action: push
#define oac_pop_mpls oac_oacu.oacu_pop_mpls		// action: pop_mpls
#define oac_group oac_oacu.oacu_group			// action: group
#define oac_nw_ttl oac_oacu.oacu_nw_ttl			// action: nw_ttl
#define oac_experimenter_header oac_oacu.oacu_experimenter_header	// action: experimenter_header
#define oac_header oac_oacu.oacu_header			// action: header
#define oac_set_queue oac_oacu.oacu_set_queue	// action: set_queue

#define oac_set_field oac_oacu.oacu_set_field	// action: set field

protected: // data structures

	cmemory action;
	std::string info;

public: // methods

	/** constructor
	 */
	cofaction(size_t datalen = COFACTION_DEFAULT_SIZE);

	/** constructor
	 */
	cofaction(
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

	/**
	 *
	 */
	uint32_t
	get_port() throw (eActionInvalType);

	/** return oxm for OFPAT_SET_FIELD
	 *
	 */
	coxmatch
	get_oxm() throw (eActionInvalType);

protected: // methods

	/** create info string
	 */
	void
	__make_info();


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
			uint32_t port,
			uint16_t max_len = 128) :
				cofaction(sizeof(struct ofp_action_output))
	{
		oac_output->type = htobe16(OFPAT_OUTPUT);
		oac_output->len = htobe16(sizeof(struct ofp_action_output));
		oac_output->port = htobe32(port);
		oac_output->max_len = htobe16(max_len);
	};

	/** destructor
	 */
	virtual
	~cofaction_output() {};

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
			uint16_t vlan_vid) :
				cofaction(sizeof(struct ofp10_action_vlan_vid))
	{
		oac_vlanvid->type = htobe16(OFP10AT_SET_VLAN_VID);
		oac_vlanvid->len = htobe16(sizeof(struct ofp10_action_vlan_vid));
		oac_vlanvid->vlan_vid = vlan_vid;
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
			uint8_t vlan_pcp) :
				cofaction(sizeof(struct ofp10_action_vlan_pcp))
	{
		oac_vlanpcp->type = htobe16(OFP10AT_SET_VLAN_PCP);
		oac_vlanpcp->len = htobe16(sizeof(struct ofp10_action_vlan_vid));
		oac_vlanpcp->vlan_pcp = vlan_pcp;
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
	cofaction_strip_vlan() :
				cofaction(sizeof(struct ofp_action_header))
	{
		oac_header->type = htobe16(OFP10AT_STRIP_VLAN);
		oac_header->len = htobe16(sizeof(struct ofp_action_header));
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
			rofl::cmacaddr const& maddr) :
				cofaction(sizeof(struct ofp10_action_dl_addr))
	{
		oac_dladdr->type = htobe16(OFP10AT_SET_DL_SRC);
		oac_dladdr->len = htobe16(sizeof(struct ofp10_action_dl_addr));
		memcpy(oac_dladdr->dl_addr, maddr.somem(), OFP_ETH_ALEN);
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
			rofl::cmacaddr const& maddr) :
				cofaction(sizeof(struct ofp10_action_dl_addr))
	{
		oac_dladdr->type = htobe16(OFP10AT_SET_DL_DST);
		oac_dladdr->len = htobe16(sizeof(struct ofp10_action_dl_addr));
		memcpy(oac_dladdr->dl_addr, maddr.somem(), OFP_ETH_ALEN);
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
			rofl::caddress const& addr) :
				cofaction(sizeof(struct ofp10_action_nw_addr))
	{
		oac_nwaddr->type = htobe16(OFP10AT_SET_NW_SRC);
		oac_nwaddr->len = htobe16(sizeof(struct ofp10_action_nw_addr));
		oac_nwaddr->nw_addr = addr.ca_s4addr->sin_addr.s_addr;
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
			rofl::caddress const& addr) :
				cofaction(sizeof(struct ofp10_action_nw_addr))
	{
		oac_nwaddr->type = htobe16(OFP10AT_SET_NW_DST);
		oac_nwaddr->len = htobe16(sizeof(struct ofp10_action_nw_addr));
		oac_nwaddr->nw_addr = addr.ca_s4addr->sin_addr.s_addr;
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
			uint8_t nw_tos) :
				cofaction(sizeof(struct ofp10_action_nw_tos))
	{
		oac_nwtos->type = htobe16(OFP10AT_SET_NW_TOS);
		oac_nwtos->len = htobe16(sizeof(struct ofp10_action_nw_tos));
		oac_nwtos->nw_tos = nw_tos;
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
			uint16_t tp_src) :
				cofaction(sizeof(struct ofp10_action_tp_port))
	{
		oac_tpport->type = htobe16(OFP10AT_SET_TP_SRC);
		oac_tpport->len = htobe16(sizeof(struct ofp10_action_tp_port));
		oac_tpport->tp_port = tp_src;
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
			uint16_t tp_dst) :
				cofaction(sizeof(struct ofp10_action_tp_port))
	{
		oac_tpport->type = htobe16(OFP10AT_SET_TP_DST);
		oac_tpport->len = htobe16(sizeof(struct ofp10_action_tp_port));
		oac_tpport->tp_port = tp_dst;
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
			uint16_t port, uint32_t queue_id) :
				cofaction(sizeof(struct ofp10_action_enqueue))
	{
		oac_enqueue->type = htobe16(OFP10AT_ENQUEUE);
		oac_enqueue->len = htobe16(sizeof(struct ofp10_action_tp_port));
		oac_enqueue->port = htobe16(port);
		oac_enqueue->queue_id = htobe32(queue_id);
	};

	/** destructor
	 */
	virtual
	~cofaction_enqueue() {};
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
			uint8_t mpls_ttl) :
				cofaction(sizeof(struct ofp_action_mpls_ttl))
	{
		oac_mpls_ttl->type = htobe16(OFPAT_SET_MPLS_TTL);
		oac_mpls_ttl->len = htobe16(sizeof(struct ofp_action_mpls_ttl));
		oac_mpls_ttl->mpls_ttl = mpls_ttl;
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
	cofaction_dec_mpls_ttl() :
				cofaction(sizeof(struct ofp_action_header))
	{
		oac_header->type = htobe16(OFPAT_DEC_MPLS_TTL);
		oac_header->len = htobe16(sizeof(struct ofp_action_mpls_ttl));
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
			uint16_t ethertype) :
				cofaction(sizeof(struct ofp_action_push))
	{
		oac_push->type = htobe16(OFPAT_PUSH_VLAN);
		oac_push->len = htobe16(sizeof(struct ofp_action_push));
		oac_push->ethertype = htobe16(ethertype);
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
			uint16_t ethertype) :
				cofaction(sizeof(struct ofp_action_push))
	{
		oac_push->type = htobe16(OFPAT_PUSH_MPLS);
		oac_push->len = htobe16(sizeof(struct ofp_action_push));
		oac_push->ethertype = htobe16(ethertype);
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
	cofaction_pop_vlan() :
				cofaction(sizeof(struct ofp_action_header))
	{
		oac_header->type = htobe16(OFPAT_POP_VLAN);
		oac_header->len = htobe16(sizeof(struct ofp_action_header));
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
			uint16_t ethertype) :
				cofaction(sizeof(struct ofp_action_pop_mpls))
	{
		oac_pop_mpls->type = htobe16(OFPAT_POP_MPLS);
		oac_pop_mpls->len = htobe16(sizeof(struct ofp_action_pop_mpls));
		oac_pop_mpls->ethertype = htobe16(ethertype);
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
			uint32_t group_id) :
				cofaction(sizeof(struct ofp_action_group))
	{
		oac_group->type = htobe16(OFPAT_GROUP);
		oac_group->len = htobe16(sizeof(struct ofp_action_group));
		oac_group->group_id = htobe32(group_id);
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
			uint8_t nw_ttl) :
				cofaction(sizeof(struct ofp_action_nw_ttl))
	{
		oac_nw_ttl->type = htobe16(OFPAT_SET_NW_TTL);
		oac_nw_ttl->len = htobe16(sizeof(struct ofp_action_nw_ttl));
		oac_nw_ttl->nw_ttl = nw_ttl;
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
	cofaction_dec_nw_ttl() :
				cofaction(sizeof(struct ofp_action_header))
	{
		oac_header->type = htobe16(OFPAT_DEC_NW_TTL);
		oac_header->len = htobe16(sizeof(struct ofp_action_header));
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
	cofaction_copy_ttl_out() :
				cofaction(sizeof(struct ofp_action_header))
	{
		oac_header->type = htobe16(OFPAT_COPY_TTL_OUT);
		oac_header->len = htobe16(sizeof(struct ofp_action_header));
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
	cofaction_copy_ttl_in() :
				cofaction(sizeof(struct ofp_action_header))
	{
		oac_header->type = htobe16(OFPAT_COPY_TTL_IN);
		oac_header->len = htobe16(sizeof(struct ofp_action_header));
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
			uint8_t queue_id) :
				cofaction(sizeof(struct ofp_action_set_queue))
	{
		oac_set_queue->type = htobe16(OFPAT_SET_QUEUE);
		oac_set_queue->len = htobe16(sizeof(struct ofp_action_nw_ttl));
		oac_set_queue->queue_id = queue_id;
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
			coxmatch const& oxm) :
				cofaction(0)
	{
		size_t total_length = 2 * sizeof(uint16_t) + oxm.length();

		size_t pad = (0x7 & total_length);

		/* append padding if not a multiple of 8 */
		if (pad) {
			total_length += 8 - pad;
		}

		action.resize(total_length);

		oac_set_field = (struct ofp_action_set_field*)action.somem();
		oac_set_field->type = htobe16(OFPAT_SET_FIELD);
		oac_set_field->len = htobe16(total_length);

		memcpy(oac_set_field->field, (void*)oxm.sooxm(), oxm.length());
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
			uint32_t exp_id,
			size_t datalen = COFACTION_DEFAULT_SIZE) :
				cofaction(sizeof(struct ofp_action_experimenter_header) + datalen)
	{
		oac_experimenter_header->type = htobe16(OFPAT_EXPERIMENTER);
		oac_experimenter_header->len = htobe16(datalen);
		oac_experimenter_header->experimenter = exp_id;
	};

	/**
	 * constructor
	 */
	cofaction_experimenter(cofaction const& action) :
		cofaction(action)
	{
		if (OFPAT_EXPERIMENTER != action.get_type())
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
	get_exp_id() const { return oac_experimenter_header->experimenter; };
};




}; // end of namespace

#endif // COFACTION_H
