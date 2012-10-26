/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFWDELEM_H
#define CFWDELEM_H 1

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
#include "cfwdtable.h"
#include "cvastring.h"
#include "cfibentry.h"
#include "croflexp.h"
#include "cphyport.h"
#include "cofbase.h"

//#include "rofl/experimental/crib.h"
#include "rofl/platform/unix/crandom.h"

#include "openflow/cofpacket.h"
#include "openflow/cofport.h"
#include "openflow/cofdpath.h"
#include "openflow/cofctrl.h"
#include "openflow/cofrpc.h"
#include "openflow/cofinst.h"
#include "openflow/cofinlist.h"
#include "openflow/cofaction.h"
#include "openflow/cofaclist.h"
#include "openflow/cofmatch.h"
#include "openflow/cfttable.h"
#include "openflow/cgttable.h"
#include "openflow/cftentry.h"
#include "openflow/cflowentry.h"
#include "openflow/cgroupentry.h"
#include "openflow/cofstats.h"
#include "openflow/extensions/cfsptable.h"
#include "openflow/cfttable.h"
#include "openflow/cgttable.h"
#include "openflow/cftentry.h"
#include "openflow/cgtentry.h"




/* error classes */
class eFwdElemBase			: public cerror {};   // base error class cfwdelem
class eFwdElemIsBusy 		: public eFwdElemBase {}; // this FwdElem is already controlled
class eFwdElemNotImpl 		: public eFwdElemBase {}; // this FwdElem's method is not implemented
class eFwdElemNoCtrl 		: public eFwdElemBase {}; // no controlling entity attached to this FwdElem
class eFwdElemNotFound 		: public eFwdElemBase {}; // internal entity not found
class eFwdElemInval			: public eFwdElemBase {}; // invalid parameter (e.g. invalid packet type)
class eFwdElemNotAttached 	: public eFwdElemBase {}; // received command from entity being not attached
class eFwdElemNoRequest 	: public eFwdElemBase {}; // no request packet found for session
class eFwdElemXidInval	 	: public eFwdElemBase {}; // invalid xid in session exchange
class eFwdElemExists		: public eFwdElemBase {}; // fwdelem with either this dpid or dpname already exists
class eFwdElemOFportNotFound : public eFwdElemBase {}; // cofport instance not found
class eFwdElemTableNotFound : public eFwdElemBase {}; // flow-table not found (e.g. unknown table_id in flow_mod)
class eFwdElemGotoTableNotFound : public eFwdElemBase {}; // table-id specified in OFPIT_GOTO_TABLE invalid
class eFwdElemFspSupportDisabled : public eFwdElemBase {};





