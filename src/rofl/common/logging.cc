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

std::filebuf logging::devnull;
std::ostream logging::emerg_	 (&logging::devnull);
std::ostream logging::alert_  (&logging::devnull);
std::ostream logging::crit_   (&logging::devnull);
std::ostream logging::error_  (&logging::devnull);
std::ostream logging::warn_   (&logging::devnull);
std::ostream logging::notice_ (&logging::devnull);
std::ostream logging::info_   (&logging::devnull);
std::ostream logging::debug_  (&logging::devnull);
std::ostream logging::debug2_ (&logging::devnull);
std::ostream logging::debug3_ (&logging::devnull);
std::ostream logging::trace_  (&logging::devnull);

unsigned int logging::curr_level(EMERG);
std::streamsize logging::width(70);
unsigned int indent::width(0);


void
logging::init()
{
	if (not logging::devnull.is_open()) {
		logging::devnull.open("/dev/null", std::ios::out);
	}
}


void
logging::close()
{
	if (logging::devnull.is_open()) {
		logging::devnull.close();
	}
}



void
logging::set_debug_level(
			unsigned int debug_level)
{
	logging::init();

	// EMERG
	logging::emerg_ .rdbuf(std::cerr.rdbuf());

	// ALERT
	if (debug_level >= ALERT) {
		logging::alert_.rdbuf(std::cerr.rdbuf());
	} else {
		logging::alert_.rdbuf(&logging::devnull);
	}

	// CRIT
	if (debug_level >= CRIT) {
		logging::crit_.rdbuf(std::cerr.rdbuf());
	} else {
		logging::crit_.rdbuf(&logging::devnull);
	}

	// ERROR
	if (debug_level >= ERROR) {
		logging::error_.rdbuf(std::cerr.rdbuf());
	} else {
		logging::error_.rdbuf(&logging::devnull);
	}

	// WARN
	if (debug_level >= WARN) {
		logging::warn_.rdbuf(std::cerr.rdbuf());
	} else {
		logging::warn_.rdbuf(&logging::devnull);
	}

	// NOTICE
	if (debug_level >= NOTICE) {
		logging::notice_.rdbuf(std::cerr.rdbuf());
	} else {
		logging::notice_.rdbuf(&logging::devnull);
	}

	// INFO
	if (debug_level >= INFO) {
		logging::info_.rdbuf(std::cerr.rdbuf());
	} else {
		logging::info_.rdbuf(&logging::devnull);
	}

	// DEBUG
	if (debug_level >= DBG) {
		logging::debug_.rdbuf(std::cerr.rdbuf());
	} else {
		logging::debug_.rdbuf(&logging::devnull);
	}

	// DEBUG2
	if (debug_level >= DBG2) {
		logging::debug2_.rdbuf(std::cerr.rdbuf());
	} else {
		logging::debug2_.rdbuf(&logging::devnull);
	}

	// DEBUG3
	if (debug_level >= DBG3) {
		logging::debug3_.rdbuf(std::cerr.rdbuf());
	} else {
		logging::debug3_.rdbuf(&logging::devnull);
	}

	// TRACE 
	if (debug_level >= TRACE) {
		logging::trace_.rdbuf(std::cerr.rdbuf());
	} else {
		logging::trace_.rdbuf(&logging::devnull);
	}

	curr_level = debug_level;
}

