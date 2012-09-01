/*
 * cadapt.cc
 *
 *  Created on: 30.08.2012
 *      Author: andreas
 */

#include <rofl/common/cadapt.h>


cadapt*
cadapt_new(
		cadapt_owner *base,
		std::string const& name	)
{
	/*
	 * create a method named "name"_new which will be loaded by ctlbase
	 * that is capable of creating new objects of type cadapt
	 */
	if (name == std::string("cadapt"))
	{
		return new cadapt(base);
	}

	return 0;
}


cadapt::cadapt(
		cadapt_owner *base) throw (eAdaptInval) :
	base(base)
{
	if (0 == base)
	{
		throw eAdaptInval();
	}

	base->adapters.insert(this);
}


cadapt::~cadapt()
{
	base->adapters.erase(this);
}


const char*
cadapt::c_str()
{
	cvastring vas(64);
	info.assign(vas("cadapter(%p) cadapt_owner:%p", this, base));
	return info.c_str();
}

