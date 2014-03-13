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

	friend std::ostream&
	operator<< (std::ostream& os, csegmentation const& seg) {

		return os;
	};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* CSEGMENTATION_H_ */
