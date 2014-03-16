#include "cofmsg_queue_stats.h"

using namespace rofl;




cofmsg_queue_stats_request::cofmsg_queue_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		rofl::openflow::cofqueue_stats_request const& queue_stats) :
	cofmsg_stats_request(of_version, xid, 0, flags),
	queue_stats(queue_stats)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow10::OFP_VERSION: {
		set_stats_type(rofl::openflow10::OFPST_QUEUE);
		set_type(rofl::openflow10::OFPT_STATS_REQUEST);
		resize(length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_stats_type(rofl::openflow12::OFPST_QUEUE);
		set_type(rofl::openflow12::OFPT_STATS_REQUEST);
		resize(length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_stats_type(rofl::openflow13::OFPMP_QUEUE);
		set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
		resize(length());
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_queue_stats_request::cofmsg_queue_stats_request(
		cmemory *memarea) :
	cofmsg_stats_request(memarea)
{

}



cofmsg_queue_stats_request::cofmsg_queue_stats_request(
		cofmsg_queue_stats_request const& stats)
{
	*this = stats;
}



cofmsg_queue_stats_request&
cofmsg_queue_stats_request::operator= (
		cofmsg_queue_stats_request const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);
	queue_stats = stats.queue_stats;

	return *this;
}



cofmsg_queue_stats_request::~cofmsg_queue_stats_request()
{

}



void
cofmsg_queue_stats_request::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_queue_stats_request::resize(size_t len)
{
	return cofmsg_stats::resize(len);
}



size_t
cofmsg_queue_stats_request::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_request) + queue_stats.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_request) + queue_stats.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_request) + queue_stats.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_queue_stats_request::pack(uint8_t *buf, size_t buflen)
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
		queue_stats.pack(buf + sizeof(struct rofl::openflow10::ofp_stats_request), sizeof(struct rofl::openflow10::ofp_queue_stats_request));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		queue_stats.pack(buf + sizeof(struct rofl::openflow12::ofp_stats_request), sizeof(struct rofl::openflow12::ofp_queue_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		queue_stats.pack(buf + sizeof(struct rofl::openflow13::ofp_multipart_request), sizeof(struct rofl::openflow13::ofp_queue_stats_request));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_queue_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_queue_stats_request::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		queue_stats.set_version(rofl::openflow10::OFP_VERSION);
		if (get_length() < length())
			throw eBadSyntaxTooShort();
		queue_stats.unpack(soframe() + sizeof(struct rofl::openflow10::ofp_stats_request), sizeof(struct rofl::openflow10::ofp_queue_stats_request));
	} break;
	case rofl::openflow12::OFP_VERSION: {
		queue_stats.set_version(rofl::openflow12::OFP_VERSION);
		if (get_length() < length())
			throw eBadSyntaxTooShort();
		queue_stats.unpack(soframe() + sizeof(struct rofl::openflow12::ofp_stats_request), sizeof(struct rofl::openflow12::ofp_queue_stats_request));
	} break;
	case rofl::openflow13::OFP_VERSION: {
		queue_stats.set_version(rofl::openflow13::OFP_VERSION);
		if (get_length() < length())
			throw eBadSyntaxTooShort();
		queue_stats.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_multipart_request), sizeof(struct rofl::openflow13::ofp_queue_stats_request));
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



rofl::openflow::cofqueue_stats_request&
cofmsg_queue_stats_request::set_queue_stats()
{
	return queue_stats;
}



rofl::openflow::cofqueue_stats_request const&
cofmsg_queue_stats_request::get_queue_stats() const
{
	return queue_stats;
}





