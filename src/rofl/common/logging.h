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

namespace rofl
{

class logging
{
public:

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

	enum logging_level {
		LOGGING_EMERG = 1,
		LOGGING_ALERT,
		LOGGING_CRIT,
		LOGGING_ERROR,
		LOGGING_WARN,
		LOGGING_NOTICE,
		LOGGING_INFO,
		LOGGING_DEBUG,
	};

	/**
	 *
	 */
	static void
	set_logfile(
			enum logging_level level,
			std::string const& filename);
};

}; // end of namespace

#endif /* LOGGING_H_ */
