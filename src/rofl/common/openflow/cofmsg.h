/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFMSG_H
#define COFMSG_H 1

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

#include "openflow.h"
#include "cofmatch.h"
#include "cofinlist.h"
#include "cofaclist.h"
#include "cofbclist.h"
#include "cofportlist.h"
#include "cofport.h"
#include "cofdescstats.h"
#include "cofflowstats.h"
#include "cofaggrstats.h"
#include "coftablestats.h"
#include "cofportstats.h"
#include "cofqueuestats.h"
#include "cofgroupstats.h"
#include "cofgroupdescstats.h"

// forward declarations
class cofbase;

namespace rofl
{

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
class cofmsg :
	public csyslog
{
public: // static

	static std::set<cofmsg*> cofpacket_list; //< list of allocated cofpacket instances

private: // static

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

	std::string 	 	 info;				// info string for method c_str()

protected: // data structures

	struct ofp_header	*ofh_header;		// generic OpenFlow header
	cmemory 			*memarea;			// OpenFlow packet received from socket

public:

	/** constructor
	 *
	 */
	cofmsg(
			size_t size = sizeof(struct ofp_header));


	/**
	 *
	 */
	cofmsg(
			cmemory *memarea);


	/** copy constructor
	 *
	 */
	cofmsg(
			cofmsg const& p);


	/** destructor
	 *
	 */
	virtual
	~cofmsg();


	/** assignment operator
	 *
	 */
	cofmsg&
	operator=(
			cofmsg const& p);


	/** dump packet content
	 *
	 */
	virtual const char*
	c_str();


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/** pack OFpacket content to specified buffer
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/** unpack OFpacket content from specified buffer
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();


	/**
	 */
	virtual void
	resize(size_t len);


	/** start of frame
	 *
	 */
	uint8_t*
	soframe() const { return memarea->somem(); };


	/** frame length
	 *
	 */
	size_t
	framelen() const { return memarea->memlen(); };


	/** start of frame
	 *
	 */
	uint8_t*
	sobody() const { return (soframe() + sizeof(struct ofp_header)); };


	/** frame length
	 *
	 */
	size_t
	bodylen() const { return (framelen() - sizeof(struct ofp_header)); };


public:




	/**
	 *
	 */
	uint8_t
	get_version() const;


	/**
	 *
	 */
	void
	set_version(uint8_t versiob);


	/**
	 *
	 */
	uint16_t
	get_length() const;


	/**
	 *
	 */
	void
	set_length(uint16_t len);


	/**
	 *
	 */
	uint8_t
	get_type() const;


	/**
	 *
	 */
	void
	set_type(uint8_t type);


	/** returns xid field in host byte order from header
	 *
	 */
	uint32_t
	get_xid() const;


	/** sets xid field in header
	 *
	 */
	void
	set_xid(uint32_t xid);


private: // methods

	/** return description for ofp_type
	 */
	static const char*
	type2desc(ofp_type type);

};




#if 0
/** cofpacket
 * - contains a packet from OpenFlow protocol
 * - provides container for validating an OpenFlow packet
 * - stores during validation actions, matches, instructions, buckets, ofports
 *   in appropriate containers
 * - stores pointer to cofbase entity the packet was received from
 *   (either cfwdelem or cofrpc)
 */
class cofmsg :
	public csyslog
{
public: // static

	static std::set<cofmsg*> cofpacket_list; //< list of allocated cofpacket instances

private: // static

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

protected: // data structures

	size_t 			stored;				// already stored bytes
	cmemory 		memarea;			// OpenFlow packet
	std::string 	info;				// info string for method c_str()

public: // data structures

	cofbase 				*entity; 			// source entity that emitted this packet
	cofmatch 				match; 				// ofp_match structure
	cofaclist 				actions; 			// list of actions (for Packet-Out messages)
	cofinlist 				instructions;		// list of instructions
	cofbclist 				buckets;			// list of buckets
	cofportlist				ports;				// list of ports (for Features-Reply messages)
	cofport					port;				// a single port (for Port-Status messages)
	cmemory					body;				// body of OF packet, e.g. data, experimental body, stats body, etc.
	cpacket					packet;				// valid for Packet-In and Packet-Out, empty otherwise
	cofdesc_stats_reply 	desc_stats_reply;	// description statistics
	cofflow_stats_request 	flow_stats_request;	// flow statistics request
	cofflow_stats_reply 	flow_stats_reply;	// flow statistics reply
	cofaggr_stats_request	aggr_stats_request;	// aggregate statistics request
	cofaggr_stats_reply		aggr_stats_reply;	// aggregate statistics reply
	coftable_stats_reply	table_stats_reply;	// table statistics reply
	cofport_stats_request	port_stats_request;	// port statistics request
	cofport_stats_reply		port_stats_reply;	// port statistics reply
	cofqueue_stats_request	queue_stats_request;// queue statistics request
	cofqueue_stats_reply	queue_stats_reply;	// queue statistics reply
	cofgroup_stats_request	group_stats_request;// group statistics request
	cofgroup_stats_reply	group_stats_reply;	// group statistics reply
	cofgroup_desc_stats_reply 	group_desc_stats_reply;		// group description statistics reply

	//int switch_features_num_ports; 		// valid only, if type == FEATURES-REPLY

	union {
		struct ofp_header				  		*ofhu_hdr;
		struct ofp13_hello						*of13hu_hello;
		struct ofp10_switch_features 			*of10hu_sfhdr;
		struct ofp12_switch_features 			*of12hu_sfhdr;
		struct ofp13_switch_features			*of13hu_sfhdr;
		struct ofp10_switch_config 				*of10hu_schdr;
		struct ofp12_switch_config 				*of12hu_schdr;
		struct ofp13_switch_config 				*of13hu_schdr;
		struct ofp10_flow_mod 					*of10hu_fmhdr;
		struct ofp12_flow_mod 					*of12hu_fmhdr;
		struct ofp13_flow_mod 					*of13hu_fmhdr;
		struct ofp10_port_mod 					*of10hu_pmhdr;
		struct ofp12_port_mod 					*of12hu_pmhdr;
		struct ofp13_port_mod 					*of13hu_pmhdr;
		struct ofp10_queue_get_config_request 	*of10hu_qgcrqhdr;
		struct ofp12_queue_get_config_request 	*of12hu_qgcrqhdr;
		struct ofp13_queue_get_config_request 	*of13hu_qgcrqhdr;
		struct ofp10_queue_get_config_reply   	*of10hu_qgcrphdr;
		struct ofp12_queue_get_config_reply   	*of12hu_qgcrphdr;
		struct ofp13_queue_get_config_reply   	*of13hu_qgcrphdr;
		struct ofp10_stats_request 				*of10hu_srqhdr;
		struct ofp12_stats_request 				*of12hu_srqhdr;
		struct ofp10_stats_reply   				*of10hu_srphdr;
		struct ofp12_stats_reply   				*of12hu_srphdr;
		struct ofp10_packet_out 				*of10hu_pohdr;
		struct ofp12_packet_out 				*of12hu_pohdr;
		struct ofp13_packet_out 				*of13hu_pohdr;
		struct ofp10_packet_in 					*of10hu_pihdr;
		struct ofp12_packet_in 					*of12hu_pihdr;
		struct ofp13_packet_in 					*of13hu_pihdr;
		struct ofp10_flow_removed 				*of10hu_frhdr;
		struct ofp12_flow_removed 				*of12hu_frhdr;
		struct ofp13_flow_removed 				*of13hu_frhdr;
		struct ofp10_port_status 				*of10hu_pshdr;
		struct ofp12_port_status 				*of12hu_pshdr;
		struct ofp13_port_status 				*of13hu_pshdr;
		struct ofp_error_msg 					*ofhu_emhdr;
		struct ofp_experimenter_header			*ofhu_exphdr;
		struct ofp_group_mod					*ofhu_grphdr;
		struct ofp_table_mod					*ofhu_tblhdr;
		struct ofp_role_request					*ofhu_rolehdr;
		struct ofp13_meter_mod					*of13hu_methdr;
	} ofh_ofhu;

#define ofh_header								ofh_ofhu.ofhu_hdr
#define ofh13_hello								ofh_ofhu.of13hu_hello
#define of10h_switch_features 					ofh_ofhu.of10hu_sfhdr
#define of12h_switch_features 					ofh_ofhu.of12hu_sfhdr
#define of13h_switch_features 					ofh_ofhu.of13hu_sfhdr
#define of10h_switch_config 					ofh_ofhu.of10hu_schdr
#define of12h_switch_config 					ofh_ofhu.of12hu_schdr
#define of13h_switch_config 					ofh_ofhu.of13hu_schdr
#define of10h_flow_mod 							ofh_ofhu.of10hu_fmhdr
#define of12h_flow_mod 							ofh_ofhu.of12hu_fmhdr
#define of13h_flow_mod 							ofh_ofhu.of13hu_fmhdr
#define of10h_port_mod 							ofh_ofhu.of10hu_pmhdr
#define of12h_port_mod 							ofh_ofhu.of12hu_pmhdr
#define of13h_port_mod 							ofh_ofhu.of13hu_pmhdr
#define of10h_queue_get_config_request 			ofh_ofhu.of10hu_qgcrqhdr
#define of12h_queue_get_config_request 			ofh_ofhu.of12hu_qgcrqhdr
#define of13h_queue_get_config_request 			ofh_ofhu.of13hu_qgcrqhdr
#define of10h_queue_get_config_reply 			ofh_ofhu.of10hu_qgcrphdr
#define of12h_queue_get_config_reply 			ofh_ofhu.of12hu_qgcrphdr
#define of13h_queue_get_config_reply 			ofh_ofhu.of13hu_qgcrphdr
#define of10h_stats_request						ofh_ofhu.of10hu_srqhdr
#define of12h_stats_request						ofh_ofhu.of12hu_srqhdr
#define of10h_stats_reply						ofh_ofhu.of10hu_srphdr
#define of12h_stats_reply						ofh_ofhu.of12hu_srphdr
#define of10h_packet_out						ofh_ofhu.of10hu_pohdr
#define of12h_packet_out						ofh_ofhu.of12hu_pohdr
#define of13h_packet_out						ofh_ofhu.of13hu_pohdr
#define of10h_packet_in							ofh_ofhu.of10hu_pihdr
#define of12h_packet_in							ofh_ofhu.of12hu_pihdr
#define of13h_packet_in							ofh_ofhu.of13hu_pihdr
#define of10h_flow_rmvd							ofh_ofhu.of10hu_frhdr
#define of12h_flow_rmvd							ofh_ofhu.of12hu_frhdr
#define of13h_flow_rmvd							ofh_ofhu.of13hu_frhdr
#define of10h_port_status						ofh_ofhu.of10hu_pshdr
#define of12h_port_status						ofh_ofhu.of12hu_pshdr
#define of13h_port_status						ofh_ofhu.of13hu_pshdr
#define ofh_error_msg							ofh_ofhu.ofhu_emhdr
#define ofh_experimenter						ofh_ofhu.ofhu_exphdr
#define ofh_group_mod							ofh_ofhu.ofhu_grphdr
#define ofh_table_mod							ofh_ofhu.ofhu_tblhdr
#define ofh_role_request						ofh_ofhu.ofhu_rolehdr
#define ofh13_meter_mod							ofh_ofhu.of13hu_methdr

#define OFP10_FLOW_MOD_STATIC_HDR_LEN				72
#define OFP12_FLOW_MOD_STATIC_HDR_LEN				48
#define OFP13_FLOW_MOD_STATIC_HDR_LEN				48
#define OFP10_FLOW_REMOVED_STATIC_HDR_LEN			88
#define OFP12_FLOW_REMOVED_STATIC_HDR_LEN			48
#define OFP13_FLOW_REMOVED_STATIC_HDR_LEN			48
#define OFP10_PACKET_IN_STATIC_HDR_LEN				20
#define OFP12_PACKET_IN_STATIC_HDR_LEN				16
#define OFP13_PACKET_IN_STATIC_HDR_LEN				24
#define OFP_FLOW_STATS_REQUEST_STATIC_HDR_LEN		(sizeof(struct ofp_stats_request) + 32)
#define OFP_FLOW_STATS_REPLY_STATIC_BODY_LEN		48
#define OFP_AGGR_STATS_REQUEST_STATIC_HDR_LEN		32	// without struct ofp_match
#define OFP_GROUP_STATS_REPLY_STATIC_BODY_LEN		32
#define OFP_GROUP_DESC_STATS_REPLY_STATIC_BODY_LEN 	8
#define OFP13_METER_MOD_STATIC_HDR_LEN				16

#if 0
	union {
		uint8_t									*ofbu_sreq;
		struct ofp12_port_stats_request			*ofbu_psreq;
		struct ofp12_port_stats_reply			*ofbu_psreply;
		struct ofp12_flow_stats_request			*ofbu_fsreq;
		struct ofp12_flow_stats					*ofbu_fsreply;
		struct ofp12_aggregate_stats_request	*ofbu_asreq;
		struct ofp12_aggregate_stats_reply		*ofbu_asreply;
		struct ofp12_group_stats_request		*ofbu_gsreq;
		struct ofp12_group_stats				*ofbu_gsreply;
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
#endif


public:

	/** constructor
	 *
	 */
	cofmsg(
			size_t size = sizeof(struct ofp_header),
			size_t used = 0);


	/** copy constructor
	 *
	 */
	cofmsg(
			cofmsg const& p);


	/** destructor
	 *
	 */
	virtual
	~cofmsg();


	/** assignment operator
	 *
	 */
	cofmsg&
	operator=(
			cofmsg const& p);


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
	soframe() const { return memarea.somem(); };

	/** frame length
	 *
	 */
	size_t
	framelen() const { return memarea.memlen(); };


	/**
	 *
	 */
	uint8_t*
	memptr() { return (soframe() + stored); };


	/**
	 *
	 */
	void
	stored_bytes(size_t __stored) { stored += __stored; };


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


#if 0
	/** returns the number of phy ports included in a FEATURES-Request
	 */
	int 
	switch_features_num_phy_ports()
	{
		return switch_features_num_ports; // valid only, if type == FEATURES-REPLY
	}
#endif

#if 0
	/** returns the ofp_phy_port structure at index i
	 */
	struct ofp_port*
	switch_features_phy_port(int i)
	{
		if (i >= switch_features_num_ports)
			return NULL;
		return &(of12h_switch_features->ports[i]);
	}
#endif
	
	/**
	 *
	 */
	uint8_t
	get_version() const
	{
		return ((0 != ofh_header) ? ofh_header->version : 0);
	};


	/**
	 *
	 */
	uint16_t
	get_length() const
	{
		return ((0 != ofh_header) ? be16toh(ofh_header->length) : 0);
	};


	/**
	 *
	 */
	virtual cofmatch&
	get_match() { return match; };


	/**
	 *
	 */
	virtual cofaclist&
	get_actions() { return actions; };


	/**
	 *
	 */
	virtual cofinlist&
	get_instructions() { return instructions; };


	/**
	 *
	 */
	virtual cofbclist&
	get_buckets() { return buckets; };


	/**
	 *
	 */
	virtual cofportlist&
	get_port_list() { return ports; };


	/**
	 *
	 */
	virtual cofport&
	get_port_desc() { return port; };


	/**
	 *
	 */
	virtual cmemory&
	get_body() { return body; };


	/**
	 *
	 */
	virtual cpacket&
	get_packet() { return packet; };


	/**
	 *
	 */
	virtual cofdesc_stats_reply&
	get_desc_stats_reply() { return desc_stats_reply; };


	/**
	 *
	 */
	virtual cofflow_stats_request&
	get_flow_stats_request() { return flow_stats_request; };


	/**
	 *
	 */
	virtual cofflow_stats_reply&
	get_flow_stats_reply() { return flow_stats_reply; };


	/**
	 *
	 */
	virtual cofaggr_stats_request&
	get_aggr_stats_request() { return aggr_stats_request; };


	/**
	 *
	 */
	virtual cofaggr_stats_reply&
	get_aggr_stats_reply() { return aggr_stats_reply; };


	/**
	 *
	 */
	virtual coftable_stats_reply&
	get_table_stats_reply() { return table_stats_reply; };


	/**
	 *
	 */
	virtual cofport_stats_request&
	get_port_stats_request() { return port_stats_request; };


	/**
	 *
	 */
	virtual cofport_stats_reply&
	get_port_stats_reply() { return port_stats_reply; };


	/**
	 *
	 */
	virtual cofqueue_stats_request&
	get_queue_stats_request() { return queue_stats_request; };


	/**
	 *
	 */
	virtual cofqueue_stats_reply&
	get_queue_stats_reply() { return queue_stats_reply; };


	/**
	 *
	 */
	virtual cofgroup_stats_request&
	get_group_stats_request() { return group_stats_request; };


	/**
	 *
	 */
	virtual cofgroup_stats_reply&
	get_group_stats_reply() { return group_stats_reply; };


	/**
	 *
	 */
	virtual cofgroup_desc_stats_reply&
	get_group_desc_stats_reply() { return group_desc_stats_reply; };


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

	/** validate meter mod messages
	 */
	bool is_valid_meter_mod();

public: // test method

	/**
	 *
	 */
	static void
	test();

private: // methods

	/** return description for ofp_type
	 */
	static const char*
	type2desc(ofp_type type);

};
#endif



#if 0
/**
 *
 */
class cofpacket_echo_request :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_echo_request(uint8_t of_version = 0, uint32_t xid = 0, uint8_t *data = (uint8_t*)0, size_t datalen = 0) :
			cofmsg(	sizeof(struct ofp_header) + datalen,
						sizeof(struct ofp_header) + datalen)
		{
			cofmsg::body.assign(data, datalen);

			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp_header) + datalen);
			ofh_header->type 		= OFPT_ECHO_REQUEST;
			ofh_header->xid			= htobe32(xid);
		};
		/** constructor
		 *
		 */
		cofpacket_echo_request(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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

			if (body.memlen() > 0)
			{
				memcpy(buf + sizeof(struct ofp_header), body.somem(), body.memlen());
			}
		};
};


