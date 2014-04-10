/*
 * coftables.cc
 *
 *  Created on: 16.02.2014
 *      Author: andreas
 */

#include "rofl/common/openflow/coftables.h"

using namespace rofl::openflow;

coftables::coftables(
			uint8_t ofp_version) :
					ofp_version(ofp_version)
{

}



coftables::~coftables()
{

}



coftables::coftables(
			coftables const& tables)
{
	*this = tables;
}



coftables&
coftables::operator= (
			coftables const& tables)
{
	if (this == &tables)
		return *this;

	ofp_version = tables.ofp_version;

	this->tables.clear();
	for (std::map<uint8_t, coftable_features>::const_iterator
			it = tables.tables.begin(); it != tables.tables.end(); ++it) {
		this->tables[it->first] = it->second;
	}

	return *this;
}



coftables&
coftables::operator+= (
		coftables const& tables)
{
	/*
	 * this operation may replace tables, if they use the same table-id
	 */
	for (std::map<uint8_t, coftable_features>::const_iterator
			it = tables.tables.begin(); it != tables.tables.end(); ++it) {
		this->tables[it->first] = it->second;
	}

	return *this;
}



void
coftables::clear()
{
	tables.clear();
}



size_t
coftables::length() const
{
	size_t len = 0;
	for (std::map<uint8_t, coftable_features>::const_iterator
			it = tables.begin(); it != tables.end(); ++it) {
		len += it->second.length();
	}
	return len;
}



void
coftables::pack(
			uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	if (buflen < length()) {
		throw eOFTablesInval();
	}

	for (std::map<uint8_t, coftable_features>::iterator
			it = tables.begin(); it != tables.end(); ++it) {
		it->second.pack(buf, it->second.length());
		buf += it->second.length();
	}
}



void
coftables::unpack(
			uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen)) {
		return;
	}

	tables.clear();
	while (buflen > 0) {
		struct rofl::openflow13::ofp_table_features *table = (struct rofl::openflow13::ofp_table_features*)buf;

		if ((be16toh(table->length) > buflen) || (be16toh(table->length) < sizeof(struct rofl::openflow13::ofp_table_features))) {
			throw eTableFeaturesReqBadLen();
		}

		rofl::openflow::coftable_features table_features(get_version());
		table_features.unpack(buf, be16toh(table->length));
		tables[table->table_id] = table_features;

		buf += be16toh(table->length);
		buflen -= be16toh(table->length);
	}
}



coftable_features&
coftables::add_table(uint8_t table_id)
{
	if (tables.find(table_id) != tables.end()) {
		tables.erase(table_id);
	}
	tables[table_id] = coftable_features(get_version());
	tables[table_id].set_table_id(table_id);
	return tables[table_id];
}



void
coftables::drop_table(uint8_t table_id)
{
	if (tables.find(table_id) == tables.end()) {
		return;
	}
	tables.erase(table_id);
}



coftable_features&
coftables::get_table(uint8_t table_id)
{
	if (tables.find(table_id) == tables.end()) {
		throw eOFTablesNotFound();
	}
	return tables[table_id];
}



coftable_features&
coftables::set_table(uint8_t table_id)
{
	if (tables.find(table_id) == tables.end()) {
		tables[table_id] = coftable_features(get_version());
		tables[table_id].set_table_id(table_id);
	}
	return tables[table_id];
}



bool
coftables::has_table(uint8_t table_id)
{
	return (tables.find(table_id) != tables.end());
}



