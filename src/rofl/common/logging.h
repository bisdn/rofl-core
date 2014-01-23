/*
 * logging.h
 *
 *  Created on: 23.11.2013
 *      Author: andreas
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LOGGING_H_
#define LOGGING_H_ 1

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

namespace rofl
{

class logging
{
public:

	static std::filebuf devnull;
	static std::filebuf logfile;
	static std::ostream emerg;
	static std::ostream alert;
	static std::ostream crit;
	static std::ostream error;
	static std::ostream warn;
	static std::ostream notice;
	static std::ostream info;
	static std::ostream debug;
	static std::streamsize width;

public:


#define DEFAULT_ROFL_LOGFILE "/var/log/rofl.log"

	/**
	 *
	 */
	static void
	init(std::string const& s_logfile = DEFAULT_ROFL_LOGFILE);

	/**
	 *
	 */
	static void
	close();

	/**
	 *
	 */
	static void
	set_debug_level(
			unsigned int debug_level);
};


class indent
{
	static unsigned int width;
	unsigned int my_width;
public:
	indent(unsigned int my_width = 0) :
		my_width(my_width) {
		indent::width += my_width;
	};
	virtual ~indent() {
		indent::width = (indent::width >= my_width) ? (indent::width - my_width) : 0;
	};
	friend std::ostream&
	operator<< (std::ostream& os, indent const& i) {
		if (indent::width) {
			os << std::setw(indent::width) << " " << std::setw(0);
		}
		return os;
	};
};


}; // end of namespace

#endif /* LOGGING_H_ */
