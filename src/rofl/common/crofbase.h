/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
#include <endian.h>
#include <string.h>
#include <time.h>
#ifndef htobe16
	#include "endian_conversion.h"
#endif

#include "rofl/common/ciosrv.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/csocket.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/logging.h"
#include "rofl/common/crofdpt.h"
#include "rofl/common/crofctl.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/crandom.h"

namespace rofl {


/* error classes */
class eRofBase                      : public RoflException {};   // base error class crofbase
class eRofBaseIsBusy                : public eRofBase {}; // this FwdElem is already controlled
class eRofBaseNotConnected          : public eRofBase {}; // this instance is not connected to the specified cofdpt/cofctl instance
class eRofBaseNotImpl               : public eRofBase {}; // this FwdElem's method is not implemented
class eRofBaseNoCtrl                : public eRofBase {}; // no controlling entity attached to this FwdElem
class eRofBaseNotFound              : public eRofBase {}; // internal entity not found
class eRofBaseInval                 : public eRofBase {}; // invalid parameter (e.g. invalid packet type)
class eRofBaseNotAttached           : public eRofBase {}; // received command from entity being not attached
class eRofBaseNoRequest             : public eRofBase {}; // no request packet found for session
class eRofBaseXidInval              : public eRofBase {}; // invalid xid in session exchange
class eRofBaseExists                : public eRofBase {}; // fwdelem with either this dpid or dpname already exists
class eRofBaseOFportNotFound        : public eRofBase {}; // cofport instance not found
class eRofBaseTableNotFound         : public eRofBase {}; // flow-table not found (e.g. unknown table_id in flow_mod)
class eRofBaseGotoTableNotFound     : public eRofBase {}; // table-id specified in OFPIT_GOTO_TABLE invalid
class eRofBaseFspSupportDisabled    : public eRofBase {};
class eRofBaseCongested             : public eRofBase {}; // control channel is congested, dropping messages



/**
 * @brief 	Base class for revised OpenFlow library
 *
 * Derive from this class in order to use ROFL's functionality.
 * crofbase supports communication to peer entities acting in either
 * controller or datapath role using the OpenFlow protocol in various
 * versions. Each peer entity is represented by an instance of class
 * crofdpt (handle for a remote datapath) or an instance of class
 * crofctl (handle for a remote controller). These classes provide
 * a rich API for managing the OpenFlow control channel, e.g.,
 * establishing of main and auxiliary control connections. Furthermore,
 * they include a protocol parser mapping OpenFlow messages from their
 * wire (TCP) representation into a correspondent C++ representation.
 * crofbase provides three groups of functionality:
 *
 * 1. Management of an arbitrary number of listening sockets for accepting
 * incoming OpenFlow connections either in controller or datapath role.
 * 2. Management of an arbitrary number of peer controller entities
 * each encapsulated in a separate instance of class crofctl.
 * 3. Management of an arbitrary number of peer datapath entities
 * each encapsulated in a separate instance of class crofdpt.
 *
 * Furthermore, crofbase manages all active transactions when acting in
 * controller role. crofctl and crofdpt instances support an arbitrary
 * number of auxiliary connections as defined by OpenFlow 1.3. See
 * class descriptions for crofctl and crofdpt for details.
 *
 * crofbase acts as environment surrounding instances of crofctl and
 * crofdpt and receives information notifications and messages
 * generated during operation. A deriving class may overwrite any of
 * the handler methods defined within crofbase to receive a specific
 * event. Overwriting handler methods is optional and crofbase defines
 * a reasonable default handler for each event.
 *
 * @see crofctl
 * @see crofdpt
 */
class crofbase :
	public ciosrv,
	public csocket_owner,
	public crofconn_env,
	public ctransactions_env,
	public crofctl_env,
	public crofdpt_env
{
	/**
	 * @enum rofl::crofbase::crofbase_event_t
	 *
	 * event types defined by crofbase for feeding the event-queue
	 */
	enum crofbase_event_t {
		EVENT_NONE         = 0,
		EVENT_CTL_DETACHED = 1,
		EVENT_DPT_DETACHED = 2,
	};

	/**
	 * @enum rofl::crofbase::crofbase_timer_t
	 *
	 * timer types defined by crofbase
	 */
	enum crofbase_timer_t {
		TIMER_NONE         = 0,
		TIMER_RUN_ENGINE   = 1,
	};

public:

	/**
	 * @brief	crofbase constructor
	 *
	 * Constructor takes a single argument defining the OpenFlow versions
	 * allowed for passively created OpenFlow sockets. You may specify
	 * multiple OpenFlow versions as needed.
	 *
	 * @param versionbitmap OpenFlow version bitmap
	 */
	crofbase(
			const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap =
					rofl::openflow::cofhello_elem_versionbitmap());

	/**
	 * @brief	crofbase destructor
	 *
	 * Destructor shuts down all active connections and listening sockets
	 * and deallocates all instance of classes crofctl and crofdpt.
	 */
	virtual
	~crofbase();

public:

	/**
	 * @name	Methods for managing listening sockets for accepting incoming OpenFlow connections
	 */

	/**@{*/

	/**
	 * @brief	Opens a listening socket for accepting connection requests from datapath elements
	 *
	 * @param socket_type one of the constants defined in csocket.h, e.g. SOCKET_TYPE_PLAIN
	 * @param params set of parameters used for creating a listening socket
	 *
	 * @see csocket
	 */
	void
	rpc_listen_for_dpts(
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& params);

	/**
	 * @brief	Opens a listening socket for accepting connection requests from controllers
	 *
	 * @param socket_type one of the constants defined in csocket.h, e.g. SOCKET_TYPE_PLAIN
	 * @param params set of parameters used for creating a listening socket
	 * @see csocket
	 */
	void
	rpc_listen_for_ctls(
			enum rofl::csocket::socket_type_t socket_type,
			const cparams& params);

	/**
	 * @brief	Closes all open crofctl, crofdpt and listening socket instances.
	 *
	 */
	void
	rpc_close_all();

	/**@}*/

public:

	/**
	 * @name	Methods for managing peer entities acting in datapath role
	 */

	/**@{*/

	/**
	 * @brief Returns the next idle identifier for a new rofl::crofdpt instance
	 *
	 * @see rofl::crofdpt
	 * @see rofl::cdptid
	 * @return Next idle identifier for a rofl::crofdpt instance
	 */
	rofl::cdptid
	get_idle_dptid() const {
		uint64_t id = 0;
		while (has_dpt(rofl::cdptid(id))) {
			id++;
		}
		return rofl::cdptid(id);
	};

	/**
	 * @brief	Deletes all existing rofl::crofdpt instances
	 */
	void
	drop_dpts() {
		for (std::map<rofl::cdptid, crofdpt*>::iterator
				it = rofdpts.begin(); it != rofdpts.end(); ++it) {
			delete it->second;
		}
		rofdpts.clear();
	};

	/**
	 * @brief	Creates new rofl::crofdpt instance for given identifier
	 *
	 * Creates a new rofl::crofdpt instance identified by the identifier
	 * specified in rofl::cdptid parameter. If an instance with this identifier
	 * already exists, it is destroyed first (this includes termination of all
	 * OpenFlow connections established in its associated OpenFlow control channel)
	 * before a new, empty instance is created. You must add OpenFlow
	 * connections to actually bind the rofl::crofdpt instance to a datapath
	 * peer entity.
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @param versionbitmap version bitmap defining all acceptable OpenFlow versions
	 * @param remove_on_channel_close when true, automatically remove this
	 * rofl::crofdpt instance, when all OpenFlow control channel connections
	 * have been terminated
	 * @result reference to new rofl::crofdpt instance
	 */
	crofdpt&
	add_dpt(
		const cdptid& dptid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false) {
		if (rofdpts.find(dptid) != rofdpts.end()) {
			delete rofdpts[dptid];
			rofdpts.erase(dptid);
		}
		rofdpts[dptid] = new crofdpt(this, dptid, remove_on_channel_close, versionbitmap);
		return *(rofdpts[dptid]);
	};

	/**
	 * @brief	Returns existing or creates new rofl::crofdpt instance for given identifier
	 *
	 * Returns rofl::crofdpt instance specified by identifier dptid. If none exists,
	 * a new empty instance is created. You must add OpenFlow
	 * connections to actually bind the rofl::crofdpt instance to a datapath
	 * peer entity.
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @param versionbitmap version bitmap defining all acceptable OpenFlow versions
	 * @param remove_on_channel_close when true, automatically remove this
	 * rofl::crofdpt instance, when all OpenFlow control channel connections
	 * have been terminated
	 * @result reference to existing or new rofl::crofdpt instance
	 */
	crofdpt&
	set_dpt(
		const cdptid& dptid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false) {
		if (rofdpts.find(dptid) == rofdpts.end()) {
			rofdpts[dptid] = new crofdpt(this, dptid, remove_on_channel_close, versionbitmap);
		}
		return *(rofdpts[dptid]);
	};

	/**
	 * @brief	Returns reference to existing rofl::crofdpt instance.
	 *
	 * Returns existing rofl::crofdpt instance specified by identifier dptid.
	 * If the identifier does not exists, throws an exception eRofBaseNotFound.
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @result reference to existing rofl::crofdpt instance
	 * @throws eRofBaseNotFound
	 */
	crofdpt&
	set_dpt(
			const cdptid& dptid) {
		if (rofdpts.find(dptid) == rofdpts.end()) {
			throw eRofBaseNotFound();
		}
		return *(rofdpts[dptid]);
	};

	/**
	 * @brief	Returns const reference to existing rofl::crofdpt instance.
	 *
	 * Returns existing rofl::crofdpt instance specified by identifier dptid.
	 * If the identifier does not exists, throws an exception eRofBaseNotFound.
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @result reference to existing rofl::crofdpt instance
	 * @throws eRofBaseNotFound
	 */
	const crofdpt&
	get_dpt(
			const cdptid& dptid) const {
		if (rofdpts.find(dptid) == rofdpts.end()) {
			throw eRofBaseNotFound();
		}
		return *(rofdpts.at(dptid));
	};

	/**
	 * @brief	Deletes a rofl::crofdpt instance given by identifier.
	 *
	 * If the identifier is non-existing, the method does nothing and returns.
	 */
	void
	drop_dpt(
		cdptid dptid) { // make a copy here, do not use a const reference
		if (rofdpts.find(dptid) == rofdpts.end()) {
			return;
		}
		delete rofdpts[dptid];
		rofdpts.erase(dptid);
	};

	/**
	 * @brief	Checks for existence of rofl::crofdpt instance with given identifier
	 *
	 * @param dptid internal datapath handle (not DPID)
	 * @result bool value
	 */
	bool
	has_dpt(
		const cdptid& dptid) const {
		return (not (rofdpts.find(dptid) == rofdpts.end()));
	};

	/**@}*/

public:

	/**
	 * @name	Methods for managing peer entities acting in controller role
	 */

	/**@{*/

	/**
	 * @brief Returns the next idle identifier for a new rofl::crofctl instance
	 *
	 * @see rofl::crofctl
	 * @see rofl::cctlid
	 * @return Next idle identifier for a rofl::crofctl instance
	 */
	rofl::cctlid
	get_idle_ctlid() const {
		uint64_t id = 0;
		while (has_ctl(rofl::cctlid(id))) {
			id++;
		}
		return rofl::cctlid(id);
	};

	/**
	 * @brief	Deletes all existing rofl::crofctl instances
	 */
	void
	drop_ctls() {
		for (std::map<rofl::cctlid, crofctl*>::iterator
				it = rofctls.begin(); it != rofctls.end(); ++it) {
			delete it->second;
		}
		rofctls.clear();
	};

	/**
	 * @brief	Creates new rofl::crofctl instance for given identifier
	 *
	 * Creates a new rofl::crofctl instance identified by the identifier
	 * specified in rofl::cctlid parameter. If an instance with this identifier
	 * already exists, it is destroyed first (this includes termination of all
	 * OpenFlow connections established in its associated OpenFlow control channel)
	 * before a new, empty instance is created. You must add OpenFlow
	 * connections to actually bind the rofl::crofctl instance to a controller
	 * peer entity.
	 *
	 * @param ctlid internal controller handle
	 * @param versionbitmap version bitmap defining all acceptable OpenFlow versions
	 * @param remove_on_channel_close when true, automatically remove this
	 * rofl::crofctl instance, when all OpenFlow control channel connections
	 * have been terminated
	 * @result reference to new rofl::crofctl instance
	 */
	crofctl&
	add_ctl(
		const cctlid& ctlid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false) {
		if (rofctls.find(ctlid) != rofctls.end()) {
			delete rofctls[ctlid];
			rofctls.erase(ctlid);
		}
		rofctls[ctlid] = new crofctl(this, ctlid, remove_on_channel_close, versionbitmap);
		return *(rofctls[ctlid]);
	};

	/**
	 * @brief	Returns existing or creates new rofl::crofctl instance for given identifier
	 *
	 * Returns rofl::crofctl instance specified by identifier ctlid. If none exists,
	 * a new empty instance is created. You must add OpenFlow
	 * connections to actually bind the rofl::crofctl instance to a controller
	 * peer entity.
	 *
	 * @param ctlid internal controller handle
	 * @param versionbitmap version bitmap defining all acceptable OpenFlow versions
	 * @param remove_on_channel_close when true, automatically remove this
	 * rofl::crofctl instance, when all OpenFlow control channel connections
	 * have been terminated
	 * @result reference to existing or new rofl::crofctl instance
	 */
	crofctl&
	set_ctl(
		const cctlid& ctlid,
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap,
		bool remove_on_channel_close = false) {
		if (rofctls.find(ctlid) == rofctls.end()) {
			rofctls[ctlid] = new crofctl(this, ctlid, remove_on_channel_close, versionbitmap);
		}
		return *(rofctls[ctlid]);
	};

	/**
	 * @brief	Returns reference to existing rofl::crofctl instance.
	 *
	 * Returns existing rofl::crofctl instance specified by identifier ctlid.
	 * If the identifier does not exists, throws an exception eRofBaseNotFound.
	 *
	 * @param ctlid internal controller handle
	 * @result reference to existing rofl::crofctl instance
	 * @throws eRofBaseNotFound
	 */
	crofctl&
	set_ctl(
			const cctlid& ctlid) {
		if (rofctls.find(ctlid) == rofctls.end()) {
			throw eRofBaseNotFound();
		}
		return *(rofctls[ctlid]);
	};

	/**
	 * @brief	Returns const reference to existing rofl::crofctl instance.
	 *
	 * Returns existing rofl::crofctl instance specified by identifier ctlid.
	 * If the identifier does not exists, throws an exception eRofBaseNotFound.
	 *
	 * @param ctlid internal controller handle
	 * @result reference to existing rofl::crofctl instance
	 * @throws eRofBaseNotFound
	 */
	const crofctl&
	get_ctl(
			const cctlid& ctlid) const {
		if (rofctls.find(ctlid) == rofctls.end()) {
			throw eRofBaseNotFound();
		}
		return *(rofctls.at(ctlid));
	};

	/**
	 * @brief	Deletes a rofl::crofctl instance given by identifier.
	 *
	 * If the identifier is non-existing, the method does nothing and returns.
	 */
	void
	drop_ctl(
		cctlid ctlid) { // make a copy here, do not use a const reference
		if (rofctls.find(ctlid) == rofctls.end()) {
			return;
		}
		delete rofctls[ctlid];
		rofctls.erase(ctlid);
	};

	/**
	 * @brief	Checks for existence of rofl::crofctl instance with given identifier
	 *
	 * @param ctlid internal controller handle
	 * @result bool value
	 */
	bool
	has_ctl(
		const cctlid& ctlid) const {
		return (not (rofctls.find(ctlid) == rofctls.end()));
	};

	/**@}*/

public:

	/**
	 * @name	Auxiliary methods
	 */

	/**@{*/

	/**
	 * @brief	Returns OpenFlow's NO_BUFFER constant for various OpenFlow versions.
	 */
	static uint32_t
	get_ofp_no_buffer(
			uint8_t ofp_version);

	/**
	 * @brief	Returns OpenFlow's OFPP_CONTROLLER constant for various OpenFlow versions.
	 */
	static uint32_t
	get_ofp_controller_port(
			uint8_t ofp_version);

	/**
	 * @brief 	Returns OpenFlow's OFPP_FLOOD constant for various OpenFlow versions.
	 */
	static uint32_t
	get_ofp_flood_port(
			uint8_t ofp_version);

	/**
	 * @brief	Returns OpenFlow's OFPFC constants for various OpenFlow versions.
	 */
	static uint8_t
	get_ofp_command(
			uint8_t ofp_version,
			const enum openflow::ofp_flow_mod_command& cmd);

	/**
	 * @brief	Returns reference to OpenFlow version bitmap used for incoming connections.
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	set_versionbitmap()
	{ return versionbitmap; };

	/**
	 * @brief 	Returns const reference to OpenFlow version bitmap used for incoming connections.
	 */
	const rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap() const
	{ return versionbitmap; };

	/**
	 * @brief 	Returns highest OpenFlow version supported for incoming connections.
	 */
	uint8_t
	get_highest_supported_ofp_version() const
	{ return versionbitmap.get_highest_ofp_version(); };

	/**
	 * @brief 	Returns true, when the given OpenFlow version is supported by this crofbase instance.
	 */
	bool
	is_ofp_version_supported(
			uint8_t ofp_version) const
	{ return versionbitmap.has_ofp_version(ofp_version); };

	/**@}*/

protected:

	/**
	 * @name 	Event handlers for management notifications for datapath elements
	 *
	 * Overwrite any of these methods for receiving datapath related event notifications.
	 */

	/**@{*/

	/**
	 * @brief	Called after establishing the associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been established, i.e., its main connection has been accepted by the remote site.
	 *
	 * @param dpt datapath instance
	 */
	virtual void
	handle_dpt_open(
			rofl::crofdpt& dpt) {
		rofl::logging::info << "[rofl-common][crofbase] control channel established, "
				<< "dptid: " << dpt.get_dptid().str() << std::endl;
	};

	/**
	 * @brief	Called after termination of associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been terminated, i.e., its main connection has been closed from the
	 * remote site. The rofl::crofdpt instance itself is not destroyed, unless
	 * its 'remove_on_channel_close' flag has been set to true during its
	 * construction.
	 *
	 * @param dpt datapath instance
	 */
	virtual void
	handle_dpt_close(
			rofl::crofdpt& dpt) {
		rofl::logging::info << "[rofl-common][crofbase] control channel terminated, "
				<< "dptid: " << dpt.get_dptid().str() << std::endl;
	};

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been established.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_established(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] connection established, "
				<< "dptid: " << dpt.get_dptid().str()  << " auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been terminated by the peer entity.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_terminated(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] connection terminated, "
				<< "dptid: " << dpt.get_dptid().str()  << " auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief 	Called when an attempt to establish a control connection has been refused.
	 *
	 * This event occurs when the C-library's connect() system call fails
	 * with the ECONNREFUSED error code. This indicates typically a problem on
	 * the remote site.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_refused(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] connection refused, "
				<< "dptid: " << dpt.get_dptid().str()  << " auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief 	Called when an attempt to establish a control connection has been failed.
	 *
	 * This event occurs when some failure occures while calling the underlying
	 * C-library connect() system call, e.g., no route to destination, etc. This may
	 * indicate a local configuration problem inside or outside of the application.
	 *
	 * @param dpt datapath instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_failed(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] connection failed, "
				<< "dptid: " << dpt.get_dptid().str()  << " auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief	Called when a congestion situation on the control connection has been solved.
	 *
	 * A control channel may face congestion situations when insufficient bandwidth
	 * on the underlying IP path or some backpressuring by the remote site requires
	 * to throttle the overall message transmission rate on a control connection.
	 * A congestion situation is indicated by the return values obtained from the various
	 * send-methods defined within rofl::crofdpt. A solved congestion situation is
	 * indicated by calling this method. Note that ROFL will store OpenFlow messages
	 * even under congestion, thus filling up its internal buffers until no further
	 * memory is available for doing so. It is up to the application designer to
	 * throttle transmission of further messages according to the channel capacity.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_writable(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] connection is writable, "
				<< "dptid: " << dpt.get_dptid().str()  << " auxid: " << auxid.str() << std::endl;
	};

