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

#include "rofl/common/openflow/openflow_common.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/caddress.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/cofportstats.h"



#ifndef OFPP_UNSPECIFIED
#define OFPP_UNSPECIFIED 0
#endif



namespace rofl {
namespace openflow {

class ePortBase 			: public RoflException {};
class ePortInval	 		: public ePortBase {};
class ePortNotFound 		: public ePortBase {};

class cofport : public cmemory
{
private: // data structures

	uint8_t								 ofp_version;	// OpenFlow version of port stored (openflow10::OFP_VERSION, openflow12::OFP_VERSION, ...)
	rofl::openflow::cofport_stats_reply	 port_stats;

public: // data structures

	union {
		uint8_t							*ofpu_port;
		struct openflow10::ofp_port		*ofpu10_port;
		struct openflow12::ofp_port		*ofpu12_port;
		struct openflow13::ofp_port		*ofpu13_port;
	} ofp_ofpu;

#define ofh_port   ofp_ofpu.ofpu_port
#define ofh10_port ofp_ofpu.ofpu10_port
#define ofh12_port ofp_ofpu.ofpu12_port
#define ofh13_port ofp_ofpu.ofpu13_port


/*
 * methods
 */
public:


	/**
	 *
	 */
	cofport(
			uint8_t of_version = openflow::OFP_VERSION_UNKNOWN);


	/**
	 *
	 */
	cofport(
			uint8_t ofp_version, uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	cofport(
			cofport const& port);


	/**
	 *
	 */
	cofport&
	operator= (
			cofport const& port);


	/**
	 *
	 */
	virtual
	~cofport();

public:

	/**
	 *
	 */
	void
	pack(
			uint8_t *buf, size_t buflen);



	/**
	 *
	 */
	void
	unpack(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	size_t
	length() const;


	/**
	 *
	 */
	rofl::openflow::cofport_stats_reply&
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
	virtual uint8_t*
	resize(size_t len);


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




public:


	friend std::ostream&
	operator<< (std::ostream& os, cofport const& port) {
		switch (port.ofp_version) {
		case openflow10::OFP_VERSION: {
			os << indent(0) << "<cofport >" << std::endl;
			os << indent(2) << "<portno:"		 	<< (int)port.get_port_no() 				<< " >" << std::endl;
			os << indent(2) << "<hwaddr:"		 	<< port.get_hwaddr() 					<< " >" << std::endl;
			os << indent(2) << "<name:" 			<< port.get_name() 						<< " >" << std::endl;
			os << indent(2) << "<config:"		 	<< (int)port.get_config() 				<< " >" << std::endl;
			os << indent(2) << "<state:"		 	<< (int)port.get_state() 				<< " >" << std::endl;
			os << indent(2) << "<curr:" 			<< (int)port.get_curr() 				<< " >" << std::endl;
			os << indent(2) << "<advertised:"	 	<< (int)port.get_advertised() 			<< " >" << std::endl;
			os << indent(2) << "<supported:" 		<< (int)port.get_supported() 			<< " >" << std::endl;
			os << indent(2) << "<peer:" 			<< (int)port.get_peer() 				<< " >" << std::endl;
		} break;
		case openflow12::OFP_VERSION: {
			os << indent(0) << "<cofport >" << std::endl;
			os << indent(2) << "<portno:"		 	<< (int)port.get_port_no() 				<< " >" << std::endl;
			os << indent(2) << "<hwaddr:"		 	<< port.get_hwaddr() 					<< " >" << std::endl;
			os << indent(2) << "<name:" 			<< port.get_name() 						<< " >" << std::endl;
			os << indent(2) << "<config:"		 	<< (int)port.get_config() 				<< " >" << std::endl;
			os << indent(2) << "<state:"		 	<< (int)port.get_state() 				<< " >" << std::endl;
			os << indent(2) << "<curr:" 			<< (int)port.get_curr() 				<< " >" << std::endl;
			os << indent(2) << "<advertised:"	 	<< (int)port.get_advertised() 			<< " >" << std::endl;
			os << indent(2) << "<supported:" 		<< (int)port.get_supported() 			<< " >" << std::endl;
			os << indent(2) << "<peer:" 			<< (int)port.get_peer() 				<< " >" << std::endl;
			os << indent(2) << "<curr-speed:" 		<< (int)port.get_curr_speed() 			<< " >" << std::endl;
			os << indent(2) << "<max-speed:" 		<< (int)port.get_max_speed() 			<< " >" << std::endl;
			os << indent(2) << "<state-blocked:" 	<< (int)port.link_state_is_blocked() 	<< " >" << std::endl;
			os << indent(2) << "<state-live:" 		<< (int)port.link_state_is_live() 		<< " >" << std::endl;
			os << indent(2) << "<state-link-down:" 	<< (int)port.link_state_is_link_down() 	<< " >" << std::endl;
			os << indent(2) << "<config-port-down:" << (int)port.config_is_port_down() 		<< " >" << std::endl;
		} break;
		case openflow13::OFP_VERSION: {
			os << indent(0) << "<cofport >" << std::endl;
			os << indent(2) << "<portno:"		 	<< (int)port.get_port_no() 				<< " >" << std::endl;
			os << indent(2) << "<hwaddr:"		 	<< port.get_hwaddr() 					<< " >" << std::endl;
			os << indent(2) << "<name:" 			<< port.get_name() 						<< " >" << std::endl;
			os << indent(2) << "<config:"		 	<< (int)port.get_config() 				<< " >" << std::endl;
			os << indent(2) << "<state:"		 	<< (int)port.get_state() 				<< " >" << std::endl;
			os << indent(2) << "<curr:" 			<< (int)port.get_curr() 				<< " >" << std::endl;
			os << indent(2) << "<advertised:"	 	<< (int)port.get_advertised() 			<< " >" << std::endl;
			os << indent(2) << "<supported:" 		<< (int)port.get_supported() 			<< " >" << std::endl;
			os << indent(2) << "<peer:" 			<< (int)port.get_peer() 				<< " >" << std::endl;
			os << indent(2) << "<curr-speed:" 		<< (int)port.get_curr_speed() 			<< " >" << std::endl;
			os << indent(2) << "<max-speed:" 		<< (int)port.get_max_speed() 			<< " >" << std::endl;
			os << indent(2) << "<state-blocked:" 	<< (int)port.link_state_is_blocked() 	<< " >" << std::endl;
			os << indent(2) << "<state-live:" 		<< (int)port.link_state_is_live() 		<< " >" << std::endl;
			os << indent(2) << "<state-link-down:" 	<< (int)port.link_state_is_link_down() 	<< " >" << std::endl;
			os << indent(2) << "<config-port-down:" << (int)port.config_is_port_down() 		<< " >" << std::endl;
		} break;
		default: {
			os << "<cofport unknown OFP version>" << std::endl;
		};
		}
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

}; // end of namespace openflow
}; // end of namespace rofl

#endif
