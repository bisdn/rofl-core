/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CROFCTLIMPL_H
#define CROFCTLIMPL_H 1

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif


#include <map>
#include <string>

#include "crofchan.h"

#include "ciosrv.h"
#include "thread_helper.h"
#include "logging.h"
#include "openflow/cofmatch.h"
#include "openflow/extensions/cfspentry.h"
#include "openflow/cofhelloelemversionbitmap.h"



#include "rofl/common/crofctl.h"

namespace rofl
{


class crofctlImpl :
	public crofctl,
	public rofl::openflow::crofchan_env,
	public ciosrv,
	public cfspentry_owner
{

private: // data structures

	friend class crofbase;

	uint64_t						ctid;					// controller id (TODO: unique hash value per controller connection)
	crofbase 						*rofbase;				// parent crofbase instance
	uint16_t 						miss_send_len;			// config: miss_send_len
	std::set<cofmatch*> 			nspaces;				// list of cofmatch structures depicting controlled namespace
	bool 							role_initialized;		// true, when role values have been initialized properly
	uint16_t 						role;					// role of this controller instance
	uint64_t 						cached_generation_id;	// generation-id used by role requests
	rofl::openflow::crofchan		rofchan;				// OFP channel
	cxidstore						xidstore;


public: // methods



	/**
	 * @brief	Constructor for creating new cofctl instance from accepted socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 */
	crofctlImpl(
			crofbase *rofbase,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap);




	/**
	 * @brief	Constructor for creating new cofctl instance from accepted socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param newsd socket descriptor
	 */
	crofctlImpl(
			crofbase *rofbase,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
			int newsd);



	/**
	 * @brief	Constructor for creating new cofctl instance for connecting socket.
	 *
	 * @param rofbase pointer to crofbase instance
	 * @param ra peer remote address
	 * @param domain socket domain
	 * @param type socket type
	 * @param protocol socket protocol
	 *
	 */
	crofctlImpl(
			crofbase *rofbase,
			rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap,
			int reconnect_start_timeout,
			caddress const& ra,
			int domain,
			int type,
			int protocol);



	/**
	 * @brief	Destructor.
	 */
	virtual
	~crofctlImpl();

public:

	/**
	 * @brief	Returns OpenFlow version negotiated for control connection.
	 */
	virtual uint8_t
	get_version() const { return rofchan.get_version(); };

	/**
	 * @brief	Returns a reference to rofchan's cofhello_elem_versionbitmap instance
	 */
	rofl::openflow::cofhello_elem_versionbitmap&
	get_versionbitmap() { return rofchan.get_versionbitmap(); };

	/**
	 * @brief	Returns a reference to the associated crofchan instance
	 */
	rofl::openflow::crofchan&
	get_channel() { return rofchan; };

	/**
	 * @brief	Returns true, when the OFP control channel is up
	 */
	virtual bool
	is_established() const { return (rofchan.is_established()); };

	/**
	 * @brief	Returns true, when the control entity is in role SLAVE
	 */
	virtual bool
	is_slave() const;

	/**
	 * @brief	Returns cxidtrans instance associated with transaction ID xid.
	 *
	 * @param xid transaction ID
	 */
	virtual cxidtrans&
	transaction(
			uint32_t xid);

	/**
	 * @brief	Sends an OpenFlow message via this cofctl instance.
	 *
	 * @param msg pointer to cofmsg instance
	 */
	virtual void
	send_message(
			cofmsg *msg, uint8_t aux_id);

	/**
	 * @brief	Returns the current role of attached control entity.
	 */
	virtual uint32_t
	get_role() const { return role; };

	/**
	 * @brief	Sets the new role of attached control of attached control entity.
	 */
	virtual void
	set_role(uint32_t role) { this->role = role; };

	/**
	 *
	 */
	virtual caddress
	get_peer_addr() { return rofchan.get_conn(0).get_rofsocket().get_socket().raddr; };

public:

	virtual void
	handle_connected(rofl::openflow::crofchan *chan, uint8_t aux_id);

	virtual void
	handle_closed(rofl::openflow::crofchan *chan, uint8_t aux_id);

	virtual void
	recv_message(rofl::openflow::crofchan *chan, uint8_t aux_id, cofmsg *msg);

	virtual uint32_t
	get_async_xid(rofl::openflow::crofchan *chan);

	virtual uint32_t
	get_sync_xid(rofl::openflow::crofchan *chan);

	virtual void
	release_sync_xid(rofl::openflow::crofchan *chan, uint32_t xid);

private:

	/** handle incoming vendor message (ROFL extensions)
	 */
	void
	experimenter_rcvd(cofmsg_experimenter *pack, uint8_t aux_id);

	/** handle incoming FEATURE requests
	 */
	void
	features_request_rcvd(cofmsg_features_request *pack, uint8_t aux_id = 0);

	/**
	 *
	 */
	void
	features_reply_sent(cofmsg *pack);

	/**
	 *
	 */
	void
	get_config_request_rcvd(cofmsg_get_config_request *pack, uint8_t aux_id = 0);

	/**
	 *
	 */
	void
	get_config_reply_sent(cofmsg *pack);

	/**
	 *
	 */
	void
	set_config_rcvd(cofmsg_set_config *pack, uint8_t aux_id = 0);

	/** handle incoming PACKET-OUT messages
	 */
	void
	packet_out_rcvd(cofmsg_packet_out *pack, uint8_t aux_id = 0);

	/** handle incoming FLOW-MOD messages
	 */
	void
	flow_mod_rcvd(cofmsg_flow_mod *pack, uint8_t aux_id = 0);

	/** handle incoming GROUP-MOD messages
	 */
	void
	group_mod_rcvd(cofmsg_group_mod *pack, uint8_t aux_id = 0);

	/** handle incoming PORT-MOD messages
	 */
	void
	port_mod_rcvd(cofmsg_port_mod *pack, uint8_t aux_id = 0);

	/** handle incoming TABLE-MOD messages
	 */
	void
	table_mod_rcvd(cofmsg_table_mod *pack, uint8_t aux_id = 0);

	/** STATS-REQUEST received
	 *
	 */
	void
	stats_request_rcvd(cofmsg_stats *pack, uint8_t aux_id = 0);

	void
	desc_stats_request_rcvd(cofmsg_desc_stats_request* msg, uint8_t aux_id = 0);
	void
	table_stats_request_rcvd(cofmsg_table_stats_request* msg, uint8_t aux_id = 0);
	void
	port_stats_request_rcvd(cofmsg_port_stats_request* msg, uint8_t aux_id = 0);
	void
	flow_stats_request_rcvd(cofmsg_flow_stats_request* msg, uint8_t aux_id = 0);
	void
	aggregate_stats_request_rcvd(cofmsg_aggr_stats_request* msg, uint8_t aux_id = 0);
	void
	queue_stats_request_rcvd(cofmsg_queue_stats_request* msg, uint8_t aux_id = 0);
	void
	group_stats_request_rcvd(cofmsg_group_stats_request* msg, uint8_t aux_id = 0);
	void
	group_desc_stats_request_rcvd(cofmsg_group_desc_stats_request* msg, uint8_t aux_id = 0);
	void
	group_features_stats_request_rcvd(cofmsg_group_features_stats_request* msg, uint8_t aux_id = 0);
	void
	experimenter_stats_request_rcvd(cofmsg_experimenter_stats_request* msg, uint8_t aux_id = 0);

	/**
	 *
	 */
	void
	stats_reply_sent(cofmsg *pack);

	/** handle incoming ROLE-REQUEST messages
	 */
	void
	role_request_rcvd(cofmsg_role_request *pack, uint8_t aux_id = 0);

	/**
	 *
	 */
	void
	role_reply_sent(cofmsg *pack);

	/** handle incoming BARRIER request
	 */
	void
	barrier_request_rcvd(cofmsg_barrier_request *pack, uint8_t aux_id = 0);

	/** BARRIER reply sent back
	 *
	 */
	void
	barrier_reply_sent(cofmsg *pack);

	/**
	 *
	 */
	void
	queue_get_config_request_rcvd(cofmsg_queue_get_config_request *pack, uint8_t aux_id = 0);

	/**
	 *
	 */
	void
	queue_get_config_reply_sent(cofmsg *pack);

	/**
	 */
	void
	get_async_config_request_rcvd(cofmsg_get_async_config_request *pack, uint8_t aux_id = 0);

	/**
	 */
	void
	set_async_config_rcvd(cofmsg_set_async_config *pack, uint8_t aux_id = 0);

	/**
	 *
	 */
	void
	get_async_config_reply_sent(cofmsg *pack);

	/**
	 *
	 */
	void
	send_error_is_slave(cofmsg *pack);

	/**
	 *
	 */
	virtual void
	handle_timeout(
			int opaque);

private:

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
	handle_closed(
			csocket *socket,
			int sd);

private:

	/**
	 *
	 */
	void
	check_role();

	/**
	 *
	 */
	void
	send_message_via_ctl_channel(
			cofmsg *msg, uint8_t aux_id);

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofctlImpl const& ctl) {
		os << indent(0) << "<crofctlImpl ";
		os << "ctid:0x" << std::hex << (unsigned long long)(ctl.ctid) << std::dec << " ";
		os << ">" << std::endl;
		indent i(2);
		os << ctl.rofchan;
		return os;
	};
};


}; // end of namespace

#endif
