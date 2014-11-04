/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFACTIONS_H
#define COFACTIONS_H 1

#include <inttypes.h>
#include <map>
#include <list>
#include <algorithm>

#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofaction.h"
#include "rofl/common/openflow/experimental/actions/gtp_actions.h"
#include "rofl/common/cindex.h"

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
			uint16_t type) const;


	/** counts number of actions output in ActionList acvec of this instruction
	 */
	int
	count_action_output(
			uint32_t port_no = 0) const;



	/** returns a list of port_no values for each ActionOutput instances
	 *
	 */
	std::list<uint32_t>
	actions_output_ports() const;

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
		for (std::map<cindex, cofaction*>::iterator
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
	const std::map<cindex, unsigned int>&
	get_actions_index() const { return actions_index; };

	/**
	 *
	 */
	std::map<cindex, cofaction*>&
	set_actions() { return actions; };

	/**
	 *
	 */
	const std::map<cindex, cofaction*>&
	get_actions() const { return actions; };

	/**
	 *
	 */
	size_t
	size() const { return actions.size(); };

	/**
	 *
	 */
	bool
	empty() const { return actions.empty(); };

public:

	void
	drop_action(const cindex& index);

	bool
	has_action(const cindex& index) const;


	// ActionOutput

	cofaction_output&
	add_action_output(const cindex& index);

	cofaction_output&
	set_action_output(const cindex& index);

	const cofaction_output&
	get_action_output(const cindex& index) const;

	void
	drop_action_output(const cindex& index);

	bool
	has_action_output(const cindex& index) const;


	// ActionSetVlanVid

	cofaction_set_vlan_vid&
	add_action_set_vlan_vid(const cindex& index);

	cofaction_set_vlan_vid&
	set_action_set_vlan_vid(const cindex& index);

	const cofaction_set_vlan_vid&
	get_action_set_vlan_vid(const cindex& index) const;

	void
	drop_action_set_vlan_vid(const cindex& index);

	bool
	has_action_set_vlan_vid(const cindex& index) const;


	// ActionSetVlanPcp

	cofaction_set_vlan_pcp&
	add_action_set_vlan_pcp(const cindex& index);

	cofaction_set_vlan_pcp&
	set_action_set_vlan_pcp(const cindex& index);

	const cofaction_set_vlan_pcp&
	get_action_set_vlan_pcp(const cindex& index) const;

	void
	drop_action_set_vlan_pcp(const cindex& index);

	bool
	has_action_set_vlan_pcp(const cindex& index) const;


	// ActionStripVlan

	cofaction_strip_vlan&
	add_action_strip_vlan(const cindex& index);

	cofaction_strip_vlan&
	set_action_strip_vlan(const cindex& index);

	const cofaction_strip_vlan&
	get_action_strip_vlan(const cindex& index) const;

	void
	drop_action_strip_vlan(const cindex& index);

	bool
	has_action_strip_vlan(const cindex& index) const;


	// ActionSetDlSrc

	cofaction_set_dl_src&
	add_action_set_dl_src(const cindex& index);

	cofaction_set_dl_src&
	set_action_set_dl_src(const cindex& index);

	const cofaction_set_dl_src&
	get_action_set_dl_src(const cindex& index) const;

	void
	drop_action_set_dl_src(const cindex& index);

	bool
	has_action_set_dl_src(const cindex& index) const;


	// ActionSetDlDst

	cofaction_set_dl_dst&
	add_action_set_dl_dst(const cindex& index);

	cofaction_set_dl_dst&
	set_action_set_dl_dst(const cindex& index);

	const cofaction_set_dl_dst&
	get_action_set_dl_dst(const cindex& index) const;

	void
	drop_action_set_dl_dst(const cindex& index);

	bool
	has_action_set_dl_dst(const cindex& index) const;


	// ActionSetNwSrc

	cofaction_set_nw_src&
	add_action_set_nw_src(const cindex& index);

	cofaction_set_nw_src&
	set_action_set_nw_src(const cindex& index);

	const cofaction_set_nw_src&
	get_action_set_nw_src(const cindex& index) const;

	void
	drop_action_set_nw_src(const cindex& index);

	bool
	has_action_set_nw_src(const cindex& index) const;


	// ActionSetNwDst

	cofaction_set_nw_dst&
	add_action_set_nw_dst(const cindex& index);

	cofaction_set_nw_dst&
	set_action_set_nw_dst(const cindex& index);

	const cofaction_set_nw_dst&
	get_action_set_nw_dst(const cindex& index) const;

	void
	drop_action_set_nw_dst(const cindex& index);

	bool
	has_action_set_nw_dst(const cindex& index) const;


	// ActionSetNwTos

	cofaction_set_nw_tos&
	add_action_set_nw_tos(const cindex& index);

	cofaction_set_nw_tos&
	set_action_set_nw_tos(const cindex& index);

	const cofaction_set_nw_tos&
	get_action_set_nw_tos(const cindex& index) const;

	void
	drop_action_set_nw_tos(const cindex& index);

	bool
	has_action_set_nw_tos(const cindex& index) const;


	// ActionSetTpSrc

	cofaction_set_tp_src&
	add_action_set_tp_src(const cindex& index);

	cofaction_set_tp_src&
	set_action_set_tp_src(const cindex& index);

	const cofaction_set_tp_src&
	get_action_set_tp_src(const cindex& index) const;

	void
	drop_action_set_tp_src(const cindex& index);

	bool
	has_action_set_tp_src(const cindex& index) const;


	// ActionSetTpDst

	cofaction_set_tp_dst&
	add_action_set_tp_dst(const cindex& index);

	cofaction_set_tp_dst&
	set_action_set_tp_dst(const cindex& index);

	const cofaction_set_tp_dst&
	get_action_set_tp_dst(const cindex& index) const;

	void
	drop_action_set_tp_dst(const cindex& index);

	bool
	has_action_set_tp_dst(const cindex& index) const;


	// ActionEnqueue

	cofaction_enqueue&
	add_action_enqueue(const cindex& index);

	cofaction_enqueue&
	set_action_enqueue(const cindex& index);

	const cofaction_enqueue&
	get_action_enqueue(const cindex& index) const;

	void
	drop_action_enqueue(const cindex& index);

	bool
	has_action_enqueue(const cindex& index) const;


	// ActionVendor

	cofaction_vendor&
	add_action_vendor(const cindex& index);

	cofaction_vendor&
	set_action_vendor(const cindex& index);

	const cofaction_vendor&
	get_action_vendor(const cindex& index) const;

	void
	drop_action_vendor(const cindex& index);

	bool
	has_action_vendor(const cindex& index) const;


	// ActionCopyTtlOut

	cofaction_copy_ttl_out&
	add_action_copy_ttl_out(const cindex& index);

	cofaction_copy_ttl_out&
	set_action_copy_ttl_out(const cindex& index);

	const cofaction_copy_ttl_out&
	get_action_copy_ttl_out(const cindex& index) const;

	void
	drop_action_copy_ttl_out(const cindex& index);

	bool
	has_action_copy_ttl_out(const cindex& index) const;


	// ActionCopyTtlIn

	cofaction_copy_ttl_in&
	add_action_copy_ttl_in(const cindex& index);

	cofaction_copy_ttl_in&
	set_action_copy_ttl_in(const cindex& index);

	const cofaction_copy_ttl_in&
	get_action_copy_ttl_in(const cindex& index) const;

	void
	drop_action_copy_ttl_in(const cindex& index);

	bool
	has_action_copy_ttl_in(const cindex& index) const;


	// ActionSetMplsTtl

	cofaction_set_mpls_ttl&
	add_action_set_mpls_ttl(const cindex& index);

	cofaction_set_mpls_ttl&
	set_action_set_mpls_ttl(const cindex& index);

	const cofaction_set_mpls_ttl&
	get_action_set_mpls_ttl(const cindex& index) const;

	void
	drop_action_set_mpls_ttl(const cindex& index);

	bool
	has_action_set_mpls_ttl(const cindex& index) const;


	// ActionDecMplsTtl

	cofaction_dec_mpls_ttl&
	add_action_dec_mpls_ttl(const cindex& index);

	cofaction_dec_mpls_ttl&
	set_action_dec_mpls_ttl(const cindex& index);

	const cofaction_dec_mpls_ttl&
	get_action_dec_mpls_ttl(const cindex& index) const;

	void
	drop_action_dec_mpls_ttl(const cindex& index);

	bool
	has_action_dec_mpls_ttl(const cindex& index) const;


	// ActionPushVlan

	cofaction_push_vlan&
	add_action_push_vlan(const cindex& index);

	cofaction_push_vlan&
	set_action_push_vlan(const cindex& index);

	const cofaction_push_vlan&
	get_action_push_vlan(const cindex& index) const;

	void
	drop_action_push_vlan(const cindex& index);

	bool
	has_action_push_vlan(const cindex& index) const;


	// ActionPopVlan

	cofaction_pop_vlan&
	add_action_pop_vlan(const cindex& index);

	cofaction_pop_vlan&
	set_action_pop_vlan(const cindex& index);

	const cofaction_pop_vlan&
	get_action_pop_vlan(const cindex& index) const;

	void
	drop_action_pop_vlan(const cindex& index);

	bool
	has_action_pop_vlan(const cindex& index) const;


	// ActionPushMpls

	cofaction_push_mpls&
	add_action_push_mpls(const cindex& index);

	cofaction_push_mpls&
	set_action_push_mpls(const cindex& index);

	const cofaction_push_mpls&
	get_action_push_mpls(const cindex& index) const;

	void
	drop_action_push_mpls(const cindex& index);

	bool
	has_action_push_mpls(const cindex& index) const;


	// ActionPopMpls

	cofaction_pop_mpls&
	add_action_pop_mpls(const cindex& index);

	cofaction_pop_mpls&
	set_action_pop_mpls(const cindex& index);

	const cofaction_pop_mpls&
	get_action_pop_mpls(const cindex& index) const;

	void
	drop_action_pop_mpls(const cindex& index);

	bool
	has_action_pop_mpls(const cindex& index) const;


	// ActionGroup

	cofaction_group&
	add_action_group(const cindex& index);

	cofaction_group&
	set_action_group(const cindex& index);

	const cofaction_group&
	get_action_group(const cindex& index) const;

	void
	drop_action_group(const cindex& index);

	bool
	has_action_group(const cindex& index) const;


	// ActionSetNwTtl

	cofaction_set_nw_ttl&
	add_action_set_nw_ttl(const cindex& index);

	cofaction_set_nw_ttl&
	set_action_set_nw_ttl(const cindex& index);

	const cofaction_set_nw_ttl&
	get_action_set_nw_ttl(const cindex& index) const;

	void
	drop_action_set_nw_ttl(const cindex& index);

	bool
	has_action_set_nw_ttl(const cindex& index) const;


	// ActionDecNwTtl

	cofaction_dec_nw_ttl&
	add_action_dec_nw_ttl(const cindex& index);

	cofaction_dec_nw_ttl&
	set_action_dec_nw_ttl(const cindex& index);

	const cofaction_dec_nw_ttl&
	get_action_dec_nw_ttl(const cindex& index) const;

	void
	drop_action_dec_nw_ttl(const cindex& index);

	bool
	has_action_dec_nw_ttl(const cindex& index) const;


	// ActionSetQueue

	cofaction_set_queue&
	add_action_set_queue(const cindex& index);

	cofaction_set_queue&
	set_action_set_queue(const cindex& index);

	const cofaction_set_queue&
	get_action_set_queue(const cindex& index) const;

	void
	drop_action_set_queue(const cindex& index);

	bool
	has_action_set_queue(const cindex& index) const;


	// ActionSetField

	cofaction_set_field&
	add_action_set_field(const cindex& index);

	cofaction_set_field&
	set_action_set_field(const cindex& index);

	const cofaction_set_field&
	get_action_set_field(const cindex& index) const;

	void
	drop_action_set_field(const cindex& index);

	bool
	has_action_set_field(const cindex& index) const;


	// ActionExperimenter

	cofaction_experimenter&
	add_action_experimenter(const cindex& index);

	cofaction_experimenter&
	set_action_experimenter(const cindex& index);

	const cofaction_experimenter&
	get_action_experimenter(const cindex& index) const;

	void
	drop_action_experimenter(const cindex& index);

	bool
	has_action_experimenter(const cindex& index) const;


	// ActionPushPbb

	cofaction_push_pbb&
	add_action_push_pbb(const cindex& index);

	cofaction_push_pbb&
	set_action_push_pbb(const cindex& index);

	const cofaction_push_pbb&
	get_action_push_pbb(const cindex& index) const;

	void
	drop_action_push_pbb(const cindex& index);

	bool
	has_action_push_pbb(const cindex& index) const;


	// ActionPopPbb

	cofaction_pop_pbb&
	add_action_pop_pbb(const cindex& index);

	cofaction_pop_pbb&
	set_action_pop_pbb(const cindex& index);

	const cofaction_pop_pbb&
	get_action_pop_pbb(const cindex& index) const;

	void
	drop_action_pop_pbb(const cindex& index);

	bool
	has_action_pop_pbb(const cindex& index) const;


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofactions const& actions) {
		os << rofl::indent(0) << "<cofactions ofp-version:" << (int)actions.get_version() <<
				" #actions:" << (int)actions.get_actions().size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<cindex, cofaction*>::const_iterator
				it = actions.actions.begin(); it != actions.actions.end(); ++it) {
			cofaction::dump(os, *(it->second));
		}
		return os;
	};

private:

	uint8_t 								ofp_version;
	std::map<cindex, cofaction*>			actions;
	std::map<cindex, unsigned int>			actions_index;

};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