	/**@}*/

protected:

	/**
	 * @name 	Event handlers for management notifications for controller entities
	 *
	 * Overwrite any of these methods for receiving controller related event notifications.
	 */

	/**@{*/

	/**
	 * @brief	Called after establishing the associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been established, i.e., its main connection has been accepted by the remote site.
	 *
	 * @param ctl controller instance
	 */
	virtual void
	handle_ctl_open(
			rofl::crofctl& ctl) {
		rofl::logging::info << "[rofl-common][crofbase] control channel established, "
				<< "ctlid: " << ctl.get_ctlid().str() << std::endl;
	};

	/**
	 * @brief	Called after termination of associated OpenFlow control channel.
	 *
	 * This method is called once the associated OpenFlow control channel has
	 * been terminated, i.e., its main connection has been closed from the
	 * remote site. The rofl::crofctl instance itself is not destroyed, unless
	 * its 'remove_on_channel_close' flag has been set to true during its
	 * construction.
	 *
	 * @param ctl controller instance
	 */
	virtual void
	handle_ctl_close(
			rofl::crofctl& ctl) {
		rofl::logging::info << "[rofl-common][crofbase] control channel terminated, "
				<< "ctlid: " << ctl.get_ctlid().str() << std::endl;
	};

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been established.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_established(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] connection established, "
				<< "ctlid: " << ctl.get_ctlid().str()  << " auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief 	Called when a control connection (main or auxiliary) has been terminated by the peer entity.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_terminated(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] connection terminated, "
				<< "ctlid: " << ctl.get_ctlid().str()  << " auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief 	Called when an attempt to establish a control connection has been refused.
	 *
	 * This event occurs when the C-library's connect() system call fails
	 * with the ECONNREFUSED error code. This indicates typically a problem on
	 * the remote site.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_refused(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] connection refused, "
				<< "ctlid: " << ctl.get_ctlid().str()  << " auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief 	Called when an attempt to establish a control connection has been failed.
	 *
	 * This event occurs when some failure occures while calling the underlying
	 * C-library connect() system call, e.g., no route to destination, etc. This may
	 * indicate a local configuration problem inside or outside of the application.
	 *
	 * @param ctl controller instance
	 * @param auxid connection identifier (main: 0)
	 */
	virtual void
	handle_conn_failed(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] connection failed, "
				<< "ctlid: " << ctl.get_ctlid().str()  << " auxid: " << auxid.str() << std::endl;
	};

	/**
	 * @brief	Called when a congestion situation on the control connection has been solved.
	 *
	 * A control channel may face congestion situations when insufficient bandwidth
	 * on the underlying IP path or some backpressuring by the remote site requires
	 * to throttle the overall message transmission rate on a control connection.
	 * A congestion situation is indicated by the return values obtained from the various
	 * send-methods defined within rofl::crofctl. A solved congestion situation is
	 * indicated by calling this method. Note that ROFL will store OpenFlow messages
	 * even under congestion, thus filling up its internal buffers until no further
	 * memory is available for doing so. It is up to the application designer to
	 * throttle transmission of further messages according to the channel capacity.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier (main: 0)
	 */
	virtual void
	handle_conn_writable(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid) {
		rofl::logging::info << "[rofl-common][crofbase] connection is writable, "
				<< "ctlid: " << ctl.get_ctlid().str()  << " auxid: " << auxid.str() << std::endl;
	};

	/**@}*/