/**
 *
 */
class cofpacket_echo_reply :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_echo_reply(uint8_t of_version = 0, uint32_t xid = 0, uint8_t *data = (uint8_t*)0, size_t datalen = 0) :
			cofmsg(	sizeof(struct ofp_header) + datalen,
						sizeof(struct ofp_header) + datalen)
		{
			cofmsg::body.assign(data, datalen);

			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp_header) + datalen);
			ofh_header->type 		= OFPT_ECHO_REPLY;
			ofh_header->xid			= htobe32(xid);
		};
		/** constructor
		 *
		 */
		cofpacket_echo_reply(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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

			if (body.memlen() > 0)
			{
				memcpy(buf + sizeof(struct ofp_header), body.somem(), body.memlen());
			}
		};
};


/** OFPT_FEATURES_REQUEST
 *
 */
class cofpacket_features_request :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_features_request(uint8_t of_version = 0, uint32_t xid = 0) :
			cofmsg(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp_header));
			ofh_header->type 		= OFPT_FEATURES_REQUEST;
			ofh_header->xid			= htobe32(xid);
		};
		/** constructor
		 *
		 */
		cofpacket_features_request(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_features_reply(
				uint8_t  of_version = 0,
				uint32_t xid = 0,
				uint64_t dpid = 0,
				uint32_t n_buffers = 0,
				uint8_t  n_tables = 0,
				uint32_t capabilities = 0,
				uint8_t  of13_auxiliary_id = 0,  /*OF1.3*/
				uint32_t of10_actions_bitmap = 0 /*OF1.0*/) :
			cofmsg(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(0);
			ofh_header->type 		= OFPT_FEATURES_REPLY;
			ofh_header->xid			= htobe32(xid);

			switch (ofh_header->version) {
			case OFP10_VERSION: {
				ofh_header->length		= htobe16(sizeof(struct ofp10_switch_features));
				cofmsg::resize(sizeof(struct ofp10_switch_features));

				of10h_switch_features->datapath_id 		= htobe64(dpid);
				of10h_switch_features->n_buffers 		= htobe32(n_buffers);
				of10h_switch_features->n_tables 		= n_tables;
				of10h_switch_features->capabilities 	= htobe32(capabilities);
				of10h_switch_features->actions			= htobe32(of10_actions_bitmap);
			} break;
			case OFP12_VERSION: {
				ofh_header->length		= htobe16(sizeof(struct ofp12_switch_features));
				cofmsg::resize(sizeof(struct ofp12_switch_features));

				of12h_switch_features->datapath_id 		= htobe64(dpid);
				of12h_switch_features->n_buffers 		= htobe32(n_buffers);
				of12h_switch_features->n_tables 		= n_tables;
				of12h_switch_features->capabilities 	= htobe32(capabilities);
			} break;
			case OFP13_VERSION: {
				ofh_header->length		= htobe16(sizeof(struct ofp13_switch_features));
				cofmsg::resize(sizeof(struct ofp13_switch_features));

				of13h_switch_features->datapath_id 		= htobe64(dpid);
				of13h_switch_features->n_buffers 		= htobe32(n_buffers);
				of13h_switch_features->n_tables 		= n_tables;
				of13h_switch_features->auxiliary_id		= of13_auxiliary_id;
				of13h_switch_features->capabilities 	= htobe32(capabilities);
			} break;
			default:
				throw eBadVersion();
			}
		};
		/** constructor
		 *
		 */
		cofpacket_features_reply(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return (sizeof(struct ofp10_switch_features) + ports.length());
			} break;
			case OFP12_VERSION: {
				return (sizeof(struct ofp12_switch_features) + ports.length());
			} break;
			case OFP13_VERSION: {
				return (sizeof(struct ofp13_switch_features));
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
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

			switch (ofh_header->version) {
			case OFP10_VERSION: {
				memcpy(buf, memarea.somem(), sizeof(struct ofp10_switch_features));
				ports.pack((struct ofp10_port*)(buf + sizeof(struct ofp10_switch_features)), ports.length());
			} break;
			case OFP12_VERSION: {
				memcpy(buf, memarea.somem(), sizeof(struct ofp12_switch_features));
				ports.pack((struct ofp12_port*)(buf + sizeof(struct ofp12_switch_features)), ports.length());
			} break;
			case OFP13_VERSION: {
				memcpy(buf, memarea.somem(), sizeof(struct ofp13_switch_features));
			} break;
			default:
				throw eBadVersion();
			}
		};
		/**
		 *
		 */
		uint64_t
		get_dpid() const
		{
			switch (get_version()) {
			case OFP10_VERSION: {
				return be64toh(of10h_switch_features->datapath_id);
			} break;
			case OFP12_VERSION: {
				return be64toh(of12h_switch_features->datapath_id);
			} break;
			case OFP13_VERSION: {
				return be64toh(of13h_switch_features->datapath_id);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_n_buffers() const
		{
			switch (get_version()) {
			case OFP10_VERSION: {
				return be32toh(of10h_switch_features->n_buffers);
			} break;
			case OFP12_VERSION: {
				return be32toh(of12h_switch_features->n_buffers);
			} break;
			case OFP13_VERSION: {
				return be32toh(of13h_switch_features->n_buffers);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
		/**
		 *
		 */
		uint8_t
		get_n_tables() const
		{
			switch (get_version()) {
			case OFP10_VERSION: {
				return be32toh(of10h_switch_features->n_tables);
			} break;
			case OFP12_VERSION: {
				return be32toh(of12h_switch_features->n_tables);
			} break;
			case OFP13_VERSION: {
				return be32toh(of13h_switch_features->n_tables);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
		/** OF1.3
		 *
		 */
		uint8_t
		get_auxiliary_id() const
		{
			switch (get_version()) {
			case OFP13_VERSION: {
				return of13h_switch_features->auxiliary_id;
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_capabilities() const
		{
			switch (get_version()) {
			case OFP10_VERSION: {
				return be32toh(of10h_switch_features->capabilities);
			} break;
			case OFP12_VERSION: {
				return be32toh(of12h_switch_features->capabilities);
			} break;
			case OFP13_VERSION: {
				return be32toh(of13h_switch_features->capabilities);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
		/** OF1.0 only
		 *
		 */
		uint32_t
		get_actions_bitmap() const
		{
			switch (get_version()) {
			case OFP10_VERSION: {
				return be32toh(of10h_switch_features->actions);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
};


/** OFPT_GET_CONFIG_REQUEST
 *
 */
class cofpacket_get_config_request :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_get_config_request(uint8_t of_version = 0, uint32_t xid = 0) :
			cofmsg(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp_header));
			ofh_header->type 		= OFPT_GET_CONFIG_REQUEST;
			ofh_header->xid			= htobe32(xid);
		};
		/** constructor
		 *
		 */
		cofpacket_get_config_request(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_get_config_reply(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint16_t flags = 0,
				uint16_t miss_send_len = 0) :
			cofmsg(	sizeof(struct ofp10_switch_config),
						sizeof(struct ofp10_switch_config))
		{
			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp10_switch_config));
			ofh_header->type 		= OFPT_GET_CONFIG_REPLY;
			ofh_header->xid			= htobe32(xid);

			switch (of_version) {
			case OFP10_VERSION:
			case OFP12_VERSION:
			case OFP13_VERSION: {
				of10h_switch_config->flags			= htobe16(flags);
				of10h_switch_config->miss_send_len	= htobe16(miss_send_len);
			} break;
			default:
				throw eBadVersion();
			}
		};
		/** constructor
		 *
		 */
		cofpacket_get_config_reply(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
			return (sizeof(struct ofp10_switch_config));
		};
		/**
		 *
		 */
		uint16_t
		get_flags()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION:
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of10h_switch_config->flags); // no change since OF1.0
			} break;
			default:
				throw eBadVersion();
			}
		};
		/**
		 *
		 */
		uint16_t
		get_miss_send_len()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION:
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of10h_switch_config->miss_send_len); // no change since OF1.0
			} break;
			default:
				throw eBadVersion();
			}
		};
};


/** OFPT_SET_CONFIG
 *
 */
class cofpacket_set_config :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_set_config(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint16_t flags = 0,
				uint16_t miss_send_len = 0) :
			cofmsg(	sizeof(struct ofp10_switch_config),
						sizeof(struct ofp10_switch_config))
		{
			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp10_switch_config));
			ofh_header->type 		= OFPT_SET_CONFIG;
			ofh_header->xid			= htobe32(xid);

			switch (of_version) {
			case OFP10_VERSION:
			case OFP12_VERSION:
			case OFP13_VERSION: {
				of10h_switch_config->flags			= htobe16(flags);
				of10h_switch_config->miss_send_len	= htobe16(miss_send_len);
			} break;
			default:
				throw eBadVersion();
			}
		};
		/** constructor
		 *
		 */
		cofpacket_set_config(cofmsg *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
			switch (ofh_header->version) {
			case OFP10_VERSION:
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return (sizeof(struct ofp10_switch_config)); // no change since OF1.0
			} break;
			default:
				throw eBadVersion();
			}
		};
		/**
		 *
		 */
		uint16_t
		get_flags()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION:
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of10h_switch_config->flags); // no change since OF1.0
			} break;
			default:
				throw eBadVersion();
			}
		};
		/**
		 *
		 */
		uint16_t
		get_miss_send_len()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION:
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of12h_switch_config->miss_send_len); // no change since OF1.2
			} break;
			default:
				throw eBadVersion();
			}
		};
};