/**
 * Forwarding element base class.
 * This class implements all core functionality for a forwarding element.
 * Classes derived from cfwdelem() are stackable, as the base class
 * uses the recursiveness of the OpenFlow API.
 * The base class acts in proxying mode, i.e. it acts as a datapath element
 * to higher layers and is able to control an arbitrary number of lower
 * layer data path elements as controlling entity.
 *
 * cfwdelem() provides the following core objects for managing lower layer entities:
 * - ofswitch_list is a set of cofswitch() entities for managing registered lower layer data path instances
 * - fe_up_queue is a std::map that holds all received OpenFlow packets from a data path entity
 *
 * cfwdelem() provides the following core objects for emulating data path functionality
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
class cfwdelem :
	public cfibentry_owner,
	public cftentry_owner,
	public cgtentry_owner,
<<<<<<< HEAD
	public crofbase
{
private:

	std::string						info;

=======
	public cofbase
{
private:


	std::string						info;


>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64
protected: // data structures

	// controlling and controlled entities
	//

	std::map<uint8_t, cfttable*> 	flow_tables; 	// OF1.1 forwarding tables for this emulated switch instance (layer-(n), not layer-(n-1)!)
	cgttable 						group_table; 	// OF1.1 group table
	cfwdtable 						fwdtable; 		// forwarding entries table (MAC learning)

public:

	std::set<cfibentry*> 			fib_table;		// FIB

	friend class cfttable;


protected:

	enum fwdelem_timer_t {
<<<<<<< HEAD
		TIMER_FE_BASE = (0x0020 << 16),
	};

	enum fwdelem_state_t {
		CFWD_STATE_DISCONNECTED = 1,
		CFWD_STATE_CONNECTING = 2,
		CFWD_STATE_CONNECTED = 3,
		CFWD_STATE_FAIL_SECURE_MODE = 4,
		CFWD_STATE_FAIL_STANDALONE_MODE = 5,
	};

	enum fwdelem_rpc_t { // for cofrpc *rpc[2]; (see below)
		RPC_CTL = 0,
		RPC_DPT = 1,
=======
		TIMER_FE_BASE = (0x002b << 16),
>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64
	};


public:

	enum fwdelem_stats_t {
		hw_byte_stats,
		hw_packet_stats
	};


<<<<<<< HEAD
=======
public: // static methods and data structures



>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64

public: // constructor + destructor

	/** Constructor.
	 *
	 * A cfwdelem instance must have a unique dpname and dpid.
	 * The constructor verifies dpname's and dpid's uniqueness and throws
	 * an exception of type eFwdElemExists if these values are already occupied.
	 *
	 * @param dpname datapath name, default is "dp0"
	 * @param dpid datapath ID, if no dpid is specified, a random uint64 number is generated
	 * @throw eFwdElemExists
	 */
	cfwdelem(std::string dpname = std::string("fwd0"),
			uint64_t dpid = crandom(8).uint64(),
			uint8_t n_tables = DEFAULT_FE_TABLES_NUM,
			uint32_t n_buffers = DEFAULT_FE_BUFFER_SIZE,
			caddress const& rpc_ctl_addr = caddress(AF_INET, "0.0.0.0", 6643),
			caddress const& rpc_dpt_addr = caddress(AF_INET, "0.0.0.0", 6633)) throw (eFwdElemExists);

	/** Destructor.
	 *
	 * Removes all pending requests from queues fe_down_queue[] and fe_up_queues[]
	 * and removes this instance from set cfwdelem::fwdelems.
	 */
	virtual
	~cfwdelem();



	/** reset all flow and group tables
	 */
	void
	tables_reset();




protected:

<<<<<<< HEAD
	/*
	 * The following methods should be overwritten by a derived class
	 * in order to get reception notifications for the various OF
	 * packets. While cfwdelem handles most of the lower layer details,
	 * a derived class must provide higher layer functionality.
	 */
=======


	/** Handle OF stats request. NOT to be overwritten by derived class.
	 *
	 * Called upon reception of a STATS.request from the controlling entity.
	 *
	 * @param pack STATS.request packet received from controller.
	 */
	void
	handle_stats_request(cofctrl *ofctrl, cofpacket *pack);
