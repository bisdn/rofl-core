/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coftablefeatureprop.cc
 *
 *  Created on: 02.02.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/coftablefeatureprop.h"

using namespace rofl::openflow;


coftable_feature_prop::coftable_feature_prop() :
		rofl::cmemory(sizeof(struct openflow13::ofp_table_feature_prop_header)),
		ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN)
{
	ofp_tfp = somem();
}


coftable_feature_prop::coftable_feature_prop(
		uint8_t ofp_version,
		uint16_t type,
		size_t len) :
				ofp_version(ofp_version)
{
	resize(len);
	set_type(type);
}


coftable_feature_prop::~coftable_feature_prop()
{

}


coftable_feature_prop::coftable_feature_prop(
			coftable_feature_prop const& tfp)
{
	*this = tfp;
}


coftable_feature_prop&
coftable_feature_prop::operator= (
			coftable_feature_prop const& tfp)
{
	if (this == &tfp)
		return *this;

	ofp_version = tfp.ofp_version;

	cmemory::operator= (tfp);

	ofp_tfp = somem();

	return *this;
}


size_t
coftable_feature_prop::length() const
{
	return cmemory::memlen();
}


void
coftable_feature_prop::pack(
			uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length()) {
		throw eOFTableFeaturePropInval();
	}

	set_length(length());

	cmemory::pack(buf, buflen);
}


void
coftable_feature_prop::unpack(
			uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		throw eOFTableFeaturePropInval();
	}

	cmemory::unpack(buf, buflen);

	ofp_tfp = cmemory::somem();
}


uint8_t
coftable_feature_prop::get_version() const
{
	return ofp_version;
}


uint8_t*
coftable_feature_prop::resize(size_t size)
{
	return (ofp_tfp = cmemory::resize(size));
}


uint16_t
coftable_feature_prop::get_type() const
{
	return be16toh(ofp_tfphdr->type);
}


void
coftable_feature_prop::set_type(uint16_t type)
{
	ofp_tfphdr->type = htobe16(type);
}


uint16_t
coftable_feature_prop::get_length() const
{
	return be16toh(ofp_tfphdr->length);
}


void
coftable_feature_prop::set_length(uint16_t len)
{
	ofp_tfphdr->length = htobe16(len);
}









/*
 * struct ofp_table_feature_prop_instructions
 */

coftable_feature_prop_instructions::coftable_feature_prop_instructions(
		uint8_t ofp_version,
		uint16_t type) :
				coftable_feature_prop(ofp_version, type, sizeof(struct openflow13::ofp_table_feature_prop_instructions))
{

}


coftable_feature_prop_instructions::~coftable_feature_prop_instructions()
{

}


coftable_feature_prop_instructions::coftable_feature_prop_instructions(
		coftable_feature_prop_instructions const& tfpi)
{
	*this = tfpi;
}


coftable_feature_prop_instructions&
coftable_feature_prop_instructions::operator= (
		coftable_feature_prop_instructions const& tfpi)
{
	if (this == &tfpi)
		return *this;

	coftable_feature_prop::operator= (tfpi);
	ofh_tfpi = somem();

	instructions_ids.clear();
	for (std::vector<rofl::openflow::ofp_instruction>::const_iterator
			it = tfpi.instructions_ids.begin(); it != tfpi.instructions_ids.end(); ++it) {
		instructions_ids.push_back(*it);
	}
	//std::copy(tfpi.instructions_ids.begin(), tfpi.instructions_ids.end(), instructions_ids.begin());

	return *this;
}


void
coftable_feature_prop_instructions::clear()
{
	instructions_ids.clear();
	resize(sizeof(struct rofl::openflow13::ofp_table_feature_prop_instructions));
	set_length(sizeof(struct rofl::openflow13::ofp_table_feature_prop_instructions));
}


size_t
coftable_feature_prop_instructions::length() const
{
	// TODO: support for experimental instructions
	size_t total_length = sizeof(struct openflow13::ofp_table_feature_prop_header) +
			instructions_ids.size() * sizeof(struct openflow::ofp_instruction);

	size_t pad = (0x7 & total_length);
	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}
	return total_length;
}