/** OFPT_BARRIER_REQUEST
 *
 */
class cofpacket_barrier_request :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_barrier_request(uint8_t of_version = 0, uint32_t xid = 0) :
			cofmsg(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp_header));
			ofh_header->type 		= OFPT_BARRIER_REQUEST;
			ofh_header->xid			= htobe32(xid);
		};
		/** constructor
		 *
		 */
		cofpacket_barrier_request(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_barrier_reply(uint8_t of_version = 0, uint32_t xid = 0) :
			cofmsg(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp_header));
			ofh_header->type 		= OFPT_BARRIER_REPLY;
			ofh_header->xid			= htobe32(xid);
		};
		/** constructor
		 *
		 */
		cofpacket_barrier_reply(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_error(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint16_t type = 0,
				uint16_t code = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0) :
			cofmsg(	sizeof(struct ofp_error_msg) + datalen,
						sizeof(struct ofp_error_msg) + datalen)
		{
			cofmsg::body.assign(data, datalen);

			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp_header) + body.memlen());
			ofh_header->type 		= OFPT_ERROR;
			ofh_header->xid			= htobe32(xid);

			ofh_error_msg->type		= htobe16(type);
			ofh_error_msg->code		= htobe16(code);
		};
		/** constructor
		 *
		 */
		cofpacket_error(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
		/**
		 *
		 */
		uint16_t
		get_type()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION:
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(ofh_error_msg->type); // no change since OF1.2
			} break;
			default:
				throw eBadVersion();
			}
		};
		/**
		 *
		 */
		uint16_t
		get_code()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION:
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(ofh_error_msg->code); // no change since OF1.2
			} break;
			default:
				throw eBadVersion();
			}
		};
};


