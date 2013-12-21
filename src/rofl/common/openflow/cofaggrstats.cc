#include "rofl/common/openflow/cofaggrstats.h"

using namespace rofl;

cofaggr_stats_request::cofaggr_stats_request(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
				of_version(of_version),
				table_id(0),
				out_port(0),
				out_group(0),
				cookie(0),
				cookie_mask(0)
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofaggr_stats_request::cofaggr_stats_request(
		uint8_t of_version,
		cofmatch const& match,
		uint8_t table_id,
		uint16_t out_port) :
				of_version(of_version),
				match(match),
				table_id(table_id),
				out_port(out_port),
				out_group(0),
				cookie(0),
				cookie_mask(0)
{}



cofaggr_stats_request::cofaggr_stats_request(
		uint8_t of_version,
		cofmatch const& match,
		uint8_t table_id,
		uint32_t out_port,
		uint32_t out_group,
		uint64_t cookie,
		uint64_t cookie_mask) :
			of_version(of_version),
			match(match),
			table_id(table_id),
			out_port(out_port),
			out_group(out_group),
			cookie(cookie),
			cookie_mask(cookie_mask)
{}



cofaggr_stats_request::~cofaggr_stats_request()
{}



const char*
cofaggr_stats_request::c_str()
{
	// TODO: fill in std::string info
	return info.c_str();
}



cofaggr_stats_request::cofaggr_stats_request(
		cofaggr_stats_request const& request)
{
	*this = request;
}



cofaggr_stats_request&
cofaggr_stats_request::operator= (
		cofaggr_stats_request const& request)
{
	if (this == &request)
		return *this;

	of_version 	= request.of_version;
	match		= request.match;
	table_id	= request.table_id;
	out_port	= request.out_port;
	out_group	= request.out_group;
	cookie		= request.cookie;
	cookie_mask	= request.cookie_mask;

	return *this;
}



void
cofaggr_stats_request::set_version(uint8_t of_version)
{
	this->of_version = of_version;
}



uint8_t
cofaggr_stats_request::get_version() const
{
	return of_version;
}



cofmatch&
cofaggr_stats_request::get_match()
{
	return match;
}



void
cofaggr_stats_request::set_table_id(uint8_t table_id)
{
	this->table_id = table_id;
}



uint8_t
cofaggr_stats_request::get_table_id() const
{
	return table_id;
}



void
cofaggr_stats_request::set_out_port(uint32_t out_port)
{
	this->out_port = out_port;
}



uint32_t
cofaggr_stats_request::get_out_port() const
{
	return out_port;
}



void
cofaggr_stats_request::set_out_group(uint32_t out_group)
{
	this->out_group = out_group;
}



uint32_t
cofaggr_stats_request::get_out_group() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
		break;
	default:
		throw eBadVersion();
	}
	return out_group;
}



void
cofaggr_stats_request::set_cookie(uint64_t cookie)
{
	this->cookie = cookie;
}



uint64_t
cofaggr_stats_request::get_cookie() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
		break;
	default:
		throw eBadVersion();
	}
	return cookie;
}



void
cofaggr_stats_request::set_cookie_mask(uint64_t cookie_mask)
{
	this->cookie_mask = cookie_mask;
}



uint64_t
cofaggr_stats_request::get_cookie_mask() const
{
	switch (of_version) {
	case openflow12::OFP_VERSION:
		break;
	default:
		throw eBadVersion();
	}
	return cookie_mask;
}



void
cofaggr_stats_request::pack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow10::ofp_flow_stats_request) - 4 + match.length()))
			throw eInval();

		struct openflow10::ofp_flow_stats_request *req = (struct openflow10::ofp_flow_stats_request*)buf;
		req->table_id 	= table_id;
		req->out_port 	= htobe16((uint16_t)(out_port & 0x0000ffff));
		match.pack((uint8_t*)&(req->match), sizeof(struct openflow10::ofp_match));
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < (sizeof(struct openflow12::ofp_flow_stats_request) - sizeof(struct openflow12::ofp_match) + match.length()))
			throw eInval();

		struct openflow12::ofp_flow_stats_request *req = (struct openflow12::ofp_flow_stats_request*)buf;
		req->table_id 		= table_id;
		req->out_port 		= htobe32(out_port);
		req->out_group		= htobe32(out_group);
		req->cookie			= htobe64(cookie);
		req->cookie_mask 	= htobe64(cookie_mask);
		match.pack((uint8_t*)&(req->match), buflen - sizeof(struct openflow12::ofp_flow_stats_request) + sizeof(struct openflow12::ofp_match));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofaggr_stats_request::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct openflow10::ofp_flow_stats_request))
			throw eInval();

		struct openflow10::ofp_flow_stats_request *req = (struct openflow10::ofp_flow_stats_request*)buf;

		match.unpack((uint8_t*)&(req->match), sizeof(struct openflow10::ofp_match));
		table_id 		= req->table_id;
		out_port 		= (uint32_t)(be16toh(req->out_port));
	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct openflow12::ofp_flow_stats_request))
			throw eInval();

		struct openflow12::ofp_flow_stats_request *req = (struct openflow12::ofp_flow_stats_request*)buf;

		match.unpack((uint8_t*)&(req->match), buflen - sizeof(struct openflow12::ofp_flow_stats_request) + sizeof(struct openflow12::ofp_match));
		table_id 		= req->table_id;
		out_port 		= be32toh(req->out_port);
		out_group 		= be32toh(req->out_group);
		cookie 			= be64toh(req->cookie);
		cookie_mask 	= be64toh(req->cookie_mask);
	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofaggr_stats_request::length() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return sizeof(struct openflow10::ofp_flow_stats_request);
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_flow_stats_request) - sizeof(struct openflow12::ofp_match) + match.length());
	} break;
	default:
		throw eBadVersion();
	}
}



