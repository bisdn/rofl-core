/*
 * ccontrol.hpp
 *
 *  Created on: 01.08.2014
 *      Author: andreas
 */

#ifndef CCONTROL_HPP_
#define CCONTROL_HPP_

#include <rofl/common/crofbase.h>

namespace testomat {

class ccontrol : public rofl::crofbase {
public:

	/**
	 *
	 */
	ccontrol(const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap) :
		rofl::crofbase(versionbitmap) {};

	/**
	 *
	 */
	virtual
	~ccontrol() {};

protected:

	/**
	 *
	 */
	virtual void
	handle_dpt_open(
			rofl::crofdpt& dpt);

	/**
	 *
	 */
	virtual void
	handle_dpt_close(
			rofl::crofdpt& dpt);

	/**
	 *
	 */
	virtual void
	handle_desc_stats_reply(
			rofl::crofdpt& dpt, const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_desc_stats_reply& msg);

};

}; // end of namespace testomat

#endif /* CCONTROL_HPP_ */
