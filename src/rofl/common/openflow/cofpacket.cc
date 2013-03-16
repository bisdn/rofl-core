/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofpacket.h"

using namespace rofl;

/*static*/std::set<cofpacket*> cofpacket::cofpacket_list;
/*static*/std::string cofpacket::pinfo;

/*static*/const char*
cofpacket::packet_info()
{
	cvastring vas;
	pinfo.assign(vas("cofpackets allocated: %d\n", cofpacket::cofpacket_list.size()));

	std::map<uint8_t, unsigned int> counter;
	for (std::set<cofpacket*>::iterator
			it = cofpacket::cofpacket_list.begin();
				it != cofpacket::cofpacket_list.end(); ++it)
	{
		cofpacket *pack = (*it);
		if (counter.find(pack->ofh_header->type) == counter.end())
		{
			counter[pack->ofh_header->type] = 1;
		}
		else
		{
			counter[pack->ofh_header->type] += 1;
		}
	}

	for (std::map<uint8_t, unsigned int>::iterator
			it = counter.begin(); it != counter.end(); ++it)
	{
		pinfo.append(vas("  %s => %lu",
						cofpacket::type2desc((enum ofp_type)it->first),
						it->second));
	}

	return pinfo.c_str();
}



cofpacket::cofpacket(size_t size, size_t used) :
	stored(used),
	memarea(size),
	entity(0),
	body(0),
	packet((size_t)0)
{
	ofh_header = (struct ofp_header*)soframe();
	ofh_header->length = stored;
	//cofpacket::cofpacket_list.insert(this);
}



cofpacket::~cofpacket()
{
	//cofpacket::cofpacket_list.erase(this);
}



cofpacket::cofpacket(cofpacket const& p) :
		body(0),
		packet((size_t)0)
{
	*this = p;
	//cofpacket::cofpacket_list.insert(this);
}



cofpacket&
cofpacket::operator=(const cofpacket &p)
{
	if (this == &p)
	{
		return *this;
	}

	memarea 			= p.memarea;
	match				= p.match;
	actions				= p.actions;
	buckets 			= p.buckets;
	instructions 		= p.instructions;
	body				= p.body;
	packet				= p.packet;
	ports				= p.ports;
	port				= p.port;
	desc_stats_reply	= p.desc_stats_reply;
	flow_stats_request	= p.flow_stats_request;
	flow_stats_reply	= p.flow_stats_reply;
	aggr_stats_request	= p.aggr_stats_request;
	aggr_stats_reply	= p.aggr_stats_reply;
	table_stats_reply	= p.table_stats_reply;

	ofh_header = (struct ofp_header*)soframe();

	return *this;
}


uint32_t
cofpacket::get_xid()
{
	return be32toh(ofh_header->xid);
}


void
cofpacket::set_xid(uint32_t xid)
{
	ofh_header->xid = htobe32(xid);
}


void
cofpacket::reset()
{
	memarea.clear();
	match.clear();
	actions.clear();
	buckets.clear();
	instructions.clear();
	ofh_header = (struct ofp_header*)memarea.somem();
}


size_t
cofpacket::length()
{
	return (sizeof(struct ofp_header));
}


void
cofpacket::pack(uint8_t *buf, size_t buflen) throw (eOFpacketInval)
{
	ofh_header->length = htobe16(length());

	if (((uint8_t*)0 == buf) || (buflen < length()))
	{
		return;
	}

	memcpy(buf, memarea.somem(), memarea.memlen());
}


void
cofpacket::unpack(uint8_t *buf, size_t buflen)
{
	memarea.assign(buf, buflen);
}


bool
cofpacket::complete() throw (eOFpacketInval)
{
	if (stored < sizeof(struct ofp_header))
	{
		return false;
	}

	WRITELOG(COFPACKET, DBG, "cofpacket(%p)::complete() "
			"bytes read => stored:%d, indicated length in OFheader:%d",
			this, stored, be16toh(ofh_header->length));

	// critical error, we read behind the real packet, need split of packet, this should never happen, though
	if (stored > be16toh(ofh_header->length))
	{
		throw eOFpacketInval();
	}
	else if (stored == be16toh(ofh_header->length))
	{
		return true;
	}

	return false;
}


bool
cofpacket::is_too_large()
{
	if (stored > be16toh(ofh_header->length))
	{
		return true;
	}
	return false;
}


size_t
cofpacket::need_bytes()
{
	// if we haven't read at least one ofp_header, fill the header
	if (stored < sizeof(struct ofp_header))
	{
		WRITELOG(COFPACKET, DBG, "cofpacket(%p)::need_bytes() "
				"bytes => stored:%d is less than sizeof(struct ofp_header):%d, need at least %d more bytes",
				 this, stored, sizeof(struct ofp_header), sizeof(struct ofp_header) - stored);

		if (memarea.memlen() < sizeof(struct ofp_header))
		{
			resize(sizeof(struct ofp_header));
		}

		return(sizeof(struct ofp_header) - stored);
	}


	/*
	 *  get residual packet bytes
	 */

	ofh_header = (struct ofp_header*)(soframe());

	size_t packlen = be16toh(ofh_header->length);

	if (packlen > memarea.memlen())
	{
		resize(packlen);
	}

	WRITELOG(COFPACKET, DBG, "cofpacket(%p)::need_bytes() "
			"bytes => stored:%d OFlength:%d, need %d more bytes",
			 this, stored, packlen, packlen - stored);

	return (packlen - stored);
}


void
cofpacket::resize(size_t len)
{
	memarea.resize(len);
	ofh_header = (struct ofp_header*)soframe();
}


bool
cofpacket::has_data()
{
	return (0 == body.memlen()) ? false : true;
}


uint8_t*
cofpacket::get_data() throw (eOFpacketNoData)
{
	if (0 == body.memlen())
	{
		throw eOFpacketNoData();
	}
	return body.somem();
}


size_t
cofpacket::get_datalen() throw (eOFpacketNoData)
{
	if (0 == body.memlen())
	{
		throw eOFpacketNoData();
	}
	return body.memlen();
}


bool
cofpacket::is_valid()
{
	ofh_header = (struct ofp_header*)soframe();
	switch (ofh_header->type) {
	case OFPT_HELLO:
		return is_valid_hello_msg();
	case OFPT_ECHO_REQUEST:
		return is_valid_echo_request();
	case OFPT_ECHO_REPLY:
		return is_valid_echo_reply();
	case OFPT_FEATURES_REQUEST:
	case OFPT_GET_CONFIG_REQUEST:
	case OFPT_BARRIER_REQUEST:
	case OFPT_BARRIER_REPLY:
		return is_valid_hdr_only();
	case OFPT_ERROR:
		return is_valid_error_msg();
	case OFPT_FEATURES_REPLY:
		return is_valid_switch_features();
	case OFPT_GET_CONFIG_REPLY:
	case OFPT_SET_CONFIG:
		return is_valid_switch_config();
	case OFPT_PACKET_IN:
		return is_valid_packet_in();
	case OFPT_PACKET_OUT:
		return is_valid_packet_out();
	case OFPT_FLOW_REMOVED:
		return is_valid_flow_removed();
	case OFPT_PORT_STATUS:
		return is_valid_port_status();
	case OFPT_FLOW_MOD:
		return is_valid_flow_mod();
	case OFPT_PORT_MOD:
		return is_valid_port_mod();
	case OFPT_STATS_REQUEST:
	//case OFPT_MULTIPART_REQUEST: // OFPT_STATS_REQUEST equals OFPT_MULTIPART_REQUEST
		return is_valid_stats_request();
	case OFPT_STATS_REPLY:
	//case OFPT_MULTIPART_REPLY: // OFPT_STATS_REPLY equals OFPT_MULTIPART_REPLY
		return is_valid_stats_reply();
	case OFPT_QUEUE_GET_CONFIG_REQUEST:
		return is_valid_queue_get_config_request();
	case OFPT_QUEUE_GET_CONFIG_REPLY:
		return is_valid_queue_get_config_reply();
	case OFPT_EXPERIMENTER:
		return is_valid_experimenter_message();
	case OFPT_GROUP_MOD:
		return is_valid_group_mod();
	case OFPT_TABLE_MOD:
		return is_valid_table_mod();
	case OFPT_METER_MOD:
		return is_valid_meter_mod();
	default:
		return false;
	}
	//WRITELOG(CPACKET, DBG, "cOFpacket is valid");
	return true;
}


