/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFIFACE_H
#define COFIFACE_H 1

#include <map>
#include <set>
#include <list>
#include <vector>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

#include "openflow12.h"
#include <endian.h>
#ifndef htobe16
#include "../endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "../cerror.h"
#include "rofl/platform/unix/crandom.h"
#include "rofl/platform/unix/csyslog.h"

#include "cofpacket.h"

/* error classes */
class eOFiface				: public cerror {};   // base error class cofbase
class eOFifaceIsBusy 		: public eOFiface {}; // this FwdElem is already controlled
class eOFifaceNotImpl 		: public eOFiface {}; // this FwdElem's method is not implemented
class eOFifaceNoCtrl 		: public eOFiface {}; // no controlling entity attached to this FwdElem
class eOFifaceNotFound 		: public eOFiface {}; // internal entity not found
class eOFifaceInval			: public eOFiface {}; // invalid parameter (e.g. invalid packet type)
class eOFifaceNotAttached 	: public eOFiface {}; // received command from entity being not attached
class eOFifaceNoRequest	 	: public eOFiface {}; // no request packet found for session
class eOFifaceXidInval	 	: public eOFiface {}; // invalid xid in session exchange
class eOFifaceExists			: public eOFiface {}; // fwdelem with either this dpid or dpname already exists


/**
 * Forwarding element base class.
 * This class implements all core functionality for a forwarding element.
 * Classes derived from cofbase() are stackable, as the base class
 * uses the recursiveness of the OpenFlow API.
 * The base class acts in proxying mode, i.e. it acts as a datapath element
 * to higher layers and is able to control an arbitrary number of lower
 * layer data path elements as controlling entity.
 *
 * cofbase() provides the following core objects for managing lower layer entities:
 * - ofswitch_list is a set of cofswitch() entities for managing registered lower layer data path instances
 * - fe_up_queue is a std::map that holds all received OpenFlow packets from a data path entity
 *
 * cofbase() provides the following core objects for emulating data path functionality
 * to controlling higher layer entities:
 * - fe_down_queue is a std::map that holds all received OpenFlow packets from a controlling
 * entity
 * - phy_ports is a set holding cofport() instances of emulated physical ports
 * - flow_table provides a flow table object for storing flow mod entries
 * - fwd_table provides basic MAC learning functionality for the emulated data path
 *
 * cofswitch() instances store all parameters for a lower layer attached datapath entity.
 * cofport() is used within cofswitch to store port details.
 * cofport() is also used for storing details of the emulated ports presented to higher layer
 * controlling entities.
 */