>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64


	/**
	 *
	 */
	virtual void
	handle_desc_stats_request(cofctrl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_table_stats_request(cofctrl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_port_stats_request(cofctrl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_flow_stats_request(cofctrl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_aggregate_stats_request(cofctrl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_queue_stats_request(cofctrl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_group_stats_request(cofctrl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_group_desc_stats_request(cofctrl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_group_features_stats_request(cofctrl *ofctrl, cofpacket *pack);


	/**
	 *
	 */
	virtual void
	handle_experimenter_stats_request(cofctrl *ofctrl, cofpacket *pack);


<<<<<<< HEAD
	/** Handle OF stats reply. To be overwritten by derived class.
	 *
	 * Called upon reception of a STATS.reply from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a STATS.reply was received
	 * @param pack STATS.reply packet received from datapath
	 */
	virtual void
	handle_stats_reply(cofdpath *sw, cofpacket *pack) { delete pack; };

	/** Handle OF stats reply timeout. To be overwritten by derived class.
	 *
	 * Called upon expiration of TIMER_FE_SEND_STATS_REPLY.
	 *
	 * @param sw cotswitch instance from whom a GET-CONFIG.reply was expected.
	 */
	virtual void
	handle_stats_reply_timeout(cofdpath *sw, uint32_t xid) {};

	/** Handle OF flow-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a FLOW-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack FLOW-MOD.message packet received from controller.
	 */
	virtual void
	handle_flow_mod(cofctrl *ofctrl, cofpacket *pack, cftentry *fte) { delete pack; };

	/** Handle OF group-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a GROUP-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack GROUP-MOD.message packet received from controller.
	 */
	virtual void
	handle_group_mod(cofctrl *ofctrl, cofpacket *pack, cgtentry *gte) { delete pack; };

	/** Handle OF table-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a TABLE-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack TABLE-MOD.message packet received from controller.
	 */
	virtual void
	handle_table_mod(cofctrl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF flow-removed message. To be overwritten by derived class.
	 *
	 * Called upon reception of a FLOW-REMOVED.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a FLOW-REMOVED.message was received
	 * @param pack FLOW-REMOVED.message packet received from datapath
	 */
	virtual void
	handle_flow_removed(cofdpath *sw, cofpacket *pack) { delete pack; };

	/** Handle OF set-config message. To be overwritten by derived class.
	 *
	 * Called upon reception of a SET-CONFIG.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack SET-CONFIG.message packet received from controller.
	 */
	virtual void
	handle_set_config(cofctrl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF queue-get-config reply. To be overwritten by derived class.
 	 *
	 * Called upon reception of a QUEUE-GET-CONFIG.reply from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a QUEUE-GET-CONFIG.reply was received
	 * @param pack QUEUE-GET-CONFIG.reply packet received from datapath
	 */
	virtual void
	handle_queue_get_config_reply(cofdpath *sw, cofpacket *pack) { delete pack; };
=======


	/** Handle new dpath
	 *
	 * Called upon creation of a new cofswitch instance.
	 *
	 * @param sw new cofswitch instance
	 */
	virtual void
	handle_dpath_open(cofdpath *sw) {};

	/** Handle close event on dpath
	 *
	 * Called upon deletion of a cofswitch instance
	 *
	 * @param sw cofswitch instance to be deleted
	 */
	virtual void
	handle_dpath_close(cofdpath *sw) {};

	/** Handle new ctrl
	 *
	 * Called upon creation of a new cofctrl instance.
	 *
	 * @param ctrl new cofctrl instance
	 */
	virtual void
	handle_ctrl_open(cofctrl *ctrl) {};

	/** Handle close event on ctrl
	 *
	 * Called upon deletion of a cofctrl instance
	 *
	 * @param ctrl cofctrl instance to be deleted
	 */
	virtual void
	handle_ctrl_close(cofctrl *ctrl) {};

	/** Handle timeout for GET-FSP request
	 *
	 */
	virtual void
	handle_get_fsp_reply_timeout(cofdpath *sw) {};

	/** Handle OF role request. To be overwritten by derived class.
	 *
	 * Called upon reception of a ROLE.request from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack ROLE.request packet received from controller.
	 */
	virtual void
	handle_role_request(cofctrl *ofctrl, cofpacket *pack) { delete pack; };

	/** Handle OF role reply. To be overwritten by derived class.
	 *
	 * Called upon reception of a ROLE.reply from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a ROLE.reply was received
	 * @param pack ROLE.reply packet received from datapath
	 */
	virtual void
	handle_role_reply(cofdpath *sw, cofpacket *pack) { delete pack; };

	/** Handle OF role reply timeout. To be overwritten by derived class.
	 *
	 * Called upon expiration of TIMER_FE_SEND_ROLE_REPLY.
	 *
	 * @param sw cotswitch instance from whom a ROLE.reply was expected.
	 */
	virtual void
	handle_role_reply_timeout(cofdpath *sw) {};
>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64

	/**
	 * @name	flow_mod_add
	 * @brief	called whenever a new flow mod entry is created
	 *
	 * This method indicates creation of a new flow-mod entry.
	 *
	 * @param ftable pointer to flow-table where the new flow table entry was added
	 * @param fte pointer to new flow table entry
	 */
	virtual void
	flow_mod_add(cfttable *ftable, cftentry *fte) {};

	/**
	 * @name	flow_mod_modify
	 * @brief	called whenever an existing flow mod entry is modified
	 *
	 * This method indicates modification of an existing flow-mod entry.
	 *
	 * @param ftable pointer to flow-table where flow table entry was updated
	 * @param fte pointer to modified flow table entry
	 */
	virtual void
	flow_mod_modify(cfttable *ftable, cftentry *fte) {};

	/**
	 * @name	flow_mod_delete
	 * @brief	called whenever an existing flow mod entry is deleted
	 *
	 * This method indicates removal of an existing flow-mod entry.
	 * The fte instance referred to by its pointer will be deleted
	 * after this method block has been left, so do not rely on fte
	 * afterwards.
	 *
	 * @param ftable pointer to flow-table where flow table entry was deleted
	 * @param fte pointer to deleted flow table entry
	 */
	virtual void
	flow_mod_delete(cfttable *ftable, cftentry *fte) {};


protected:	// overloaded from ciosrv

	/** Handle timeout events. This method is overwritten from ciosrv.
	 *
	 * @param opaque expired timer type
	 */
	virtual void
	handle_timeout(
		int opaque);



public: // specific hardware support

	/** Create and return a new cftentry (flow-table entry) instance on heap.
	 * To be overwritten by derived class.
	 *
	 * Derived classes with specific hardware devices may require cftentry instances
	 * with extended abilities. This method is called to create a derived instance from #
	 * base class cftentry. The derived cftentry class should include hardware specific
	 * adaptations.
	 *
	 * The default behavior returns an instance of the base class cftentry with no hardware support.
	 *
	 * @param *owner The owner is notified when a flow table entry expires and a notification
	 * must be sent to a controlling entity.
	 * @param *flow_table The flow-table instance (cfttable) that holds this cftentry instance.
	 * The cftentry instance removes itself from cfttable once its destructor gets called.
	 * @param *flow_mod The OF flow-mod structure to be stored in the cftentry instance.
	 * @return A derived or the base version of cftentry.
	 */
	virtual
	cftentry*
	hw_create_cftentry(
		cftentry_owner *owner,
		std::set<cftentry*> *flow_table,
		cofpacket *pack);

public: // overloaded from cftentry_owner

	/** called upon timer expiration
	 */
	virtual void
	ftentry_timeout(
			cftentry *fte, uint16_t timeout);

public: // overloaded from cgtentry_owner

	/** called upon timer expiration
	 */
	virtual void
	gtentry_timeout(
			cgtentry *gte, uint16_t timeout);

public: // overloaded from cfibentry_owner

	/** called upon timer expiration
	 */
	virtual void
	fibentry_timeout(cfibentry *fibentry);

public: // miscellaneous methods

	/** dump info string
	 */
	virtual const char*
	c_str();

	/** get dpid
	 */
	uint64_t
	getdpid() { return dpid; };

	/**
	 *
	 */
	cfttable&
	get_fttable(uint8_t tableid) throw (eFwdElemNotFound);

	/**
	 *
	 */
	cfttable&
	get_succ_fttable(uint8_t tableid) throw (eFwdElemNotFound);



<<<<<<< HEAD



public: // FIB related methods

	/**
	 *
	 */
	uint32_t
	fib_table_find(uint64_t from, uint64_t to) throw (eFwdElemNotFound);
=======


public:

	// allow class cofswitch access to these methods
	friend class cofdpath;
	// allow class cofctrl access to these methods
	friend class cofctrl;
	// allow class cadaptor access to these methods
	friend class chandler;

	friend class bcm_port;
	// allow class cgttable access to error method
	friend class cgttable;


public: // FIB related methods

	/**
	 *
	 */
	uint32_t
	fib_table_find(uint64_t from, uint64_t to) throw (eFwdElemNotFound);

>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64
};


#endif
