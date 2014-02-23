/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFPORTS_H_
#define COFPORTS_H_ 1

#include <algorithm>

#include "rofl/common/cvastring.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/coflist.h"
#include "rofl/common/openflow/openflow.h"

#include "rofl/common/openflow/cofport.h"

namespace rofl
{

class ePortsBase 		: public RoflException {}; // base error class cofinlist
class ePortsInval 		: public ePortsBase {}; // invalid parameter
class ePortsNotFound 	: public ePortsBase {}; // element not found
class ePortsOutOfRange 	: public ePortsBase {}; // out of range

class cofports : public std::map<uint32_t, cofport*>
{
	uint8_t 				ofp_version;

public: // iterators

	typedef typename std::map<uint32_t, cofport*>::iterator iterator;
	typedef typename std::map<uint32_t, cofport*>::const_iterator const_iterator;

	typedef typename std::map<uint32_t, cofport*>::reverse_iterator reverse_iterator;
	typedef typename std::map<uint32_t, cofport*>::const_reverse_iterator const_reverse_iterator;

public: // methods

	/**
	 *
	 */
	cofports(
			uint8_t ofp_version = openflow::OFP_VERSION_UNKNOWN);

	/**
	 *
	 */
	cofports(
			uint8_t ofp_version, uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	cofports(
			cofports const& ports);

	/**
	 *
	 */
	cofports&
	operator= (
			cofports const& ports);

	/**
	 *
	 */
	virtual
	~cofports();

public:

	/**
	 *
	 */
	void
	clear();


	/**
	 *
	 */
	void
	unpack(uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	void
	pack(uint8_t* buf, size_t buflen);


	/** returns required length for array of struct ofp_instruction
	 * for all instructions defined in this->invec
	 */
	size_t
	length() const;

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

	/**
	 *
	 */
	cofport&
	add_port(uint32_t portno);

	/**
	 *
	 */
	cofport&
	set_port(uint32_t portno);

	/**
	 *
	 */
	cofport const&
	get_port(uint32_t portno) const;

	/**
	 *
	 */
	void
	drop_port(uint32_t portno);

	/**
	 *
	 */
	bool
	has_port(uint32_t portno);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofports const& ports) {
		os << indent(0) << "<cofports size:" << (int)ports.size() << " >" << std::endl;;
		indent i(2);
		for (cofports::const_iterator
				it = ports.begin(); it != ports.end(); ++it) {
			os << *(it->second);
		}
#if 0
		for (cofportlist::const_iterator
				it = ofportlist.begin(); it != ofportlist.end(); ++it) {
			os << *it;
		}
#endif
		return os;
	};
};

}; // end of namespace

#endif