bool
cofpacket::is_valid_hello_msg()
{
	switch (ofh_header->version) {
	case OFP12_VERSION: {
		ofh_header = (struct ofp_header*)soframe();
		if (stored < sizeof(struct ofp_header))
			return false;
		if (stored < be16toh(ofh_header->length))
			return false;
		if (stored > sizeof(struct ofp_header))
		{
			if (0 == body.memlen())
			{
				body.assign(soframe() + sizeof(struct ofp_header),
						stored - sizeof(struct ofp_header));
			}
		}
	} break;
	case OFP13_VERSION: {
		ofh_header = (struct ofp_header*)soframe();
		if (stored < sizeof(struct ofp_header))
			return false;
		if (stored < be16toh(ofh_header->length))
			return false;
		// TODO: create hello elements list
	} break;
	default: {
		throw eBadRequestBadVersion();
	} break;
	}

	return true;
}


bool
cofpacket::is_valid_echo_request()
{
	ofh_header = (struct ofp_header*)soframe();
	if (stored < sizeof(struct ofp_header))
		return false;
	if (stored < be16toh(ofh_header->length))
		return false;
	if (stored > sizeof(struct ofp_header))
	{
		if (0 == body.memlen())
		{
			body.assign(soframe() + sizeof(struct ofp_header),
					stored - sizeof(struct ofp_header));
		}
	}
	return true;
}


bool
cofpacket::is_valid_echo_reply()
{
	ofh_header = (struct ofp_header*)soframe();
	if (stored < sizeof(struct ofp_header))
		return false;
	if (stored < be16toh(ofh_header->length))
		return false;
	if (stored > sizeof(struct ofp_header))
	{
		if (0 == body.memlen())
		{
			body.assign(soframe() + sizeof(struct ofp_header),
					stored - sizeof(struct ofp_header));
		}
	}
	return true;
}


bool
cofpacket::is_valid_hdr_only()
{
	if (stored < sizeof(struct ofp_header))
		return false;
	if (stored < be16toh(ofh_header->length))
		return false;
	return true;
}


bool
cofpacket::is_valid_experimenter_message()
{
	ofh_experimenter = (struct ofp_experimenter_header*)soframe();
	if (stored < sizeof(struct ofp_header))
		return false;
	if (stored < be16toh(ofh_experimenter->header.length))
		return false;
	body.assign(soframe() + sizeof(struct ofp_experimenter_header),
			stored - sizeof(struct ofp_experimenter_header));
	return true;
}


bool
cofpacket::is_valid_error_msg()
{
	ofh_error_msg = (struct ofp_error_msg*)soframe();
	if (stored < sizeof(struct ofp_error_msg))
	{
		WRITELOG(COFPACKET, DBG, "cofpacket(%p)::is_valid_error_msg() "
				"frame is too short: %d", this, framelen());
		return false;
	}

	switch (be16toh(ofh_error_msg->type)) {
	case OFPET_HELLO_FAILED:
		// access via emhdr->data
		break;
	case OFPET_BAD_REQUEST:
		// access via emhdr->data
		break;
	case OFPET_BAD_ACTION:
		// access via emhdr->data
		break;
	case OFPET_FLOW_MOD_FAILED:
		// access via emhdr->data
		break;
	case OFPET_PORT_MOD_FAILED:
		// access via emhdr->data
		break;
	case OFPET_QUEUE_OP_FAILED:
		// access via emhdr->data
		break;
	default:
		WRITELOG(COFPACKET, DBG, "cofpacket(%p)::is_valid_error_msg() "
				"unknown error msg type: %d", this, be16toh(ofh_error_msg->type));
		break;
	}
	body.assign(ofh_error_msg->data,
			stored - sizeof(struct ofp_error_msg));
	return true;
}

bool
cofpacket::is_valid_switch_features()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		of10h_switch_features = (struct ofp10_switch_features*)soframe();
		if (stored < sizeof(struct ofp10_switch_features))
			return false;

		size_t ports_len = stored - sizeof(struct ofp10_switch_features);

		if (ports_len > 0) {
			ports.unpack(of10h_switch_features->ports, ports_len);
		} else {
			ports.clear();
		}

	} break;
	case OFP12_VERSION: {
		of12h_switch_features = (struct ofp12_switch_features*)soframe();
		if (stored < sizeof(struct ofp12_switch_features))
			return false;

		size_t ports_len = stored - sizeof(struct ofp12_switch_features);

		if (ports_len > 0) {
			ports.unpack(of12h_switch_features->ports, ports_len);
		} else {
			ports.clear();
		}

	} break;
	case OFP13_VERSION: {
		of13h_switch_features = (struct ofp13_switch_features*)soframe();
		if (stored < sizeof(struct ofp13_switch_features))
			return false;

		ports.clear();

	} break;
	default: {
		throw eBadRequestBadVersion();
	} break;
	}


	return true;
}

bool
cofpacket::is_valid_switch_config()
{
	switch (ofh_header->version) {
	case OFP10_VERSION:
	case OFP12_VERSION:
	case OFP13_VERSION: {
		of10h_switch_config = (struct ofp10_switch_config*)soframe();
		if (stored < sizeof(struct ofp10_switch_config))
			return false;
		if (stored < be16toh(ofh_header->length))
			return false;
	} break;
	default:
		throw eBadRequestBadVersion();
	}
	return true;
}

bool
cofpacket::is_valid_packet_in()
{

	switch (ofh_header->version) {
	case OFP10_VERSION: {
		/*
		 * static part of struct ofp_packet_in also contains static fields from struct ofp_match (i.e. type and length)
		 */
		if (stored < OFP10_PACKET_IN_STATIC_HDR_LEN) {
			return false;
		}

		/*
		 * set data and datalen variables
		 */
		uint16_t offset = OFP10_PACKET_IN_STATIC_HDR_LEN + 2;

		//body.assign((uint8_t*)(soframe() + offset), stored - (offset)); // +2: magic :)

		uint32_t in_port = be32toh(of10h_packet_in->in_port);

		packet.unpack(in_port, (uint8_t*)(soframe() + offset), stored - (offset)); // +2: magic :)

	} break;
	case OFP12_VERSION: {
		/*
		 * static part of struct ofp_packet_in also contains static fields from struct ofp_match (i.e. type and length)
		 */
		if (stored < OFP12_PACKET_IN_STATIC_HDR_LEN) {
			return false;
		}

		/*
		 * get variable length struct ofp_match
		 */
		if (be16toh(of12h_packet_in->match.type) != OFPMT_OXM) // must be extensible match
		{
			return false;
		}

		if (be16toh(of12h_packet_in->match.length) > (stored - 16 /* fixed part outside of struct ofp_match is 16bytes */))
		{
			return false;
		}

		match.unpack(&(of12h_packet_in->match), be16toh(of12h_packet_in->match.length));

		/*
		 * set data and datalen variables
		 */
		uint16_t offset = OFP12_PACKET_IN_STATIC_HDR_LEN + match.length() + 2;

		//body.assign((uint8_t*)(soframe() + offset), stored - (offset)); // +2: magic :)

		uint32_t in_port = 0;

		try {
			in_port = match.get_in_port();
		} catch (eOFmatchNotFound& e) {
			in_port = 0;
		}

		packet.unpack(in_port, (uint8_t*)(soframe() + offset), stored - (offset)); // +2: magic :)

	} break;
	case OFP13_VERSION: {
		/*
		 * static part of struct ofp_packet_in also contains static fields from struct ofp_match (i.e. type and length)
		 */
		if (stored < OFP13_PACKET_IN_STATIC_HDR_LEN) {
			return false;
		}

		/*
		 * get variable length struct ofp_match
		 */
		if (be16toh(of13h_packet_in->match.type) != OFPMT_OXM) // must be extensible match
		{
			return false;
		}

		if (be16toh(of13h_packet_in->match.length) > (stored - 16 /* fixed part outside of struct ofp_match is 16bytes */))
		{
			return false;
		}

		match.unpack(&(of13h_packet_in->match), be16toh(of12h_packet_in->match.length));

		/*
		 * set data and datalen variables
		 */
		uint16_t offset = OFP13_PACKET_IN_STATIC_HDR_LEN + match.length() + 2;

		//body.assign((uint8_t*)(soframe() + offset), stored - (offset)); // +2: magic :)

		uint32_t in_port = 0;

		try {
			in_port = match.get_in_port();
		} catch (eOFmatchNotFound& e) {
			in_port = 0;
		}

		packet.unpack(in_port, (uint8_t*)(soframe() + offset), stored - (offset)); // +2: magic :)

	} break;
	default: {
		throw eBadRequestBadVersion();
	}
	}
	return true;
}

