/*
 * cevents.h
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#ifndef CEVENTS_H_
#define CEVENTS_H_

#include <list>
#include <iostream>

#include "rofl/common/cevent.h"
#include "rofl/common/logging.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/croflexception.h"

namespace rofl {

class eEventsBase 		: public RoflException {};
class eEventsNotFound	: public eEventsBase {};

class cevents {

	std::list<cevent> 	events;
	PthreadRwLock		rwlock;

public:

	/**
	 *
	 */
	cevents();

	/**
	 *
	 */
	cevents(cevents const& events);

	/**
	 *
	 */
	cevents&
	operator= (cevents const& events);

	/**
	 *
	 */
	virtual
	~cevents();

public:

	/**
	 *
	 */
	void
	add_event(cevent const& event);

	/**
	 *
	 */
	cevent
	get_event();

	/**
	 *
	 */
	bool
	empty();

	/**
	 *
	 */
	void
	clear();

public:

	friend std::ostream&
	operator<< (std::ostream& os, cevents const& events) {
		os << indent(0) << "<cevents: >" << std::endl;
			indent i(2);
			for (std::list<cevent>::const_iterator
					it = events.events.begin(); it != events.events.end(); ++it) {
				os << (*it) << " ";
			}
		return os;
	};
};

};

#endif /* CEVENTS_H_ */
