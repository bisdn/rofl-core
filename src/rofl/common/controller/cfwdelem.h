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
#include "../openflow/openflow12.h"
#include "../openflow/openflow_rofl.h"
#include <endian.h>
#include <string.h>
#include <time.h>

#ifndef htobe16
#include "../endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "../ciosrv.h"
#include "../cerror.h"
#include "cfwdtable.h"
#include "../cvastring.h"
#include "cfibentry.h"
#include "../croflexp.h"
#include "../cphyport.h"
#include "../crofbase.h"

//#include "rofl/experimental/crib.h"
#include "rofl/platform/unix/crandom.h"

#include "../openflow/cofpacket.h"
#include "../openflow/cofport.h"
#include "../openflow/cofinst.h"
#include "../openflow/cofinlist.h"
#include "../openflow/cofaction.h"
#include "../openflow/cofaclist.h"
#include "../openflow/cofmatch.h"
#include "cfttable.h"
#include "cgttable.h"
#include "cftentry.h"
#include "cgtentry.h"
#include "../openflow/cflowentry.h"
#include "../openflow/cgroupentry.h"
#include "../openflow/cofstats.h"
#include "../openflow/extensions/cfsptable.h"




/* error classes */
class eFwdElemBase					: public cerror {};   // base error class cfwdelem
class eFwdElemNotFound 				: public eFwdElemBase {}; // internal entity not found
class eFwdElemTableNotFound 		: public eFwdElemBase {};
class eFwdElemGotoTableNotFound		: public eFwdElemBase {};


class cfttable;


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
	public cfttable_owner,
	public cfibentry_owner,
	public cgtentry_owner,
	public crofbase
{
public: // static methods and data structures

	static std::set<cfwdelem*> fwdelems; /**< set of all registered fwdelems */

	/** Find crofbase with specified dpid.
	 *
	 * A static method for finding a crofbase entity by its datapath ID.
	 *
	 * @param dpid datapath ID
	 * @throw eFwdEleNotFound
	 */
	static cfwdelem*
	find_by_dpid(uint64_t dpid) throw (eFwdElemNotFound);

	/** Find crofbase with specified dpname.
	 *
	 * A static method for finding a crofbase entity by its datapath name.
	 *
	 * @param dpname datapath name, e.g. "dp0"
	 * @throw eRofBaseNotFound
	 */
	static cfwdelem*
	find_by_name(const std::string &dpname) throw (eFwdElemNotFound);


private:

	std::string						info;

protected: // data structures

	enum fwdelem_state_t {
		CFWD_STATE_DISCONNECTED = 1,
		CFWD_STATE_CONNECTING = 2,
		CFWD_STATE_CONNECTED = 3,
		CFWD_STATE_FAIL_SECURE_MODE = 4,
		CFWD_STATE_FAIL_STANDALONE_MODE = 5,
	};

	// controlling and controlled entities
	//

	std::string 					dpname;			// datapath device name
	uint64_t 						dpid; 			// datapath ID presented to higher layer
	uint32_t 						n_buffers; 		// number of buffer entries for queuing packets
	uint8_t  						n_tables; 		// number of tables
	uint32_t 						capabilities; 	// capabilities
	uint16_t 						flags; 			// config: flags
	uint16_t 						miss_send_len; 	// config: miss_send_len
	std::map<uint32_t, cphyport*> 	phy_ports; 		// ports that we present to the higher layer

	std::map<uint8_t, cfttable*> 	flow_tables; 	// OF1.1 forwarding tables for this emulated switch instance (layer-(n), not layer-(n-1)!)
	cgttable 						group_table; 	// OF1.1 group table
	cfwdtable 						fwdtable; 		// forwarding entries table (MAC learning)

public:

	/*
	 * default constants
	 */

	enum fwdelem_const_t {
		DEFAULT_FE_BUFFER_SIZE = 65536,
		DEFAULT_FE_MISS_SEND_LEN = 128,
		DEFAULT_FE_TABLES_NUM = 1,
	};

	enum fwdelem_stats_t {
		hw_byte_stats,
		hw_packet_stats
	};

	std::set<cfibentry*> 			fib_table;		// FIB
	std::string 					s_dpid;			// dpid as string

	friend class cfttable;


protected:

	enum fwdelem_timer_t {
		TIMER_FE_BASE = (0x0020 << 16),
	};





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
			caddress const& rpc_dpt_addr = caddress(AF_INET, "0.0.0.0", 6633));

	/** Destructor.
	 *
	 * Removes all pending requests from queues fe_down_queue[] and fe_up_queues[]
	 * and removes this instance from set cfwdelem::fwdelems.
	 */
	virtual
	~cfwdelem();


	/** returns this->dpid
	 *
	 */
	uint64_t
	get_dpid()
	{
		return dpid;
	};

	const char*
	get_s_dpid()
	{
		return s_dpid.c_str();
	};

	/**
	 * getter for this->dpname
	 * @return datapath name
	 */
	const std::string&
	get_dpname() const
	{
		return dpname;
	}


	/** get dpid
	 */
	uint64_t
	getdpid() { return dpid; };


	/**
	 *
	 */
	virtual void
	port_attach(
			std::string devname,
			uint32_t port_no);

	/**
	 *
	 */
	virtual void
	port_detach(
			uint32_t port_no);


	/** reset all flow and group tables
	 */
	void
	tables_reset();