bool
cofpacket::is_valid_packet_out()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		of10h_packet_out = (struct ofp10_packet_out*)soframe();
		if (stored < sizeof(struct ofp10_packet_out))
			return false;

		packet.unpack((uint32_t)be16toh(of10h_packet_out->in_port),
					((uint8_t*)of10h_packet_out) +
						sizeof(struct ofp10_packet_out) +
							be16toh(of10h_packet_out->actions_len),
							be16toh(of10h_packet_out->header.length) -
												sizeof(struct ofp10_packet_out) -
													be16toh(of10h_packet_out->actions_len));

		actions.unpack(of10h_packet_out->actions,
						be16toh(of10h_packet_out->actions_len));

	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		of12h_packet_out = (struct ofp12_packet_out*)soframe();
		if (stored < sizeof(struct ofp12_packet_out))
			return false;

		packet.unpack(be32toh(of12h_packet_out->in_port),
					((uint8_t*)of12h_packet_out) +
						sizeof(struct ofp12_packet_out) +
							be16toh(of12h_packet_out->actions_len),
							be16toh(of12h_packet_out->header.length) -
												sizeof(struct ofp12_packet_out) -
													be16toh(of12h_packet_out->actions_len));

		actions.unpack(of12h_packet_out->actions,
						be16toh(of12h_packet_out->actions_len));


	} break;
	default:
		throw eBadVersion();
	}
	return true;
}

bool
cofpacket::is_valid_flow_removed()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		of10h_flow_rmvd = (struct ofp10_flow_removed*)soframe();

		if (stored < sizeof(struct ofp10_flow_removed))
			return false;

		try {
			match.unpack(&(of10h_flow_rmvd->match), sizeof(struct ofp10_match));
		} catch (eOFmatchInval& e) {
			return false;
		}

	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		of12h_flow_rmvd = (struct ofp12_flow_removed*)soframe();

		size_t frgenlen = sizeof(struct ofp12_flow_removed) - sizeof(struct ofp12_match);

		if (stored < frgenlen)
			return false;

		size_t matchlen = be16toh(of12h_flow_rmvd->header.length);

		try  {
			match.unpack(&(of12h_flow_rmvd->match), matchlen);
		} catch (eOFmatchInval& e) {
			return false;
		}

	} break;
	default:
		throw eBadRequestBadVersion();
	}
	return true;	
}



bool
cofpacket::is_valid_port_status()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		if (stored < sizeof(struct ofp10_port_status))
			return false;

		port.unpack(&(of10h_port_status->desc), sizeof(struct ofp10_port));

	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		if (stored < sizeof(struct ofp12_port_status))
			return false;

		port.unpack(&(of12h_port_status->desc), sizeof(struct ofp12_port));

	} break;
	default:
		throw eBadVersion();
	}
	return true;
}



bool
cofpacket::is_valid_flow_mod()
{
	try {

		switch (ofh_header->version) {
		case OFP12_VERSION: {
			if (stored < sizeof(struct ofp12_flow_mod)) {// includes static part of struct ofp_match (i.e. type and length)
				return false;
			}

			/* OFP_FLOW_MOD_STATIC_HDR_LEN = length of generic flow-mod header
			 * according to OpenFlow-spec-1.2 is 48bytes
			 */

			if ((be16toh(of12h_flow_mod->match.length)) > (stored - OFP12_FLOW_MOD_STATIC_HDR_LEN)) {
							// stored - OFP_FLOW_MOD_STATIC_HDR_LEN is #bytes for struct ofp_match and array of struct ofp_instructions
				fprintf(stderr, "1.2 ");
				return false; // match too long
			}

			/*
			 * unpack ofp_match structure
			 */

			match.unpack(&(of12h_flow_mod->match), be16toh(of12h_flow_mod->match.length));

			WRITELOG(COFPACKET, DBG, "cofpacket(%p)::is_valid_flow_mod() "
					"match: %s", this, match.c_str());

			// check size of received flow-mod message
			/*
			 * match.length() returns length of struct ofp_match including padding
			 */
			if ((OFP12_FLOW_MOD_STATIC_HDR_LEN + match.length()) > stored) {
				return false;
			}

			/*
			 * unpack instructions list
			 */

			struct ofp_instruction *insts = (struct ofp_instruction*)(
									(uint8_t*)&(of12h_flow_mod->match) + match.length());

			size_t instslen = stored -
					(OFP12_FLOW_MOD_STATIC_HDR_LEN + match.length());

			instructions.unpack(insts, instslen);

		} break;
		case OFP13_VERSION: {
			if (stored < sizeof(struct ofp13_flow_mod)) {// includes static part of struct ofp_match (i.e. type and length)
				return false;
			}

			/* OFP_FLOW_MOD_STATIC_HDR_LEN = length of generic flow-mod header
			 * according to OpenFlow-spec-1.2 is 48bytes
			 */

			if ((be16toh(of13h_flow_mod->match.length)) > (stored - OFP13_FLOW_MOD_STATIC_HDR_LEN)) {
							// stored - OFP_FLOW_MOD_STATIC_HDR_LEN is #bytes for struct ofp_match and array of struct ofp_instructions
				fprintf(stderr, "1.2 ");
				return false; // match too long
			}

			/*
			 * unpack ofp_match structure
			 */

			match.unpack(&(of13h_flow_mod->match), be16toh(of13h_flow_mod->match.length));

			WRITELOG(COFPACKET, DBG, "cofpacket(%p)::is_valid_flow_mod() "
					"match: %s", this, match.c_str());

			// check size of received flow-mod message
			/*
			 * match.length() returns length of struct ofp_match including padding
			 */
			if ((OFP13_FLOW_MOD_STATIC_HDR_LEN + match.length()) > stored) {
				return false;
			}

			/*
			 * unpack instructions list
			 */

			struct ofp_instruction *insts = (struct ofp_instruction*)(
									(uint8_t*)&(of13h_flow_mod->match) + match.length());

			size_t instslen = stored -
					(OFP13_FLOW_MOD_STATIC_HDR_LEN + match.length());

			instructions.unpack(insts, instslen);

		} break;
		default: {
			throw eBadRequestBadVersion();
		}
		}

	} catch (eOFmatchInval& e) {
		fprintf(stderr, "1.4 ");
		return false;

	} catch (eInstructionBadLen& e) {
		fprintf(stderr, "1.5 ");
		return false;

	}

	return true;
}


bool
cofpacket::is_valid_group_mod()
{
	try {
		ofh_group_mod = (struct ofp_group_mod*)soframe();
		if (stored < sizeof(struct ofp_group_mod))
			return false;

		buckets.unpack(ofh_group_mod->buckets,
				be16toh(ofh_group_mod->header.length) -
				sizeof(struct ofp_group_mod));

		return true;

	} catch (eBadActionBadOutPort& e) {
		return true; // FIXME: dont ask
	}
	return false; // just to make eclipse happy ...
}


bool
cofpacket::is_valid_table_mod()
{
	ofh_table_mod = (struct ofp_table_mod*)soframe();
	if (stored < sizeof(struct ofp_table_mod))
		return false;

	return true;
}



bool
cofpacket::is_valid_port_mod()
{
	switch (get_version()) {
	case OFP10_VERSION: {
		of10h_port_mod = (struct ofp10_port_mod*)soframe();
		if (stored < sizeof(struct ofp10_port_mod))
			return false;

	} break;
	case OFP12_VERSION: {
		of12h_port_mod = (struct ofp12_port_mod*)soframe();
		if (stored < sizeof(struct ofp12_port_mod))
			return false;

	} break;
	default:
		throw eBadVersion();
	}
	return true;
}