void
coftable_feature_prop_instructions::pack(
			uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length()) {
		throw eOFTableFeaturePropInval();
	}

	// resize internal buffer (including padding)
	resize(buflen = length());

	// set length field (excluding padding)
	set_length(sizeof(struct openflow13::ofp_table_feature_prop_header) +
			instructions_ids.size() * sizeof(struct openflow::ofp_instruction)); // without padding

	// copy internal buffer into external [buf, buflen]
	memcpy(buf, somem(), sizeof(struct rofl::openflow13::ofp_table_feature_prop_instructions));
	buf += sizeof(struct rofl::openflow13::ofp_table_feature_prop_instructions);

	// fill in instruction-ids (internal buffer)
	for (unsigned int i = 0; i < instructions_ids.size(); i++) {
		struct rofl::openflow::ofp_instruction* inst = (struct rofl::openflow::ofp_instruction*)buf;
		inst->type = htobe16(instructions_ids[i].type);
		inst->len  = htobe16(instructions_ids[i].len);
		buf += sizeof(struct rofl::openflow::ofp_instruction);
	}
}


void
coftable_feature_prop_instructions::unpack(
			uint8_t* buf, size_t buflen)
{
	try {
		if ((0 == buf) || (buflen < sizeof(struct openflow13::ofp_table_feature_prop_instructions))) {
			throw eOFTableFeaturePropInval();
		}

		instructions_ids.clear();

		coftable_feature_prop::unpack(buf, buflen);

		ofh_tfpi = somem();

		// sanity check: length field must contain at least sizeof tfpi header
		if (get_length() < sizeof(struct openflow13::ofp_table_feature_prop_instructions)) {
			throw eOFTableFeaturePropInval();
		}

		// sanity check: overall buflen must contain length field from tfpihdr + padding
		size_t total_length = get_length();
		size_t pad = (0x7 & total_length);
		/* append padding if not a multiple of 8 */
		if (pad) {
			total_length += 8 - pad;
		}
		if (buflen < total_length) {
			throw eOFTableFeaturePropInval();
		}

		buf += sizeof(struct rofl::openflow13::ofp_table_feature_prop_instructions);
		while (buflen > sizeof(struct rofl::openflow::ofp_instruction)) {
			// TODO: experimental instruction-ids
			struct rofl::openflow::ofp_instruction *inst = (struct rofl::openflow::ofp_instruction*)buf;
			struct rofl::openflow::ofp_instruction instruction;
			instruction.type 	= be16toh(inst->type);
			instruction.len 	= be16toh(inst->len);

			if (instruction.len < sizeof(struct rofl::openflow::ofp_instruction))
				break;

			instructions_ids.push_back(instruction);
			buf += be16toh(inst->len);
			buflen -= be16toh(inst->len);
		}
	} catch (eInstructionInvalType& e) {
		// padding
	}
}


uint8_t*
coftable_feature_prop_instructions::resize(
		size_t size)
{
	return (ofh_tfpi = coftable_feature_prop::resize(size));
}


void
coftable_feature_prop_instructions::add_instruction(
		uint16_t type, uint16_t len)
{
	struct rofl::openflow::ofp_instruction instruction;
	instruction.type = type; 	// yes, host-byte-order
	instruction.len  = len;		// yes, host-byte-order
	instructions_ids.push_back(instruction);
}














/*
 * struct ofp_table_feature_prop_next_tables
 */

coftable_feature_prop_next_tables::coftable_feature_prop_next_tables(
		uint8_t ofp_version,
		uint16_t type) :
				coftable_feature_prop(ofp_version, type, sizeof(struct openflow13::ofp_table_feature_prop_next_tables))
{

}


coftable_feature_prop_next_tables::~coftable_feature_prop_next_tables()
{

}


coftable_feature_prop_next_tables::coftable_feature_prop_next_tables(
		coftable_feature_prop_next_tables const& tfpnxt)
{
	*this = tfpnxt;
}


coftable_feature_prop_next_tables&
coftable_feature_prop_next_tables::operator= (
		coftable_feature_prop_next_tables const& tfpnxt)
{
	if (this == &tfpnxt)
		return *this;

	coftable_feature_prop::operator= (tfpnxt);
	ofh_tfpnxt = somem();

	std::vector<uint8_t>::clear();
	for (std::vector<uint8_t>::const_iterator
			it = tfpnxt.begin(); it != tfpnxt.end(); ++it) {
		(*this).push_back(*it);
	}
	//std::copy(tfpnxt.begin(), tfpnxt.end(), std::vector<uint8_t>::begin());

	return *this;
}


void
coftable_feature_prop_next_tables::clear()
{
	std::vector<uint8_t>::clear();
	resize(sizeof(struct rofl::openflow13::ofp_table_feature_prop_next_tables));
	set_length(sizeof(struct rofl::openflow13::ofp_table_feature_prop_next_tables));
}


size_t
coftable_feature_prop_next_tables::length() const
{
	size_t total_length = sizeof(struct openflow13::ofp_table_feature_prop_next_tables) +
			std::vector<uint8_t>::size() * sizeof(uint8_t);

	size_t pad = (0x7 & total_length);
	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}
	return total_length;
}


