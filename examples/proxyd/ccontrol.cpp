/*
 * ccontrol.cpp
 *
 *  Created on: 01.08.2014
 *      Author: andreas
 */

#include "ccontrol.hpp"

using namespace rofl::examples::proxy;

ccontrol::ccontrol()
{
	std::cout << "[proxyd][ccontrol] " << std::endl;

	/*
	 * create listening socket
	 */
	enum rofl::csocket::socket_type_t socket_type = rofl::csocket::SOCKET_TYPE_PLAIN;
	rofl::cparams socket_params = rofl::csocket::get_default_params(socket_type);

	socket_params.set_param(rofl::csocket::PARAM_KEY_DOMAIN).set_string(rofl::csocket::PARAM_DOMAIN_VALUE_INET);
	socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("0.0.0.0");
	socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string("7744");
	rofl::common::crofshim::add_listening_socket_in4(socket_type, socket_params);

	/*
	 * connect to higher layer entity
	 */
	socket_params = rofl::csocket::get_default_params(socket_type);
	socket_params.set_param(rofl::csocket::PARAM_KEY_DOMAIN).set_string(rofl::csocket::PARAM_DOMAIN_VALUE_INET);
	socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("0.0.0.0");
	socket_params.set_param(rofl::csocket::PARAM_KEY_REMOTE_HOSTNAME).set_string("127.0.0.1");
	socket_params.set_param(rofl::csocket::PARAM_KEY_REMOTE_PORT).set_string("6653");

	rofl::common::crofshim::connect(socket_type, socket_params);

	addrs_in4.insert(rofl::common::ctspaddress_in4(
			rofl::caddress_in4(socket_params.set_param(rofl::csocket::PARAM_KEY_REMOTE_HOSTNAME).get_string()),
			atoi(socket_params.set_param(rofl::csocket::PARAM_KEY_REMOTE_PORT).get_string().c_str())));
}



void
ccontrol::handle_message(
		const rofl::common::ctspaddress_in4& addr, rofl::openflow::cofmsg *msg) {

	if (addrs_in4.find(addr) == addrs_in4.end()) {
		addrs_in4.insert(addr);
	}

	std::cerr << "NEW MESSAGE:" << std::endl << *msg;

	for (std::set<rofl::common::ctspaddress_in4>::iterator
			it = addrs_in4.begin(); it != addrs_in4.end(); ++it) {
		if ((*it) == addr) {
			continue;	// do not send message back to where it came from
		}
		rofl::common::crofshim::set_active_rofsocket_in4(addr).send_message(new rofl::openflow::cofmsg(*msg));
	}

	delete msg; /* to be overwritten */
}



void
ccontrol::handle_message(
		const rofl::common::ctspaddress_in6& addr, rofl::openflow::cofmsg *msg) {

	if (addrs_in6.find(addr) == addrs_in6.end()) {
		addrs_in6.insert(addr);
	}

	std::cerr << "NEW MESSAGE:" << std::endl << *msg;

	for (std::set<rofl::common::ctspaddress_in6>::iterator
			it = addrs_in6.begin(); it != addrs_in6.end(); ++it) {
		if ((*it) == addr) {
			continue;	// do not send message back to where it came from
		}
		rofl::common::crofshim::set_active_rofsocket_in6(addr).send_message(new rofl::openflow::cofmsg(*msg));
	}

	delete msg; /* to be overwritten */
}



