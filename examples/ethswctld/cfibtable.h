/*
 * cfib.h
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

namespace etherswitch {

class eFibBase			: public std::exception {};
class eFibBusy			: public eFibBase {};
class eFibInval			: public eFibBase {};
class eFibExists		: public eFibBase {};
class eFibNotFound		: public eFibBase {};

class cfibtable : public cfibentry_env {
public:

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
			throw eFibNotFound();
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
	 * @brief	Checks existence of cfibtable for fiven identifier.
	 *
	 * @param dptid rofl-common's internal handle for datapath element
	 */
	static
	bool
	has_fib(
			const rofl::cdptid& dptid) {
		return (not (cfibtable::fibtables.find(dptid) == cfibtable::fibtables.end()));
	};

public:

	/**
	 *
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
	 *
	 */
	cfibentry&
	add_fib_entry(const rofl::caddress_ll& hwaddr, uint32_t portno) {
		if (hwaddr.is_multicast() || hwaddr.is_null()) {
			throw eFibInval();
		}
		if (ftable.find(hwaddr) != ftable.end()) {
			drop_fib_entry(hwaddr);
		}
		ftable[hwaddr] = new cfibentry(this, dptid, hwaddr, portno);
		return *(ftable[hwaddr]);
	};

	/**
	 *
	 */
	cfibentry&
	set_fib_entry(const rofl::caddress_ll& hwaddr, uint32_t portno) {
		if (hwaddr.is_multicast() || hwaddr.is_null()) {
			throw eFibInval();
		}
		if (ftable.find(hwaddr) == ftable.end()) {
			ftable[hwaddr] = new cfibentry(this, dptid, hwaddr, portno);
		}
		return *(ftable[hwaddr]);
	};

	/**
	 *
	 */
	cfibentry&
	set_fib_entry(const rofl::caddress_ll& hwaddr) {
		if (hwaddr.is_multicast() || hwaddr.is_null()) {
			throw eFibInval();
		}
		if (ftable.find(hwaddr) == ftable.end()) {
			throw eFibNotFound();
		}
		return *(ftable[hwaddr]);
	};

	/**
	 *
	 */
	const cfibentry&
	get_fib_entry(const rofl::caddress_ll& hwaddr) const {
		if (ftable.find(hwaddr) == ftable.end()) {
			throw eFibNotFound();
		}
		return *(ftable.at(hwaddr));
	};

	/**
	 *
	 */
	void
	drop_fib_entry(const rofl::caddress_ll& hwaddr) {
		if (ftable.find(hwaddr) == ftable.end()) {
			return;
		}
		cfibentry* fibentry = ftable[hwaddr];
		ftable.erase(hwaddr);
		delete fibentry;
	};

	/**
	 *
	 */
	bool
	has_fib_entry(const rofl::caddress_ll& hwaddr) const {
		return (not (ftable.find(hwaddr) == ftable.end()));
	};

private:

	/**
	 *
	 */
	cfibtable(const rofl::cdptid& dptid) :
		dptid(dptid) {
		cfibtable::fibtables[dptid] = this;
	};

	/**
	 *
	 */
	virtual
	~cfibtable() {
		clear();
		cfibtable::fibtables.erase(dptid);
	};


	friend class cfibentry;

	/**
	 *
	 */
	virtual void
	fib_timer_expired(const rofl::caddress_ll& hwaddr) {
		drop_fib_entry(hwaddr);
	};

	/**
	 *
	 */
	virtual void
	fib_port_update(const cfibentry& entry) {

	};


public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cfibtable const& fib) {
		try {
			os << rofl::indent(0) << "<cfibtable dpid:" << "dpid: "
					<< rofl::crofdpt::get_dpt(fib.dptid).get_dpid().str() << " >" << std::endl;
		} catch (rofl::eRofDptNotFound& e) {
			os << rofl::indent(0) << "<cfibtable dptid:" << "dpid: " << fib.dptid << " >" << std::endl;
		}
		rofl::indent i(2);
		for (std::map<rofl::caddress_ll, cfibentry*>::const_iterator
				it = fib.ftable.begin(); it != fib.ftable.end(); ++it) {
			os << *(it->second);
		}
		return os;
	};

private:

	rofl::cdptid								dptid;
	std::map<rofl::caddress_ll, cfibentry*>		ftable; // hwaddr => cfibentry
	static std::map<rofl::cdptid, cfibtable*> 	fibtables;
};

}; // end of namespace

#endif /* CFIB_H_ */