cofmsg_queue_stats_reply::cofmsg_queue_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		rofl::openflow::cofqueuestatsarray const& queuestatsarray) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	queuestatsarray(queuestatsarray)
{
	switch (of_version) {
	case rofl::openflow::OFP_VERSION_UNKNOWN: {

	} break;
	case rofl::openflow10::OFP_VERSION: {
		set_stats_type(rofl::openflow10::OFPST_QUEUE);
		set_type(rofl::openflow10::OFPT_STATS_REPLY);
		resize(length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		set_stats_type(rofl::openflow12::OFPST_QUEUE);
		set_type(rofl::openflow12::OFPT_STATS_REPLY);
		resize(length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		set_stats_type(rofl::openflow13::OFPMP_QUEUE);
		set_type(rofl::openflow13::OFPT_MULTIPART_REPLY);
		resize(length());
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_queue_stats_reply::cofmsg_queue_stats_reply(
		cmemory *memarea) :
	cofmsg_stats_reply(memarea)
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct rofl::openflow10::ofp_stats_reply);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_queue_stats_reply::cofmsg_queue_stats_reply(
		cofmsg_queue_stats_reply const& stats)
{
	*this = stats;
}



cofmsg_queue_stats_reply&
cofmsg_queue_stats_reply::operator= (
		cofmsg_queue_stats_reply const& stats)
{
	if (this == &stats)
		return *this;

	cofmsg_stats::operator =(stats);

	ofh_queue_stats = soframe();

	return *this;
}



cofmsg_queue_stats_reply::~cofmsg_queue_stats_reply()
{

}



void
cofmsg_queue_stats_reply::reset()
{
	cofmsg_stats::reset();
}



uint8_t*
cofmsg_queue_stats_reply::resize(size_t len)
{
	cofmsg_stats::resize(len);
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct rofl::openflow10::ofp_stats_reply);
	} break;
	case rofl::openflow12::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply);
	} break;
	case rofl::openflow13::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply);
	} break;
	default:
		throw eBadVersion();
	}
	return soframe();
}



size_t
cofmsg_queue_stats_reply::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_stats_reply) + queuestatsarray.length());
	} break;
	case rofl::openflow12::OFP_VERSION: {
		return (sizeof(struct rofl::openflow12::ofp_stats_reply) + queuestatsarray.length());
	} break;
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_multipart_reply) + queuestatsarray.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_queue_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::pack(buf, buflen); // copies common statistics header

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		queuestatsarray.pack(buf + sizeof(struct rofl::openflow10::ofp_stats_reply), buflen - sizeof(struct rofl::openflow10::ofp_stats_reply));

	} break;
	case rofl::openflow12::OFP_VERSION: {

		queuestatsarray.pack(buf + sizeof(struct rofl::openflow12::ofp_stats_reply), buflen - sizeof(struct rofl::openflow12::ofp_stats_reply));

	} break;
	case rofl::openflow13::OFP_VERSION: {

		queuestatsarray.pack(buf + sizeof(struct rofl::openflow13::ofp_multipart_reply), buflen - sizeof(struct rofl::openflow13::ofp_multipart_reply));

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_queue_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg_stats::unpack(buf, buflen);

	validate();
}



void
cofmsg_queue_stats_reply::validate()
{
	cofmsg_stats::validate(); // check generic statistics header

	queuestatsarray.clear();

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow10::ofp_stats_reply))
			throw eBadSyntaxTooShort();

		queuestatsarray.unpack(soframe() + sizeof(struct rofl::openflow10::ofp_stats_reply), framelen() - sizeof(struct rofl::openflow10::ofp_stats_reply));

	} break;
	case rofl::openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow12::ofp_stats_reply))
			throw eBadSyntaxTooShort();

		queuestatsarray.unpack(soframe() + sizeof(struct rofl::openflow12::ofp_stats_reply), framelen() - sizeof(struct rofl::openflow12::ofp_stats_reply));

	} break;
	case rofl::openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct rofl::openflow13::ofp_multipart_reply))
			throw eBadSyntaxTooShort();

		queuestatsarray.unpack(soframe() + sizeof(struct rofl::openflow13::ofp_multipart_reply), framelen() - sizeof(struct rofl::openflow13::ofp_multipart_reply));

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



