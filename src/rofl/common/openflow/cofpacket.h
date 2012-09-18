/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFPACKET_H
#define COFPACKET_H

#include <set>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif
#include <strings.h>
#include <arpa/inet.h>

#include <endian.h>
#ifndef htobe16
#include "../endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "../cerror.h"
#include "../cvastring.h"
#include "../fframe.h"
#include "../cpacket.h"
#include "rofl/platform/unix/csyslog.h"

#include "openflow12.h"
#include "cofmatch.h"
#include "cofinlist.h"
#include "cofaclist.h"
#include "cofbclist.h"
#include "cofportlist.h"

// forward declarations
class cofbase;

class eOFpacketBase : public cerror {};
class eOFpacketInval : public eOFpacketBase {};
class eOFpacketNoData : public eOFpacketBase {};
class eOFpacketHeaderInval : public eOFpacketBase {}; // invalid header



/** cofpacket
 * - contains a packet from OpenFlow protocol
 * - provides container for validating an OpenFlow packet
 * - stores during validation actions, matches, instructions, buckets, ofports
 *   in appropriate containers
 * - stores pointer to cofbase entity the packet was received from
 *   (either cfwdelem or cofrpc)
 */
class cofpacket :
	public csyslog
{
private: // static

	static std::set<cofpacket*> cofpacket_list; //< list of allocated cofpacket instances
	static std::string pinfo; //< information string for cofpacket::cofpacket_list

public:

	/** return information string about allocated cofpacket instances
	 */
	static
	const char*
	packet_info();

	typedef struct {
		ofp_type type;
		char desc[64];
	} typedesc_t;


private: // data structures

	size_t 			stored;				// already stored bytes

protected: // data structures

	cmemory 		memarea;			// OpenFlow packet
	std::string 	info;				// info string for method c_str()

public: // data structures

	cofbase 		*entity; 			// source entity that emitted this packet
	cofmatch 		match; 				// ofp_match structure
	cofaclist 		actions; 			// list of actions (for Packet-Out messages)
	cofinlist 		instructions;		// list of instructions
	cofbclist 		buckets;			// list of buckets
	cofportlist		ports;				// list of ports (for Features-Reply messages)
	cmemory			body;				// body of OF packet, e.g. data, experimental body, stats body, etc.
	cpacket			packet;				// valid for Packet-In and Packet-Out, empty otherwise

	int switch_features_num_ports; 		// valid only, if type == FEATURES-REPLY

	union {
		struct ofp_header				  		*ofhu_hdr;
		struct ofp_switch_features 				*ofhu_sfhdr;
		struct ofp_switch_config 				*ofhu_schdr;
		struct ofp_flow_mod 					*ofhu_fmhdr;
		struct ofp_port_mod 					*ofhu_pmhdr;
		struct ofp_queue_get_config_request 	*ofhu_qgcrqhdr;
		struct ofp_queue_get_config_reply   	*ofhu_qgcrphdr;
		struct ofp_stats_request 				*ofhu_srqhdr;
		struct ofp_stats_reply   				*ofhu_srphdr;
		struct ofp_packet_out 					*ofhu_pohdr;
		struct ofp_packet_in  					*ofhu_pihdr;
		struct ofp_flow_removed 				*ofhu_frhdr;
		struct ofp_port_status 					*ofhu_pshdr;
		struct ofp_error_msg 					*ofhu_emhdr;
		struct ofp_experimenter_header			*ofhu_exphdr;
		struct ofp_group_mod					*ofhu_grphdr;
		struct ofp_table_mod					*ofhu_tblhdr;
		struct ofp_role_request					*ofhu_rolehdr;
	} ofh_ofhu;

#define ofh_header								ofh_ofhu.ofhu_hdr
#define ofh_switch_features 					ofh_ofhu.ofhu_sfhdr
#define ofh_switch_config 						ofh_ofhu.ofhu_schdr
#define ofh_flow_mod 							ofh_ofhu.ofhu_fmhdr
#define ofh_port_mod 							ofh_ofhu.ofhu_pmhdr
#define ofh_queue_get_config_request 			ofh_ofhu.ofhu_qgcrqhdr
#define ofh_queue_get_config_reply 				ofh_ofhu.ofhu_qgcrphdr
#define ofh_stats_request						ofh_ofhu.ofhu_srqhdr
#define ofh_stats_reply							ofh_ofhu.ofhu_srphdr
#define ofh_packet_out							ofh_ofhu.ofhu_pohdr
#define ofh_packet_in							ofh_ofhu.ofhu_pihdr
#define ofh_flow_removed						ofh_ofhu.ofhu_frhdr
#define ofh_port_status							ofh_ofhu.ofhu_pshdr
#define ofh_error_msg							ofh_ofhu.ofhu_emhdr
#define ofh_experimenter						ofh_ofhu.ofhu_exphdr
#define ofh_group_mod							ofh_ofhu.ofhu_grphdr
#define ofh_table_mod							ofh_ofhu.ofhu_tblhdr
#define ofh_role_request						ofh_ofhu.ofhu_rolehdr

#define OFP_FLOW_MOD_STATIC_HDR_LEN					48
#define OFP_FLOW_REMOVED_STATIC_HDR_LEN				48
#define OFP_PACKET_IN_STATIC_HDR_LEN				16
#define OFP_FLOW_STATS_REQUEST_STATIC_HDR_LEN		(sizeof(struct ofp_stats_request) + 32)
#define OFP_FLOW_STATS_REPLY_STATIC_BODY_LEN		48
#define OFP_GROUP_STATS_REPLY_STATIC_BODY_LEN		32
#define OFP_GROUP_DESC_STATS_REPLY_STATIC_BODY_LEN 8

	union {
		uint8_t									*ofbu_sreq;
		struct ofp_port_stats_request			*ofbu_psreq;
		struct ofp_port_stats_reply				*ofbu_psreply;
		struct ofp_flow_stats_request			*ofbu_fsreq;
		struct ofp_flow_stats					*ofbu_fsreply;
		struct ofp_aggregate_stats_request		*ofbu_asreq;
		struct ofp_aggregate_stats_reply		*ofbu_asreply;
		struct ofp_group_stats_request			*ofbu_gsreq;
		struct ofp_group_stats					*ofbu_gsreply;
	} ofb_ofbu;


#define ofb_stats_request						ofb_ofbu.ofbu_sreq
#define ofb_port_stats_request					ofb_ofbu.ofbu_psreq
#define ofb_port_stats_reply					ofb_ofbu.ofbu_psreply
#define ofb_flow_stats_request					ofb_ofbu.ofbu_fsreq
#define ofb_flow_stats_reply					ofb_ofbu.ofbu_fsreply
#define ofb_aggregate_stats_request				ofb_ofbu.ofbu_asreq
#define ofb_aggregate_stats_reply				ofb_ofbu.ofbu_asreply
#define ofb_group_stats_request					ofb_ofbu.ofbu_gsreq
#define ofb_group_stats_reply					ofb_ofbu.ofbu_gsreply



public:

	/** constructor
	 *
	 */
	cofpacket(
			size_t size = sizeof(struct ofp_header),
			size_t used = 0);


	/** copy constructor
	 *
	 */
	cofpacket(
			cofpacket const& p);


	/** destructor
	 *
	 */
	virtual
	~cofpacket();


	/** assignment operator
	 *
	 */
	cofpacket&
	operator=(
			cofpacket const& p);


	/** dump packet content
	 *
	 */
	virtual const char*
	c_str();


	/** reset packet content
	 *
	 */
	void
	reset();


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length();


	/** pack OFpacket content to specified buffer
	 *
	 */
	virtual void
	pack(
			uint8_t *buf = (uint8_t*)0,
			size_t buflen = 0) throw (eOFpacketInval);


	/** unpack OFpacket content from specified buffer
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf,
			size_t buflen);


	/** check for completeness of packet
	 */
	virtual bool
	complete() throw (eOFpacketInval);

	/** get number of bytes missing for complete packet
	 */
	virtual size_t need_bytes();
	/** parse packet and validate it
	 */
	bool is_valid();
	/** check, whether multiple packets were received from socket at once
	 */
	bool is_too_large();
	/**
	 */
	virtual void
	resize(size_t len);

	/**
	 *
	 */
	bool
	has_data();

	/**
	 *
	 */
	uint8_t*
	get_data() throw (eOFpacketNoData);

	/**
	 *
	 */
	size_t
	get_datalen() throw (eOFpacketNoData);

	/** start of frame
	 *
	 */
	uint8_t*
	soframe()
	{
		return memarea.somem();
	};

	/** frame length
	 *
	 */
	size_t
	framelen()
	{
		return memarea.memlen();
	};


	/**
	 *
	 */
	uint8_t*
	memptr()
	{
		return (soframe() + stored);
	};


	/**
	 *
	 */
	void
	stored_bytes(size_t __stored)
	{
		stored += __stored;
	};


	/** returns xid field in host byte order from header
	 *
	 */
	uint32_t
	get_xid();


	/** sets xid field in header
	 *
	 */
	void
	set_xid(uint32_t xid);


	/** returns the number of phy ports included in a FEATURES-Request
	 */
	int 
	switch_features_num_phy_ports()
	{
		return switch_features_num_ports; // valid only, if type == FEATURES-REPLY
	}

	/** returns the ofp_phy_port structure at index i
	 */
	struct ofp_port*
	switch_features_phy_port(int i)
	{
		if (i >= switch_features_num_ports)
			return NULL;
		return &(ofh_switch_features->ports[i]);
	}
	
protected:

	/** validate hello messages
	 */
	bool is_valid_hello_msg();

	/** validate echo requests
	 */
	bool is_valid_echo_request();

	/** validate echo replies
	 */
	bool is_valid_echo_reply();

	/** validate header only messages
	 */
	bool is_valid_hdr_only();

	/** validate error messages
	 */
	bool is_valid_error_msg();

	/** validate switch features messages
	 */
	bool is_valid_switch_features();

	/** validate switch config messages
	 */
	bool is_valid_switch_config();

	/** validate packet in messages
	 */ 
	bool is_valid_packet_in();

	/** validate packet out messages
	 */ 
	bool is_valid_packet_out();

	/** validate flow removed messages
	 */ 
	bool is_valid_flow_removed();

	/** validate port status messages
	 */ 
	bool is_valid_port_status();

	/** validate flow mod messages
	 */ 
	bool is_valid_flow_mod();

	/** validate port mod messages
	 */ 
	bool is_valid_port_mod();

	/** validate stats request messages
	 */ 
	bool is_valid_stats_request();

	/** validate stats reply messages
	 */ 
	bool is_valid_stats_reply();

	/** validate queue get config request messages
	 */ 
	bool is_valid_queue_get_config_request();

	/** validate queue get config reply messages
	 */ 
	bool is_valid_queue_get_config_reply();

	/** validate experimenter messages
	 */
	bool is_valid_experimenter_message();

	/** validate group mod messages
	 */
	bool is_valid_group_mod();

	/** validate table mod messages
	 */
	bool is_valid_table_mod();


public: // test method

	/**
	 *
	 */
	static void
	test();

private: // methods

	/** return description for ofp_type
	 */
	const char*
	type2desc(ofp_type type);

};