/** OFPT_FLOW_MOD
 *
 */
class cofpacket_flow_mod :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_flow_mod(
				uint8_t of_version = 0,
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
			cofmsg(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= of_version;
			ofh_header->type 		= OFPT_FLOW_MOD;
			ofh_header->length		= htobe16(length());
			ofh_header->xid			= htobe32(xid);

			switch (of_version) {
			case OFP10_VERSION: {
				cofmsg::resize(OFP10_FLOW_MOD_STATIC_HDR_LEN);

				of10h_flow_mod->cookie			= htobe64(cookie);
				of10h_flow_mod->command			= htobe16((uint16_t)command);
				of10h_flow_mod->idle_timeout	= htobe16(idle_timeout);
				of10h_flow_mod->hard_timeout	= htobe16(hard_timeout);
				of10h_flow_mod->priority		= htobe16(priority);
				of10h_flow_mod->buffer_id		= htobe32(buffer_id);
				of10h_flow_mod->out_port		= htobe16(out_port & 0x0000ffff);
				of10h_flow_mod->flags			= htobe16(flags);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				cofmsg::resize(OFP12_FLOW_MOD_STATIC_HDR_LEN);

				of12h_flow_mod->cookie			= htobe64(cookie);
				of12h_flow_mod->cookie_mask		= htobe64(cookie_mask);
				of12h_flow_mod->table_id		= table_id;
				of12h_flow_mod->command			= command;
				of12h_flow_mod->idle_timeout	= htobe16(idle_timeout);
				of12h_flow_mod->hard_timeout	= htobe16(hard_timeout);
				of12h_flow_mod->priority		= htobe16(priority);
				of12h_flow_mod->buffer_id		= htobe32(buffer_id);
				of12h_flow_mod->out_port		= htobe32(out_port);
				of12h_flow_mod->out_group		= htobe32(out_group);
				of12h_flow_mod->flags			= htobe16(flags);
			} break;
			default:
				throw eBadVersion();
			}
		};
		/** constructor
		 *
		 */
		cofpacket_flow_mod(cofmsg const *pack)
		{
			cofmsg::operator= (*pack);
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
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return (OFP10_FLOW_MOD_STATIC_HDR_LEN + match.length() + actions.length());
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return (OFP12_FLOW_MOD_STATIC_HDR_LEN + match.length() + instructions.length());
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
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

			switch (ofh_header->version) {
			case OFP10_VERSION: {
				memcpy(buf, memarea.somem(), OFP10_FLOW_MOD_STATIC_HDR_LEN);

				match.pack((struct ofp10_match*)
						(struct ofp10_match*)(buf + sizeof(struct ofp_header)),
														sizeof(struct ofp10_match));

				actions.pack((struct ofp_action_header*)
						(buf + OFP10_FLOW_MOD_STATIC_HDR_LEN), actions.length());

			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				memcpy(buf, memarea.somem(), OFP12_FLOW_MOD_STATIC_HDR_LEN);

				match.pack((struct ofp12_match*)
						(buf + OFP12_FLOW_MOD_STATIC_HDR_LEN), match.length());

				instructions.pack((struct ofp_instruction*)
						(buf + OFP12_FLOW_MOD_STATIC_HDR_LEN + match.length()), instructions.length());

			} break;
			default:
				throw eBadVersion();
			}
		};
		/**
		 *
		 */
		uint8_t
		get_table_id()
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return of12h_flow_mod->table_id;
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
		/**
		 *
		 */
		uint8_t
		get_command()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return (be16toh(of10h_flow_mod->command) & 0x00ff);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return of12h_flow_mod->command;
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
		/**
		 *
		 */
		uint64_t
		get_cookie()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be64toh(of10h_flow_mod->cookie);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be64toh(of12h_flow_mod->cookie);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint64_t
		get_cookie_mask()
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be64toh(of12h_flow_mod->cookie_mask);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint16_t
		get_idle_timeout()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be16toh(of10h_flow_mod->idle_timeout);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of12h_flow_mod->idle_timeout);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint16_t
		get_hard_timeout()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be16toh(of10h_flow_mod->hard_timeout);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of12h_flow_mod->hard_timeout);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint16_t
		get_priority()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be16toh(of10h_flow_mod->priority);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of12h_flow_mod->priority);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_buffer_id()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be32toh(of10h_flow_mod->buffer_id);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(of12h_flow_mod->buffer_id);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_out_port()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return (uint32_t)be16toh(of10h_flow_mod->out_port);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(of12h_flow_mod->out_port);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_out_group()
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(of12h_flow_mod->out_group);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint16_t
		get_flags()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be16toh(of10h_flow_mod->flags);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of12h_flow_mod->flags);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
};


