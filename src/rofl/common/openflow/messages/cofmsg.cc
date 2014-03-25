/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/openflow/messages/cofmsg.h"

using namespace rofl::openflow;



cofmsg::typedesc_t typedesc_of10[] = {
	{ rofl::openflow10::OFPT_HELLO, 					"HELLO" },
	{ rofl::openflow10::OFPT_ERROR, 					"ERROR" },
	{ rofl::openflow10::OFPT_ECHO_REQUEST, 				"ECHO-REQUEST" },
	{ rofl::openflow10::OFPT_ECHO_REPLY, 				"ECHO-REPLY" },
	{ rofl::openflow10::OFPT_VENDOR, 					"VENDOR" },

	{ rofl::openflow10::OFPT_FEATURES_REQUEST, 			"FEATURES-REQUEST" },
	{ rofl::openflow10::OFPT_FEATURES_REPLY, 			"FEATURES-REPLY" },
	{ rofl::openflow10::OFPT_GET_CONFIG_REQUEST,	 	"GET-CONFIG-REQUEST" },
	{ rofl::openflow10::OFPT_GET_CONFIG_REPLY, 			"GET-CONFIG-REPLY" },
	{ rofl::openflow10::OFPT_SET_CONFIG, 				"SET-CONFIG" },

	{ rofl::openflow10::OFPT_PACKET_IN, 				"PACKET-IN" },
	{ rofl::openflow10::OFPT_FLOW_REMOVED, 				"FLOW-REMOVED" },
	{ rofl::openflow10::OFPT_PORT_STATUS, 				"PORT-STATUS" },

	{ rofl::openflow10::OFPT_PACKET_OUT, 				"PACKET-OUT" },
	{ rofl::openflow10::OFPT_FLOW_MOD, 					"FLOW-MOD" },
	{ rofl::openflow10::OFPT_PORT_MOD, 					"PORT-MOD" },

	{ rofl::openflow10::OFPT_STATS_REQUEST, 			"STATS-REQUEST" },
	{ rofl::openflow10::OFPT_STATS_REPLY, 				"STATS-REPLY" },

	{ rofl::openflow10::OFPT_BARRIER_REQUEST, 			"BARRIER-REQUEST" },
	{ rofl::openflow10::OFPT_BARRIER_REPLY, 			"BARRIER-REPLY" },

	{ rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REQUEST, 	"QUEUE-GET-CONFIG-REQUEST" },
	{ rofl::openflow10::OFPT_QUEUE_GET_CONFIG_REPLY, 	"QUEUE-GET-CONFIG-REPLY" },
};



cofmsg::typedesc_t typedesc_of12[] = {
	{ rofl::openflow12::OFPT_HELLO, 					"HELLO" },
	{ rofl::openflow12::OFPT_ERROR, 					"ERROR" },
	{ rofl::openflow12::OFPT_ECHO_REQUEST, 				"ECHO-REQUEST" },
	{ rofl::openflow12::OFPT_ECHO_REPLY, 				"ECHO-REPLY" },
	{ rofl::openflow12::OFPT_EXPERIMENTER, 				"EXPERIMENTER" },

	{ rofl::openflow12::OFPT_FEATURES_REQUEST, 			"FEATURES-REQUEST" },
	{ rofl::openflow12::OFPT_FEATURES_REPLY, 			"FEATURES-REPLY" },
	{ rofl::openflow12::OFPT_GET_CONFIG_REQUEST, 		"GET-CONFIG-REQUEST" },
	{ rofl::openflow12::OFPT_GET_CONFIG_REPLY, 			"GET-CONFIG-REPLY" },
	{ rofl::openflow12::OFPT_SET_CONFIG, 				"SET-CONFIG" },

	{ rofl::openflow12::OFPT_PACKET_IN, 				"PACKET-IN" },
	{ rofl::openflow12::OFPT_FLOW_REMOVED, 				"FLOW-REMOVED" },
	{ rofl::openflow12::OFPT_PORT_STATUS, 				"PORT-STATUS" },

	{ rofl::openflow12::OFPT_PACKET_OUT, 				"PACKET-OUT" },
	{ rofl::openflow12::OFPT_FLOW_MOD, 					"FLOW-MOD" },
	{ rofl::openflow12::OFPT_GROUP_MOD, 				"GROUP-MOD" },
	{ rofl::openflow12::OFPT_PORT_MOD, 					"PORT-MOD" },
	{ rofl::openflow12::OFPT_TABLE_MOD, 				"TABLE-MOD" },

	{ rofl::openflow12::OFPT_STATS_REQUEST, 			"STATS-REQUEST" },
	{ rofl::openflow12::OFPT_STATS_REPLY, 				"STATS-REPLY" },

	{ rofl::openflow12::OFPT_BARRIER_REQUEST, 			"BARRIER-REQUEST" },
	{ rofl::openflow12::OFPT_BARRIER_REPLY, 			"BARRIER-REPLY" },

	{ rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REQUEST, 	"QUEUE-GET-CONFIG-REQUEST" },
	{ rofl::openflow12::OFPT_QUEUE_GET_CONFIG_REPLY, 	"QUEUE-GET-CONFIG-REPLY" },

	{ rofl::openflow12::OFPT_ROLE_REQUEST, 				"ROLE-REQUEST" },
	{ rofl::openflow12::OFPT_ROLE_REPLY, 				"ROLE-REPLY" },
};


