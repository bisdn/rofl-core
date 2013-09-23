/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cgroupentry.h"

using namespace rofl;

cgroupentry::cgroupentry() :
	group_mod(NULL),
	group_mod_area(sizeof(struct ofp12_group_mod) + 128/*space for actions, will be extended in method pack() if necessary*/)
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

	this->group_mod = (struct ofp12_group_mod*)this->group_mod_area.somem();

	return *this;
}


void
cgroupentry::reset()
{
	bzero(group_mod_area.somem(), group_mod_area.memlen());
	group_mod = (struct ofp12_group_mod*)group_mod_area.somem();

	group_mod->command = htobe16(OFPGC_ADD);			// default: add flow-mod entry
	group_mod->type = OFPGT_ALL;
	group_mod->group_id = htobe32(0);
}

void
cgroupentry::set_command(uint16_t command)
{
	group_mod->command = htobe16(command);
}


void
cgroupentry::set_type(uint8_t type)
{
	group_mod->type = type;
}


void
cgroupentry::set_group_id(uint32_t group_id)
{
	group_mod->group_id = htobe32(group_id);
}



const char*
cgroupentry::c_str()
{
	std::string cmd;
	switch (be16toh(group_mod->command)) {
	case OFPGC_ADD:
		cmd.assign("-ADD-");
		break;
	case OFPGC_MODIFY:
		cmd.assign("-MODIFY-");
		break;
	case OFPGC_DELETE:
		cmd.assign("-DELETE-");
		break;
	default:
		cmd.assign("-UNKNOWN-");
		break;
	}

	std::string type;
	switch (group_mod->type) {
	case OFPGT_ALL:
		type.assign("-ALL-");
		break;
	case OFPGT_SELECT:
		type.assign("-SELECT-");
		break;
	case OFPGT_INDIRECT:
		type.assign("-INDIRECT-");
		break;
	case OFPGT_FF:
		type.assign("-FAST-FAILOVER-");
		break;
	default:
		type.assign("-UNKNOWN-");
		break;
	}

	cvastring vas(2048);
	info.assign(vas("cgroupentry(%p) command:%s type:%s group_id:%d buckets: %s",
			this, cmd.c_str(), type.c_str(), be32toh(group_mod->group_id), buckets.c_str()));

	return info.c_str();
}


size_t
cgroupentry::pack()
{
	//size_t instslen = cofinst::insts_len(instvec);

	size_t bclen = buckets.length(); // length required for packing buckets in binary array of "struct ofp_bucket"

	WRITELOG(UNKNOWN, DBG, "cgroupentry(%p)::pack() [0] bclen[%d] group_mod_area: %s",
			this, bclen, group_mod_area.c_str());

	if ((sizeof(struct ofp12_group_mod) + bclen) > group_mod_area.memlen()) // not enough space? => resize memory area for group_mod
	{
		group_mod_area.resize(sizeof(struct ofp12_group_mod) + bclen);
		group_mod = (struct ofp12_group_mod*)group_mod_area.somem();
	}

	WRITELOG(UNKNOWN, DBG, "cgroupentry(%p)::pack() [1] bclen[%d] group_mod_area: %s",
				this, bclen, group_mod_area.c_str());

	//WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [1] flow_mod_area: %s", this, flow_mod_area.c_str());

	buckets.pack((uint8_t*)group_mod->buckets, bclen); // pack our bucket list into the memory area group_mod->buckets

	//WRITELOG(UNKNOWN, DBG, "cflowentry(%p)::pack() [2] flow_mod_area: %s", this, flow_mod_area.c_str());

	WRITELOG(UNKNOWN, DBG, "cgroupentry(%p)::pack() [2] bclen[%d] group_mod_area: %s",
				this, bclen, group_mod_area.c_str());

	return (sizeof(struct ofp12_group_mod) + bclen); // return size of struct ofp12_group_mod including appended buckets
}






#if 0

cofaction ac(OFPAT_OUTPUT, 1);

cofbucket bc(0x800, 5, 17);
bc.action_add(ac);

cgroupentry ge;

//ge.bucket_add(bc);



int idx1 = ge.buckets[0].actions.add_action_set_nw_tos(2);

ge.buckets[0].watch_port = 3;
ge.buckets[0].actions[0] = cofaction_output(5);
ge.buckets[0].actions[1] = cofaction_set_vlan_vid(1000);
ge.buckets[0].actions[2] = cofaction_set_vlan_pcp(3);

cmemory dl_src(OFP_ETH_ALEN);
ge.buckets[0].actions[3] = cofaction_set_dl_src(dl_src);

cmemory dl_dst(OFP_ETH_ALEN);
ge.buckets[0].actions[4] = cofaction_set_dl_dst(dl_dst);



// in cfwdelem::send_group_mod(...)
cmemory area(1024);

ge.pack((struct ofp12_group_mod*)area.somem(), 256);


ge.buckets[1].aclist[0].type = OFPAT_OUTPUT;

#endif



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
	ge.buckets[0].actions[0] = cofaction_output(OFP12_VERSION, 6);
	ge.buckets[1].actions[0] = cofaction_pop_vlan(OFP12_VERSION);
	ge.buckets[1].actions[1] = cofaction_push_vlan(OFP12_VERSION, 600);
	ge.buckets[1].actions[2] = cofaction_dec_nw_ttl(OFP12_VERSION);
	ge.buckets[1].actions[3] = cofaction_set_queue(OFP12_VERSION, 3);
	ge.buckets[1].actions[4] = cofaction_copy_ttl_out(OFP12_VERSION);
	ge.buckets[1].actions[5] = cofaction_copy_ttl_in(OFP12_VERSION);
	ge.buckets[1].actions[6] = cofaction_output(OFP12_VERSION, 16);


	fprintf(stderr, "XXXXX ==>: %s\n", ge.c_str());
}