/**
 *
 */
class cofpacket_hello :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_hello(uint32_t xid = 0, uint8_t* data = 0, size_t datalen = 0) :
			cofpacket(	sizeof(struct ofp_header) + datalen,
						sizeof(struct ofp_header) + datalen)
		{
			body.assign(data, datalen);

			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_header) + sizeof(uint32_t));
			ofh_header->type 		= OFPT_HELLO;
			ofh_header->xid			= htobe32(xid);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_hello() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_header) + body.memlen());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), sizeof(struct ofp_header));

			memcpy(buf + sizeof(struct ofp_header), body.somem(), body.memlen());
		};
};


/**
 *
 */
class cofpacket_echo_request :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_echo_request(uint32_t xid = 0, uint8_t *data = (uint8_t*)0, size_t datalen = 0) :
			cofpacket(	sizeof(struct ofp_header) + datalen,
						sizeof(struct ofp_header) + datalen)
		{
			cofpacket::body.assign(data, datalen);

			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_header) + datalen);
			ofh_header->type 		= OFPT_ECHO_REQUEST;
			ofh_header->xid			= htobe32(xid);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_echo_request() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_header) + body.memlen());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), sizeof(struct ofp_header));

			memcpy(buf + sizeof(struct ofp_header), body.somem(), body.memlen());
		};
};