class cofiface :
	public virtual csyslog
{
private:

	static std::set<cofbase*>		cofbase_list;

public:

	/**
	 *
	 */
	static cofbase*
	cofbase_exists(cofbase* ofbase) throw (eOFbaseNotFound);


	/*
	 * default constants
	 */

	enum cofbase_const_t {
		FE_HELLO_ACTIVE = 10,
		FE_HELLO_BYE = 11,
	};

public: // constructor + destructor

	/** Constructor.
	 *
	 */
	cofiface();

	/** Destructor.
	 *
	 */
	virtual
	~cofiface();

#if 0
public: // methods for attaching/detaching other cofbase instances

	/** attach data path
	 */
	virtual void
	dpath_attach(cofiface* dp) = 0;

	/** detach data path
	 */
	virtual void
	dpath_detach(cofiface* dp) = 0;

	/** attach controlling entity
	 */
	virtual void
	ctrl_attach(cofiface* dp) throw (eOFifaceIsBusy) = 0;

	/** detach controlling entity
	 */
	virtual void
	ctrl_detach(cofiface* dp) = 0;
#endif

public: // methods

	/**
	 *
	 */
	void
	fe_up_command(
			cofiface *entity,
			cofpacket *pack) throw (eOFifaceInval);

	/**
	 *
	 */
	void
	fe_down_command(
			cofiface *entity,
			cofpacket *pack) throw (eOFifaceInval);


public:	// OpenFlow related methods for inter-cofbase instance communication

	/* The following methods are used for sending/receiving OF messages
	 * between stacked cofbase instances. Usually, they are not intended
	 * to be called from other objects. Two types exist:
	 * - fe_up_XXX(cofbase *entity, cofpacket *pack)
	 * - fw_down_XXX(cofbase *entity, cofpacket *pack)
	 * Convention:
	 * - fe_up_XXX() is called when a data path sends to a controlling entity.
	 * - fe_down_XXX() is called when a controller sends to a data path entity.
	 */

	// HELLO messages
	//

	/** Send a OF HELLO.message to data path.
	 *
	 */
	virtual void
	fe_down_hello_message(
			cofiface *entity,
			cofpacket *pack) = 0;

	/** Send a OF HELLO.message to data path.
	 *
	 */
	virtual void
	fe_up_hello_message(
			cofiface *entity,
			cofpacket *pack) = 0;

	// FEATURES request/reply
	//

	/** Send a OF FEATURES.request to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_features_request(
		cofiface *entity,
		cofpacket *pack) = 0;

	/** Send OF FEATURES.reply to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_features_reply(
		cofiface *entity,
		cofpacket *pack) = 0;

	// GET-CONFIG request/reply
	//

	/** Send a OF GET-CONFIG.request to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_get_config_request(
		cofiface *entity,
		cofpacket *pack) = 0;

	/** Send OF GET-CONFIG.reply to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_get_config_reply(
		cofiface *entity,
		cofpacket *pack) = 0;

	// STATS request/reply
	//

	/** Send a OF STATS.request to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_stats_request(
		cofiface *entity,
		cofpacket *pack) = 0;

	/** Send OF STATS.reply to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_stats_reply(
		cofiface *entity,
		cofpacket *pack) = 0;

	// PACKET-IN message
	//

	/** Send OF PACKET-IN.message to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_packet_in(
		cofiface *entity,
		cofpacket *pack) = 0;

	// PACKET-OUT message
	//

	/** Send a OF PACKET-OUT.message to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_packet_out(
		cofiface *entity,
		cofpacket *pack) = 0;

	// SET-CONFIG message
	//

	/** Send a OF SET-CONFIG.request to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_set_config_request(
		cofiface *entity,
		cofpacket *pack) = 0;

	// BARRIER request/reply
	//

	/** Send a OF BARRIER.request to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_barrier_request(
		cofiface *entity,
		cofpacket *pack) = 0;

	/** Send OF BARRIER.reply to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_barrier_reply(
		cofiface *entity,
		cofpacket *pack) = 0;

	// ERROR message
	//

	/** Send OF ERROR.message to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_error(
		cofiface *entity,
		cofpacket *pack) = 0;

	// FLOW-MOD message
	//

	/** Send a OF FLOW-MOD.message to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_flow_mod(
		cofiface *entity,
		cofpacket *pack) = 0;

	// GROUP-MOD message
	//

	/** Send a OF GROUP-MOD.message to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_group_mod(
		cofiface *entity,
		cofpacket *pack) = 0;

	// TABLE-MOD message
	//

	/** Send a OF TABLE-MOD.message to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_table_mod(
		cofiface *entity,
		cofpacket *pack) = 0;

	// PORT-MOD message
	//

	/** Send a OF PORT-MOD.message to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_port_mod(
		cofiface *entity,
		cofpacket *pack) = 0;

	// FLOW-REMOVED message
	//

	/** Send OF FLOW-REMOVED.message to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_flow_removed(
		cofiface *entity,
		cofpacket *pack) = 0;

	// PORT-STATUS message
	//

	/** Send OF PORT-STATUS.message to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_port_status(
		cofiface *entity,
		cofpacket *pack) = 0;

	// QUEUE-GET-CONFIG request/reply
	//

	/** Send a OF QUEUE-GET-CONFIG.request to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_queue_get_config_request(
		cofiface *entity,
		cofpacket *pack) = 0;

	/** Send OF QUEUE-GET-CONFIG.reply to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_queue_get_config_reply(
		cofiface *entity,
		cofpacket *pack) = 0;

	/** Send a OF VENDOR.message to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_experimenter_message(
		cofiface *entity,
		cofpacket *pack) = 0;

	/** Send OF VENDOR.message to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_experimenter_message(
		cofiface *entity,
		cofpacket *pack) = 0;

	// ROLE request/reply
	//

	/** Send a OF ROLE.request to data path.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemIsBusy
	 */
	virtual void
	fe_down_role_request(
		cofiface *entity,
		cofpacket *pack) = 0;

	/** Send OF ROLE.reply to controlling entity.
	 *
	 * @param entity cofbase instance that sent this packet
	 * @param pack OF packet sent
	 * @throw eFwdElemNotAttached
	 */
	virtual void
	fe_up_role_reply(
		cofiface *entity,
		cofpacket *pack) = 0;




public: // miscellaneous methods

	/** dump info string
	 */
	virtual const char*
	c_str() = 0;



protected: // data structures

	// queue for incoming packets rcvd from layer-(n+1) controlling entity
	std::map<int, std::list<cofpacket*> > fe_down_queue;
	// queue for incoming packets rcvd from layer-(n-1) controlled entities
	std::map<int, std::list<cofpacket*> > fe_up_queue;
	// info string
	char info[256];




protected: // openflow transactions related methods and data structures

	/** add pending request to transaction queue
	 * - allocates new xid not in xid_used
	 * - adds xid to xid_used
	 * - adds pair(type, xid) to ta_pending_requests
	 */
	uint32_t ta_add_request(uint8_t type);

	/** remove pending request from transaction queue
	 */
	void ta_rem_request(uint32_t xid);

	/** return boolean flag for pending request of type x
	 */
	bool ta_pending(uint32_t xid, uint8_t type);

	/** return new xid for asynchronous calls
	 * - adds xid to xid_used
	 * - does not add xid to ta_pending_requests
	 */
	uint32_t ta_new_async_xid();

	/** validate incoming reply for transaction
	 * checks for existing type and associated xid
	 * removes request from ta_pending_reqs, if found
	 */
	bool ta_validate(uint32_t xid, uint8_t type) throw (eOFifaceXidInval);

	/** validate a cofpacket, calls ta_validate(xid, type)
	 */
	bool ta_validate(cofpacket *pack) throw ();

	/** returns true if a xid is used by a pending
	 * transaction
	 */
	bool ta_active_xid(uint32_t xid);

	// list of pending requests
	std::map<uint32_t, uint8_t> ta_pending_reqs;
	// list of recently used xids
	std::vector<uint32_t> xids_used;
	// reusing xids: max number of currently blocked xid entries stored
	size_t xid_used_max;
#define CPCP_DEFAULT_XID_USED_MAX       16
	// start value xid
	uint32_t xid_start;

	// Ethernet header
	struct eth_hdr_t {
		uint8_t dst[OFP_ETH_ALEN];
		uint8_t src[OFP_ETH_ALEN];
		uint16_t dl_type;
	} __attribute__((packed));


};




#endif
