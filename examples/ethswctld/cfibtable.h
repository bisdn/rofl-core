/*
 * cfibtable.h
 *
 *  Created on: 15.07.2013
 *      Author: andreas
 */

#ifndef CFIB_H_
#define CFIB_H_ 1

#include <map>
#include <ostream>
#include <exception>
#include <inttypes.h>

#include <rofl/common/crofbase.h>
#include <rofl/common/crofdpt.h>
#include <rofl/common/caddress.h>

#include "cfibentry.h"

namespace rofl {
namespace examples {
namespace ethswctld {

/**
 * @ingroup common_howto_ethswctld
 *
 * @brief	Forwarding Information Base
 *
 * This class stores all active mappings between a host ethernet address and
 * its associated port on the switch for reaching this host on a given datapath element.
 * Class cfibentry defines the container for an active FIB entry. Class cfibtable
 * defines two groups of methods:
 *
 * 1. Static methods on CRUD operations for instances of class cfibtable
 *
 * 2. Methods on CRUD operations for instances of class cfibentry in a cfibtable instance
 *
 * @see cfibentry
 * @see cfibentry_env
 */
class cfibtable : public cfibentry_env {
public:

	/**
	 * @name	Methods for managing Forwarding Information Bases
	 */

	/**@{*/

	/**
	 * @brief	Returns reference to new or existing and resetted cfibtable instance.
	 *
	 * This method creates a new or resets an existing instance of
	 * class cfibtable for the given datapath handle.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 */
	static
	cfibtable&
	add_fib(
			const rofl::cdptid& dptid) {
		if (cfibtable::fibtables.find(dptid) != cfibtable::fibtables.end()) {
			delete cfibtable::fibtables[dptid];
			cfibtable::fibtables.erase(dptid);
		}
		new cfibtable(dptid);
		return *(cfibtable::fibtables[dptid]);
	};

	/**
	 * @brief	Returns reference to existing cfibtable instance or creates a new empty one.
	 *
	 * This method returns a reference to an existing cfibtable instance.
	 * If no instance for the given identifier is found, a new instance
	 * is created.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 */
	static
	cfibtable&
	set_fib(
			const rofl::cdptid& dptid) {
		if (cfibtable::fibtables.find(dptid) == cfibtable::fibtables.end()) {
			new cfibtable(dptid);
		}
		return *(cfibtable::fibtables[dptid]);
	};

	/**
	 * @brief	Returns const reference to existing cfibtable instance or throws exception.
	 *
	 * This method returns a const reference to an existing cfibtable instance.
	 * If no instance for the given identifier is found, an exception of type
	 * eFibNotFound is thrown.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 * @exception eFibNotFound
	 */
	static
	const cfibtable&
	get_fib(
			const rofl::cdptid& dptid) {
		if (cfibtable::fibtables.find(dptid) == cfibtable::fibtables.end()) {
			throw exceptions::eFibNotFound("cfibtable::get_fib() dptid not found");
		}
		return *(cfibtable::fibtables.at(dptid));
	};

	/**
	 * @brief	Removes an existing cfibtable instance.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 */
	static
	void
	drop_fib(
			const rofl::cdptid& dptid) {
		if (cfibtable::fibtables.find(dptid) == cfibtable::fibtables.end()) {
			return;
		}
		delete cfibtable::fibtables[dptid];
		cfibtable::fibtables.erase(dptid);
	};

	/**
	 * @brief	Checks existence of cfibtable for given identifier.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 */
	static
	bool
	has_fib(
			const rofl::cdptid& dptid) {
		return (not (cfibtable::fibtables.find(dptid) == cfibtable::fibtables.end()));
	};

	/**@}*/

public:

	/**
	 * @name	Methods for CRUD operations on FIB entries
	 */

	/**@{*/

	/**
	 * @brief	Deletes all entries stored in this cfibtable instance.
	 */
	void
	clear() {
		for (std::map<rofl::caddress_ll, cfibentry*>::iterator
				it = ftable.begin(); it != ftable.end(); ++it) {
			delete it->second;
		}
		ftable.clear();
	};

	/**
	 * @brief	Returns reference to empty cfibentry instance for given hardware address.
	 *
	 * Creates a new cfibentry instance or resets an already existing one for the
	 * given identifier.
	 *
	 * @param hwaddr ethernet hardware address
	 * @param portno host is reachable via port using this OpenFlow port number
	 * @exception eFibInval hardware address validation failed
	 */
	cfibentry&
	add_fib_entry(
			const rofl::caddress_ll& hwaddr,
			uint32_t portno) {
		if (hwaddr.is_multicast() || hwaddr.is_null()) {
			throw exceptions::eFibInval("cfibtable::add_fib_entry() hwaddr validation failed");
		}
		if (ftable.find(hwaddr) != ftable.end()) {
			drop_fib_entry(hwaddr);
		}
		ftable[hwaddr] = new cfibentry(this, dptid, hwaddr, portno);
		return *(ftable[hwaddr]);
	};

