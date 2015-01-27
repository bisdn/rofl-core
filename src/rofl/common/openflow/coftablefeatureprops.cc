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



size_t
coftable_feature_props::length() const
{
	size_t len = 0;
	for (std::map<uint16_t, coftable_feature_prop*>::const_iterator
			it = tfprops.begin(); it != tfprops.end(); ++it) {
		len += it->second->length();
	}
	return len;
}



void
coftable_feature_props::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	size_t total_length = length();

	if (buflen < total_length) {
		throw eOFTableFeaturePropsInval();
	}

	for (std::map<uint16_t, coftable_feature_prop*>::iterator
			it = tfprops.begin(); it != tfprops.end(); ++it) {
		size_t prop_len = it->second->length();
		if (sizeof(struct openflow13::ofp_table_feature_prop_header) > prop_len) {
			LOGGING_ERROR << "[rofl][table-feature-props] internal error, tfp with invalid length" << std::endl;
			assert(sizeof(struct openflow13::ofp_table_feature_prop_header) <= prop_len);
			return;
		}
		it->second->pack(buf, prop_len);
		buf += prop_len;
	}
}



void
coftable_feature_props::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	coftable_feature_props::clear();

	while (buflen > 0) {
		struct openflow13::ofp_table_feature_prop_header *prop = (struct openflow13::ofp_table_feature_prop_header*)buf;

		if (buflen < sizeof(struct openflow13::ofp_table_feature_prop_header)) {
			return; // ignore padding
		}

		if (be16toh(prop->length) < sizeof(struct openflow13::ofp_table_feature_prop_header)) {
			throw eTableFeaturesReqBadLen();
		}

		size_t total_length = be16toh(prop->length) + ((0x7 & be16toh(prop->length)) ? 8 - (0x7 & be16toh(prop->length)) : 0);
		if (total_length > buflen) {
			throw eTableFeaturesReqBadLen();
		}

		switch (be16toh(prop->type)) {
		case rofl::openflow13::OFPTFPT_INSTRUCTIONS:
		case rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS: {
			(tfprops[be16toh(prop->type)] = new coftable_feature_prop_instructions(
					ofp_version, be16toh(prop->type)))->unpack(buf, total_length);
		} break;
		case rofl::openflow13::OFPTFPT_NEXT_TABLES:
		case rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS: {
			(tfprops[be16toh(prop->type)] = new coftable_feature_prop_next_tables(
					ofp_version, be16toh(prop->type)))->unpack(buf, total_length);
		} break;
		case rofl::openflow13::OFPTFPT_WRITE_ACTIONS:
		case rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS:
		case rofl::openflow13::OFPTFPT_APPLY_ACTIONS:
		case rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS: {
			(tfprops[be16toh(prop->type)] = new coftable_feature_prop_actions(
					ofp_version, be16toh(prop->type)))->unpack(buf, total_length);
		} break;
		case rofl::openflow13::OFPTFPT_MATCH:
		case rofl::openflow13::OFPTFPT_WILDCARDS:
		case rofl::openflow13::OFPTFPT_WRITE_SETFIELD:
		case rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS:
		case rofl::openflow13::OFPTFPT_APPLY_SETFIELD:
		case rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS: {
			(tfprops[be16toh(prop->type)] = new coftable_feature_prop_oxm(
					ofp_version, be16toh(prop->type)))->unpack(buf, total_length);
		} break;
		default: {
			throw eTableFeaturesReqBadType();
		} break;
		}

		buf += total_length;
		buflen -= total_length;
	}
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



const coftable_feature_prop&
coftable_feature_props::get_tfp(unsigned int type) const
{
	if (tfprops.find(type) == tfprops.end()) {
		throw eOFTableFeaturePropsNotFound();
	}
	return dynamic_cast<const coftable_feature_prop&>(*(tfprops.at(type)));
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
coftable_feature_props::has_tfp(unsigned int type) const
{
	return not (tfprops.find(type) == tfprops.end());
}






