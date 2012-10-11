/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cadapt.h
 *
 *  Created on: 30.08.2012
 *      Author: andreas
 */

#ifndef CADAPT_H
#define CADAPT_H 1

#include <list>
#include <string>

#include <rofl/common/cerror.h>
#include <rofl/common/cpacket.h>
#include <rofl/common/cvastring.h>
#include <rofl/common/openflow/cofmatch.h>
#include <rofl/common/openflow/cofaclist.h>
#include <rofl/common/openflow/cflowentry.h>
#include <rofl/common/openflow/extensions/cfspentry.h>
#include <rofl/platform/unix/csyslog.h>
#include <rofl/common/cxidstore.h>


// forward declarations, see below for the definitions
class cadapt_ctl;
class cadapt_dpt;



class eAdaptBase 				: public cerror {};
class eAdaptInval 				: public eAdaptBase {};
class eAdaptNotFound			: public eAdaptBase {};
class eAdaptNotConnected		: public eAdaptBase {};






/**
 * @class	cadapt_ctl
 */
class cadapt_ctl :
		public cfspentry_owner,
		public cxidowner
{
/*
 * data structured
 */
protected:

		cadapt_dpt		*dpt;


/*
 * methods
 */
public:


		/**
		 *
		 */
		cadapt_ctl(
				cadapt_dpt *dpt = (cadapt_dpt*)0) :
			dpt(dpt)
		{
			bind(dpt);
		};


		/**
		 *
		 */
		virtual
		~cadapt_ctl()
		{
			unbind(dpt);
		};


		/**
		 *
		 */
		virtual void
		bind(
				cadapt_dpt *dpt);


		/**
		 *
		 */
		virtual void
		unbind(
				cadapt_dpt *dpt);


public: // auxiliary methods


		/**
		 *
		 */
		virtual uint32_t
		ctl_get_free_portno(
				cadapt_dpt *dpt,
				uint32_t proposed = 0)
			throw (eAdaptNotFound) = 0;



public: // methods offered to cadapt instances by cadapt_owner


		/**
		 */
		virtual void
		ctl_handle_error(
				cadapt_dpt *dpt,
				uint16_t type,
				uint16_t code,
				uint8_t *data = 0,
				size_t datalen = 0) = 0;


		/**
		 *
		 */
		virtual void
		ctl_handle_port_status(
				cadapt_dpt *dpt,
				uint8_t reason,
				cofport *ofport) = 0;



		/**
		 *
		 */
		virtual void
		ctl_handle_packet_in(
				cadapt_dpt *dpt,
				uint32_t buffer_id,
				uint16_t total_len,
				uint8_t table_id,
				uint8_t reason,
				cofmatch& match,
				cpacket& pack) = 0;


		/**
		 *
		 */
		virtual void
		ctl_handle_stats_reply(
				cadapt_dpt *dpt,
				uint32_t xid,
				uint16_t type,
				uint8_t* body,
				size_t bodylen) = 0;


protected:


		/**
		 * called once a dpt has bound to this ctl
		 */
		virtual void
		bound(
				cadapt_dpt *dpt) = 0;


		/**
		 * called once a dpt has unbound from this ctl
		 */
		virtual void
		unbound(
				cadapt_dpt *dpt) = 0;

};







/**
 *
 */
class cadapt_dpt :
		public csyslog
{
/*
 *  data structures
 */
protected:

		cadapt_ctl			*ctl;		// adapter container hosting this cadapt_dpt instance


/*
 * methods
 */
public:


		/**
		 */
		cadapt_dpt(
				cadapt_ctl *ctl = (cadapt_ctl*)0) :
			ctl(ctl)
		{
			bind(ctl);
		};


		/**
		 */
		virtual
		~cadapt_dpt()
		{
			unbind(ctl);
		};


		/**
		 *
		 */
		virtual void
		bind(
				cadapt_ctl *ctl);


		/**
		 *
		 */
		virtual void
		unbind(
				cadapt_ctl *ctl);


public:


		/**
		 */
		virtual void
		dpt_handle_packet_out(
				cadapt_ctl *ctl,
				uint32_t buffer_id,
				uint32_t in_port,
				cofaclist& aclist,
				cpacket& pack) = 0;


		/**
		 *
		 */
		virtual void
		dpt_handle_flow_mod(
				cadapt_ctl *ctl,
				cflowentry& fe) = 0;


		/**
		 *
		 */
		virtual void
		dpt_handle_port_mod(
				cadapt_ctl *ctl,
				uint32_t port_no,
				uint32_t config,
				uint32_t mask,
				uint32_t advertise) = 0;


		/**
		 */
		virtual void
		dpt_flowspace_open(
				cadapt_ctl* ctl,
				cofmatch& match) throw (eAdaptNotConnected) = 0;


		/**
		 */
		virtual void
		dpt_flowspace_close(
				cadapt_ctl* ctl,
				cofmatch& match) throw (eAdaptNotConnected) = 0;


		/**
		 *
		 */
		virtual cofaclist
		dpt_filter_match(
				cadapt_ctl *ctl,
				uint32_t port_no,
				cofmatch& match) throw (eAdaptNotFound) = 0;


		/**
		 *
		 */
		virtual cofaclist
		dpt_filter_action(
				cadapt_ctl *ctl,
				uint32_t port_no,
				cofaction& action) throw (eAdaptNotFound) = 0;


		/**
		 *
		 */
		virtual void
		dpt_filter_packet(
				cadapt_ctl *ctl,
				uint32_t port_no,
				cpacket& pack) throw (eAdaptNotFound) = 0;


		/**
		 *
		 */
		virtual cofport*
		dpt_find_port(
				cadapt_ctl *ctl,
				uint32_t port_no)
						throw (eAdaptNotFound) = 0;


		/**
		 * @return xid of stats-request sent
		 */
		virtual uint32_t
		dpt_handle_stats_request(
				cadapt_ctl *ctl,
				uint32_t port_no,
				uint16_t type,
				uint8_t *body,
				size_t bodylen)
						throw (eAdaptNotFound) = 0;


protected:


		/**
		 * called once a ctl has bound to this dpt
		 */
		virtual void
		bound(
				cadapt_ctl *ctl) = 0;



		/**
		 * called once a ctl has unbound from this dpt
		 */
		virtual void
		unbound(
				cadapt_ctl *ctl) = 0;
};




/**
 * @class	cadapt
 *
 * A base class for any new adapter: inherits properties from both ctl and dpt,
 * so it acts as a control and dpath entity at the same time.
 */
class cadapt :
	public cadapt_ctl,
	public cadapt_dpt
{
private:

		std::string			info;

public:


		/**
		 *
		 */
		cadapt(
				cadapt_ctl *ctl = (cadapt_ctl*)0,
				cadapt_dpt *dpt = (cadapt_dpt*)0) :
					cadapt_ctl(dpt),
					cadapt_dpt(ctl)
		{};


		/**
		 *
		 */
		virtual
		~cadapt()
		{};


		/**
		 *
		 */
		const char*
		c_str()
		{
			cvastring vas;
			info.append(vas("cadapt(%p)", this));
			return info.c_str();
		};
};




#endif /* CADAPT_H */
