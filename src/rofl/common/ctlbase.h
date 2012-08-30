/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CTLBASE_H
#define CTLBASE_H 1

#include <string>

#include <rofl/common/cfwdelem.h>
#include <rofl/common/openflow/cflowentry.h>
#include <rofl/common/cvastring.h>



class eCtlBaseBase : public eFwdElemBase {};
class eCtlBaseInval : public eCtlBaseBase {};
class eCtlBaseExists : public eCtlBaseBase {};
class eCtlBaseNotFound : public eCtlBaseBase {};
class eCtlBaseDetached : public eCtlBaseBase {};

class eCtlBaseEndpntNotFound : public eCtlBaseBase {};

// forward declaration
class cadapter; // adaptor classes


class ctlbase :
	public cfwdelem
{

/*
 *
 */

protected: // data structures

	std::set<cadapter*> adapters; 	// list of all adaptors registered

	std::map<uint32_t, cadapter*> n_ports; // map of portno's => cadaptor mappings

	friend class cadapter;

public: // methods

	/**
	 *
	 */
	ctlbase(
			std::string dpname = std::string("ctlelem0"),
			uint64_t dpid = crandom(8).uint64(),
			uint8_t n_tables = DEFAULT_FE_TABLES_NUM,
			uint32_t n_buffers = DEFAULT_FE_BUFFER_SIZE,
			caddress const& rpc_ctl_addr = caddress(AF_INET, "0.0.0.0", 6643),
			caddress const& rpc_dpt_addr = caddress(AF_INET, "0.0.0.0", 6633));

	/**
	 *
	 */
	virtual
	~ctlbase();

	/** get free port number (from this->phy_ports)
	 *
	 */
	virtual uint32_t
	phy_port_get_free_portno()
	throw (eFwdElemNotFound)
	{
		return cfwdelem::phy_port_get_free_portno();
	};

	/** get free port number (from n_ports)
	 *
	 */
	virtual uint32_t
	n_port_get_free_portno() throw (eFwdElemNotFound);

	/**
	 *
	 */
	const char*
	c_str();

#if 0
protected: // methods called by classes derived from ctlbase for sending FLOW-MOD and PACKET-OUT to layer (N-1)

	/**
	 *
	 */
	void
	down_ofp_packet_out(
			uint32_t buffer_id,
			uint32_t in_port,
			cofaclist& actions,
			fframe& frame);

	/**
	 *
	 */
	void
	down_ofp_packet_out(
			uint32_t buffer_id,
			uint32_t in_port,
			cofaclist& actions);

	/**
	 *
	 */
	void
	down_ofp_flow_mod(
			cflowentry& fe);
#endif


protected: // methods for finding an adapted port managed by a cadaptor instance

	/**
	 *
	 */
	cadapter*
	find_adaptor_by_portno(uint32_t portno)
		throw (eCtlBaseNotFound);

public: // methods called by cadaptor instances registered within ctlbase
		// these must be overwritten by classes derived from ctlbase in order to
		// handle events from the cadaptor instances



public: // methods called by cadaptor instances registered within ctlbase

	/*
	 * upwards methods
	 */

	/**
	 */
	virtual void
	up_port_attach(
			uint32_t portno,
			cadapter *adaptor,
			cofport *ofport) throw (eCtlBaseExists);
	/**
	 */
	virtual void
	up_port_detach(
			uint32_t portno,
			cadapter *adapter,
			cofport *ofport) throw (eCtlBaseNotFound);

	/**
	 */
	virtual void
	up_port_modify(
			uint32_t portno,
			cadapter *adapter,
			cofport *ofport) throw (eCtlBaseNotFound);

	/**
	 */
	virtual void
	up_packet_in(
			cadapter *ctlmod,
			uint32_t buffer_id,
			uint16_t total_len,
			uint8_t table_id,
			uint8_t reason,
			cofmatch& match,
			fframe& frame);

	/**
	 */
	virtual void
	up_experimenter_message(
			cadapter *ctlmod,
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body,
			size_t bodylen);

	/*
	 * downwards methods
	 */

	/**
	 */
	virtual void
	down_fsp_open(
			cadapter *ctlmod,
			cofdpath *dpath,
			cofmatch const& m);

	/**
	 */
	virtual void
	down_fsp_close(
			cadapter *ctlmod,
			cofdpath *dpath,
			cofmatch const& m);

	/**
	 */
	virtual void
	down_fsp_close(
			cadapter *ctlmod,
			cofdpath *dpath);

	/**
	 */
	virtual void
	down_packet_out(
			cadapter *ctlmod,
			cofdpath *sw,
			uint32_t buffer_id,
			uint32_t in_port,
			cofaclist& aclist,
			fframe& frame)
	{
		send_packet_out_message(
				sw,
				buffer_id,
				in_port,
				aclist,
				frame.soframe(), frame.framelen());
	};

	/**
	 */
	virtual void
	down_flow_mod(
			cadapter *ctlmod,
			cofdpath *sw,
			cflowentry& fe)
	{
		send_flow_mod_message(sw, fe);
	};

	/**
	 */
	virtual void
	down_group_mod(
			cadapter *ctlmod,
			cofdpath *sw,
			cgroupentry& ge)
	{
		send_group_mod_message(sw, ge);
	};

	/**
	 */
	virtual void
	down_barrier_request(
			cadapter *ctlmod,
			cofdpath *sw)
	{
		send_barrier_request(sw);
	};

	/**
	 */
	virtual void
	down_experimenter_message(
			cadapter *ctlmod,
			cofdpath *sw,
			uint32_t experimenter_id,
			uint32_t exp_type,
			uint8_t *body,
			size_t bodylen)
	{
		send_experimenter_message(sw, experimenter_id, exp_type, body, bodylen);
	};



protected:

	/* methods from cfwdelem overwritten by ctlbase
	 * all remaining methods are overwritten in derived controller classes
	 * cethctl, cipctl, etc.
	 *
	 * only "up" messages are handled in ctlbase
	 */

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
	handle_stats_reply_timeout(cofdpath *sw) {};

	/** Handle OF packet-out messages. To be overwritten by derived class.
	 *
	 * Called upon reception of a PACKET-OUT.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack PACKET-OUT.message packet received from controller.
	 */
	virtual void
	handle_packet_out(cofctrl *ofctrl, cofpacket *pack);

	/** Handle OF packet-in messages. To be overwritten by derived class.
	 *
	 * Called upon reception of a PACKET-IN.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a PACKET-IN.message was received
	 * @param pack PACKET-IN.message packet received from datapath
	 */
	virtual void
	handle_packet_in(cofdpath *sw, cofpacket *pack);

	/** Handle OF barrier reply. To be overwritten by derived class.
	 *
	 * Called upon reception of a BARRIER.reply from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a BARRIER.reply was received
	 * @param pack BARRIER.reply packet received from datapath
	 */
	virtual void
	handle_barrier_reply(cofdpath *sw, cofpacket *pack) { delete pack; };

	/** Handle OF barrier reply timeout. To be overwritten by derived class.
	 *
	 * Called upon expiration of TIMER_FE_SEND_BARRIER_REPLY.
	 *
	 * @param sw cotswitch instance from whom a BARRIER.reply was expected.
	 */
	virtual void
	handle_barrier_reply_timeout(cofdpath *sw) {};

	/** Handle OF error message. To be overwritten by derived class.
	 *
	 * Called upon reception of an ERROR.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom an ERROR.message was received
	 * @param pack ERROR.message packet received from datapath
	 */
	virtual void
	handle_error(cofdpath *sw, cofpacket *pack) { delete pack; };

	/** Handle OF flow-removed message. To be overwritten by derived class.
	 *
	 * Called upon reception of a FLOW-REMOVED.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a FLOW-REMOVED.message was received
	 * @param pack FLOW-REMOVED.message packet received from datapath
	 */
	virtual void
	handle_flow_removed(cofdpath *sw, cofpacket *pack);

	/** Handle OF flow-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a FLOW-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack FLOW-MOD.message packet received from controller.
	 */
	virtual void
	handle_flow_mod(cofctrl *ofctrl, cofpacket *pack, cftentry *fte);

	/** Handle OF group-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a GROUP-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack GROUP-MOD.message packet received from controller.
	 */
	virtual void
	handle_group_mod(cofctrl *ofctrl, cofpacket *pack, cgtentry *gte);

	/** Handle OF table-mod message. To be overwritten by derived class.
	 *
	 * Called upon reception of a TABLE-MOD.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack TABLE-MOD.message packet received from controller.
	 */
	virtual void
	handle_table_mod(cofctrl *ofctrl, cofpacket *pack);

	/** Handle OF port-status message. To be overwritten by derived class.
	 *
	 * Called upon reception of a PORT-STATUS.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a PORT-STATUS.message was received
	 * @param pack PORT-STATUS.message packet received from datapath
	 */
	virtual void
	handle_port_status(cofdpath *sw, cofpacket *pack, cofport *port);

	/** Handle OF experimenter message. To be overwritten by derived class.
	 *
	 * Called upon reception of a VENDOR.message from the controlling entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param pack VENDOR.message packet received from controller.
	 */
	virtual void
	handle_experimenter_message(cofctrl *ofctrl, cofpacket *pack);

	/** Handle OF experimenter message. To be overwritten by derived class.
	 *
	 * Called upon reception of a VENDOR.message from a datapath entity.
	 * The OF packet must be removed from heap by the overwritten method.
	 *
	 * @param sw cofswitch instance from whom a VENDOR.message was received
	 * @param pack VENDOR.message packet received from datapath
	 */
	virtual void
	handle_experimenter_message(cofdpath *sw, cofpacket *pack);

	/** Handle new dpath
	 *
	 * Called upon creation of a new cofswitch instance.
	 *
	 * @param sw new cofswitch instance
	 */
	virtual void
	handle_dpath_open(cofdpath *sw);

	/** Handle close event on dpath
	 *
	 * Called upon deletion of a cofswitch instance
	 *
	 * @param sw cofswitch instance to be deleted
	 */
	virtual void
	handle_dpath_close(cofdpath *sw);

	/** Handle new ctrl
	 *
	 * Called upon creation of a new cofctrl instance.
	 *
	 * @param ctrl new cofctrl instance
	 */
	virtual void
	handle_ctrl_open(cofctrl *ctrl);

	/** Handle close event on ctrl
	 *
	 * Called upon deletion of a cofctrl instance
	 *
	 * @param ctrl cofctrl instance to be deleted
	 */
	virtual void
	handle_ctrl_close(cofctrl *ctrl);

	/** Handle timeout for GET-FSP request
	 *
	 */
	virtual void
	handle_get_fsp_reply_timeout(cofdpath *sw) {};


private: // methods

	/**
	 *
	 */
	uint64_t
	filter_instructions(
			cofinlist& old_insts,
			cofinlist& new_insts,
			cofmatch& match);

	/**
	 *
	 */
	uint64_t
	filter_buckets(
			cofbclist& old_buckets,
			cofbclist& new_buckets);

	/**
	 *
	 */
	uint64_t
	filter_actions(
			cofaclist& old_actions,
			cofaclist& new_actions,
			cofmatch& match,
			cpacket *pack = NULL) throw (eCtlBaseInval, eNotImplemented);

#if 0
	/**
	 *
	 */
	void
	handle_rofl_experimenter_message(
			cofdpath *sw,
			cofpacket *pack);
#endif


private: // data structures

	std::string info;

};



