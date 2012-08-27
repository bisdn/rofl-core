/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFPORT_H
#define COFPORT_H 1

#include <map>
#include <limits>

#ifdef __cplusplus
extern "C" {
#endif

#include "openflow12.h"
#include <string.h>
#include <endian.h>
#include <inttypes.h>

#ifndef htobe16
#include "../endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "../cvastring.h"
#include "../cmemory.h"
#include "../cerror.h"
#include "../cmacaddr.h"
#include "../coflist.h"
#include "rofl/platform/unix/csyslog.h"




#ifndef OFPP_UNSPECIFIED
#define OFPP_UNSPECIFIED 0
#endif

// forward declarations
class cofdpath;

/* error classes */
class eOFportBase : public cerror {};
class eOFportInval : public eOFportBase {};
class eOFportNotFound : public eOFportBase {};
class eOFportMalformed : public eOFportBase {}; // malformed array of structs ofp_phy_port



class cofport :
	public csyslog
{
/*
 *  data structures
 */
public:

	/*
	 * parameters from struct ofp_port
	 */
	uint32_t 							port_no;
	cmacaddr 							hwaddr;
	std::string 						name;
	uint32_t 							config;
	uint32_t 							state;
	uint32_t 							curr;
	uint32_t 							advertised;
	uint32_t 							supported;
	uint32_t 							peer;
	uint32_t 							curr_speed;
	uint32_t 							max_speed;

	/*
	 * port statistics
	 */
	uint64_t 							rx_packets;
	uint64_t 							tx_packets;
	uint64_t 							rx_bytes;
	uint64_t 							tx_bytes;
	uint64_t 							rx_dropped;
	uint64_t 							tx_dropped;
	uint64_t 							rx_errors;
	uint64_t 							tx_errors;
	uint64_t 							rx_frame_err;
	uint64_t 							rx_over_err;
	uint64_t 							rx_crc_err;
	uint64_t 							collisions;

private:

	std::map<uint32_t, cofport*> 		*port_list; 	// port_list this port belongs to
	std::string 						info; 			// info string



/*
 * methods
 */
public:

	/** constructor
	 *
	 */
	cofport(
			std::map<uint32_t, cofport*> *port_list,
			struct ofp_port *port = (struct ofp_port*)0,
			size_t port_len = 0);


	/** constructor
	 *
	 */
	cofport(
			struct ofp_port *port = (struct ofp_port*)0,
			size_t portlen = 0);



	/** destructor
	 */
	virtual
	~cofport();


	/** copy constructor
	 */
	cofport(
			cofport const& port);


	/** assignment operator
	 */
	cofport&
	operator= (
			cofport const& port);


	/**
	 *
	 */
	void
	recv_port_mod(
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);



	/**
	 *
	 */
	struct ofp_port*
	pack(
			struct ofp_port *port,
			size_t portlen) throw (eOFportInval);


	/**
	 *
	 */
	struct ofp_port*
	unpack(
			struct ofp_port *port,
			size_t portlen) throw (eOFportInval);

	

	/** dump internals
	 */
	const char*
	c_str();


	/**
	 *
	 */
	size_t
	length();


	/** sets all statistics counters to zero
	 *
	 */
	void
	reset_stats();


	/**
	 *
	 */
	void
	get_port_stats(
			cmemory& body);



public: // static

	/**
	 *
	 */
	static void
	test();


	/** parse array of struct ofp_phy_ports
	 */
	static std::map<uint32_t, cofport*>
	ports_parse(
		struct ofp_port *ports,
		int portslen) throw (eOFportMalformed);


	/** get a free port-no for a specific cofport list
	 */
	static uint32_t
	ports_get_free_port_no(
		std::map<uint32_t, cofport*> *port_list) throw (eOFportNotFound);


};


class cofport_find_by_port_no {
	uint32_t port_no;
public:
	cofport_find_by_port_no(uint32_t _port_no) :
		port_no(_port_no) {};
	bool operator() (cofport *ofport) {
		return (ofport->port_no == port_no);
	};
	bool operator() (std::pair<uint32_t, cofport*> const& p) {
		return (p.second->port_no == port_no);
	};
};


class cofport_find_by_port_name {
	std::string port_name;
public:
	cofport_find_by_port_name(std::string _port_name) :
		port_name(_port_name) {};
	bool operator() (cofport *ofport) {
		std::string name(ofport->name);
		return (name == port_name);
	};
	bool operator() (std::pair<uint32_t, cofport*> const& p) {
		std::string name(p.second->name);
		return (name == port_name);
	};
};



class cofport_find_by_maddr {
	cmacaddr maddr;
public:
	cofport_find_by_maddr(cmacaddr const& maddr) :
		maddr(maddr) {};
	bool operator() (cofport *ofport) {
		cmacaddr hwaddr(ofport->hwaddr);
		return (maddr == hwaddr);
	};
	bool operator() (std::pair<uint32_t, cofport*> const& p) {
		cmacaddr hwaddr(p.second->hwaddr);
		return (maddr == hwaddr);
	};
};


class cofport_find_port_no {
public:
	cofport_find_port_no(uint32_t port_no) :
		port_no(port_no) {};
	bool operator() (cofport const& ofport) {
		return (ofport.port_no == port_no);
	};
	uint32_t port_no;
};

#endif
