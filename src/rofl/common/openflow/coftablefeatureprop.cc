/*
 * coftablefeatureprop.cc
 *
 *  Created on: 02.02.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/coftablefeatureprop.h"

using namespace rofl::openflow;

coftable_feature_prop::coftable_feature_prop(
		uint8_t ofp_version,
		size_t len) :
				ofp_version(ofp_version)
{
	resize(len);
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

	if (cmemory::memlen() < buflen) {
		resize(buflen);
	}

	cmemory::assign(buf, buflen);
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
		uint8_t ofp_version) :
				coftable_feature_prop(ofp_version, sizeof(struct openflow13::ofp_table_feature_prop_instructions))
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

	return *this;
}


size_t
coftable_feature_prop_instructions::length() const
{
	// TODO: support for experimental instructions
	size_t total_length = sizeof(struct openflow13::ofp_table_feature_prop_header) +
			instruction_ids.size() * sizeof(struct openflow::ofp_instruction);

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

	set_length(length());

	for (unsigned int i = 0; i < instruction_ids.size(); i++) {
		ofh_tfpihdr->instruction_ids[i].type = htobe16(instruction_ids[i].get_length());
		ofh_tfpihdr->instruction_ids[i].len  = htobe16(instruction_ids[i].get_type());
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

		instruction_ids.clear();

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

		// #instruction-id entries
		unsigned int n_instruction_ids =
				(get_length() - sizeof(struct openflow13::ofp_table_feature_prop_header)) /
															sizeof(struct openflow::ofp_instruction);

		// TODO: experimental instruction-ids
		for (unsigned int i = 0; i < n_instruction_ids; i++) {
			instruction_ids.push_back(cofinst(get_version(),
					(uint8_t*)&(ofh_tfpihdr->instruction_ids[i]), sizeof(struct openflow::ofp_instruction)));
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


