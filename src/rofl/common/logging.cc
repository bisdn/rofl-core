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

std::ostream* logging::os_emerg 	= &(std::cerr);
std::ostream* logging::os_alert 	= &(std::cerr);
std::ostream* logging::os_crit 		= &(std::cerr);
std::ostream* logging::os_error 	= &(std::cerr);
std::ostream* logging::os_warn	 	= &(std::cerr);
std::ostream* logging::os_notice 	= &(std::cerr);
std::ostream* logging::os_info	 	= &(std::cerr);
std::ostream* logging::os_debug 	= &(std::cerr);
