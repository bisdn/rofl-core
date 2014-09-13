/*
 * cofmsg_async_config.h
 *
 *  Created on: 15.06.2013
 *      Author: andi
 */

#ifndef COFMSG_ASYNC_CONFIG_H_
#define COFMSG_ASYNC_CONFIG_H_ 1

#include "rofl/common/openflow/messages/cofmsg.h"
#include "rofl/common/openflow/cofasyncconfig.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_get_async_config_request :
	public cofmsg
{
public:


	/** constructor
	 *
	 */
	cofmsg_get_async_config_request(
			uint8_t of_version = 0,
			uint32_t xid = 0);


	/**
	 *
	 */
	cofmsg_get_async_config_request(
			cofmsg_get_async_config_request const& get_async_config_request);


	/**
	 *
	 */
	cofmsg_get_async_config_request&
	operator= (
			cofmsg_get_async_config_request const& get_async_config_request);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_get_async_config_request();


	/**
	 *
	 */
	cofmsg_get_async_config_request(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_get_async_config_request const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_get_async_config_request >" << std::endl;
		return os;
	};
};








/**
 *
 */
class cofmsg_get_async_config_reply :
	public cofmsg
{
private:

	rofl::openflow::cofasync_config			async_config;


public:


	/** constructor
	 *
	 */
	cofmsg_get_async_config_reply(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			rofl::openflow::cofasync_config const& async_config = rofl::openflow::cofasync_config());


	/**
	 *
	 */
	cofmsg_get_async_config_reply(
			cofmsg_get_async_config_reply const& config);


	/**
	 *
	 */
	cofmsg_get_async_config_reply&
	operator= (
			cofmsg_get_async_config_reply const& config);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_get_async_config_reply();


	/**
	 *
	 */
	cofmsg_get_async_config_reply(cmemory *memarea);

public:


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();


public:

	/**
	 *
	 */
	rofl::openflow::cofasync_config&
	set_async_config() { return async_config; };

	/**
	 *
	 */
	rofl::openflow::cofasync_config const&
	get_async_config() const { return async_config; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_get_async_config_reply const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_get_async_config_reply >" << std::endl;
		rofl::indent i(4);
		os << msg.async_config;
		return os;
	};
};






/**
 *
 */
class cofmsg_set_async_config :
	public cofmsg
{
private:

	rofl::openflow::cofasync_config			async_config;


public:


	/** constructor
	 *
	 */
	cofmsg_set_async_config(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			rofl::openflow::cofasync_config const& async_config = rofl::openflow::cofasync_config());


	/**
	 *
	 */
	cofmsg_set_async_config(
			cofmsg_set_async_config const& config);


	/**
	 *
	 */
	cofmsg_set_async_config&
	operator= (
			cofmsg_set_async_config const& config);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_set_async_config();


	/**
	 *
	 */
	cofmsg_set_async_config(cmemory *memarea);

public:


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();


public:

	/**
	 *
	 */
	rofl::openflow::cofasync_config&
	set_async_config() { return async_config; };

	/**
	 *
	 */
	rofl::openflow::cofasync_config const&
	get_async_config() const { return async_config; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_set_async_config const& msg) {
		os << dynamic_cast<cofmsg const&>( msg );
		os << indent(2) << "<cofmsg_set_async_config >" << std::endl;
		rofl::indent i(4);
		os << msg.async_config;
		return os;
	};
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_ASYNC_CONFIG_H_ */