void
coftable_feature_prop_next_tables::pack(
			uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length()) {
		throw eOFTableFeaturePropInval();
	}

	// resize internal buffer (including padding)
	resize(buflen = length());

	// set length field (excluding padding)
	set_length(sizeof(struct openflow13::ofp_table_feature_prop_next_tables) +
				(*this).size() * sizeof(uint8_t)); // without padding

	// copy internal buffer into external [buf, buflen]
	memcpy(buf, somem(), sizeof(struct rofl::openflow13::ofp_table_feature_prop_next_tables));
	buf += sizeof(struct rofl::openflow13::ofp_table_feature_prop_next_tables);

	// fill in next-tables-ids (internal buffer)
	for (unsigned int i = 0; i < std::vector<uint8_t>::size(); i++) {
		buf[i] = std::vector<uint8_t>::operator[](i);
	}
}


void
coftable_feature_prop_next_tables::unpack(
			uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (buflen < sizeof(struct openflow13::ofp_table_feature_prop_next_tables))) {
		throw eOFTableFeaturePropInval();
	}

	std::vector<uint8_t>::clear();

	coftable_feature_prop::unpack(buf, buflen);

	ofh_tfpnxt = somem();

	// sanity check: length field must contain at least sizeof tfpnxt header
	if (get_length() < sizeof(struct openflow13::ofp_table_feature_prop_next_tables)) {
		throw eOFTableFeaturePropInval();
	}

	// sanity check: overall buflen must contain length field from tfpnxthdr + padding
	size_t total_length = get_length();
	size_t pad = (0x7 & total_length);
	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}
	if (buflen < total_length) {
		throw eOFTableFeaturePropInval();
	}

	// #next-tables entries
	unsigned int n_next_table_ids =
			(get_length() - sizeof(struct openflow13::ofp_table_feature_prop_header)) /
														sizeof(uint8_t);

	for (unsigned int i = 0; i < n_next_table_ids; i++) {
		std::vector<uint8_t>::push_back(ofh_tfpnxthdr->next_table_ids[i]);
	}
}


uint8_t*
coftable_feature_prop_next_tables::resize(
		size_t size)
{
	return (ofh_tfpnxt = coftable_feature_prop::resize(size));
}


void
coftable_feature_prop_next_tables::add_table_id(
		uint8_t table_id)
{
	(*this).push_back(table_id);
}













/*
 * struct ofp_table_feature_prop_actions
 */

coftable_feature_prop_actions::coftable_feature_prop_actions(
		uint8_t ofp_version,
		uint16_t type) :
				coftable_feature_prop(ofp_version, type, sizeof(struct openflow13::ofp_table_feature_prop_actions))
{

}


coftable_feature_prop_actions::~coftable_feature_prop_actions()
{

}


coftable_feature_prop_actions::coftable_feature_prop_actions(
		coftable_feature_prop_actions const& tfpa)
{
	*this = tfpa;
}


coftable_feature_prop_actions&
coftable_feature_prop_actions::operator= (
		coftable_feature_prop_actions const& tfpa)
{
	if (this == &tfpa)
		return *this;

	coftable_feature_prop::operator= (tfpa);
	ofh_tfpa = somem();

	actions.clear();
	for (std::vector<struct rofl::openflow::ofp_action>::const_iterator
			it = tfpa.actions.begin(); it != tfpa.actions.end(); ++it) {
		actions.push_back(*it);
	}
	//std::copy(tfpa.begin(), tfpa.end(), std::vector<std::pair<uint16_t, uint16_t> >::begin());

	return *this;
}


void
coftable_feature_prop_actions::clear()
{
	actions.clear();
	resize(sizeof(struct rofl::openflow13::ofp_table_feature_prop_actions));
	set_length(sizeof(struct rofl::openflow13::ofp_table_feature_prop_actions));
}


size_t
coftable_feature_prop_actions::length() const
{
	// TODO: support for experimental actions
	size_t total_length = sizeof(struct openflow13::ofp_table_feature_prop_header) +
			actions.size() * sizeof(struct openflow::ofp_action);

	size_t pad = (0x7 & total_length);
	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}
	return total_length;
}