/**
 *
 */
class cofpacket_echo_reply :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_echo_reply(uint32_t xid = 0, uint8_t *data = (uint8_t*)0, size_t datalen = 0) :
			cofpacket(	sizeof(struct ofp_header) + datalen,
						sizeof(struct ofp_header) + datalen)
		{
			cofpacket::body.assign(data, datalen);

			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_header) + datalen);
			ofh_header->type 		= OFPT_ECHO_REPLY;
			ofh_header->xid			= htobe32(xid);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_echo_reply() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_header) + body.memlen());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), sizeof(struct ofp_header));

			memcpy(buf + sizeof(struct ofp_header), body.somem(), body.memlen());
		};
};


/** OFPT_FEATURES_REQUEST
 *
 */
class cofpacket_features_request :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_features_request(uint32_t xid = 0) :
			cofpacket(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_header));
			ofh_header->type 		= OFPT_FEATURES_REQUEST;
			ofh_header->xid			= htobe32(xid);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_features_request() {};
};


/** OFPT_FEATURES_REPLY
 *
 */
class cofpacket_features_reply :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_features_reply(
				uint32_t xid = 0,
				uint64_t dpid = 0,
				uint32_t n_buffers = 0,
				uint8_t n_tables = 0,
				uint32_t capabilities = 0) :
			cofpacket(	sizeof(struct ofp_switch_features),
						sizeof(struct ofp_switch_features))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_switch_features));
			ofh_header->type 		= OFPT_FEATURES_REPLY;
			ofh_header->xid			= htobe32(xid);

			ofh_switch_features->datapath_id 	= htobe64(dpid);
			ofh_switch_features->n_buffers 		= htobe32(n_buffers);
			ofh_switch_features->n_tables 		= n_tables;
			ofh_switch_features->capabilities 	= htobe32(capabilities);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_features_reply() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_switch_features) + ports.length());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), sizeof(struct ofp_switch_features));
			ports.pack((struct ofp_port*)(buf + sizeof(struct ofp_switch_features)), ports.length());
		};
};