bool
cofpacket::is_valid_stats_request()
{
	switch (get_version()) {
	case OFP10_VERSION: {
		of10h_stats_request = (struct ofp10_stats_request*)soframe();
		if (stored < sizeof(struct ofp10_stats_request))
			return false;

		size_t body_len = stored - sizeof(struct ofp10_stats_request);

		match.clear();

		switch (be16toh(of10h_stats_request->type)) {
		case OFPST_FLOW: {
			flow_stats_request.set_version(OFP10_VERSION);
			flow_stats_request.unpack(of10h_stats_request->body, body_len);
		} break;
		case OFPST_AGGREGATE: {
			aggr_stats_request.set_version(OFP10_VERSION);
			aggr_stats_request.unpack(of10h_stats_request->body, body_len);
		} break;
		case OFPST_PORT: {
			port_stats_request.set_version(OFP10_VERSION);
			port_stats_request.unpack(of10h_stats_request->body, body_len);
		} break;
		default:
			body.assign(of10h_stats_request->body, body_len);
		}

	} break;
	case OFP12_VERSION: {
		of12h_stats_request = (struct ofp12_stats_request*)soframe();
		if (stored < sizeof(struct ofp12_stats_request))
			return false;

		size_t body_len = stored - sizeof(struct ofp12_stats_request);

		match.clear();

		switch (be16toh(of12h_stats_request->type)) {
		case OFPST_FLOW: {
			flow_stats_request.set_version(OFP12_VERSION);
			flow_stats_request.unpack(of12h_stats_request->body, body_len);
		} break;
		case OFPST_AGGREGATE: {
			aggr_stats_request.set_version(OFP12_VERSION);
			aggr_stats_request.unpack(of12h_stats_request->body, body_len);
		} break;
		case OFPST_PORT: {
			port_stats_request.set_version(OFP12_VERSION);
			port_stats_request.unpack(of12h_stats_request->body, body_len);
		} break;
		default:
			body.assign(of12h_stats_request->body, body_len);
		}

	} break;
	default:
		throw eBadVersion();
	}

	// TODO: description fields
	return true;
}



bool
cofpacket::is_valid_stats_reply()
{
	switch (get_version()) {
	case OFP10_VERSION: {
		of10h_stats_reply = (struct ofp10_stats_reply*)soframe();
		if (stored < sizeof(struct ofp10_stats_reply))
			return false;

		size_t body_len = stored - sizeof(struct ofp10_stats_reply);

		match.clear();

		switch (be16toh(of10h_stats_reply->type)) {
		case OFPST_DESC: {
			desc_stats_reply.set_version(OFP10_VERSION);
			desc_stats_reply.unpack(of10h_stats_reply->body, body_len);
		} break;
		case OFPST_FLOW: {
			flow_stats_reply.set_version(OFP10_VERSION);
			flow_stats_reply.unpack(of10h_stats_reply->body, body_len);
		} break;
		case OFPST_AGGREGATE: {
			aggr_stats_reply.set_version(OFP10_VERSION);
			aggr_stats_reply.unpack(of10h_stats_reply->body, body_len);
		} break;
		case OFPST_TABLE: {
			table_stats_reply.set_version(OFP10_VERSION);
			table_stats_reply.unpack(of10h_stats_reply->body, body_len);
		} break;
		case OFPST_PORT: {
			port_stats_reply.set_version(OFP10_VERSION);
			port_stats_reply.unpack(of10h_stats_reply->body, body_len);
		} break;
		default:
			body.assign((uint8_t*)of10h_stats_reply->body, body_len);
		}

	} break;
	case OFP12_VERSION: {
		of12h_stats_reply = (struct ofp12_stats_reply*)soframe();
		if (stored < sizeof(struct ofp12_stats_reply))
			return false;

		size_t body_len = stored - sizeof(struct ofp12_stats_reply);

		match.clear();

		switch (be16toh(of12h_stats_reply->type)) {
		case OFPST_DESC: {
			desc_stats_reply.set_version(OFP12_VERSION);
			desc_stats_reply.unpack(of12h_stats_reply->body, body_len);
		} break;
		case OFPST_FLOW: {
			flow_stats_reply.set_version(OFP12_VERSION);
			flow_stats_reply.unpack(of12h_stats_reply->body, body_len);
		} break;
		case OFPST_AGGREGATE: {
			aggr_stats_reply.set_version(OFP12_VERSION);
			aggr_stats_reply.unpack(of12h_stats_reply->body, body_len);
		} break;
		case OFPST_TABLE: {
			table_stats_reply.set_version(OFP12_VERSION);
			table_stats_reply.unpack(of12h_stats_reply->body, body_len);
		} break;
		case OFPST_PORT: {
			port_stats_reply.set_version(OFP12_VERSION);
			port_stats_reply.unpack(of12h_stats_reply->body, body_len);
		} break;
		default:
			body.assign(of12h_stats_reply->body, body_len);
		}

	} break;
	default:
		throw eBadVersion();
	}

	// TODO: description fields
	return true;
}



bool
cofpacket::is_valid_queue_get_config_request()
{
	switch (get_version()) {
	case OFP10_VERSION: {
		of10h_queue_get_config_request = (struct ofp10_queue_get_config_request*)soframe();
		if (stored < sizeof(struct ofp10_queue_get_config_request))
			return false;

	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		of12h_queue_get_config_request = (struct ofp12_queue_get_config_request*)soframe();
		if (stored < sizeof(struct ofp12_queue_get_config_request))
			return false;

	} break;
	default:
		throw eBadVersion();
	}
	return true;
}



bool
cofpacket::is_valid_queue_get_config_reply()
{
	switch (get_version()) {
	case OFP10_VERSION: {
		of10h_queue_get_config_reply = (struct ofp10_queue_get_config_reply*)soframe();
		if (stored < sizeof(struct ofp10_queue_get_config_reply))
			return false;
		if (stored < be16toh(of10h_queue_get_config_reply->header.length))
			return false;
		body.assign((uint8_t*)of10h_queue_get_config_reply->queues,
				stored - sizeof(struct ofp10_queue_get_config_reply));

	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		of12h_queue_get_config_reply = (struct ofp12_queue_get_config_reply*)soframe();
		if (stored < sizeof(struct ofp12_queue_get_config_reply))
			return false;
		if (stored < be16toh(of12h_queue_get_config_reply->header.length))
			return false;
		body.assign((uint8_t*)of12h_queue_get_config_reply->queues,
				stored - sizeof(struct ofp12_queue_get_config_reply));

	} break;
	default:
		throw eBadVersion();
	}
	return true;
}



bool
cofpacket::is_valid_meter_mod()
{
	ofh13_meter_mod = (struct ofp13_meter_mod*)soframe();
	if (stored < sizeof(struct ofp13_meter_mod))
		return false;
	if (stored < be16toh(ofh13_meter_mod->header.length))
		return false;
	// TODO: create list of meter bands
	return true;
}



const char*
cofpacket::c_str()
{
	cvastring vas(4096);

	fframe frame(memarea.somem(), stored);

	info.assign(vas("cofpacket(%p): %s vers:%d len:%d xid:0x%x framelen:%u %s",
			this,
			type2desc((ofp_type)this->ofh_header->type),
			this->ofh_header->version,
			be16toh(this->ofh_header->length),
			be32toh(this->ofh_header->xid),
			stored,
			frame.c_str()));

	if (not instructions.empty())
	{
		info.append(vas("\ncofpacket(%p): instructions: %s", this, instructions.c_str()));
	}

	if (not actions.empty())
	{
		info.append(vas("\ncofpacket(%p): actions: %s", this, actions.c_str()));
	}

	if (true)
	{
		info.append(vas("\ncofpacket(%p): match: %s", this, match.c_str()));
	}

	if (0 != body.memlen())
	{
		info.append(vas("\ncofpacket(%p): body: %s", this, body.c_str()));
	}

	if (0 != packet.framelen())
	{
		info.append(vas("\ncofpacket(%p): packet: %s", this, packet.c_str()));
	}


#if 0
	switch (ofh_header->type) {
	case OFPT_HELLO:
		break;
	case OFPT_ERROR:
		break;
	case OFPT_ECHO_REQUEST:
	case OFPT_ECHO_REPLY:
	case OFPT_EXPERIMENTER:
	case OFPT_FEATURES_REQUEST:
	case OFPT_FEATURES_REPLY:
	case OFPT_GET_CONFIG_REQUEST:
	case OFPT_GET_CONFIG_REPLY:
	case OFPT_SET_CONFIG:
	case OFPT_PACKET_IN:
	case OFPT_FLOW_REMOVED:
	case OFPT_PORT_STATUS:
	case OFPT_PACKET_OUT:
	case OFPT_FLOW_MOD:
	case OFPT_GROUP_MOD:
	case OFPT_PORT_MOD:
	case OFPT_TABLE_MOD:
	case OFPT_STATS_REQUEST:
	case OFPT_STATS_REPLY:
	case OFPT_BARRIER_REQUEST:
	case OFPT_BARRIER_REPLY:
	case OFPT_QUEUE_GET_CONFIG_REQUEST:
	case OFPT_QUEUE_GET_CONFIG_REPLY:
	case OFPT_ROLE_REQUEST:
	case OFPT_ROLE_REPLY:
		break;
	}
#endif

	return info.c_str();
}


