/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cflowtable.h
 *
 *  Created on: 15.08.2014
 *      Author: andreas
 */

#ifndef CFLOWTABLE_H_
#define CFLOWTABLE_H_

#include <map>
#include <ostream>
#include <exception>
#include <inttypes.h>

#include <rofl/common/crofbase.h>
#include <rofl/common/crofdpt.h>
#include <rofl/common/caddress.h>

#include "cflowentry.h"

namespace rofl {
namespace examples {
namespace ethswctld {

/**
 * @ingroup common_howto_ethswctld
 *
 * @brief	Flow Table of active flows
 *
 * This class stores all active flows created on a given datapath element.
 * Class cflowentry defines the container for an active flow. Class cflowtable
 * defines two groups of methods:
 *
 * 1. Static methods on CRUD operations for instances of class cflowtable
 *
 * 2. Methods on CRUD operations for instances of class cflowentry in a cflowtable instance
 *
 * @see cflowentry
 * @see cflowentry_env
 */
class cflowtable : public cflowentry_env {
public:

	/**
	 * @name	Methods for managing Active Flow Tables
	 */

	/**@{*/

	/**
	 * @brief	Returns reference to new or existing and resetted cflowtable instance.
	 *
	 * This method creates a new or resets an existing instance of
	 * class cflowtable for the given datapath handle.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 */
	static
	cflowtable&
	add_flowtable(
			const rofl::cdptid& dptid) {
		if (cflowtable::flowtables.find(dptid) != cflowtable::flowtables.end()) {
			delete cflowtable::flowtables[dptid];
			cflowtable::flowtables.erase(dptid);
		}
		new cflowtable(dptid);
		return *(cflowtable::flowtables[dptid]);
	};

	/**
	 * @brief	Returns reference to existing cflowtable instance or creates a new empty one.
	 *
	 * This method returns a reference to an existing cflowtable instance.
	 * If no instance for the given identifier is found, a new instance
	 * is created.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 */
	static
	cflowtable&
	set_flowtable(
			const rofl::cdptid& dptid) {
		if (cflowtable::flowtables.find(dptid) == cflowtable::flowtables.end()) {
			new cflowtable(dptid);
		}
		return *(cflowtable::flowtables[dptid]);
	};

	/**
	 * @brief	Returns const reference to existing cflowtable instance or throws exception.
	 *
	 * This method returns a const reference to an existing cflowtable instance.
	 * If no instance for the given identifier is found, an exception of type
	 * eFibNotFound is thrown.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 * @exception eFibNotFound
	 */
	static
	const cflowtable&
	get_flowtable(
			const rofl::cdptid& dptid) {
		if (cflowtable::flowtables.find(dptid) == cflowtable::flowtables.end()) {
			throw exceptions::eFlowNotFound("cflowtable::get_flowtable() dptid not found");
		}
		return *(cflowtable::flowtables.at(dptid));
	};

	/**
	 * @brief	Removes an existing cflowtable instance.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 */
	static
	void
	drop_flowtable(
			const rofl::cdptid& dptid) {
		if (cflowtable::flowtables.find(dptid) == cflowtable::flowtables.end()) {
			return;
		}
		delete cflowtable::flowtables[dptid];
		cflowtable::flowtables.erase(dptid);
	};

	/**
	 * @brief	Checks existence of cflowtable for given identifier.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 */
	static
	bool
	has_flowtable(
			const rofl::cdptid& dptid) {
		return (not (cflowtable::flowtables.find(dptid) == cflowtable::flowtables.end()));
	};

	/**@}*/

public:

	/**
	 * @name	Methods for CRUD operations on active flow entries.
	 */

	/**@{*/

	/**
	 * @brief	Deletes all entries stored in this cflowtable instance.
	 */
	void
	clear() {
		for (std::map<rofl::caddress_ll, std::map<rofl::caddress_ll, cflowentry*> >::iterator
				it = ftable.begin(); it != ftable.end(); ++it) {
			for (std::map<rofl::caddress_ll, cflowentry*>::iterator
					jt = it->second.begin(); jt != it->second.end(); ++jt) {
				delete jt->second;
			}
		}
		ftable.clear();
	};

	/**
 	 * @brief	Returns reference to empty cflowentry instance for given hardware address.
	 *
	 * Creates a new cflowentry instance or resets an already existing one for the
	 * given identifier.
	 *
	 * @param src ethernet hardware address for source station of flow
	 * @param dst ethernet hardware address for destination station of flow
	 * @param portno outgoing port for this flow
	 * @exception eFlowInval hardware address validation failed
	 */
	cflowentry&
	add_flow_entry(
			const rofl::caddress_ll& src,
			const rofl::caddress_ll& dst,
			uint32_t portno) {
		if (src.is_multicast() || src.is_null() || dst.is_multicast() || dst.is_null()) {
			throw exceptions::eFlowInval("cflowtable::add_flow_entry() invalid address");
		}
		if (ftable[src].find(dst) != ftable[src].end()) {
			drop_flow_entry(src, dst);
		}
		ftable[src][dst] = new cflowentry(this, dptid, src, dst, portno);
		return *(ftable[src][dst]);
	};

