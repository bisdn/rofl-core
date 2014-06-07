#ifndef PPPOE_ACTIONS_H
#define PPPOE_ACTIONS_H 1

#include "rofl/common/openflow/cofaction.h"

namespace rofl {
namespace openflow {
namespace experimental {
namespace pppoe {

#define ROFL_EXPERIMENTER_ID 0x5571aa88 // random number


class cofaction_push_pppoe : public cofaction_experimenter {
public:

	/* Action structure for OFPAT_PUSH_PPPOE. */
	struct ofx_action_push {
	    uint16_t type;                  /* OFPAT_POP_PPPOE. */
	    uint16_t len;                   /* Length is 8. */
	    uint16_t ethertype;             /* Ethertype */
	    uint8_t pad[2];
	};
	OFP_ASSERT(sizeof(struct ofx_action_push) == 8);

	enum cofaction_push_pppoe_action_type {
		OFXAT_PUSH_PPPOE = 1,
	};

public:

	/**
	 *
	 */
	cofaction_push_pppoe(
			uint8_t ofp_version = 0,
			uint16_t eth_type = 0) :
				cofaction_experimenter(
						ofp_version, ROFL_EXPERIMENTER_ID,
						rofl::cmemory(sizeof(struct ofx_action_push))),
						eth_type(eth_type) {};

	/**
	 *
	 */
	virtual
	~cofaction_push_pppoe();

	/**
	 *
	 */
	cofaction_push_pppoe(
			const cofaction_push_pppoe& action) { *this = action; };

	/**
	 *
	 */
	cofaction_push_pppoe&
	operator= (
			const cofaction_push_pppoe& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter::operator= (action);
		eth_type = action.eth_type;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_eth_type(uint16_t eth_type) { this->eth_type = eth_type; };

	/**
	 *
	 */
	uint16_t
	get_eth_type() const { return eth_type; };

public:

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

private:

	uint16_t eth_type;
};



class cofaction_pop_pppoe : public cofaction_experimenter {
public:

	/* Action structure for OFPAT_PUSH_PPPOE. */
	struct ofx_action_pop {
	    uint16_t type;                  /* OFPAT_POP_PPPOE. */
	    uint16_t len;                   /* Length is 8. */
	    uint16_t ethertype;             /* Ethertype */
	    uint8_t pad[2];
	};
	OFP_ASSERT(sizeof(struct ofx_action_pop) == 8);

	enum cofaction_pop_pppoe_action_type {
		OFXAT_PUSH_PPPOE = 1,
	};

public:

	/**
	 *
	 */
	cofaction_pop_pppoe(
			uint8_t ofp_version = 0,
			uint16_t eth_type = 0) :
				cofaction_experimenter(
						ofp_version, ROFL_EXPERIMENTER_ID,
						rofl::cmemory(sizeof(struct ofx_action_pop))),
						eth_type(eth_type) {};

	/**
	 *
	 */
	virtual
	~cofaction_pop_pppoe();

	/**
	 *
	 */
	cofaction_pop_pppoe(
			const cofaction_pop_pppoe& action) { *this = action; };

	/**
	 *
	 */
	cofaction_pop_pppoe&
	operator= (
			const cofaction_pop_pppoe& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter::operator= (action);
		eth_type = action.eth_type;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	set_eth_type(uint16_t eth_type) { this->eth_type = eth_type; };

	/**
	 *
	 */
	uint16_t
	get_eth_type() const { return eth_type; };

public:

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

private:

	uint16_t eth_type;
};



}; // end of namespace pppoe
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
