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
cofactionset::actionset_clear(cofinst& inst)
{
	//WRITELOG(COFACTION, DBG, "cofactionset(%p)::actionset_clear()", this);
	logging::debug << "[rofl]"<<this<<" actionset_clear()"<< std::endl;
	acset.clear();

	cofactions::iterator at;
	for (std::list<cofaction*>::iterator
			at = inst.get_actions().begin(); at != inst.get_actions().end(); ++at) {
		cofaction& action = *(*at);
		switch (be16toh(action.oac_header->type)) {
		case openflow12::OFPAT_SET_FIELD: {
			coxmatch oxm(
					(struct openflow::ofp_oxm_hdr*)action.oac_12set_field->field,
					be16toh(action.oac_12set_field->len));
			acfields[oxm.get_oxm_class()].erase(oxm.get_oxm_field());
		} break;
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
	//WRITELOG(COFACTION, DBG, "cofactionset(%p)::actionset_write_actions() inst->actions.elems.size()=%u", this, inst.actions.size());
	logging::debug << "[rofl]"<<this<<" actionset_write_actions() inst->actions.elems.size()="<< inst.actions.size() << std::endl;

	for (std::list<cofaction*>::iterator
			at = inst.get_actions().begin(); at != inst.get_actions().end(); ++at) {
		cofaction& action = *(*at);

		/*WRITELOG(COFACTION, DBG, "write action %u at position %u",
				be16toh(action.oac_header->type),
				cofactionset::action_indices[be16toh(action.oac_header->type)]);
		*/
		logging::debug << "[rofl]"<<this<<" write action "<< be16toh(action.oac_header->type)<<" at position "<<cofactionset::action_indices[be16toh(action.oac_header->type)]<< std::endl;

		switch (be16toh(action.oac_header->type)) {
		case openflow12::OFPAT_SET_FIELD: {
			coxmatch oxm(
					(struct openflow::ofp_oxm_hdr*)action.oac_12set_field->field,
					be16toh(action.oac_12set_field->len));
			acfields[oxm.get_oxm_class()][oxm.get_oxm_field()] = action;
		} break;
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


