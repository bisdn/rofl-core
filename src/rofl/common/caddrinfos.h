/*
 * caddrinfos.h
 *
 *  Created on: 12.06.2014
 *      Author: andreas
 */

#ifndef CADDRINFOS_H_
#define CADDRINFOS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <map>
#include <string>

#include "rofl/common/caddrinfo.h"

namespace rofl {

class caddrinfos {
public:

	/**
	 *
	 */
	caddrinfos() {};

	/**
	 *
	 */
	~caddrinfos() {};

public:

	/**
	 *
	 */
	const std::string&
	get_node() const { return node; };

	/**
	 *
	 */
	std::string&
	set_node() { return node; };

	/**
	 *
	 */
	void
	set_node(const std::string& node) { this->node = node; };

	/**
	 *
	 */
	const std::string&
	get_service() const { return service; };

	/**
	 *
	 */
	std::string&
	set_service() { return service; };

	/**
	 *
	 */
	void
	set_service(const std::string& service) { this->service = service; };

	/**
	 *
	 */
	const caddrinfo&
	get_ai_hints() const { return hints; };

	/**
	 *
	 */
	caddrinfo&
	set_ai_hints() { return hints; };

	/**
	 *
	 */
	const std::map<unsigned int, caddrinfo>&
	get_addr_infos() const { return ais; };

public:

	/**
	 *
	 */
	void
	resolve();

	/**
	 *
	 */
	void
	clear() { ais.clear(); };

	/**
	 *
	 */
	unsigned int
	size() const { return ais.size(); };

	/**
	 *
	 */
	caddrinfo&
	add_addr_info(unsigned int index);

	/**
	 *
	 */
	caddrinfo&
	set_addr_info(unsigned int index);

	/**
	 *
	 */
	void
	drop_addr_info(unsigned int index);

	/**
	 *
	 */
	const caddrinfo&
	get_addr_info(unsigned int index) const;

	/**
	 *
	 */
	bool
	has_addr_info(unsigned int index) const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, const caddrinfos& dns) {
		os << rofl::indent(0) << "<caddrinfos node:" << dns.get_node()
				<< " service:" << dns.get_service() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<unsigned int, caddrinfo>::const_iterator
				it = dns.get_addr_infos().begin(); it != dns.get_addr_infos().end(); ++it) {
			os << it->second;
		}
		return os;
	};

private:

	std::string node;
	std::string service;
	caddrinfo	hints;

	std::map<unsigned int, caddrinfo>	ais;
};

}; // end of namespace rofl

#endif /* CADDRINFOS_H_ */