/*static*/void
coftables::map_tablestatsarray_to_tables(
		rofl::openflow::coftablestatsarray& tablestatsarray, rofl::openflow::coftables& tables)
{
	tables.clear();
	tables.set_version(rofl::openflow13::OFP_VERSION); // yes, OF13

	for (std::map<uint8_t, rofl::openflow::coftable_stats_reply>::const_iterator
			it = tablestatsarray.get_table_stats().begin();
					it != tablestatsarray.get_table_stats().end(); ++it) {
		uint8_t table_id = it->first;
		rofl::openflow::coftable_stats_reply const& table_stats = it->second;

		// add or reset table
		rofl::openflow::coftable_features& table = tables.add_table(table_id);

		// set table-id
		table.set_table_id(table_id);
		// set name
		table.set_name(table_stats.get_name());
		// set metadata-match
		table.set_metadata_match(table_stats.get_metadata_match());
		// set metadata-write
		table.set_metadata_write(table_stats.get_metadata_write());
		// set config
		table.set_config(table_stats.get_config());
		// set max-entries
		table.set_max_entries(table_stats.get_max_entries());


		// set supported matches
		map_match_to_prop_oxm(table_stats.get_match(), table.set_properties().set_tfp_match());

		// set supported wildcards
		map_match_to_prop_oxm(table_stats.get_wildcards(), table.set_properties().set_tfp_wildcards());

		// set supported write actions
		map_actions_to_prop_actions(table_stats.get_write_actions(), table.set_properties().set_tfp_write_actions());
		table.set_properties().set_tfp_write_actions_miss() = table.set_properties().get_tfp_write_actions();

		// set supported apply actions
		map_actions_to_prop_actions(table_stats.get_apply_actions(), table.set_properties().set_tfp_apply_actions());
		table.set_properties().set_tfp_apply_actions_miss() = table.set_properties().get_tfp_apply_actions();

		// set supported write setfields
		map_match_to_prop_oxm(table_stats.get_write_setfields(), table.set_properties().set_tfp_write_setfield());
		table.set_properties().set_tfp_write_setfield_miss() = table.set_properties().get_tfp_write_setfield();

		// set supported apply setfields
		map_match_to_prop_oxm(table_stats.get_apply_setfields(), table.set_properties().set_tfp_apply_setfield());
		table.set_properties().set_tfp_apply_setfield_miss() = table.set_properties().get_tfp_apply_setfield();

		// set instructions
		map_instructions_to_prop_instructions(table_stats.get_instructions(), table.set_properties().set_tfp_instructions());
		table.set_properties().set_tfp_instructions_miss() = table.set_properties().get_tfp_instructions();
	}
}



/*static*/void
coftables::map_tables_to_tablestatsarray(
		rofl::openflow::coftables& tables, rofl::openflow::coftablestatsarray& tablestatsarray)
{
	uint64_t match = 0;
	uint32_t actions = 0;
	uint32_t instructions = 0;

	tablestatsarray.clear();
	tablestatsarray.set_version(rofl::openflow12::OFP_VERSION);

	for (std::map<uint8_t, rofl::openflow::coftable_features>::iterator
			it = tables.set_tables().begin(); it != tables.set_tables().end(); ++it) {
		uint8_t table_id = it->first;
		rofl::openflow::coftable_features& table_features = it->second;

		// add or reset table
		rofl::openflow::coftable_stats_reply& table = tablestatsarray.add_table_stats(table_id);

		// set table-id
		table.set_table_id(table_id);
		// set name
		table.set_name(table_features.get_name());
		// set metadata-match
		table.set_metadata_match(table_features.get_metadata_match());
		// set metadata-write
		table.set_metadata_write(table_features.get_metadata_write());
		// set config
		table.set_config(table_features.get_config());
		// set max-entries
		table.set_max_entries(table_features.get_max_entries());


		// set supported matches
		map_prop_oxm_to_match(table_features.set_properties().get_tfp_match(), match);
		table.set_match(match);

		// set supported wildcards
		map_prop_oxm_to_match(table_features.set_properties().get_tfp_wildcards(), match);
		table.set_wildcards(match);

		// set supported write-actions
		map_prop_actions_to_actions(table_features.set_properties().get_tfp_write_actions(), actions);
		table.set_write_actions(actions);

		// set supported apply-actions
		map_prop_actions_to_actions(table_features.set_properties().get_tfp_apply_actions(), actions);
		table.set_apply_actions(actions);

		// set supported write-setfields
		map_prop_oxm_to_match(table_features.set_properties().get_tfp_write_setfield(), match);
		table.set_write_setfields(match);

		// set supported apply-setfields
		map_prop_oxm_to_match(table_features.set_properties().get_tfp_apply_setfield(), match);
		table.set_apply_setfields(match);

		// set supported instructions
		map_prop_instructions_to_instructions(table_features.set_properties().get_tfp_instructions(), instructions);
		table.set_instructions(instructions);
	}
}




/*static*/void
coftables::map_match_to_prop_oxm(
		uint64_t match, rofl::openflow::coftable_feature_prop_oxm& prop_oxm)
{
	//
	// TODO: masked values?
	//

	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PORT)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PORT);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PHY_PORT)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_METADATA)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_METADATA);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_DST)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_DST);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_SRC)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_SRC);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_TYPE)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_VID)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_VID);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_PCP)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_DSCP)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_DSCP);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_ECN)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_ECN);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_PROTO)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_PROTO);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_SRC)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_DST)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV4_DST);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_SRC)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_TCP_SRC);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_DST)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_TCP_DST);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_SRC)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_UDP_SRC);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_DST)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_UDP_DST);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_SRC)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_DST)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_SCTP_DST);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_TYPE)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_CODE)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_OP)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_OP);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SPA)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SPA);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_TPA)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_TPA);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SHA)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SHA);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_THA)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_THA);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_SRC)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_DST)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_DST);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_FLABEL)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_TYPE)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_CODE)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TARGET)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_SLL)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TLL)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_LABEL)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL);
	}
	if (match & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_TC)) {
		prop_oxm.add_oxm(rofl::openflow::OXM_TLV_BASIC_MPLS_TC);
	}
}



