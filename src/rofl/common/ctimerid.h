/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ctimerid.h
 *
 *  Created on: 16.05.2014
 *      Author: andreas
 */

#ifndef CTIMERID_H_
#define CTIMERID_H_

#include <inttypes.h>
#include <iostream>
#include "rofl/common/logging.h"

namespace rofl {

class ctimerid {

	/*
	 * static members and methods
	 */
	static uint32_t next_timer_id;

	static uint32_t get_next_timer_id();


	/*
	 * non-static members and methods
	 */

	uint32_t 		tid;

public:

	/**
	 *
	 */
	ctimerid();

	/**
	 *
	 */
	~ctimerid();

	/**
	 *
	 */
	ctimerid(
			ctimerid const& tid);

	/**
	 *
	 */
	ctimerid&
	operator= (
			ctimerid const& tid);

	/**
	 *
	 */
	bool
	operator== (
			ctimerid const& tid);

public:

	/**
	 *
	 */
	uint32_t&
	set_tid() {
		return tid;
	};

	/**
	 *
	 */
	uint32_t const&
	get_tid() const { return tid; };

	/**
	 *
	 */
	void
	set_tid(uint32_t tid) {
		this->tid = tid;
	};

public:

	friend std::ostream&
	operator<< (std::ostream& os, ctimerid const& timerid) {
		os << rofl::indent(0) << "<ctimerid tid:0x" << std::hex << timerid.tid << std::dec
				<< " this:" << &timerid << " >" << std::endl;
		//os << rofl::indent(2) << "<this: 0x" << std::hex << (&timerid) << std::dec << " >" << std::endl;
		return os;
	};
};

};

#endif /* CTIMERID_H_ */