protected:

	/**
	 * @name 	Event handlers for OpenFlow message notifications received from datapath elements
	 *
	 * Overwrite any of these methods for receiving messages from the attached datapath element.
	 * Once this method terminates, rofl will destroy the message object. If you want to save
	 * a message or parts of it, you must create a local copy. All messages are internally
	 * allocated on the heap and both allocation and deallocation is handled by rofl-common.
	 * Do not delete any of these messages or its internals.
	 */

	/**@{*/

	/**
	 * @brief	OpenFlow Features-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_features_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Features-Reply message.
	 *
	 * No Features-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_features_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Get-Config-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_config_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_config_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Get-Config-Reply message.
	 *
	 * No Get-Config-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_get_config_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Stats-Reply message.
	 *
	 * No Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 * @param stats_type statistics message subtype
	 */
	virtual void
	handle_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid,
			uint8_t stats_type)
	{};

	/**
	 * @brief	OpenFlow Desc-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_desc_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_desc_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Desc-Stats-Reply message.
	 *
	 * No Desc-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_desc_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Table-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Table-Stats-Reply message.
	 *
	 * No Table-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_table_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Port-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Port-Stats-Reply message.
	 *
	 * No Port-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_port_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Flow-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Flow-Stats-Reply message.
	 *
	 * No Flow-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_flow_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Aggregate-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_aggregate_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_aggr_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Aggregate-Stats-Reply message.
	 *
	 * No Aggregate-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_aggregate_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Queue-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Queue-Stats-Reply message.
	 *
	 * No Queue-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_queue_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Group-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Group-Stats-Reply message.
	 *
	 * No Group-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_group_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Group-Desc-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_desc_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_desc_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Group-Desc-Stats-Reply message.
	 *
	 * No Group-Desc-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_group_desc_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Group-Features-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_features_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_features_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Group-Features-Stats-Reply message.
	 *
	 * No Group-Features-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_group_features_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Meter-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Meter-Stats-Reply message.
	 *
	 * No Meter-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_meter_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Meter-Config-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_config_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_config_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Meter-Config-Stats-Reply message.
	 *
	 * No Meter-Config-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_meter_config_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Meter-Features-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_features_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_features_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Meter-Features-Stats-Reply message.
	 *
	 * No Meter-Features-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_meter_features_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Table-Features-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_features_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Table-Features-Stats-Reply message.
	 *
	 * No Table-Features-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_table_features_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Port-Desc-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_desc_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Port-Desc-Stats-Reply message.
	 *
	 * No Port-Desc-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_port_desc_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Experimenter-Stats-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_stats_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter_stats_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Experimenter-Stats-Reply message.
	 *
	 * No Experimenter-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_experimenter_stats_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Packet-In message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_packet_in(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_packet_in& msg)
	{};

	/**
	 * @brief	OpenFlow Barrier-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_barrier_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_barrier_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Barrier-Reply message.
	 *
	 * No Barrier-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_barrier_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Flow-Removed message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_removed(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_removed& msg)
	{};

	/**
	 * @brief	OpenFlow Port-Status-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_status(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_status& msg)
	{};

	/**
	 * @brief	OpenFlow Queue-Get-Config-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_get_config_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_get_config_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Table-Stats-Reply message.
	 *
	 * No Table-Stats-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_queue_get_config_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Error message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_error_message(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_error& msg)
	{};

	/**
	 * @brief	OpenFlow Experimenter message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_message(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Experimenter message.
	 *
	 * No Experimenter message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_experimenter_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Role-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_role_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_role_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Role-Reply message.
	 *
	 * No Role-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_role_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**
	 * @brief	OpenFlow Get-Async-Config-Reply message received.
	 *
	 * @param dpt datapath instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_async_config_reply(
			rofl::crofdpt& dpt,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_async_config_reply& msg)
	{};

	/**
	 * @brief	Timer expired while waiting for OpenFlow Get-Async-Config-Reply message.
	 *
	 * No Get-Async-Config-Reply message was received in the specified time interval
	 * for the given OpenFlow transaction identifier.
	 *
	 * @param dpt datapath instance
	 * @param xid OpenFlow transaction identifier
	 */
	virtual void
	handle_get_async_config_reply_timeout(
			rofl::crofdpt& dpt,
			uint32_t xid)
	{};

	/**@}*/

