/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cfsm.h"

using namespace rofl;

cfsm::cfsm() :
	fsm_state(0)
{
}


cfsm::~cfsm()
{
}


void
cfsm::init_state(int state)
{
	fsm_state = state;
}


int
cfsm::new_state(int state)
{
	fsm_state = state;
	return fsm_state;
}


int
cfsm::cur_state() const
{
	return fsm_state;
}


const char*
cfsm::c_str()
{
	char str[64];
	bzero(str, sizeof(str));
	snprintf(str, sizeof(str) - 1, "cfsm(state=%d)", fsm_state);
	info.assign(str);
	return info.c_str();
}

