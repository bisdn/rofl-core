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
#include <rofl/common/openflow/cofaction.h>

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

struct ofp_action_exp_capwap_hdr {
	uint16_t exptype;
	uint16_t explen;
	uint8_t data[0];
} __attribute__((packed));

struct ofp_action_exp_capwap_push_capwap {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));

struct ofp_action_exp_capwap_pop_capwap {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));



class cofaction_experimenter_capwap : public cofaction_experimenter {
public:

	/**
	 *
	 */
	cofaction_experimenter_capwap(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t exp_id = CAPWAP_EXP_ID,
			uint16_t exptype = 0,
			size_t bodylen = 0) :
				cofaction_experimenter(ofp_version, exp_id, rofl::cmemory(bodylen)),
				exptype(exptype) {};

	/**
	 *
	 */
	~cofaction_experimenter_capwap() {};

	/**
	 *
	 */
	cofaction_experimenter_capwap(
			const cofaction_experimenter_capwap& action) { *this = action; };

	/**
	 *
	 */
	cofaction_experimenter_capwap&
	operator= (
			const cofaction_experimenter_capwap& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter::operator= (action);
		exptype = action.exptype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_experimenter_capwap(
			const cofaction_experimenter& action) { *this = action; };

	/**
	 *
	 */
	cofaction_experimenter_capwap&
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
	operator<< (std::ostream& os, const cofaction_experimenter_capwap& action) {
		os << rofl::indent(0) << "<cofaction_experimenter_capwap exp-type: 0x" <<
				std::hex << (unsigned int)action.get_exp_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction&>( action );
		return os;
	};

private:

	uint16_t exptype;

};



class cofaction_push_capwap : public cofaction_experimenter_capwap {
public:

	/**
	 *
	 */
	cofaction_push_capwap(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t ethertype = 0) :
				cofaction_experimenter_capwap(
						ofp_version, CAPWAP_EXP_ID, CAPWAP_ACTION_PUSH_CAPWAP,
								sizeof(struct ofp_action_exp_capwap_push_capwap)),
				ethertype(ethertype) {};

	/**
	 *
	 */
	~cofaction_push_capwap() {};

	/**
	 *
	 */
	cofaction_push_capwap(
			const cofaction_push_capwap& action) { *this = action; };

	/**
	 *
	 */
	cofaction_push_capwap&
	operator= (
			const cofaction_push_capwap& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_capwap::operator= (action);
		ethertype 	= action.ethertype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_push_capwap(
			const cofaction_experimenter_capwap& action) { *this = action; };

	/**
	 *
	 */
	cofaction_push_capwap&
	operator= (
			const cofaction_experimenter_capwap& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_capwap::operator= (action);
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
	operator<< (std::ostream& os, const cofaction_push_capwap& action) {
		os << rofl::indent(0) << "<cofaction_push_capwap ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction_experimenter_capwap&>( action );
		return os;
	};

private:

	uint16_t ethertype;	// IPv4 (0x0800) or IPv6 (0x86dd)
};



class cofaction_pop_capwap : public cofaction_experimenter_capwap {
public:

	/**
	 *
	 */
	cofaction_pop_capwap(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t ethertype = 0) :
				cofaction_experimenter_capwap(
						ofp_version, CAPWAP_EXP_ID, CAPWAP_ACTION_POP_CAPWAP,
							sizeof(struct ofp_action_exp_capwap_pop_capwap)),
				ethertype(ethertype) {};

	/**
	 *
	 */
	~cofaction_pop_capwap() {};

	/**
	 *
	 */
	cofaction_pop_capwap(
			const cofaction_pop_capwap& action) { *this = action; };

	/**
	 *
	 */
	cofaction_pop_capwap&
	operator= (
			const cofaction_pop_capwap& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_capwap::operator= (action);
		ethertype 	= action.ethertype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_pop_capwap(
			const cofaction_experimenter_capwap& action) { *this = action; };

	/**
	 *
	 */
	cofaction_pop_capwap&
	operator= (
			const cofaction_experimenter_capwap& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_capwap::operator= (action);
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
	operator<< (std::ostream& os, const cofaction_pop_capwap& action) {
		os << rofl::indent(0) << "<cofaction_pop_capwap ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction_experimenter_capwap&>( action );
		return os;
	};

private:

	uint16_t ethertype;	// IPv4 (0x0800) or IPv6 (0x86dd)
};


}; // end of namespace capwap
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* CAPWAP_ACTIONS_H_ */
