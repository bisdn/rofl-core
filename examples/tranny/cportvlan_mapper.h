#ifndef cportvlan_mapper_H
#define cportvlan_mapper_H 1

#include <vector>
#include <ostream>
#include <utility>
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <stdint.h>

#define MAXVLANID 4095

class cportvlan_mapper {		// a class that will translate only ports and vlans

public:
static const uint32_t NO_VLAN = 0xfffffffd;
static const uint32_t ANY_VLAN = 0xfffffffe;
static const uint32_t ANY_PORT = 0xffffffff;

struct port_spec {
uint32_t port; /* Input switch port. - only 16 bits are valid - could also be set to ANY_PORT*/
uint32_t vlan; /* Input VLAN id. Only values 0-0x0fff are valid - could also be set to ANY_VLAN, NO_VLAN */

class PORT {
    friend struct port_spec;
protected:
    uint32_t val;
    explicit PORT (uint16_t, bool any):val(cportvlan_mapper::ANY_PORT) {}
public:
static const PORT ANY;
explicit PORT (uint16_t p):val(p) { }
static cportvlan_mapper::port_spec::PORT make_ANY() { return PORT(0, true); }
};

class VLANID {
    friend struct port_spec;
protected:
    uint32_t val;
    explicit VLANID (uint16_t, bool any_or_none):val(any_or_none?cportvlan_mapper::ANY_VLAN:cportvlan_mapper::NO_VLAN) {}
public:
static const VLANID ANY;
static const VLANID NONE;
explicit VLANID (uint16_t v):val(v) { if(v>MAXVLANID) throw std::out_of_range("VLAN ID cannot be greater than MAXVLANID"); }
static cportvlan_mapper::port_spec::VLANID make_ANY() { return VLANID(0, true); }
static cportvlan_mapper::port_spec::VLANID make_NONE() { return VLANID(0, false); }
};

port_spec(PORT port_, VLANID vlan_ = VLANID::NONE ):port(port_.val),vlan(vlan_.val) {}

bool port_is_wild() const { return (port==ANY_PORT); }
bool vlanid_is_wild() const { return (vlan==ANY_VLAN); }
bool vlanid_is_none() const { return (vlan==NO_VLAN); }

friend std::ostream & operator<< (std::ostream & os, const struct port_spec & spec) {
	os << "port: ";
	if(spec.port_is_wild()) os << "*"; else os << (unsigned int) spec.port;
	os << " vlan: ";
	if(spec.vlanid_is_wild()) os << "*";
	else if(spec.vlanid_is_none()) os << "NO_VLAN";
	else os << (unsigned int) spec.vlan;
	return os;
	}
};

enum port_spec_fields { // mimics ofp_flow_wildcards
OFPFW_IN_PORT = 1 << 0, /* Switch input port. */
OFPFW_DL_VLAN = 1 << 1, /* VLAN id. */
};

typedef struct port_spec port_spec_t;

protected:
std::vector<port_spec_t> m_virtual_to_abstract;

public:

cportvlan_mapper() {}

template <typename InputIterator> cportvlan_mapper(InputIterator begin, InputIterator end) {
// template <typename InputIterator> cportvlan_mapper(InputIterator begin, InputIterator end):m_virtual_to_abstract(begin, end) {
	while(begin != end) {
		add_virtual_port(*begin);	// will check that there are no wildcards in the port_spec and copy them to m_virtual_to_abstract
	++begin;
	}
}

port_spec_t get_actual_port(const uint16_t virtual_port) const {	// could throw rofl::ePortInval
	if(virtual_port==0) { std::stringstream ss; ss << __FUNCTION__ << ": Port " << virtual_port << " is invalid. Ports are numbered from 1."; throw std::out_of_range( (std::string)ss.str() ); }
	try {
		return m_virtual_to_abstract.at(virtual_port-1);
	} catch (std::out_of_range & e) { std::stringstream ss; ss << __FUNCTION__ << ": Port " << virtual_port << " invalid."; throw std::out_of_range( (std::string)ss.str() ); }
}

size_t get_number_virtual_ports() const { return m_virtual_to_abstract.size(); }

// returns the number of the port that was just added.
unsigned add_virtual_port(port_spec_t n) {	// throws std::invalid_argument
	// check that there are no wildcards in the port_spec
	if(n.port_is_wild()||n.vlanid_is_wild()) {
		std::stringstream ss;
		ss << __FUNCTION__ << ": when adding virtual port #" << get_number_virtual_ports() << " supplied port_spec_t (" << n << ") cannot have wildcards";
		throw std::invalid_argument(ss.str());
	}
	m_virtual_to_abstract.push_back(n);
	// return virtual port number of the just added port.
	return get_number_virtual_ports();
}

std::vector<std::pair<uint16_t, port_spec_t> > actual_to_virtual_map(port_spec_t a /*ctual_port*/) const {	// returns a vector with all matching virtual ports, and their specific actual ports
	// scan through m_virtual_to_abstract to find a matching virtual port
	std::vector<std::pair<uint16_t, port_spec_t> > out;
	for(size_t i=0;i<m_virtual_to_abstract.size();++i) {
		port_spec_t p = m_virtual_to_abstract[i];
		if( (!a.port_is_wild()) && (!p.port_is_wild()) && (a.port!=p.port) ) continue;
		if( (!a.vlanid_is_wild()) && (!p.vlanid_is_wild()) && (a.vlan!=p.vlan) ) continue;
		out.push_back(std::make_pair<uint16_t, port_spec_t>(i+1, p));
	}
	return out;
	}

// rofl::cofaclist action_convertor(const rofl::cofaclist & in) const;
	

friend std::ostream & operator<< (std::ostream & os, const cportvlan_mapper & translator) {
	size_t N = translator.get_number_virtual_ports();
	os << "List of " << N <<  " ports.\n";
	for(size_t i = 1; i <= N; ++i)
		os << "virtual port: " << i << " actual " << translator.get_actual_port(i) << "\n";
	return os;
	}

};

typedef cportvlan_mapper::port_spec_t::PORT PV_PORT_T;
typedef cportvlan_mapper::port_spec_t::VLANID PV_VLANID_T;

#endif
