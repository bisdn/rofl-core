/*
 * cfibentry.h
 *
 *  Created on: 15.07.2013
 *      Author: andreas
 */

#ifndef CFIBENTRY_H_
#define CFIBENTRY_H_ 1

#include <ostream>

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include <rofl/common/ciosrv.h>
#include <rofl/common/cmacaddr.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/openflow/cofdpt.h>

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
	rofl::cofdpt				*dpt;
	int							entry_timeout;

public:

	/**
	 *
	 */
	cfibentry(
			cfibtable *fib,
			rofl::crofbase *rofbase,
			rofl::cofdpt *dpt,
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
		os << "fibentry<";
			os << "dst: " << entry.dst << " ";
			os << "out-port: " << entry.out_port_no << " ";
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif /* CFIBENTRY_H_ */
