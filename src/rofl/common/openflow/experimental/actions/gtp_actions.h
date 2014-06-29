/*
 * gtp_actions.h
 *
 *  Created on: 02.08.2013
 *      Author: andreas
 */

#include <inttypes.h>
#include <iostream>

#include <rofl/common/logging.h>
#include <rofl/common/openflow/cofaction.h>

#ifndef GTP_ACTIONS_H_
#define GTP_ACTIONS_H_

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

struct ofp_action_push_gtp_body {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
};

struct ofp_action_pop_gtp_body {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
};

class cofaction_push_gtp : public cofaction_experimenter {
public:

	/**
	 *
	 */
	cofaction_push_gtp(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t ethertype = 0) :
		cofaction_experimenter(ofp_version, GTP_EXP_ID,
				rofl::cmemory(sizeof(struct ofp_action_push_gtp_body))),
						exptype(GTP_ACTION_PUSH_GTP), ethertype(ethertype) {};

	/**
	 *
	 */
	~cofaction_push_gtp() {};

	/**
	 *
	 */
	cofaction_push_gtp(
			const cofaction_push_gtp& action) { *this = action; };

	/**
	 *
	 */
	cofaction_push_gtp&
	operator= (
			const cofaction_push_gtp& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter::operator= (action);
		exptype 	= action.exptype;
		ethertype 	= action.ethertype;
		return *this;
	};

public:

	/**
	 *
	 */
	uint16_t
	get_exp_type() const { return exptype; };

	// no setter for exp_type

	/**
	 *
	 */
	uint16_t
	get_ether_type() const { return ethertype; };

	/**
	 *
	 */
	void
	set_ether_type(uint16_t ethertype) { this->ethertype = ethertype; };

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
	pack(uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_push_gtp& action) {
		os << rofl::indent(0) << "<cofaction_push_gtp ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction&>( action );
		return os;
	};

private:

	uint16_t exptype;
	uint16_t ethertype;	// IPv4 (0x0800) or IPv6 (0x86dd)
};

class cofaction_pop_gtp : public cofaction_experimenter {
public:

	/**
	 *
	 */
	cofaction_pop_gtp(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t ethertype = 0) :
		cofaction_experimenter(ofp_version, GTP_EXP_ID,
				rofl::cmemory(sizeof(struct ofp_action_pop_gtp_body))),
						exptype(GTP_ACTION_PUSH_GTP), ethertype(ethertype) {};

	/**
	 *
	 */
	~cofaction_pop_gtp() {};

	/**
	 *
	 */
	cofaction_pop_gtp(
			const cofaction_pop_gtp& action) { *this = action; };

	/**
	 *
	 */
	cofaction_pop_gtp&
	operator= (
			const cofaction_pop_gtp& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter::operator= (action);
		exptype 	= action.exptype;
		ethertype 	= action.ethertype;
		return *this;
	};

public:

	/**
	 *
	 */
	uint16_t
	get_exp_type() const { return exptype; };

	// no setter for exp_type

	/**
	 *
	 */
	uint16_t
	get_ether_type() const { return ethertype; };

	/**
	 *
	 */
	void
	set_ether_type(uint16_t ethertype) { this->ethertype = ethertype; };

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
	pack(uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t* buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cofaction_pop_gtp& action) {
		os << rofl::indent(0) << "<cofaction_pop_gtp ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction&>( action );
		return os;
	};

private:

	uint16_t exptype;
	uint16_t ethertype;	// IPv4 (0x0800) or IPv6 (0x86dd)
};


}; // end of namespace gtp
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* GTP_ACTIONS_H_ */
