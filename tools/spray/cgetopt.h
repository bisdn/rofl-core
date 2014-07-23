/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cgetopt.h
 *
 *  Created on: 31.08.2013
 *      Author: andreas
 */

#ifndef CGETOPT_H_
#define CGETOPT_H_

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <exception>

namespace unixenv
{

class eGetOptBase 		: public std::exception {};
class eGetOptNotFound	: public eGetOptBase {};

class cgetopt
{
	static cgetopt *getopt;
	cgetopt();
	~cgetopt();

	struct short_option {
		char c;
		int has_arg;
	};

	std::string 						s_short_options;
	std::vector<struct short_option>	short_options;
	std::vector<struct option> 			long_options;
	std::vector<std::string> 			long_option_names;
	std::map<std::string, std::string> 	voptions;

public:

	static cgetopt&
	get_instance();

public:

	enum cgetopt_has_arg_t {
		NO_ARG = 0,
		REQUIRED_ARG = 1,
		OPTIONAL_ARG = 2,
	};

	void
	parse(int argc, char** argv);

	void
	add_short_option(
			char c,
			int has_arg = NO_ARG);

	void
	add_long_option(
			std::string const& name,
			int has_arg = NO_ARG,
			int *flag = 0,
			int val = 0);

	std::string&
	get_opt(std::string const& name);

	bool
	has_opt(std::string const& name) const;

private:

	void
	make_short_options_string();
};

}; // end of namespace

#endif /* CGETOPT_H_ */
