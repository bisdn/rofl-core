/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofactionset.cc
 *
 *  Created on: Feb 6, 2012
 *      Author: tobi
 */

#include "rofl/common/openflow/cofactionset.h"
#include "rofl/common/logging.h"

using namespace rofl::openflow;

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
		cofactionset::action_indices[openflow12::OFPAT_COPY_TTL_IN] 	= 0;
		cofactionset::action_indices[openflow12::OFPAT_POP_MPLS] 		= 1;
		cofactionset::action_indices[openflow12::OFPAT_POP_VLAN] 		= 2;
		cofactionset::action_indices[openflow12::OFPAT_PUSH_MPLS]	  	= 3;
		cofactionset::action_indices[openflow12::OFPAT_PUSH_VLAN] 	= 4;
		cofactionset::action_indices[openflow12::OFPAT_COPY_TTL_OUT] 	= 5;
		cofactionset::action_indices[openflow12::OFPAT_DEC_MPLS_TTL] 	= 6;
		cofactionset::action_indices[openflow12::OFPAT_DEC_NW_TTL] 	= 7;
		cofactionset::action_indices[openflow12::OFPAT_SET_MPLS_TTL] 	= 8;
		cofactionset::action_indices[openflow12::OFPAT_SET_NW_TTL]	= 9;
		cofactionset::action_indices[openflow12::OFPAT_SET_FIELD]		= 10;
		cofactionset::action_indices[openflow12::OFPAT_SET_QUEUE] 	= 11;
		cofactionset::action_indices[openflow12::OFPAT_GROUP] 		= 12;
		cofactionset::action_indices[openflow12::OFPAT_OUTPUT] 		= 13;
	}
}

/* static */int
cofactionset::get_action_index(uint16_t action_type)
{
	init_action_indices();
	return action_indices[action_type];
}

void
cofactionset::actionset_clear(const cofinstruction_actions& inst)
{
	//WRITELOG(COFACTION, DBG, "cofactionset(%p)::actionset_clear()", this);
	LOGGING_DEBUG << "[rofl]"<<this<<" actionset_clear()"<< std::endl;
	acset.clear();

	for (std::map<cindex, unsigned int>::const_iterator
			it = inst.get_actions().get_actions_index().begin();
					it != inst.get_actions().get_actions_index().end(); ++it) {
		const cindex& index 		= it->first;
		const unsigned int& type	= it->second;

		switch (type) {
		case rofl::openflow13::OFPAT_SET_FIELD: {
			const coxmatch& oxm = inst.get_actions().get_action_set_field(index).get_oxm();
			acfields[oxm.get_oxm_class()].erase(oxm.get_oxm_field());
		} break;
		default:
			continue;
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
cofactionset::actionset_write_actions(const cofinstruction_actions& inst)
{
	//WRITELOG(COFACTION, DBG, "cofactionset(%p)::actionset_write_actions() inst->actions.elems.size()=%u", this, inst.actions.size());
	LOGGING_DEBUG << "[rofl]"<<this<<" actionset_write_actions() inst->actions.elems.size()="<< inst.get_actions().get_actions_index().size() << std::endl;


	for (std::map<cindex, unsigned int>::const_iterator
			it = inst.get_actions().get_actions_index().begin();
					it != inst.get_actions().get_actions_index().end(); ++it) {
#if 0
		const unsigned int& index = it->first;
		const unsigned int& type	= it->second;

		switch (type) {
		case rofl::openflow13::OFPAT_OUTPUT: {
			acset[ cofactionset::action_indices[type] ] = action;
		} break;
		case rofl::openflow13::OFPAT_SET_FIELD: {
			const coxmatch& oxm = inst.get_actions().get_action_set_field(index).get_oxm();
			acfields[oxm.get_oxm_class()] = new cofaction_set_field(inst.get_actions().get_action_set_field(index));
		} break;
		default: {
			acset[ cofactionset::action_indices[type] ] = action; // copy action into action set
		}
		}
#endif
	}

}


void
cofactionset::actionset_write_metadata(const cofinstruction_write_metadata& inst)
{
	uint64_t mask =  inst.get_metadata_mask();
	uint64_t value = inst.get_metadata();

	this->metadata = (this->metadata & ~mask) | (value & mask);
}


