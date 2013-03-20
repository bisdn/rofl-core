#include "cofmsg_stats.h"

using namespace rofl;



cofmsg_stats::cofmsg_stats(
		uint8_t of_version,
		uint8_t  type,
		uint32_t xid,
		uint32_t stats_type,
		uint32_t stats_flags) :
	cofmsg(sizeof(struct ofp_header))
{
	ofh_stats_request = soframe();

	set_version(of_version);
	set_type(type);
	set_xid(xid);

	switch (of_version) {
	case OFP10_VERSION: {
		resize(sizeof(struct ofp10_stats_request));
		ofh12_stats_request->type			= htobe16(stats_type);
		ofh12_stats_request->flags			= htobe16(stats_flags);
	} break;
	case OFP12_VERSION: {
		resize(sizeof(struct ofp12_stats_request));
		ofh12_stats_request->type			= htobe16(stats_type);
		ofh12_stats_request->flags			= htobe16(stats_flags);
	} break;
	case OFP13_VERSION: {
		resize(sizeof(struct ofp13_multipart_request));
		ofh13_multipart_request->type		= htobe16(stats_type);
		ofh13_multipart_request->flags		= htobe16(stats_flags);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_stats::cofmsg_stats(
		cmemory *memarea) :
	cofmsg(memarea)
{
	validate();
}



cofmsg_stats::cofmsg_stats(
		cofmsg_stats const& stats)
{
	*this = stats;
}



cofmsg_stats&
cofmsg_stats::operator= (
		cofmsg_stats const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg::operator =(stats);

	ofh_stats_request = soframe();

	return *this;
}



cofmsg_stats::~cofmsg_stats()
{

}



void
cofmsg_stats::reset()
{
	cofmsg::reset();
}



void
cofmsg_stats::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_stats_request = soframe();
}



size_t
cofmsg_stats::length() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return sizeof(struct ofp10_stats_request);
	} break;
	case OFP12_VERSION: {
		return sizeof(struct ofp12_stats_request);
	} break;
	case OFP13_VERSION: {
		return sizeof(struct ofp13_multipart_request);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_stats::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_stats_request))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp10_stats_request));
	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_stats_request))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp12_stats_request));
	} break;
	case OFP13_VERSION: {
		if (buflen < sizeof(struct ofp13_multipart_request))
			throw eInval();
		memcpy(buf, soframe(), sizeof(struct ofp13_multipart_request));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_stats::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_stats::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_stats_request = soframe();

	switch (get_version()) {
	case OFP10_VERSION: {
		if (get_length() < sizeof(struct ofp10_stats_request))
			throw eBadSyntaxTooShort();
	} break;
	case OFP12_VERSION: {
		if (get_length() < sizeof(struct ofp12_stats_request))
			throw eBadSyntaxTooShort();
	} break;
	case OFP13_VERSION: {
		if (get_length() < sizeof(struct ofp13_multipart_request))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint16_t
cofmsg_stats::get_stats_type() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_stats_request->type);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_stats_request->type);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_multipart_request->type);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_stats::set_stats_type(uint16_t stats_type)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_stats_request->type = htobe16(stats_type);
	} break;
	case OFP12_VERSION: {
		ofh12_stats_request->type = htobe16(stats_type);
	} break;
	case OFP13_VERSION: {
		ofh13_multipart_request->type = htobe16(stats_type);
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofmsg_stats::get_stats_flags() const
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(ofh10_stats_request->flags);
	} break;
	case OFP12_VERSION: {
		return be16toh(ofh12_stats_request->flags);
	} break;
	case OFP13_VERSION: {
		return be16toh(ofh13_multipart_request->flags);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_stats::set_stats_flags(uint16_t stats_flags)
{
	switch (get_version()) {
	case OFP10_VERSION: {
		ofh10_stats_request->flags = htobe16(stats_flags);
	} break;
	case OFP12_VERSION: {
		ofh12_stats_request->flags = htobe16(stats_flags);
	} break;
	case OFP13_VERSION: {
		ofh13_multipart_request->flags = htobe16(stats_flags);
	} break;
	default:
		throw eBadVersion();
	}
}