protected:

	/**
	 * @name 	Event handlers for OpenFlow message notifications received from controller entities
	 *
	 * Overwrite any of these methods for receiving messages from the attached controller entity.
	 * Once this method terminates, rofl will destroy the message object. If you want to save
	 * a message or parts of it, you must create a local copy. All messages are internally
	 * allocated on the heap and both allocation and deallocation is handled by rofl-common.
	 * Do not delete any of these messages or its internals.
	 *
	 * Higher logic implemented on top of crofbase and acting in datapath role should handle
	 * incoming requests, e.g., sending back a reply to the controller entity. rofl-common does
	 * not generate reply messages automatically. You may throw any of the OpenFlow related exceptions
	 * defined by rofl-common within these handler methods. rofl-common will detect such exceptions
	 * and generate appropriate error messages for transmission to the peer entity.
	 */

	/**@{*/

	/**
	 * @brief	OpenFlow Features-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_features_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_features_request& msg)
	{};

	/**
	 * @brief	OpenFlow Get-Config-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_config_request& msg)
	{};

	/**
	 * @brief	OpenFlow Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Desc-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_desc_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Table-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Port-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Flow-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Aggregate-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_aggregate_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_aggr_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Queue-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Group-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Group-Desc-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_desc_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Group-Features-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_features_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Meter-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Meter-Config-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_config_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_config_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Meter-Features-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_features_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Table-Features-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_features_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_features_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Port-Desc-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_desc_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_desc_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Experimenter-Stats-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_stats_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter_stats_request& msg)
	{};

	/**
	 * @brief	OpenFlow Packet-Out message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_packet_out(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_packet_out& msg)
	{};

	/**
	 * @brief	OpenFlow Barrier-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_barrier_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_barrier_request& msg)
	{};

	/**
	 * @brief	OpenFlow Flow-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_flow_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_flow_mod& msg)
	{};

	/**
	 * @brief	OpenFlow Group-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_group_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_group_mod& msg)
	{};

	/**
	 * @brief	OpenFlow Table-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_table_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_table_mod& msg)
	{};

	/**
	 * @brief	OpenFlow Port-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_port_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_port_mod& msg)
	{};

	/**
	 * @brief	OpenFlow Queue-Get-Config-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_queue_get_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_queue_get_config_request& msg)
	{};

	/**
	 * @brief	OpenFlow Set-Config message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_set_config(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_set_config& msg)
	{};

	/**
	 * @brief	OpenFlow Experimenter message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_experimenter_message(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_experimenter& msg)
	{};

	/**
	 * @brief	OpenFlow error message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_error_message(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_error& msg)
	{};

	/**
	 * @brief	OpenFlow Role-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_role_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_role_request& msg)
	{};

	/**
	 * @brief	OpenFlow Get-Async-Config-Request message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_get_async_config_request(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_get_async_config_request& msg)
	{};

	/**
	 * @brief	OpenFlow Set-Async-Config message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_set_async_config(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_set_async_config& msg)
	{};

	/**
	 * @brief	OpenFlow Meter-Mod message received.
	 *
	 * @param ctl controller instance
	 * @param auxid control connection identifier
	 * @param msg OpenFlow message instance
	 */
	virtual void
	handle_meter_mod(
			rofl::crofctl& ctl,
			const rofl::cauxid& auxid,
			rofl::openflow::cofmsg_meter_mod& msg)
	{};

	/**@}*/

