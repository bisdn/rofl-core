/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofpacket.h"

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
	body(0),
	packet((size_t)0),
	switch_features_num_ports(0)
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

	memarea 		= p.memarea;
	match			= p.match;
	actions			= p.actions;
	buckets 		= p.buckets;
	instructions 	= p.instructions;
	body			= p.body;
	packet			= p.packet;

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
	match.reset();
	actions.reset();
	buckets.reset();
	instructions.reset();
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
		return is_valid_stats_request();
	case OFPT_STATS_REPLY:
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
	default:
		return false;
	}
	//WRITELOG(CPACKET, DBG, "cOFpacket is valid");
	return true;
}


bool
cofpacket::is_valid_hello_msg()
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
		return true;
	}
	body.assign(ofh_error_msg->data,
			stored - sizeof(struct ofp_error_msg));
	return true;
}

bool
cofpacket::is_valid_switch_features()
{
	ofh_switch_features = (struct ofp_switch_features*)soframe();
	if (stored < sizeof(struct ofp_switch_features))
		return false;

	size_t ports_len = stored - sizeof(struct ofp_switch_features);

	if (ports_len > 0)
	{
		body.assign((uint8_t*)ofh_switch_features->ports, ports_len);
		switch_features_num_ports = (ports_len) / sizeof(struct ofp_port);
	}
	else
	{
		body.clear();
		switch_features_num_ports = 0;
	}

	return true;
}

bool
cofpacket::is_valid_switch_config()
{
	ofh_switch_config = (struct ofp_switch_config*)soframe();
	if (stored < sizeof(struct ofp_switch_config))
		return false;
	return true;
}

bool
cofpacket::is_valid_packet_in()
{
	ofh_packet_in = (struct ofp_packet_in*)soframe();

	/*
	 * static part of struct ofp_packet_in also contains static fields from struct ofp_match (i.e. type and length)
	 */
	if (stored < sizeof(struct ofp_packet_in))
	{
		return false;
	}

	/*
	 * get variable length struct ofp_match
	 */
	if (be16toh(ofh_packet_in->match.type) != OFPMT_OXM) // must be extensible match
	{
		return false;
	}

	if (be16toh(ofh_packet_in->match.length) > (stored - 16 /* fixed part outside of struct ofp_match is 16bytes */))
	{
		return false;
	}

	match.unpack(&(ofh_packet_in->match), be16toh(ofh_packet_in->match.length));

	/*
	 * set data and datalen variables
	 */
	uint16_t offset = OFP_PACKET_IN_STATIC_HDR_LEN + match.length() + 2;

	//body.assign((uint8_t*)(soframe() + offset), stored - (offset)); // +2: magic :)

	uint32_t in_port = 0;

	try {
		in_port = match.get_in_port();
	} catch (eOFmatchNotFound& e) {
		in_port = 0;
	}

	packet.unpack(in_port, (uint8_t*)(soframe() + offset), stored - (offset)); // +2: magic :)

	return true; // packet is valid
}

bool
cofpacket::is_valid_packet_out()
{
	ofh_packet_out = (struct ofp_packet_out*)soframe();
	if (stored < sizeof(struct ofp_packet_out))
		return false;

#if 0
	body.assign(((uint8_t*)ofh_packet_out) +
					sizeof(struct ofp_packet_out) +
						be16toh(ofh_packet_out->actions_len),
						be16toh(ofh_packet_out->header.length) -
											sizeof(struct ofp_packet_out) -
												be16toh(ofh_packet_out->actions_len));
#endif

	packet.unpack(OFPP_CONTROLLER,
				((uint8_t*)ofh_packet_out) +
					sizeof(struct ofp_packet_out) +
						be16toh(ofh_packet_out->actions_len),
						be16toh(ofh_packet_out->header.length) -
											sizeof(struct ofp_packet_out) -
												be16toh(ofh_packet_out->actions_len));

	actions.unpack(ofh_packet_out->actions,
					be16toh(ofh_packet_out->actions_len));

	return true;
}

bool
cofpacket::is_valid_flow_removed()
{
	ofh_flow_removed = (struct ofp_flow_removed*)soframe();

	size_t frgenlen = sizeof(struct ofp_flow_removed) - sizeof(struct ofp_match);

	if (stored < frgenlen)
		return false;

	size_t matchlen = be16toh(ofh_flow_removed->header.length);

	try  {
		match.unpack(&(ofh_flow_removed->match), matchlen);
	} catch (eOFmatchInval& e) {
		return false;
	}

	return true;	
}

bool
cofpacket::is_valid_port_status()
{
	ofh_port_status = (struct ofp_port_status*)soframe();
	if (stored < sizeof(struct ofp_port_status))
		return false;
	return true;
}