/** OFPT_FLOW_REMOVED
 *
 */
class cofpacket_flow_removed :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_flow_removed(
				uint8_t of_version = 0,
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
			cofmsg(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(length());
			ofh_header->type 		= OFPT_FLOW_REMOVED;
			ofh_header->xid			= htobe32(xid);

			switch (of_version) {
			case OFP10_VERSION: {
				cofmsg::resize(OFP10_FLOW_REMOVED_STATIC_HDR_LEN);

				of10h_flow_rmvd->cookie 		= htobe64(cookie);
				of10h_flow_rmvd->priority		= htobe16(priority);
				of10h_flow_rmvd->reason			= reason;
				of10h_flow_rmvd->duration_sec	= htobe32(duration_sec);
				of10h_flow_rmvd->duration_nsec	= htobe32(duration_nsec);
				of10h_flow_rmvd->idle_timeout	= htobe16(idle_timeout);
				of10h_flow_rmvd->packet_count	= htobe64(packet_count);
				of10h_flow_rmvd->byte_count		= htobe64(byte_count);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				cofmsg::resize(OFP12_FLOW_REMOVED_STATIC_HDR_LEN);

				of12h_flow_rmvd->cookie			= htobe64(cookie);
				of12h_flow_rmvd->priority		= htobe16(priority);
				of12h_flow_rmvd->reason			= reason;
				of12h_flow_rmvd->table_id		= table_id;
				of12h_flow_rmvd->duration_sec	= htobe32(duration_sec);
				of12h_flow_rmvd->duration_nsec	= htobe32(duration_nsec);
				of12h_flow_rmvd->idle_timeout	= htobe16(idle_timeout);
				of12h_flow_rmvd->hard_timeout	= htobe16(hard_timeout);
				of12h_flow_rmvd->packet_count	= htobe64(packet_count);
				of12h_flow_rmvd->byte_count		= htobe64(byte_count);
			} break;
			default:
				throw eBadVersion();
			}
		};
		/** constructor
		 *
		 */
		cofpacket_flow_removed(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator=(*pack);
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
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return OFP10_FLOW_REMOVED_STATIC_HDR_LEN;
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return (OFP12_FLOW_REMOVED_STATIC_HDR_LEN + match.length());
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
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

			switch (ofh_header->version) {
			case OFP10_VERSION: {
				memcpy(buf, memarea.somem(), OFP10_FLOW_REMOVED_STATIC_HDR_LEN);

				match.pack((struct ofp10_match*)
						(buf + sizeof(struct ofp_header)), sizeof(struct ofp10_match));
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				memcpy(buf, memarea.somem(), OFP12_FLOW_REMOVED_STATIC_HDR_LEN);

				match.pack((struct ofp12_match*)
						(buf + OFP12_FLOW_MOD_STATIC_HDR_LEN), match.length());
			} break;
			default:
				throw eBadVersion();
			}
		};
		/**
		 *
		 */
		uint64_t
		get_cookie()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be64toh(of10h_flow_rmvd->cookie);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be64toh(of12h_flow_rmvd->cookie);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint16_t
		get_priority()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be16toh(of10h_flow_rmvd->priority);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of12h_flow_rmvd->priority);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint8_t
		get_reason()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return of10h_flow_rmvd->reason;
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return of12h_flow_rmvd->reason;
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint8_t
		get_table_id()
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return of12h_flow_rmvd->table_id;
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_duration_sec()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be32toh(of10h_flow_rmvd->duration_sec);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(of12h_flow_rmvd->duration_sec);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_duration_nsec()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be32toh(of10h_flow_rmvd->duration_nsec);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(of12h_flow_rmvd->duration_nsec);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint16_t
		get_idle_timeout()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be16toh(of10h_flow_rmvd->idle_timeout);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of12h_flow_rmvd->idle_timeout);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint16_t
		get_hard_timeout()
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of12h_flow_rmvd->hard_timeout);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint64_t
		get_packet_count()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be64toh(of10h_flow_rmvd->packet_count);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be64toh(of12h_flow_rmvd->packet_count);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint64_t
		get_byte_count()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be64toh(of10h_flow_rmvd->byte_count);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be64toh(of12h_flow_rmvd->byte_count);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
};


/** OFPT_PACKET_IN
 *
 */