/**
 *
 */
class cadapter :
	public csyslog,
	public cfspentry_owner // behaves like a cfspentry_owner for flowspace registrations in cfwdelem::fsptable
{
private: // data structures

		ctlbase *pctlelem;	// adapter container hosting this cadapter
		std::string info;	// info string
		uint64_t dpid;		// datapath element this adapter is assigned to

public:

	/**
	 */
	cadapter(
			ctlbase *__ctlelem,
			uint64_t __dpid = 0) throw (eCtlBaseInval) :
		pctlelem(__ctlelem),
		dpid(__dpid)
	{
		if (NULL == pctlelem)
			throw eCtlBaseInval();

		pctlelem->adapters.insert(this);
	};

	/**
	 */	virtual
	~cadapter()
	{
		pctlelem->adapters.erase(this);
	};

	ctlbase&
	ctlelem() throw (eCtlBaseDetached)
	{
		if (NULL == pctlelem)
			throw eCtlBaseDetached();
		return (*pctlelem);
	};

	virtual const char*
	c_str()
	{
		cvastring vas;
		info.assign(vas("cadapter(%p)",
				this));
		return info.c_str();
	};


public: // lightweight OpenFlow interface for access by ctlbase

	/*
	 * do not implement the downward methods flow_mod and packet_out
	 * Outgoing commands will always be filtered and handled directly
	 * by ctlbase
	 */

	/**
	 */
	virtual void
	ofp_flow_rmvd(
			ctlbase *ctlelem,
			cofdpath *sw,
			cofpacket *pack) { delete pack; };

	/**
	 */
	virtual void
	ofp_packet_in(
			ctlbase *ctl,
			cofdpath *sw,
			cofpacket *pack) { delete pack; };

	/**
	 */
	virtual void
	ofp_barrier_reply(
			ctlbase *ctl,
			cofdpath *sw,
			cofpacket *pack) { delete pack; };

	/**
	 */
	virtual void
	ofp_stats_reply(
			ctlbase *ctl,
			cofdpath *sw,
			cofpacket *pack) { delete pack; };

	/**
	 */
	virtual void
	ofp_error(
			ctlbase *ctl,
			cofdpath *ofdpath,
			cofpacket *pack) { delete pack; };

	/**
	 */
	virtual void
	ofp_port_status(
			ctlbase *ctl,
			cofdpath *ofdpath,
			uint8_t reason,
			cofport *port) {};

	/**
	 */
	virtual void
	ofp_up_experimenter(
			ctlbase *ctl,
			cofdpath *ofdpath,
			cofpacket *pack) {};

	/**
	 */
	virtual void
	ofp_down_experimenter(
			ctlbase *ctl,
			cofctrl *ofctrl,
			cofpacket *pack) {};


public: // management interface

	/**
	 */
	virtual uint64_t
	filter_inport_match(
			ctlbase *ctlelem,
			uint32_t in_port,
			cofmatch& match,
			cofaclist& actions,
			cpacket* pack = 0)
	{
		return 0;
	};

	/**
	 */
	virtual uint64_t
	filter_outport_actions(
			ctlbase *ctlelem,
			uint32_t in_port,
			cofaclist& actions,
			cpacket *pack = 0)
	{
		return 0;
	};

	/**
	 */
	virtual void
	dpath_open(
			ctlbase *ctlelem,
			cofdpath *sw) {};

	/**
	 */
	virtual void
	dpath_close(
			ctlbase *ctlelem,
			cofdpath *sw) {};

	/**
	 */
	virtual void
	ctrl_open(
			ctlbase *ctlelem,
			cofctrl *ofctrl) {};

	/**
	 */
	virtual void
	ctrl_close(
			ctlbase *ctlelem,
			cofctrl *ofctrl) {};
};

#endif


