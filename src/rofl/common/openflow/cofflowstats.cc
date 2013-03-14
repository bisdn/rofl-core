#include "rofl/common/openflow/cofflowstats.h"

using namespace rofl;

cofflowstatsrequest::cofflowstatsrequest(
		uint8_t of_version) :
				of_version(of_version),
				table_id(0),
				out_port(0),
				out_group(0),
				cookie(0),
				cookie_mask(0)
{}



cofflowstatsrequest::cofflowstatsrequest(
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



cofflowstatsrequest::cofflowstatsrequest(
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



cofflowstatsrequest::~cofflowstatsrequest()
{}



cofflowstatsrequest::cofflowstatsrequest(
		cofflowstatsrequest const& request)
{
	*this = request;
}



cofflowstatsrequest&
cofflowstatsrequest::operator= (
		cofflowstatsrequest const& request)
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



uint8_t
cofflowstatsrequest::get_version() const
{
	return of_version;
}



cofmatch&
cofflowstatsrequest::get_match()
{
	return match;
}



uint8_t
cofflowstatsrequest::get_table_id() const
{
	return table_id;
}



uint32_t
cofflowstatsrequest::get_out_port() const
{
	return out_port;
}



uint32_t
cofflowstatsrequest::get_out_group() const
{
	switch (of_version) {
	case OFP12_VERSION:
		break;
	default:
		throw eBadVersion();
	}
	return out_group;
}



uint64_t
cofflowstatsrequest::get_cookie() const
{
	switch (of_version) {
	case OFP12_VERSION:
		break;
	default:
		throw eBadVersion();
	}
	return cookie;
}



uint64_t
cofflowstatsrequest::get_cookie_mask() const
{
	switch (of_version) {
	case OFP12_VERSION:
		break;
	default:
		throw eBadVersion();
	}
	return cookie_mask;
}



void
cofflowstatsrequest::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case OFP10_VERSION: {
		if (buflen < (sizeof(struct ofp10_flow_stats_request) - 4 + match.length()))
			throw eInval();

		struct ofp10_flow_stats_request *req = (struct ofp10_flow_stats_request*)buf;
		req->table_id 	= table_id;
		req->out_port 	= htobe16((uint16_t)(out_port & 0x0000ffff));
		match.pack(&(req->match), sizeof(struct ofp10_match));
	} break;
	case OFP12_VERSION: {
		if (buflen < (sizeof(struct ofp12_flow_stats_request) - 4 + match.length()))
			throw eInval();

		struct ofp12_flow_stats_request *req = (struct ofp12_flow_stats_request*)buf;
		req->table_id 		= table_id;
		req->out_port 		= htobe32(out_port);
		req->out_group		= htobe32(out_group);
		req->cookie			= htobe64(cookie);
		req->cookie_mask 	= htobe64(cookie_mask);
		match.pack(&(req->match), sizeof(struct ofp12_flow_stats_request) - 4);
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofflowstatsrequest::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_flow_stats_request))
			throw eInval();

		struct ofp10_flow_stats_request *req = (struct ofp10_flow_stats_request*)buf;

		match.unpack(&(req->match), sizeof(struct ofp10_match));
		table_id 		= req->table_id;
		out_port 		= (uint32_t)(be16toh(req->out_port));
	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_flow_stats_request))
			throw eInval();

		struct ofp12_flow_stats_request *req = (struct ofp12_flow_stats_request*)buf;

		match.unpack(&(req->match), buflen - (sizeof(struct ofp12_flow_stats_request) - 4));
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



cofflowstats::cofflowstats(
		uint8_t of_version) :
				of_version(of_version),
				table_id(0),
				duration_sec(0),
				duration_nsec(0),
				priority(0),
				idle_timeout(0),
				hard_timeout(0),
				cookie(0),
				packet_count(0),
				byte_count(0)
{}



cofflowstats::cofflowstats(
		uint8_t of_version,
		uint8_t table_id,
		uint32_t duration_sec,
		uint32_t duration_nsec,
		uint16_t priority,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint64_t cookie,
		uint64_t packet_count,
		uint64_t byte_count,
		cofmatch const& match,
		cofaclist const& actions) :
				of_version(of_version),
				table_id(table_id),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec),
				priority(priority),
				idle_timeout(idle_timeout),
				hard_timeout(hard_timeout),
				cookie(cookie),
				packet_count(packet_count),
				byte_count(byte_count),
				match(match),
				actions(actions)
{}



cofflowstats::cofflowstats(
		uint8_t of_version,
		uint8_t table_id,
		uint32_t duration_sec,
		uint32_t duration_nsec,
		uint16_t priority,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint64_t cookie,
		uint64_t packet_count,
		uint64_t byte_count,
		cofmatch const& match,
		cofinlist const& instructions) :
				of_version(of_version),
				table_id(table_id),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec),
				priority(priority),
				idle_timeout(idle_timeout),
				hard_timeout(hard_timeout),
				cookie(cookie),
				packet_count(packet_count),
				byte_count(byte_count),
				match(match),
				instructions(instructions)
{}



cofflowstats::~cofflowstats()
{}



cofflowstats::cofflowstats(
		cofflowstats const& flowstats)
{
	*this = flowstats;
}



cofflowstats&
cofflowstats::operator= (
		cofflowstats const& flowstats);


/**
 *
 */
void
cofflowstats::pack(uint8_t *buf, size_t buflen) const;


/**
 *
 */
void
cofflowstats::unpack(uint8_t *buf, size_t buflen);


/**
 *
 */
uint8_t
cofflowstats::get_version() const;


/**
 *
 */
uint8_t
cofflowstats::get_table_id() const;

/**
 *
 */
uint32_t
cofflowstats::get_duration_sec() const;

/**
 *
 */
uint32_t
cofflowstats::get_duration_nsec() const;

/**
 *
 */
uint16_t
cofflowstats::get_priority() const;

/**
 *
 */
uint16_t
cofflowstats::get_idle_timeout() const;

/**
 *
 */
uint16_t
cofflowstats::get_hard_timeout() const;

/**
 *
 */
uint64_t
cofflowstats::get_cookie() const;

/**
 *
 */
uint64_t
cofflowstats::get_packet_count() const;

/**
 *
 */
uint64_t
cofflowstats::get_byte_count() const;

/**
 *
 */
cofmatch&
cofflowstats::get_match();

/**
 *
 */
cofaclist&
cofflowstats::get_actions();

/**
 *
 */
cofinlist&
cofflowstats::get_instructions();

