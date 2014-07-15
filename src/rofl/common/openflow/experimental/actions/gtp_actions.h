/*
 * gtp_actions.h
 *
 *  Created on: 02.08.2013
 *      Author: andreas
 */

#ifndef GTP_ACTIONS_H_
#define GTP_ACTIONS_H_

#include <inttypes.h>
#include <iostream>

#include <rofl/common/logging.h>
#include <rofl/common/cmemory.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace gtp {

enum gtp_exp_id_t {
	GTP_EXP_ID = 0x5555a781,
};

enum gtp_action_type_t {
	GTP_ACTION_PUSH_GTP,
	GTP_ACTION_POP_GTP,
};

struct ofp_exp_gtp_action_body_hdr {
	uint16_t exptype;
	uint16_t explen;
	uint8_t data[0];
} __attribute__((packed));

struct ofp_exp_gtp_action_body_push_gtp {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));

struct ofp_exp_gtp_action_body_pop_gtp {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));



class cofaction_exp_body_gtp : public rofl::cmemory {
public:

	/**
	 *
	 */
	cofaction_exp_body_gtp(
			uint16_t exptype = 0,
			size_t bodylen = 0) :
				rofl::cmemory(bodylen) {
		set_exp_type(exptype);
		set_exp_len(bodylen);
	};

	/**
	 *
	 */
	~cofaction_exp_body_gtp() {};

	/**
	 *
	 */
	cofaction_exp_body_gtp(
			const cofaction_exp_body_gtp& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_gtp&
	operator= (
			const cofaction_exp_body_gtp& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_gtp(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_gtp&
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
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gtp_action_body_hdr))
			throw eInval();
		return be16toh(((struct ofp_exp_gtp_action_body_hdr*)rofl::cmemory::somem())->exptype);
	};

	/**
	 *
	 */
	void
	set_exp_type(uint16_t exptype) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gtp_action_body_hdr))
			throw eInval();
		((struct ofp_exp_gtp_action_body_hdr*)rofl::cmemory::somem())->exptype = htobe16(exptype);
	};

	/**
	 *
	 */
	uint16_t
	get_exp_len() const {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gtp_action_body_hdr))
			throw eInval();
		return be16toh(((struct ofp_exp_gtp_action_body_hdr*)rofl::cmemory::somem())->explen);
	};

	/**
	 *
	 */
	void
	set_exp_len(uint16_t explen) {
		if (rofl::cmemory::memlen() < sizeof(struct ofp_exp_gtp_action_body_hdr))
			throw eInval();
		((struct ofp_exp_gtp_action_body_hdr*)rofl::cmemory::somem())->explen = htobe16(explen);
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_gtp& body) {
		os << rofl::indent(0) << "<cofaction_exp_body_gtp exp-type: 0x" <<
				std::hex << (unsigned int)body.get_exp_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( body );
		return os;
	};
};



class cofaction_exp_body_push_gtp : public cofaction_exp_body_gtp {
public:

	/**
	 *
	 */
	cofaction_exp_body_push_gtp(
			uint16_t ethertype = 0) :
				cofaction_exp_body_gtp(
						GTP_ACTION_PUSH_GTP,
						sizeof(struct ofp_exp_gtp_action_body_push_gtp)) {
		set_ether_type(ethertype);
	};

	/**
	 *
	 */
	~cofaction_exp_body_push_gtp() {};

	/**
	 *
	 */
	cofaction_exp_body_push_gtp(
			const cofaction_exp_body_push_gtp& action) { *this = action; };

	/**
	 *
	 */
	cofaction_exp_body_push_gtp&
	operator= (
			const cofaction_exp_body_push_gtp& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_push_gtp(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_push_gtp&
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
	get_ether_type() const { return be16toh(((struct ofp_exp_gtp_action_body_push_gtp*)rofl::cmemory::somem())->ethertype); };

	/**
	 *
	 */
	void
	set_ether_type(uint16_t ethertype) { ((struct ofp_exp_gtp_action_body_push_gtp*)rofl::cmemory::somem())->ethertype = htobe16(ethertype); };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_push_gtp& action) {
		os << rofl::indent(0) << "<cofaction_push_gtp ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( action );
		return os;
	};

};




class cofaction_exp_body_pop_gtp : public cofaction_exp_body_gtp {
public:

	/**
	 *
	 */
	cofaction_exp_body_pop_gtp(
			uint16_t ethertype = 0) :
				cofaction_exp_body_gtp(
						GTP_ACTION_POP_GTP,
						sizeof(struct ofp_exp_gtp_action_body_pop_gtp)) {
		set_ether_type(ethertype);
	};

	/**
	 *
	 */
	~cofaction_exp_body_pop_gtp() {};

	/**
	 *
	 */
	cofaction_exp_body_pop_gtp(
			const cofaction_exp_body_pop_gtp& action) { *this = action; };

	/**
	 *
	 */
	cofaction_exp_body_pop_gtp&
	operator= (
			const cofaction_exp_body_pop_gtp& body) {
		if (this == &body)
			return *this;
		rofl::cmemory::operator= (body);
		return *this;
	};

	/**
	 *
	 */
	cofaction_exp_body_pop_gtp(
			const rofl::cmemory& body) { *this = body; };

	/**
	 *
	 */
	cofaction_exp_body_pop_gtp&
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
	get_ether_type() const { return be16toh(((struct ofp_exp_gtp_action_body_pop_gtp*)rofl::cmemory::somem())->ethertype); };

	/**
	 *
	 */
	void
	set_ether_type(uint16_t ethertype) { ((struct ofp_exp_gtp_action_body_pop_gtp*)rofl::cmemory::somem())->ethertype = htobe16(ethertype); };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_exp_body_pop_gtp& action) {
		os << rofl::indent(0) << "<cofaction_pop_gtp ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const rofl::cmemory&>( action );
		return os;
	};

};


}; // end of namespace gtp
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* GTP_ACTIONS_H_ */
