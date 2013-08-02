#ifndef PPPOE_ACTIONS_H
#define PPPOE_ACTIONS_H 1

#include <rofl/common/openflow/cofaction.h>

namespace rofl
{

/** OFXAT_PUSH_VLAN
 *
 */
class cofaction_push_pppoe : public cofaction_experimenter {

public:

#define OFXAT_PUSH_PPPOE						1

	union {
		uint8_t									*oacu_action;
		struct ofp_action_header				*oacu_header;
		struct ofp_action_push 					*oacu_push;
	} eoac_oacu;

#define eoac_action eoac_oacu.oacu_action			// start of action
#define eoac_header eoac_oacu.oacu_header			// action: plain header
#define eoac_push eoac_oacu.oacu_push				// action: push


public:
	/** constructor
	 */
	cofaction_push_pppoe(
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
	struct ofx_action_pop_pppoe {
	    uint16_t type;                  /* OFPAT_POP_PPPOE. */
	    uint16_t len;                   /* Length is 8. */
	    uint16_t ethertype;             /* Ethertype */
	    uint8_t pad[2];
	};
	OFP_ASSERT(sizeof(struct ofx_action_pop_pppoe) == 8);


#define OFXAT_POP_PPPOE							2

	union {
		uint8_t									*oacu_action;
		struct ofp_action_header				*oacu_header;
		struct ofx_action_pop_pppoe 			*oacu_pop_pppoe;
	} eoac_oacu;

#define eoac_action eoac_oacu.oacu_action			// start of action
#define eoac_header eoac_oacu.oacu_header			// action: plain header
#define eoac_pop_pppoe eoac_oacu.oacu_pop_pppoe		// action: pop_pppoe


public:
	/** constructor
	 */
	cofaction_pop_pppoe(
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
