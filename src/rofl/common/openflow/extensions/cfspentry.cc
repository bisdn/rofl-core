/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cfspentry.h"

using namespace rofl;

cfspentry::cfspentry(
		cfspentry_owner *__fspowner,
		cofmatch __ofmatch,
		std::set<cfspentry*> *__fsp_list) :
		fsp_list(__fsp_list),
		ofmatch(__ofmatch),
		fspowner(__fspowner)
{
	if (fsp_list)
	{
		fsp_list->insert(this);
	}
}


cfspentry::~cfspentry()
{
	if (fsp_list)
	{
		fsp_list->erase(this);
	}
}


cfspentry::cfspentry(cfspentry& entry)
{
	*this = entry;
}


cfspentry&
cfspentry::operator= (cfspentry& entry)
{
	if (this == &entry)
		return *this;

	this->fspowner = entry.fspowner;
	this->ofmatch = entry.ofmatch;

	return *this;
}


const char*
cfspentry::c_str()
{
	cvastring vas(1024);

	info.assign(vas("cfspentry(%p) fspowner:%p ofmatch:%s",
			this,
			fspowner,
			ofmatch.c_str()));

	return info.c_str();
}