cofaggr_stats_reply::cofaggr_stats_reply(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
				of_version(of_version),
				packet_count(0),
				byte_count(0),
				flow_count(0)
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofaggr_stats_reply::cofaggr_stats_reply(
		uint8_t of_version,
		uint64_t packet_count,
		uint64_t byte_count,
		uint32_t flow_count) :
				of_version(of_version),
				packet_count(packet_count),
				byte_count(byte_count),
				flow_count(flow_count)
{}



cofaggr_stats_reply::~cofaggr_stats_reply()
{}



cofaggr_stats_reply::cofaggr_stats_reply(
		cofaggr_stats_reply const& flowstats)
{
	*this = flowstats;
}



cofaggr_stats_reply&
cofaggr_stats_reply::operator= (
		cofaggr_stats_reply const& as)
{
	if (this == &as)
		return *this;

	of_version 		= as.of_version;
	packet_count	= as.packet_count;
	byte_count		= as.byte_count;
	flow_count		= as.flow_count;

	return *this;
}



void
cofaggr_stats_reply::pack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct openflow10::ofp_aggregate_stats_reply *as = (struct openflow10::ofp_aggregate_stats_reply*)buf;
		as->packet_count	= htobe64(packet_count);
		as->byte_count		= htobe64(byte_count);
		as->flow_count		= htobe32(flow_count);

	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < length())
			throw eInval();

		struct openflow12::ofp_aggregate_stats_reply *as = (struct openflow12::ofp_aggregate_stats_reply*)buf;
		as->packet_count	= htobe64(packet_count);
		as->byte_count		= htobe64(byte_count);
		as->flow_count		= htobe32(flow_count);

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofaggr_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct openflow10::ofp_aggregate_stats_reply))
			throw eInval();

		struct openflow10::ofp_aggregate_stats_reply* as = (struct openflow10::ofp_aggregate_stats_reply*)buf;
		packet_count	= be64toh(as->packet_count);
		byte_count		= be64toh(as->byte_count);
		flow_count		= be32toh(as->flow_count);

	} break;
	case openflow12::OFP_VERSION: {
		if (buflen < sizeof(struct openflow12::ofp_aggregate_stats_reply))
			throw eInval();

		struct openflow12::ofp_aggregate_stats_reply* as = (struct openflow12::ofp_aggregate_stats_reply*)buf;
		packet_count	= be64toh(as->packet_count);
		byte_count		= be64toh(as->byte_count);
		flow_count		= be32toh(as->flow_count);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofaggr_stats_reply::length() const
{
	switch (of_version) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_aggregate_stats_reply));
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_aggregate_stats_reply));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofaggr_stats_reply::set_version(uint8_t of_version)
{
	this->of_version = of_version;
}



uint8_t
cofaggr_stats_reply::get_version() const
{
	return of_version;
}



uint64_t
cofaggr_stats_reply::get_packet_count() const
{
	return packet_count;
}



uint64_t
cofaggr_stats_reply::get_byte_count() const
{
	return byte_count;
}



uint32_t
cofaggr_stats_reply::get_flow_count() const
{
	return flow_count;
}



void
cofaggr_stats_reply::set_packet_count(uint64_t packet_count)
{
	this->packet_count = packet_count;
}



void
cofaggr_stats_reply::set_byte_count(uint64_t byte_count)
{
	this->byte_count = byte_count;
}



void
cofaggr_stats_reply::set_flow_count(uint32_t flow_count)
{
	this->flow_count = flow_count;
}


