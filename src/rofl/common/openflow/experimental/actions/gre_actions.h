/*
 * gre_actions.h
 *
 *  Created on: 23.09.2014
 *      Author: andreas
 */

#ifndef GRE_ACTIONS_H_
#define GRE_ACTIONS_H_

#include <inttypes.h>
#include <iostream>

#include <rofl/common/logging.h>
#include <rofl/common/cmemory.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace gre {

enum gre_exp_id_t {
	GRE_EXP_ID = 0x5555a784,
};

enum gre_action_type_t {
	GRE_ACTION_PUSH_GRE,
	GRE_ACTION_POP_GRE,
};

struct ofp_exp_gre_action_body_hdr {
	uint16_t exptype;
	uint16_t explen;
	uint8_t data[0];
} __attribute__((packed));

struct ofp_exp_gre_action_body_push_gre {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));

struct ofp_exp_gre_action_body_pop_gre {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));



class cofaction_exp_body_gre : public rofl::cmemory {
public:

	/**
	 *
	 */
	cofaction_exp_body_gre(
			uint16_t exptype = 0,
			size_t bodylen = sizeof(struct ofp_exp_gre_action_body_hdr)) :
				rofl::cmemory(bodylen) {
		set_exp_type(exptype);
		set_exp_len(bodylen);
	};

	/**
	 *
	 */
	~cofaction_exp_body_gre() {};

	/**
	 *
	 */
	cofaction_exp_body_gre(
			const cofaction_exp_body_gre& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_gre&
	operator= (
			const cofaction_exp_body_gre& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_gre(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_gre&
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
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gre_action_body_hdr))
			throw eInval();
		return be16toh(((struct ofp_exp_gre_action_body_hdr*)rofl::cmemory::somem())->exptype);
	};

	/**
	 *
	 */
	void
	set_exp_type(uint16_t exptype) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gre_action_body_hdr))
			throw eInval();
		((struct ofp_exp_gre_action_body_hdr*)rofl::cmemory::somem())->exptype = htobe16(exptype);
	};

	/**
	 *
	 */
	uint16_t
	get_exp_len() const {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gre_action_body_hdr))
			throw eInval();
		return be16toh(((struct ofp_exp_gre_action_body_hdr*)rofl::cmemory::somem())->explen);
	};

	/**
	 *
	 */
	void
	set_exp_len(uint16_t explen) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gre_action_body_hdr))
			throw eInval();
		((struct ofp_exp_gre_action_body_hdr*)rofl::cmemory::somem())->explen = htobe16(explen);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_gre& body) {
		os << rofl::indent(0) << "<cofaction_exp_body_gre exp-type: 0x" <<
				std::hex << (unsigned int)body.get_exp_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( body );
		return os;
	};
};



class cofaction_exp_body_push_gre : public cofaction_exp_body_gre {
public:

	/**
	 *
	 */
	cofaction_exp_body_push_gre(
			uint16_t ethertype = 0) :
				cofaction_exp_body_gre(
						GRE_ACTION_PUSH_GRE,
						sizeof(struct ofp_exp_gre_action_body_push_gre)) {
		set_ether_type(ethertype);
	};

	/**
	 *
	 */
	~cofaction_exp_body_push_gre() {};

	/**
	 *
	 */
	cofaction_exp_body_push_gre(
			const cofaction_exp_body_push_gre& action) { *this = action; };

	/**
	 *
	 */
	cofaction_exp_body_push_gre&
	operator= (
			const cofaction_exp_body_push_gre& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_push_gre(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_push_gre&
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
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gre_action_body_push_gre))
			throw eInval();
		return be16toh(((struct ofp_exp_gre_action_body_push_gre*)rofl::cmemory::somem())->ethertype);
	};

	/**
	 *
	 */
	void
	set_ether_type(uint16_t ethertype) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gre_action_body_push_gre))
			throw eInval();
		((struct ofp_exp_gre_action_body_push_gre*)rofl::cmemory::somem())->ethertype = htobe16(ethertype);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_push_gre& action) {
		os << rofl::indent(0) << "<cofaction_push_gre ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( action );
		return os;
	};

};




class cofaction_exp_body_pop_gre : public cofaction_exp_body_gre {
public:

	/**
	 *
	 */
	cofaction_exp_body_pop_gre(
			uint16_t ethertype = 0) :
				cofaction_exp_body_gre(
						GRE_ACTION_POP_GRE,
						sizeof(struct ofp_exp_gre_action_body_pop_gre)) {
		set_ether_type(ethertype);
	};

	/**
	 *
	 */
	~cofaction_exp_body_pop_gre() {};

	/**
	 *
	 */
	cofaction_exp_body_pop_gre(
			const cofaction_exp_body_pop_gre& action) { *this = action; };

	/**
	 *
	 */
	cofaction_exp_body_pop_gre&
	operator= (
			const cofaction_exp_body_pop_gre& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_pop_gre(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_pop_gre&
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
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gre_action_body_pop_gre))
			throw eInval();
		return be16toh(((struct ofp_exp_gre_action_body_pop_gre*)rofl::cmemory::somem())->ethertype);
	};

	/**
	 *
	 */
	void
	set_ether_type(uint16_t ethertype) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gre_action_body_pop_gre))
			throw eInval();
		((struct ofp_exp_gre_action_body_pop_gre*)rofl::cmemory::somem())->ethertype = htobe16(ethertype);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_pop_gre& action) {
		os << rofl::indent(0) << "<cofaction_pop_gre ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( action );
		return os;
	};

};


}; // end of namespace gre
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* GRE_ACTIONS_H_ */
