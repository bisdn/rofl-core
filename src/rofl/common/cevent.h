/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cvent.h
 *
 *  Created on: 26.01.2014
 *      Author: andreas
 */

#ifndef CVENT_H_
#define CVENT_H_

#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"

namespace rofl {

class cevent
{
public:

	/**
	 *
	 */
	cevent() : cmd(-1) {};

	/**
	 *
	 */
	cevent(int cmd) : cmd(cmd) {};

	/**
	 *
	 */
	cevent(int cmd, cmemory const& opaque) : cmd(cmd), opaque(opaque) {};

	/**
	 *
	 */
	virtual
	~cevent() {};

	/**
	 *
	 */
	cevent(cevent const& ioctl) {
		*this = ioctl;
	};

	/**
	 *
	 */
	cevent&
	operator= (cevent const& event) {
		if (this == &event)
			return *this;
		cmd 	= event.cmd;
		opaque 	= event.opaque;
		return *this;
	};

	/**
	 *
	 */
	int
	get_cmd() const { return cmd; };

	/**
	 *
	 */
	cmemory&
	get_opaque() { return opaque; };

public: // data structures

	int 	cmd; 	// command
	cmemory opaque; // additional data

public: // auxiliary classes

	class cevent_find_by_cmd {
		int cmd;
	public:
		cevent_find_by_cmd(int __cmd) :
			cmd(__cmd) {};
		bool operator() (cevent const* ev) {
			return (ev->cmd == cmd);
		};
	};

public:
	friend std::ostream&
	operator<< (std::ostream& os, cevent const& event) {
		os << indent(0) << "<cevent ";
		os << "cmd:" 	<< event.cmd 	<< " ";
		os << ">" << std::endl;
			indent i(2);
			os << event.opaque;
		return os;
	};
};


};

#endif /* CVENT_H_ */
