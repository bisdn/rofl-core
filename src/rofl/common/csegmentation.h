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
#include "rofl/common/ciosrv.h"
#include "rofl/common/ctimerid.h"

namespace rofl {

class eSegmentationBase 		: public RoflException {};
class eSegmentationInval		: public eSegmentationBase {};
class eSegmentationNotFound		: public eSegmentationBase {};

class csegmentation :
		public ciosrv
{

	std::map<uint32_t, csegmsg> 		segmsgs;		// all current pending transactions with fragments

	enum csegmentation_timer_t {
		TIMER_CHECK_EXPIRATION = 1,
	};

	ctimerid							check_expiration_id;	// timer-id
	time_t								check_expiration_interval;

	static time_t const DEFAULT_CHECK_EXPIRATION_INTERVAL = 8; 	// seconds

public:

	/**
	 *
	 */
	csegmentation(
			time_t check_expiration_interval = DEFAULT_CHECK_EXPIRATION_INTERVAL,
			pthread_t tid = 0);

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

private:

	/**
	 *
	 */
	virtual void
	handle_timeout(int opaque, void *data = (void*)0);

	/**
	 *
	 */
	void
	drop_expired_sessions();

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

}; // end of namespace rofl

#endif /* CSEGMENTATION_H_ */
