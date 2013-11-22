/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofmsg.h"

using namespace rofl;



cofmsg::typedesc_t typedesc_of10[] = {
	{ OFPT10_HELLO, 					"HELLO" },
	{ OFPT10_ERROR, 					"ERROR" },
	{ OFPT10_ECHO_REQUEST, 				"ECHO-REQUEST" },
	{ OFPT10_ECHO_REPLY, 				"ECHO-REPLY" },
	{ OFPT10_VENDOR, 					"VENDOR" },

	{ OFPT10_FEATURES_REQUEST, 			"FEATURES-REQUEST" },
	{ OFPT10_FEATURES_REPLY, 			"FEATURES-REPLY" },
	{ OFPT10_GET_CONFIG_REQUEST,	 	"GET-CONFIG-REQUEST" },
	{ OFPT10_GET_CONFIG_REPLY, 			"GET-CONFIG-REPLY" },
	{ OFPT10_SET_CONFIG, 				"SET-CONFIG" },

	{ OFPT10_PACKET_IN, 				"PACKET-IN" },
	{ OFPT10_FLOW_REMOVED, 				"FLOW-REMOVED" },
	{ OFPT10_PORT_STATUS, 				"PORT-STATUS" },

	{ OFPT10_PACKET_OUT, 				"PACKET-OUT" },
	{ OFPT10_FLOW_MOD, 					"FLOW-MOD" },
	{ OFPT10_PORT_MOD, 					"PORT-MOD" },

	{ OFPT10_STATS_REQUEST, 			"STATS-REQUEST" },
	{ OFPT10_STATS_REPLY, 				"STATS-REPLY" },

	{ OFPT10_BARRIER_REQUEST, 			"BARRIER-REQUEST" },
	{ OFPT10_BARRIER_REPLY, 			"BARRIER-REPLY" },

	{ OFPT10_QUEUE_GET_CONFIG_REQUEST, 	"QUEUE-GET-CONFIG-REQUEST" },
	{ OFPT10_QUEUE_GET_CONFIG_REPLY, 	"QUEUE-GET-CONFIG-REPLY" },
};



cofmsg::typedesc_t typedesc_of12[] = {
	{ OFPT12_HELLO, 					"HELLO" },
	{ OFPT12_ERROR, 					"ERROR" },
	{ OFPT12_ECHO_REQUEST, 				"ECHO-REQUEST" },
	{ OFPT12_ECHO_REPLY, 				"ECHO-REPLY" },
	{ OFPT12_EXPERIMENTER, 				"EXPERIMENTER" },

	{ OFPT12_FEATURES_REQUEST, 			"FEATURES-REQUEST" },
	{ OFPT12_FEATURES_REPLY, 			"FEATURES-REPLY" },
	{ OFPT12_GET_CONFIG_REQUEST, 		"GET-CONFIG-REQUEST" },
	{ OFPT12_GET_CONFIG_REPLY, 			"GET-CONFIG-REPLY" },
	{ OFPT12_SET_CONFIG, 				"SET-CONFIG" },

	{ OFPT12_PACKET_IN, 				"PACKET-IN" },
	{ OFPT12_FLOW_REMOVED, 				"FLOW-REMOVED" },
	{ OFPT12_PORT_STATUS, 				"PORT-STATUS" },

	{ OFPT12_PACKET_OUT, 				"PACKET-OUT" },
	{ OFPT12_FLOW_MOD, 					"FLOW-MOD" },
	{ OFPT12_GROUP_MOD, 				"GROUP-MOD" },
	{ OFPT12_PORT_MOD, 					"PORT-MOD" },
	{ OFPT12_TABLE_MOD, 				"TABLE-MOD" },

	{ OFPT12_STATS_REQUEST, 			"STATS-REQUEST" },
	{ OFPT12_STATS_REPLY, 				"STATS-REPLY" },

	{ OFPT12_BARRIER_REQUEST, 			"BARRIER-REQUEST" },
	{ OFPT12_BARRIER_REPLY, 			"BARRIER-REPLY" },

	{ OFPT12_QUEUE_GET_CONFIG_REQUEST, 	"QUEUE-GET-CONFIG-REQUEST" },
	{ OFPT12_QUEUE_GET_CONFIG_REPLY, 	"QUEUE-GET-CONFIG-REPLY" },

	{ OFPT12_ROLE_REQUEST, 				"ROLE-REQUEST" },
	{ OFPT12_ROLE_REPLY, 				"ROLE-REPLY" },
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
	case OFP10_VERSION: {
        for (int i = 0; i < (int)(sizeof(typedesc_of10) / sizeof(cofmsg::typedesc_t)); i++) {
			if (typedesc_of10[i].type == ptype) {
				return typedesc_of10[i].desc;
			}
        }
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
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
		memarea(new cmemory(size)),
		ofh_header(0)
{
	ofh_header = (struct ofp_header*)soframe();
	ofh_header->length = htobe16(size);
}



cofmsg::cofmsg(cmemory *memarea) :
		memarea(memarea),
		ofh_header(0)
{
	if (0 == memarea) {
		throw eInval();
	}
	ofh_header = (struct ofp_header*)soframe();
}



cofmsg::cofmsg(cofmsg const& p) :
		memarea(0),
		ofh_header(0)
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
	ofh_header 	= (struct ofp_header*)(soframe());

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

	validate();
}



void
cofmsg::validate()
{
	if (framelen() < sizeof(struct ofp_header))
		throw eBadSyntaxTooShort();

	ofh_header = (struct ofp_header*)soframe();

	if (get_length() > framelen())
		throw eBadSyntaxTooShort();
}



void
cofmsg::resize(size_t len)
{
	memarea->resize(len);
	ofh_header = (struct ofp_header*)soframe();
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