void
coftable_feature_prop_actions::pack(
			uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length()) {
		throw eOFTableFeaturePropInval();
	}

	// resize internal buffer (including padding)
	resize(buflen = length());

	// set length field (excluding padding)
	set_length(sizeof(struct rofl::openflow13::ofp_table_feature_prop_header) +
			actions.size() * sizeof(struct openflow::ofp_action)); // without padding

	memcpy(buf, somem(), sizeof(struct rofl::openflow13::ofp_table_feature_prop_header));

	buf += sizeof(struct rofl::openflow13::ofp_table_feature_prop_header);

	// fill in next-tables-ids (internal buffer)
	for (std::vector<struct rofl::openflow::ofp_action>::iterator
			it = actions.begin(); it != actions.end(); ++it) {
		struct rofl::openflow::ofp_action *action = (struct rofl::openflow::ofp_action*)buf;
		action->type = htobe16(it->type);
		action->len  = htobe16(it->len);
		buf += sizeof(struct rofl::openflow::ofp_action);
	}
}


void
coftable_feature_prop_actions::unpack(
			uint8_t* buf, size_t buflen)
{
	try {
		if ((0 == buf) || (buflen < sizeof(struct openflow13::ofp_table_feature_prop_actions))) {
			throw eOFTableFeaturePropInval();
		}

		clear();

		coftable_feature_prop::unpack(buf, buflen);

		ofh_tfpa = somem();

		// sanity check: length field must contain at least sizeof tfpi header
		if (get_length() < sizeof(struct openflow13::ofp_table_feature_prop_actions)) {
			throw eOFTableFeaturePropInval();
		}

		// sanity check: overall buflen must contain length field from tfpihdr + padding
		size_t total_length = get_length();
		size_t pad = (0x7 & total_length);
		/* append padding if not a multiple of 8 */
		if (pad) {
			total_length += 8 - pad;
		}
		if (buflen < total_length) {
			throw eOFTableFeaturePropInval();
		}

		// #action-id entries
		unsigned int n_action_ids =
				(get_length() - sizeof(struct rofl::openflow13::ofp_table_feature_prop_header)) /
															sizeof(struct rofl::openflow::ofp_action);

		buf += sizeof(struct rofl::openflow13::ofp_table_feature_prop_header);

		for (unsigned int i = 0; i < n_action_ids; i++) {
			struct rofl::openflow::ofp_action* act = (struct rofl::openflow::ofp_action*)buf;
			struct rofl::openflow::ofp_action action;
			action.type = be16toh(act->type);
			action.len  = be16toh(act->len);
			actions.push_back(action);
			buf += sizeof(struct rofl::openflow::ofp_action);
			// TODO: experimental action-ids
		}
	} catch (eBadActionBadType& e) {
		// padding
	}
}


uint8_t*
coftable_feature_prop_actions::resize(
		size_t size)
{
	return (ofh_tfpa = coftable_feature_prop::resize(size));
}


void
coftable_feature_prop_actions::add_action(
		uint16_t type, uint16_t len)
{
	struct rofl::openflow::ofp_action action;
	action.type = type;
	action.len  = len;
	actions.push_back(action);
}












/*
 * struct ofp_table_feature_prop_oxm
 */

coftable_feature_prop_oxm::coftable_feature_prop_oxm(
		uint8_t ofp_version,
		uint16_t type) :
				coftable_feature_prop(ofp_version, type, sizeof(struct openflow13::ofp_table_feature_prop_oxm))
{

}


coftable_feature_prop_oxm::~coftable_feature_prop_oxm()
{

}


coftable_feature_prop_oxm::coftable_feature_prop_oxm(
		coftable_feature_prop_oxm const& tfpoxm)
{
	*this = tfpoxm;
}


coftable_feature_prop_oxm&
coftable_feature_prop_oxm::operator= (
		coftable_feature_prop_oxm const& tfpoxm)
{
	if (this == &tfpoxm)
		return *this;

	coftable_feature_prop::operator= (tfpoxm);
	ofh_tfpoxm = somem();

	oxm_ids.clear();
	for (std::vector<uint32_t>::const_iterator
			it = tfpoxm.oxm_ids.begin(); it != tfpoxm.oxm_ids.end(); ++it) {
		oxm_ids.push_back(*it);
	}
	//std::copy(tfpoxm.oxm_ids.begin(), tfpoxm.oxm_ids.end(), oxm_ids.begin());
	oxm_ids_exp.clear();
	for (std::vector<uint64_t>::const_iterator
			it = tfpoxm.oxm_ids_exp.begin(); it != tfpoxm.oxm_ids_exp.end(); ++it) {
		oxm_ids_exp.push_back(*it);
	}
	//std::copy(tfpoxm.oxm_ids_exp.begin(), tfpoxm.oxm_ids_exp.end(), oxm_ids_exp.begin());

	return *this;
}


