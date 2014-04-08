/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFACLIST_H
#define COFACLIST_H 1

#include <inttypes.h>
#include <vector>
#include <algorithm>

#include "rofl/common/croflexception.h"
#include "rofl/common/cvastring.h"

#include "rofl/common/coflist.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofaction.h"

namespace rofl {
namespace openflow {

class eActionsBase 			: public RoflException {}; 		// base error class for cofactions
class eActionsInval 		: public eActionsBase {}; 	// parameter is invalid
class eActionsNotFound 		: public eActionsBase {}; 	// element not found
class eActionsOutOfRange 	: public eActionsBase {}; 	// index out of range in operator[]


class cofactions : public std::list<cofaction*>
{
public: // iterators

	typedef std::list<cofaction*>::iterator iterator;
	typedef std::list<cofaction*>::const_iterator const_iterator;

	typedef std::list<cofaction*>::reverse_iterator reverse_iterator;
	typedef std::list<cofaction*>::const_reverse_iterator const_reverse_iterator;

public:

	uint8_t 		ofp_version;

	/** constructor
	 */
	cofactions(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN);


	/** constructor
	 */
	cofactions(
			uint8_t ofp_version, uint8_t *achdr, size_t aclen);


	/**
	 */
	cofactions(
			cofactions const& aclist);


	/**
	 */
	cofactions&
	operator= (
			cofactions const& aclist);


	/**
	 */
	bool
	operator== (
			cofactions const& aclist);


	/** destructor
	 */
	virtual
	~cofactions();


	/**
	 *
	 */
	void
	pop_front();

	/**
	 *
	 */
	void
	pop_back();

	/**
	 *
	 */
	cofaction&
	front();

	/**
	 *
	 */
	cofaction&
	back();

	/**
	 *
	 */
	void
	clear();

	/** create a std::list<cofaction*> from a struct ofp_flow_mod
	 */
	void
	unpack(uint8_t *buf, size_t buflen);



	/** builds an array of struct ofp_instructions
	 * from a std::vector<cofinst*>
	 */
	uint8_t*
	pack(uint8_t* buf, size_t buflen);



	/** returns required length for array of struct ofp_action_headers
	 * for all actions defined in std::vector<cofaction*>
	 */
	size_t
	length() const;

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

private:


	void
	map_and_insert(cofaction const& action);


public:

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


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofactions const& actions) {
		os << indent(0) << "<cofactions ofp-version:" << (int)actions.ofp_version <<
				" #actions:" << (int)actions.size() << " >" << std::endl;
		indent i(2);
		for (std::list<cofaction*>::const_iterator
				it = actions.begin(); it != actions.end(); ++it) {
			cofaction const& action = *(*it);
			switch (action.get_type()) {
			case openflow::OFPAT_OUTPUT:
				os << dynamic_cast<cofaction_output const&>( action ); break;
			case openflow::OFPAT_SET_VLAN_VID:
				os << dynamic_cast<cofaction_set_vlan_vid const&>( action ); break;
			case openflow::OFPAT_SET_VLAN_PCP:
				os << dynamic_cast<cofaction_set_vlan_pcp const&>( action ); break;
			case openflow::OFPAT_STRIP_VLAN:
				os << dynamic_cast<cofaction_strip_vlan const&>( action ); break;
			case openflow::OFPAT_SET_DL_SRC:
				os << dynamic_cast<cofaction_set_dl_src const&>( action ); break;
			case openflow::OFPAT_SET_DL_DST:
				os << dynamic_cast<cofaction_set_dl_dst const&>( action ); break;
			case openflow::OFPAT_SET_NW_SRC:
				os << dynamic_cast<cofaction_set_nw_src const&>( action ); break;
			case openflow::OFPAT_SET_NW_DST:
				os << dynamic_cast<cofaction_set_nw_dst const&>( action ); break;
			case openflow::OFPAT_SET_NW_TOS:
				os << dynamic_cast<cofaction_set_nw_tos const&>( action ); break;
			case openflow::OFPAT_SET_TP_SRC:
				os << dynamic_cast<cofaction_set_tp_src const&>( action ); break;
			case openflow::OFPAT_SET_TP_DST:
				os << dynamic_cast<cofaction_set_tp_dst const&>( action ); break;
			case openflow::OFPAT_COPY_TTL_OUT: // = openflow10::OFPAT_ENQUEUE
				switch (actions.ofp_version) {
				case openflow10::OFP_VERSION:
					os << dynamic_cast<cofaction_enqueue const&>( action ); break;
				default:
					os << dynamic_cast<cofaction_copy_ttl_out const&>( action ); break;
				} break;
			case openflow::OFPAT_COPY_TTL_IN:
				os << dynamic_cast<cofaction_copy_ttl_in const&>( action ); break;
			case openflow::OFPAT_SET_MPLS_TTL:
				os << dynamic_cast<cofaction_set_mpls_ttl const&>( action ); break;
			case openflow::OFPAT_DEC_MPLS_TTL:
				os << dynamic_cast<cofaction_dec_mpls_ttl const&>( action ); break;
			case openflow::OFPAT_PUSH_VLAN:
				os << dynamic_cast<cofaction_push_vlan const&>( action ); break;
			case openflow::OFPAT_POP_VLAN:
				os << dynamic_cast<cofaction_pop_vlan const&>( action ); break;
			case openflow::OFPAT_PUSH_MPLS:
				os << dynamic_cast<cofaction_push_mpls const&>( action ); break;
			case openflow::OFPAT_POP_MPLS:
				os << dynamic_cast<cofaction_pop_mpls const&>( action ); break;
			case openflow::OFPAT_SET_QUEUE:
				os << dynamic_cast<cofaction_set_queue const&>( action ); break;
			case openflow::OFPAT_GROUP:
				os << dynamic_cast<cofaction_group const&>( action ); break;
			case openflow::OFPAT_SET_NW_TTL:
				os << dynamic_cast<cofaction_set_nw_ttl const&>( action ); break;
			case openflow::OFPAT_DEC_NW_TTL:
				os << dynamic_cast<cofaction_dec_nw_ttl const&>( action ); break;
			case openflow::OFPAT_SET_FIELD:
				os << dynamic_cast<cofaction_set_field const&>( action ); break;
			case openflow::OFPAT_EXPERIMENTER:
				switch (actions.ofp_version) {
				case openflow10::OFP_VERSION:
					os << dynamic_cast<cofaction_vendor const&>( action ); break;
				default:
					os << dynamic_cast<cofaction_experimenter const&>( action ); break;
				} break;
			default:
				os << action; break;
			}
		}
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif
