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
#include <rofl/common/openflow/extensions/cfspentry.h>
#include <rofl/platform/unix/csyslog.h>


// forward declarations, see below ...
class cadapt;
class cadapt_owner;



class eAdaptBase 				: public cerror {};
class eAdaptInval 				: public eAdaptBase {};
class eAdaptNotFound			: public eAdaptBase {};





/**
 *
 */
class cadapt :
	public csyslog,
	public cfspentry_owner
				// behaves like a cfspentry_owner for
				// flowspace registrations in cfwdelem::fsptable
{
/*
 *  data structures
 */
protected:

		cadapt_owner		*base;		// adapter container hosting this cadapter

private:

		size_t				 index;		// index for base->vector
		std::string 		 info;		// info string



/*
 * methods
 */
public:

	/**
	 */
	cadapt(
			cadapt_owner *base,
			unsigned int index = 0) throw (eAdaptInval);


	/**
	 */
	virtual
	~cadapt();


	/**
	 *
	 */
	virtual
	const char*
	c_str();


	/*
	 * friends
	 */
	friend class cadapt_owner;


public: // lightweight OpenFlow interface for access by cadapt_owner


#if 0
	/**
	 */
	virtual void
	handle_dpath_open(
			cofdpath *sw) {};


	/**
	 */
	virtual void
	handle_dpath_close(
			cofdpath *sw) {};
#endif


	/*
	 * do not implement the downward methods flow_mod and packet_out
	 * Outgoing commands will always be filtered and handled directly
	 * by cadapt_owner
	 */


	/**
	 */
	virtual void
	handle_packet_in(
			cofpacket *pack) { delete pack; };


	/**
	 */
	virtual void
	handle_error(
			uint16_t type,
			uint16_t code,
			uint8_t *data = 0,
			size_t datalen = 0) {};


	/**
	 */
	virtual void
	handle_port_status(
			uint8_t reason,
			cofport *port) {};


public:


	/**
	 *
	 */
	virtual void
	handle_port_mod(
			uint32_t port_no,
			uint32_t config,
			uint32_t mask,
			uint32_t advertise) {};


	/**
	 *
	 */
	virtual cofaclist
	filter_match(
			uint32_t port_no,
			cofmatch& match) throw (eAdaptNotFound)
	{ cofaclist actions; return actions; };


	/**
	 *
	 */
	virtual cofaclist
	filter_action(
			uint32_t port_no,
			cofaction& action) throw (eAdaptNotFound)
	{ cofaclist actions; return actions; };


	/**
	 *
	 */
	virtual void
	filter_packet(
			uint32_t port_no,
			cpacket *pack) throw (eAdaptNotFound)
	{};


	/**
	 *
	 */
	virtual cofport*
	find_port(
			uint32_t port_no)
					throw (eAdaptNotFound)
	{ throw eAdaptNotFound(); };
};



class cadapt_owner
{
/*
 * data structured
 */
protected:

		std::vector<std::list<cadapt*> > 		adapters;		// set of all adapters registered with cadapt_owner


/*
 * methods
 */
public:
		friend class cadapt;

		/**
		 *
		 */
		cadapt_owner(size_t n = 16) :
		adapters(n) {};


		/**
		 *
		 */
		virtual
		~cadapt_owner()
		{
			for (std::vector<std::list<cadapt*> >::iterator
					it = adapters.begin(); it != adapters.end(); ++it)
			{
				std::list<cadapt*>& adapts = (*it);

				while (not adapts.empty())
				{
					delete adapts.front();
				}
			}
		};


public: // auxiliary methods


		/**
		 *
		 */
		virtual uint32_t
		get_free_portno()
			throw (eAdaptNotFound) = 0;



public: // flowspace related methods


		/**
		 */
		virtual void
		flowspace_open(
				cadapt *adapt,
				cofmatch const& m = cofmatch() /* all wildcard */) = 0;


		/**
		 */
		virtual void
		flowspace_close(
				cadapt *adapt,
				cofmatch const& m = cofmatch() /* all wildcard */) = 0;



public: // methods offered to cadapt instances by cadapt_owner

		/*
		 * upwards methods
		 */

		/**
		 */
		virtual void
		send_port_status(
				cadapt *adapt,
				uint8_t reason,
				cofport *ofport) = 0;



		/**
		 */
		virtual void
		send_packet_in(
				cadapt *adapt,
				uint32_t buffer_id,
				uint16_t total_len,
				uint8_t table_id,
				uint8_t reason,
				cofmatch& match,
				cpacket& pack) = 0;




public: // methods offered to cadapt instances by cadapt_owner

		/*
		 * downwards methods
		 */

		/**
		 */
		virtual void
		send_packet_out(
				cadapt *adapt,
				uint32_t buffer_id,
				uint32_t in_port,
				cofaclist& aclist,
				cpacket& pack) = 0;
};

#endif /* CADAPT_H_ */
