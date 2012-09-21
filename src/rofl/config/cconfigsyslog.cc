/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cconfigsyslog.cc
 *
 *  Created on: Oct 29, 2012
 *      Author: tobi
 */

#include "cconfigsyslog.h"

/*static */cconfigsyslog&
cconfigsyslog::getInstance()
{
	static cconfigsyslog instance;
	return instance;
}

void
cconfigsyslog::print_debug_level_all(std::string *out) const
{
	for (int i = 0; i < MAX_DEBUG_CLASSES; ++i) {
		print_debug_level_of_class(static_cast<DebugClass>(i), out);
	}
}

void
cconfigsyslog::print_debug_level_of_class(const std::string &class_name,
		std::string *out) const
{
	DebugClass class_id = csyslog::debug_class_pton(class_name);
	if (csyslog::UNDEF_DEBUG_CLASS == class_id) {
		out->append(class_name);
		out->append(":\t");
		out->append("is unknown");
	} else {
		print_debug_level_of_class(class_id, out);
	}
}

void
cconfigsyslog::print_debug_level_of_class(csyslog::DebugClass class_id,
		std::string *out) const
{
	try {
		out->append(debug_class_ntop(class_id));
		out->append(":\t");
	} catch (std::out_of_range& e) {
		out->append("unknown class");
		return;
	}

	try {
		out->append(debug_level_ntop(csyslog::debugClasses.at(class_id)));
	} catch (std::out_of_range& e) {
		out->append("is unknown");
	}

	out->append("\r\n");
}

cconfigsyslog::cconfigsyslog()
{
}

cconfigsyslog::~cconfigsyslog()
{
}