class cofpacket_packet_in :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_packet_in(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint32_t buffer_id = 0,
				uint16_t total_len = 0,
				uint8_t reason = 0,
				uint8_t table_id = 0,
				uint64_t cookie = 0,
				uint32_t in_port = 0, /*OF1.0*/
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0) :
			cofmsg(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			//cofpacket::body.assign(data, datalen);
			cofmsg::packet.unpack(OFPP_CONTROLLER, data, datalen);

			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(OFP12_PACKET_IN_STATIC_HDR_LEN + 2 + packet.framelen());
			ofh_header->type 		= OFPT_PACKET_IN;
			ofh_header->xid			= htobe32(xid);

			switch (of_version) {
			case OFP10_VERSION: {
				cofmsg::resize(OFP10_PACKET_IN_STATIC_HDR_LEN);
				cofmsg::stored = OFP10_PACKET_IN_STATIC_HDR_LEN;

				of10h_packet_in->buffer_id		= htobe32(buffer_id);
				of10h_packet_in->total_len		= htobe16(total_len);
				of10h_packet_in->in_port		= htobe16((uint16_t)(in_port & 0x0000ffff));
				of10h_packet_in->reason			= reason;
			} break;
			case OFP12_VERSION: {
				cofmsg::resize(OFP12_PACKET_IN_STATIC_HDR_LEN);
				cofmsg::stored = OFP12_PACKET_IN_STATIC_HDR_LEN;

				of12h_packet_in->buffer_id		= htobe32(buffer_id);
				of12h_packet_in->total_len		= htobe16(total_len);
				of12h_packet_in->reason			= reason;
				of12h_packet_in->table_id		= table_id;
			} break;
			case OFP13_VERSION: {
				cofmsg::resize(OFP13_PACKET_IN_STATIC_HDR_LEN);
				cofmsg::stored = OFP13_PACKET_IN_STATIC_HDR_LEN;

				of13h_packet_in->buffer_id		= htobe32(buffer_id);
				of13h_packet_in->total_len		= htobe16(total_len);
				of13h_packet_in->reason			= reason;
				of13h_packet_in->table_id		= table_id;
				of13h_packet_in->cookie			= htobe64(cookie);
			} break;
			default:
				throw eBadVersion();
			}
		};
		/** constructor
		 *
		 */
		cofpacket_packet_in(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return (OFP10_PACKET_IN_STATIC_HDR_LEN + 2 + packet.framelen());
			} break;
			case OFP12_VERSION: {
				return (OFP12_PACKET_IN_STATIC_HDR_LEN + match.length() + 2 + packet.framelen());
			} break;
			case OFP13_VERSION: {
				return (OFP13_PACKET_IN_STATIC_HDR_LEN + match.length() + 2 + packet.framelen());
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
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

			/*
			 * Please note: +2 magic => provides proper alignment of IPv4 addresses in pin_data as defined by OF spec
			 */
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				memcpy(buf, memarea.somem(), OFP10_PACKET_IN_STATIC_HDR_LEN);
				memcpy(buf + OFP10_PACKET_IN_STATIC_HDR_LEN + 2, packet.soframe(), packet.framelen());

			} break;
			case OFP12_VERSION: {
				memcpy(buf, memarea.somem(), OFP12_PACKET_IN_STATIC_HDR_LEN);
				match.pack((struct ofp12_match*)
						(buf + OFP12_PACKET_IN_STATIC_HDR_LEN), match.length());
				memcpy(buf + OFP12_PACKET_IN_STATIC_HDR_LEN + match.length() + 2, packet.soframe(), packet.framelen());

			} break;
			case OFP13_VERSION: {
				memcpy(buf, memarea.somem(), OFP13_PACKET_IN_STATIC_HDR_LEN);
				match.pack((struct ofp13_match*)
						(buf + OFP13_PACKET_IN_STATIC_HDR_LEN), match.length());
				memcpy(buf + OFP13_PACKET_IN_STATIC_HDR_LEN + match.length() + 2, packet.soframe(), packet.framelen());

			} break;
			default:
				throw eBadVersion();
			}
		};
		/*
		 * TODO: overwrite methods get_data() and get_datalen() ???
		 */
		/**
		 *
		 */
		uint8_t
		get_table_id()
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return of12h_packet_in->table_id;
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint8_t
		get_reason()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return of10h_packet_in->reason;
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return of12h_packet_in->reason;
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint16_t
		get_total_len()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be16toh(of10h_packet_in->total_len);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of12h_packet_in->total_len);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_buffer_id()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be32toh(of10h_packet_in->buffer_id);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(of12h_packet_in->buffer_id);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint64_t
		get_cookie()
		{
			switch (ofh_header->version) {
			case OFP13_VERSION: {
				return be64toh(of13h_packet_in->cookie);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_in_port()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be16toh(of10h_packet_in->in_port);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
};


/** OFPT_PACKET_OUT
 *
 */
class cofpacket_packet_out :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_packet_out(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint32_t buffer_id = 0,
				uint32_t in_port = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0) :
			cofmsg(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			cofmsg::packet.unpack(in_port, data, datalen);

			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(0);
			ofh_header->type 		= OFPT_PACKET_OUT;
			ofh_header->xid			= htobe32(xid);

			switch (of_version) {
			case OFP10_VERSION: {
				cofmsg::resize(sizeof(struct ofp10_packet_out));

				of10h_packet_out->buffer_id		= htobe32(buffer_id);
				of10h_packet_out->in_port		= htobe16((uint16_t)(in_port & 0x0000ffff));
				of10h_packet_out->actions_len	= htobe16(0); // filled in when method pack() is called

			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				cofmsg::resize(sizeof(struct ofp12_packet_out));

				of12h_packet_out->buffer_id		= htobe32(buffer_id);
				of12h_packet_out->in_port		= htobe32(in_port);
				of12h_packet_out->actions_len	= htobe16(0); // filled in when method pack() is called

			} break;
			default:
				throw eBadVersion();
			}
		};
		/** constructor
		 *
		 */
		cofpacket_packet_out(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return (sizeof(struct ofp10_packet_out) + actions.length() + packet.framelen());
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return (sizeof(struct ofp12_packet_out) + actions.length() + packet.framelen());
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval)
		{
			ofh_header->length 				= htobe16(length());
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				of10h_packet_out->actions_len 	= htobe16(actions.length());

			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				of12h_packet_out->actions_len 	= htobe16(actions.length());

			} break;
			default:
				throw eBadVersion();
			}

			if (((uint8_t*)0 == buf) || (buflen < length()))
			{
				return;
			}

			switch (ofh_header->version) {
			case OFP10_VERSION: {
				memcpy(buf, memarea.somem(), sizeof(struct ofp10_packet_out));
				actions.pack((struct ofp_action_header*)(buf + sizeof(struct ofp10_packet_out)), actions.length());
				memcpy(buf + sizeof(struct ofp10_packet_out) + actions.length(), packet.soframe(), packet.framelen());

			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				memcpy(buf, memarea.somem(), sizeof(struct ofp12_packet_out));
				actions.pack((struct ofp_action_header*)(buf + sizeof(struct ofp12_packet_out)), actions.length());
				memcpy(buf + sizeof(struct ofp12_packet_out) + actions.length(), packet.soframe(), packet.framelen());

			} break;
			default:
				throw eBadVersion();
			}
		};
		/**
		 *
		 */
		uint32_t
		get_buffer_id()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be32toh(of10h_packet_out->buffer_id);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(of12h_packet_out->buffer_id);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_in_port()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return (uint32_t)be16toh(of10h_packet_out->in_port);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(of12h_packet_out->in_port);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint16_t
		get_actions_len()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return be16toh(of10h_packet_out->actions_len);
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be16toh(of12h_packet_out->actions_len);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
};



/** OFPT_PORT_STATUS
 *
 */