protected:

	/**
	 * @name	Methods for sending asynchronous messages towards a controller entity.
	 *
	 * This group of methods allows an entity acting in datapath role to send
	 * asynchronous OpenFlow messages towards a group of controller entities
	 * based on their roles as defined in OpenFlow. This includes message transmission
	 * based on individual controller modes like master, slave and equal and
	 * the asynchronous configuration defining the set of messages sent towards
	 * each controller. rofl-common maintains these roles automatically within
	 * rofl::crofctl instances based on OpenFlow Role-Request message received
	 * from the control plane.
	 */

	/**@{*/

	/**
	 * @brief	Sends OpenFlow Packet-In message to controller entity.
	 *
	 * @param auxid controller connection identifier
	 * @param buffer_id OpenFlow buffer identifier where this packet is stored in
	 * @param total_len total length of buffer stored on datapath element
	 * @param reason one of OpenFlow's OFPR constants specifying the reason for sending a packet-in
	 * @param table_id identifier of OpenFlow table emitting the packet-in event
	 * @param cookie cookie of flow-table entry emitting the packet-in event
	 * @param in_port port where packet causing this packet-in event was received (OpenFlow 1.0 only)
	 * @param match set of OpenFlow matches generated by the datapath element's packet parser
	 * @param data pointer to start of data buffer containing the packet
	 * @param datalen length of  data buffer containing the packet
	 * @throws eRofBaseNotConnected
	 */
	void
	send_packet_in_message(
			const rofl::cauxid& auxid,
			uint32_t buffer_id,
			uint16_t total_len,
			uint8_t reason,
			uint8_t table_id,
			uint64_t cookie,
			uint16_t in_port, // for OF1.0
			rofl::openflow::cofmatch &match,
			uint8_t *data,
			size_t datalen);

	/**
	 * @brief	Sends OpenFlow Flow-Removed message to controller entity.
	 *
	 * @param auxid controller connection identifier
	 * @param match set of OpenFlow matches generated by the datapath element's packet parser
	 * @param cookie cookie of flow-table entry emitting the flow-removed event
	 * @param priority priority of flow-table entry emitting the flow-removed event
	 * @param reason one of OpenFlow's OFPRR constants specifying the reason for sending a flow-removed
	 * @param table_id identifier of OpenFlow table emitting the flow-removed event
	 * @param duration_sec overall lifetime of this flow-table entry (seconds)
	 * @param duration_nsec overall lifetime of this flow-table entry (nseconds)
	 * @param idle_timeout idle_timeout of flow-table entry emitting the flow-removed event
	 * @param hard_timeout hard_timeout of flow-table entry emitting the flow-removed event
	 * @param packet_count number of packets that traversed this flow-table entry emitting the flow-removed event
	 * @param byte_count number of bytes that traversed this flow-table entry emitting the flow-removed event
	 */
	void
	send_flow_removed_message(
			const rofl::cauxid& auxid,
			rofl::openflow::cofmatch& match,
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

	/**
	 * @brief	Sends OpenFlow Port-Status message to controller entity.
	 *
	 * @param auxid controller connection identifier
	 * @param reason one of OpenFlow's OFPPR constants specifying the reason for sending a port-status
	 * @param port cofport instance definine the port's properties
	 */
	void
	send_port_status_message(
			const rofl::cauxid& auxid,
			uint8_t reason,
			const rofl::openflow::cofport& port);

	/**@}*/

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofbase const& rofbase) {
		os << "<crofbase >" << std::endl;
		for (std::map<cctlid, crofctl*>::const_iterator
				it = rofbase.rofctls.begin(); it != rofbase.rofctls.end(); ++it) {
			rofl::indent i(2);
			os << it->first;
		}
		for (std::map<cdptid, crofdpt*>::const_iterator
				it = rofbase.rofdpts.begin(); it != rofbase.rofdpts.end(); ++it) {
			rofl::indent i(2);
			os << it->first;
		}
		return os;
	};