/** OFPT_GET_CONFIG_REQUEST
 *
 */
class cofpacket_get_config_request :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_get_config_request(uint32_t xid = 0) :
			cofpacket(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_header));
			ofh_header->type 		= OFPT_GET_CONFIG_REQUEST;
			ofh_header->xid			= htobe32(xid);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_get_config_request() {};
};


/** OFPT_GET_CONFIG_REPLY
 *
 */
class cofpacket_get_config_reply :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_get_config_reply(
				uint32_t xid = 0,
				uint16_t flags = 0,
				uint16_t miss_send_len = 0) :
			cofpacket(	sizeof(struct ofp_switch_config),
						sizeof(struct ofp_switch_config))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_switch_config));
			ofh_header->type 		= OFPT_GET_CONFIG_REPLY;
			ofh_header->xid			= htobe32(xid);

			ofh_switch_config->flags			= htobe16(flags);
			ofh_switch_config->miss_send_len	= htobe16(miss_send_len);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_get_config_reply() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_switch_config));
		};
};


/** OFPT_SET_CONFIG
 *
 */
class cofpacket_set_config :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_set_config(
				uint32_t xid = 0,
				uint16_t flags = 0,
				uint16_t miss_send_len = 0) :
			cofpacket(	sizeof(struct ofp_switch_config),
						sizeof(struct ofp_switch_config))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_switch_config));
			ofh_header->type 		= OFPT_SET_CONFIG;
			ofh_header->xid			= htobe32(xid);

			ofh_switch_config->flags			= htobe16(flags);
			ofh_switch_config->miss_send_len	= htobe16(miss_send_len);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_set_config() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_switch_config));
		};
};


/** OFPT_BARRIER_REQUEST
 *
 */
class cofpacket_barrier_request :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_barrier_request(uint32_t xid = 0) :
			cofpacket(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_header));
			ofh_header->type 		= OFPT_BARRIER_REQUEST;
			ofh_header->xid			= htobe32(xid);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_barrier_request() {};
};


/** OFPT_BARRIER_REPLY
 *
 */
class cofpacket_barrier_reply :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_barrier_reply(uint32_t xid = 0) :
			cofpacket(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_header));
			ofh_header->type 		= OFPT_BARRIER_REPLY;
			ofh_header->xid			= htobe32(xid);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_barrier_reply() {};
};


