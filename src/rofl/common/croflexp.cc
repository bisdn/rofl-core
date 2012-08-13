/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "croflexp.h"

croflexp::croflexp() :
	cmemory(ROFL_EXP_MAX_SIZE)
{
	rext_body = (uint8_t*)somem();
}


croflexp::~croflexp()
{

}


const char*
croflexp::c_str()
{
	cvastring vas;

	info.assign(vas("croflexp(%p) %s", this, cmemory::c_str()));

	return info.c_str();
}


void
croflexp::pack(uint8_t *__body, size_t __bodylen) throw (eRoflExpInval)
{
	size_t len = size();

	if (__bodylen < len)
	{
		throw eRoflExpInval();
	}

	memcpy(__body, somem(), memlen());
}


void
croflexp::unpack(uint8_t *__body, size_t __bodylen)
{
	resize(__bodylen);

	rext_body = (uint8_t*)somem();

	memcpy(somem(), __body, __bodylen);
}


size_t
croflexp::size()
{
	return memlen();
}