cofpacket::typedesc_t typedesc[] = {
		{ OFPT_HELLO, "HELLO" },
		{ OFPT_ERROR, "ERROR" },
		{ OFPT_ECHO_REQUEST, "ECHO-REQUEST" },
		{ OFPT_ECHO_REPLY, "ECHO-REPLY" },
		{ OFPT_EXPERIMENTER, "EXPERIMENTER" },

		{ OFPT_FEATURES_REQUEST, "FEATURES-REQUEST" },
		{ OFPT_FEATURES_REPLY, "FEATURES-REPLY" },
		{ OFPT_GET_CONFIG_REQUEST, "GET-CONFIG-REQUEST" },
		{ OFPT_GET_CONFIG_REPLY, "GET-CONFIG-REPLY" },
		{ OFPT_SET_CONFIG, "SET-CONFIG" },

		{ OFPT_PACKET_IN, "PACKET-IN" },
		{ OFPT_FLOW_REMOVED, "FLOW-REMOVED" },
		{ OFPT_PORT_STATUS, "PORT-STATUS" },

		{ OFPT_PACKET_OUT, "PACKET-OUT" },
		{ OFPT_FLOW_MOD, "FLOW-MOD" },
		{ OFPT_GROUP_MOD, "GROUP-MOD" },
		{ OFPT_PORT_MOD, "PORT-MOD" },
		{ OFPT_TABLE_MOD, "TABLE-MOD" },

		{ OFPT_STATS_REQUEST, "STATS-REQUEST" },
		{ OFPT_STATS_REPLY, "STATS-REPLY" },

		{ OFPT_BARRIER_REQUEST, "BARRIER-REQUEST" },
		{ OFPT_BARRIER_REPLY, "BARRIER-REPLY" },

		{ OFPT_QUEUE_GET_CONFIG_REQUEST, "QUEUE-GET-CONFIG-REQUEST" },
		{ OFPT_QUEUE_GET_CONFIG_REPLY, "QUEUE-GET-CONFIG-REPLY" },
	};


const char*
cofpacket::type2desc(ofp_type ptype)
{
	for (int i = 0; i < (int)(sizeof(typedesc) / sizeof(typedesc_t)); i++)
	{
		if (typedesc[i].type == ptype) {
			return typedesc[i].desc;
		}
	}
	return NULL;
}