protected:


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
	handle_features_request(cofctl *ofctrl, cofpacket *pack);


	/** Handle OF get-config request. To be overwritten by derived class.
	 *
	 * Called from within crofbase::fe_down_get_config_request().
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param ctrl cofdpath instance from whom the GET-CONFIG.request was received.
	 * @pack OF GET-CONFIG.request packet received from controller
	 */
	virtual void
	handle_get_config_request(cofctl *ctrl, cofpacket *pack);



protected:

	/*
	 * The following methods should be overwritten by a derived class
	 * in order to get reception notifications for the various OF
	 * packets. While cfwdelem handles most of the lower layer details,
	 * a derived class must provide higher layer functionality.
	 */


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


	/** Handle OF stats reply. To be overwritten by derived class.
	 *
	 * Called upon reception of a STATS.reply from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a STATS.reply was received
	 * @param pack STATS.reply packet received from datapath
	 */
	virtual void
	handle_stats_reply(cofdpt *sw, cofpacket *pack);

	/** Handle OF stats reply timeout. To be overwritten by derived class.
	 *
	 * Called upon expiration of TIMER_FE_SEND_STATS_REPLY.
	 *
	 * @param sw cotswitch instance from whom a GET-CONFIG.reply was expected.
	 */
	virtual void
	handle_stats_reply_timeout(cofdpt *sw, uint32_t xid) {};

	/** Handle OF flow-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a FLOW-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack FLOW-MOD.message packet received from controller.
	 */
	virtual void
	handle_flow_mod(cofctl *ofctrl, cofpacket *pack);

	/** Handle OF group-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a GROUP-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack GROUP-MOD.message packet received from controller.
	 */
	virtual void
	handle_group_mod(cofctl *ofctrl, cofpacket *pack);

	/** Handle OF table-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a TABLE-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack TABLE-MOD.message packet received from controller.
	 */
	virtual void
	handle_table_mod(cofctl *ofctrl, cofpacket *pack);

	/** Handle OF port-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a PORT-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack PORT-MOD.message packet received from controller.
	 */
	virtual void
	handle_port_mod(cofctl *ofctrl, cofpacket *pack);

	/** Handle OF flow-removed message. To be overwritten by derived class.
	 *
	 * Called upon reception of a FLOW-REMOVED.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a FLOW-REMOVED.message was received
	 * @param pack FLOW-REMOVED.message packet received from datapath
	 */
	virtual void
	handle_flow_removed(cofdpt *sw, cofpacket *pack);

	/** Handle OF set-config message. To be overwritten by derived class.
	 *
	 * Called upon reception of a SET-CONFIG.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack SET-CONFIG.message packet received from controller.
	 */
	virtual void
	handle_set_config(cofctl *ofctrl, cofpacket *pack);

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
	flow_mod_add(cofctl *ofctrl, cofpacket *pack, cfttable *ftable, cftentry *fte) {};

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
	flow_mod_modify(cofctl *ofctrl, cofpacket *pack, cfttable *ftable, cftentry *fte) {};

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
	flow_mod_delete(cofctl *ofctrl, cofpacket *pack, cfttable *ftable, cftentry *fte) {};

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
	group_mod_add(cofpacket *pack, cgtentry *gte) {};

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
	group_mod_modify(cofpacket *pack, cgtentry *gte) {};

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
	group_mod_delete(cofpacket *pack, cgtentry *gte) {};


protected:	// overloaded from ciosrv

	/** Handle timeout events. This method is overwritten from ciosrv.
	 *
	 * @param opaque expired timer type
	 */
	virtual void
	handle_timeout(
		int opaque);


public: // overloaded from cfttable_owner


	/**
	 *
	 */
	virtual void
	inc_group_reference_count(
			uint32_t groupid,
			cftentry *fte);


	/**
	 *
	 */
	virtual void
	dec_group_reference_count(
			uint32_t groupid,
			cftentry *fte);


	/**
	 *
	 */
	virtual void
	cftentry_idle_timeout(
			cftentry *fte);


	/**
	 *
	 */
	virtual void
	cftentry_hard_timeout(
			cftentry *fte);


	/**
	 *
	 */
	virtual void
	cftentry_delete(
			cftentry *fte);



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

	/** get free port number (with respect to this->phy_ports)
	 *
	 */
	virtual uint32_t
	phy_port_get_free_portno()
	throw (eFwdElemNotFound);






public: // FIB related methods

	/**
	 *
	 */
	uint32_t
	fib_table_find(uint64_t from, uint64_t to) throw (eFwdElemNotFound);


private: // data structures

	/** find cfwdelem in set cfwdelem::fwdelems by dpname (e.g. "ctl0")
	 *
	 */
	class cfwdelem_find_by_name : public std::unary_function<cfwdelem,bool> {
		const std::string &name;
	public:
		cfwdelem_find_by_name(const std::string& s_name) :
			name(s_name) {};
		bool operator() (const cfwdelem* fe) { return (name == fe->dpname);	};
	};

	/** find cfwdelem in set cfwdelem::fwdelems by dpid
	 *
	 */
	class cfwdelem_find_by_dpid : public std::unary_function<cfwdelem,bool> {
		uint64_t dpid;
	public:
		cfwdelem_find_by_dpid(uint64_t n_dpid) :
			dpid(n_dpid) {};
		bool operator() (const cfwdelem* fe) {
			return (dpid == fe->dpid);
		};
	};
};


#endif
