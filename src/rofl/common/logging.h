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

#define __LOGGING_STR_PASTE(a, b) a ## b
#define __LOGGING_COND_PRINT(LEVEL, DESC)\
	 if(rofl::logging::curr_level >= rofl::logging::LEVEL ) DESC

	static std::filebuf devnull;
	static std::filebuf logfile;
	static std::ostream emerg_;
#define LOGGING_EMERG __LOGGING_COND_PRINT(EMERG, rofl::logging::emerg_)
	static std::ostream alert_;
#define LOGGING_ALERT __LOGGING_COND_PRINT(ALERT, rofl::logging::alert_)
	static std::ostream crit_;
#define LOGGING_CRIT __LOGGING_COND_PRINT(CRIT, rofl::logging::crit_)
	static std::ostream error_;
#define LOGGING_ERROR __LOGGING_COND_PRINT(ERROR, rofl::logging::error_)
	static std::ostream warn_;
#define LOGGING_WARN __LOGGING_COND_PRINT(WARN, rofl::logging::warn_)
	static std::ostream notice_;
#define LOGGING_NOTICE __LOGGING_COND_PRINT(NOTICE, rofl::logging::notice_)
	static std::ostream info_;
#define LOGGING_INFO __LOGGING_COND_PRINT(INFO, rofl::logging::info_)
	static std::ostream debug_;
#define LOGGING_DEBUG __LOGGING_COND_PRINT(DBG, rofl::logging::debug_)
	static std::ostream debug2_;
#define LOGGING_DEBUG2 __LOGGING_COND_PRINT(DBG2, rofl::logging::debug2_)
	static std::ostream debug3_;
#define LOGGING_DEBUG3 __LOGGING_COND_PRINT(DBG3, rofl::logging::debug3_)
	static std::ostream trace_;
#define LOGGING_TRACE __LOGGING_COND_PRINT(TRACE, rofl::logging::trace_)
	static std::streamsize width;


public:


	/*
	* Logging levels. It is recommended to use these identifiers
	* for code portability while setting debug level
	*/
	static const unsigned int EMERG 	= 0;
	static const unsigned int ALERT 	= 1;
	static const unsigned int CRIT 		= 2;
	static const unsigned int ERROR 	= 3;
	static const unsigned int WARN		= 4;
	static const unsigned int NOTICE	= 5;
	static const unsigned int INFO		= 6;
	static const unsigned int DBG		= 7; //Prevent DEBUG macro expansion
	static const unsigned int DBG2		= 8; //Prevent DEBUG macro expansion
	static const unsigned int DBG3		= 9; //Prevent DEBUG macro expansion
	static const unsigned int TRACE		= 10;

	/**
	 * Initialize C++ logging facility
	 */
	static void
	init();

	/**
	 * Destroy C++ logging facility resources
	 */
	static void
	close();

	/**
	 * Set the debug level. It is recommended, for portability reasons to use
	 * logging::EMERG, logging::ALERT ... constants
	 */
	static void
	set_debug_level(
			unsigned int debug_level);

	static unsigned int curr_level;
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
	static void inc(unsigned int width) {
		indent::width += width;
	};
	static void dec(unsigned int width) {
		indent::width = (indent::width >= width) ? (indent::width - width) : 0;
	};
	static void null() {
		indent::width = 0;
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