/*static*/
void
cofpacket::test()
{
#ifndef NDEBUG
	cmemory packed;
	cofmatch match;

	uint32_t xid = 0x74747474;
	cmemory body(10);
	for (unsigned int i = 0; i < body.memlen(); i++)
	{
		body[i] = 0x69;
	}


	match.set_in_port(47);
	match.set_in_phy_port(47);
	match.set_metadata(0x4343434343434343);



	/*
	 * HELLO
	 */
	uint32_t cookie = 0x55555555;
	cofpacket_hello hello(OFP12_VERSION, xid, (uint8_t*)&cookie, sizeof(cookie));
	packed.clear();
	packed.resize(hello.length());
	hello.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "HELLO: %s\n", packed.c_str());

	/*
	 * ECHO-REQUEST
	 */
	cofpacket_echo_request echo_request(OFP12_VERSION, xid, body.somem(), body.memlen());
	packed.clear();
	packed.resize(echo_request.length());
	echo_request.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "ECHO-REQUEST: %s\n", packed.c_str());

	/*
	 * ECHO-REPLY
	 */
	cofpacket_echo_reply echo_reply(OFP12_VERSION, xid, body.somem(), body.memlen());
	packed.clear();
	packed.resize(echo_reply.length());
	echo_reply.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "ECHO-REPLY: %s\n", packed.c_str());

	/*
	 * ERROR
	 */
	cofpacket_error error(OFP12_VERSION, xid, /*type=*/3, /*code=*/4, body.somem(), body.memlen());
	packed.clear();
	packed.resize(error.length());
	error.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "ERROR: %s\n", packed.c_str());

	/*
	 * EXPERIMENTER
	 */
	cofpacket_experimenter experimenter(OFP12_VERSION, xid, /*exp_id=*/7, /*exp_type=*/8, body.somem(), body.memlen());
	packed.clear();
	packed.resize(experimenter.length());
	experimenter.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "EXPERIMENTER: %s\n", packed.c_str());

	/*
	 * FEATURES-REQUEST
	 */
	cofpacket_features_request features_request(OFP12_VERSION, xid);
	packed.clear();
	packed.resize(features_request.length());
	features_request.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "FEATURES-REQUEST: %s\n", packed.c_str());

	/*
	 * FEATURES-REPLY
	 */
	uint64_t dpid = 0xaaeeaaeeaaeeaaee;
	uint32_t n_buffers = 2048;
	uint8_t n_tables = 1;
	uint32_t capabilities = 0x31313131;
	cofpacket_features_reply features_reply(xid, dpid, n_buffers, n_tables, capabilities);

	uint32_t port_no = 0x88888888;
	cmacaddr hwaddr("11:11:11:11:11:11");
	std::string devname("eth0");
	uint32_t config 	= 0xaabbcc01;
	uint32_t state 		= 0xaabbcc02;
	uint32_t curr		= 0xaabbcc03;
	uint32_t advertised = 0xaabbcc04;
	uint32_t supported 	= 0xaabbcc05;
	uint32_t peer		= 0xaabbcc06;
	uint32_t curr_speed	= 0xaabbcc07;
	uint32_t max_speed 	= 0xaabbcc08;

	cofport ofport;

	ofport.set_port_no(port_no);
	ofport.set_hwaddr(hwaddr);
	ofport.set_name(devname);
	ofport.set_config(config);
	ofport.set_state(state);
	ofport.set_curr(curr);
	ofport.set_advertised(advertised);
	ofport.set_supported(supported);
	ofport.set_peer(peer);
	ofport.set_curr_speed(curr_speed);
	ofport.set_max_speed(max_speed);

	features_reply.ports.next() = ofport;

	packed.clear();
	packed.resize(features_reply.length());
	features_reply.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "FEATURES-REPLY: %s\n", packed.c_str());

	/*
	 * GET-CONFIG-REQUEST
	 */
	cofpacket_get_config_request get_config_request(xid);
	packed.clear();
	packed.resize(get_config_request.length());
	get_config_request.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "GET-CONFIG-REQUEST: %s\n", packed.c_str());

	/*
	 * GET-CONFIG-REPLY
	 */
	uint16_t flags = 0xcccc;
	uint16_t miss_send_len = 128;
	cofpacket_get_config_reply get_config_reply(xid, flags, miss_send_len);
	packed.clear();
	packed.resize(get_config_reply.length());
	get_config_reply.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "GET-CONFIG-REPLY: %s\n", packed.c_str());

	/*
	 * SET-CONFIG
	 */
	cofpacket_set_config set_config(xid, flags, miss_send_len);
	packed.clear();
	packed.resize(set_config.length());
	set_config.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "SET-CONFIG: %s\n", packed.c_str());

	/*
	 * PACKET-IN
	 */
	{
		uint32_t buffer_id = OFP_NO_BUFFER;
		uint16_t total_len = body.memlen();
		uint8_t reason = 0xbb;
		uint8_t table_id = 0xdd;
		cofpacket_packet_in packet_in(OFP12_VERSION, xid, buffer_id, total_len, reason, table_id, /*cookie=*/0, /*OF1.0-inport=*/0, body.somem(), body.memlen());
		packet_in.match.set_in_port(48);
		packet_in.match.set_in_phy_port(48);
		packet_in.match.set_metadata(0xee11ee11ee11ee11);
		packed.clear();
		packed.resize(packet_in.length());
		packet_in.pack(packed.somem(), packed.memlen());
		fprintf(stderr, "PACKET-IN: %s\n", packed.c_str());
	}

	/*
	 * FLOW-REMOVED
	 */
	{
		uint32_t xid = 0x74747474;
		uint64_t cookie = 0xb5b5b5b5b5b5b5b5;
		uint16_t priority = 0x9999;
		uint8_t reason = 0xbb;
		uint8_t table_id = 0xdd;
		uint32_t duration_sec = 0x11111111;
		uint32_t duration_nsec = 0x22222222;
		uint16_t idle_timeout = 0x3333;
		uint16_t hard_timeout = 0x4444;
		uint64_t packet_count = 0x5353535353535353;
		uint64_t byte_count = 0x6464646464646464;

		cofpacket_flow_removed flow_removed(
										OFP12_VERSION,
										xid,
										cookie,
										priority,
										reason,
										table_id,
										duration_sec,
										duration_nsec,
										idle_timeout,
										hard_timeout,
										packet_count,
										byte_count);

		flow_removed.match.set_in_port(48);
		flow_removed.match.set_in_phy_port(48);
		flow_removed.match.set_metadata(0xee11ee11ee11ee11);
		packed.clear();
		packed.resize(flow_removed.length());
		flow_removed.pack(packed.somem(), packed.memlen());
		fprintf(stderr, "FLOW-REMOVED: %s\n", packed.c_str());
	}

	/*
	 * PORT-STATUS
	 */
	{
		uint8_t reason = 0xbb;
		cofport desc;
		desc.set_port_no(0xaabbcc01);
		desc.set_hwaddr(cmacaddr("11:11:11:11:11:11"));
		desc.set_name(std::string("eth0"));
		desc.set_config(0xaabbcc02);
		desc.set_state(0xaabbcc03);
		desc.set_curr(0xaabbcc04);
		desc.set_advertised(0xaabbcc05);
		desc.set_supported(0xaabbcc06);
		desc.set_peer(0xaabbcc07);
		desc.set_curr_speed(0xaabbcc08);
		desc.set_max_speed(0xaabbcc09);

		cofpacket_port_status port_status(
										OFP12_VERSION,
										xid,
										reason,
										desc);

		packed.clear();
		packed.resize(port_status.length());
		port_status.pack(packed.somem(), packed.memlen());
		fprintf(stderr, "PORT-STATUS: %s\n", packed.c_str());
	}

	/*
	 * PACKET-OUT
	 */
	{
		uint32_t buffer_id 	= OFP_NO_BUFFER;
		uint32_t in_port 	= 48;

		cofpacket_packet_out packet_out(
										OFP12_VERSION,
										xid,
										buffer_id,
										in_port,
										body.somem(),
										body.memlen());

		cofaclist actions;
		actions.next() = cofaction_output(47);

		packet_out.actions = actions;

		packed.clear();
		packed.resize(packet_out.length());
		packet_out.pack(packed.somem(), packed.memlen());
		fprintf(stderr, "PACKET-OUT: %s\n", packed.c_str());
	}

	/*
	 * FLOW-MOD
	 */
	{
		uint32_t xid 			= 0x74747474;
		uint64_t cookie 		= 0x5353535353535353;
		uint64_t cookie_mask 	= 0x6464646464646464;
		uint8_t table_id 		= 0xbb;
		uint8_t command 		= 0xcc;
		uint16_t idle_timeout 	= 0x1111;
		uint16_t hard_timeout 	= 0x2222;
		uint16_t priority 		= 0x3333;
		uint32_t buffer_id 		= OFP_NO_BUFFER;
		uint32_t out_port 		= 48;
		uint32_t out_group 		= 0xf1f2f3f4;
		uint16_t flags 			= 0xdddd;

		cofpacket_flow_mod flow_mod(
								OFP12_VERSION,
								xid,
								cookie,
								cookie_mask,
								table_id,
								command,
								idle_timeout,
								hard_timeout,
								priority,
								buffer_id,
								out_port,
								out_group,
								flags);

		flow_mod.match.set_in_port(48);
		flow_mod.match.set_in_phy_port(48);
		flow_mod.match.set_metadata(0xee11ee11ee11ee11);
		flow_mod.match.set_ip_proto(6);

		flow_mod.instructions.next() = cofinst_apply_actions();
		flow_mod.instructions[0].actions[0] = cofaction_output(0);
		flow_mod.instructions[0].actions[1] = cofaction_dec_nw_ttl();

		packed.clear();
		packed.resize(flow_mod.length());
		flow_mod.pack(packed.somem(), packed.memlen());
		fprintf(stderr, "FLOW-MOD: %s\n", packed.c_str());

		cofpacket unpack;
		unpack.memarea = packed;
		unpack.stored_bytes(packed.memlen());
		unpack.is_valid();
	}

	/*
	 * GROUP-MOD
	 */
	{
		uint32_t xid 			= 0x74747474;
		uint16_t command 		= 0xcccc;
		uint8_t  type			= 0xdd;
		uint32_t group_id		= 0xeeeeeeee;

		cofpacket_group_mod pack(
								OFP12_VERSION,
								xid,
								command,
								type,
								group_id);

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "GROUP-MOD: %s\n", mem.c_str());
	}

	/*
	 * PORT-MOD
	 */
	{
		uint32_t xid 			= 0x74747474;
		uint32_t port_no 		= 0x11111111;
		cmacaddr hwaddr("22:22:22:22:22:22");
		uint32_t config 		= 0x33333333;
		uint32_t mask 			= 0x44444444;
		uint32_t advertise 		= 0x55555555;

		cofpacket_port_mod pack(
								OFP12_VERSION,
								xid,
								port_no,
								hwaddr,
								config,
								mask,
								advertise);

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "PORT-MOD: %s\n", mem.c_str());
	}

	/*
	 * TABLE-MOD
	 */
	{
		uint32_t xid 			= 0x74747474;
		uint8_t  table_id		= 0x11;
		uint32_t config 		= 0x33333333;

		cofpacket_table_mod pack(
								OFP12_VERSION,
								xid,
								table_id,
								config);

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "TABLE-MOD: %s\n", mem.c_str());
	}

	/*
	 * STATS-REQUEST
	 */
	{
		uint32_t xid 			= 0x74747474;
		uint16_t type			= 0x1111;
		uint16_t flags			= 0x2222;

		cofpacket_stats_request pack(
								OFP12_VERSION,
								xid,
								type,
								flags,
								body.somem(),
								body.memlen());

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "STATS-REQUEST: %s\n", mem.c_str());
	}

	/*
	 * STATS-REPLY
	 */
	{
		uint32_t xid 			= 0x74747474;
		uint16_t type			= 0x1111;
		uint16_t flags			= 0x2222;

		cofpacket_stats_reply pack(
								OFP12_VERSION,
								xid,
								type,
								flags,
								body.somem(),
								body.memlen());

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "STATS-REPLY: %s\n", mem.c_str());
	}

	/*
	 * BARRIER-REQUEST
	 */
	{
		uint32_t xid 			= 0x74747474;

		cofpacket_barrier_request pack(
								OFP12_VERSION,
								xid);

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "BARRIER-REQUEST: %s\n", mem.c_str());
	}

	/*
	 * BARRIER-REPLY
	 */
	{
		uint32_t xid 			= 0x74747474;

		cofpacket_barrier_reply pack(
								OFP12_VERSION,
								xid);

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "BARRIER-REPLY: %s\n", mem.c_str());
	}

	/*
	 * QUEUE-GET-CONFIG-REQUEST
	 */
	{
		uint32_t xid 			= 0x74747474;
		uint32_t port			= 0x11223344;

		cofpacket_queue_get_config_request pack(
								OFP12_VERSION,
								xid,
								port);

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "QUEUE-GET-CONFIG-REQUEST: %s\n", mem.c_str());
	}

	/*
	 * QUEUE-GET-CONFIG-REPLY
	 */
	{
		uint32_t xid 			= 0x74747474;
		uint32_t port			= 0x11223344;

		cofpacket_queue_get_config_reply pack(
								OFP12_VERSION,
								xid,
								port);

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "QUEUE-GET-CONFIG-REPLY: %s\n", mem.c_str());
	}

	/*
	 * ROLE-REQUEST
	 */
	{
		uint32_t xid 			= 0x74747474;
		uint32_t role			= 0x11111111;
		uint64_t generation_id	= 0x22ee22ee22ee22ee;

		cofpacket_role_request pack(
								OFP12_VERSION,
								xid,
								role,
								generation_id);

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "ROLE-REQUEST: %s\n", mem.c_str());
	}

	/*
	 * ROLE-REPLY
	 */
	{
		uint32_t xid 			= 0x74747474;
		uint32_t role			= 0x11111111;
		uint64_t generation_id	= 0x22ee22ee22ee22ee;

		cofpacket_role_reply pack(
								OFP12_VERSION,
								xid,
								role,
								generation_id);

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "ROLE-REPLY: %s\n", mem.c_str());
	}