	/**
 	 * @brief	Returns reference to existing cflowentry instance for given hardware address.
	 *
	 * Returns reference to existing cflowentry for given identifier or creates
	 * new one if non exists yet.
	 *
	 * @param src ethernet hardware address for source station of flow
	 * @param dst ethernet hardware address for destination station of flow
	 * @param portno outgoing port for this flow
	 * @exception eFlowInval hardware address validation failed
	 */
	cflowentry&
	set_flow_entry(
			const rofl::caddress_ll& src,
			const rofl::caddress_ll& dst,
			uint32_t portno) {
		if (src.is_multicast() || src.is_null() || dst.is_multicast() || dst.is_null()) {
			throw exceptions::eFlowInval("cflowtable::set_flow_entry() invalid address");
		}
		if (ftable[src].find(dst) == ftable[src].end()) {
			ftable[src][dst] = new cflowentry(this, dptid, src, dst, portno);
		}
		return *(ftable[src][dst]);
	};

	/**
	 * @brief	Returns reference to existing cflowentry instance for given hardware address.
	 *
	 * Returns reference to existing cflowentry for given identifier or
	 * throw exception, if none exists.
	 *
	 * @param src ethernet hardware address for source station of flow
	 * @param dst ethernet hardware address for destination station of flow
	 * @exception eFlowInval hardware address validation failed
	 * @exception eFlowNotFound flow destination address not found
	 */
	cflowentry&
	set_flow_entry(
			const rofl::caddress_ll& src,
			const rofl::caddress_ll& dst) {
		if (src.is_multicast() || src.is_null() || dst.is_multicast() || dst.is_null()) {
			throw exceptions::eFlowInval("cflowtable::set_flow_entry() invalid address");
		}
		if (ftable[src].find(dst) == ftable[src].end()) {
			throw exceptions::eFlowNotFound("cflowtable::set_flow_entry() destination address not found");
		}
		return *(ftable[src][dst]);
	};

	/**
	 * @brief	Returns const reference to existing cflowentry instance for given hardware address.
	 *
	 * Returns reference to existing cflowentry for given identifier or
	 * throw exception, if none exists.
	 *
	 * @param src ethernet hardware address for source station of flow
	 * @param dst ethernet hardware address for destination station of flow
	 * @exception eFlowInval hardware address validation failed
	 * @exception eFlowNotFound flow destination address not found
	 */
	const cflowentry&
	get_flow_entry(
			const rofl::caddress_ll& src,
			const rofl::caddress_ll& dst) const {
		if (src.is_multicast() || src.is_null() || dst.is_multicast() || dst.is_null()) {
			throw exceptions::eFlowInval("cflowtable::get_flow_entry() invalid address");
		}
		if (ftable.at(src).find(dst) == ftable.at(src).end()) {
			throw exceptions::eFlowNotFound("cflowtable::get_flow_entry() destination address not found");
		}
		return *(ftable.at(src).at(dst));
	};

	/**
	 * @brief 	Removes an existing cflowentry for given source and destination hardware address.
	 *
	 * @param src ethernet hardware address for source station of flow
	 * @param dst ethernet hardware address for destination station of flow
	 */
	void
	drop_flow_entry(
			const rofl::caddress_ll& src,
			const rofl::caddress_ll& dst) {
		if (ftable[src].find(dst) == ftable[src].end()) {
			return;
		}
		cflowentry* flowentry = ftable[src][dst];
		ftable[src].erase(dst);
		delete flowentry;
	};

	/**
	 * @brief	Checks whether a cflowentry exists for given source and destination hardware address.
	 *
	 * @param src ethernet hardware address for source station of flow
	 * @param dst ethernet hardware address for destination station of flow
	 */
	bool
	has_flow_entry(const rofl::caddress_ll& src, const rofl::caddress_ll& dst) const {
		return (not (ftable.at(src).find(dst) == ftable.at(src).end()));
	};

	/**@}*/

private:

	/**
	 * @brief	cflowtable constructor for given datapath handle
	 */
	cflowtable(
			const rofl::cdptid& dptid) :
				dptid(dptid) {
		cflowtable::flowtables[dptid] = this;
	};

	/**
	 * @brief	cflowtable destructor
	 */
	virtual
	~cflowtable() {
		clear();
		cflowtable::flowtables.erase(dptid);
	};

private:

	/**
	 * @brief	Called once the timer for a flow entry has expired.
	 */
	virtual void
	flow_timer_expired(
			const cflowentry& entry) {
		drop_flow_entry(entry.get_src(), entry.get_dst());
	};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cflowtable const& flowtable) {
		try {
			os << rofl::indent(0) << "<cflowtable dpid:"
					<< rofl::crofdpt::get_dpt(flowtable.dptid).get_dpid().str() << " >" << std::endl;
		} catch (rofl::eRofDptNotFound& e) {
			os << rofl::indent(0) << "<cflowtable dptid:" << flowtable.dptid << " >" << std::endl;
		}
		rofl::indent i(2);
		for (std::map<rofl::caddress_ll, std::map<rofl::caddress_ll, cflowentry*> >::const_iterator
				it = flowtable.ftable.begin(); it != flowtable.ftable.end(); ++it) {
			for (std::map<rofl::caddress_ll, cflowentry*>::const_iterator
					jt = it->second.begin(); jt != it->second.end(); ++jt) {
				os << *(jt->second);
			}
		}
		return os;
	};

private:

	rofl::cdptid dptid;
	std::map<rofl::caddress_ll, std::map<rofl::caddress_ll, cflowentry*> > ftable;
	static std::map<rofl::cdptid, cflowtable*> flowtables;
};

}; // namespace ethswctld
}; // namespace examples
}; // namespace rofl

#endif /* CFLOWTABLE_H_ */
