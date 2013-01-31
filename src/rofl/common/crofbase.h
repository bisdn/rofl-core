/*
 * crofbase.h
 *
 *  Created on: 25.10.2012
 *      Author: andreas
 */

#ifndef CROFBASE_H
#define CROFBASE_H 1

#include <map>
#include <set>
#include <list>
#include <vector>
#include <bitset>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif
#include "openflow/openflow12.h"
#include "openflow/openflow_rofl.h"
#include <endian.h>
#include <string.h>
#include <time.h>

#ifndef htobe16
#include "endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "ciosrv.h"
#include "cerror.h"
#include "cvastring.h"
#include "croflexp.h"
#include "cphyport.h"
#include "csocket.h"

//#include "rofl/experimental/crib.h"
#include "rofl/platform/unix/crandom.h"

#include "openflow/cofpacket.h"
#include "openflow/cofport.h"
#include "openflow/cofdpt.h"
#include "openflow/cofctl.h"
#include "openflow/cofinst.h"
#include "openflow/cofinlist.h"
#include "openflow/cofaction.h"
#include "openflow/cofaclist.h"
#include "openflow/cofmatch.h"
#include "openflow/cflowentry.h"
#include "openflow/cgroupentry.h"
#include "openflow/cofstats.h"
#include "openflow/extensions/cfsptable.h"
#include "openflow/openflow_rofl_exceptions.h"

