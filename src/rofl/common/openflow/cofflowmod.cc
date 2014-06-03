/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofflowmod.h"

using namespace rofl::openflow;

cofflowmod::cofflowmod(uint8_t ofp_version) :
		ofp_version(ofp_version),
		match(ofp_version, rofl::openflow::OFPMT_OXM),
		actions(ofp_version),
		instructions(ofp_version),
		command(0),
		table_id(0),
		idle_timeout(0),
		hard_timeout(0),
		cookie(0),
		cookie_mask(0),
		priority(0),
		buffer_id(0),
		out_port(0),
		out_group(0),
		flags(0)
{

}



cofflowmod::~cofflowmod()
{

}



cofflowmod::cofflowmod(const cofflowmod& fe)
{
	*this = fe;
}



cofflowmod&
cofflowmod::operator= (const cofflowmod& fe)
{
	if (this == &fe)
		return *this;

	ofp_version		= fe.ofp_version;

	match 			= fe.match;
	actions			= fe.actions;
	instructions 	= fe.instructions;

	command			= fe.command;
	table_id		= fe.table_id;
	idle_timeout	= fe.idle_timeout;
	hard_timeout	= fe.hard_timeout;
	cookie			= fe.cookie;
	cookie_mask		= fe.cookie_mask;
	priority		= fe.priority;
	buffer_id		= fe.buffer_id;
	out_port		= fe.out_port;
	out_group		= fe.out_group;
	flags			= fe.flags;

	return *this;
}




