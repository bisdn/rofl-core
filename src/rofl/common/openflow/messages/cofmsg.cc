/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cofmsg.h"

using namespace rofl;



cofmsg::typedesc_t typedesc[] = {
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

	{ OFPT_ROLE_REQUEST, "ROLE-REQUEST" },
	{ OFPT_ROLE_REPLY, "ROLE-REPLY" },
};


/*static*/std::set<cofmsg*> cofmsg::cofpacket_list;
/*static*/std::string cofmsg::pinfo;

/*static*/const char*
cofmsg::packet_info()
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
						cofmsg::type2desc((enum ofp_type)it->first),
						it->second));
	}

	return pinfo.c_str();
}



const char*
cofmsg::type2desc(ofp_type ptype)
{
        for (int i = 0; i < (int)(sizeof(typedesc) / sizeof(cofmsg::typedesc_t)); i++)
        {
                if (typedesc[i].type == ptype) {
                        return typedesc[i].desc;
                }
        }
        return NULL;
}





cofmsg::cofmsg(size_t size) :
		destroy_mem_during_destruction(true),
		memarea(new cmemory(size)),
		ofh_header(0)
{
	ofh_header = (struct ofp_header*)soframe();
	ofh_header->length = htobe16(size);
}



cofmsg::cofmsg(cmemory *memarea) :
		destroy_mem_during_destruction(false),
		memarea(memarea),
		ofh_header(0)
{
	if (0 == memarea) {
		throw eInval();
	}

	validate();

	destroy_mem_during_destruction = true;
}



cofmsg::cofmsg(cofmsg const& p) :
		destroy_mem_during_destruction(true),
		memarea(0),
		ofh_header(0)
{
	*this = p;
}



cofmsg::~cofmsg()
{
	if ((destroy_mem_during_destruction) && (0 != memarea)) {
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



const char*
cofmsg::c_str()
{
	return (memarea == 0) ? info.c_str() : memarea->c_str();
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

