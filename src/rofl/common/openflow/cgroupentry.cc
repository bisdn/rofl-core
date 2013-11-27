/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cgroupentry.h"

using namespace rofl;

cgroupentry::cgroupentry() :
	group_mod_area(sizeof(struct openflow12::ofp_group_mod) + 128/*space for actions, will be extended in method pack() if necessary*/),
	group_mod(NULL)
{
	reset();
}


cgroupentry::~cgroupentry()
{

}


cgroupentry&
cgroupentry::operator= (const cgroupentry& ge)
{
	if (this == &ge)
		return *this;

	this->buckets = ge.buckets;
	this->group_mod_area = ge.group_mod_area;

	this->group_mod = (struct openflow12::ofp_group_mod*)this->group_mod_area.somem();

	return *this;
}


void
cgroupentry::reset()
{
	bzero(group_mod_area.somem(), group_mod_area.memlen());
	group_mod = (struct openflow12::ofp_group_mod*)group_mod_area.somem();

	group_mod->command = htobe16(OFPGC_ADD);			// default: add flow-mod entry
	group_mod->type = OFPGT_ALL;
	group_mod->group_id = htobe32(0);
}


uint16_t
cgroupentry::get_command() const
{
	return be16toh(group_mod->command);
}


void
cgroupentry::set_command(uint16_t command)
{
	group_mod->command = htobe16(command);
}


uint8_t
cgroupentry::get_type() const
{
	return group_mod->type;
}


void
cgroupentry::set_type(uint8_t type)
{
	group_mod->type = type;
}


uint32_t
cgroupentry::get_group_id() const
{
	return be32toh(group_mod->group_id);
}


void
cgroupentry::set_group_id(uint32_t group_id)
{
	group_mod->group_id = htobe32(group_id);
}


size_t
cgroupentry::pack()
{
	size_t bclen = buckets.length(); // length required for packing buckets in binary array of "struct ofp_bucket"

	if ((sizeof(struct openflow12::ofp_group_mod) + bclen) > group_mod_area.memlen()) // not enough space? => resize memory area for group_mod
	{
		group_mod_area.resize(sizeof(struct openflow12::ofp_group_mod) + bclen);
		group_mod = (struct openflow12::ofp_group_mod*)group_mod_area.somem();
	}

	buckets.pack((uint8_t*)group_mod->buckets, bclen); // pack our bucket list into the memory area group_mod->buckets

	return (sizeof(struct openflow12::ofp_group_mod) + bclen); // return size of struct openflow12::ofp_group_mod including appended buckets
}







void
cgroupentry::test()
{
	cgroupentry ge;

	ge.set_command((uint16_t)OFPGC_ADD);
	ge.set_group_id(32);
	ge.set_type(OFPGT_ALL);

	ge.buckets[0].watch_group = 1;
	ge.buckets[0].watch_port = 8;
	ge.buckets[0].weight = 0x0800;
	ge.buckets[0].actions[0] = cofaction_output(openflow12::OFP_VERSION, 6);
	ge.buckets[1].actions[0] = cofaction_pop_vlan(openflow12::OFP_VERSION);
	ge.buckets[1].actions[1] = cofaction_push_vlan(openflow12::OFP_VERSION, 600);
	ge.buckets[1].actions[2] = cofaction_dec_nw_ttl(openflow12::OFP_VERSION);
	ge.buckets[1].actions[3] = cofaction_set_queue(openflow12::OFP_VERSION, 3);
	ge.buckets[1].actions[4] = cofaction_copy_ttl_out(openflow12::OFP_VERSION);
	ge.buckets[1].actions[5] = cofaction_copy_ttl_in(openflow12::OFP_VERSION);
	ge.buckets[1].actions[6] = cofaction_output(openflow12::OFP_VERSION, 16);

	std::cerr << "XXXXXX => " << ge << std::endl;
}
