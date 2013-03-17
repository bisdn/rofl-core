/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CPORT_H
#define CPORT_H 1

#include <algorithm>
#include <string>
#include <bitset>
#include <set>
#include <map>

#include "ciosrv.h"
#include "cpacket.h"
#include "cerror.h"
#include "cmacaddr.h"
#include "thread_helper.h"
#include "cvastring.h"
#include "openflow/cofportstats.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "openflow/openflow12.h"
#include <pthread.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <endian.h>

#ifdef __cplusplus
}
#endif

#include "rofl/platform/unix/crandom.h"

namespace rofl
{

#define SIOCBASE 0x01

class ePortBase : // base error class cport
	public cerror
{
public:
	ePortBase(std::string desc = std::string("")) :
		cerror(desc) {};
};
class ePortNotOpen : // port is not operational
	public ePortBase
{
public:
	ePortNotOpen(std::string desc = std::string("")) :
		ePortBase(desc) {};
};
class ePortNotFound 		: public ePortBase {}; 	// port not found
class ePortIsAttached   	: public ePortBase {}; 	// port is already attached
class ePortNotAttached		: public ePortBase {}; 	// port is not attached to this datapath
class ePortInval			: public ePortBase {}; 	// invalid parameter
class ePortSetHwAddrFailed 	: public ePortBase {}; 	// setting hardware address failed
class ePortOutOfMem 		: public ePortBase {}; 	// out of memory
class ePortSocketCallFailed : public ePortBase {}; 	// socket() system call failed
class ePortIoctlCallFailed 	: public ePortBase {}; 	// ioctl() system call failed

class ePortOpenFailed : // port initialization failed
	public ePortBase
{
public:
	ePortOpenFailed(std::string desc = std::string("")) :
		ePortBase(desc) {};
};
class ePortCloseFailed	:  // port closing failed
	public ePortBase
{
public:
	ePortCloseFailed(std::string desc = std::string("")) :
		ePortBase(desc) {};
};





class cport; // forward declaration, see below




/** callback abstract base class for cport
 *
 */
class cport_owner
{
public:


	/**
	 *
	 */
	virtual
	~cport_owner() {};


	/**
	 *
	 */
	virtual void
	port_init(
			cport *port) = 0;


	/**
	 *
	 */
	virtual void
	port_open(
			cport *port) = 0;


	/**
	 *
	 */
	virtual void
	port_close(
			cport *port) = 0;


	/**
	 *
	 */
	virtual void
	port_destroy(
			cport *port) = 0;


	/**
	 *
	 */
	virtual void
	enqueue(
			cport *port,
			cpacket* pack) = 0;

	/**
	 *
	 */
	virtual void
	enqueue(
			cport *port,
			std::deque<cpacket*>& packets) = 0;
};











class cport :
	public ciosrv
{
private: // data structures

	std::string 				info;
	cofport_stats_reply			port_stats;

protected: // data structures

	enum cport_timer_t { // cport related timer types
		CPORT_TIMER_BASE = (0x0032 << 16),
		CPORT_TIMER_POUT_QUEUE,
	};

	enum cport_event_t {
		CPORT_EVENT_OUT_QUEUE = 0xfeedbacc,
	};

	enum cport_flag_t {
		CPORT_FLAG_POUT_QUEUE = (1 << 0),
		PORT_IFF_UP = (1 << 1),	// logical IFF_UP flag is set on this port
		PORT_DESTROY_UPON_DETACHING = (1 << 2),
	};

#define CPORT_DEFAULT_MAX_OUT_QUEUE_SIZE	100
	unsigned int				max_out_queue_size;

	cport_owner 				*owner;			// owner of this cport (the one who gets packets from us)
	std::deque<cpacket*>		pout_queue;

public: // data structures

	static std::set<cport*> 	cport_list; 	// static list of all basic cports

	// values for struct ofp_phy_port
	std::string 				devname;    // port name
	std::string 				devtype; 	// port type ("phy", "bcm", "vport", rofl.)

#if 0
	// port statistics
	uint64_t 					rx_packets;
	uint64_t 					tx_packets;
	uint64_t 					rx_bytes;
	uint64_t 					tx_bytes;
	uint64_t 					rx_dropped;
	uint64_t 					tx_dropped;
	uint64_t 					rx_errors;
	uint64_t 					tx_errors;
	uint64_t 					rx_frame_err;
	uint64_t 					rx_over_err;
	uint64_t 					rx_crc_err;
	uint64_t 					collisions;
#endif




public: // class wide static methods and data structures


	/**
	 * finds cport based on devname
	 * @return cport* pointer to cport instance
	 */
	static cport*
	find(
			std::string const& devname)
		throw (ePortNotFound);



public: // per instance methods


	/** constructor
	 */
	cport(
			cport_owner *owner,
			std::string devname,
			std::string devtype);


	/** destructor
	 */
	virtual
	~cport();


public:


	/**
	 * enqueue an outgoing packet on this ports queue for sending it out
	 */
	void
	enqueue(
			cpacket* pkt);


	/**
	 * enqueue a list of outgoing packet on this ports queue for sending it out
	 */
	void
	enqueue(
			std::deque<cpacket*>& pktlist);


	/** enable interface (set IFF_UP flag)
	 */
	virtual void
	enable_interface()
		throw (ePortSocketCallFailed, ePortIoctlCallFailed) = 0;


	/** disable interface (clear IFF_UP flag)
	 */
	virtual void
	disable_interface()
		throw (ePortSocketCallFailed, ePortIoctlCallFailed) = 0;


	/** return c_str with cport info
	 */
	const char*
	c_str();

#if 0
	/** get port statistics
	 */
	struct ofp_port_stats*
	get_port_stats(
			struct ofp_port_stats* port_stats,
			size_t port_stats_len);
#endif

	/**
	 *
	 */
	cofport_stats_reply&
	get_port_stats();


protected: // methods

	/**
	 * handle out queue
	 */
	virtual void
	handle_out_queue() = 0;


public: // helper classes

	/** helper class for finding cport in this->port_list
	 * based on port name of this cport instance
	 */
	class cport_find_by_name : public std::unary_function<cport,bool> {
		std::string devname;
	public:
		cport_find_by_name(const std::string& _devname) :
			devname(_devname) {};
		bool operator() (cport* port) {
			return (devname == port->devname);
		}
	};
};

}; // end of namespace

#endif

