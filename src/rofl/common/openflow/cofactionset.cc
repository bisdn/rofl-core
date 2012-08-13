/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofactionset.cc
 *
 *  Created on: Feb 6, 2012
 *      Author: tobi
 */

#include "cofactionset.h"

/*static*/std::map<uint16_t, int> cofactionset::action_indices;

cofactionset::cofactionset() :
		metadata(0),
		metadata_mask(0xffffffffffffffffULL)
{
	// initialize static map of mappings OFPAT_* => priority

	init_action_indices();
}

cofactionset::~cofactionset()
{
}


/* static */void
cofactionset::init_action_indices()
{
	if (cofactionset::action_indices.empty())
	{
		cofactionset::action_indices[OFPAT_COPY_TTL_IN] 	= 0;
		cofactionset::action_indices[OFPAT_POP_PPP]     	= 1;
		cofactionset::action_indices[OFPAT_POP_PPPOE]   	= 2;
		cofactionset::action_indices[OFPAT_POP_MPLS] 		= 3;
		cofactionset::action_indices[OFPAT_POP_VLAN] 		= 4;
		cofactionset::action_indices[OFPAT_PUSH_PPPOE]  	= 5;
		cofactionset::action_indices[OFPAT_PUSH_PPP]    	= 6;
		cofactionset::action_indices[OFPAT_PUSH_MPLS] 		= 7;
		cofactionset::action_indices[OFPAT_PUSH_VLAN] 		= 8;
		cofactionset::action_indices[OFPAT_COPY_TTL_OUT] 	= 9;
		cofactionset::action_indices[OFPAT_DEC_MPLS_TTL] 	= 10;
		cofactionset::action_indices[OFPAT_DEC_NW_TTL] 	 	= 11;
		cofactionset::action_indices[OFPAT_SET_MPLS_TTL] 	= 12;
		cofactionset::action_indices[OFPAT_SET_NW_TTL] 		= 13;
		cofactionset::action_indices[OFPAT_SET_FIELD]		= 14;
		cofactionset::action_indices[OFPAT_SET_QUEUE] 		= 15;
		cofactionset::action_indices[OFPAT_GROUP] 			= 16;
		cofactionset::action_indices[OFPAT_OUTPUT] 			= 17;
	}
}

/* static */int
cofactionset::get_action_index(uint16_t action_type)
{
	init_action_indices();
	return action_indices[action_type];
}

void
cofactionset::actionset_clear(cofinst& inst)
{
	WRITELOG(COFACTION, DBG, "cofactionset(%p)::actionset_clear()", this);
	acset.clear();

	cofaclist::iterator at;
	for (at = inst.actions.begin(); at != inst.actions.end(); ++at)
	{
		cofaction& action = (*at);

		switch (be16toh(action.oac_header->type)) {
		case OFPAT_SET_FIELD:
			{
				coxmatch oxm(
						(struct ofp_oxm_hdr*)action.oac_set_field->field,
						be16toh(action.oac_set_field->len));
				acfields[oxm.get_oxm_class()].erase(oxm.get_oxm_field());
			}
			break;
		}
	}
}


void
cofactionset::actionset_clear()
{
	acset.clear();
	acfields.clear();
}


void
cofactionset::actionset_write_actions(cofinst& inst)
{
	WRITELOG(COFACTION, DBG, "cofactionset(%p)::actionset_write_actions() inst->actions.elems.size()=%u", this, inst.actions.size());

	cofaclist::iterator at;
	for (at = inst.actions.begin(); at != inst.actions.end(); ++at)
	{
		cofaction& action = (*at);

		WRITELOG(COFACTION, DBG, "write action %u at position %u",
				be16toh(action.oac_header->type),
				cofactionset::action_indices[be16toh(action.oac_header->type)]);

		switch (be16toh(action.oac_header->type)) {
		case OFPAT_SET_FIELD:
			{
				coxmatch oxm(
						(struct ofp_oxm_hdr*)action.oac_set_field->field,
						be16toh(action.oac_set_field->len));
				acfields[oxm.get_oxm_class()][oxm.get_oxm_field()] = action;
			}
			break;
		default:
			acset[ cofactionset::action_indices[be16toh(action.oac_header->type)] ] = action; // copy action into action set
			break;
		}
	}
}


void
cofactionset::actionset_write_metadata(cofinst & inst)
{
	uint64_t mask =  be64toh(inst.oin_write_metadata->metadata_mask);
	uint64_t value = be64toh(inst.oin_write_metadata->metadata);

	this->metadata = (this->metadata & ~mask) | (value & mask);
}


const char*
cofactionset::c_str()
{
	cvastring vas;

	info.assign(vas("cofactionset(%p) =>", this));

	std::map<int, cofaction>::iterator it;
	for (it = acset.begin(); it != acset.end(); ++it)
	{
		info.append(vas("\n  %8d -> %s", it->first, it->second.c_str()));
	}
	info.append(vas("\nacfields => "));
	for (std::map<uint16_t, std::map<uint8_t, cofaction> >::iterator
			it = acfields.begin(); it != acfields.end(); ++it)
	{
		for (std::map<uint8_t, cofaction>::iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt)
		{
			info.append(vas("\n  set.field: class:0x%x field:%d -> %s",
					it->first, jt->first, jt->second.c_str()));
		}
	}
	return info.c_str();
}
