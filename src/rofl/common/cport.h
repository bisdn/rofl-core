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




class cport :
	public ciosrv
{
protected:

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

public: // class wide static methods and data structures

	class cport_owner; // forward declaration

	// static list of all basic cports
	static std::set<cport*> cport_list;

	/** find free port-no
	 *
	 */
	static uint32_t
	find_free_port_no(
			std::set<cport*> *port_list);

	/**
	 * finds cport based on devname
	 * @return cport* pointer to cport instance
	 */
	static cport*
	find(
			const std::string& devname) throw (ePortNotFound);

	/** returns struct ofp_port for all ports in cport_list
	 */
	static uint8_t*
	get_ofp_ports(
			uint8_t *mem,
			size_t memlen) throw (ePortOutOfMem);

public: // data structures

	// values for struct ofp_phy_port
	std::string devname;    // port name
	std::string devtype; 	// port type ("phy", "bcm", "vport", rofl.)

	uint32_t port_no;       // port number

	// port statistics
	uint64_t rx_packets;
	uint64_t tx_packets;
	uint64_t rx_bytes;
	uint64_t tx_bytes;
	uint64_t rx_dropped;
	uint64_t tx_dropped;
	uint64_t rx_errors;
	uint64_t tx_errors;
	uint64_t rx_frame_err;
	uint64_t rx_over_err;
	uint64_t rx_crc_err;
	uint64_t collisions;

public: // per instance methods

	/** constructor
	 */
	cport(
			std::string devname,
			std::string devtype,
			int port_no = 0);

	/** destructor
	 */
	virtual
	~cport();

	/** attach this port to a cport_owner
	 */
	virtual void
	attach(cport_owner *owner) throw (ePortIsAttached);

	/** detach this port from its owner
	 */
	virtual void
	detach() throw (ePortNotAttached);

	/** notification: port is now attached, called by cport_owner
	 */
	virtual void
	attached(cport_owner *owner);

	/** notification: port is now detached, called by cport_owner
	 */
	virtual void
	detached(cport_owner *owner);

public:

	/**
	 * enqueue an outgoing packet on this ports queue for sending it out
	 */
	void
	enqueue(
			cpacket* pack);

	/**
	 * handle timeout events, overwritten from class ciosrv
	 */
	virtual void
	handle_timeout(
			int opaque);

	/**
	 * handle common events, overwritten from class ciosrv
	 */
	virtual void
	handle_event(cevent const& ev);

	/** return struct ofp_phy_port for this cport
	 */
	void
	get_ofp_port(
			struct ofp_port *phy_port,
			size_t phy_port_len);

	/** enable interface (set IFF_UP flag)
	 */
	virtual void
	enable_interface() throw (ePortSocketCallFailed, ePortIoctlCallFailed) = 0;

	/** disable interface (clear IFF_UP flag)
	 */
	virtual void
	disable_interface() throw (ePortSocketCallFailed, ePortIoctlCallFailed) = 0;

	/** get port number: returns uint32_t
	 */
	virtual uint32_t
	get_port_no();

	const uint32_t
	set_port_no(const uint32_t port_no);

	/** get hw_addr: stores mac address of port in cmacaddr
	 *
	 * @param ma
	 * @return
	 */
	virtual cmacaddr&
	get_hw_addr()  throw (ePortSocketCallFailed, ePortIoctlCallFailed) = 0;

	/** set hw_addr
	 */
	virtual void
	set_hw_addr(cmacaddr const& maddr) throw (ePortSocketCallFailed, ePortInval, ePortSetHwAddrFailed) = 0;

	/** set config
	 */
	void
	set_config(
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);

	/** get config
	 */
	uint32_t
	get_config();

	/** get state
	 */
	virtual uint32_t
	get_state() throw (ePortSocketCallFailed, ePortIoctlCallFailed) = 0;

	/** get curr
	 */
	uint32_t
	get_curr();

	/** get advertised
	 */
	uint32_t
	get_advertised();

	/** get supported
	 */
	uint32_t
	get_supported();

	/** get peer
	 */
	uint32_t
	get_peer();

	/** get curr speed
	 */
	virtual uint32_t
	get_curr_speed() = 0;

	/** get max speed
	 */
	virtual uint32_t
	get_max_speed() = 0;

	/** return c_str with cport info
	 */
	const char*
	c_str();

	/** get port statistics
	 */
	struct ofp_port_stats*
	get_port_stats(
			struct ofp_port_stats* port_stats,
			size_t port_stats_len);

	/**
	 * enforce this port to (re)read its default configuration
	 */
	virtual void
	init_default_configuration() {};

protected: // data structures

	std::deque<cpacket*> pout_queue; // list of outgoing packets

	std::bitset<32> pflags; // port flags

	cmacaddr hwaddr;
	uint32_t config;      // OFPPCrofl. (port config)
	uint32_t state;
	uint32_t curr;
	uint32_t advertised;
	uint32_t supported;
	uint32_t peer;
	uint32_t curr_speed;
	uint32_t max_speed;

//	cport_config *port_configuration;

	std::string info;

protected: // methods

	/**
	 * handle out queue
	 */
	virtual void
	handle_out_queue() = 0;

	/**
	 */
	cport_owner*
	port_owner();


protected: // data structures

	pthread_mutex_t queuelock; // mutex for locking tx queue

private: // data structures

	cport_owner *powner;// owner of this cport (the one who gets packets from us)

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

	/** helper class for finding cport in this->port_list
	 * based on port_no of cport instance
	 */
	class cport_find_by_portno : public std::unary_function<cport,bool> {
		uint32_t portno;
	public:
		cport_find_by_portno(uint32_t portno) :
			portno(portno) {};
		bool operator() (cport* port) {
			return (portno == port->port_no);
		};
	};

public:

	/** callback abstract base class for cport
	 *
	 */
	class cport_owner : public virtual ciosrv {
		static std::set<cport_owner*> owner_list;
	public: // static

		enum unittest_timer_t {
			CPORT_OWNER_TIMER_BASE = (0x9b33 << 16), // base timer value
			CPORT_OWNER_TIMER_PACKET_IN,	// packet in event on cport
		};

		enum cport_owner_flag_t {
			CPORT_OWNER_FLAG_PIN_QUEUE = (1 << 0),
		};

		enum cport_owner_event_t {
			CPORT_OWNER_EVENT_PIN_QUEUE = ((0x8be3 << 16) + (1 << 0)),
		};

		std::bitset<32> flags; // port owner flags

	protected: //static
		static std::set<cport_owner*> cport_owner_list;

	public: // static

		// cport_owner exists?
		static cport_owner*
		exists(cport_owner *owner) throw (ePortNotFound)
		{
			if (cport_owner_list.find(owner) == cport_owner_list.end())
			{
#if 1
				std::set<cport_owner*>::iterator it;
				for (it = cport_owner_list.begin(); it != cport_owner_list.end(); ++it)
				{
					WRITELOG(CFWD, ROFL_DBG, "cport_owner::exists(%p) (*it): %p", owner, (*it));
				}
#endif
				throw ePortNotFound();
			}
			return owner;
		};

		/**
		 *
		 */
		const char*
		c_str();

	public: // overloaded from ciosrv

		/** Handle timeout events. This method is overwritten from ciosrv.
		 *
		 * @param opaque expired timer type
		 */
		virtual void
		handle_timeout(
				int opaque);

		/**
		 *
		 */
		virtual void
		handle_event(
				cevent const& ev);

	public:
		// constructor
		cport_owner();

		// destructor
		virtual
		~cport_owner();

		// port_attach
		virtual void
		port_attach(cport *port);

		// port_detach
		virtual void
		port_detach(cport *port);

		/** store packet in cport_owner::pin_queue
		 * this does not inform cport_owner about new packets (see enqueue() for this)
		 */
		void
		store(cport* port, cpacket* pack);

		/** send notification (=enqueue) about new packets to cport_owner
		 * this triggers consumption of pin_queue by cport_owner
		 */
		void
		enqueue(cport *port);


	protected: // data structures

		std::set<cport*> port_list; // list of attached cports

	protected: // methods

		/** handle packets received from an attached port in derived class
		 * cpacket *pack must be deleted from heap by called method
		 */
		virtual void
		handle_cport_packet_in(
				cport* port,
				std::deque<cpacket*>& pktlist) = 0; // pure virtual

		/**
		 *
		 */
		virtual void
		handle_port_attach(cport *port) = 0; // pure virtual

		/**
		 *
		 */
		virtual void
		handle_port_detach(cport *port) = 0; // pure virtual


	private: // data structures

		//std::map<cport*, std::list<cpacket*> > pin_queue; // list of incoming packets per cport
		//std::list<std::pair<cport*, cpacket*> > pin_queue; // list of incoming packets per cport

		std::map<cport*, std::deque<cpacket*> > pin_queue; // list of incoming packets per cport

		std::deque<cport*> pin_cports; // list of cports sending a notification to cport_owner

		std::string info;

	private: // methods

		/** handle packets received from an attached cport
		 */
		void
		__handle_cport_packet_in();
	};

};

#endif
