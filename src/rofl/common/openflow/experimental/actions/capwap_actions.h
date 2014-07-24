/*
 * capwap_actions.h
 *
 *  Created on: 29.06.2014
 *      Author: andreas
 */

#ifndef CAPWAP_ACTIONS_H_
#define CAPWAP_ACTIONS_H_

#include <inttypes.h>
#include <iostream>

#include <rofl/common/logging.h>
#include <rofl/common/cmemory.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace capwap {

enum capwap_exp_id_t {
	CAPWAP_EXP_ID = 0x5555a782,
};

enum capwap_action_type_t {
	CAPWAP_ACTION_PUSH_CAPWAP,
	CAPWAP_ACTION_POP_CAPWAP,
};

struct ofp_exp_capwap_action_body_hdr {
	uint16_t exptype;
	uint16_t explen;
	uint8_t data[0];
} __attribute__((packed));

struct ofp_exp_capwap_action_body_push_capwap {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));

struct ofp_exp_capwap_action_body_pop_capwap {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));



class cofaction_exp_body_capwap : public rofl::cmemory {
public:

	/**
	 *
	 */
	cofaction_exp_body_capwap(
			uint16_t exptype = 0,
			size_t bodylen = 0) :
				rofl::cmemory(bodylen) {
		set_exp_type(exptype);
		set_exp_len(bodylen);
	};

	/**
	 *
	 */
	~cofaction_exp_body_capwap() {};

	/**
	 *
	 */
	cofaction_exp_body_capwap(
			const cofaction_exp_body_capwap& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_capwap&
	operator= (
			const cofaction_exp_body_capwap& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_capwap(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_capwap&
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
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_capwap_action_body_hdr))
			throw eInval();
		return be16toh(((struct ofp_exp_capwap_action_body_hdr*)rofl::cmemory::somem())->exptype);
	};

	/**
	 *
	 */
	void
	set_exp_type(uint16_t exptype) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_capwap_action_body_hdr))
			throw eInval();
		((struct ofp_exp_capwap_action_body_hdr*)rofl::cmemory::somem())->exptype = htobe16(exptype);
	};

	/**
	 *
	 */
	uint16_t
	get_exp_len() const {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_capwap_action_body_hdr))
			throw eInval();
		return be16toh(((struct ofp_exp_capwap_action_body_hdr*)rofl::cmemory::somem())->explen);
	};

	/**
	 *
	 */
	void
	set_exp_len(uint16_t explen) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_capwap_action_body_hdr))
			throw eInval();
		((struct ofp_exp_capwap_action_body_hdr*)rofl::cmemory::somem())->explen = htobe16(explen);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_capwap& body) {
		os << rofl::indent(0) << "<cofaction_exp_body_capwap exp-type: 0x" <<
				std::hex << (unsigned int)body.get_exp_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( body );
		return os;
	};
};



class cofaction_exp_body_push_capwap : public cofaction_exp_body_capwap {
public:

	/**
	 *
	 */
	cofaction_exp_body_push_capwap(
			uint16_t ethertype = 0) :
				cofaction_exp_body_capwap(
						CAPWAP_ACTION_PUSH_CAPWAP,
						sizeof(struct ofp_exp_capwap_action_body_push_capwap)) {
		set_ether_type(ethertype);
	};

	/**
	 *
	 */
	~cofaction_exp_body_push_capwap() {};

	/**
	 *
	 */
	cofaction_exp_body_push_capwap(
			const cofaction_exp_body_push_capwap& action) { *this = action; };

	/**
	 *
	 */
	cofaction_exp_body_push_capwap&
	operator= (
			const cofaction_exp_body_push_capwap& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_push_capwap(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_push_capwap&
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
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_capwap_action_body_push_capwap))
			throw eInval();
		return be16toh(((struct ofp_exp_capwap_action_body_push_capwap*)rofl::cmemory::somem())->ethertype);
	};

	/**
	 *
	 */
	void
	set_ether_type(uint16_t ethertype) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_capwap_action_body_push_capwap))
			throw eInval();
		((struct ofp_exp_capwap_action_body_push_capwap*)rofl::cmemory::somem())->ethertype = htobe16(ethertype);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_push_capwap& action) {
		os << rofl::indent(0) << "<cofaction_push_capwap ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( action );
		return os;
	};

};




class cofaction_exp_body_pop_capwap : public cofaction_exp_body_capwap {
public:

	/**
	 *
	 */
	cofaction_exp_body_pop_capwap(
			uint16_t ethertype = 0) :
				cofaction_exp_body_capwap(
						CAPWAP_ACTION_POP_CAPWAP,
						sizeof(struct ofp_exp_capwap_action_body_pop_capwap)) {
		set_ether_type(ethertype);
	};

	/**
	 *
	 */
	~cofaction_exp_body_pop_capwap() {};

	/**
	 *
	 */
	cofaction_exp_body_pop_capwap(
			const cofaction_exp_body_pop_capwap& action) { *this = action; };

	/**
	 *
	 */
	cofaction_exp_body_pop_capwap&
	operator= (
			const cofaction_exp_body_pop_capwap& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_pop_capwap(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_pop_capwap&
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
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_capwap_action_body_pop_capwap))
			throw eInval();
		return be16toh(((struct ofp_exp_capwap_action_body_pop_capwap*)rofl::cmemory::somem())->ethertype);
	};

	/**
	 *
	 */
	void
	set_ether_type(uint16_t ethertype) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_capwap_action_body_pop_capwap))
			throw eInval();
		((struct ofp_exp_capwap_action_body_pop_capwap*)rofl::cmemory::somem())->ethertype = htobe16(ethertype);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_pop_capwap& action) {
		os << rofl::indent(0) << "<cofaction_pop_capwap ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( action );
		return os;
	};

};


}; // end of namespace capwap
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* CAPWAP_ACTIONS_H_ */
