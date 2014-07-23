/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ctimeval.h
 *
 *  Created on: 01.09.2013
 *      Author: andreas
 */

#ifndef CTIMEVAL_H_
#define CTIMEVAL_H_

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

namespace spray
{

class ctimeval
{
	struct timeval tv;

public:

	static ctimeval
	now();

public:

	ctimeval();

	virtual
	~ctimeval();

	ctimeval(const ctimeval& tval);

	ctimeval& operator= (const ctimeval& tval);
};

}; // end of namespace

#endif /* CTIMEVAL_H_ */