	/**
	 * @brief	Returns reference to existing cfibentry instance for given hardware address.
	 *
	 * Returns reference to existing cfibentry for given identifier or creates
	 * new one if non exists yet.
	 *
	 * @param hwaddr ethernet hardware address
	 * @param portno host is reachable via port using this OpenFlow port number
	 * @exception eFibInval hardware address validation failed
	 */
	cfibentry&
	set_fib_entry(
			const rofl::caddress_ll& hwaddr,
			uint32_t portno) {
		if (hwaddr.is_multicast() || hwaddr.is_null()) {
			throw exceptions::eFibInval("cfibtable::set_fib_entry() hwaddr validation failed");
		}
		if (ftable.find(hwaddr) == ftable.end()) {
			ftable[hwaddr] = new cfibentry(this, dptid, hwaddr, portno);
		}
		return *(ftable[hwaddr]);
	};

	/**
	 * @brief	Returns reference to existing cfibentry instance for given hardware address.
	 *
	 * Returns reference to existing cfibentry for given identifier or
	 * throw exception, if none exists.
	 *
	 * @param hwaddr ethernet hardware address
	 * @exception eFibInval hardware address validation failed
	 * @exception eFibNotFound no cfibentry for hardware address found
	 */
	cfibentry&
	set_fib_entry(
			const rofl::caddress_ll& hwaddr) {
		if (hwaddr.is_multicast() || hwaddr.is_null()) {
			throw exceptions::eFibInval("cfibtable::set_fib_entry() hwaddr validation failed");
		}
		if (ftable.find(hwaddr) == ftable.end()) {
			throw exceptions::eFibNotFound("cfibtable::set_fib_entry() hwaddr not found");
		}
		return *(ftable[hwaddr]);
	};

	/**
	 * @brief	Returns const reference to existing cfibentry instance for given hardware address.
	 *
	 * Returns const reference to existing cfibentry for given identifier or
	 * throw exception, if none exists.
	 *
	 * @param hwaddr ethernet hardware address
	 * @exception eFibInval hardware address validation failed
	 * @exception eFibNotFound no cfibentry for hardware address found
	 */
	const cfibentry&
	get_fib_entry(
			const rofl::caddress_ll& hwaddr) const {
		if (hwaddr.is_multicast() || hwaddr.is_null()) {
			throw exceptions::eFibInval("cfibtable::get_fib_entry() hwaddr validation failed");
		}
		if (ftable.find(hwaddr) == ftable.end()) {
			throw exceptions::eFibNotFound("cfibtable::set_fib_entry() hwaddr not found");
		}
		return *(ftable.at(hwaddr));
	};

	/**
	 * @brief 	Removes an existing cfibentry for given hardware address.
	 *
	 * @param hwaddr ethernet hardware address
	 */
	void
	drop_fib_entry(
			const rofl::caddress_ll& hwaddr) {
		if (ftable.find(hwaddr) == ftable.end()) {
			return;
		}
		cfibentry* fibentry = ftable[hwaddr];
		ftable.erase(hwaddr);
		delete fibentry;
	};

	/**
	 * @brief	Checks whether a cfibentry exists for given hardware address.
	 *
	 * @param hwaddr ethernet hardware address
	 */
	bool
	has_fib_entry(
			const rofl::caddress_ll& hwaddr) const {
		return (not (ftable.find(hwaddr) == ftable.end()));
	};

	/**@}*/

private:

	/**
	 * @brief	cfibtable constructor
	 */
	cfibtable(
			const rofl::cdptid& dptid) :
		dptid(dptid) {
		cfibtable::fibtables[dptid] = this;
	};

	/**
	 * @brief	cfibtable destructor
	 */
	virtual
	~cfibtable() {
		clear();
		cfibtable::fibtables.erase(dptid);
	};

private:

	/*
	 * methods overwritten from cfibentry_env
	 */

	/**
	 * @brief	Called when a cfibentry's timer has expired.
	 */
	virtual void
	fib_timer_expired(
			const rofl::caddress_ll& hwaddr)
	{ drop_fib_entry(hwaddr); };

	/**
	 * @brief	Called when a cfibentry's assigned port has changed.
	 */
	virtual void
	fib_port_update(
			const cfibentry& entry)
	{};

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cfibtable const& fib) {
		try {
			os << rofl::indent(0) << "<cfibtable dpid: "
					<< rofl::crofdpt::get_dpt(fib.dptid).get_dpid().str() << " >" << std::endl;
		} catch (rofl::eRofDptNotFound& e) {
			os << rofl::indent(0) << "<cfibtable dptid:" << fib.dptid.str() << " >" << std::endl;
		}
		rofl::indent i(2);
		for (std::map<rofl::caddress_ll, cfibentry*>::const_iterator
				it = fib.ftable.begin(); it != fib.ftable.end(); ++it) {
			os << *(it->second);
		}
		return os;
	};

private:

	static std::map<rofl::cdptid, cfibtable*> fibtables;
	rofl::cdptid dptid;
	std::map<rofl::caddress_ll, cfibentry*> ftable;
};

}; // namespace ethswctld
}; // namespace examples
}; // namespace rofl

#endif /* CFIB_H_ */