/*static*/void
coftables::map_prop_oxm_to_match(
		rofl::openflow::coftable_feature_prop_oxm& prop_oxm, uint64_t& match)
{
	match = 0;

	for (std::vector<uint32_t>::const_iterator
			it = prop_oxm.get_oxm_ids().begin(); it != prop_oxm.get_oxm_ids().end(); ++it) {
		switch (*it) {
		case rofl::openflow::OXM_TLV_BASIC_IN_PORT: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PORT);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PHY_PORT);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_METADATA:
		case rofl::openflow::OXM_TLV_BASIC_METADATA_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_METADATA);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ETH_DST:
		case rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_DST);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ETH_SRC:
		case rofl::openflow::OXM_TLV_BASIC_ETH_SRC_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_DST);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ETH_TYPE: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_TYPE);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_VLAN_VID:
		case rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_VID);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_VLAN_PCP: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_PCP);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IP_DSCP: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_DSCP);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IP_ECN: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_ECN);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IP_PROTO: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_PROTO);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IPV4_SRC:
		case rofl::openflow::OXM_TLV_BASIC_IPV4_SRC_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_SRC);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IPV4_DST:
		case rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_DST);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_TCP_SRC: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_SRC);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_TCP_DST: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_DST);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_UDP_SRC: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_SRC);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_UDP_DST: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_DST);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_SCTP_SRC: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_SRC);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_SCTP_DST: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_DST);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_TYPE);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_CODE);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ARP_OP: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_OP);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ARP_SPA:
		case rofl::openflow::OXM_TLV_BASIC_ARP_SPA_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SPA);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ARP_TPA:
		case rofl::openflow::OXM_TLV_BASIC_ARP_TPA_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_TPA);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ARP_SHA:
		case rofl::openflow::OXM_TLV_BASIC_ARP_SHA_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SHA);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ARP_THA:
		case rofl::openflow::OXM_TLV_BASIC_ARP_THA_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_THA);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IPV6_SRC:
		case rofl::openflow::OXM_TLV_BASIC_IPV6_SRC_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_SRC);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IPV6_DST:
		case rofl::openflow::OXM_TLV_BASIC_IPV6_DST_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_DST);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL:
		case rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL_MASK: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_FLABEL);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_TYPE);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_CODE);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TARGET);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_SLL);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TLL);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_LABEL);
		} break;
		case rofl::openflow::OXM_TLV_BASIC_MPLS_TC: {
			match |= (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_TC);
		} break;
		default: {
			// do nothing
		};
		}
	}
}



/*static*/void
coftables::map_actions_to_prop_actions(
		uint32_t actions, rofl::openflow::coftable_feature_prop_actions& prop_actions)
{
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_OUTPUT)) {
		prop_actions.add_action(rofl::openflow::OFPAT_OUTPUT);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_COPY_TTL_OUT)) {
		prop_actions.add_action(rofl::openflow::OFPAT_COPY_TTL_OUT);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_COPY_TTL_IN)) {
		prop_actions.add_action(rofl::openflow::OFPAT_COPY_TTL_IN);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_MPLS_TTL)) {
		prop_actions.add_action(rofl::openflow::OFPAT_SET_MPLS_TTL);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_DEC_MPLS_TTL)) {
		prop_actions.add_action(rofl::openflow::OFPAT_DEC_MPLS_TTL);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_PUSH_VLAN)) {
		prop_actions.add_action(rofl::openflow::OFPAT_PUSH_VLAN);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_POP_VLAN)) {
		prop_actions.add_action(rofl::openflow::OFPAT_POP_VLAN);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_PUSH_MPLS)) {
		prop_actions.add_action(rofl::openflow::OFPAT_PUSH_MPLS);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_POP_MPLS)) {
		prop_actions.add_action(rofl::openflow::OFPAT_POP_MPLS);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_QUEUE)) {
		prop_actions.add_action(rofl::openflow::OFPAT_SET_QUEUE);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_GROUP)) {
		prop_actions.add_action(rofl::openflow::OFPAT_GROUP);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_NW_TTL)) {
		prop_actions.add_action(rofl::openflow::OFPAT_SET_NW_TTL);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_DEC_NW_TTL)) {
		prop_actions.add_action(rofl::openflow::OFPAT_DEC_NW_TTL);
	}
	if (actions & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_FIELD)) {
		prop_actions.add_action(rofl::openflow::OFPAT_SET_FIELD);
	}
}