#endif
}




/*
 * Port-Modify
 */


cofpacket_port_mod::cofpacket_port_mod(
		uint8_t of_version,
		uint32_t xid,
		uint32_t port_no,
		cmacaddr const& hwaddr,
		uint32_t config,
		uint32_t mask,
		uint32_t advertise) :
	cofpacket(	sizeof(struct ofp_header),
				sizeof(struct ofp_header))
{
	ofh_header->version 	= of_version;
	ofh_header->length		= htobe16(length());
	ofh_header->type 		= OFPT_PORT_MOD;
	ofh_header->xid			= htobe32(xid);

	switch (of_version) {
	case OFP10_VERSION: {
		cofpacket::resize(sizeof(struct ofp10_port_mod));
		cofpacket::stored = sizeof(struct ofp10_port_mod);

		of10h_port_mod->port_no			= htobe16((uint16_t)(port_no & 0x0000ffff));
		of10h_port_mod->config			= htobe32(config);
		of10h_port_mod->mask			= htobe32(mask);
		of10h_port_mod->advertise		= htobe32(advertise);

		memcpy(of10h_port_mod->hw_addr, hwaddr.somem(), OFP_ETH_ALEN);

	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		cofpacket::resize(sizeof(struct ofp12_port_mod));
		cofpacket::stored = sizeof(struct ofp12_port_mod);

		of12h_port_mod->port_no			= htobe32(port_no);
		of12h_port_mod->config			= htobe32(config);
		of12h_port_mod->mask			= htobe32(mask);
		of12h_port_mod->advertise		= htobe32(advertise);

		memcpy(of12h_port_mod->hw_addr, hwaddr.somem(), OFP_ETH_ALEN);

	} break;
	default:
		throw eBadVersion();
	}
};



cofpacket_port_mod::cofpacket_port_mod(cofpacket const *pack) :
	cofpacket(pack->framelen(), pack->framelen())
{
	cofpacket::operator =(*pack);
};




cofpacket_port_mod::~cofpacket_port_mod() {};



size_t
cofpacket_port_mod::length()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_port_mod));
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return (sizeof(struct ofp12_port_mod));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
};



uint32_t
cofpacket_port_mod::get_port_no()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return (uint32_t)be16toh(of10h_port_mod->port_no);
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return be32toh(of12h_port_mod->port_no);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
};



uint32_t
cofpacket_port_mod::get_config()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return be32toh(of10h_port_mod->config);
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return be32toh(of12h_port_mod->config);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
};



uint32_t
cofpacket_port_mod::get_mask()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return be32toh(of10h_port_mod->mask);
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return be32toh(of12h_port_mod->mask);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
};



uint32_t
cofpacket_port_mod::get_advertise()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return be32toh(of10h_port_mod->advertise);
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return be32toh(of12h_port_mod->advertise);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
};



/*
 * OFPT_GROUP_MOD
 */

cofpacket_group_mod::cofpacket_group_mod(
				uint8_t of_version,
				uint32_t xid,
				uint16_t command,
				uint8_t  type,
				uint32_t group_id) :
			cofpacket(	sizeof(struct ofp_group_mod),
						sizeof(struct ofp_group_mod))
{
	ofh_header->version 	= of_version;
	ofh_header->length		= htobe16(sizeof(struct ofp_group_mod));
	ofh_header->type 		= OFPT_GROUP_MOD;
	ofh_header->xid			= htobe32(xid);

	switch (of_version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		ofh_group_mod->command		= htobe16(command);
		ofh_group_mod->type			= type;
		ofh_group_mod->group_id		= htobe32(group_id);

	} break;
	default:
		throw eBadVersion();
	}
}



cofpacket_group_mod::cofpacket_group_mod(cofpacket const *pack) :
			cofpacket(pack->framelen(), pack->framelen())
{
	cofpacket::operator =(*pack);
};




cofpacket_group_mod::~cofpacket_group_mod() {};