void
coftable_feature_prop_oxm::clear()
{
	oxm_ids.clear();
	oxm_ids_exp.clear();
	resize(sizeof(struct rofl::openflow13::ofp_table_feature_prop_header));
	set_length(sizeof(struct rofl::openflow13::ofp_table_feature_prop_header));
}


size_t
coftable_feature_prop_oxm::length() const
{
	size_t total_length = sizeof(struct openflow13::ofp_table_feature_prop_oxm) +
			oxm_ids.size() * sizeof(uint32_t) + oxm_ids_exp.size() * sizeof(uint64_t);

	size_t pad = (0x7 & total_length);
	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}
	return total_length;
}


void
coftable_feature_prop_oxm::pack(
			uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length()) {
		throw eOFTableFeaturePropInval();
	}

	// resize internal buffer (including padding)
	resize(buflen = length());

	// set length field (excluding padding)
	set_length(sizeof(struct rofl::openflow13::ofp_table_feature_prop_oxm) +
			oxm_ids.size() * sizeof(uint32_t) + oxm_ids_exp.size() * sizeof(uint64_t)); // without padding

	memcpy(buf, somem(), sizeof(struct rofl::openflow13::ofp_table_feature_prop_oxm));

	// fill in oxm-ids (internal buffer)
	uint32_t *uint32 = (uint32_t*)((uint8_t*)(buf + sizeof(struct rofl::openflow13::ofp_table_feature_prop_oxm)));
	for (unsigned int i = 0; i < oxm_ids.size(); i++) {
		uint32[i] = htobe32(oxm_ids[i]);
	}

	// fill in experimental oxm-ids (internal buffer)
	uint64_t *uint64 = (uint64_t*)((uint8_t*)(buf + sizeof(struct rofl::openflow13::ofp_table_feature_prop_oxm)) + oxm_ids.size() * sizeof(uint32_t));
	for (unsigned int i = 0; i < oxm_ids_exp.size(); i++) {
		uint64[i] = htobe64(oxm_ids_exp[i]);
	}
}


void
coftable_feature_prop_oxm::unpack(
			uint8_t* buf, size_t buflen)
{
	if ((0 == buf) || (buflen < sizeof(struct openflow13::ofp_table_feature_prop_oxm))) {
		throw eOFTableFeaturePropInval();
	}

	oxm_ids.clear();
	oxm_ids_exp.clear();

	coftable_feature_prop::unpack(buf, buflen);

	ofh_tfpoxm = somem();

	// sanity check: length field must contain at least sizeof tfpoxm header
	if (get_length() < sizeof(struct openflow13::ofp_table_feature_prop_oxm)) {
		throw eOFTableFeaturePropInval();
	}

	// sanity check: overall buflen must contain length field from tfpoxmhdr + padding
	size_t total_length = get_length();
	size_t pad = (0x7 & total_length);
	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}
	if (buflen < total_length) {
		throw eOFTableFeaturePropInval();
	}


	size_t remaining = get_length() - sizeof(struct openflow13::ofp_table_feature_prop_oxm);
	while (remaining >= sizeof(uint32_t)) {
		struct openflow::ofp_oxm_hdr *oxm = (struct openflow::ofp_oxm_hdr*)
				(((struct openflow13::ofp_table_feature_prop_oxm*)buf)->oxm_ids);

		switch (be16toh(oxm->oxm_class)) {
		case 0: return;

		case rofl::openflow::OFPXMC_EXPERIMENTER: {
			if (remaining < sizeof(uint64_t)) {
				return;
			}
			oxm_ids_exp.push_back(be64toh(*(uint64_t*)(oxm)));
			buf += sizeof(uint64_t);
			remaining -= sizeof(uint64_t);
		} break;
		case rofl::openflow::OFPXMC_OPENFLOW_BASIC: {
			if (remaining < sizeof(uint32_t)) {
				return;
			}
			oxm_ids.    push_back(be32toh(*(uint32_t*)(oxm)));
			buf += sizeof(uint32_t);
			remaining -= sizeof(uint32_t);
		} break;
		default: {
			// do nothing ?
		};
		}
	}
}


uint8_t*
coftable_feature_prop_oxm::resize(
		size_t size)
{
	return (ofh_tfpoxm = coftable_feature_prop::resize(size));
}


void
coftable_feature_prop_oxm::add_oxm(
		uint32_t oxm_id)
{
	oxm_ids.push_back(oxm_id);
}


void
coftable_feature_prop_oxm::add_oxm_exp(
		uint64_t oxm_exp_id)
{
	oxm_ids_exp.push_back(oxm_exp_id);
}