namespace rofl
{


/* error classes */
class eRofBase						: public cerror {};   // base error class crofbase
class eRofBaseIsBusy 				: public eRofBase {}; // this FwdElem is already controlled
class eRofBaseNotImpl 				: public eRofBase {}; // this FwdElem's method is not implemented
class eRofBaseNoCtrl 				: public eRofBase {}; // no controlling entity attached to this FwdElem
class eRofBaseNotFound 				: public eRofBase {}; // internal entity not found
class eRofBaseInval					: public eRofBase {}; // invalid parameter (e.g. invalid packet type)
class eRofBaseNotAttached 			: public eRofBase {}; // received command from entity being not attached
class eRofBaseNoRequest 			: public eRofBase {}; // no request packet found for session
class eRofBaseXidInval	 			: public eRofBase {}; // invalid xid in session exchange
class eRofBaseExists				: public eRofBase {}; // fwdelem with either this dpid or dpname already exists
class eRofBaseOFportNotFound 		: public eRofBase {}; // cofport instance not found
class eRofBaseTableNotFound 		: public eRofBase {}; // flow-table not found (e.g. unknown table_id in flow_mod)
class eRofBaseGotoTableNotFound 	: public eRofBase {}; // table-id specified in OFPIT_GOTO_TABLE invalid
class eRofBaseFspSupportDisabled 	: public eRofBase {};




class cofctl;
class cofdpt;


class crofbase :
	public ciosrv,
	public csocket_owner,
	public cfsm
{
private: // data structures

	enum crofbase_flag_t {
		NSP_ENABLED = 0x01,
	};

	std::bitset<32> fe_flags;


private: // packet queues for OpenFlow messages


	std::string 				info;			// info string


protected: // data structures

	std::set<cofctl*>			ofctl_set;		// set of connected controllers
	std::set<cofdpt*>			ofdpt_set;		// set of connected data path elements

	cfsptable 					fsptable; 		// namespace table


public:


	friend class cport;



protected:

	enum fwdelem_timer_t {
		TIMER_FE_BASE = (0x0020 << 16),
		TIMER_FE_DUMP_OFPACKETS,
	};

	enum crofbase_rpc_t { // for cofrpc *rpc[2]; (see below)
		RPC_CTL = 0,
		RPC_DPT = 1,
	};

	std::set<csocket*>			rpc[2];			// RPC endpoints: rpc[RPC_CTL] and rpc[RPC_DPT]


public: // static methods and data structures


	static std::set<crofbase*> rofbases; 		/**< set of all registered fwdelems */


public: // constructor + destructor


	/** Constructor.
	 *
	 * A crofbase instance must have a unique dpname and dpid.
	 * The constructor verifies dpname's and dpid's uniqueness and throws
	 * an exception of type eRofBaseExists if these values are already occupied.
	 *
	 * @throw eRofBaseExists
	 */
	crofbase() throw (eRofBaseExists);


	/** Destructor.
	 *
	 * Removes all pending requests from queues fe_down_queue[] and fe_up_queues[]
	 * and removes this instance from set crofbase::fwdelems.
	 */
	virtual
	~crofbase();


public:

	/*
	 * methods for connecting/disconnecting/accepting calls to/from ctls and dpts
	 */

	/**
	 * @name	rpc_listen_for_dpts
	 * @brief	Opens a listening socket for accepting connection requests from dpts
	 *
	 * Opens a listening socket for accepting connection requests from dpts.
	 *
	 * @param addr Address to bind before listening.
	 */
	void
	rpc_listen_for_dpts(
			caddress const& addr = caddress(AF_INET, "0.0.0.0", 6633),
			int domain = PF_INET,
			int type = SOCK_STREAM,
			int protocol = IPPROTO_TCP,
			int backlog = 10);


	/**
	 * @name	rpc_listen_for_ctls
	 * @brief	Opens a listening socket for accepting connection requests from ctls
	 *
	 * Opens a listening socket for accepting connection requests from ctls.
	 *
	 * @param addr Address to bind before listening.
	 */
	void
	rpc_listen_for_ctls(
			caddress const& addr = caddress(AF_INET, "0.0.0.0", 6644),
			int domain = PF_INET,
			int type = SOCK_STREAM,
			int protocol = IPPROTO_TCP,
			int backlog = 10);


	/** Establish OF TCP connection to control entity
	 *
	 */
	void
	rpc_connect_to_ctl(
			caddress const& ra,
			int domain = PF_INET,
			int type = SOCK_STREAM,
			int protocol = IPPROTO_TCP);


	/** Close OF TCP connection to control entity
	 *
	 */
	void
	rpc_disconnect_from_ctl(
			cofctl *ctrl);



	/** Establish OF TCP connection to datapath entity
	 *
	 */
	void
	rpc_connect_to_dpt(
			caddress const& ra,
			int domain = PF_INET,
			int type = SOCK_STREAM,
			int protocol = IPPROTO_TCP);


	/** Close OF TCP connection to datapath entity
	 *
	 */
	void
	rpc_disconnect_from_dpt(
			cofdpt *dpath);

protected:


	/**
	 *
	 */
	virtual cofctl*
	cofctl_factory(
			crofbase* owner,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol);


	/**
	 *
	 */
	virtual cofctl*
	cofctl_factory(
			crofbase* owner,
			caddress const& ra,
			int domain,
			int type,
			int protocol);


	/**
	 *
	 */
	virtual cofdpt*
	cofdpt_factory(
			crofbase* owner,
			int newsd,
			caddress const& ra,
			int domain,
			int type,
			int protocol);


	/**
	 *
	 */
	virtual cofdpt*
	cofdpt_factory(
			crofbase* owner,
			caddress const& ra,
			int domain,
			int type,
			int protocol);



protected:

	/*
	 * The following methods should be overwritten by a derived class
	 * in order to get reception notifications for the various OF
	 * packets. While crofbase handles most of the lower layer details,
	 * a derived class must provide higher layer functionality.
	 */

	/** Handle OF features request. To be overwritten by derived class.
	 *
	 * OF FEATURES.requests are handled by the crofbase base class in method
	 * crofbase::send_features_reply(). However,
	 * this method handle_features_request() may be overloaded by a derived class to get a notification
	 * upon reception of a FEATURES.request from the controlling entity.
	 * Default behaviour is to remove the packet from the heap.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack OF packet received from controlling entity.
	 */
	virtual void
	handle_features_request(cofctl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF features reply. To be overwritten by derived class.
	 *
	 * OF FEATURES.replies are handled by the crofbase base class in method
	 * crofbase::fe_up_features_reply(). This method handle_features_reply()
	 * may be overwritten by a derived class to get a notification upon
	 * reception of a FEATURES.reply from a controlled datapath.
	 * Default behaviour is to remove the packet from the heap.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw The cofswitch instance holding all parameters from the
	 * attached datapath.
	 * @param pack The FEATURES.reply packet received.
	 */
	virtual void
	handle_features_reply(cofdpt *sw, cofpacket *pack) { delete pack; };

	/** Handle OF features reply timeout. To be overwritten by derived class.
	 *
	 * This method is called when TIMER_SEND_FEATURES_REPLY expires, i.e.
	 * the FEATURES.reply of a datapath element was lost. This method
	 * should be overwritten by a derived class.
	 *
	 * @param sw The cofswitch instance holding all parameters from the attached
	 * datapath.
	 */
	virtual void
	handle_features_reply_timeout(cofdpt *dpt);

	/** Handle OF get-config request. To be overwritten by derived class.
	 *
	 * Called from within crofbase::fe_down_get_config_request().
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param ctrl cofdpath instance from whom the GET-CONFIG.request was received.
	 * @pack OF GET-CONFIG.request packet received from controller
	 */
	virtual void
	handle_get_config_request(cofctl *ctrl, cofpacket *pack) { delete pack; };

	/** Handle OF get-config reply. To be overwritten by derived class.
	 *
	 * Called from within crofbase::fe_up_get_config_reply().
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom the GET-CONFIG.reply was received.
	 * @pack OF GET-CONFIG.reply packet received from datapath
	 */
	virtual void
	handle_get_config_reply(cofdpt *sw, cofpacket *pack) { delete pack; };

	/** Handle OF get-config reply timeout. To be overwritten by derived class.
	 *
	 * Called upon expiration of TIMER_FE_SEND_GET_CONFIG_REPLY.
	 *
	 * @param sw cotswitch instance from whom a GET-CONFIG.reply was expected.
	 */
	virtual void
	handle_get_config_reply_timeout(cofdpt *dpt);

	/** Handle OF stats request. NOT to be overwritten by derived class.
	 *
	 * Called upon reception of a STATS.request from the controlling entity.
	 *
	 * @param pack STATS.request packet received from controller.
	 */
	void
	handle_stats_request(cofctl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_desc_stats_request(cofctl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_table_stats_request(cofctl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_port_stats_request(cofctl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_flow_stats_request(cofctl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_aggregate_stats_request(cofctl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_queue_stats_request(cofctl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_group_stats_request(cofctl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_group_desc_stats_request(cofctl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_group_features_stats_request(cofctl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_experimenter_stats_request(cofctl *ofctrl, cofpacket *pack);


	/** Handle OF stats reply. To be overwritten by derived class.
	 *
	 * Called upon reception of a STATS.reply from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a STATS.reply was received
	 * @param pack STATS.reply packet received from datapath
	 */
	virtual void
	handle_stats_reply(cofdpt *sw, cofpacket *pack) { delete pack; };

	/** Handle OF stats reply timeout. To be overwritten by derived class.
	 *
	 * Called upon expiration of TIMER_FE_SEND_STATS_REPLY.
	 *
	 * @param sw cotswitch instance from whom a GET-CONFIG.reply was expected.
	 */
	virtual void
	handle_stats_reply_timeout(cofdpt *sw, uint32_t xid) {};

	/** Handle OF packet-out messages. To be overwritten by derived class.
	 *
	 * Called upon reception of a PACKET-OUT.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack PACKET-OUT.message packet received from controller.
	 */
	virtual void
	handle_packet_out(cofctl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF packet-in messages. To be overwritten by derived class.
	 *
	 * Called upon reception of a PACKET-IN.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a PACKET-IN.message was received
	 * @param pack PACKET-IN.message packet received from datapath
	 */
	virtual void
	handle_packet_in(cofdpt *sw, cofpacket *pack) { delete pack; };

	/** Handle OF barrier request. To be overwritten by derived class.
	 *
	 * Called upon reception of a BARRIER.request from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack BARRIER.request packet received from controller.
	 */
	virtual void
	handle_barrier_request(cofctl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF barrier reply. To be overwritten by derived class.
	 *
	 * Called upon reception of a BARRIER.reply from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a BARRIER.reply was received
	 * @param pack BARRIER.reply packet received from datapath
	 */
	virtual void
	handle_barrier_reply(cofdpt *sw, cofpacket *pack) { delete pack; };

	/** Handle OF barrier reply timeout. To be overwritten by derived class.
	 *
	 * Called upon expiration of TIMER_FE_SEND_BARRIER_REPLY.
	 *
	 * @param sw cotswitch instance from whom a BARRIER.reply was expected.
	 */
	virtual void
	handle_barrier_reply_timeout(cofdpt *sw, uint32_t xid) {};

	/** Handle OF error message. To be overwritten by derived class.
	 *
	 * Called upon reception of an ERROR.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom an ERROR.message was received
	 * @param pack ERROR.message packet received from datapath
	 */
	virtual void
	handle_error(cofdpt *sw, cofpacket *pack) { delete pack; };

	/** Handle OF flow-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a FLOW-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack FLOW-MOD.message packet received from controller.
	 */
	virtual void
	handle_flow_mod(cofctl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF group-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a GROUP-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack GROUP-MOD.message packet received from controller.
	 */
	virtual void
	handle_group_mod(cofctl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF table-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a TABLE-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack TABLE-MOD.message packet received from controller.
	 */
	virtual void
	handle_table_mod(cofctl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF port-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a PORT-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack PORT-MOD.message packet received from controller.
	 */
	virtual void
	handle_port_mod(cofctl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF flow-removed message. To be overwritten by derived class.
	 *
	 * Called upon reception of a FLOW-REMOVED.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a FLOW-REMOVED.message was received
	 * @param pack FLOW-REMOVED.message packet received from datapath
	 */
	virtual void
	handle_flow_removed(cofdpt *sw, cofpacket *pack) { delete pack; };

	/** Handle OF port-status message. To be overwritten by derived class.
	 *
	 * Called upon reception of a PORT-STATUS.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a PORT-STATUS.message was received
	 * @param pack PORT-STATUS.message packet received from datapath
	 */
	virtual void
	handle_port_status(cofdpt *sw, cofpacket *pack, cofport *port) { delete pack; };

	/** Handle OF set-config message. To be overwritten by derived class.
	 *
	 * Called upon reception of a SET-CONFIG.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack SET-CONFIG.message packet received from controller.
	 */
	virtual void
	handle_set_config(cofctl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF queue-get-config request. To be overwritten by derived class.
 	 *
	 * Called upon reception of a QUEUE-GET-CONFIG.reply from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a QUEUE-GET-CONFIG.reply was received
	 * @param pack QUEUE-GET-CONFIG.reply packet received from datapath
	 */
	virtual void
	handle_queue_get_config_request(cofctl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF queue-get-config reply. To be overwritten by derived class.
 	 *
	 * Called upon reception of a QUEUE-GET-CONFIG.reply from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a QUEUE-GET-CONFIG.reply was received
	 * @param pack QUEUE-GET-CONFIG.reply packet received from datapath
	 */
	virtual void
	handle_queue_get_config_reply(cofdpt *sw, cofpacket *pack) { delete pack; };

	/** Handle OF experimenter message. To be overwritten by derived class.
	 *
	 * Called upon reception of a VENDOR.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack VENDOR.message packet received from controller.
	 */
	virtual void
	handle_experimenter_message(cofctl *ofctrl, cofpacket *pack);

	/** Handle OF experimenter message. To be overwritten by derived class.
	 *
	 * Called upon reception of a VENDOR.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a VENDOR.message was received
	 * @param pack VENDOR.message packet received from datapath
	 */
	virtual void
	handle_experimenter_message(cofdpt *sw, cofpacket *pack) { delete pack; };

	/** Handle new dpath
	 *
	 * Called upon creation of a new cofswitch instance.
	 *
	 * @param sw new cofswitch instance
	 */
	virtual void
	handle_dpath_open(cofdpt *sw) {};

	/** Handle close event on dpath
	 *
	 * Called upon deletion of a cofswitch instance
	 *
	 * @param sw cofswitch instance to be deleted
	 */
	virtual void
	handle_dpath_close(cofdpt *sw) {};

	/** Handle new ctrl
	 *
	 * Called upon creation of a new cofctrl instance.
	 *
	 * @param ctrl new cofctrl instance
	 */
	virtual void
	handle_ctrl_open(cofctl *ctrl) {};

	/** Handle close event on ctrl
	 *
	 * Called upon deletion of a cofctrl instance
	 *
	 * @param ctrl cofctrl instance to be deleted
	 */
	virtual void
	handle_ctrl_close(cofctl *ctrl) {};

	/** Handle timeout for GET-FSP request
	 *
	 */
	virtual void
	handle_get_fsp_reply_timeout(cofdpt *sw) {};

	/** Handle OF role request. To be overwritten by derived class.
	 *
	 * Called upon reception of a ROLE.request from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack ROLE.request packet received from controller.
	 */
	virtual void
	handle_role_request(cofctl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF role reply. To be overwritten by derived class.
	 *
	 * Called upon reception of a ROLE.reply from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a ROLE.reply was received
	 * @param pack ROLE.reply packet received from datapath
	 */
	virtual void
	handle_role_reply(cofdpt *sw, cofpacket *pack) { delete pack; };

	/** Handle OF role reply timeout. To be overwritten by derived class.
	 *
	 * Called upon expiration of TIMER_FE_SEND_ROLE_REPLY.
	 *
	 * @param sw cotswitch instance from whom a ROLE.reply was expected.
	 */
	virtual void
	handle_role_reply_timeout(cofdpt *sw) {};


protected:	// overloaded from ciosrv

	/** Handle timeout events. This method is overwritten from ciosrv.
	 *
	 * @param opaque expired timer type
	 */
	virtual void
	handle_timeout(
		int opaque);


public: // miscellaneous methods

	/** dump info string
	 */
	virtual const char*
	c_str();


	/** enable/disable namespace support
	 *
	 */
	void
	nsp_enable(bool enable = true);







public:

	// allow class cofswitch access to these methods
	friend class cofdpt;
	// allow class cofctrl access to these methods
	friend class cofctl;
	// allow class cadaptor access to these methods
	friend class chandler;

	friend class bcm_port;
	// allow class cgttable access to error method
	friend class cgttable;

	/* These methods may be called by a derived class to send
	 * a specific OF packet.
	 *
	 */

	// HELLO messages
	//

	/** Send a OF HELLO.message to data path.
	 *
	 */
	virtual void
	send_hello_message(
			cofdpt *dpt,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);

	/** Send a OF HELLO.message to controller.
	 *
	 */
	virtual void
	send_hello_message(
			cofctl *ctl,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);

	// ECHO request/reply
	//

	/**
	 *
	 */
	virtual void
	send_echo_request(
			cofdpt *dpt,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);


	/**
	 *
	 */
	virtual void
	send_echo_reply(
			cofdpt *dpt,
			uint32_t xid,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);


	/**
	 *
	 */
	virtual void
	send_echo_request(
			cofctl *ctl,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);


	/**
	 *
	 */
	virtual void
	send_echo_reply(
			cofctl *ctl,
			uint32_t xid,
			uint8_t *body = (uint8_t*)0, size_t bodylen = 0);


	// FEATURES request/reply
	//

	/** Send OF FEATURES.request to data path entity.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 */
	virtual void
	send_features_request(
		cofdpt *sw);

	/** Send OF FEATURES.reply to controlling entity.
	 *
	 */
	virtual void
	send_features_reply(
			cofctl *ofctrl,
			uint32_t xid,
			uint64_t dpid,
			uint32_t n_buffers,
			uint8_t n_tables,
			uint32_t capabilities,
			uint8_t *ports = (uint8_t*)0,
			size_t portslen = 0);

	// GET-CONFIG request/reply
	//

	/** Send OF GET-CONFIG.request to data path entity.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 */
	virtual void
	send_get_config_request(
		cofdpt *sw);

	/** Send OF GET-CONFIG.reply to controlling entity.
	 *
	 */
	virtual void
	send_get_config_reply(
			cofctl *ofctrl,
			uint32_t xid,
			uint16_t flags,
			uint16_t miss_send_len);

	// STATS request/reply
	//

	/** Send OF STATS.request to data path entity.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 * @param type One of the OFPST_* constants
	 * @param flags OFPSF_REQ_* flags
	 * @param body Body of a STATS request
	 * @param bodylen length of STATS request body
	 */
	virtual uint32_t
	send_stats_request(
		cofdpt *sw,
		uint16_t type,
		uint16_t flags,
		uint8_t *body = NULL,
		size_t bodylen = 0);

private:

	/** Helper method for handling DESCription STATS.requests.
	 * Only used within crofbase internally.
	 */
	void
	send_stats_reply_local();

public:

	/** Send OF STATS.reply to controlling entity.
	 *
	 * @param xid transaction ID used in STATS request
	 * @param stats_type One of the OFPST_* constants
	 * @param body Body of a STATS reply
	 * @param bodylen length of STATS reply body
	 * @param more flag if multiple STATS replies will be sent
	 */
	virtual void
	send_stats_reply(
		cofctl *ofctrl,
		uint32_t xid,
		uint16_t stats_type,
		uint8_t *body = NULL,
		size_t bodylen = 0,
		bool more = false);

	// PACKET-OUT message
	//

	/** Send OF PACKET-OUT.message to data path entity.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 * @param buffer_id buffer ID assigned by datapath (-1 if none) in host byte order
	 * @param in_port Packet’s input port (OFPP_NONE if none) in host byte order
	 * @param acvec vector containing cofaction instances
	 * @param data optional packet data to be sent out
	 * @param datalen
	 */
	virtual void
	send_packet_out_message(
		cofdpt *sw,
		uint32_t buffer_id,
		uint32_t in_port,
		cofaclist& aclist,
		uint8_t *data = NULL,
		size_t datalen = 0);


	// PACKET-IN message
	//

	/** Send OF PACKET-IN.message to controlling entity.
	 *
	 * @param buffer_id buffer ID assigned by datapath in host byte order
	 * @param total_len Full length of frame
	 * @param in_port Port on which frame was received
	 * @param reason Reason packet is being sent (one of OFPR_*)
	 * @param data Ethernet frame
	 * @param datalen Ethernet frame length
	 */
	virtual void
	send_packet_in_message(
		uint32_t buffer_id,
		uint16_t total_len,
		uint8_t reason,
		uint8_t table_id,
		cofmatch &match,
		uint8_t *data,
		size_t datalen) throw(eRofBaseNoCtrl);

	// BARRIER request/reply
	//

	/** Send OF BARRIER.request to data path entity.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 */
	virtual uint32_t
	send_barrier_request(
		cofdpt *sw);

	/** Send OF BARRIER.reply to controlling entity.
	 *
	 */
	virtual void
	send_barrier_reply(
			cofctl *ofctrl,
			uint32_t xid);

	// ERROR message
	//

	/** Send OF ERROR.message to controlling entity.
	 *
	 * @param type One of OFPET_*
	 * @param code error code
	 * @param data failed request
	 * @param datalen length of failed request
	 */
	virtual void
	send_error_message(
		cofctl *ofctrl,
		uint32_t xid,
		uint16_t type,
		uint16_t code,
		uint8_t* data = NULL,
		size_t datalen = 0);

	/** Send OF ERROR.message to datapath entity.
	 *
	 * @param type One of OFPET_*
	 * @param code error code
	 * @param data failed request
	 * @param datalen length of failed request
	 */
	virtual void
	send_error_message(
		cofdpt *dpt,
		uint32_t xid,
		uint16_t type,
		uint16_t code,
		uint8_t* data = NULL,
		size_t datalen = 0);

	// FLOW-MOD message
	//

	/** Send OF FLOW-MOD.message to data path entity.
	 *
	 * All values in host byte order.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 * @param ofmatch An instance of class cofmatch holding the matching fields
	 * @param cookie Opaque controller-issued identifier
	 * @param command One of OFPFC_*
	 * @param idle_timeout Idle time before discarding (seconds)
	 * @param hard_timeout Max time before discarding (seconds)
	 * @param priority Priority level of flow entry
	 * @param buffer_id Buffered packet to apply to (or -1)
	 * @param out_port For OFPFC_DELETE* commands, require matching entries to include this as an output port. A value of OFPP_NONE indicates no restriction
	 * @param flags One of OFPFF_*
	 * @param acvec Vector of cofaction instances
	 */
	virtual void
	send_flow_mod_message(
		cofdpt *sw,
		cofmatch& ofmatch,
		uint64_t cookie,
		uint64_t cookie_mask,
		uint8_t table_id,
		uint8_t command,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint16_t priority,
		uint32_t buffer_id,
		uint32_t out_port,
		uint32_t out_group,
		uint16_t flags,
		cofinlist& inlist);

	virtual void
	send_flow_mod_message(
			cofdpt *sw,
			cflowentry& flowentry);

	// GROUP-MOD message
	//

	/** Send OF GROUP-MOD.message to data path entity.
	 *
	 * All values in host byte order.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 * @param ofmatch An instance of class cofmatch holding the matching fields
	 * @param cookie Opaque controller-issued identifier
	 * @param command One of OFPFC_*
	 * @param idle_timeout Idle time before discarding (seconds)
	 * @param hard_timeout Max time before discarding (seconds)
	 * @param priority Priority level of flow entry
	 * @param buffer_id Buffered packet to apply to (or -1)
	 * @param out_port For OFPFC_DELETE* commands, require matching entries to include this as an output port. A value of OFPP_NONE indicates no restriction
	 * @param flags One of OFPFF_*
	 * @param acvec Vector of cofaction instances
	 */
#if 0
	void
	send_group_mod_message(
		cofdpt *sw,
		uint16_t command,
		uint8_t type,
		uint32_t group_id,
		std::vector<cofbucket*>& buckets);
#endif

	virtual void
	send_group_mod_message(
			cofdpt *sw,
			cgroupentry& groupentry);

	// TABLE-MOD message
	//

	/** Send OF GROUP-MOD.message to data path entity.
	 *
	 * All values in host byte order.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 * @param table_id the table id according to OF1.1
	 * @param config table configuration, see openflow.h for OFPTC_TABLE_*
	 */
	virtual void
	send_table_mod_message(
		cofdpt *sw,
		uint8_t table_id,
		uint32_t config);

	// PORT-MOD message
	//

	/** Send OF PORT-MOD.message to data path entity.
	 *
	 * All values in host byte order.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 * @param ofmatch An instance of class cofmatch holding the matching fields
	 * @param port_no Number of port to modify
	 * @param hw_addr Memory area containing the ports hardware address
	 * @param hw_addr_len Length of memory area
	 * @param config Bitmap of OFPPC_* flags
	 * @param mask Bitmap of OFPPC_* flags to be changed
	 * @param advertise Bitmap of "ofp_port_features"s. Zero all bits to prevent any action taking place.
	 */
	virtual void
	send_port_mod_message(
		cofdpt *sw,
		uint32_t port_no,
		cmacaddr const& hwaddr,
		uint32_t config,
		uint32_t mask,
		uint32_t advertise);

	// FLOW-REMOVED message
	//

	/** Send OF FLOW-REMOVED.message to controlling entity.
	 *
	 * All values in host byte order.
	 *
	 * @param ofmatch An instance of class cofmatch holding the matching fields
	 * @param cookie Opaque controller-issued identifier
	 * @param priority Priority level of flow entry
	 * @param reason One of OFPRR_*
	 * @param duration_sec Time flow was alive in seconds
	 * @param duration_nsec Time flow was alive in nanoseconds beyond duration_sec
	 * @param idle_timeout Idle timeout from original flow mod
	 * @param packet_count number of packets handled by this flow mod
	 * @param byte_count number of bytes handled by this flow mod
	 */
	virtual void
	send_flow_removed_message(
		cofctl *ofctrl,
		cofmatch& ofmatch,
		uint64_t cookie,
		uint16_t priority,
		uint8_t reason,
		uint8_t table_id,
		uint32_t duration_sec,
		uint32_t duration_nsec,
		uint16_t idle_timeout,
		uint16_t hard_timeout,
		uint64_t packet_count,
		uint64_t byte_count);

	// PORT-STATUS message
	//

	/** Send OF PORT-STATUS.message to controlling entity.
	 *
	 * @param reason One of OFPPR_*
	 * @param port cofport instance that changed its status
	 */
	virtual void
	send_port_status_message(
		uint8_t reason,
		cofport *port);

	/** Send OF PORT-STATUS.message to controlling entity.
	 *
	 * @param reason One of OFPPR_*
	 * @param phy_port phy_port structure containing the ports current status
	 */
	virtual void
	send_port_status_message(
		uint8_t reason,
		struct ofp_port *phy_port);

	// SET-CONFIG message
	//

	/** Send OF SET-CONFIG.message to data path entity.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 * @param flags OFPC_* flags
	 * @param miss_send_len Max bytes of new flow that datapath should send to the controller
	 */
	virtual void
	send_set_config_message(
		cofdpt *sw,
		uint16_t flags,
		uint16_t miss_send_len);

	// QUEUE-GET-CONFIG request/reply
	//

	/** Send OF QUEUE-GET-CONFIG.request to data path entity.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 * @param port Port to be queried. Should refer to a valid physical port (i.e. < OFPP_MAX)
	 */
	virtual void
	send_queue_get_config_request(
		cofdpt *sw,
		uint32_t port);

	/** Send OF QUEUE-GET-CONFIG.reply to controlling entity.
	 *
	 */
	virtual void
	send_queue_get_config_reply(
			cofctl *ctl,
			uint32_t xid,
			uint32_t portno);

	/** Send OF experimenter message to data path
	 *
	 */
	virtual void
	send_experimenter_message(
			cofdpt *sw,
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body = NULL,
			size_t bodylen = 0);

	/** Send OF experimenter message to controller
	 *
	 */
	virtual void
	send_experimenter_message(
			cofctl *ctrl,
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body = NULL,
			size_t bodylen = 0);


	// ROLE request/reply
	//

	/** Send OF ROLE.request to data path entity.
	 *
	 * @param sw cofswitch instance representing the data path to be addressed
	 */
	virtual void
	send_role_request(
		cofdpt *sw,
		uint32_t role,
		uint64_t generation_id);

	/** Send OF ROLE.reply to controlling entity.
	 *
	 */
	virtual void
	send_role_reply(
			cofctl *ofctrl,
			uint32_t xid,
			uint32_t role,
			uint64_t generation_id);




private: // methods

	// FEATURES request
	//

	/** receive (dequeue) feature requests
	 * and call overloaded handle_feature_request() method
	 */
	void
	recv_features_request(cofctl *ctl, cofpacket *pack);

	// GET-CONFIG request
	//

	/**
	 *
	 */
	void
	recv_get_config_request(cofctl *ctl, cofpacket *pack);

	// STATS request
	//

	/** receive (dequeue) stats requests
	 * and call handle_stats_request() method
	 */
	void
	recv_stats_request(cofctl *ctl, cofpacket *pack);

	// PACKET-OUT message
	//

	/** receive (dequeue) packet-out messages
	 * and call overloaded handle_packet_out() method
	 */
	void
	recv_packet_out(cofctl *ctl, cofpacket *pack);

	// PACKET-IN message
	//

	/** receive (dequeue) packet-in messages
	 * and call overloaded handle_packet_in() method
	 */
	void
	recv_packet_in(cofdpt *dpt, cofpacket *pack);

	// ERROR message
	//

	/** receive (dequeue) error messages
	 * and call overloaded handle_error() method
	 */
	void
	recv_error(cofdpt *dpt, cofpacket *pack);

	// FLOW-MOD message
	//

	/** receive (dequeue) flow-mod messages
	 * and call overloaded handle_flow_mod() method
	 */
	void
	recv_flow_mod(cofctl *ctl, cofpacket *pack);

	// GROUP-MOD message
	//

	/** receive (dequeue) group-mod messages
	 * and call overloaded handle_group_mod() method
	 */
	void
	recv_group_mod(cofctl *ctl, cofpacket *pack);

	// TABLE-MOD message
	//

	/** receive (dequeue) table-mod messages
	 * and call overloaded handle_table_mod() method
	 */
	void
	recv_table_mod(cofctl *ctl, cofpacket *pack);

	// PORT-MOD message
	//

	/** receive (dequeue) port-mod messages
	 * and call overloaded handle_port_mod() method
	 */
	void
	recv_port_mod(cofctl *ctl, cofpacket *pack);

	// FLOW-REMOVED message
	//

	/** receive (dequeue) flow-removed messages
	 * and call overloaded handle_flow_removed() method
	 */
	void
	recv_flow_removed(cofdpt *dpt, cofpacket *pack);

	// PORT-STATUS message
	//

	/** receive (dequeue) port-status messages
	 * and call overloaded handle_port_status() method
	 */
	void
	recv_port_status(cofdpt *dpt, cofpacket *pack);

	// SET-CONFIG message
	//

	/** receive (dequeue) set-config messages
	 * and call overloaded handle_set_config() method
	 */
	void
	recv_set_config(cofctl *ctl, cofpacket *pack);

	// BARRIER request
	//

	/** receive (dequeue) barrier request
	 * and call overloaded handle_barrier_request() method
	 */
	void
	recv_barrier_request(cofctl *ctl, cofpacket *pack);

	// EXPERIMENTER message
	//

	/** receive experimenter message
	 */
	void
	recv_experimenter_message(cofctl *ctl, cofpacket *pack);

	/** receive experimenter message
	 */
	void
	recv_experimenter_message(cofdpt *dpt, cofpacket *pack);

	// ROLE-REQUEST message
	//

	/** receive (dequeue) role-request messages
	 * and call overloaded handle_role_request() method
	 */
	void
	recv_role_request(cofctl *ctl, cofpacket *pack);

	// ROLE-REPLY message
	//

	/** receive (dequeue) role-reply messages
	 * and call overloaded handle_role_reply() method
	 */
	void
	recv_role_reply(cofdpt *dpt, cofpacket *pack);

	// QUEUE-GET-CONFIG-REQUEST message
	//

	/** receive (dequeue) queue-get-config-request messages
	 * and call overloaded handle_queue_get_config_request() method
	 */
	void
	recv_queue_get_config_request(cofctl *ctl, cofpacket *pack);

	// QUEUE-GET-CONFIG-REPLY message
	//

	/** receive (dequeue) queue-get-config-reply messages
	 * and call overloaded handle_queue_get_config_reply() method
	 */
	void
	recv_queue_get_config_reply(cofdpt *dpt, cofpacket *pack);


public:


	/**
	 *
	 */
	virtual void
	handle_accepted(
			csocket *socket,
			int newsd,
			caddress const& ra);


	/**
	 *
	 */
	virtual void
	handle_connected(
			csocket *socket,
			int sd);


	/**
	 *
	 */
	virtual void
	handle_connect_refused(
			csocket *socket,
			int sd);


	/**
	 *
	 */
	virtual void
	handle_read(
			csocket *socket,
			int sd);


	/**
	 *
	 */
	virtual void
	handle_closed(
			csocket *socket,
			int sd);


public:

	// COFSWITCH related methods
	//

	/** find cofswitch instance
	 */
	cofdpt&
	dpath_find(
		uint64_t dpid) throw (eRofBaseNotFound);

	cofdpt&
	dpath_find(
		std::string s_dpid) throw (eRofBaseNotFound);

	cofdpt&
	dpath_find(
		cmacaddr dl_dpid) throw (eRofBaseNotFound);


	/** find cofswitch instance
	 */
	cofdpt*
	ofswitch_find(
			cofdpt* dpt) throw (eRofBaseNotFound);


	/** find cofswitch instance
	 */
	cofdpt*
	ofswitch_exists(
			const cofdpt *ofswitch) throw (eRofBaseNotFound);

	// COFCTRL related methods
	//


	/** find cofctrl instance
	 */
	cofctl*
	ofctrl_find(
			cofctl* entity) throw (eRofBaseNotFound);


	/** find cofctrl instance
	 */
	cofctl*
	ofctrl_exists(
			const cofctl *ofctrl) throw (eRofBaseNotFound);


	/** for use by cofdpt
	 *
	 */
	void
	handle_dpt_open(cofdpt *dpt);


	/** for use by cofdpt
	 *
	 */
	void
	handle_dpt_close(cofdpt *dpt);


	/** for use by cofctl
	 *
	 */
	void
	handle_ctl_open(cofctl *ctl);


	/** for use by cofctl
	 *
	 */
	void
	handle_ctl_close(cofctl *ctl);


protected:


	/** add pending request to transaction queue
	 * - allocates new xid not in xid_used
	 * - adds xid to xid_used
	 * - adds pair(type, xid) to ta_pending_requests
	 */
	uint32_t
	ta_add_request(
			uint8_t type);


	/** remove pending request from transaction queue
	 */
	void
	ta_rem_request(
			uint32_t xid);


	/** return boolean flag for pending request of type x
	 */
	bool
	ta_pending(
			uint32_t xid,
			uint8_t type);


	/** return new xid for asynchronous calls
	 * - adds xid to xid_used
	 * - does not add xid to ta_pending_requests
	 */
	uint32_t
	ta_new_async_xid();


	/** validate incoming reply for transaction
	 * checks for existing type and associated xid
	 * removes request from ta_pending_reqs, if found
	 */
	bool
	ta_validate(
			uint32_t xid,
			uint8_t type) throw (eRofBaseXidInval);


	/** validate a cofpacket, calls ta_validate(xid, type)
	 */
	bool
	ta_validate(
			cofpacket *pack);


	/** returns true if a xid is used by a pending
	 * transaction
	 */
	bool
	ta_active_xid(
			uint32_t xid);


	/*
	 * data structures
	 */

	std::map<uint32_t, uint8_t> 			ta_pending_reqs; 	// list of pending requests
	std::vector<uint32_t> 					xids_used;			// list of recently used xids
	size_t 									xid_used_max; 		// reusing xids: max number of currently blocked xid entries stored
	uint32_t 								xid_start; 			// start value xid
#define CPCP_DEFAULT_XID_USED_MAX       16


private: // methods for attaching/detaching other cofbase instances

	/** attach data path
	 */
	void
	dpath_attach(cofbase* dp);

	/** detach data path
	 */
	void
	dpath_detach(cofbase* dp);

	/** attach controlling entity
	 */
	void
	ctrl_attach(cofbase* dp) throw (eRofBaseFspSupportDisabled);

	/** detach controlling entity
	 */
	void
	ctrl_detach(cofbase* dp);




};

}; // end of namespace

#endif


