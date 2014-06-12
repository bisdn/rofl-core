/*
 * cfibentry.h
 *
 *  Created on: 15.07.2013
 *      Author: andreas
 */

#ifndef CFIBENTRY_H_
#define CFIBENTRY_H_ 1

#include <ostream>
#include <inttypes.h>

#include <rofl/common/ciosrv.h>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/crofdpt.h>
#include <rofl/common/logging.h>
#include <rofl/common/ctimerid.h>

namespace etherswitch
{

class cfibentry; // forward declaration, see below

class cfibtable
{
public:
	virtual ~cfibtable() {};
	virtual void fib_timer_expired(cfibentry *entry) = 0;
};

class cfibentry :
		public rofl::ciosrv
{
#define CFIBENTRY_DEFAULT_TIMEOUT		60

private:

	enum cfibentry_timer_t {
		CFIBENTRY_ENTRY_EXPIRED = 0x99ae,
	};

	cfibtable					*fib;
	uint32_t					out_port_no;
	rofl::cmacaddr				dst;
	rofl::crofbase				*rofbase;
	rofl::crofdpt				*dpt;
	int							entry_timeout;
	rofl::ctimerid				expiration_timer_id;

public:

	/**
	 *
	 */
	cfibentry(
			cfibtable *fib,
			rofl::crofbase *rofbase,
			rofl::crofdpt *dpt,
			rofl::cmacaddr dst,
			uint32_t out_port_no);

	/**
	 *
	 */
	virtual
	~cfibentry();

	/**
	 *
	 */
	uint32_t
	get_out_port_no() const { return out_port_no; };

	/**
	 *
	 */
	void
	set_out_port_no(uint32_t out_port_no);

	/**
	 *
	 */
	rofl::cmacaddr const&
	get_lladdr() const { return dst; };

	/**
	 *
	 */
	void
	flow_mod_add();

	/**
	 *
	 */
	void
	flow_mod_delete();

	/**
	 *
	 */
	void
	flow_mod_modify();

private:

	/**
	 *
	 */
	virtual void
	handle_timeout(int opaque);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cfibentry const& entry)
	{
		os << rofl::indent(0) << "<fibentry ";
			os << "hwaddr: " << entry.dst << " ";
			os << "port: " << entry.out_port_no << " ";
		os << ">" << std::endl;
		return os;
	};
};

}; // end of namespace

#endif /* CFIBENTRY_H_ */
