/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cconfigsyslog.h
 *
 *  Created on: Oct 29, 2012
 *      Author: tobi
 */

#ifndef CCONFIGSYSLOG_H_
#define CCONFIGSYSLOG_H_

#include "rofl/platform/unix/csyslog.h"

namespace rofl
{

class cconfigsyslog : public csyslog
{
public:

	static cconfigsyslog&
	getInstance();

	void
	print_debug_level_all(std::string *out) const;

	void
	print_debug_level_of_class(const std::string &class_name,
			std::string *out) const;

	void
	print_debug_level_of_class(csyslog::DebugClass class_id,
			std::string *out) const;

private:
	cconfigsyslog();
	~cconfigsyslog();
};

}; // end of namespace

#endif /* CCONFIGSYSLOG_H_ */
