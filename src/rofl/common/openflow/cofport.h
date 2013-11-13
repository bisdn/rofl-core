/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFPORT_H
#define COFPORT_H 1

#include <map>
#include <limits>

#include <string.h>
#include <endian.h>
#include <inttypes.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "openflow.h"
#include "../cvastring.h"
#include "../cmemory.h"
#include "../cerror.h"
#include "../cmacaddr.h"
#include "../coflist.h"
#include "rofl/platform/unix/csyslog.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "cofportstats.h"



#ifndef OFPP_UNSPECIFIED
#define OFPP_UNSPECIFIED 0
#endif



namespace rofl
{

/* error classes */
class eOFportBase 			: public cerror {};
class eOFportInval	 		: public eOFportBase {};
class eOFportNotFound 		: public eOFportBase {};
class eOFportMalformed 		: public eOFportBase {}; // malformed array of structs ofp_phy_port



class cofport :
		public csyslog
{
private: // data structures

	uint8_t								 of_version;	// OpenFlow version of port stored (OFP10_VERSION, OFP12_VERSION, ...)
	std::map<uint32_t, cofport*> 		*port_list; 	// port_list this port belongs to
	std::string 						 info; 			// info string
	cmemory 							 memarea;		// ofpXX_port structure
	cofport_stats_reply					 port_stats;

public: // data structures

	union {
		uint8_t							*ofpu_port;
		struct ofp10_port				*ofpu10_port;
		struct ofp12_port				*ofpu12_port;
		struct ofp13_port				*ofpu13_port;
	} ofp_ofpu;

#define ofh_port   ofp_ofpu.ofpu_port
#define ofh10_port ofp_ofpu.ofpu10_port
#define ofh12_port ofp_ofpu.ofpu12_port
#define ofh13_port ofp_ofpu.ofpu13_port

#if 0
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
#endif


/*
 * methods
 */
public:


	/** default constructor
	 *
	 */
	cofport(uint8_t of_version = OFP12_VERSION);


	/** constructor
	 *
	 */
	cofport(
			struct ofp10_port *port,
			size_t port_len,
			std::map<uint32_t, cofport*> *port_list = 0,
			uint32_t port_no = 0);


	/** constructor
	 *
	 */
	cofport(
			struct ofp12_port *port,
			size_t port_len,
			std::map<uint32_t, cofport*> *port_list = 0,
			uint32_t port_no = 0);


	/** constructor
	 *
	 */
	cofport(
			struct ofp13_port *port,
			size_t port_len,
			std::map<uint32_t, cofport*> *port_list = 0,
			uint32_t port_no = 0);





	/** destructor
	 */
	virtual
	~cofport();


	/** copy constructor
	 */
	cofport(
			cofport const& port);



	/** copy constructor
	 *
	 */
	cofport(
			cofport const& port,
			std::map<uint32_t, cofport*> *port_list,
			uint32_t port_no);



	/** assignment operator
	 */
	cofport&
	operator= (
			cofport const& port);

	/**
	 *
	 */
	template<class T>
	T*
	pack(
			T* port,
			size_t portlen) const throw (eOFportInval);



	/**
	 *
	 */
	struct ofp10_port*
	pack(
			struct ofp10_port *port,
			size_t portlen) const throw (eOFportInval);


	/**
	 *
	 */
	struct ofp12_port*
	pack(
			struct ofp12_port *port,
			size_t portlen) const throw (eOFportInval);


	/**
	 *
	 */
	struct ofp13_port*
	pack(
			struct ofp13_port *port,
			size_t portlen) const throw (eOFportInval);

	/**
	 *
	 */
	template<class T>
	T*
	unpack(
			T* port,
			size_t portlen) throw (eOFportInval);


	/**
	 *
	 */
	struct ofp10_port*
	unpack(
			struct ofp10_port *port,
			size_t portlen) throw (eOFportInval);



	/**
	 *
	 */
	struct ofp12_port*
	unpack(
			struct ofp12_port *port,
			size_t portlen) throw (eOFportInval);


	/**
	 *
	 */
	struct ofp13_port*
	unpack(
			struct ofp13_port *port,
			size_t portlen) throw (eOFportInval);



	/** dump internals
	 */
	const char*
	c_str();


	/**
	 *
	 */
	size_t
	length() const;


	/** sets all statistics counters to zero
	 *
	 */
	void
	reset_stats();


	/**
	 *
	 */
	cofport_stats_reply&
	get_port_stats();


	/**
	 *
	 */
	uint8_t
	get_version() const;


public:


	/**
	 */
	uint32_t
	get_port_no() const;


	/**
	 */
	void
	set_port_no(uint32_t port_no);


	/**
	 */
	cmacaddr
	get_hwaddr() const;


	/**
	 */
	void
	set_hwaddr(cmacaddr const& maddr);


	/**
	 */
	std::string
	get_name() const;


	/**
	 */
	void
	set_name(std::string name);

	/**
	 */
	uint32_t
	get_config() const;


	/**
	 */
	void
	set_config(uint32_t config);

	/**
	 */
	uint32_t
	get_state() const;


	/**
	 */
	void
	set_state(uint32_t state);


	/**
	 */
	uint32_t
	get_curr() const;


	/**
	 */
	void
	set_curr(uint32_t curr);


	/**
	 */
	uint32_t
	get_advertised() const;


	/**
	 */
	void
	set_advertised(uint32_t advertised);


	/**
	 */
	uint32_t
	get_supported() const;


	/**
	 */
	void
	set_supported(uint32_t supported);


	/**
	 */
	uint32_t
	get_peer() const;


	/**
	 */
	void
	set_peer(uint32_t peer);


	/**
	 */
	uint32_t
	get_curr_speed() const;


	/**
	 */
	void
	set_curr_speed(uint32_t curr_speed);


	/**
	 */
	uint32_t
	get_max_speed() const;


	/**
	 */
	void
	set_max_speed(uint32_t max_speed);


	/**
	 *
	 */
	virtual void
	link_state_set_blocked();


	/**
	 *
	 */
	virtual void
	link_state_clr_blocked();


	/**
	 *
	 */
	virtual bool
	link_state_is_blocked() const;


	/**
	 *
	 */
	virtual void
	link_state_set_live();


	/**
	 *
	 */
	virtual void
	link_state_clr_live();


	/**
	 *
	 */
	virtual bool
	link_state_is_live() const;


	/**
	 *
	 */
	virtual void
	link_state_set_link_down();


	/**
	 *
	 */
	virtual void
	link_state_clr_link_down();


	/**
	 *
	 */
	virtual bool
	link_state_is_link_down() const;


	/**
	 *
	 */
	virtual void
	link_state_phy_down();


	/**
	 *
	 */
	virtual void
	link_state_phy_up();


	/**
	 *
	 */
	bool
	link_state_phy_is_up() const;


	/**
	 *
	 */
	bool
	config_is_port_down() const;


	/**
	 *
	 */
	virtual void
	recv_port_mod(
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);

private:

	/**
	 *
	 */
	virtual void
	recv_port_mod_of10(
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);


	/**
	 *
	 */
	virtual void
	recv_port_mod_of12(
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);


	/**
	 *
	 */
	virtual void
	recv_port_mod_of13(
			uint32_t config,
			uint32_t mask,
			uint32_t advertise);






public: // static

	/**
	 *
	 */
	static void
	test();


	/** parse array of struct ofp10_ports
	 */
	static void
	ports_parse(
			std::map<uint32_t, cofport*>& portsmap,
			struct ofp10_port *ports,
			int portslen) throw (eOFportMalformed);


	/** parse array of struct ofp12_ports
	 */
	static void
	ports_parse(
			std::map<uint32_t, cofport*>& portsmap,
			struct ofp12_port *ports,
			int portslen) throw (eOFportMalformed);


	/** parse array of struct ofp13_ports
	 */
	static void
	ports_parse(
			std::map<uint32_t, cofport*>& portsmap,
			struct ofp13_port *ports,
			int portslen) throw (eOFportMalformed);


#if 0
	/** get a free port-no for a specific cofport list
	 */
	static uint32_t
	ports_get_free_port_no(
		std::map<uint32_t, cofport*> *port_list) throw (eOFportNotFound);
#endif

public:


	friend std::ostream&
	operator<< (std::ostream& os, cofport const& port) {
		os << "<cofport ";
			os << "portno:" << (int)port.get_port_no() << " ";
			os << "hwaddr:" << port.get_hwaddr() << " ";
			os << "name:" << port.get_name() << " ";
			os << "config:" << (int)port.get_config() << " ";
			os << "state:" << (int)port.get_state() << " ";
			os << "curr:" << (int)port.get_curr() << " ";
			os << "advertised:" << (int)port.get_advertised() << " ";
			os << "supported:" << (int)port.get_supported() << " ";
			os << "peer:" << (int)port.get_peer() << " ";
			os << "curr-speed:" << (int)port.get_curr_speed() << " ";
			os << "max-speed:" << (int)port.get_max_speed() << " ";
			os << "state-blocked:" << (int)port.link_state_is_blocked() << " ";
			os << "state-live:" << (int)port.link_state_is_live() << " ";
			os << "state-link-down:" << (int)port.link_state_is_link_down() << " ";
			os << "config-port-down:" << (int)port.config_is_port_down() << " ";
		os << ">";
		return os;
	};
};


class cofport_find_by_port_no {
	uint32_t port_no;
public:
	cofport_find_by_port_no(uint32_t _port_no) :
		port_no(_port_no) {};
	bool operator() (cofport *ofport) {
		return (ofport->get_port_no() == port_no);
	};
	bool operator() (std::pair<uint32_t, cofport*> const& p) {
		return (p.second->get_port_no() == port_no);
	};
};


class cofport_find_by_port_name {
	std::string port_name;
public:
	cofport_find_by_port_name(std::string _port_name) :
		port_name(_port_name) {};
	bool operator() (cofport *ofport) {
		std::string name(ofport->get_name());
		return (name == port_name);
	};
	bool operator() (std::pair<uint32_t, cofport*> const& p) {
		std::string name(p.second->get_name());
		return (name == port_name);
	};
};



class cofport_find_by_maddr {
	cmacaddr maddr;
public:
	cofport_find_by_maddr(cmacaddr const& maddr) :
		maddr(maddr) {};
	bool operator() (cofport *ofport) {
		cmacaddr hwaddr(ofport->get_hwaddr());
		return (maddr == hwaddr);
	};
	bool operator() (std::pair<uint32_t, cofport*> const& p) {
		cmacaddr hwaddr(p.second->get_hwaddr());
		return (maddr == hwaddr);
	};
};


class cofport_find_port_no {
public:
	cofport_find_port_no(uint32_t port_no) :
		port_no(port_no) {};
	bool operator() (cofport const& ofport) {
		return (ofport.get_port_no() == port_no);
	};
	uint32_t port_no;
};

}; // end of namespace

#endif
