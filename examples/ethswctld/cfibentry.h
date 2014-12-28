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

namespace etherswitch {

class cfibentry; // forward declaration, see below

class cfibentry_env {
public:
	virtual ~cfibentry_env() {};
	virtual void fib_timer_expired(const rofl::caddress_ll& hwaddr) = 0;
	virtual void fib_port_update(const cfibentry& entry) = 0;
};

class cfibentry : public rofl::ciosrv {
public:

	/**
	 *
	 */
	cfibentry(
			cfibentry_env *fibenv,
			const rofl::cdptid& dptid,
			const rofl::caddress_ll& hwaddr,
			uint32_t port_no);

	/**
	 *
	 */
	virtual
	~cfibentry();

	/**
	 *
	 */
	uint32_t
	get_port_no() const { return port_no; };

	/**
	 *
	 */
	void
	set_port_no(uint32_t port_no);

	/**
	 *
	 */
	const rofl::caddress_ll&
	get_hwaddr() const { return hwaddr; };

private:

	/**
	 *
	 */
	virtual void
	handle_timeout(int opaque, void* data = (void*)NULL);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cfibentry const& entry) {
		os << rofl::indent(0) << "<cfibentry portno: "
				<< entry.port_no << " >" << std::endl;
		rofl::indent i(2);
		os << entry.hwaddr;
		return os;
	};

private:

#define CFIBENTRY_DEFAULT_TIMEOUT		60

	enum cfibentry_timer_t {
		CFIBENTRY_ENTRY_EXPIRED = 1,
	};

	cfibentry_env						*fibenv;
	rofl::cdptid				dptid;
	uint32_t					port_no;
	rofl::caddress_ll			hwaddr;
	int							entry_timeout;
	rofl::ctimerid				expiration_timer_id;
};

}; // end of namespace

#endif /* CFIBENTRY_H_ */
