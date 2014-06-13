/*
 * caddressinfo.h
 *
 *  Created on: 12.06.2014
 *      Author: andreas
 */

#ifndef CADDRESSINFO_H_
#define CADDRESSINFO_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <string>

#include "rofl/common/csockaddr.h"

namespace rofl {

class caddress_info {
public:

	/**
	 *
	 */
	caddress_info();

	/**
	 *
	 */
	caddress_info(
	                const std::string& host,
	                const std::string& service,
	                int ai_flags,
	                int preferred_family,
	                int preferred_socktype,
	                int preferred_protocol);

	/**
	 *
	 */
	~caddress_info() {};


public:

	/**
	 *
	 */
	void
	resolve(
			const std::string& host,
			const std::string& service,
			int ai_flags,
			int preferred_family,
			int preferred_socktype,
			int preferred_protocol);

	/**
	 *
	 */
	const csockaddr&
	get_sockaddr() const { return saddr; };

private:

	std::string host;
	std::string service;
	int 		ai_flags;
	int 		preferred_family;
	int 		preferred_socktype;
	int 		preferred_protocol;

	int			domain;
	int			socktype;
	int			protocol;
	csockaddr	saddr;
};

}; // end of namespace rofl

#endif /* CADDRESSINFO_H_ */