bool
cofpacket::is_valid_flow_mod()
{
	try {
		ofh_flow_mod = (struct ofp_flow_mod*)soframe();

		if (stored < sizeof(struct ofp_flow_mod)) // includes static part of struct ofp_match (i.e. type and length)
		{
			return false;
		}

		/* OFP_FLOW_MOD_STATIC_HDR_LEN = length of generic flow-mod header
		 * according to OpenFlow-spec-1.2 is 48bytes
		 */

		if ((be16toh(ofh_flow_mod->match.length)) > (stored - OFP_FLOW_MOD_STATIC_HDR_LEN))
						// stored - OFP_FLOW_MOD_STATIC_HDR_LEN is #bytes for struct ofp_match and array of struct ofp_instructions
		{
			fprintf(stderr, "1.2 ");
			return false; // match too long
		}

		/*
		 * unpack ofp_match structure
		 */

		match.unpack(&(ofh_flow_mod->match), be16toh(ofh_flow_mod->match.length));

		WRITELOG(COFPACKET, DBG, "cofpacket(%p)::is_valid_flow_mod() "
				"match: %s", this, match.c_str());

		// check size of received flow-mod message
		/*
		 * match.length() returns length of struct ofp_match including padding
		 */
		if ((OFP_FLOW_MOD_STATIC_HDR_LEN + match.length()) > stored)
		{
			return false;
		}

		/*
		 * unpack instructions list
		 */

		struct ofp_instruction *insts = (struct ofp_instruction*)(
								(uint8_t*)&(ofh_flow_mod->match) + match.length());

		size_t instslen = stored -
				(OFP_FLOW_MOD_STATIC_HDR_LEN + match.length());

		instructions.unpack(insts, instslen);

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

	} catch (eActionBadOutPort& e) {
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
	ofh_port_mod = (struct ofp_port_mod*)soframe();
	if (stored < sizeof(struct ofp_port_mod))
		return false;
	return true;
}

bool
cofpacket::is_valid_stats_request()
{
	ofh_stats_request = (struct ofp_stats_request*)soframe();
	if (stored < sizeof(struct ofp_stats_request))
		return false;

	size_t body_len = stored - sizeof(struct ofp_stats_request);

	body.assign((uint8_t*)ofh_stats_request->body, body_len);

	ofb_stats_request = (uint8_t*)body.somem();

	switch (be16toh(ofh_stats_request->type)) {
	case OFPST_FLOW:
		{
			match.reset();
			size_t match_len = stored - OFP_FLOW_STATS_REQUEST_STATIC_HDR_LEN;
			match.unpack(&(ofb_flow_stats_request->match), match_len);
		}
		break;
	case OFPST_AGGREGATE:
		{
			match.reset();
			size_t match_len = stored - OFP_AGGR_STATS_REQUEST_STATIC_HDR_LEN;
			match.unpack(&(ofb_aggregate_stats_request->match), match_len);
		}
		break;
	}

	// TODO: description fields
	return true;
}

bool
cofpacket::is_valid_stats_reply()
{
	ofh_stats_reply = (struct ofp_stats_reply*)soframe();
	if (stored < sizeof(struct ofp_stats_reply))
		return false;

	body.assign((uint8_t*)ofh_stats_reply->body,
			stored - sizeof(struct ofp_stats_reply));

	ofb_stats_request = (uint8_t*)body.somem();

	// TODO: description fields
	return true;
}

bool
cofpacket::is_valid_queue_get_config_request()
{
	ofh_queue_get_config_request = (struct ofp_queue_get_config_request*)soframe();
	if (stored < sizeof(struct ofp_queue_get_config_request))
		return false;
	return true;
}

bool
cofpacket::is_valid_queue_get_config_reply()
{
	ofh_queue_get_config_reply = (struct ofp_queue_get_config_reply*)soframe();
	if (stored < sizeof(struct ofp_queue_get_config_reply))
		return false;
	body.assign((uint8_t*)ofh_queue_get_config_reply->queues,
			stored - sizeof(struct ofp_queue_get_config_reply));
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


	match.oxmlist[OFPXMT_OFB_IN_PORT] = coxmatch_ofb_in_port(47);
	match.oxmlist[OFPXMT_OFB_IN_PHY_PORT] = coxmatch_ofb_in_phy_port(47);
	match.oxmlist[OFPXMT_OFB_METADATA] = coxmatch_ofb_metadata(0x4343434343434343);



	/*
	 * HELLO
	 */
	uint32_t cookie = 0x55555555;
	cofpacket_hello hello(xid, (uint8_t*)&cookie, sizeof(cookie));
	packed.clear();
	packed.resize(hello.length());
	hello.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "HELLO: %s\n", packed.c_str());

	/*
	 * ECHO-REQUEST
	 */
	cofpacket_echo_request echo_request(xid, body.somem(), body.memlen());
	packed.clear();
	packed.resize(echo_request.length());
	echo_request.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "ECHO-REQUEST: %s\n", packed.c_str());

	/*
	 * ECHO-REPLY
	 */
	cofpacket_echo_reply echo_reply(xid, body.somem(), body.memlen());
	packed.clear();
	packed.resize(echo_reply.length());
	echo_reply.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "ECHO-REPLY: %s\n", packed.c_str());

	/*
	 * ERROR
	 */
	cofpacket_error error(xid, /*type=*/3, /*code=*/4, body.somem(), body.memlen());
	packed.clear();
	packed.resize(error.length());
	error.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "ERROR: %s\n", packed.c_str());

	/*
	 * EXPERIMENTER
	 */
	cofpacket_experimenter experimenter(xid, /*exp_id=*/7, /*exp_type=*/8, body.somem(), body.memlen());
	packed.clear();
	packed.resize(experimenter.length());
	experimenter.pack(packed.somem(), packed.memlen());
	fprintf(stderr, "EXPERIMENTER: %s\n", packed.c_str());

	/*
	 * FEATURES-REQUEST
	 */
	cofpacket_features_request features_request(xid);
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

	ofport.port_no 		= port_no;
	ofport.hwaddr 		= hwaddr;
	ofport.name 		= devname;
	ofport.config 		= config;
	ofport.state 		= state;
	ofport.curr 		= curr;
	ofport.advertised 	= advertised;
	ofport.supported 	= supported;
	ofport.peer 		= peer;
	ofport.curr_speed 	= curr_speed;
	ofport.max_speed 	= max_speed;

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
		cofpacket_packet_in packet_in(xid, buffer_id, total_len, reason, table_id, body.somem(), body.memlen());
		packet_in.match.oxmlist[OFPXMT_OFB_IN_PORT] = coxmatch_ofb_in_port(48);
		packet_in.match.oxmlist[OFPXMT_OFB_IN_PHY_PORT] = coxmatch_ofb_in_phy_port(48);
		packet_in.match.oxmlist[OFPXMT_OFB_METADATA] = coxmatch_ofb_metadata(0xee11ee11ee11ee11);
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

		flow_removed.match.oxmlist[OFPXMT_OFB_IN_PORT] = coxmatch_ofb_in_port(48);
		flow_removed.match.oxmlist[OFPXMT_OFB_IN_PHY_PORT] = coxmatch_ofb_in_phy_port(48);
		flow_removed.match.oxmlist[OFPXMT_OFB_METADATA] = coxmatch_ofb_metadata(0xee11ee11ee11ee11);
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
		struct ofp_port desc;
		desc.port_no 		= 0xaabbcc01;
		desc.hw_addr[0]		= 0x11;
		desc.hw_addr[1]		= 0x11;
		desc.hw_addr[2]		= 0x11;
		desc.hw_addr[3]		= 0x11;
		desc.hw_addr[4]		= 0x11;
		desc.hw_addr[5]		= 0x11;
		strncpy(desc.name, "eth0", OFP_MAX_PORT_NAME_LEN - 1);
		desc.config  		= 0xaabbcc02;
		desc.state			= 0xaabbcc03;
		desc.curr			= 0xaabbcc04;
		desc.advertised 	= 0xaabbcc05;
		desc.supported		= 0xaabbcc06;
		desc.peer			= 0xaabbcc07;
		desc.curr_speed		= 0xaabbcc08;
		desc.max_speed		= 0xaabbcc09;

		cofpacket_port_status port_status(
										xid,
										reason,
										&desc,
										sizeof(desc));

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

		flow_mod.match.oxmlist[OFPXMT_OFB_IN_PORT] = coxmatch_ofb_in_port(48);
		flow_mod.match.oxmlist[OFPXMT_OFB_IN_PHY_PORT] = coxmatch_ofb_in_phy_port(48);
		flow_mod.match.oxmlist[OFPXMT_OFB_METADATA] = coxmatch_ofb_metadata(0xee11ee11ee11ee11);
		flow_mod.match.oxmlist[OFPXMT_OFB_IP_PROTO] = coxmatch_ofb_ip_proto(6);

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
								xid,
								role,
								generation_id);

		cmemory mem(pack.length());
		pack.pack(mem.somem(), mem.memlen());
		fprintf(stderr, "ROLE-REPLY: %s\n", mem.c_str());
	}
#endif
}