/** OFPT_ERROR
 *
 */
class cofpacket_error :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_error(
				uint32_t xid = 0,
				uint16_t type = 0,
				uint16_t code = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0) :
			cofpacket(	sizeof(struct ofp_error_msg) + datalen,
						sizeof(struct ofp_error_msg) + datalen)
		{
			cofpacket::body.assign(data, datalen);

			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_header) + body.memlen());
			ofh_header->type 		= OFPT_ERROR;
			ofh_header->xid			= htobe32(xid);

			ofh_error_msg->type		= htobe16(type);
			ofh_error_msg->code		= htobe16(code);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_error() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_error_msg) + body.memlen());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), sizeof(struct ofp_error_msg));

			memcpy(buf + sizeof(struct ofp_error_msg), body.somem(), body.memlen());
		};
};


/** OFPT_FLOW_MOD
 *
 */
class cofpacket_flow_mod :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_flow_mod(
				uint32_t xid = 0,
				uint64_t cookie = 0,
				uint64_t cookie_mask = 0,
				uint8_t table_id = 0,
				uint8_t command = 0,
				uint16_t idle_timeout = 0,
				uint16_t hard_timeout = 0,
				uint16_t priority = 0,
				uint32_t buffer_id = 0,
				uint32_t out_port = 0,
				uint32_t out_group = 0,
				uint16_t flags = 0) :
			cofpacket(	OFP_FLOW_MOD_STATIC_HDR_LEN,
						OFP_FLOW_MOD_STATIC_HDR_LEN)
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(OFP_FLOW_MOD_STATIC_HDR_LEN);
			ofh_header->type 		= OFPT_FLOW_MOD;
			ofh_header->xid			= htobe32(xid);

			ofh_flow_mod->cookie			= htobe64(cookie);
			ofh_flow_mod->cookie_mask		= htobe64(cookie_mask);
			ofh_flow_mod->table_id			= table_id;
			ofh_flow_mod->command			= command;
			ofh_flow_mod->idle_timeout		= htobe16(idle_timeout);
			ofh_flow_mod->hard_timeout		= htobe16(hard_timeout);
			ofh_flow_mod->priority			= htobe16(priority);
			ofh_flow_mod->buffer_id			= htobe32(buffer_id);
			ofh_flow_mod->out_port			= htobe32(out_port);
			ofh_flow_mod->out_group			= htobe32(out_group);
			ofh_flow_mod->flags				= htobe16(flags);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_flow_mod() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (OFP_FLOW_MOD_STATIC_HDR_LEN + match.length() + instructions.length());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), OFP_FLOW_MOD_STATIC_HDR_LEN);

			match.pack((struct ofp_match*)
					(buf + OFP_FLOW_MOD_STATIC_HDR_LEN), match.length());

			instructions.pack((struct ofp_instruction*)
					(buf + OFP_FLOW_MOD_STATIC_HDR_LEN + match.length()), instructions.length());
		};
};


/** OFPT_FLOW_REMOVED
 *
 */
class cofpacket_flow_removed :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_flow_removed(
				uint32_t xid = 0,
				uint64_t cookie = 0,
				uint16_t priority = 0,
				uint8_t reason = 0,
				uint8_t table_id = 0,
				uint32_t duration_sec = 0,
				uint32_t duration_nsec = 0,
				uint16_t idle_timeout = 0,
				uint16_t hard_timeout = 0,
				uint64_t packet_count = 0,
				uint64_t byte_count = 0) :
			cofpacket(	OFP_FLOW_REMOVED_STATIC_HDR_LEN,
						OFP_FLOW_REMOVED_STATIC_HDR_LEN)
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(OFP_FLOW_REMOVED_STATIC_HDR_LEN);
			ofh_header->type 		= OFPT_FLOW_REMOVED;
			ofh_header->xid			= htobe32(xid);

			ofh_flow_removed->cookie			= htobe64(cookie);
			ofh_flow_removed->priority			= htobe16(priority);
			ofh_flow_removed->reason			= reason;
			ofh_flow_removed->table_id			= table_id;
			ofh_flow_removed->duration_sec		= htobe32(duration_sec);
			ofh_flow_removed->duration_nsec		= htobe32(duration_nsec);
			ofh_flow_removed->idle_timeout		= htobe16(idle_timeout);
			ofh_flow_removed->hard_timeout		= htobe16(hard_timeout);
			ofh_flow_removed->packet_count		= htobe64(packet_count);
			ofh_flow_removed->byte_count		= htobe64(byte_count);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_flow_removed() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (OFP_FLOW_REMOVED_STATIC_HDR_LEN + match.length());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), OFP_FLOW_REMOVED_STATIC_HDR_LEN);

			match.pack((struct ofp_match*)
					(buf + OFP_FLOW_MOD_STATIC_HDR_LEN), match.length());
		};
};


