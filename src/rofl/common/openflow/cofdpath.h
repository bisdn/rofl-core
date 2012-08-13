/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFDPATH_H
#define COFDPATH_H 1

#include <map>
#include <set>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "openflow12.h"
#include <stdio.h>
#include <strings.h>

#ifdef __cplusplus
}
#endif

#include "../ciosrv.h"
#include "../cfwdtable.h"
#include "../cmemory.h"
#include "../ciosrv.h"
#include "../cerror.h"
#include "../cfwdelem.h"
#include "../cmemory.h"
#include "../cvastring.h"
#include "../cfibentry.h"
#include "../cfsm.h"
#include "../protocols/fetherframe.h"

#include "cfttable.h"
#include "cgttable.h"
#include "cofpacket.h"
#include "cofport.h"
#include "extensions/cfsptable.h"

class cfwdelem;
class cofbase;
class cfttable;

/* error classes */
class eDataPathIdInUse : public cerror {}; // datapath id already in use
class eDataPathAlreadyAttached : public cerror {}; // cfwdelem *entity is already attached
class eOFswitchBase : public cerror {};
class eOFswitchInvalid : public eOFswitchBase {};
class eOFdpathNotFound : public eOFswitchBase {}; // element not found



class cofdpath :
	public cftentry_owner,
	public cgtentry_owner,
	public cfsm,
	public ciosrv
{

		/* cofdpath timer types */
		enum cofdpath_timer_t {
			COFDPATH_TIMER_BASE = 0x21,
			COFDPATH_TIMER_FEATURES_REQUEST 	= ((COFDPATH_TIMER_BASE) << 16 | (0x01 << 8)),
			COFDPATH_TIMER_FEATURES_REPLY 		= ((COFDPATH_TIMER_BASE) << 16 | (0x02 << 8)),
			COFDPATH_TIMER_GET_CONFIG_REQUEST 	= ((COFDPATH_TIMER_BASE) << 16 | (0x03 << 8)),
			COFDPATH_TIMER_GET_CONFIG_REPLY 	= ((COFDPATH_TIMER_BASE) << 16 | (0x04 << 8)),
			COFDPATH_TIMER_STATS_REQUEST 		= ((COFDPATH_TIMER_BASE) << 16 | (0x05 << 8)),
			COFDPATH_TIMER_STATS_REPLY 			= ((COFDPATH_TIMER_BASE) << 16 | (0x06 << 8)),
			COFDPATH_TIMER_GET_FSP_REQUEST		= ((COFDPATH_TIMER_BASE) << 16 | (0x07 << 8)),
			COFDPATH_TIMER_GET_FSP_REPLY	    = ((COFDPATH_TIMER_BASE) << 16 | (0x08 << 8)),
			COFDPATH_TIMER_BARRIER_REQUEST 		= ((COFDPATH_TIMER_BASE) << 16 | (0x09 << 8)),
			COFDPATH_TIMER_BARRIER_REPLY 		= ((COFDPATH_TIMER_BASE) << 16 | (0x0a << 8)),
			COFDPATH_TIMER_LLDP_SEND_DISC 		= ((COFDPATH_TIMER_BASE) << 16 | (0x0b << 8)),
		};

		/* cofdpath state types */
		enum cofdpath_state_t {
			DP_STATE_INIT 				= (1 << 0),
			DP_STATE_RUNNING 			= (1 << 1),
		};

#define DEFAULT_DP_FEATURES_REPLY_TIMEOUT 		10
#define DEFAULT_DP_GET_CONFIG_REPLY_TIMEOUT 	10
#define DEFAULT_DP_STATS_REPLY_TIMEOUT 			10
#define DEFAULT_DP_GET_FSP_REPLY_TIMEOUT 		10
#define DEFAULT_DB_BARRIER_REPLY_TIMEOUT 		10


public: // data structures


		cfwdelem 						*fwdelem;		// layer-(n) entity
		cofbase 						*entity;		// layer-(n-1) entity
		std::map<cofbase*, cofdpath*> 	*ofswitch_list; // cofswitch map this
														// instance belongs to
														// (hosted by parent cfwdelem instance)
		uint64_t 						dpid;			// datapath id
		std::string	 					s_dpid;			// datapath id as std::string
		cmacaddr 						dpmac;			// datapath mac address
		uint32_t 						n_buffers; 		// number of buffer lines
		uint8_t 						n_tables;		// number of tables
		uint32_t 						capabilities;	// capabilities flags
		uint32_t 						actions;		// supported actions
		std::map<uint32_t, cofport*> 	ports;			// list of ports
		uint16_t 						flags;			// 'fragmentation' flags
		uint16_t 						miss_send_len; 	// length of bytes sent to controller
		std::string 					info;			// info string
		std::map<uint8_t, cfttable*> 	flow_tables;	// flow_table of this switch instance
		cgttable 						grp_table;		// group_table of this switch instance
		cfwdtable 						fwdtable; 		// forwarding table for attached MAC
														// addresses (Ethernet endpoints)
		cfsptable 						fsptable;		// flowspace registration table


		int 							features_reply_timeout;
		int 							get_config_reply_timeout;
		int 							stats_reply_timeout;
		int 							get_fsp_reply_timeout;
		int 							barrier_reply_timeout;




public:

	/** constructor
	 */
	cofdpath(
		cfwdelem *fwdelem,
		cofbase *entity,
		std::map<cofbase*, cofdpath*>* dpath_list,
		uint64_t dpid = 0,
		uint32_t n_buffers = 0,
		uint8_t n_tables = 0,
		uint32_t capabilities = 0,
		uint32_t actions = 0,
		struct ofp_port *ports = NULL, int num_ports = 0);


	/** destructor
	 */
	virtual
	~cofdpath();


	/** find cofport from this->port_list
	 */
	cofport*
	find_cofport(
			uint32_t port_no) throw (eOFdpathNotFound);


	/** find cofport from this->port_list
	 */
	cofport*
	find_cofport(
			std::string port_name) throw (eOFdpathNotFound);


	/** handle FEATURES request/reply
	 */
	void
	features_request_sent();
	void
	features_reply_rcvd(
			cofpacket *pack);


	/** handle GET-CONFIG request/reply
	 */
	void
	get_config_request_sent();
	void
	get_config_reply_rcvd(
			cofpacket *pack);


	/** handle STATS request/reply
	 */
	void
	stats_request_sent();
	void
	stats_reply_rcvd(
			cofpacket *pack);


	/** handle GET-FSP request/reply
	 */
	void
	get_fsp_request_sent();
	void
	get_fsp_reply_rcvd(
			cofpacket *pack);


	/** update internal status for a barrier request
	 */
	void
	barrier_request_sent();
	void
	barrier_reply_rcvd();


	/** handle FLOW-MOD/FLOW-REMOVED message
	 */
	void
	flow_mod_sent(
			cofpacket *pack) throw (eOFdpathNotFound);
	void
	flow_removed_rcvd(
			cofpacket *pack);


	/** handle GROUP-MOD message
	 */
	void
	group_mod_sent(
			cofpacket *pack);


	/** handle TABLE-MOD message
	 */
	void
	table_mod_sent(
			cofpacket *pack);


	/** handle PACKET-IN message
	 */
	void
	packet_in_rcvd(
			cofpacket *pack);


	/** handle PORT-STATUS message
	 */
	void
	port_status_rcvd(
			cofpacket *pack);


	/** handle VENDOR message
	 */
	void
	experimenter_message_rcvd(
			cofpacket *pack);


	/** handle ROLE-REPLY messages
	 */
	void
	role_reply_rcvd(
			cofpacket *pack);


	/** open flowspace
	 */
	void
	fsp_open(
			cofmatch const& ofmatch);


	/** close flowspace
	 */
	void
	fsp_close(
			cofmatch const& ofmatch);


	/** return info string
	 */
	const char*
	c_str();


	/** remove all FlowMods from datapath
	 */
	void
	flow_mod_reset();


	/** remove all GroupMods from datapath
	 */
	void
	group_mod_reset();


public: // overloaded from cftentry_owner


	/** notification for ftentry hard timeout
	 */
	virtual void
    ftentry_timeout(
    		cftentry *entry,
    		uint16_t timeout);


public: // overloaded from cftentry_owner


	/** notification for ftentry hard timeout
	 */
	virtual void
    gtentry_timeout(
    		cgtentry *entry,
    		uint16_t timeout);


public: // overloaded from ciosrv

	/** handle timeout
	 */
	void
	handle_timeout(
		int opaque);




protected:

	/** save datapath features from OFPT_FEATURES_REPLY
	 */
	void
	features_save(
		cofpacket *pack);


	/** save datapath configuration from OFPT_GET_CONFIG_REPLY
	 */
	void
	config_save(
		cofpacket *pack);


	/** handle FEATURES reply timeout
	 */
	void
	handle_features_reply_timeout();


	/** handle GET-CONFIG reply timeout
	 */
	void
	handle_get_config_reply_timeout();


	/** handle STATS reply timeout
	 */
	void
	handle_stats_reply_timeout();


	/** handle GET-FSP reply timeout
	 */
	void
	handle_get_fsp_reply_timeout();


	/** handle BARRIER reply timeout
	 */
	void
	handle_barrier_reply_timeout();

};

#endif
