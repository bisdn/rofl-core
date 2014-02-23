#include "cofmsg_queue_stats.h"

using namespace rofl;




cofmsg_queue_stats_request::cofmsg_queue_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		cofqueue_stats_request const& queue_stats) :
	cofmsg_stats_request(of_version, xid, 0, flags),
	queue_stats(queue_stats)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_stats_type(openflow10::OFPST_QUEUE);
		set_type(openflow10::OFPT_STATS_REQUEST);
		resize(length());
	} break;
	case openflow12::OFP_VERSION: {
		set_stats_type(openflow12::OFPST_QUEUE);
		set_type(openflow12::OFPT_STATS_REQUEST);
		resize(length());
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
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
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_stats_request) + queue_stats.length());
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_stats_request) + queue_stats.length());
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
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
	case openflow10::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		queue_stats.pack(buf + sizeof(struct openflow10::ofp_stats_request), sizeof(struct openflow10::ofp_queue_stats_request));
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		queue_stats.pack(buf + sizeof(struct openflow12::ofp_stats_request), sizeof(struct openflow12::ofp_queue_stats_request));
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
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
	case openflow10::OFP_VERSION: {
		queue_stats.set_version(openflow10::OFP_VERSION);
		if (get_length() < length())
			throw eBadSyntaxTooShort();
		queue_stats.unpack(soframe() + sizeof(struct openflow10::ofp_stats_request), sizeof(struct openflow10::ofp_queue_stats_request));
	} break;
	case openflow12::OFP_VERSION: {
		queue_stats.set_version(openflow12::OFP_VERSION);
		if (get_length() < length())
			throw eBadSyntaxTooShort();
		queue_stats.unpack(soframe() + sizeof(struct openflow12::ofp_stats_request), sizeof(struct openflow12::ofp_queue_stats_request));
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



cofqueue_stats_request&
cofmsg_queue_stats_request::get_queue_stats()
{
	return queue_stats;
}





cofmsg_queue_stats_reply::cofmsg_queue_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t flags,
		std::vector<cofqueue_stats_reply> const& queue_stats) :
	cofmsg_stats_reply(of_version, xid, 0, flags),
	queue_stats(queue_stats)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_stats_type(openflow10::OFPST_QUEUE);
		set_type(openflow10::OFPT_STATS_REPLY);
		resize(length());
	} break;
	case openflow12::OFP_VERSION: {
		set_stats_type(openflow12::OFPST_QUEUE);
		set_type(openflow12::OFPT_STATS_REPLY);
		resize(length());
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
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
	case openflow10::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct openflow10::ofp_stats_reply);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct openflow12::ofp_stats_reply);
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented();
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
	case openflow10::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct openflow10::ofp_stats_reply);
	} break;
	case openflow12::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct openflow12::ofp_stats_reply);
	} break;
	case openflow13::OFP_VERSION: {
		ofh_queue_stats = soframe() + sizeof(struct openflow13::ofp_multipart_reply);
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
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_stats_reply) + queue_stats.size() * sizeof(struct openflow10::ofp_queue_stats));
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_stats_reply) + queue_stats.size() * sizeof(struct openflow12::ofp_queue_stats));
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
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
	case openflow10::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		size_t offset = 0;
		for (unsigned int i = 0; i < queue_stats.size(); i++) {
			queue_stats[i].pack(buf + sizeof(struct openflow10::ofp_stats_reply) + offset, queue_stats[i].length());
			offset += queue_stats[i].length();
		}
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();
		size_t offset = 0;
		for (unsigned int i = 0; i < queue_stats.size(); i++) {
			queue_stats[i].pack(buf + sizeof(struct openflow12::ofp_stats_reply) + offset, queue_stats[i].length());
			offset += queue_stats[i].length();
		}
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
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

	queue_stats.clear();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_stats_reply))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct openflow10::ofp_stats_reply)) / sizeof(struct openflow10::ofp_queue_stats)); i++) {
			cofqueue_stats_reply queue_stats_reply(openflow10::OFP_VERSION);
			queue_stats_reply.unpack(soframe() + sizeof(struct openflow10::ofp_stats_reply) + i * sizeof(struct openflow10::ofp_queue_stats), sizeof(struct openflow10::ofp_queue_stats));
			queue_stats.push_back(queue_stats_reply);
		}
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < (sizeof(struct openflow12::ofp_stats_reply) + sizeof(struct openflow12::ofp_queue_stats)))
			throw eBadSyntaxTooShort();
		for (unsigned int i = 0; i < ((get_length() - sizeof(struct openflow12::ofp_stats_reply)) / sizeof(struct openflow12::ofp_queue_stats)); i++) {
			cofqueue_stats_reply queue_stats_reply(openflow12::OFP_VERSION);
			queue_stats_reply.unpack(soframe() + sizeof(struct openflow12::ofp_stats_reply) + i * sizeof(struct openflow12::ofp_queue_stats), sizeof(struct openflow12::ofp_queue_stats));
			queue_stats.push_back(queue_stats_reply);
		}
	} break;
	case openflow13::OFP_VERSION: {
		// TODO
		throw eNotImplemented();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



std::vector<cofqueue_stats_reply>&
cofmsg_queue_stats_reply::get_queue_stats()
{
	return queue_stats;
}


