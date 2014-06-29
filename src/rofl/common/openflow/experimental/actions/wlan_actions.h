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
#include <rofl/common/openflow/cofaction.h>

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

struct ofp_action_exp_wlan_hdr {
	uint16_t exptype;
	uint16_t explen;
	uint8_t data[0];
} __attribute__((packed));

struct ofp_action_exp_wlan_push_wlan {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));

struct ofp_action_exp_wlan_pop_wlan {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));



class cofaction_experimenter_wlan : public cofaction_experimenter {
public:

	/**
	 *
	 */
	cofaction_experimenter_wlan(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t exp_id = WLAN_EXP_ID,
			uint16_t exptype = 0,
			size_t bodylen = 0) :
				cofaction_experimenter(ofp_version, exp_id, rofl::cmemory(bodylen)),
				exptype(exptype) {};

	/**
	 *
	 */
	~cofaction_experimenter_wlan() {};

	/**
	 *
	 */
	cofaction_experimenter_wlan(
			const cofaction_experimenter_wlan& action) { *this = action; };

	/**
	 *
	 */
	cofaction_experimenter_wlan&
	operator= (
			const cofaction_experimenter_wlan& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter::operator= (action);
		exptype = action.exptype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_experimenter_wlan(
			const cofaction_experimenter& action) { *this = action; };

	/**
	 *
	 */
	cofaction_experimenter_wlan&
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
	operator<< (std::ostream& os, const cofaction_experimenter_wlan& action) {
		os << rofl::indent(0) << "<cofaction_experimenter_wlan exp-type: 0x" <<
				std::hex << (unsigned int)action.get_exp_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction&>( action );
		return os;
	};

private:

	uint16_t exptype;

};



class cofaction_push_wlan : public cofaction_experimenter_wlan {
public:

	/**
	 *
	 */
	cofaction_push_wlan(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t ethertype = 0) :
				cofaction_experimenter_wlan(
						ofp_version, WLAN_EXP_ID, WLAN_ACTION_PUSH_WLAN,
								sizeof(struct ofp_action_exp_wlan_push_wlan)),
				ethertype(ethertype) {};

	/**
	 *
	 */
	~cofaction_push_wlan() {};

	/**
	 *
	 */
	cofaction_push_wlan(
			const cofaction_push_wlan& action) { *this = action; };

	/**
	 *
	 */
	cofaction_push_wlan&
	operator= (
			const cofaction_push_wlan& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_wlan::operator= (action);
		ethertype 	= action.ethertype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_push_wlan(
			const cofaction_experimenter_wlan& action) { *this = action; };

	/**
	 *
	 */
	cofaction_push_wlan&
	operator= (
			const cofaction_experimenter_wlan& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_wlan::operator= (action);
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
	operator<< (std::ostream& os, const cofaction_push_wlan& action) {
		os << rofl::indent(0) << "<cofaction_push_wlan ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction_experimenter_wlan&>( action );
		return os;
	};

private:

	uint16_t ethertype;	// IPv4 (0x0800) or IPv6 (0x86dd)
};



class cofaction_pop_wlan : public cofaction_experimenter_wlan {
public:

	/**
	 *
	 */
	cofaction_pop_wlan(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t ethertype = 0) :
				cofaction_experimenter_wlan(
						ofp_version, WLAN_EXP_ID, WLAN_ACTION_POP_WLAN,
							sizeof(struct ofp_action_exp_wlan_pop_wlan)),
				ethertype(ethertype) {};

	/**
	 *
	 */
	~cofaction_pop_wlan() {};

	/**
	 *
	 */
	cofaction_pop_wlan(
			const cofaction_pop_wlan& action) { *this = action; };

	/**
	 *
	 */
	cofaction_pop_wlan&
	operator= (
			const cofaction_pop_wlan& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_wlan::operator= (action);
		ethertype 	= action.ethertype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_pop_wlan(
			const cofaction_experimenter_wlan& action) { *this = action; };

	/**
	 *
	 */
	cofaction_pop_wlan&
	operator= (
			const cofaction_experimenter_wlan& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_wlan::operator= (action);
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
	operator<< (std::ostream& os, const cofaction_pop_wlan& action) {
		os << rofl::indent(0) << "<cofaction_pop_wlan ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction_experimenter_wlan&>( action );
		return os;
	};

private:

	uint16_t ethertype;	// IPv4 (0x0800) or IPv6 (0x86dd)
};


}; // end of namespace wlan
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* WLAN_ACTIONS_H_ */