size_t
cofpacket_group_mod::length()
{
	switch (ofh_header->version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return (sizeof(struct ofp_group_mod) + buckets.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofpacket_group_mod::pack(uint8_t *buf, size_t buflen) throw (eOFpacketInval)
{
	ofh_header->length = htobe16(length());

	if (((uint8_t*)0 == buf) || (buflen < length()))
	{
		return;
	}

	switch (ofh_header->version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		memcpy(buf, memarea.somem(), sizeof(struct ofp_group_mod));
		buckets.pack((struct ofp_bucket*)(buf + sizeof(struct ofp_group_mod)), buckets.length());

	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofpacket_group_mod::get_command()
{
	switch (ofh_header->version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return be16toh(ofh_group_mod->command);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}



uint8_t
cofpacket_group_mod::get_port_no()
{
	switch (ofh_header->version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return ofh_group_mod->type;
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}



uint32_t
cofpacket_group_mod::get_group_id()
{
	switch (ofh_header->version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return be32toh(ofh_group_mod->group_id);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}



/*
 * OFPT_TABLE_MOD
 */


cofpacket_table_mod::cofpacket_table_mod(
		uint8_t of_version,
		uint32_t xid,
		uint8_t  table_id,
		uint32_t config) :
	cofpacket(	sizeof(struct ofp_table_mod),
				sizeof(struct ofp_table_mod))
{
	ofh_header->version 	= of_version;
	ofh_header->length		= htobe16(sizeof(struct ofp_table_mod));
	ofh_header->type 		= OFPT_TABLE_MOD;
	ofh_header->xid			= htobe32(xid);

	switch (ofh_header->version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		ofh_table_mod->table_id			= table_id;
		ofh_table_mod->config			= htobe32(config);
	} break;
	default:
		throw eBadVersion();
	}
}



cofpacket_table_mod::cofpacket_table_mod(cofpacket const *pack) :
	cofpacket(pack->framelen(), pack->framelen())
{
	cofpacket::operator =(*pack);
}



cofpacket_table_mod::~cofpacket_table_mod()
{

}



size_t
cofpacket_table_mod::length()
{
	switch (ofh_header->version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return (sizeof(struct ofp_table_mod));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



uint8_t
cofpacket_table_mod::get_table_id()
{
	switch (ofh_header->version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return ofh_table_mod->table_id;
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}



uint32_t
cofpacket_table_mod::get_config()
{
	switch (ofh_header->version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return be32toh(ofh_table_mod->config);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}





/*
 * OFPT_STATS_REQUEST
 */


cofpacket_stats_request::cofpacket_stats_request(
		uint8_t of_version,
		uint32_t xid,
		uint16_t type,
		uint16_t flags,
		uint8_t *data,
		size_t datalen) :
	cofpacket(	sizeof(struct ofp_header),
				sizeof(struct ofp_header))
{
	cofpacket::body.assign(data, datalen);

	ofh_header->version 	= of_version;
	ofh_header->length		= htobe16(length());
	ofh_header->type 		= OFPT_STATS_REQUEST;
	ofh_header->xid			= htobe32(xid);

	switch (get_version()) {
	case OFP10_VERSION: {
		cofpacket::resize(sizeof(struct ofp10_stats_request));
		of10h_stats_request->type	= htobe16(type);
		of10h_stats_request->flags	= htobe16(flags);
	} break;
	case OFP12_VERSION: {
		cofpacket::resize(sizeof(struct ofp12_stats_request));
		of12h_stats_request->type	= htobe16(type);
		of12h_stats_request->flags	= htobe16(flags);
	} break;
	default:
		throw eBadVersion();
	}
}



cofpacket_stats_request::cofpacket_stats_request(cofpacket const *pack) :
	cofpacket(pack->framelen(), pack->framelen())
{
	cofpacket::operator =(*pack);
}



cofpacket_stats_request::~cofpacket_stats_request()
{

}



size_t
cofpacket_stats_request::length()
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_stats_request) + body.memlen());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_stats_request) + body.memlen()); // 4 bytes padding make the different from 1.0 to 1.2
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofpacket_stats_request::pack(uint8_t *buf, size_t buflen) throw (eOFpacketInval)
{
	ofh_header->length = htobe16(length());

	if (((uint8_t*)0 == buf) || (buflen < length()))
	{
		return;
	}

	switch (get_version()) {
	case OFP10_VERSION: {
		memcpy(buf, memarea.somem(), sizeof(struct ofp10_stats_request));
		memcpy(buf + sizeof(struct ofp10_stats_request), body.somem(), body.memlen());
	} break;
	case OFP12_VERSION: {
		memcpy(buf, memarea.somem(), sizeof(struct ofp12_stats_request));
		memcpy(buf + sizeof(struct ofp12_stats_request), body.somem(), body.memlen());
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofpacket_stats_request::get_type()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return be16toh(of10h_stats_request->type);
	} break;
	case OFP12_VERSION: {
		return be16toh(of12h_stats_request->type);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



uint16_t
cofpacket_stats_request::get_flags()
{
	switch (ofh_header->version) {
	case OFP13_VERSION: {
		return be16toh(of10h_stats_request->flags);
	} break;
	case OFP12_VERSION: {
		return be16toh(of12h_stats_request->flags);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}




/*
 * OFPT_STATS_REPLY
 */


cofpacket_stats_reply::cofpacket_stats_reply(
		uint8_t of_version,
		uint32_t xid,
		uint16_t type,
		uint16_t flags,
		uint8_t *data,
		size_t datalen) :
	cofpacket(	sizeof(struct ofp_header),
				sizeof(struct ofp_header))
{
	cofpacket::body.assign(data, datalen);

	ofh_header->version 	= of_version;
	ofh_header->length		= htobe16(length());
	ofh_header->type 		= OFPT_STATS_REPLY;
	ofh_header->xid			= htobe32(xid);

	switch (of_version) {
	case OFP10_VERSION: {
		cofpacket::resize(sizeof(struct ofp10_stats_reply));
		of10h_stats_reply->type		= htobe16(type);
		of10h_stats_reply->flags	= htobe16(flags);

	} break;
	case OFP12_VERSION: {
		cofpacket::resize(sizeof(struct ofp12_stats_reply));
		of12h_stats_reply->type		= htobe16(type);
		of12h_stats_reply->flags	= htobe16(flags);

	} break;
	default:
		throw eBadVersion();
	}
}



cofpacket_stats_reply::cofpacket_stats_reply(cofpacket const *pack) :
	cofpacket(pack->framelen(), pack->framelen())
{
	cofpacket::operator=(*pack);
}



cofpacket_stats_reply::~cofpacket_stats_reply()
{

}



size_t
cofpacket_stats_reply::length()
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_stats_reply) + body.memlen());
	} break;
	case OFP12_VERSION: {
		return (sizeof(struct ofp12_stats_reply) + body.memlen());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofpacket_stats_reply::pack(uint8_t *buf, size_t buflen) throw (eOFpacketInval)
{
	ofh_header->length = htobe16(length());

	if (((uint8_t*)0 == buf) || (buflen < length()))
	{
		return;
	}

	switch (get_version()) {
	case OFP10_VERSION: {
		memcpy(buf, memarea.somem(), sizeof(struct ofp10_stats_reply));
		if (body.memlen() > 0) {
			memcpy(buf + sizeof(struct ofp10_stats_reply), body.somem(), body.memlen());
		}
	} break;
	case OFP12_VERSION: {
		memcpy(buf, memarea.somem(), sizeof(struct ofp12_stats_reply));
		if (body.memlen() > 0) {
			memcpy(buf + sizeof(struct ofp12_stats_reply), body.somem(), body.memlen());
		}
	} break;
	default:
		throw eBadVersion();
	}
}



uint16_t
cofpacket_stats_reply::get_type()
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(of10h_stats_reply->type);
	} break;
	case OFP12_VERSION: {
		return be16toh(of12h_stats_reply->type);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



uint16_t
cofpacket_stats_reply::get_flags()
{
	switch (get_version()) {
	case OFP10_VERSION: {
		return be16toh(of10h_stats_reply->flags);
	} break;
	case OFP12_VERSION: {
		return be16toh(of12h_stats_reply->flags);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}






/*
 * OFPT_QUEUE_GET_CONFIG_REQUEST
 */



cofpacket_queue_get_config_request::cofpacket_queue_get_config_request(
		uint8_t of_version,
		uint32_t xid,
		uint32_t port) :
	cofpacket(	sizeof(struct ofp_header),
				sizeof(struct ofp_header))
{
	ofh_header->version 	= of_version;
	ofh_header->length		= htobe16(length());
	ofh_header->type 		= OFPT_QUEUE_GET_CONFIG_REQUEST;
	ofh_header->xid			= htobe32(xid);

	switch (ofh_header->version) {
	case OFP10_VERSION: {
		cofpacket::resize(sizeof(struct ofp10_queue_get_config_request));
		of10h_queue_get_config_request->port = htobe16((uint16_t)(port & 0x0000ffff));
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		cofpacket::resize(sizeof(struct ofp12_queue_get_config_request));
		of12h_queue_get_config_request->port = htobe32(port);
	} break;
	default:
		throw eBadVersion();
	}
}



cofpacket_queue_get_config_request::cofpacket_queue_get_config_request(cofpacket const *pack) :
	cofpacket(pack->framelen(), pack->framelen())
{
	cofpacket::operator =(*pack);
}



cofpacket_queue_get_config_request::~cofpacket_queue_get_config_request()
{

}



size_t
cofpacket_queue_get_config_request::length()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_queue_get_config_request));
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return (sizeof(struct ofp12_queue_get_config_request));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



uint32_t
cofpacket_queue_get_config_request::get_port() const
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return (uint32_t)be16toh(of10h_queue_get_config_request->port);
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return be32toh(of12h_queue_get_config_request->port);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}





/*
 * OFPT_QUEUE_GET_CONFIG_REPLY
 */



cofpacket_queue_get_config_reply::cofpacket_queue_get_config_reply(
		uint8_t of_version,
		uint32_t xid,
		uint32_t port) :
	cofpacket(	sizeof(struct ofp_header),
				sizeof(struct ofp_header))
{
	ofh_header->version 	= of_version;
	ofh_header->length		= htobe16(length());
	ofh_header->type 		= OFPT_QUEUE_GET_CONFIG_REPLY;
	ofh_header->xid			= htobe32(xid);

	switch (ofh_header->version) {
	case OFP10_VERSION: {
		cofpacket::resize(sizeof(struct ofp10_queue_get_config_reply));
		of10h_queue_get_config_reply->port = htobe16((uint16_t)(port & 0x0000ffff));
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		cofpacket::resize(sizeof(struct ofp12_queue_get_config_reply));
		of12h_queue_get_config_reply->port = htobe32(port);
	} break;
	default:
		throw eBadVersion();
	}
}



cofpacket_queue_get_config_reply::cofpacket_queue_get_config_reply(cofpacket const *pack) :
	cofpacket(pack->framelen(), pack->framelen())
{
	cofpacket::operator =(*pack);
}



cofpacket_queue_get_config_reply::~cofpacket_queue_get_config_reply()
{

}



size_t
cofpacket_queue_get_config_reply::length()
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return (sizeof(struct ofp10_queue_get_config_reply));
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return (sizeof(struct ofp12_queue_get_config_reply));
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofpacket_queue_get_config_reply::get_port() const
{
	switch (ofh_header->version) {
	case OFP10_VERSION: {
		return (uint32_t)be16toh(of10h_queue_get_config_reply->port);
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		return be32toh(of12h_queue_get_config_reply->port);
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
	return 0;
}




