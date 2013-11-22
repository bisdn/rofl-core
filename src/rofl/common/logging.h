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

namespace rofl
{

class logging
{
	static std::ostream *os_emerg;
	static std::ostream *os_alert;
	static std::ostream *os_crit;
	static std::ostream *os_error;
	static std::ostream *os_warn;
	static std::ostream *os_notice;
	static std::ostream *os_info;
	static std::ostream *os_debug;

public:

	static std::ostream& emerg() 	{ return *os_emerg; }

	static std::ostream& alert() 	{ return *os_error; };

	static std::ostream& crit() 	{ return *os_crit; };

	static std::ostream& error() 	{ return *os_error; };

	static std::ostream& warn()		{ return *os_warn; };

	static std::ostream& notice()	{ return *os_notice; };

	static std::ostream& info()		{ return *os_info; };

	static std::ostream& debug()	{ return *os_debug; };
};

}; // end of namespace

#endif /* LOGGING_H_ */