private:

	virtual void
	role_request_rcvd(
			rofl::crofctl& ctl,
			uint32_t role,
			uint64_t rcvd_generation_id);

private:

	virtual void
	handle_connect_refused(
			crofconn& conn);

	virtual void
	handle_connect_failed(
			crofconn& conn);

	virtual void
	handle_connected(
			crofconn& conn,
			uint8_t ofp_version);

	virtual void
	handle_closed(
			crofconn& conn)
	{};

	virtual void
	handle_write(
			crofconn& conn)
	{};

	virtual void
	recv_message(
			crofconn& conn,
			rofl::openflow::cofmsg *msg)
	{ delete msg; };

	virtual uint32_t
	get_async_xid(
			crofconn& conn)
	{ return transactions.get_async_xid(); };

	virtual uint32_t
	get_sync_xid(
			crofconn& conn,
			uint8_t msg_type = 0,
			uint16_t msg_sub_type = 0)
	{ return transactions.add_ta(cclock(5, 0), msg_type, msg_sub_type); };

	virtual void
	release_sync_xid(
			crofconn& conn,
			uint32_t xid)
	{ transactions.drop_ta(xid); };

private:

	virtual void
	ta_expired(ctransactions& tas, ctransaction& ta)
	{};

private:

	virtual void
	handle_listen(
			csocket& socket,
			int newsd);

	virtual void
	handle_accepted(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_accept_refused(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_connected(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_connect_refused(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_connect_failed(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_read(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_write(
			csocket& socket)
	{ /*  do nothing here */ };

	virtual void
	handle_closed(
			csocket& socket);

private:

	virtual void
	handle_dpt_attached(crofdpt& dpt) {
		handle_dpt_open(dpt);
	};

	virtual void
	handle_dpt_detached(crofdpt& dpt) {
		// destroy crofdpt object, when is was created upon an incoming connection from a peer entity
		if (dpt.remove_on_channel_termination()) {
			dpts_detached.insert(dpt.get_dptid());
			push_on_eventqueue(EVENT_DPT_DETACHED);
		} else {
			handle_dpt_close(dpt);
		}
	};

	virtual void
	handle_ctl_attached(crofctl& ctl) {
		handle_ctl_open(ctl);
	};

	virtual void
	handle_ctl_detached(crofctl& ctl) {
		// destroy crofctl object, when is was created upon an incoming connection from a peer entity
		if (ctl.remove_on_channel_termination()) {
			ctls_detached.insert(ctl.get_ctlid());
			push_on_eventqueue(EVENT_CTL_DETACHED);
		} else {
			handle_ctl_close(ctl);
		}
	};

	virtual void
	handle_timeout(
			int opaque, void* data = (void*)0) {
		switch (opaque) {
		case TIMER_RUN_ENGINE: {
			work_on_eventqueue();
		} break;
		default: {
			// do nothing
		};
		}
	};

	void
	push_on_eventqueue(
			enum crofbase_event_t event = EVENT_NONE) {
		if (EVENT_NONE != event) {
			eventqueue.push_back(event);
		}
		register_timer(TIMER_RUN_ENGINE, rofl::ctimespec(/*second(s)=*/0));
	};

	void
	work_on_eventqueue() {
		while (not eventqueue.empty()) {
			crofbase_event_t event = eventqueue.front();
			eventqueue.pop_front();
			switch (event) {
			case EVENT_CTL_DETACHED: {
				event_ctls_detached();
			} break;
			case EVENT_DPT_DETACHED: {
				event_dpts_detached();
			} break;
			default: {
				// do nothing for unknown event types
			};
			}
		}
	};

	void
	event_ctls_detached() {
		for (std::set<rofl::cctlid>::iterator
				it = ctls_detached.begin(); it != ctls_detached.end(); ++it) {
			handle_ctl_close(rofl::crofctl::get_ctl(*it));
			rofl::logging::info << "[rofl-common][crofbase] "
					<< "dropping crofctl instance, ctlid:" << it->str() << std::endl;
			drop_ctl(*it);
		}
		ctls_detached.clear();
	};

	void
	event_dpts_detached() {
		for (std::set<rofl::cdptid>::iterator
				it = dpts_detached.begin(); it != dpts_detached.end(); ++it) {
			handle_dpt_close(rofl::crofdpt::get_dpt(*it));
			rofl::logging::info << "[rofl-common][crofbase] "
					<< "dropping crofdpt instance, dptid:" << it->str() << std::endl;
			drop_dpt(*it);
		}
		dpts_detached.clear();
	};

private:

	/**< set of all active crofbase instances */
	static std::set<crofbase*> 		rofbases;

	/**< set of active controller connections */
	std::map<cctlid, crofctl*>		rofctls;
	/**< set of active data path connections */
	std::map<cdptid, crofdpt*>		rofdpts;
	/**< listening sockets for incoming connections from datapath elements */
	std::set<csocket*>				dpt_sockets;
	/**< listening sockets for incoming connections from controller entities */
	std::set<csocket*>				ctl_sockets;
	// supported OpenFlow versions
	rofl::openflow::cofhello_elem_versionbitmap
									versionbitmap;
	// pending OpenFlow transactions
	ctransactions					transactions;
	// generation_id used for roles initially defined?
	bool							generation_is_defined;
	// cached generation_id as defined by OpenFlow
	uint64_t						cached_generation_id;
	// event-queue
	std::list<enum crofbase_event_t>
									eventqueue;
	// ephemeral set of crofdpt instances in detached state
	std::set<rofl::cdptid>			dpts_detached;
	// ephemeral set of crofctl instances in detached state
	std::set<rofl::cctlid>			ctls_detached;
};

}; // end of namespace

#endif


