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

namespace exceptions {
/**
 * @ingroup common_howto_ethswctld
 * @exception eFlowBase
 *
 * @brief	Base class for all exceptions thrown by class cflowtable.
 */
class eFlowBase         : public std::runtime_error {
public:
	eFlowBase(const std::string& __arg) : std::runtime_error(__arg) {};
};

/**
 * @ingroup common_howto_ethswctld
 * @exception eFlowInval
 *
 * @brief	Invalid parameter specified.
 */
class eFlowInval        : public eFlowBase {
public:
	eFlowInval(const std::string& __arg) : eFlowBase(__arg) {};
};

/**
 * @ingroup common_howto_ethswctld
 * @exception eFlowNotFound
 *
 * @brief	Element not found.
 */
class eFlowNotFound     : public eFlowBase {
public:
	eFlowNotFound(const std::string& __arg) : eFlowBase(__arg) {};
};

}; // namespace exceptions



class cflowentry; // forward declaration

/**
 * @ingroup common_howto_ethswctld
 * @interface cflowentry_env
 *
 * @brief	Defines the environment expected by an instance of class cflowentry.
 */
class cflowentry_env {
	friend class cflowentry;
public:

	/**
	 * @brief	cflowentry_env destructor
	 */
	virtual
	~cflowentry_env()
	{};

protected:

	/**
	 * @brief	Called once the timer for this flow entry has expired.
	 */
	virtual void
	flow_timer_expired(
			const cflowentry& entry) = 0;
};


/**
 * @ingroup common_howto_ethswctld
 *
 * @brief	Stores an active flow entry.
 *
 * This class stores and controls an active flow entry on the attached
 * datapath element. A flow entry is unidirectional and consists of
 * a source and a destination host identified by their respective
 * ethernet hardware addresses. The installed flow entry on the datapath
 * element forwards frames via the specified outgoing port. Class
 * cflowentry_env defines the environment expected by an instance
 * of class cflowentry. A flow entry is a soft-state entity and installs
 * a timer upon creation
 *
 * @see cflowentry_env
 */
class cflowentry : public rofl::ciosrv {
public:

	/**
	 * @brief	cflowentry constructor
	 *
	 * @param env environment for this cflowentry instance
	 * @param dptid rofl-common's internal datapath handle
	 * @param src ethernet hardware address used by source station
	 * @param dst ethernet hardware address used by destination station
	 * @param port_no OpenFlow port number of port pointing towards the station
	 */
	cflowentry(
			cflowentry_env *flowenv,
			const rofl::cdptid& dptid,
			const rofl::caddress_ll& src,
			const rofl::caddress_ll& dst,
			uint32_t port_no);

	/**
	 * @brief	cflowentry destructor
	 */
	virtual
	~cflowentry();

public:

	/**
	 * @name	Access to class parameters
	 */

	/**@{*/

	/**
	 * @brief	Returns outgoing port number stored for this flow
	 *
	 * @return OpenFlow port number of port pointing towards destination
	 */
	uint32_t
	get_out_port_no() const
	{ return port_no; };

	/**
	 * @brief	Update port number stored for this flow
	 *
	 * @param port_no new OpenFlow port number of port pointing towards destination
	 */
	void
	set_out_port_no(
			uint32_t out_port_no);

	/**
	 * @brief	Returns ethernet hardware address identifying the destination host
	 *
	 * @return host ethernet hardware address of destination host
	 */
	const rofl::caddress_ll&
	get_dst() const
	{ return dst; };

	/**
	 * @brief	Returns ethernet hardware address identifying the source host
	 *
	 * @return host ethernet hardware address of source host
	 */
	const rofl::caddress_ll&
	get_src() const
	{ return src; };

	/**@}*/

private:

	void
	flow_mod_add();

	void
	flow_mod_delete();

	void
	flow_mod_modify();

private:

	virtual void
	handle_timeout(
			int opaque,
			void* data = (void*)NULL);

public:

	/**
	 * @brief	Output operator
	 */
	friend std::ostream&
	operator<< (std::ostream& os, const cflowentry& entry) {
		os << rofl::indent(0) << "<cflowentry portno: " << (unsigned int)entry.port_no << " >" << std::endl;
		rofl::indent i(2);
		os << entry.src;
		os << entry.dst;
		return os;
	};

private:

	static const long CFLOWENTRY_DEFAULT_TIMEOUT = 60;

	enum cflowentry_timer_t {
		CFLOWENTRY_ENTRY_EXPIRED = 1,
	};

	cflowentry_env*     env;
	rofl::cdptid        dptid;
	uint32_t            port_no;
	rofl::caddress_ll   src;
	rofl::caddress_ll   dst;
	int                 entry_timeout;
	rofl::ctimerid      expiration_timer_id;
};

}; // namespace ethswctld
}; // namespace examples
}; // namespace rofl

#endif /* CFLOW_H_ */
