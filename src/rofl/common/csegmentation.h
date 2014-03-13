/*
 * csegmentation.h
 *
 *  Created on: 13.03.2014
 *      Author: andreas
 */

#ifndef CSEGMENTATION_H_
#define CSEGMENTATION_H_

#include <inttypes.h>

#include <iostream>
#include <map>

#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/csegmsg.h"

namespace rofl {
namespace openflow {

class csegmentation {

	std::map<uint32_t, csegmsg> 		segmsgs;		// all current pending transactions with fragments

public:

	/**
	 *
	 */
	csegmentation();

	/**
	 *
	 */
	virtual
	~csegmentation();

	/**
	 *
	 */
	csegmentation(
			csegmentation const& seg);

	/**
	 *
	 */
	csegmentation&
	operator= (csegmentation const& seg);

public:

	/**
	 *
	 */
	csegmsg&
	add_transaction(uint32_t xid);

	/**
	 *
	 */
	void
	drop_transaction(uint32_t xid);

	/**
	 *
	 */
	csegmsg&
	set_transaction(uint32_t xid);

	/**
	 *
	 */
	csegmsg const&
	get_transaction(uint32_t xid) const;

	/**
	 *
	 */
	bool
	has_transaction(uint32_t xid);

public:

	friend std::ostream&
	operator<< (std::ostream& os, csegmentation const& seg) {
		os << rofl::indent(0) << "<csegmentation #transactions:" << (int)seg.segmsgs.size() << " >" << std::endl;
		rofl::indent i(2);
		for (std::map<uint32_t, csegmsg>::const_iterator
				it = seg.segmsgs.begin(); it != seg.segmsgs.end(); ++it) {
			os << it->second;
		}
		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* CSEGMENTATION_H_ */