/** OFPT_PACKET_IN
 *
 */
class cofpacket_packet_in :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_packet_in(
				uint32_t xid = 0,
				uint32_t buffer_id = 0,
				uint16_t total_len = 0,
				uint8_t reason = 0,
				uint8_t table_id = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0) :
			cofpacket(	OFP_PACKET_IN_STATIC_HDR_LEN,
						OFP_PACKET_IN_STATIC_HDR_LEN)
		{
			//cofpacket::body.assign(data, datalen);
			cofpacket::packet.pack(data, datalen);

			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(OFP_PACKET_IN_STATIC_HDR_LEN + 2 + body.memlen());
			ofh_header->type 		= OFPT_PACKET_IN;
			ofh_header->xid			= htobe32(xid);

			ofh_packet_in->buffer_id			= htobe32(buffer_id);
			ofh_packet_in->total_len			= htobe16(total_len);
			ofh_packet_in->reason				= reason;
			ofh_packet_in->table_id				= table_id;
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_packet_in() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (OFP_PACKET_IN_STATIC_HDR_LEN + match.length() + 2 + body.memlen());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), OFP_PACKET_IN_STATIC_HDR_LEN);

			match.pack((struct ofp_match*)
					(buf + OFP_PACKET_IN_STATIC_HDR_LEN), match.length());

#if 0
			memcpy(buf + OFP_PACKET_IN_STATIC_HDR_LEN + match.length() + 2, body.somem(), body.memlen());
#endif
			memcpy(buf + OFP_PACKET_IN_STATIC_HDR_LEN + match.length() + 2, packet.soframe(), packet.framelen());

			/*
			 * Please note: +2 magic => provides proper alignment of IPv4 addresses in pin_data as defined by OF spec
			 */
		};
		/*
		 * TODO: overwrite methods get_data() and get_datalen() ???
		 */
};


/** OFPT_PACKET_OUT
 *
 */
class cofpacket_packet_out :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_packet_out(
				uint32_t xid = 0,
				uint32_t buffer_id = 0,
				uint32_t in_port = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0) :
			cofpacket(	sizeof(struct ofp_packet_out) + datalen,
						sizeof(struct ofp_packet_out) + datalen)
		{
			//cofpacket::body.assign(data, datalen);
			cofpacket::packet.pack(data, datalen);

			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_packet_out) + datalen);
			ofh_header->type 		= OFPT_PACKET_OUT;
			ofh_header->xid			= htobe32(xid);

			ofh_packet_out->buffer_id		= htobe32(buffer_id);
			ofh_packet_out->in_port			= htobe32(in_port);
			ofh_packet_out->actions_len		= htobe16(0); // filled in when method pack() is being called
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_packet_out() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_packet_out) + actions.length() + body.memlen());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length 				= htobe16(length());
			ofh_packet_out->actions_len 	= htobe16(actions.length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), sizeof(struct ofp_packet_out));

			actions.pack((struct ofp_action_header*)(buf + sizeof(struct ofp_packet_out)), actions.length());

#if 0
			memcpy(buf + sizeof(struct ofp_packet_out) + actions.length(), body.somem(), body.memlen());
#endif

			memcpy(buf + sizeof(struct ofp_packet_out) + actions.length(), packet.soframe(), packet.framelen());
		};
};



/** OFPT_PORT_STATUS
 *
 */
class cofpacket_port_status :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_port_status(
				uint32_t xid = 0,
				uint8_t reason = 0,
				struct ofp_port *desc = (struct ofp_port*)0,
				size_t desclen = 0) :
			cofpacket(	sizeof(struct ofp_port_status),
						sizeof(struct ofp_port_status))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_port_status));
			ofh_header->type 		= OFPT_PORT_STATUS;
			ofh_header->xid			= htobe32(xid);

			ofh_port_status->reason				= reason;

			if (desclen >= sizeof(struct ofp_port))
			{
				memcpy((uint8_t*)&(ofh_port_status->desc), desc, sizeof(struct ofp_port));
			}
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_port_status() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_port_status));
		};
};


