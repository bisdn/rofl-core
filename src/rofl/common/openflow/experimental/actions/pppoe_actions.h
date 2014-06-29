#ifndef PPPOE_ACTIONS_H
#define PPPOE_ACTIONS_H 1

#include <inttypes.h>
#include <iostream>

#include <rofl/common/logging.h>
#include <rofl/common/openflow/cofaction.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace pppoe {

enum pppoe_exp_id_t {
	PPPOE_EXP_ID = 0x5555a780,
};

enum pppoe_action_type_t {
	PPPOE_ACTION_PUSH_PPPOE,
	PPPOE_ACTION_POP_PPPOE,
};

struct ofp_action_exp_pppoe_hdr {
	uint16_t exptype;
	uint16_t explen;
	uint8_t data[0];
} __attribute__((packed));

struct ofp_action_exp_pppoe_push_pppoe {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));

struct ofp_action_exp_pppoe_pop_pppoe {
	uint16_t exptype;
	uint16_t explen;
	uint16_t ethertype;
} __attribute__((packed));



class cofaction_experimenter_pppoe : public cofaction_experimenter {
public:

	/**
	 *
	 */
	cofaction_experimenter_pppoe(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t exp_id = PPPOE_EXP_ID,
			uint16_t exptype = 0,
			size_t bodylen = 0) :
				cofaction_experimenter(ofp_version, exp_id, rofl::cmemory(bodylen)),
				exptype(exptype) {};

	/**
	 *
	 */
	~cofaction_experimenter_pppoe() {};

	/**
	 *
	 */
	cofaction_experimenter_pppoe(
			const cofaction_experimenter_pppoe& action) { *this = action; };

	/**
	 *
	 */
	cofaction_experimenter_pppoe&
	operator= (
			const cofaction_experimenter_pppoe& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter::operator= (action);
		exptype = action.exptype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_experimenter_pppoe(
			const cofaction_experimenter& action) { *this = action; };

	/**
	 *
	 */
	cofaction_experimenter_pppoe&
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
	operator<< (std::ostream& os, const cofaction_experimenter_pppoe& action) {
		os << rofl::indent(0) << "<cofaction_experimenter_pppoe exp-type: 0x" <<
				std::hex << (unsigned int)action.get_exp_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction&>( action );
		return os;
	};

private:

	uint16_t exptype;

};



class cofaction_push_pppoe : public cofaction_experimenter_pppoe {
public:

	/**
	 *
	 */
	cofaction_push_pppoe(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t ethertype = 0) :
				cofaction_experimenter_pppoe(
						ofp_version, PPPOE_EXP_ID, PPPOE_ACTION_PUSH_PPPOE,
								sizeof(struct ofp_action_exp_pppoe_push_pppoe)),
				ethertype(ethertype) {};

	/**
	 *
	 */
	~cofaction_push_pppoe() {};

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
		cofaction_experimenter_pppoe::operator= (action);
		ethertype 	= action.ethertype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_push_pppoe(
			const cofaction_experimenter_pppoe& action) { *this = action; };

	/**
	 *
	 */
	cofaction_push_pppoe&
	operator= (
			const cofaction_experimenter_pppoe& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_pppoe::operator= (action);
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
	operator<< (std::ostream& os, const cofaction_push_pppoe& action) {
		os << rofl::indent(0) << "<cofaction_push_pppoe ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction_experimenter_pppoe&>( action );
		return os;
	};

private:

	uint16_t ethertype;	// IPv4 (0x0800) or IPv6 (0x86dd)
};



class cofaction_pop_pppoe : public cofaction_experimenter_pppoe {
public:

	/**
	 *
	 */
	cofaction_pop_pppoe(
			uint8_t ofp_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint16_t ethertype = 0) :
				cofaction_experimenter_pppoe(
						ofp_version, PPPOE_EXP_ID, PPPOE_ACTION_POP_PPPOE,
							sizeof(struct ofp_action_exp_pppoe_pop_pppoe)),
				ethertype(ethertype) {};

	/**
	 *
	 */
	~cofaction_pop_pppoe() {};

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
		cofaction_experimenter_pppoe::operator= (action);
		ethertype 	= action.ethertype;
		return *this;
	};

	/**
	 *
	 */
	cofaction_pop_pppoe(
			const cofaction_experimenter_pppoe& action) { *this = action; };

	/**
	 *
	 */
	cofaction_pop_pppoe&
	operator= (
			const cofaction_experimenter_pppoe& action) {
		if (this == &action)
			return *this;
		cofaction_experimenter_pppoe::operator= (action);
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
	operator<< (std::ostream& os, const cofaction_pop_pppoe& action) {
		os << rofl::indent(0) << "<cofaction_pop_pppoe ether-type: 0x" <<
				std::hex << (unsigned int)action.get_ether_type() << std::dec
				<< ">" << std::endl;
		rofl::indent i(2);
		os << dynamic_cast<const cofaction_experimenter_pppoe&>( action );
		return os;
	};

private:

	uint16_t ethertype;	// IPv4 (0x0800) or IPv6 (0x86dd)
};


}; // end of namespace pppoe
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
