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
		rofl::openflow::coftablestatsarray const& tablestatsarray, rofl::openflow::coftables& tables)
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

		//
		// TODO: masked values?
		//

		// set supported matches
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PORT)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PORT);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PHY_PORT)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_METADATA)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_METADATA);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_DST)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_DST);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_SRC)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_SRC);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_TYPE)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_VID)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_VID);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_PCP)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_DSCP)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_DSCP);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_ECN)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_ECN);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_PROTO)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_PROTO);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_SRC)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_DST)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV4_DST);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_SRC)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_TCP_SRC);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_DST)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_TCP_DST);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_SRC)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_UDP_SRC);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_DST)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_UDP_DST);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_SRC)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_DST)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_SCTP_DST);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_TYPE)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_CODE)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_OP)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_OP);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SPA)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SPA);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_TPA)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_TPA);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SHA)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SHA);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_THA)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_THA);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_SRC)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_DST)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_DST);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_FLABEL)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_TYPE)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_CODE)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TARGET)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_SLL)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TLL)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_LABEL)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL);
		}
		if (table_stats.get_match() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_TC)) {
			table.set_properties().set_tfp_match().add_oxm(rofl::openflow::OXM_TLV_BASIC_MPLS_TC);
		}


		//
		// TODO: masked values?
		//

		// set supported wildcards
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PORT)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PORT);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PHY_PORT)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_METADATA)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_METADATA);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_DST)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_DST);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_SRC)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_SRC);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_TYPE)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_VID)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_VID);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_PCP)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_DSCP)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_DSCP);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_ECN)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_ECN);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_PROTO)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_PROTO);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_SRC)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_DST)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV4_DST);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_SRC)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_TCP_SRC);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_DST)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_TCP_DST);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_SRC)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_UDP_SRC);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_DST)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_UDP_DST);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_SRC)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_DST)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_SCTP_DST);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_TYPE)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_CODE)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_OP)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_OP);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SPA)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SPA);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_TPA)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_TPA);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SHA)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SHA);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_THA)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_THA);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_SRC)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_DST)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_DST);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_FLABEL)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_TYPE)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_CODE)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TARGET)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_SLL)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TLL)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_LABEL)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL);
		}
		if (table_stats.get_wildcards() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_TC)) {
			table.set_properties().set_tfp_wildcards().add_oxm(rofl::openflow::OXM_TLV_BASIC_MPLS_TC);
		}



		// set supported write actions
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_OUTPUT)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_OUTPUT);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_COPY_TTL_OUT)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_COPY_TTL_OUT);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_COPY_TTL_IN)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_COPY_TTL_IN);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_MPLS_TTL)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_SET_MPLS_TTL);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_DEC_MPLS_TTL)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_DEC_MPLS_TTL);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_PUSH_VLAN)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_PUSH_VLAN);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_POP_VLAN)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_POP_VLAN);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_PUSH_MPLS)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_PUSH_MPLS);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_POP_MPLS)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_POP_MPLS);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_QUEUE)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_SET_QUEUE);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_GROUP)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_GROUP);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_NW_TTL)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_SET_NW_TTL);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_DEC_NW_TTL)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_DEC_NW_TTL);
		}
		if (table_stats.get_write_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_FIELD)) {
			table.set_properties().set_tfp_write_actions().add_action(rofl::openflow::OFPAT_SET_FIELD);
		}

		table.set_properties().set_tfp_write_actions_miss() = table.set_properties().get_tfp_write_actions();


		// set supported apply actions
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_OUTPUT)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_OUTPUT);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_COPY_TTL_OUT)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_COPY_TTL_OUT);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_COPY_TTL_IN)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_COPY_TTL_IN);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_MPLS_TTL)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_SET_MPLS_TTL);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_DEC_MPLS_TTL)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_DEC_MPLS_TTL);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_PUSH_VLAN)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_PUSH_VLAN);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_POP_VLAN)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_POP_VLAN);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_PUSH_MPLS)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_PUSH_MPLS);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_POP_MPLS)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_POP_MPLS);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_QUEUE)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_SET_QUEUE);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_GROUP)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_GROUP);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_NW_TTL)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_SET_NW_TTL);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_DEC_NW_TTL)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_DEC_NW_TTL);
		}
		if (table_stats.get_apply_actions() & (((uint32_t)1) << rofl::openflow12::OFPAT_SET_FIELD)) {
			table.set_properties().set_tfp_apply_actions().add_action(rofl::openflow::OFPAT_SET_FIELD);
		}

		table.set_properties().set_tfp_apply_actions_miss() = table.set_properties().get_tfp_apply_actions();


		// set supported write setfields
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PORT)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PORT);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PHY_PORT)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_METADATA)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_METADATA);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_DST)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_DST);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_SRC)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_SRC);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_TYPE)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_VID)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_VID);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_PCP)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_DSCP)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_DSCP);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_ECN)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_ECN);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_PROTO)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_PROTO);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_SRC)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_DST)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV4_DST);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_SRC)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_TCP_SRC);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_DST)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_TCP_DST);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_SRC)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_UDP_SRC);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_DST)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_UDP_DST);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_SRC)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_DST)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_SCTP_DST);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_TYPE)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_CODE)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_OP)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_OP);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SPA)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SPA);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_TPA)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_TPA);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SHA)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SHA);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_THA)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_THA);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_SRC)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_DST)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_DST);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_FLABEL)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_TYPE)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_CODE)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TARGET)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_SLL)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TLL)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_LABEL)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL);
		}
		if (table_stats.get_write_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_TC)) {
			table.set_properties().set_tfp_write_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_MPLS_TC);
		}

		table.set_properties().set_tfp_write_setfield_miss() = table.set_properties().get_tfp_write_setfield();


		// set supported apply setfields
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PORT)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PORT);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IN_PHY_PORT)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_METADATA)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_METADATA);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_DST)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_DST);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_SRC)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_SRC);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ETH_TYPE)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ETH_TYPE);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_VID)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_VID);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_VLAN_PCP)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_VLAN_PCP);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_DSCP)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_DSCP);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_ECN)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_ECN);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IP_PROTO)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IP_PROTO);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_SRC)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV4_SRC);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV4_DST)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV4_DST);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_SRC)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_TCP_SRC);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_TCP_DST)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_TCP_DST);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_SRC)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_UDP_SRC);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_UDP_DST)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_UDP_DST);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_SRC)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_SCTP_SRC);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_SCTP_DST)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_SCTP_DST);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_TYPE)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV4_CODE)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_OP)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_OP);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SPA)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SPA);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_TPA)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_TPA);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_SHA)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_SHA);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ARP_THA)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ARP_THA);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_SRC)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_SRC);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_DST)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_DST);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_FLABEL)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_TYPE)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_ICMPV6_CODE)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TARGET)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_SLL)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_IPV6_ND_TLL)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_LABEL)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL);
		}
		if (table_stats.get_apply_setfields() & (((uint64_t)1) << rofl::openflow12::OFPXMT_OFB_MPLS_TC)) {
			table.set_properties().set_tfp_apply_setfield().add_oxm(rofl::openflow::OXM_TLV_BASIC_MPLS_TC);
		}

		table.set_properties().set_tfp_apply_setfield_miss() = table.set_properties().get_tfp_apply_setfield();



		// set instructions
		if (table_stats.get_instructions() & (((uint64_t)1) << rofl::openflow12::OFPIT_GOTO_TABLE)) {
			table.set_properties().set_tfp_instructions().add_instruction(rofl::openflow::OFPIT_GOTO_TABLE);
		}
		if (table_stats.get_instructions() & (((uint64_t)1) << rofl::openflow12::OFPIT_WRITE_METADATA)) {
			table.set_properties().set_tfp_instructions().add_instruction(rofl::openflow::OFPIT_WRITE_METADATA);
		}
		if (table_stats.get_instructions() & (((uint64_t)1) << rofl::openflow12::OFPIT_WRITE_ACTIONS)) {
			table.set_properties().set_tfp_instructions().add_instruction(rofl::openflow::OFPIT_WRITE_ACTIONS);
		}
		if (table_stats.get_instructions() & (((uint64_t)1) << rofl::openflow12::OFPIT_APPLY_ACTIONS)) {
			table.set_properties().set_tfp_instructions().add_instruction(rofl::openflow::OFPIT_APPLY_ACTIONS);
		}
		if (table_stats.get_instructions() & (((uint64_t)1) << rofl::openflow12::OFPIT_CLEAR_ACTIONS)) {
			table.set_properties().set_tfp_instructions().add_instruction(rofl::openflow::OFPIT_CLEAR_ACTIONS);
		}

		table.set_properties().set_tfp_instructions_miss() = table.set_properties().get_tfp_instructions();
	}
}



/*static*/void
coftables::map_tables_to_tablestatsarray(
		rofl::openflow::coftables const& tables, rofl::openflow::coftablestatsarray& tablestatsarray)
{

}