/** OFPT_PORT_MOD
 *
 */
class cofpacket_port_mod :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_port_mod(
				uint32_t xid = 0,
				uint32_t port_no = 0,
				cmacaddr const& hwaddr = cmacaddr("00:00:00:00:00:00"),
				uint32_t config = 0,
				uint32_t mask = 0,
				uint32_t advertise = 0) :
			cofpacket(	sizeof(struct ofp_port_mod),
						sizeof(struct ofp_port_mod))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_port_mod));
			ofh_header->type 		= OFPT_PORT_MOD;
			ofh_header->xid			= htobe32(xid);

			ofh_port_mod->port_no			= htobe32(port_no);
			ofh_port_mod->config			= htobe32(config);
			ofh_port_mod->mask				= htobe32(mask);
			ofh_port_mod->advertise			= htobe32(advertise);

			memcpy(ofh_port_mod->hw_addr, hwaddr.somem(), OFP_ETH_ALEN);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_port_mod() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_port_mod));
		};
};




/** OFPT_GROUP_MOD
 *
 */
class cofpacket_group_mod :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_group_mod(
				uint32_t xid = 0,
				uint16_t command = 0,
				uint8_t  type = 0,
				uint32_t group_id = 0) :
			cofpacket(	sizeof(struct ofp_group_mod),
						sizeof(struct ofp_group_mod))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_group_mod));
			ofh_header->type 		= OFPT_GROUP_MOD;
			ofh_header->xid			= htobe32(xid);

			ofh_group_mod->command		= htobe16(command);
			ofh_group_mod->type			= type;
			ofh_group_mod->group_id		= htobe32(group_id);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_group_mod() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_group_mod) + buckets.length());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), sizeof(struct ofp_group_mod));

			buckets.pack((struct ofp_bucket*)(buf + sizeof(struct ofp_group_mod)), buckets.length());
		};
};



/** OFPT_TABLE_MOD
 *
 */
class cofpacket_table_mod :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_table_mod(
				uint32_t xid = 0,
				uint8_t  table_id = 0,
				uint32_t config = 0) :
			cofpacket(	sizeof(struct ofp_table_mod),
						sizeof(struct ofp_table_mod))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_table_mod));
			ofh_header->type 		= OFPT_TABLE_MOD;
			ofh_header->xid			= htobe32(xid);

			ofh_table_mod->table_id			= table_id;
			ofh_table_mod->config			= htobe32(config);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_table_mod() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_table_mod));
		};
};


/** OFPT_STATS_REQUEST
 *
 */
class cofpacket_stats_request :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_stats_request(
				uint32_t xid = 0,
				uint16_t type = 0,
				uint16_t flags = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0) :
			cofpacket(	sizeof(struct ofp_stats_request),
						sizeof(struct ofp_stats_request))
		{
			cofpacket::body.assign(data, datalen);

			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_stats_request) + body.memlen());
			ofh_header->type 		= OFPT_STATS_REQUEST;
			ofh_header->xid			= htobe32(xid);

			ofh_stats_request->type		= htobe16(type);
			ofh_stats_request->flags	= htobe16(flags);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_stats_request() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_stats_request) + body.memlen());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), sizeof(struct ofp_stats_request));

			memcpy(buf + sizeof(struct ofp_stats_request), body.somem(), body.memlen());
		};
};


/** OFPT_STATS_REPLY
 *
 */
class cofpacket_stats_reply :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_stats_reply(
				uint32_t xid = 0,
				uint16_t type = 0,
				uint16_t flags = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0) :
			cofpacket(	sizeof(struct ofp_stats_reply),
						sizeof(struct ofp_stats_reply))
		{
			cofpacket::body.assign(data, datalen);

			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_stats_reply) + body.memlen());
			ofh_header->type 		= OFPT_STATS_REPLY;
			ofh_header->xid			= htobe32(xid);

			ofh_stats_reply->type		= htobe16(type);
			ofh_stats_reply->flags		= htobe16(flags);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_stats_reply() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_stats_reply) + body.memlen());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), sizeof(struct ofp_stats_reply));

			memcpy(buf + sizeof(struct ofp_stats_reply), body.somem(), body.memlen());
		};
};


