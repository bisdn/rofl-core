/*
 * coftablefeatureprops.cc
 *
 *  Created on: 04.02.2014
 *      Author: andi
 */

#include "rofl/common/openflow/coftablefeatureprops.h"

using namespace rofl::openflow;


coftable_feature_props::coftable_feature_props(uint8_t ofp_version) :
		ofp_version(ofp_version)
{

}



coftable_feature_props::~coftable_feature_props()
{
	coftable_feature_props::clear();
}



coftable_feature_props::coftable_feature_props(
		coftable_feature_props const& tfps)
{
	*this = tfps;
}



coftable_feature_props&
coftable_feature_props::operator= (
		coftable_feature_props const& tfps)
{
	if (this == &tfps)
		return *this;

	ofp_version = tfps.ofp_version;

	coftable_feature_props::clear();

	for (std::map<uint16_t, coftable_feature_prop*>::const_iterator
			it = tfps.tfprops.begin(); it != tfps.tfprops.end(); ++it) {

		unsigned int type = it->first;

		switch (type) {
		case rofl::openflow13::OFPTFPT_INSTRUCTIONS:
		case rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS: {
			tfprops[type] = new coftable_feature_prop_instructions(*dynamic_cast<coftable_feature_prop_instructions const*>( it->second ));
		} break;
		case rofl::openflow13::OFPTFPT_NEXT_TABLES:
		case rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS: {
			tfprops[type] = new coftable_feature_prop_next_tables(*dynamic_cast<coftable_feature_prop_next_tables const*>( it->second ));
		} break;
		case rofl::openflow13::OFPTFPT_WRITE_ACTIONS:
		case rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS:
		case rofl::openflow13::OFPTFPT_APPLY_ACTIONS:
		case rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS: {
			tfprops[type] = new coftable_feature_prop_actions(*dynamic_cast<coftable_feature_prop_actions const*>( it->second ));
		} break;
		case rofl::openflow13::OFPTFPT_MATCH:
		case rofl::openflow13::OFPTFPT_WILDCARDS:
		case rofl::openflow13::OFPTFPT_WRITE_SETFIELD:
		case rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS:
		case rofl::openflow13::OFPTFPT_APPLY_SETFIELD:
		case rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS: {
			tfprops[type] = new coftable_feature_prop_oxm(*dynamic_cast<coftable_feature_prop_oxm const*>( it->second ));
		} break;
		default: {
			tfprops[type] = new coftable_feature_prop(*dynamic_cast<coftable_feature_prop const*>( it->second ));
		} break;
		}
	}

	return *this;
}



void
coftable_feature_props::clear()
{
	for (std::map<uint16_t, coftable_feature_prop*>::iterator
				it = tfprops.begin(); it != tfprops.end(); ++it) {
		delete it->second;
	}
	tfprops.clear();
}



coftable_feature_prop&
coftable_feature_props::add_tfp(unsigned int type)
{
	if (tfprops.find(type) != tfprops.end()) {
		delete tfprops[type]; tfprops.erase(type);
	}

	switch (type) {
	case rofl::openflow13::OFPTFPT_INSTRUCTIONS:
	case rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS: {
		tfprops[type] = new coftable_feature_prop_instructions(ofp_version, type);
	} break;
	case rofl::openflow13::OFPTFPT_NEXT_TABLES:
	case rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS: {
		tfprops[type] = new coftable_feature_prop_next_tables(ofp_version, type);
	} break;
	case rofl::openflow13::OFPTFPT_WRITE_ACTIONS:
	case rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS:
	case rofl::openflow13::OFPTFPT_APPLY_ACTIONS:
	case rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS: {
		tfprops[type] = new coftable_feature_prop_actions(ofp_version, type);
	} break;
	case rofl::openflow13::OFPTFPT_MATCH:
	case rofl::openflow13::OFPTFPT_WILDCARDS:
	case rofl::openflow13::OFPTFPT_WRITE_SETFIELD:
	case rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS:
	case rofl::openflow13::OFPTFPT_APPLY_SETFIELD:
	case rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS: {
		tfprops[type] = new coftable_feature_prop_oxm(ofp_version, type);
	} break;
	default: {
		tfprops[type] = new coftable_feature_prop(ofp_version, type);
	} break;
	}
	return *(tfprops[type]);
}



coftable_feature_prop&
coftable_feature_props::set_tfp(unsigned int type)
{
	if (tfprops.find(type) == tfprops.end()) {
		switch (type) {
		case rofl::openflow13::OFPTFPT_INSTRUCTIONS:
		case rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS: {
			tfprops[type] = new coftable_feature_prop_instructions(ofp_version, type);
		} break;
		case rofl::openflow13::OFPTFPT_NEXT_TABLES:
		case rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS: {
			tfprops[type] = new coftable_feature_prop_next_tables(ofp_version, type);
		} break;
		case rofl::openflow13::OFPTFPT_WRITE_ACTIONS:
		case rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS:
		case rofl::openflow13::OFPTFPT_APPLY_ACTIONS:
		case rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS: {
			tfprops[type] = new coftable_feature_prop_actions(ofp_version, type);
		} break;
		case rofl::openflow13::OFPTFPT_MATCH:
		case rofl::openflow13::OFPTFPT_WILDCARDS:
		case rofl::openflow13::OFPTFPT_WRITE_SETFIELD:
		case rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS:
		case rofl::openflow13::OFPTFPT_APPLY_SETFIELD:
		case rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS: {
			tfprops[type] = new coftable_feature_prop_oxm(ofp_version, type);
		} break;
		default: {
			tfprops[type] = new coftable_feature_prop(ofp_version, type);
		} break;
		}
	}
	return *(tfprops[type]);
}



coftable_feature_prop&
coftable_feature_props::get_tfp(unsigned int type)
{
	if (tfprops.find(type) == tfprops.end()) {
		throw eOFTableFeaturePropsNotFound();
	}
	return *(tfprops[type]);
}



void
coftable_feature_props::drop_tfp(unsigned int type)
{
	if (tfprops.find(type) == tfprops.end()) {
		return;
	}
	delete tfprops[type]; tfprops.erase(type);
}



bool
coftable_feature_props::has_tfp(unsigned int type)
{
	return not (tfprops.find(type) == tfprops.end());
}






