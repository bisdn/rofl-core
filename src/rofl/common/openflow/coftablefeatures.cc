/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/coftablefeatures.h"

#ifndef htobe16
#include "../endian_conversion.h"
#endif


using namespace rofl::openflow;


coftable_features::coftable_features(
		uint8_t ofp_version) :
				rofl::cmemory(sizeof(struct rofl::openflow13::ofp_table_features)),
				ofp_version(ofp_version),
				table_feature_props(ofp_version)
{
	ofh_tf_generic = somem();
}


coftable_features::~coftable_features()
{}



coftable_features::coftable_features(
		coftable_features const& table_features)
{
	*this = table_features;
}



coftable_features&
coftable_features::operator= (
		coftable_features const& table_features)
{
	if (this == &table_features)
		return *this;

	ofh_tf_generic 		= rofl::cmemory::operator= (table_features).somem();
	ofp_version 		= table_features.ofp_version;
	table_feature_props	= table_features.table_feature_props;

	return *this;
}


size_t
coftable_features::length() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_table_features) + table_feature_props.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_features::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	set_length(length());

	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		memcpy(buf, somem(), sizeof(struct rofl::openflow13::ofp_table_features));
		table_feature_props.pack(buf + sizeof(struct rofl::openflow13::ofp_table_features), buflen - sizeof(struct rofl::openflow13::ofp_table_features));
	} break;
	default:
		throw eBadVersion();
	}
}



void
coftable_features::unpack(uint8_t *buf, size_t buflen)
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct rofl::openflow13::ofp_table_features)) {
			throw eOFTableFeaturesInval();
		}

		rofl::cmemory::unpack(buf, sizeof(struct rofl::openflow13::ofp_table_features));
		ofh_tf_generic = rofl::cmemory::somem();

		if (get_length() > buflen) {
			throw eTableFeaturesReqBadLen();
		}

		table_feature_props.clear();
		table_feature_props.unpack(buf + sizeof(struct rofl::openflow13::ofp_table_features),
									buflen - sizeof(struct rofl::openflow13::ofp_table_features));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