/** OFPT_QUEUE_GET_CONFIG_REQUEST
 *
 */
class cofpacket_queue_get_config_request :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_queue_get_config_request(
				uint32_t xid = 0,
				uint32_t port = 0) :
			cofpacket(	sizeof(struct ofp_queue_get_config_request),
						sizeof(struct ofp_queue_get_config_request))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_queue_get_config_request));
			ofh_header->type 		= OFPT_QUEUE_GET_CONFIG_REQUEST;
			ofh_header->xid			= htobe32(xid);

			ofh_queue_get_config_request->port		= htobe32(port);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_queue_get_config_request() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_queue_get_config_request));
		};
};


/** OFPT_QUEUE_GET_CONFIG_REPLY
 *
 */
class cofpacket_queue_get_config_reply :
	public cofpacket
{
		/*
		 * Note: we have no queue support yet. Thus, QUEUE-GET-CONFIG-REPLY contains always
		 * an empty queues array!
		 */
public:
		/** constructor
		 *
		 */
		cofpacket_queue_get_config_reply(
				uint32_t xid = 0,
				uint32_t port = 0) :
			cofpacket(	sizeof(struct ofp_queue_get_config_reply),
						sizeof(struct ofp_queue_get_config_reply))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_queue_get_config_reply));
			ofh_header->type 		= OFPT_QUEUE_GET_CONFIG_REPLY;
			ofh_header->xid			= htobe32(xid);

			ofh_queue_get_config_reply->port		= htobe32(port);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_queue_get_config_reply() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_queue_get_config_reply));
		};
};


/** OFPT_EXPERIMENTER
 *
 */
class cofpacket_experimenter :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_experimenter(
				uint32_t xid = 0,
				uint32_t experimenter = 0,
				uint32_t exp_type = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0) :
			cofpacket(	sizeof(struct ofp_experimenter_header) + datalen,
						sizeof(struct ofp_experimenter_header) + datalen)
		{
			cofpacket::body.assign(data, datalen);

			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_experimenter_header) + body.memlen());
			ofh_header->type 		= OFPT_EXPERIMENTER;
			ofh_header->xid			= htobe32(xid);


			ofh_experimenter->experimenter		= htobe32(experimenter);
			ofh_experimenter->exp_type			= htobe32(exp_type);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_experimenter() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_experimenter_header) + body.memlen());
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length = htobe16(length());

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			memcpy(buf, memarea.somem(), sizeof(struct ofp_experimenter_header));

			memcpy(buf + sizeof(struct ofp_experimenter_header), body.somem(), body.memlen());
		};
};


/** OFPT_ROLE_REQUEST
 *
 */
class cofpacket_role_request :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_role_request(
				uint32_t xid = 0,
				uint32_t role = 0,
				uint64_t generation_id = 0) :
			cofpacket(	sizeof(struct ofp_role_request),
						sizeof(struct ofp_role_request))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_queue_get_config_reply));
			ofh_header->type 		= OFPT_ROLE_REQUEST;
			ofh_header->xid			= htobe32(xid);

			ofh_role_request->role				= htobe32(role);
			ofh_role_request->generation_id 	= htobe64(generation_id);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_role_request() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_role_request));
		};
};


/** OFPT_ROLE_REPLY
 *
 */
class cofpacket_role_reply :
	public cofpacket
{
public:
		/** constructor
		 *
		 */
		cofpacket_role_reply(
				uint32_t xid = 0,
				uint32_t role = 0,
				uint64_t generation_id = 0) :
			cofpacket(	sizeof(struct ofp_role_request),
						sizeof(struct ofp_role_request))
		{
			ofh_header->version 	= OFP_VERSION;
			ofh_header->length		= htobe16(sizeof(struct ofp_queue_get_config_reply));
			ofh_header->type 		= OFPT_ROLE_REPLY;
			ofh_header->xid			= htobe32(xid);

			ofh_role_request->role				= htobe32(role);
			ofh_role_request->generation_id 	= htobe64(generation_id);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_role_reply() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_role_request));
		};
};


#endif
