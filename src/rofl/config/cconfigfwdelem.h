/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cconfigfwdelem.h
 *
 *  Created on: Jul 16, 2012
 *      Author: tobi
 */

#ifndef CCONFIGFWDELEM_H_
#define CCONFIGFWDELEM_H_

#include <string>
#include "rofl/common/cfwdelem.h"

class cconfigfwdelem {
public:
	static cconfigfwdelem &
	getInstance();

	void
	print_datapath_information(std::string *out) const;

	bool
	is_datapath(const std::string &dpname) const;


private:
	cconfigfwdelem();
	~cconfigfwdelem();
};


#endif /* CCONFIGFWDELEM_H_ */