class cofpacket_port_status :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_port_status(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint8_t reason = 0,
				cofport const& port = cofport()) :
			cofmsg(	sizeof(struct ofp_header),
						sizeof(struct ofp_header))
		{
			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(0);
			ofh_header->type 		= OFPT_PORT_STATUS;
			ofh_header->xid			= htobe32(xid);

			if (port.get_version() != of_version) {
				throw eBadVersion();
			}

			switch (of_version) {
			case OFP10_VERSION: {
				cofmsg::resize(sizeof(struct ofp10_port_status));

				of10h_port_status->reason = reason;
				port.pack(&(of10h_port_status->desc), sizeof(struct ofp10_port));

			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				cofmsg::resize(sizeof(struct ofp12_port_status));

				of12h_port_status->reason	= reason;
				port.pack(&(of12h_port_status->desc), sizeof(struct ofp12_port));

			} break;
			default:
				throw eBadVersion();
			}
		};
		/** constructor
		 *
		 */
		cofpacket_port_status(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
			switch (ofh_header->version) {
			case OFP10_VERSION: {
				return (sizeof(struct ofp10_port_status));
			} break;
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return (sizeof(struct ofp12_port_status));
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint8_t
		get_reason()
		{
			switch (ofh_header->version) {
			case OFP10_VERSION: return of10h_port_status->reason;
			case OFP12_VERSION: return of12h_port_status->reason;
			case OFP13_VERSION: return of13h_port_status->reason;
			default: throw eBadVersion();
			}
			return 0;
		}
};


/** OFPT_PORT_MOD
 *
 */
class cofpacket_port_mod :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_port_mod(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint32_t port_no = 0,
				cmacaddr const& hwaddr = cmacaddr("00:00:00:00:00:00"),
				uint32_t config = 0,
				uint32_t mask = 0,
				uint32_t advertise = 0);
		/** constructor
		 *
		 */
		cofpacket_port_mod(cofmsg const *pack);
		/** destructor
		 *
		 */
		virtual
		~cofpacket_port_mod();
		/** length
		 *
		 */
		virtual size_t
		length();
		/**
		 *
		 */
		uint32_t
		get_port_no();
		/**
		 *
		 */
		uint32_t
		get_config();
		/**
		 *
		 */
		uint32_t
		get_mask();
		/**
		 *
		 */
		uint32_t
		get_advertise();
};




/** OFPT_GROUP_MOD
 *
 */
class cofpacket_group_mod :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_group_mod(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint16_t command = 0,
				uint8_t  type = 0,
				uint32_t group_id = 0);
		/** constructor
		 *
		 */
		cofpacket_group_mod(cofmsg const *pack);
		/** destructor
		 *
		 */
		virtual
		~cofpacket_group_mod();
		/** length
		 *
		 */
		virtual size_t
		length();
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval);
		/**
		 *
		 */
		uint16_t
		get_command();
		/**
		 *
		 */
		uint8_t
		get_port_no();
		/**
		 *
		 */
		uint32_t
		get_group_id();
};



/** OFPT_TABLE_MOD
 *
 */
class cofpacket_table_mod :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_table_mod(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint8_t  table_id = 0,
				uint32_t config = 0);
		/** constructor
		 *
		 */
		cofpacket_table_mod(cofmsg const *pack);
		/** destructor
		 *
		 */
		virtual
		~cofpacket_table_mod();
		/** length
		 *
		 */
		virtual size_t
		length();
		/**
		 *
		 */
		uint8_t
		get_table_id();
		/**
		 *
		 */
		uint32_t
		get_config();
};



/** OFPT_STATS_REQUEST
 *
 */
class cofpacket_stats_request :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_stats_request(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint16_t type = 0,
				uint16_t flags = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0);
		/** constructor
		 *
		 */
		cofpacket_stats_request(cofmsg const *pack);
		/** destructor
		 *
		 */
		virtual
		~cofpacket_stats_request();
		/** length
		 *
		 */
		virtual size_t
		length();
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval);
		/**
		 *
		 */
		uint16_t
		get_type();
		/**
		 *
		 */
		uint16_t
		get_flags();
};



/** OFPT_STATS_REPLY
 *
 */
class cofpacket_stats_reply :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_stats_reply(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint16_t type = 0,
				uint16_t flags = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0);
		/** constructor
		 *
		 */
		cofpacket_stats_reply(cofmsg const *pack);
		/** destructor
		 *
		 */
		virtual
		~cofpacket_stats_reply();
		/** length
		 *
		 */
		virtual size_t
		length();
		/**
		 *
		 */
		virtual void
		pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0) throw (eOFpacketInval);
		/**
		 *
		 */
		uint16_t
		get_type();
		/**
		 *
		 */
		uint16_t
		get_flags();
};



/** OFPT_DESC_STATS_REPLY
 *
 */
