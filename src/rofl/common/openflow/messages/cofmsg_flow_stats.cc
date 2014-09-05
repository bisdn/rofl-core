#include "cofmsg_flow_stats.h"

using namespace rofl::openflow;

cofmsg_flow_stats_request::cofmsg_flow_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofflow_stats_request const& flow_stats) :
	cofmsg_stats_request(of_version, xid, 0, flags),
	flow_stats(flow_stats)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow10::OFP_VERSION: {
		set_type(rofl::openflow10::OFPT_STATS_REQUEST);
		set_stats_type(rofl::openflow10::OFPST_FLOW);
		resize(length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_STATS_REQUEST);
		set_stats_type(rofl::openflow12::OFPST_FLOW);
		resize(length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
		set_stats_type(rofl::openflow13::OFPMP_FLOW);
		resize(length());
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_flow_stats_request::cofmsg_flow_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea)
{
	flow_stats.set_version(get_version());
}



cofmsg_flow_stats_request::cofmsg_flow_stats_request(
		cofmsg_flow_stats_request const& stats)
{
	*this = stats;
}



cofmsg_flow_stats_request&
cofmsg_flow_stats_request::operator= (
		cofmsg_flow_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_flow_stats = soframe();

	return *this;
}



cofmsg_flow_stats_request::~cofmsg_flow_stats_request()
{

}



void
cofmsg_flow_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_flow_stats_request::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct rofl::openflow10::ofp_stats_request);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_request);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_request);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_flow_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_request) + flow_stats.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_request) + flow_stats.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request) + flow_stats.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_stats_request::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		flow_stats.pack(buf + sizeof(struct rofl::openflow10::ofp_stats_request), flow_stats.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		flow_stats.pack(buf + sizeof(struct rofl::openflow12::ofp_stats_request), flow_stats.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		flow_stats.pack(buf + sizeof(struct rofl::openflow13::ofp_multipart_request), flow_stats.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_flow_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_flow_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	ofh_flow_stats = soframe();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow10::ofp_stats_request))
			throw eBadSyntaxTooShort();
		flow_stats.unpack(soframe() + sizeof(struct rofl::openflow10::ofp_stats_request), get_length() - sizeof(struct rofl::openflow10::ofp_stats_request));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow12::ofp_stats_request) + sizeof(struct rofl::openflow12::ofp_flow_stats_request)))
			throw eBadSyntaxTooShort();
		flow_stats.unpack(soframe() + sizeof(struct rofl::openflow12::ofp_stats_request), get_length() - sizeof(struct rofl::openflow12::ofp_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < (sizeof(struct rofl::openflow13::ofp_multipart_request) + sizeof(struct rofl::openflow13::ofp_flow_stats_request)))
			throw eBadSyntaxTooShort();
		flow_stats.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_multipart_request), get_length() - sizeof(struct rofl::openflow13::ofp_multipart_request));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofflow_stats_request&
cofmsg_flow_stats_request::set_flow_stats()
{
	return flow_stats;
}



cofflow_stats_request const&
cofmsg_flow_stats_request::get_flow_stats() const
{
	return flow_stats;
}







cofmsg_flow_stats_reply::cofmsg_flow_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		rofl::openflow::cofflowstatsarray const& flowstatsarray) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	flowstatsarray(flowstatsarray)
{
	this->flowstatsarray.set_version(of_version);

	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow10::OFP_VERSION: {
		set_type(rofl::openflow10::OFPT_STATS_REPLY);
		set_stats_type(rofl::openflow10::OFPST_FLOW);
		resize(length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_type(rofl::openflow12::OFPT_STATS_REPLY);
		set_stats_type(rofl::openflow12::OFPST_FLOW);
		resize(length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_MULTIPART_REPLY);
		set_stats_type(rofl::openflow13::OFPMP_FLOW);
		resize(length());
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_flow_stats_reply::cofmsg_flow_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea),
	flowstatsarray(get_version())
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct rofl::openflow10::ofp_stats_reply);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_flow_stats_reply::cofmsg_flow_stats_reply(
		cofmsg_flow_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_flow_stats_reply&
cofmsg_flow_stats_reply::operator= (
		cofmsg_flow_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_flow_stats = soframe();

	flowstatsarray = stats.flowstatsarray;

	return *this;
}



cofmsg_flow_stats_reply::~cofmsg_flow_stats_reply()
{

}



void
cofmsg_flow_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_flow_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct rofl::openflow10::ofp_stats_reply);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_flow_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_flow_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_reply) + flowstatsarray.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_reply) + flowstatsarray.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + flowstatsarray.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_flow_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		flowstatsarray.pack(buf + sizeof(struct rofl::openflow10::ofp_stats_reply), buflen - sizeof(struct rofl::openflow10::ofp_stats_reply));

	} break;
	case rofl::openflow12::OFP_VERSION: {

		flowstatsarray.pack(buf + sizeof(struct rofl::openflow12::ofp_stats_reply), buflen - sizeof(struct rofl::openflow12::ofp_stats_reply));

	} break;
	case rofl::openflow13::OFP_VERSION: {

		flowstatsarray.pack(buf + sizeof(struct rofl::openflow13::ofp_multipart_reply), buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_flow_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_flow_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	flowstatsarray.clear();
	flowstatsarray.set_version(get_version());

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow10::ofp_stats_reply))
			throw eBadSyntaxTooShort();

		flowstatsarray.unpack(soframe() + sizeof(struct rofl::openflow10::ofp_stats_reply), framelen() - sizeof(struct rofl::openflow10::ofp_stats_reply));

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow12::ofp_stats_reply))
			throw eBadSyntaxTooShort();

		flowstatsarray.unpack(soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply), framelen() - sizeof(struct rofl::openflow12::ofp_stats_reply));

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow13::ofp_multipart_reply))
			throw eBadSyntaxTooShort();

		flowstatsarray.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply), framelen() - sizeof(struct rofl::openflow13::ofp_multipart_reply));

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}





