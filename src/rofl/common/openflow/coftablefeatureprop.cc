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
	std::copy(tfpi.instructions_ids.begin(), tfpi.instructions_ids.end(), instructions_ids.begin());

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

	// fill in instruction-ids (internal buffer)
	for (unsigned int i = 0; i < instructions_ids.size(); i++) {
		ofh_tfpihdr->instruction_ids[i].type = htobe16(instructions_ids[i].first);
		ofh_tfpihdr->instruction_ids[i].len  = htobe16(instructions_ids[i].second);
	}

	// copy internal buffer into external [buf, buflen]
	coftable_feature_prop::pack(buf, buflen);
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

		buf += sizeof(struct rofl::openflow13::ofp_table_feature_prop_header);
		while (buflen > sizeof(struct rofl::openflow::ofp_instruction)) {
			// TODO: experimental instruction-ids
			struct rofl::openflow::ofp_instruction *inst = (struct rofl::openflow::ofp_instruction*)buf;
			if (be16toh(inst->len) < sizeof(struct rofl::openflow::ofp_instruction))
				break;
			instructions_ids.push_back(std::pair<uint16_t, uint16_t>(be16toh(inst->type), be16toh(inst->len)));
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
	std::copy(tfpnxt.begin(), tfpnxt.end(), std::vector<uint8_t>::begin());

	return *this;
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
				std::vector<uint8_t>::size() * sizeof(uint8_t)); // without padding

	// fill in next-tables-ids (internal buffer)
	for (unsigned int i = 0; i < std::vector<uint8_t>::size(); i++) {
		ofh_tfpnxthdr->next_table_ids[i] = std::vector<uint8_t>::operator[](i);
	}

	// copy internal buffer into external [buf, buflen]
	coftable_feature_prop::pack(buf, buflen);
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

	std::vector<cofaction>::clear();
	std::copy(tfpa.begin(), tfpa.end(), std::vector<cofaction>::begin());

	return *this;
}


size_t
coftable_feature_prop_actions::length() const
{
	// TODO: support for experimental actions
	size_t total_length = sizeof(struct openflow13::ofp_table_feature_prop_header) +
			std::vector<cofaction>::size() * sizeof(struct openflow::ofp_action);

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
	set_length(sizeof(struct openflow13::ofp_table_feature_prop_header) +
			std::vector<cofaction>::size() * sizeof(struct openflow::ofp_action)); // without padding

	// fill in next-tables-ids (internal buffer)
	for (unsigned int i = 0; i < std::vector<cofaction>::size(); i++) {
		ofh_tfpahdr->action_ids[i].type = htobe16(std::vector<cofaction>::operator[](i).get_type());
		ofh_tfpahdr->action_ids[i].len  = htobe16(std::vector<cofaction>::operator[](i).get_length());
	}

	// copy internal buffer into external [buf, buflen]
	coftable_feature_prop::pack(buf, buflen);
}


void
coftable_feature_prop_actions::unpack(
			uint8_t* buf, size_t buflen)
{
	try {
		if ((0 == buf) || (buflen < sizeof(struct openflow13::ofp_table_feature_prop_actions))) {
			throw eOFTableFeaturePropInval();
		}

		std::vector<cofaction>::clear();

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
				(get_length() - sizeof(struct openflow13::ofp_table_feature_prop_header)) /
															sizeof(struct openflow::ofp_action);

		// TODO: experimental action-ids
		for (unsigned int i = 0; i < n_action_ids; i++) {
			std::vector<cofaction>::push_back(cofaction(get_version(),
					(struct openflow::ofp_action_header*)&(ofh_tfpahdr->action_ids[i]),
									sizeof(struct openflow::ofp_action)));
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
	std::copy(tfpoxm.oxm_ids.begin(), tfpoxm.oxm_ids.end(), oxm_ids.begin());
	oxm_ids_exp.clear();
	std::copy(tfpoxm.oxm_ids_exp.begin(), tfpoxm.oxm_ids_exp.end(), oxm_ids_exp.begin());

	return *this;
}


size_t
coftable_feature_prop_oxm::length() const
{
	size_t total_length = sizeof(struct openflow13::ofp_table_feature_prop_oxm) +
			oxm_ids.size() * sizeof(uint32_t) * oxm_ids_exp.size() * sizeof(uint64_t);

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
	set_length(sizeof(struct openflow13::ofp_table_feature_prop_oxm) +
			oxm_ids.size() * sizeof(uint32_t) * oxm_ids_exp.size() * sizeof(uint64_t)); // without padding

	// fill in oxm-ids (internal buffer)
	uint32_t *uint32 = (uint32_t*)((uint8_t*)(ofh_tfpoxmhdr->oxm_ids));
	for (unsigned int i = 0; i < oxm_ids.size(); i++) {
		uint32[i] = htobe32(oxm_ids[i]);
	}

	// fill in experimental oxm-ids (internal buffer)
	uint64_t *uint64 = (uint64_t*)((uint8_t*)(ofh_tfpoxmhdr->oxm_ids) + oxm_ids.size() * sizeof(uint32_t));
	for (unsigned int i = 0; i < oxm_ids_exp.size(); i++) {
		uint64[i] = htobe64(oxm_ids_exp[i]);
	}

	// copy internal buffer into external [buf, buflen]
	coftable_feature_prop::pack(buf, buflen);
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


	size_t remaining = total_length - sizeof(struct openflow13::ofp_table_feature_prop_oxm);
	while (remaining > sizeof(uint32_t)) {
		struct openflow::ofp_oxm_hdr *oxm = (struct openflow::ofp_oxm_hdr*)
				(((struct openflow13::ofp_table_feature_prop_oxm*)buf)->oxm_ids);

		switch (be16toh(oxm->oxm_class)) {
		case OFPXMC_EXPERIMENTER: {
			if (remaining < sizeof(uint64_t)) {
				return;
			}
			oxm_ids_exp.push_back(be64toh(*(uint64_t*)(oxm)));
			remaining -= sizeof(uint64_t);
		} break;
		default: {
			if (remaining < sizeof(uint32_t)) {
				return;
			}
			oxm_ids.    push_back(be32toh(*(uint32_t*)(oxm)));
			remaining -= sizeof(uint32_t);
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












