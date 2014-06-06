/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofflowmod.h"

using namespace rofl::openflow;

cofflowmod::cofflowmod(uint8_t ofp_version) :
		ofp_version(ofp_version),
		match(ofp_version),
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



void
cofflowmod::clear()
{
	match.clear();
	actions.clear();
	instructions.clear();
}



void
cofflowmod::check_prerequisites() const
{
	match.check_prerequisites();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		actions.check_prerequisites();
	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		instructions.check_prerequisites();
	} break;
	default:
		throw eBadVersion("cofflowmod::check_prerequisites() no version defined");
	}
}



size_t
cofflowmod::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct ofp10_flow_mod) + actions.length());
	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct ofp13_flow_mod) + match.length() + instructions.length());
	} break;
	default:
		throw eBadVersion("cofflowmod::length() no version defined");
	}
}



void
cofflowmod::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval("cofflowmod::pack() buflen too short");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct ofp10_flow_mod* hdr = (struct ofp10_flow_mod*)buf;

		match.pack((uint8_t*)&(hdr->match), sizeof(hdr->match));

		hdr->cookie			= htobe64(cookie);
		hdr->command		= htobe16((uint16_t)command);
		hdr->idle_timeout	= htobe16(idle_timeout);
		hdr->hard_timeout	= htobe16(hard_timeout);
		hdr->priority		= htobe16(priority);
		hdr->buffer_id		= htobe32(buffer_id);
		hdr->out_port		= htobe16((uint16_t)out_port);
		hdr->flags			= htobe16(flags);

		actions.pack(hdr->actions, actions.length());

	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		struct ofp13_flow_mod* hdr = (struct ofp13_flow_mod*)buf;

		hdr->cookie			= htobe64(cookie);
		hdr->cookie_mask	= htobe64(cookie_mask);
		hdr->table_id		= table_id;
		hdr->command		= command;
		hdr->idle_timeout	= htobe16(idle_timeout);
		hdr->hard_timeout	= htobe16(hard_timeout);
		hdr->priority		= htobe16(priority);
		hdr->buffer_id		= htobe32(buffer_id);
		hdr->out_port		= htobe32(out_port);
		hdr->out_group		= htobe32(out_group);
		hdr->flags			= htobe16(flags);

		match.pack(hdr->match, match.length());

		instructions.pack(hdr->match + match.length(), instructions.length());

	} break;
	default:
		throw eBadVersion("cofflowmod::pack() no version defined");
	}
}



void
cofflowmod::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	match.clear();
	actions.clear();
	instructions.clear();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		if (buflen < sizeof(struct ofp10_flow_mod))
			throw eInval("cofflowmod::unpack() buflen too short");

		struct ofp10_flow_mod* hdr = (struct ofp10_flow_mod*)buf;

		match.unpack((uint8_t*)&(hdr->match), sizeof(hdr->match));

		cookie			= be64toh(hdr->cookie);
		command			= (uint8_t)(be16toh(hdr->command) && 0x00ff);
		idle_timeout	= be16toh(hdr->idle_timeout);
		hard_timeout	= be16toh(hdr->hard_timeout);
		priority		= be16toh(hdr->priority);
		buffer_id		= be32toh(hdr->buffer_id);
		out_port		= be16toh(hdr->out_port);
		flags			= be16toh(hdr->flags);

		size_t actionslen = buflen - sizeof(struct ofp10_flow_mod);

		if (actionslen > 0) {
			actions.unpack(hdr->actions, actionslen);
		}

	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {

		if (buflen < sizeof(struct ofp13_flow_mod))
			throw eInval("cofflowmod::unpack() buflen too short");

		struct ofp13_flow_mod* hdr = (struct ofp13_flow_mod*)buf;

		cookie			= be64toh(hdr->cookie);
		cookie_mask		= be64toh(hdr->cookie_mask);
		table_id		= hdr->table_id;
		command			= hdr->command;
		idle_timeout	= be16toh(hdr->idle_timeout);
		hard_timeout	= be16toh(hdr->hard_timeout);
		priority		= be16toh(hdr->priority);
		buffer_id		= be32toh(hdr->buffer_id);
		out_port		= be32toh(hdr->out_port);
		out_group		= be32toh(hdr->out_group);
		flags			= be16toh(hdr->flags);

		size_t matchhdrlen = buflen - sizeof(struct ofp13_flow_mod);

		if (matchhdrlen < sizeof(struct rofl::openflow13::ofp_match))
			throw eInval("cofflowmod::unpack() buflen too short");

		struct rofl::openflow13::ofp_match* m = (struct rofl::openflow13::ofp_match*)(hdr->match);

		size_t matchlen = be16toh(m->length);

		match.unpack(hdr->match, matchlen);

		size_t instructionslen = buflen - sizeof(struct ofp13_flow_mod) - match.length();

		if (instructionslen > 0) {
			instructions.unpack(hdr->match + match.length(), instructionslen);
		}

	} break;
	default:
		throw eBadVersion("cofflowmod::unpack() no version defined");
	}
}



