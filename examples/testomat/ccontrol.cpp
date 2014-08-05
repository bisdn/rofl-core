/*
 * ccontrol.cpp
 *
 *  Created on: 01.08.2014
 *      Author: andreas
 */

#include "ccontrol.hpp"

using namespace testomat;

void
ccontrol::handle_dpt_open(
		rofl::crofdpt& dpt)
{
	std::cout << "[ccontrol] dpt open, dpid: " << dpt.get_dpid_s() << std::endl;

	std::cout << "[ccontrol] sending Desc-Stats-Request " << std::endl;
	dpt.send_desc_stats_request(rofl::cauxid(0), 0);
}



void
ccontrol::handle_dpt_close(
		rofl::crofdpt& dpt)
{
	std::cout << "[ccontrol] dpt close, dpid: " << dpt.get_dpid_s() << std::endl;
}



void
ccontrol::handle_desc_stats_reply(
		rofl::crofdpt& dpt, const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_desc_stats_reply& msg)
{
	std::cout << "[ccontrol] rcvd Desc-Stats-Reply: " << std::endl << msg;
}