class cofpacket_desc_stats_reply :
	public cofpacket_stats_reply
{
public:
		/** constructor
		 *
		 */
		cofpacket_desc_stats_reply(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint16_t flags = 0,
				cofdesc_stats_reply const& desc_stats = cofdesc_stats_reply()) :
			cofpacket_stats_reply(of_version, xid, OFPST_DESC, flags)
		{
			body.resize(desc_stats.length());
			desc_stats.pack(body.somem(), body.memlen());
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_desc_stats_reply() {};
		/**
		 *
		 */
		cofdesc_stats_reply&
		get_desc_stats()
		{
			return desc_stats_reply;
		};
};



/** OFPT_FLOW_STATS_REPLY
 *
 */
class cofpacket_flow_stats_reply :
	public cofpacket_stats_reply
{
public:
		/** constructor
		 *
		 */
		cofpacket_flow_stats_reply(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint16_t flags = 0,
				cofflow_stats_reply const& flow_stats_reply = cofflow_stats_reply()) :
			cofpacket_stats_reply(of_version, xid, OFPST_FLOW, flags)
		{
			body.resize(flow_stats_reply.length());
			flow_stats_reply.pack(body.somem(), body.memlen());
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_flow_stats_reply() {};
		/**
		 *
		 */
		cofflow_stats_reply&
		get_flow_stats_reply()
		{
			return flow_stats_reply;
		};
};



/** OFPT_FLOW_STATS_REQUEST
 *
 */
class cofpacket_flow_stats_request :
	public cofpacket_stats_request
{
public:
		/** constructor
		 *
		 */
		cofpacket_flow_stats_request(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint16_t type = OFPST_FLOW,
				uint16_t flags = 0,
				uint8_t table_id = 0,
				uint32_t out_port = OFPP_ANY,
				uint32_t out_group = OFPG_ANY,
				uint64_t cookie = 0,
				uint64_t cookie_mask = 0) :
			cofpacket_stats_request(of_version, xid, type, flags)
		{
			switch (ofh_header->version) {
			case OFP12_VERSION: {
				size_t flow_stats_body_len = 32;
				cofmsg::body.resize(flow_stats_body_len);
				struct ofp12_flow_stats_request* flow_stats = (struct ofp12_flow_stats_request*)(body.somem());

				flow_stats->table_id		= table_id;
				flow_stats->out_port		= htobe32(out_port);
				flow_stats->out_group		= htobe32(out_group);
				flow_stats->cookie			= htobe64(cookie);
				flow_stats->cookie_mask		= htobe64(cookie_mask);

			} break;
			case OFP13_VERSION: {
				// TODO
				throw eNotImplemented();
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
		};
		/** constructor
		 *
		 */
		cofpacket_flow_stats_request(cofmsg const *pack)
		{
			cofmsg::operator =(*pack);
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_flow_stats_request() {};
		/** length
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp12_stats_request) + body.memlen() + match.length());
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

			memcpy(buf, memarea.somem(), sizeof(struct ofp12_stats_request));

			memcpy(buf + sizeof(struct ofp12_stats_request), body.somem(), body.memlen());

			match.pack((struct ofp12_match*)(buf + sizeof(struct ofp12_stats_request) +
														body.memlen()), match.length());
		};
};





/** OFPT_STATS_DESC_REPLY
 *
 */
class cofpacket_stats_desc_reply :
	public cofpacket_stats_reply
{
public:
		/** constructor
		 *
		 */
		cofpacket_stats_desc_reply(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint16_t type = OFPST_DESC,
				uint16_t flags = 0,
				std::string const& mfr_desc = std::string(""),
				std::string const&  hw_desc = std::string(""),
				std::string const&  sw_desc = std::string(""),
				std::string const&  serial_num = std::string(""),
				std::string const&  dp_desc = std::string("")) :
			cofpacket_stats_reply(of_version, xid, type, flags)
		{
			switch (get_version()) {
			case OFP10_VERSION: {
				cofpacket_stats_reply::body.resize(sizeof(struct ofp10_desc_stats));
				struct ofp10_desc_stats *desc_stats = (struct ofp10_desc_stats*)(cofmsg::body.somem());

				snprintf(desc_stats->mfr_desc, DESC_STR_LEN, mfr_desc.c_str(), mfr_desc.length());
				snprintf(desc_stats->hw_desc, DESC_STR_LEN, hw_desc.c_str(), hw_desc.length());
				snprintf(desc_stats->sw_desc, DESC_STR_LEN, sw_desc.c_str(), sw_desc.length());
				snprintf(desc_stats->serial_num, DESC_STR_LEN, serial_num.c_str(), serial_num.length());
				snprintf(desc_stats->dp_desc, DESC_STR_LEN, dp_desc.c_str(), dp_desc.length());
			} break;
			case OFP12_VERSION: {
				cofpacket_stats_reply::body.resize(sizeof(struct ofp12_desc_stats));
				struct ofp12_desc_stats *desc_stats = (struct ofp12_desc_stats*)(cofmsg::body.somem());

				snprintf(desc_stats->mfr_desc, DESC_STR_LEN, mfr_desc.c_str(), mfr_desc.length());
				snprintf(desc_stats->hw_desc, DESC_STR_LEN, hw_desc.c_str(), hw_desc.length());
				snprintf(desc_stats->sw_desc, DESC_STR_LEN, sw_desc.c_str(), sw_desc.length());
				snprintf(desc_stats->serial_num, DESC_STR_LEN, serial_num.c_str(), serial_num.length());
				snprintf(desc_stats->dp_desc, DESC_STR_LEN, dp_desc.c_str(), dp_desc.length());
			} break;
			case OFP13_VERSION: {
				// TODO
				throw eNotImplemented();
			} break;
			default:
				throw eBadVersion();
			}
		};
		/** destructor
		 *
		 */
		virtual
		~cofpacket_stats_desc_reply() {};
		/**
		 *
		 */
		size_t
		length() const
		{
			switch (get_version()) {
			case OFP10_VERSION: {
				return sizeof(struct ofp10_desc_stats);
			} break;
			case OFP12_VERSION: {
				return sizeof(struct ofp12_desc_stats);
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
};











/** OFPT_QUEUE_GET_CONFIG_REQUEST
 *
 */
class cofpacket_queue_get_config_request :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_queue_get_config_request(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint32_t port = 0);
		/** constructor
		 *
		 */
		cofpacket_queue_get_config_request(cofmsg const *pack);
		/** destructor
		 *
		 */
		virtual
		~cofpacket_queue_get_config_request();
		/** length
		 *
		 */
		virtual size_t
		length();
		/**
		 *
		 */
		uint32_t
		get_port() const;
};







/** OFPT_QUEUE_GET_CONFIG_REPLY
 *
 */
class cofpacket_queue_get_config_reply :
	public cofmsg
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
				uint8_t of_version,
				uint32_t xid = 0,
				uint32_t port = 0);
		/** constructor
		 *
		 */
		cofpacket_queue_get_config_reply(cofmsg const *pack);
		/** destructor
		 *
		 */
		virtual
		~cofpacket_queue_get_config_reply();
		/** length
		 *
		 */
		virtual size_t
		length();
		/**
		 *
		 */
		uint32_t
		get_port() const;
};









/** OFPT_EXPERIMENTER
 *
 */
class cofpacket_experimenter :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_experimenter(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint32_t experimenter = 0,
				uint32_t exp_type = 0,
				uint8_t *data = (uint8_t*)0,
				size_t datalen = 0) :
			cofmsg(	sizeof(struct ofp_experimenter_header) + datalen,
						sizeof(struct ofp_experimenter_header) + datalen)
		{
			cofmsg::body.assign(data, datalen);

			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp_experimenter_header) + body.memlen());
			ofh_header->type 		= OFPT_EXPERIMENTER;
			ofh_header->xid			= htobe32(xid);


			ofh_experimenter->experimenter		= htobe32(experimenter);
			ofh_experimenter->exp_type			= htobe32(exp_type);
		};
		/** constructor
		 *
		 */
		cofpacket_experimenter(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
		/**
		 *
		 */
		uint32_t
		get_experimenter() const
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(ofh_experimenter->experimenter);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_exp_type() const
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(ofh_experimenter->exp_type);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
};


/** OFPT_ROLE_REQUEST
 *
 */
class cofpacket_role_request :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_role_request(
				uint8_t of_version = 0,
				uint32_t xid = 0,
				uint32_t role = 0,
				uint64_t generation_id = 0) :
			cofmsg(	sizeof(struct ofp_role_request),
						sizeof(struct ofp_role_request))
		{
			ofh_header->version 	= of_version;
			ofh_header->length		= htobe16(sizeof(struct ofp_role_request));
			ofh_header->type 		= OFPT_ROLE_REQUEST;
			ofh_header->xid			= htobe32(xid);

			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				ofh_role_request->role				= htobe32(role);
				ofh_role_request->generation_id 	= htobe64(generation_id);
			} break;
			default:
				throw eBadVersion();
			}
		};
		/** constructor
		 *
		 */
		cofpacket_role_request(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return (sizeof(struct ofp_role_request));
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_role() const
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(ofh_role_request->role);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
		/**
		 *
		 */
		uint64_t
		get_generation_id() const
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be64toh(ofh_role_request->generation_id);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
};


/** OFPT_ROLE_REPLY
 *
 */
class cofpacket_role_reply :
	public cofmsg
{
public:
		/** constructor
		 *
		 */
		cofpacket_role_reply(
				uint8_t version,
				uint32_t xid = 0,
				uint32_t role = 0,
				uint64_t generation_id = 0) :
			cofmsg(	sizeof(struct ofp_role_request),
						sizeof(struct ofp_role_request))
		{
			ofh_header->version 	= version;
			ofh_header->length		= htobe16(sizeof(struct ofp_role_request));
			ofh_header->type 		= OFPT_ROLE_REPLY;
			ofh_header->xid			= htobe32(xid);

			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				ofh_role_request->role				= htobe32(role);
				ofh_role_request->generation_id 	= htobe64(generation_id);
			} break;
			default:
				throw eBadVersion();
			}
		};
		/** constructor
		 *
		 */
		cofpacket_role_reply(cofmsg const *pack) :
			cofmsg(pack->framelen(), pack->framelen())
		{
			cofmsg::operator =(*pack);
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
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return (sizeof(struct ofp_role_request));
			} break;
			default:
				throw eBadVersion();
			}
			return 0;
		};
		/**
		 *
		 */
		uint32_t
		get_role() const
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be32toh(ofh_role_request->role);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
		/**
		 *
		 */
		uint64_t
		get_generation_id() const
		{
			switch (ofh_header->version) {
			case OFP12_VERSION:
			case OFP13_VERSION: {
				return be64toh(ofh_role_request->generation_id);
			} break;
			default: {
				throw eBadVersion();
			} break;
			}
			return 0;
		};
};
#endif
}; // end of namespace

#endif
