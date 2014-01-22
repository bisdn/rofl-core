/*
 * logging.cc
 *
 *  Created on: 23.11.2013
 *      Author: andreas
 *
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "logging.h"

using namespace rofl;

bool logging::initialized = false;
std::filebuf logging::devnull;
std::ostream logging::emerg	 (&logging::devnull);
std::ostream logging::alert  (&logging::devnull);
std::ostream logging::crit   (&logging::devnull);
std::ostream logging::error  (&logging::devnull);
std::ostream logging::warn   (&logging::devnull);
std::ostream logging::notice (&logging::devnull);
std::ostream logging::info   (&logging::devnull);
std::ostream logging::debug  (&logging::devnull);
std::streamsize logging::width(70);
unsigned int indent::width(0);


void
logging::init()
{
	if (not logging::initialized) {
		logging::devnull.open("/dev/null", std::ios::out);
		logging::initialized = true;
	}
}


void
logging::close()
{
	logging::devnull.close();
	logging::initialized = false;
}


void
logging::set_logfile(
			enum logging_level level,
			std::string const& filename)
{
	switch (level) {
	case LOGGING_EMERG:		logging::emerg .rdbuf((new std::filebuf())->open(filename.c_str(), std::ios::out)); break;
	case LOGGING_ALERT:		logging::alert .rdbuf((new std::filebuf())->open(filename.c_str(), std::ios::out)); break;
	case LOGGING_CRIT:		logging::crit  .rdbuf((new std::filebuf())->open(filename.c_str(), std::ios::out)); break;
	case LOGGING_ERROR:		logging::error .rdbuf((new std::filebuf())->open(filename.c_str(), std::ios::out)); break;
	case LOGGING_WARN:		logging::warn  .rdbuf((new std::filebuf())->open(filename.c_str(), std::ios::out)); break;
	case LOGGING_NOTICE:	logging::notice.rdbuf((new std::filebuf())->open(filename.c_str(), std::ios::out)); break;
	case LOGGING_INFO:		logging::info  .rdbuf((new std::filebuf())->open(filename.c_str(), std::ios::out)); break;
	case LOGGING_DEBUG:		logging::debug .rdbuf((new std::filebuf())->open(filename.c_str(), std::ios::out)); break;
	}
}


void
logging::set_debug_level(
			unsigned int debug_level)
{
	logging::emerg .rdbuf(&logging::devnull);
	logging::alert .rdbuf(&logging::devnull);
	logging::crit  .rdbuf(&logging::devnull);
	logging::error .rdbuf(&logging::devnull);
	logging::warn  .rdbuf(&logging::devnull);
	logging::notice.rdbuf(&logging::devnull);
	logging::info  .rdbuf(&logging::devnull);
	logging::debug .rdbuf(&logging::devnull);

	if (debug_level >= 0) {
		logging::emerg .rdbuf(std::cerr.rdbuf());
	}
	if (debug_level >= 1) {
		logging::alert .rdbuf(std::cerr.rdbuf());
	}
	if (debug_level >= 2) {
		logging::crit  .rdbuf(std::cerr.rdbuf());
	}
	if (debug_level >= 3) {
		logging::error .rdbuf(std::cout.rdbuf());
	}
	if (debug_level >= 4) {
		logging::warn  .rdbuf(std::cout.rdbuf());
	}
	if (debug_level >= 5) {
		logging::notice.rdbuf(std::cout.rdbuf());
	}
	if (debug_level >= 6) {
		logging::info  .rdbuf(std::cout.rdbuf());
	}
	if (debug_level >= 7) {
		logging::debug .rdbuf(std::cout.rdbuf());
	}
}


