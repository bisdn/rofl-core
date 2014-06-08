/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFACTIONS_H
#define COFACTIONS_H 1

#include <inttypes.h>
#include <map>
#include <algorithm>

#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofaction.h"

namespace rofl {
namespace openflow {

class cofactions {
public:

	/**
	 *
	 */
	cofactions(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	virtual
	~cofactions();

	/**
	 *
	 */
	cofactions(
			const cofactions& actions);

	/**
	 *
	 */
	cofactions&
	operator= (
			const cofactions& actions);

	/**
	 *
	 */
	bool
	operator== (
			const cofactions& actions);

public:

	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	unpack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	pack(
			uint8_t* buf, size_t buflen);

public:

	/**
	 *
	 */
	void
	clear();

	/** counts number of actions of specific type in ActionList acvec of this instruction
	 */
	int
	count_action_type(
			uint16_t type);


	/** counts number of actions output in ActionList acvec of this instruction
	 */
	int
	count_action_output(
			uint32_t port_no = 0) const;



	/** returns a list of port_no values for each ActionOutput instances
	 *
	 */
	std::list<uint32_t>
	actions_output_ports();

	/**
	 *
	 */
	void
	check_prerequisites() const;

public:

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) {
		this->ofp_version = ofp_version;
		for (std::map<unsigned int, cofaction*>::iterator
				it = actions.begin(); it != actions.end(); ++it) {
			it->second->set_version(ofp_version);
		}
	};

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	const std::map<unsigned int, unsigned int>&
	get_actions_index() const { return actions_index; };

	/**
	 *
	 */
	std::map<unsigned int, cofaction*>&
	set_actions() { return actions; };

	/**
	 *
	 */
	const std::map<unsigned int, cofaction*>&
	get_actions() const { return actions; };

public:

	cofaction&
	add_action(unsigned int index);

	cofaction&
	set_action(unsigned int index);

	const cofaction&
	get_action(unsigned int index) const;

	void
	drop_action(unsigned int index);

	bool
	has_action(unsigned int index) const;


	// ActionOutput

	cofaction_output&
	add_action_output(unsigned int index = 0);

	cofaction_output&
	set_action_output(unsigned int index = 0);

	const cofaction_output&
	get_action_output(unsigned int index = 0) const;

	void
	drop_action_output(unsigned int index = 0);

	bool
	has_action_output(unsigned int index = 0) const;


	// ActionSetVlanVid

	cofaction_set_vlan_vid&
	add_action_set_vlan_vid(unsigned int index = 0);

	cofaction_set_vlan_vid&
	set_action_set_vlan_vid(unsigned int index = 0);

	const cofaction_set_vlan_vid&
	get_action_set_vlan_vid(unsigned int index = 0) const;

	void
	drop_action_set_vlan_vid(unsigned int index = 0);

	bool
	has_action_set_vlan_vid(unsigned int index = 0) const;

	// ActionSetVlanPcp

	cofaction_set_vlan_pcp&
	add_action_set_vlan_pcp(unsigned int index = 0);

	cofaction_set_vlan_pcp&
	set_action_set_vlan_pcp(unsigned int index = 0);

	const cofaction_set_vlan_pcp&
	get_action_set_vlan_pcp(unsigned int index = 0) const;

	void
	drop_action_set_vlan_pcp(unsigned int index = 0);

	bool
	has_action_set_vlan_pcp(unsigned int index = 0) const;

	// ActionSetDlSrc

	cofaction_set_dl_src&
	add_action_set_dl_src(unsigned int index = 0);

	cofaction_set_dl_src&
	set_action_set_dl_src(unsigned int index = 0);

	const cofaction_set_dl_src&
	get_action_set_dl_src(unsigned int index = 0) const;

	void
	drop_action_set_dl_src(unsigned int index = 0);

	bool
	has_action_set_dl_src(unsigned int index = 0) const;

	// ActionSetDlDst

	cofaction_set_dl_dst&
	add_action_set_dl_dst(unsigned int index = 0);

	cofaction_set_dl_dst&
	set_action_set_dl_dst(unsigned int index = 0);

	const cofaction_set_dl_dst&
	get_action_set_dl_dst(unsigned int index = 0) const;

	void
	drop_action_set_dl_dst(unsigned int index = 0);

	bool
	has_action_set_dl_dst(unsigned int index = 0) const;





