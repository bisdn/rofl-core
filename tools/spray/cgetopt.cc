/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cgetopt.cc
 *
 *  Created on: 31.08.2013
 *      Author: andreas
 */

#include "cgetopt.h"

using namespace unixenv;

cgetopt* cgetopt::getopt = (cgetopt*)0;

cgetopt&
cgetopt::get_instance()
{
	if (0 == cgetopt::getopt) {
		cgetopt::getopt = new cgetopt();
	}
	return *(cgetopt::getopt);
}


cgetopt::cgetopt()
{

}


cgetopt::~cgetopt()
{

}



void
cgetopt::parse(int argc, char** argv)
{
	int c = 0;
	int option_index = 0;

	make_short_options_string();

	while (true) {

		if ((c = getopt_long(argc, argv, s_short_options.c_str(),
				&long_options[0], &option_index)) < 0) {
			break;
		}

		//std::cerr << "c: " << c << std::endl;
		switch (c) {
		case 0: {
			if (long_options[option_index].has_arg)
				voptions[std::string(long_options[option_index].name)] = std::string(optarg);
			else
				voptions[std::string(long_options[option_index].name)] = std::string("");
		} break;
		}
	}

#if 0
	for(std::map<std::string,std::string>::iterator
			it = voptions.begin(); it != voptions.end(); ++it) {
		std::cerr << it->first << ":" << it->second << std::endl;
	}
#endif
}



void
cgetopt::add_short_option(
		char c,
		int has_arg)
{
	struct short_option opt;
	memset(&opt, 0, sizeof(opt));

	opt.c 		= c;
	opt.has_arg = has_arg;

	short_options.push_back(opt);
}



void
cgetopt::add_long_option(
		std::string const& name,
		int has_arg,
		int *flag,
		int val)
{
	struct option opt;
	memset(&opt, 0, sizeof(opt));

	long_option_names.push_back(name);

	opt.name 	= long_option_names.back().c_str();
	opt.has_arg = has_arg;
	opt.flag 	= flag;
	opt.val  	= val;

	long_options.push_back(opt);
}



void
cgetopt::make_short_options_string()
{
	std::stringstream s_str;

	for (std::vector<struct short_option>::iterator
			it = short_options.begin(); it != short_options.end(); ++it) {
		struct short_option& opt = (*it);
		s_str << opt.c;
		switch (opt.has_arg) {
		case OPTIONAL_ARG: s_str << ":"; /* FALL-THROUGH */
		case REQUIRED_ARG: s_str << ":"; /* FALL-THROUGH */
		}
	}

	s_short_options.assign(s_str.str());
}




std::string&
cgetopt::get_opt(std::string const& name)
{
	if (voptions.find(name) == voptions.end()) {
		throw eGetOptNotFound();
	}
	return voptions[name];
}



bool
cgetopt::has_opt(std::string const& name) const
{
	if (voptions.find(name) == voptions.end()) {
		return false;
	}
	return true;
}


