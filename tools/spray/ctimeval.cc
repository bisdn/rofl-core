/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ctimeval.cc
 *
 *  Created on: 01.09.2013
 *      Author: andreas
 */

#include "ctimeval.h"

using namespace spray;

extern int errno;

ctimeval
ctimeval::now()
{
	ctimeval tmp;

	int rc = 0;

	if ((rc = gettimeofday(&(tmp.tv), (struct timezone*)0)) < 0) {
		fprintf(stderr, "error gettimeofday() call: %d (%s)\n", errno, strerror(errno));
	}

	return tmp;
}


ctimeval::ctimeval()
{
	memset(&tv, 0, sizeof(tv));
}


ctimeval::~ctimeval()
{

}



ctimeval::ctimeval(const ctimeval& tval)
{
	*this = tval;
}



ctimeval&
ctimeval::operator= (const ctimeval& tval)
{
	if (this == &tval)
		return *this;

	tv.tv_sec	= tval.tv.tv_sec;
	tv.tv_usec	= tval.tv.tv_usec;

	return *this;
}