/*static*/std::set<cofmsg*> cofmsg::cofpacket_list;
/*static*/std::string cofmsg::pinfo;

/*static*/const char*
cofmsg::packet_info(uint8_t ofp_version)
{
	cvastring vas;
	pinfo.assign(vas("cofpackets allocated: %d\n", cofmsg::cofpacket_list.size()));

	std::map<uint8_t, unsigned int> counter;
	for (std::set<cofmsg*>::iterator
			it = cofmsg::cofpacket_list.begin();
				it != cofmsg::cofpacket_list.end(); ++it)
	{
		cofmsg *pack = (*it);
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
						cofmsg::type2desc(ofp_version, it->first),
						it->second));
	}

	return pinfo.c_str();
}



const char*
cofmsg::type2desc(uint8_t ofp_version, uint8_t ptype)
{
	switch (ofp_version) {
	case openflow10::OFP_VERSION: {
        for (int i = 0; i < (int)(sizeof(typedesc_of10) / sizeof(cofmsg::typedesc_t)); i++) {
			if (typedesc_of10[i].type == ptype) {
				return typedesc_of10[i].desc;
			}
        }
	} break;
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
        for (int i = 0; i < (int)(sizeof(typedesc_of12) / sizeof(cofmsg::typedesc_t)); i++) {
			if (typedesc_of12[i].type == ptype) {
				return typedesc_of12[i].desc;
			}
        }
	} break;
	default:
		throw eBadVersion();
	}

	return NULL;
}





cofmsg::cofmsg(size_t size) :
		memarea(new cmemory(size))
{
	ofh_header = (struct openflow::ofp_header*)soframe();
	ofh_header->length = htobe16(size);
}



cofmsg::cofmsg(cmemory *memarea) :
		memarea(memarea)
{
	if (0 == memarea) {
		throw eInval();
	}
	ofh_header = (struct openflow::ofp_header*)soframe();
}



cofmsg::cofmsg(cofmsg const& p) :
		memarea(0)
{
	*this = p;
}



cofmsg::~cofmsg()
{
	if (0 != memarea) {
		delete memarea;
	}
}



cofmsg&
cofmsg::operator=(const cofmsg &p)
{
	if (this == &p)
		return *this;

	if (memarea)
		delete memarea;

	memarea 	= new cmemory(*(p.memarea));
	ofh_header 	= (struct openflow::ofp_header*)(soframe());

	return *this;
}



void
cofmsg::reset()
{
	if (0 != memarea) {
		memarea->clear();
	}
}



size_t
cofmsg::length() const
{
	return framelen();
}



void
cofmsg::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < framelen())
		throw eInval();

	memcpy(buf, soframe(), framelen());
}



void
cofmsg::unpack(uint8_t *buf, size_t buflen)
{
	if (0 == memarea) {
		memarea = new cmemory(buflen);
	} else {
		memarea->resize(buflen);
		memarea->clear();
	}
	memarea->assign(buf, buflen);

	ofh_ofhu.ofhu_generic = soframe();

	validate();
}



void
cofmsg::validate()
{
	if (framelen() < sizeof(struct openflow::ofp_header))
		throw eBadSyntaxTooShort();

	ofh_header = (struct openflow::ofp_header*)soframe();

	if (get_length() > framelen())
		throw eBadSyntaxTooShort();
}



uint8_t*
cofmsg::resize(size_t len)
{
	memarea->resize(len);
	return (ofh_generic = soframe());
}



uint32_t
cofmsg::get_xid() const
{
	if (0 == ofh_header)
		throw eInval();
	return be32toh(ofh_header->xid);
}



void
cofmsg::set_xid(uint32_t xid)
{
	if (0 == ofh_header)
		throw eInval();
	ofh_header->xid = htobe32(xid);
}



uint8_t
cofmsg::get_version() const
{
	if (0 == ofh_header)
		throw eInval();
	return ofh_header->version;
}



void
cofmsg::set_version(uint8_t version)
{
	if (0 == ofh_header)
		throw eInval();
	ofh_header->version = version;
}



uint16_t
cofmsg::get_length() const
{
	if (0 == ofh_header)
		throw eInval();
	return be16toh(ofh_header->length);
}



void
cofmsg::set_length(uint16_t len)
{
	if (0 == ofh_header)
		throw eInval();
	ofh_header->length = htobe16(len);
}



uint8_t
cofmsg::get_type() const
{
	if (0 == ofh_header)
		throw eInval();
	return ofh_header->type;
}



void
cofmsg::set_type(uint8_t type)
{
	if (0 == ofh_header)
		throw eInval();
	ofh_header->type = type;
}

