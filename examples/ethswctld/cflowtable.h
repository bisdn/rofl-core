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

namespace etherswitch {

class eFlowBase			: public std::exception {};
class eFlowBusy			: public eFlowBase {};
class eFlowInval			: public eFlowBase {};
class eFlowExists		: public eFlowBase {};
class eFlowNotFound		: public eFlowBase {};

class cflowtable : public cflowenv {
public:

	/**
	 *
	 */
	static cflowtable&
	get_flowtable(const rofl::cdptid& dptid) {
		if (cflowtable::flowtables.find(dptid) == cflowtable::flowtables.end()) {
			new cflowtable(dptid);
		}
		return *(cflowtable::flowtables[dptid]);
	};

public:

	/**
	 *
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
	 *
	 */
	cflowentry&
	add_flow_entry(const rofl::caddress_ll& src, const rofl::caddress_ll& dst, uint32_t portno) {
		if (src.is_multicast() || src.is_null() || dst.is_multicast() || dst.is_null()) {
			throw eFlowInval();
		}
		if (ftable[src].find(dst) != ftable[src].end()) {
			drop_flow_entry(src, dst);
		}
		ftable[src][dst] = new cflowentry(this, dptid, src, dst, portno);
		return *(ftable[src][dst]);
	};

	/**
	 *
	 */
	cflowentry&
	set_flow_entry(const rofl::caddress_ll& src, const rofl::caddress_ll& dst, uint32_t portno) {
		if (src.is_multicast() || src.is_null() || dst.is_multicast() || dst.is_null()) {
			throw eFlowInval();
		}
		if (ftable[src].find(dst) == ftable[src].end()) {
			ftable[src][dst] = new cflowentry(this, dptid, src, dst, portno);
		}
		return *(ftable[src][dst]);
	};

	/**
	 *
	 */
	cflowentry&
	set_flow_entry(const rofl::caddress_ll& src, const rofl::caddress_ll& dst) {
		if (src.is_multicast() || src.is_null() || dst.is_multicast() || dst.is_null()) {
			throw eFlowInval();
		}
		if (ftable[src].find(dst) == ftable[src].end()) {
			throw eFlowNotFound();
		}
		return *(ftable[src][dst]);
	};

	/**
	 *
	 */
	const cflowentry&
	get_flow_entry(const rofl::caddress_ll& src, const rofl::caddress_ll& dst) const {
		if (ftable.at(src).find(dst) == ftable.at(src).end()) {
			throw eFlowNotFound();
		}
		return *(ftable.at(src).at(dst));
	};

	/**
	 *
	 */
	void
	drop_flow_entry(const rofl::caddress_ll& src, const rofl::caddress_ll& dst) {
		if (ftable[src].find(dst) == ftable[src].end()) {
			return;
		}
		cflowentry* flowentry = ftable[src][dst];
		ftable[src].erase(dst);
		delete flowentry;
	};

	/**
	 *
	 */
	bool
	has_flow_entry(const rofl::caddress_ll& src, const rofl::caddress_ll& dst) const {
		return (not (ftable.at(src).find(dst) == ftable.at(src).end()));
	};

private:

	/**
	 *
	 */
	cflowtable(const rofl::cdptid& dptid) :
		dptid(dptid) {
		cflowtable::flowtables[dptid] = this;
	};

	/**
	 *
	 */
	virtual
	~cflowtable() {
		clear();
		cflowtable::flowtables.erase(dptid);
	};


	friend class cflowentry;

	/**
	 *
	 */
	virtual void
	flow_timer_expired(const cflowentry& entry) {
		drop_flow_entry(entry.get_src(), entry.get_dst());
	};


public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cflowtable const& flowtable) {
		os << rofl::indent(0) << "<cflowtable dpid: "
				<< rofl::crofdpt::get_dpt(flowtable.dptid).get_dpid_s() << " >" << std::endl;

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

	rofl::cdptid															dptid;
	std::map<rofl::caddress_ll, std::map<rofl::caddress_ll, cflowentry*> >	ftable; // src-hwaddr => dst-hwaddr => cflowentry
	static std::map<rofl::cdptid, cflowtable*> 								flowtables;
};

}; // end of namespace

#endif /* CFLOWTABLE_H_ */
