/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cparams.cc
 *
 *  Created on: 01.04.2014
 *      Author: andreas
 */

#include "rofl/common/cparams.h"

using namespace rofl;

cparams::cparams()
{}



cparams::~cparams()
{}



cparams::cparams(cparams const& params)
{
	*this = params;
}



cparams&
cparams::operator= (cparams const& p)
{
	if (this == &p)
		return *this;

	clear();

	for (std::map<std::string, cparam>::const_iterator
			it = p.params.begin(); it != p.params.end(); ++it) {
		params[it->first] = it->second;
	}

	return *this;
}



cparam&
cparams::add_param(std::string const& key)
{
	if (params.find(key) != params.end()) {
		params.erase(key);
	}
	return params[key];
}



void
cparams::drop_param(std::string const& key)
{
	if (params.find(key) == params.end()) {
		return;
	}
	params.erase(key);
}



cparam&
cparams::set_param(std::string const& key)
{
	if (params.find(key) == params.end()) {
		(void)params[key];
	}
	return params[key];
}



cparam const&
cparams::get_param(std::string const& key) const
{
	if (params.find(key) == params.end()) {
		throw eParamNotFound();
	}
	return params.at(key);
}



bool
cparams::has_param(std::string const& key) const
{
	return (not (params.find(key) == params.end()));
}


#if 0
void
test()
{
	cparams p;
	p.add_param("remote-addr") = cparam("127.0.0.1");
	p.add_param("capath") = cparam("/home/andreas/---");
	p.get_param("port").toInteger();
}
#endif
