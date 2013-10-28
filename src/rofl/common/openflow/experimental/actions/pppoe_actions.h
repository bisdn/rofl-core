#ifndef PPPOE_ACTIONS_H
#define PPPOE_ACTIONS_H 

#include "../../cofaction.h"

namespace rofl
{

/** OFXAT_PUSH_PPPOE
 *
 */
class cofaction_push_pppoe : public cofaction_experimenter {

public:

	/* Action structure for OFPAT_POP_PPPOE. */
	struct ofx_action_push_pppoe_header {
	    uint16_t type;                  /* OFPAT_POP_PPPOE. */
	    uint16_t len;                   /* Length is 8. */
	    uint16_t ethertype;             /* Ethertype */
	    uint8_t pad[2];
	};
	OFP_ASSERT(sizeof(struct ofx_action_push_pppoe_header) == 8);

	struct ofx_action_push_pppoe {
		struct ofp12_experimenter_header 	exphdr;
		struct ofx_action_push_pppoe_header	expbody;
	};

	union {
		uint8_t									*oacu_action;
		struct ofp_action_header				*oacu_header;
		struct ofx_action_push_pppoe 			*oacu_push_pppoe;
	} eoac_oacu;

#define eoac_action 	eoac_oacu.oacu_action		// start of action
#define eoac_header 	eoac_oacu.oacu_header		// action: plain header
#define eoac_push_pppoe eoac_oacu.oacu_push_pppoe	// action: push


public:
	/** constructor
	 */
	cofaction_push_pppoe(
			uint8_t ofp_version,
			uint16_t ethertype);

	/**
	 * constructor
	 */
	cofaction_push_pppoe(
			cofaction const& action);

	/** destructor
	 */
	virtual
	~cofaction_push_pppoe();

	/**
	 *
	 */
	uint16_t
	get_ethertype() const;
};


/** OFPAT_POP_PPPOE
 *
 */
class cofaction_pop_pppoe : public cofaction_experimenter {
public:

	/* Action structure for OFPAT_POP_PPPOE. */
	struct ofx_action_pop_pppoe_header {
	    uint16_t type;                  /* OFPAT_POP_PPPOE. */
	    uint16_t len;                   /* Length is 8. */
	    uint16_t ethertype;             /* Ethertype */
	    uint8_t pad[2];
	};
	OFP_ASSERT(sizeof(struct ofx_action_pop_pppoe_header) == 8);

	struct ofx_action_pop_pppoe {
		struct ofp12_experimenter_header 	exphdr;
		struct ofx_action_pop_pppoe_header 	expbody;
	};

	union {
		uint8_t									*oacu_action;
		struct ofp_action_header				*oacu_header;
		struct ofx_action_pop_pppoe 			*oacu_pop_pppoe;
	} eoac_oacu;

#define eoac_action 	eoac_oacu.oacu_action		// start of action
#define eoac_header 	eoac_oacu.oacu_header		// action: plain header
#define eoac_pop_pppoe eoac_oacu.oacu_pop_pppoe		// action: pop_pppoe


public:
	/** constructor
	 */
	cofaction_pop_pppoe(
			uint8_t ofp_version,
			uint16_t ethertype);

	/**
	 * constructor
	 */
	cofaction_pop_pppoe(
			cofaction const& action);

	/** destructor
	 */
	virtual
	~cofaction_pop_pppoe();

	/**
	 *
	 */
	uint16_t
	get_ethertype() const;
};

}; // end of namespace

#endif
