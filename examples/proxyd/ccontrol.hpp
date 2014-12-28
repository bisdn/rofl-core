/*
 * ccontrol.hpp
 *
 *  Created on: 01.08.2014
 *      Author: andreas
 */

#ifndef CCONTROL_HPP_
#define CCONTROL_HPP_

#include <set>
#include <rofl/common/crofshim.h>
#include <rofl/common/protocols/fvlanframe.h>

namespace rofl {
namespace examples {
namespace proxy {

class ccontrol : public rofl::common::crofshim {
public:

	/**
	 *
	 */
	ccontrol();

	/**
	 *
	 */
	virtual
	~ccontrol() {};

public:

	/**
	 *
	 */
	virtual void
	handle_message(
			const rofl::common::ctspaddress_in4& addr, rofl::openflow::cofmsg *msg);

	/**
	 *
	 */
	virtual void
	handle_message(
			const rofl::common::ctspaddress_in6& addr, rofl::openflow::cofmsg *msg);

private:

	std::set<rofl::common::ctspaddress_in4> 	addrs_in4;	// addresses of all attached entities
	std::set<rofl::common::ctspaddress_in6> 	addrs_in6;	// addresses of all attached entities
};

}; // namespace testomat
}; // namespace examples
}; // namespace rofl

#endif /* CCONTROL_HPP_ */
