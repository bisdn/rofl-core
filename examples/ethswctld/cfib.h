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

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include <rofl/common/crofbase.h>
#include <rofl/common/crofdpt.h>
#include <rofl/common/cmacaddr.h>

#include <cfibentry.h>

namespace etherswitch
{

class eFibBase			: public std::exception {};
class eFibBusy			: public eFibBase {};
class eFibInval			: public eFibBase {};
class eFibExists		: public eFibBase {};
class eFibNotFound		: public eFibBase {};

class cfib :
		public cfibtable
{
	static std::map<uint64_t, cfib*> fibs;

public:

	static cfib&
	get_fib(uint64_t dpid);

private:

	uint64_t 								dpid;
	rofl::crofbase							*rofbase;
	rofl::crofdpt							*dpt;
	std::map<rofl::cmacaddr, cfibentry*>	fibtable;

public:

	/**
	 *
	 */
	cfib(uint64_t dpid);

	/**
	 *
	 */
	virtual
	~cfib();

	/**
	 *
	 */
	void
	dpt_bind(rofl::crofbase *rofbase, rofl::crofdpt *dpt);

	/**
	 *
	 */
	void
	dpt_release(rofl::crofbase *rofbase = 0, rofl::crofdpt *dpt = 0);

	/**
	 *
	 */
	void
	fib_update(
			rofl::crofbase *rofbase,
			rofl::crofdpt& dpt,
			rofl::cmacaddr const& src,
			uint32_t in_port);

	/**
	 *
	 */
	cfibentry&
	fib_lookup(
			rofl::crofbase *rofbase,
			rofl::crofdpt& dpt,
			rofl::cmacaddr const& dst,
			rofl::cmacaddr const& src,
			uint32_t in_port);

	/**
	 *
	 */
	void
	clear();


private:

	friend class cfibentry;

	/**
	 *
	 */
	virtual void
	fib_timer_expired(cfibentry *entry);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cfib const& fib)
	{
		os << "fib<";
			os << "dpid: " << fib.dpid << " ";
		os << ">";
		if (not fib.fibtable.empty()) { os << std::endl; }
		std::map<rofl::cmacaddr, cfibentry*>::const_iterator it;
		for (it = fib.fibtable.begin(); it != fib.fibtable.end(); ++it) {
			os << "\t" << *(it->second) << std::endl;
		}
		return os;
	};
};

}; // end of namespace

#endif /* CFIB_H_ */
