/*
 * wlan_actions.h
 *
 *  Created on: 29.06.2014
 *      Author: andreas
 */

#ifndef WLAN_ACTIONS_H_
#define WLAN_ACTIONS_H_

#include <inttypes.h>
#include <iostream>

#include <rofl/common/logging.h>
#include <rofl/common/cmemory.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace wlan {

enum wlan_exp_id_t {
	WLAN_EXP_ID = 0x5555a783,
};

enum wlan_action_type_t {
	WLAN_ACTION_PUSH_WLAN,
	WLAN_ACTION_POP_WLAN,
};

struct ofp_exp_wlan_action_body_hdr {
	uint16_t exptype;
	uint16_t explen;
	uint8_t data[0];
} __attribute__((packed));

struct ofp_exp_wlan_action_body_push_wlan {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));

struct ofp_exp_wlan_action_body_pop_wlan {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));



class cofaction_exp_body_wlan : public rofl::cmemory {
public:

	/**
	 *
	 */
	cofaction_exp_body_wlan(
			uint16_t exptype = 0,
			size_t bodylen = 0) :
				rofl::cmemory(bodylen) {
		set_exp_type(exptype);
		set_exp_len(bodylen);
	};

	/**
	 *
	 */
	~cofaction_exp_body_wlan() {};

	/**
	 *
	 */
	cofaction_exp_body_wlan(
			const cofaction_exp_body_wlan& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_wlan&
	operator= (
			const cofaction_exp_body_wlan& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_wlan(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_wlan&
	operator= (
			const rofl::cmemory& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

public:

	/**
	 *
	 */
	uint16_t
	get_exp_type() const {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_wlan_action_body_hdr))
			throw eInval();
		return be16toh(((struct ofp_exp_wlan_action_body_hdr*)rofl::cmemory::somem())->exptype);
	};

	/**
	 *
	 */
	void
	set_exp_type(uint16_t exptype) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_wlan_action_body_hdr))
			throw eInval();
		((struct ofp_exp_wlan_action_body_hdr*)rofl::cmemory::somem())->exptype = htobe16(exptype);
	};

	/**
	 *
	 */
	uint16_t
	get_exp_len() const {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_wlan_action_body_hdr))
			throw eInval();
		return be16toh(((struct ofp_exp_wlan_action_body_hdr*)rofl::cmemory::somem())->explen);
	};

	/**
	 *
	 */
	void
	set_exp_len(uint16_t explen) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_wlan_action_body_hdr))
			throw eInval();
		((struct ofp_exp_wlan_action_body_hdr*)rofl::cmemory::somem())->explen = htobe16(explen);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_wlan& body) {
		os << rofl::indent(0) << "<cofaction_exp_body_wlan exp-type: 0x" <<
				std::hex << (unsigned int)body.get_exp_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( body );
		return os;
	};
};



class cofaction_exp_body_push_wlan : public cofaction_exp_body_wlan {
public:

	/**
	 *
	 */
	cofaction_exp_body_push_wlan(
			uint16_t ethertype = 0) :
				cofaction_exp_body_wlan(
						WLAN_ACTION_PUSH_WLAN,
						sizeof(struct ofp_exp_wlan_action_body_push_wlan)) {
		set_ether_type(ethertype);
	};

	/**
	 *
	 */
	~cofaction_exp_body_push_wlan() {};

	/**
	 *
	 */
	cofaction_exp_body_push_wlan(
			const cofaction_exp_body_push_wlan& action) { *this = action; };

	/**
	 *
	 */
	cofaction_exp_body_push_wlan&
	operator= (
			const cofaction_exp_body_push_wlan& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_push_wlan(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_push_wlan&
	operator= (
			const rofl::cmemory& action) {
		if (this == &action)
			return *this;
		rofl::cmemory::operator= (action);
		return *this;
	};

public:

	/**
	 *
	 */
	uint16_t
	get_ether_type() const {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_wlan_action_body_push_wlan))
			throw eInval();
		return be16toh(((struct ofp_exp_wlan_action_body_push_wlan*)rofl::cmemory::somem())->ethertype);
	};

	/**
	 *
	 */
	void
	set_ether_type(uint16_t ethertype) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_wlan_action_body_push_wlan))
			throw eInval();
		((struct ofp_exp_wlan_action_body_push_wlan*)rofl::cmemory::somem())->ethertype = htobe16(ethertype);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_push_wlan& action) {
		os << rofl::indent(0) << "<cofaction_push_wlan ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( action );
		return os;
	};

};




class cofaction_exp_body_pop_wlan : public cofaction_exp_body_wlan {
public:

	/**
	 *
	 */
	cofaction_exp_body_pop_wlan(
			uint16_t ethertype = 0) :
				cofaction_exp_body_wlan(
						WLAN_ACTION_POP_WLAN,
						sizeof(struct ofp_exp_wlan_action_body_pop_wlan)) {
		set_ether_type(ethertype);
	};

	/**
	 *
	 */
	~cofaction_exp_body_pop_wlan() {};

	/**
	 *
	 */
	cofaction_exp_body_pop_wlan(
			const cofaction_exp_body_pop_wlan& action) { *this = action; };

	/**
	 *
	 */
	cofaction_exp_body_pop_wlan&
	operator= (
			const cofaction_exp_body_pop_wlan& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_pop_wlan(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_pop_wlan&
	operator= (
			const rofl::cmemory& action) {
		if (this == &action)
			return *this;
		rofl::cmemory::operator= (action);
		return *this;
	};

public:

	/**
	 *
	 */
	uint16_t
	get_ether_type() const {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_wlan_action_body_pop_wlan))
			throw eInval();
		return be16toh(((struct ofp_exp_wlan_action_body_pop_wlan*)rofl::cmemory::somem())->ethertype);
	};

	/**
	 *
	 */
	void
	set_ether_type(uint16_t ethertype) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_wlan_action_body_pop_wlan))
			throw eInval();
		((struct ofp_exp_wlan_action_body_pop_wlan*)rofl::cmemory::somem())->ethertype = htobe16(ethertype);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_pop_wlan& action) {
		os << rofl::indent(0) << "<cofaction_pop_wlan ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( action );
		return os;
	};

};


}; // end of namespace wlan
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* WLAN_ACTIONS_H_ */
