/*
 * cadapt.cc
 *
 *  Created on: 30.08.2012
 *      Author: andreas
 */

#include <rofl/common/cadapt.h>




cadapt::cadapt(
		cadapt_owner *base,
		unsigned int index) throw (eAdaptInval) :
	base(base),
	index(index)
{
	if (0 == base)
	{
		throw eAdaptInval();
	}

	if (index >= base->adapters.size())
	{
		throw eAdaptInval();
	}

	base->adapters[index].push_back(this);
}


cadapt::~cadapt()
{
	base->adapters[index].remove(this);
}


const char*
cadapt::c_str()
{
	cvastring vas(64);
	info.assign(vas("cadapter(%p) cadapt_owner:%p", this, base));
	return info.c_str();
}

