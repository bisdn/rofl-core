#ifndef CPORTVLAN_MESSAGETRANSLATOR_H
#define CPORTVLAN_MESSAGETRANSLATOR_H 1

#include <vector>
#include <ostream>
#include <utility>
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <rofl/common/crofbase.h>
#include <rofl/common/cerror.h>
#include <rofl/common/openflow/openflow_rofl_exceptions.h>
#include <rofl/common/openflow/cofdpt.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/openflow10.h>
#include "cmessagetranslator.h"

class cportvlan_messagetranslator : public cmessagetranslator {		// a class that will translate only ports and vlans

public:
static const uint16_t NO_VLAN = 0xffff;

struct port_spec {		// mimics ofp10_match
uint32_t field_set; /* specifes which fields are set */
uint16_t port; /* Input switch port. */
uint16_t vlan; /* Input VLAN id. Only values 0-0x0fff are valid - anything above means that the vlan tag is not present*/
port_spec(uint16_t port_, uint16_t vlan_ = NO_VLAN, bool port_set = true, bool vlan_set = true ):field_set( (port_set?OFPFW_IN_PORT:0) | (vlan_set?OFPFW_DL_VLAN:0) ),port(port_),vlan((vlan_>0x0fff)?NO_VLAN:vlan_) {}

friend std::ostream & operator<< (std::ostream & os, const struct port_spec & spec) {
		os << "port: ";
		if(spec.field_set&OFPFW_IN_PORT) os << spec.port; else os << "*";
		os << " vlan: ";
		if(spec.field_set&OFPFW_DL_VLAN) {
			if(spec.vlan>0x0fff) os << "NO_VLAN";
			else os << spec.vlan;
		} else os << "*";
		return os;
	}
};

enum port_spec_fields {	// mimics ofp_flow_wildcards
	OFPFW_IN_PORT = 1 << 0, /* Switch input port. */
	OFPFW_DL_VLAN = 1 << 1, /* VLAN id. */
};

typedef struct port_spec port_spec_t;
typedef std::vector<port_spec_t> port_list_t;

protected:
std::vector<port_spec_t> m_virtual_to_abstract;

public:

cportvlan_messagetranslator(port_list_t port_list):m_virtual_to_abstract(port_list) {}

port_spec_t get_actual_port(const uint16_t virtual_port) const {	// could throw rofl::ePortInval
	try {
		return m_virtual_to_abstract.at(virtual_port);
	} catch (std::out_of_range & e) { std::stringstream ss; ss << __FUNCTION__ << ": Port " << virtual_port << " invalid."; throw std::out_of_range( (std::string)ss.str() ); }
}

size_t get_number_virtual_ports() const { return m_virtual_to_abstract.size(); }

std::vector<std::pair<uint16_t, port_spec_t> > actual_to_virtual_map(port_spec_t a /*ctual_port*/) const {	// returns a vector with all matching virtual ports, and their specific actual ports
	// fix up vlan port to NO_VLAN if necessary
	if(a.port>0x0fff) a.port = NO_VLAN;
	// scan through m_virtual_to_abstract to find a matching virtual port
	std::vector<std::pair<uint16_t, port_spec_t> > out;
	for(size_t i=0;i<m_virtual_to_abstract.size();++i) {
		port_spec_t p = m_virtual_to_abstract[i];
		if( (a.field_set&OFPFW_IN_PORT) && (p.field_set&OFPFW_IN_PORT) && (a.port!=p.port) ) continue;
		if( (a.field_set&OFPFW_DL_VLAN) && (p.field_set&OFPFW_DL_VLAN) && (a.vlan!=p.vlan) ) continue;
		out.push_back(std::make_pair<uint16_t, port_spec_t>(i, p));
	}
	return out;
	}

friend std::ostream & operator<< (std::ostream & os, const cportvlan_messagetranslator & translator) {
	size_t N = translator.get_number_virtual_ports();
	os << "List of " << N <<  " ports.\n";
	for(size_t i = 0; i < N; ++i)
		os << "virtual port: " << i << " actual " << translator.get_actual_port(i) << "\n";
	return os;
	}

};

#endif