	// ActionSetField

	cofaction_set_field&
	add_action_set_field(unsigned int index = 0);

	cofaction_set_field&
	set_action_set_field(unsigned int index = 0);

	const cofaction_set_field&
	get_action_set_field(unsigned int index = 0) const;

	void
	drop_action_set_field(unsigned int index = 0);

	bool
	has_action_set_field(unsigned int index = 0) const;


public:

#if 0
	cofaction&
	append_action(cofaction const action);

	cofaction_output&
	append_action_output(uint32_t port_no, uint16_t max_len = 128);

	cofaction_set_vlan_vid&
	append_action_set_vlan_vid(uint16_t vid);

	cofaction_set_vlan_pcp&
	append_action_set_vlan_pcp(uint8_t pcp);

	cofaction_strip_vlan&
	append_action_strip_vlan();

	cofaction_set_dl_src&
	append_action_set_dl_src(cmacaddr const& maddr);

	cofaction_set_dl_dst&
	append_action_set_dl_dst(cmacaddr const& maddr);

	cofaction_set_nw_src&
	append_action_set_nw_src(caddress const& addr);

	cofaction_set_nw_dst&
	append_action_set_nw_dst(caddress const& addr);

	cofaction_set_nw_tos&
	append_action_set_nw_tos(uint8_t tos);

	cofaction_set_tp_src&
	append_action_set_tp_src(uint16_t tp_src);

	cofaction_set_tp_dst&
	append_action_set_tp_dst(uint16_t tp_dst);

	cofaction_enqueue&
	append_action_enqueue(uint16_t port_no, uint32_t queue_id);

	cofaction_copy_ttl_out&
	append_action_copy_ttl_out();

	cofaction_copy_ttl_in&
	append_action_copy_ttl_in();

	cofaction_set_mpls_ttl&
	append_action_set_mpls_ttl(uint8_t ttl);

	cofaction_dec_mpls_ttl&
	append_action_dec_mpls_ttl();

	cofaction_push_vlan&
	append_action_push_vlan(uint16_t eth_type);

	cofaction_pop_vlan&
	append_action_pop_vlan();

	cofaction_push_mpls&
	append_action_push_mpls(uint16_t eth_type);

	cofaction_pop_mpls&
	append_action_pop_mpls(uint16_t eth_type);

	cofaction_set_queue&
	append_action_set_queue(uint32_t queue_id);

	cofaction_group&
	append_action_group(uint32_t group_id);

	cofaction_set_nw_ttl&
	append_action_set_nw_ttl(uint8_t ttl);

	cofaction_dec_nw_ttl&
	append_action_dec_nw_ttl();

	cofaction_set_field&
	append_action_set_field(coxmatch const& oxm);

	cofaction_experimenter&
	append_action_experimenter(cofaction const& action);

	cofaction_vendor&
	append_action_vendor(cofaction const& action);
#endif

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofactions const& actions) {
		os << rofl::indent(0) << "<cofactions ofp-version:" << (int)actions.get_version() <<
				" #actions:" << (int)actions.get_actions().size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<unsigned int, cofaction*>::const_iterator
				it = actions.begin(); it != actions.end(); ++it) {
			cofaction::dump(os, *(it->second));
		}
		return os;
	};

private:

	uint8_t 								ofp_version;
	std::map<unsigned int, cofaction*>		actions;
	std::map<unsigned int, unsigned int>	actions_index;

public:

    typedef typename std::map<unsigned int, cofaction*>::iterator iterator;
    typedef typename std::map<unsigned int, cofaction*>::const_iterator const_iterator;
    typedef typename std::map<unsigned int, cofaction*>::reverse_iterator reverse_iterator;
    typedef typename std::map<unsigned int, cofaction*>::const_reverse_iterator const_reverse_iterator;

    iterator begin() 						{ return actions.begin(); }
    iterator end() 							{ return actions.end(); }
    const_iterator begin() 	const 			{ return actions.begin(); }
    const_iterator end() 	const 			{ return actions.end(); }

    reverse_iterator rbegin() 				{ return actions.rbegin(); }
    reverse_iterator rend() 				{ return actions.rend(); }
    const_reverse_iterator rbegin() const  	{ return actions.rbegin(); }
    const_reverse_iterator rend() 	const  	{ return actions.rend(); }

};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
