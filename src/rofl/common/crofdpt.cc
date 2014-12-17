/*
 * crofdpt.cc
 *
 *  Created on: 07.01.2014
 *      Author: andreas
 */

#include "rofl/common/crofdpt.h"

using namespace rofl;

uint64_t crofdpt::next_dptid = 0;

/*static*/std::set<crofdpt_env*> crofdpt_env::rofdpt_envs;
/*static*/std::map<cdptid, crofdpt*> crofdpt::rofdpts;

/*static*/crofdpt&
crofdpt::get_dpt(
		const cdptid& dptid)
{
	if (crofdpt::rofdpts.find(dptid) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound();
	}
	return *(crofdpt::rofdpts[dptid]);
}



/*static*/crofdpt&
crofdpt::get_dpt(
		const cdpid& dpid)
{
	std::map<cdptid, crofdpt*>::iterator it;
	if ((it = find_if(crofdpt::rofdpts.begin(), crofdpt::rofdpts.end(),
			crofdpt::crofdpt_find_by_dpid(dpid.get_uint64_t()))) == crofdpt::rofdpts.end()) {
		throw eRofDptNotFound();
	}
	return *(it->second);
}