/*static*/void
coftables::map_prop_actions_to_actions(
	rofl::openflow::coftable_feature_prop_actions& prop_actions, uint32_t& actions)
{
	actions = 0;

	for (std::vector<struct rofl::openflow::ofp_action>::iterator
			it = prop_actions.get_action_ids().begin(); it != prop_actions.get_action_ids().end(); ++it) {
		switch (it->type) {
		case rofl::openflow::OFPAT_OUTPUT: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_OUTPUT);
		} break;
		case rofl::openflow::OFPAT_COPY_TTL_OUT: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_COPY_TTL_OUT);
		} break;
		case rofl::openflow::OFPAT_COPY_TTL_IN: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_COPY_TTL_IN);
		} break;
		case rofl::openflow::OFPAT_SET_MPLS_TTL: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_SET_MPLS_TTL);
		} break;
		case rofl::openflow::OFPAT_DEC_MPLS_TTL: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_DEC_MPLS_TTL);
		} break;
		case rofl::openflow::OFPAT_PUSH_VLAN: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_PUSH_VLAN);
		} break;
		case rofl::openflow::OFPAT_POP_VLAN: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_POP_VLAN);
		} break;
		case rofl::openflow::OFPAT_PUSH_MPLS: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_PUSH_MPLS);
		} break;
		case rofl::openflow::OFPAT_POP_MPLS: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_POP_MPLS);
		} break;
		case rofl::openflow::OFPAT_SET_QUEUE: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_SET_QUEUE);
		} break;
		case rofl::openflow::OFPAT_GROUP: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_GROUP);
		} break;
		case rofl::openflow::OFPAT_SET_NW_TTL: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_SET_NW_TTL);
		} break;
		case rofl::openflow::OFPAT_DEC_NW_TTL: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_DEC_NW_TTL);
		} break;
		case rofl::openflow::OFPAT_SET_FIELD: {
			actions |= (((uint32_t)1) << rofl::openflow12::OFPAT_SET_FIELD);
		} break;
		default: {
			// do nothing
		};
		}
	}
}



/*static*/void
coftables::map_instructions_to_prop_instructions(
		uint32_t instructions, rofl::openflow::coftable_feature_prop_instructions& prop_instructions)
{
	if (instructions & (((uint64_t)1) << rofl::openflow12::OFPIT_GOTO_TABLE)) {
		prop_instructions.add_instruction(rofl::openflow::OFPIT_GOTO_TABLE);
	}
	if (instructions & (((uint64_t)1) << rofl::openflow12::OFPIT_WRITE_METADATA)) {
		prop_instructions.add_instruction(rofl::openflow::OFPIT_WRITE_METADATA);
	}
	if (instructions & (((uint64_t)1) << rofl::openflow12::OFPIT_WRITE_ACTIONS)) {
		prop_instructions.add_instruction(rofl::openflow::OFPIT_WRITE_ACTIONS);
	}
	if (instructions & (((uint64_t)1) << rofl::openflow12::OFPIT_APPLY_ACTIONS)) {
		prop_instructions.add_instruction(rofl::openflow::OFPIT_APPLY_ACTIONS);
	}
	if (instructions & (((uint64_t)1) << rofl::openflow12::OFPIT_CLEAR_ACTIONS)) {
		prop_instructions.add_instruction(rofl::openflow::OFPIT_CLEAR_ACTIONS);
	}
}



/*static*/void
coftables::map_prop_instructions_to_instructions(
		rofl::openflow::coftable_feature_prop_instructions& prop_instructions, uint32_t& instructions)
{
	instructions = 0;

	for (std::vector<struct rofl::openflow::ofp_instruction>::iterator
			it = prop_instructions.get_instruction_ids().begin(); it != prop_instructions.get_instruction_ids().end(); ++it) {
		switch (it->type) {
		case rofl::openflow::OFPIT_GOTO_TABLE: {
			instructions |= (((uint64_t)1) << rofl::openflow12::OFPIT_GOTO_TABLE);
		} break;
		case rofl::openflow::OFPIT_WRITE_METADATA: {
			instructions |= (((uint64_t)1) << rofl::openflow12::OFPIT_WRITE_METADATA);
		} break;
		case rofl::openflow::OFPIT_WRITE_ACTIONS: {
			instructions |= (((uint64_t)1) << rofl::openflow12::OFPIT_WRITE_ACTIONS);
		} break;
		case rofl::openflow::OFPIT_APPLY_ACTIONS: {
			instructions |= (((uint64_t)1) << rofl::openflow12::OFPIT_APPLY_ACTIONS);
		} break;
		case rofl::openflow::OFPIT_CLEAR_ACTIONS: {
			instructions |= (((uint64_t)1) << rofl::openflow12::OFPIT_CLEAR_ACTIONS);
		} break;
		default: {
			// do nothing
		};
		}
	}
}



