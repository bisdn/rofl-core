#include "rofl/common/openflow/cofflowstats.h"

using namespace rofl;

cofflow_stats_request::cofflow_stats_request(
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



cofflow_stats_request::cofflow_stats_request(
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



cofflow_stats_request::cofflow_stats_request(
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



cofflow_stats_request::~cofflow_stats_request()
{}



cofflow_stats_request::cofflow_stats_request(
		cofflow_stats_request const& request)
{
	*this = request;
}



cofflow_stats_request&
cofflow_stats_request::operator= (
		cofflow_stats_request const& request)
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
cofflow_stats_request::set_version(uint8_t of_version)
{
	this->of_version = of_version;
}



uint8_t
cofflow_stats_request::get_version() const
{
	return of_version;
}



void
cofflow_stats_request::set_match(cofmatch const& match)
{
	this->match = match;
}



cofmatch&
cofflow_stats_request::get_match()
{
	return match;
}



void
cofflow_stats_request::set_table_id(uint8_t table_id)
{
	this->table_id = table_id;
}



uint8_t
cofflow_stats_request::get_table_id() const
{
	return table_id;
}



void
cofflow_stats_request::set_out_port(uint32_t out_port)
{
	this->out_port = out_port;
}



uint32_t
cofflow_stats_request::get_out_port() const
{
	return out_port;
}




void
cofflow_stats_request::set_out_group(uint32_t out_group)
{
	switch (of_version) {
	case OFP12_VERSION: {
	} break;
	default:
		throw eBadVersion();
	}
	this->out_group = out_group;
}



uint32_t
cofflow_stats_request::get_out_group() const
{
	switch (of_version) {
	case OFP12_VERSION:
		break;
	default:
		throw eBadVersion();
	}
	return out_group;
}



void
cofflow_stats_request::set_cookie(uint64_t cookie)
{
	this->cookie = cookie;
}



uint64_t
cofflow_stats_request::get_cookie() const
{
	switch (of_version) {
	case OFP12_VERSION:
		break;
	default:
		throw eBadVersion();
	}
	return cookie;
}



void
cofflow_stats_request::set_cookie_mask(uint64_t cookie_mask)
{
	this->cookie_mask = cookie_mask;
}



uint64_t
cofflow_stats_request::get_cookie_mask() const
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
cofflow_stats_request::pack(uint8_t *buf, size_t buflen) const
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
		if (buflen < (sizeof(struct ofp12_flow_stats_request) - sizeof(struct ofp12_match) + match.length()))
			throw eInval();

		struct ofp12_flow_stats_request *req = (struct ofp12_flow_stats_request*)buf;
		req->table_id 		= table_id;
		req->out_port 		= htobe32(out_port);
		req->out_group		= htobe32(out_group);
		req->cookie			= htobe64(cookie);
		req->cookie_mask 	= htobe64(cookie_mask);
		match.pack(&(req->match), buflen - sizeof(struct ofp12_flow_stats_request) + sizeof(struct ofp12_match));
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofflow_stats_request::unpack(uint8_t *buf, size_t buflen)
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

		match.unpack(&(req->match), buflen - sizeof(struct ofp12_flow_stats_request) + sizeof(struct ofp12_match));
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
cofflow_stats_request::length() const
{
	switch (of_version) {
	case OFP10_VERSION: {
		return sizeof(struct ofp10_flow_stats_request);
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_flow_stats_request) - sizeof(struct ofp12_match) + match.length());
	} break;
	default:
		throw eBadVersion();
	}
}



cofflow_stats_reply::cofflow_stats_reply(
		uint8_t of_version,
		uint8_t *buf,
		size_t buflen) :
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
{
	if ((buflen > 0) && (0 != buf)) {
		unpack(buf, buflen);
	}
}



cofflow_stats_reply::cofflow_stats_reply(
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



cofflow_stats_reply::cofflow_stats_reply(
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



cofflow_stats_reply::~cofflow_stats_reply()
{}



cofflow_stats_reply::cofflow_stats_reply(
		cofflow_stats_reply const& flowstats)
{
	*this = flowstats;
}



cofflow_stats_reply&
cofflow_stats_reply::operator= (
		cofflow_stats_reply const& fs)
{
	if (this == &fs)
		return *this;

	of_version 		= fs.of_version;
	table_id 		= fs.table_id;
	duration_sec 	= fs.duration_sec;
	duration_nsec 	= fs.duration_nsec;
	priority 		= fs.priority;
	idle_timeout 	= fs.idle_timeout;
	hard_timeout 	= fs.hard_timeout;
	cookie 			= fs.cookie;
	packet_count 	= fs.packet_count;
	byte_count 		= fs.byte_count;
	match 			= fs.match;
	actions 		= fs.actions;
	instructions 	= fs.instructions;

	return *this;
}



void
cofflow_stats_reply::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case OFP10_VERSION: {
		if (buflen < length())
			throw eInval();

		struct ofp10_flow_stats *fs = (struct ofp10_flow_stats*)buf;

		fs->length 			= htobe16(length());
		fs->table_id 		= table_id;
		match.pack(&(fs->match), sizeof(struct ofp10_match));
		fs->duration_sec 	= htobe32(duration_sec);
		fs->duration_nsec 	= htobe32(duration_nsec);
		fs->priority		= htobe16(priority);
		fs->idle_timeout	= htobe16(idle_timeout);
		fs->hard_timeout	= htobe16(hard_timeout);
		fs->cookie			= htobe64(cookie);
		fs->packet_count	= htobe64(packet_count);
		fs->byte_count		= htobe64(byte_count);
		actions.pack(fs->actions, buflen - sizeof(struct ofp10_flow_stats));

	} break;
	case OFP12_VERSION: {
		if (buflen < length())
			throw eInval();

		struct ofp12_flow_stats *fs = (struct ofp12_flow_stats*)buf;

		fs->length 			= htobe16(length());
		fs->table_id 		= table_id;
		fs->duration_sec 	= htobe32(duration_sec);
		fs->duration_nsec 	= htobe32(duration_nsec);
		fs->priority		= htobe16(priority);
		fs->idle_timeout	= htobe16(idle_timeout);
		fs->hard_timeout	= htobe16(hard_timeout);
		fs->cookie			= htobe64(cookie);
		fs->packet_count	= htobe64(packet_count);
		fs->byte_count		= htobe64(byte_count);
		match.pack(&(fs->match), match.length());
		instructions.pack((struct ofp_instruction*)((uint8_t*)&(fs->match)) + match.length(), instructions.length());

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofflow_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_flow_stats))
			throw eInval();

		struct ofp10_flow_stats* fs = (struct ofp10_flow_stats*)buf;

		table_id		= fs->table_id;
		duration_sec	= be32toh(fs->duration_sec);
		duration_nsec	= be32toh(fs->duration_nsec);
		priority		= be16toh(fs->priority);
		idle_timeout	= be16toh(fs->idle_timeout);
		hard_timeout	= be16toh(fs->hard_timeout);
		cookie			= be64toh(fs->cookie);
		packet_count	= be64toh(fs->packet_count);
		byte_count		= be64toh(fs->byte_count);

		match.unpack(&(fs->match), sizeof(struct ofp10_match));
		actions.unpack(fs->actions, buflen - sizeof(struct ofp10_flow_stats));

	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_flow_stats))
			throw eInval();

		struct ofp12_flow_stats* fs = (struct ofp12_flow_stats*)buf;

		table_id		= fs->table_id;
		duration_sec	= be32toh(fs->duration_sec);
		duration_nsec	= be32toh(fs->duration_nsec);
		priority		= be16toh(fs->priority);
		idle_timeout	= be16toh(fs->idle_timeout);
		hard_timeout	= be16toh(fs->hard_timeout);
		cookie			= be64toh(fs->cookie);
		packet_count	= be64toh(fs->packet_count);
		byte_count		= be64toh(fs->byte_count);

		// derive length for match
		uint16_t matchlen = be16toh(fs->match.length);

		size_t pad = (0x7 & matchlen);
		/* append padding if not a multiple of 8 */
		if (pad) {
			matchlen += 8 - pad;
		}

		if (buflen < (sizeof(struct ofp12_flow_stats) - 4 + matchlen))
			throw eInval();

		match.unpack(&(fs->match), matchlen);
		instructions.unpack((struct ofp_instruction*)(buf + sizeof(struct ofp12_flow_stats) - 4 + matchlen),
									buflen - sizeof(struct ofp12_flow_stats) + 4 + matchlen);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofflow_stats_reply::length() const
{
	switch (of_version) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_flow_stats) + actions.length());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_flow_stats) + match.length() + instructions.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofflow_stats_reply::set_version(uint8_t of_version)
{
	this->of_version = of_version;
}



uint8_t
cofflow_stats_reply::get_version() const
{
	return of_version;
}



uint8_t
cofflow_stats_reply::get_table_id() const
{
	return table_id;
}



uint32_t
cofflow_stats_reply::get_duration_sec() const
{
	return duration_sec;
}



uint32_t
cofflow_stats_reply::get_duration_nsec() const
{
	return duration_nsec;
}



uint16_t
cofflow_stats_reply::get_priority() const
{
	return priority;
}



uint16_t
cofflow_stats_reply::get_idle_timeout() const
{
	return idle_timeout;
}



uint16_t
cofflow_stats_reply::get_hard_timeout() const
{
	return hard_timeout;
}



uint64_t
cofflow_stats_reply::get_cookie() const
{
	return cookie;
}



uint64_t
cofflow_stats_reply::get_packet_count() const
{
	return packet_count;
}



uint64_t
cofflow_stats_reply::get_byte_count() const
{
	return byte_count;
}



cofmatch&
cofflow_stats_reply::get_match()
{
	return match;
}



cofaclist&
cofflow_stats_reply::get_actions()
{
	return actions;
}



cofinlist&
cofflow_stats_reply::get_instructions()
{
	return instructions;
}


