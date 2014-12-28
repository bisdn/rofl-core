/*
 * cflow.h
 *
 *  Created on: 15.08.2014
 *      Author: andreas
 */

#ifndef CFLOW_H_
#define CFLOW_H_

#include <ostream>
#include <inttypes.h>

#include <rofl/common/ciosrv.h>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/crofdpt.h>
#include <rofl/common/logging.h>
#include <rofl/common/ctimerid.h>

namespace rofl {
namespace examples {
namespace ethswctld {

class cflowentry; // forward declaration, see below

class cflowentry_env {
	friend class cflowentry;
public:
	/**
	 *
	 */
	virtual
	~cflowentry_env()
	{};
protected:
	/**
	 *
	 */
	virtual void
	flow_timer_expired(
			const cflowentry& entry) = 0;
};

class cflowentry : public rofl::ciosrv {
public:

	/**
	 *
	 */
	cflowentry(
			cflowentry_env *flowenv,
			const rofl::cdptid& dptid,
			const rofl::caddress_ll& src,
			const rofl::caddress_ll& dst,
			uint32_t port_no);

	/**
	 *
	 */
	virtual
	~cflowentry();

	/**
	 *
	 */
	uint32_t
	get_out_port_no() const { return port_no; };

	/**
	 *
	 */
	void
	set_port_no(uint32_t out_port_no);

	/**
	 *
	 */
	const rofl::caddress_ll&
	get_dst() const { return dst; };

	/**
	 *
	 */
	const rofl::caddress_ll&
	get_src() const { return src; };

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
	handle_timeout(int opaque, void* data = (void*)NULL);

public:

	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cflowentry const& entry) {
		os << rofl::indent(0) << "<cflowentry portno: " << (unsigned int)entry.port_no << " >" << std::endl;
		rofl::indent i(2);
		os << entry.src;
		os << entry.dst;
		return os;
	};

private:

#define CFIBENTRY_DEFAULT_TIMEOUT		60

	enum cflowentry_timer_t {
		CFIBENTRY_ENTRY_EXPIRED = 1,
	};

	cflowentry_env					*flowenv;
	rofl::cdptid				dptid;
	uint32_t					port_no;
	rofl::caddress_ll			src;
	rofl::caddress_ll			dst;
	int							entry_timeout;
	rofl::ctimerid				expiration_timer_id;
};

}; // namespace ethswctld
}; // namespace examples
}; // namespace rofl

#endif /* CFLOW_H_ */
