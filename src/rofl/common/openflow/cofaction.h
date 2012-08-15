/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFACTION_H
#define COFACTION_H 1

#include <vector>
#include <string>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif
#include "openflow12.h"
#include <endian.h>
#ifndef htobe16
#include "../endian_conversion.h"
#endif
#ifdef __cplusplus
}
#endif

#include "../cerror.h"
#include "../cmemory.h"
#include "../cmacaddr.h"
#include "../cvastring.h"
#include "../cerror.h"

#include "rofl/platform/unix/csyslog.h"

#include "coxmatch.h"

#ifndef ORAN
#define ORAN 1
#endif

/* error classes */
class eActionBase : public cerror {};
class eActionInval : public eActionBase {}; // invalid parameter
class eActionBadLen : public eActionBase {}; // bad length
class eActionInvalType : public eActionBase {}; // invalid action type
class eActionBadOutPort : public eActionBase {}; // bad out port specified
class eActionHeaderBadLen : public eActionBase {}; // invalid action header



class cofaction : public csyslog {

	#define COFACTION_DEFAULT_SIZE	128  // default action size: 128 bytes

public: // static methods

public: // data structures

	union { // for OpenFlow 1.1
		struct ofp_action_header				*oacu_header;
		struct ofp_action_output				*oacu_output;
		struct ofp_action_vendor_header			*oacu_vendor;

		struct ofp_action_mpls_ttl				*oacu_mpls_ttl;
		struct ofp_action_push 					*oacu_push;
		struct ofp_action_pop_mpls 				*oacu_pop_mpls;
		struct ofp_action_group 				*oacu_group;
		struct ofp_action_nw_ttl 				*oacu_nw_ttl;
		struct ofp_action_experimenter_header 	*oacu_experimenter_header;
		struct ofp_action_set_queue 			*oacu_set_queue;
		struct ofp_action_pop_pppoe 			*oacu_pop_pppoe;
		struct ofp_action_set_field 			*oacu_set_field;
	} oac_oacu;

#define oac_header oac_oacu.oacu_header			// action: plain header
#define oac_output oac_oacu.oacu_output			// action: output
#define oac_enqueue oac_oacu.oacu_enqueue		// action: enqueue
#define oac_vendor oac_oacu.oacu_vendor			// action: vendor

#define oac_mpls_ttl oac_oacu.oacu_mpls_ttl		// action: mpls_ttl
#define oac_push oac_oacu.oacu_push				// action: push
#define oac_pop_mpls oac_oacu.oacu_pop_mpls		// action: pop_mpls
#define oac_group oac_oacu.oacu_group			// action: group
#define oac_nw_ttl oac_oacu.oacu_nw_ttl			// action: nw_ttl
#define oac_experimenter_header oac_oacu.oacu_experimenter_header	// action: experimenter_header
#define oac_header oac_oacu.oacu_header			// action: header
#define oac_set_queue oac_oacu.oacu_set_queue	// action: set_queue

#define oac_push_pppoe oac_oacu.oacu_push		// action: push pppoe
#define oac_pop_pppoe oac_oacu.oacu_pop_pppoe	// action: pop_pppoe
#define oac_push_ppp oac_oacu.oacu_header		// action: push ppp (with plain header)
#define oac_pop_ppp oac_oacu.oacu_header		// action: pop ppp (with plain header)
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
		size_t aclen) throw (eActionBadLen, eActionBadOutPort);

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
	soaction();

	/** return length of action in bytes
	 */
	size_t
	length();

	/** dump info string for this action
	 */
	const char*
	c_str();

	/** copy struct ofp_action_header
	 */
	struct ofp_action_header*
	pack(
		struct ofp_action_header* achdr,
		size_t aclen) throw (eActionInval);

	/** unpack
	 */
	void
	unpack(
		struct ofp_action_header *achdr,
		size_t aclen)
		throw (eActionBadLen, eActionBadOutPort);

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

	/**
	 *
	 */
	uint16_t
	get_max_len() throw (eActionInvalType);

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
};


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


/** OFPAT_PUSH_VLAN
 *
 */
class cofaction_push_pppoe : public cofaction {
public:
	/** constructor
	 */
	cofaction_push_pppoe(
			uint16_t ethertype) :
				cofaction(sizeof(struct ofp_action_push))
	{
		oac_push->type = htobe16(OFPAT_PUSH_PPPOE);
		oac_push->len = htobe16(sizeof(struct ofp_action_push));
		oac_push->ethertype = htobe16(ethertype);
	};

	/** destructor
	 */
	virtual
	~cofaction_push_pppoe() {};
};


/** OFPAT_POP_PPPOE
 *
 */
class cofaction_pop_pppoe : public cofaction {
public:
	/** constructor
	 */
	cofaction_pop_pppoe(
			uint16_t ethertype) :
				cofaction(sizeof(struct ofp_action_pop_pppoe))
	{
		oac_pop_pppoe->type = htobe16(OFPAT_POP_PPPOE);
		oac_pop_pppoe->len = htobe16(sizeof(struct ofp_action_pop_pppoe));
		oac_pop_pppoe->ethertype = htobe16(ethertype);
	};

	/** destructor
	 */
	virtual
	~cofaction_pop_pppoe() {};
};




/** OFPAT_PUSH_PPP
 *
 */
class cofaction_push_ppp : public cofaction {
public:
	/** constructor
	 */
	cofaction_push_ppp() :
				cofaction(sizeof(struct ofp_action_header))
	{
		oac_header->type = htobe16(OFPAT_PUSH_PPP);
		oac_header->len = htobe16(sizeof(struct ofp_action_header));
	};

	/** destructor
	 */
	virtual
	~cofaction_push_ppp() {};
};


/** OFPAT_POP_PPP
 *
 */
class cofaction_pop_ppp : public cofaction {
public:
	/** constructor
	 */
	cofaction_pop_ppp() :
				cofaction(sizeof(struct ofp_action_header))
	{
		oac_header->type = htobe16(OFPAT_POP_PPP);
		oac_header->len = htobe16(sizeof(struct ofp_action_header));
	};

	/** destructor
	 */
	virtual
	~cofaction_pop_ppp() {};
};



#endif // COFACTION_H
