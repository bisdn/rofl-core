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
#include <rofl/common/openflow/cofaction.h>

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

struct ofp_action_exp_gtp_hdr {
	uint16_t exptype;
	uint16_t explen;
	uint8_t data[0];
} __attribute__((packed));

struct ofp_action_exp_gtp_push_gtp {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));

struct ofp_action_exp_gtp_pop_gtp {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));



class cofaction_experimenter_gtp : public cofaction_experimenter {
public:

	/**
	 *
	 */
	cofaction_experimenter_gtp(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t exp_id = GTP_EXP_ID,
			uint16_t exptype = 0,
			size_t bodylen = 0) :
				cofaction_experimenter(ofp_version, exp_id, rofl::cmemory(bodylen)),
				exptype(exptype) {};

	/**
	 *
	 */
	~cofaction_experimenter_gtp() {};

	/**
	 *
	 */
	cofaction_experimenter_gtp(
			const cofaction_experimenter_gtp& action) { *this = action; };

	/**
	 *
	 */
	cofaction_experimenter_gtp&
	operator= (
			const cofaction_experimenter_gtp& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter::operator= (action);
		exptype = action.exptype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_experimenter_gtp(
			const cofaction_experimenter& action) { *this = action; };

	/**
	 *
	 */
	cofaction_experimenter_gtp&
	operator= (
			const cofaction_experimenter& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter::operator= (action);
		exptype = 0;
		unpack(get_exp_body().somem(), get_exp_body().memlen());
		return *this;
	};

public:

	/**
	 *
	 */
	uint16_t
	get_exp_type() const { return exptype; };

	/**
	 *
	 */
	void
	set_exp_type(uint16_t exptype) { this->exptype = exptype; };

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
	operator<< (std::ostream& os, const cofaction_experimenter_gtp& action) {
		os << rofl::indent(0) << "<cofaction_experimenter_gtp exp-type: 0x" <<
				std::hex << (unsigned int)action.get_exp_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction&>( action );
		return os;
	};

private:

	uint16_t exptype;

};



class cofaction_push_gtp : public cofaction_experimenter_gtp {
public:

	/**
	 *
	 */
	cofaction_push_gtp(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t ethertype = 0) :
				cofaction_experimenter_gtp(
						ofp_version, GTP_EXP_ID, GTP_ACTION_PUSH_GTP,
								sizeof(struct ofp_action_exp_gtp_push_gtp)),
				ethertype(ethertype) {};

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
		cofaction_experimenter_gtp::operator= (action);
		ethertype 	= action.ethertype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_push_gtp(
			const cofaction_experimenter_gtp& action) { *this = action; };

	/**
	 *
	 */
	cofaction_push_gtp&
	operator= (
			const cofaction_experimenter_gtp& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_gtp::operator= (action);
		ethertype 	= 0;
		unpack(get_exp_body().somem(), get_exp_body().memlen());
		return *this;
	};

public:

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
		os << dynamic_cast<const cofaction_experimenter_gtp&>( action );
		return os;
	};

private:

	uint16_t ethertype;	// IPv4 (0x0800) or IPv6 (0x86dd)
};



class cofaction_pop_gtp : public cofaction_experimenter_gtp {
public:

	/**
	 *
	 */
	cofaction_pop_gtp(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t ethertype = 0) :
				cofaction_experimenter_gtp(
						ofp_version, GTP_EXP_ID, GTP_ACTION_POP_GTP,
							sizeof(struct ofp_action_exp_gtp_pop_gtp)),
				ethertype(ethertype) {};

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
		cofaction_experimenter_gtp::operator= (action);
		ethertype 	= action.ethertype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_pop_gtp(
			const cofaction_experimenter_gtp& action) { *this = action; };

	/**
	 *
	 */
	cofaction_pop_gtp&
	operator= (
			const cofaction_experimenter_gtp& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_gtp::operator= (action);
		ethertype 	= 0;
		unpack(get_exp_body().somem(), get_exp_body().memlen());
		return *this;
	};

public:

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
		os << dynamic_cast<const cofaction_experimenter_gtp&>( action );
		return os;
	};

private:

	uint16_t ethertype;	// IPv4 (0x0800) or IPv6 (0x86dd)
};


}; // end of namespace gtp
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* GTP_ACTIONS_H_ */
