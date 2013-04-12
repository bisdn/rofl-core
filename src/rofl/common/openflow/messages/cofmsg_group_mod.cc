#include "cofmsg_group_mod.h"

using namespace rofl;



cofmsg_group_mod::cofmsg_group_mod(
		uint8_t of_version,
		uint32_t xid,
		uint16_t command,
		uint8_t  group_type,
		uint32_t group_id,
		cofbclist const& buckets) :
	cofmsg(sizeof(struct ofp_header))
{
	ofh_group_mod = soframe();

	set_version(of_version);
	set_type(OFPT_GROUP_MOD);
	set_xid(xid);

	switch (of_version) {
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_group_mod));

		ofh12_group_mod->command		= htobe16(command);
		ofh12_group_mod->type			= group_type;
		ofh12_group_mod->group_id		= htobe32(group_id);
	} break;
	case OFP13_VERSION: {
		resize(sizeof(struct ofp13_group_mod));

		ofh13_group_mod->command		= htobe16(command);
		ofh13_group_mod->type			= group_type;
		ofh13_group_mod->group_id		= htobe32(group_id);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_group_mod::cofmsg_group_mod(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_group_mod = soframe();
}



cofmsg_group_mod::cofmsg_group_mod(
		cofmsg_group_mod const& group_mod)
{
	*this = group_mod;
}



cofmsg_group_mod&
cofmsg_group_mod::operator= (
		cofmsg_group_mod const& group_mod)
{
	if (this == &group_mod)
		return *this;

	cofmsg::operator =(group_mod);

	ofh_group_mod = soframe();

	return *this;
}



cofmsg_group_mod::~cofmsg_group_mod()
{

}



void
cofmsg_group_mod::reset()
{
	cofmsg::reset();
}



void
cofmsg_group_mod::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_group_mod = soframe();
}



size_t
cofmsg_group_mod::length() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_group_mod) + buckets.length());
	} break;
	case OFP13_VERSION: {
		return (sizeof(struct ofp13_group_mod) + buckets.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_mod::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP12_VERSION: {
		if (buflen < (sizeof(struct ofp12_group_mod) + buckets.length()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp12_group_mod));
		buckets.pack((struct ofp12_bucket*)(buf + sizeof(struct ofp12_group_mod)), buckets.length());
	} break;
	case OFP13_VERSION: {
		if (buflen < (sizeof(struct ofp13_group_mod) + buckets.length()))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp13_group_mod));
		buckets.pack((struct ofp13_bucket*)(buf + sizeof(struct ofp13_group_mod)), buckets.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_group_mod::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_group_mod::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_group_mod = soframe();

	switch (get_version()) {
	case OFP12_VERSION: {
		if (get_length() < sizeof(struct ofp12_group_mod))
			throw eBadSyntaxTooShort();
		buckets.unpack(ofh12_group_mod->buckets, get_length() - sizeof(struct ofp12_group_mod));
	} break;
	case OFP13_VERSION: {
		if (get_length() < sizeof(struct ofp13_group_mod))
			throw eBadSyntaxTooShort();
		buckets.unpack(ofh13_group_mod->buckets, get_length() - sizeof(struct ofp13_group_mod));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint16_t
cofmsg_group_mod::get_command() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return be16toh(ofh12_group_mod->command);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_group_mod->command);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_mod::set_command(uint16_t command)
{
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh12_group_mod->command = htobe16(command);
	} break;
	case OFP13_VERSION: {
		ofh13_group_mod->command = htobe16(command);
	} break;
	default:
		throw eBadVersion();
	}
}





uint8_t
cofmsg_group_mod::get_group_type() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return (ofh12_group_mod->type);
	} break;
	case OFP13_VERSION: {
		return (ofh13_group_mod->type);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_mod::set_group_type(uint8_t group_type)
{
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh12_group_mod->type = group_type;
	} break;
	case OFP13_VERSION: {
		ofh13_group_mod->type = group_type;
	} break;
	default:
		throw eBadVersion();
	}
}




uint32_t
cofmsg_group_mod::get_group_id() const
{
	switch (get_version()) {
	case OFP12_VERSION: {
		return be32toh(ofh12_group_mod->group_id);
	} break;
	case OFP13_VERSION: {
		return be32toh(ofh13_group_mod->group_id);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_group_mod::set_group_id(uint32_t group_id)
{
	switch (get_version()) {
	case OFP12_VERSION: {
		ofh12_group_mod->group_id = htobe32(group_id);
	} break;
	case OFP13_VERSION: {
		ofh13_group_mod->group_id = htobe32(group_id);
	} break;
	default:
		throw eBadVersion();
	}
}



cofbclist&
cofmsg_group_mod::get_buckets()
{
	return buckets;
}



