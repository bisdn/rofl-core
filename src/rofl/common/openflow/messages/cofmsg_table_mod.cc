#include "rofl/common/openflow/messages/cofmsg_table_mod.h"

using namespace rofl::openflow;

cofmsg_table_mod::cofmsg_table_mod(
		uint8_t of_version,
		uint32_t xid,
		uint8_t  table_id,
		uint32_t config) :
	cofmsg(sizeof(struct openflow::ofp_header))
{
	ofh_table_mod = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_TABLE_MOD);
		resize(sizeof(struct openflow12::ofp_table_mod));

		ofh12_table_mod->table_id		= table_id;
		ofh12_table_mod->config			= htobe32(config);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_TABLE_MOD);
		resize(sizeof(struct openflow13::ofp_table_mod));

		ofh13_table_mod->table_id		= table_id;
		ofh13_table_mod->config			= htobe32(config);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_table_mod::cofmsg_table_mod(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_table_mod = soframe();
}



cofmsg_table_mod::cofmsg_table_mod(
		cofmsg_table_mod const& table_mod)
{
	*this = table_mod;
}



cofmsg_table_mod&
cofmsg_table_mod::operator= (
		cofmsg_table_mod const& table_mod)
{
	if (this == &table_mod)
		return *this;

	cofmsg::operator =(table_mod);

	ofh_table_mod = soframe();

	return *this;
}



cofmsg_table_mod::~cofmsg_table_mod()
{

}



void
cofmsg_table_mod::reset()
{
	cofmsg::reset();
}



uint8_t*
cofmsg_table_mod::resize(size_t len)
{
	return (ofh_table_mod = cofmsg::resize(len));
}



size_t
cofmsg_table_mod::length() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_table_mod));
	} break;
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_table_mod));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_table_mod::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct openflow12::ofp_table_mod))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow12::ofp_table_mod));
	} break;
	case openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct openflow13::ofp_table_mod))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct openflow13::ofp_table_mod));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_table_mod::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_table_mod::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_table_mod = soframe();

	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow12::ofp_table_mod))
			throw eBadSyntaxTooShort();
	} break;
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_table_mod))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}




uint8_t
cofmsg_table_mod::get_table_id() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return (ofh12_table_mod->table_id);
	} break;
	case openflow13::OFP_VERSION: {
		return (ofh13_table_mod->table_id);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_table_mod::set_table_id(uint8_t table_id)
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		ofh12_table_mod->table_id = (table_id);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_table_mod->table_id = (table_id);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_table_mod::get_config() const
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_table_mod->config);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_table_mod->config);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_table_mod::set_config(uint32_t config)
{
	switch (get_version()) {
	case openflow12::OFP_VERSION: {
		ofh12_table_mod->config = htobe32(config);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_table_mod->config = htobe32(config);
	} break;
	default:
		throw eBadVersion();
	}
}





