/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CUNIXENV_H
#define CUNIXENV_H 1

#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <execinfo.h>
#include <errno.h>
#ifdef __cplusplus
}
#endif

#include <iostream>
#include <string>
#include <stdexcept>
#include "csyslog.h"


/*
* Helper class to manipulate easily arguments in C++ programs. Argmuents are ALWAYS strings
*/

#define NO_ARGUMENT no_argument
#define REQUIRED_ARGUMENT required_argument
#define OPTIONAL_ARGUMENT optional_argument

class coption
{

public:
	bool optional;
	int value_type;
	char shortcut;
	std::string full_name;
	std::string description;
	std::string default_value;
	
	/* Current state */
	bool present;
	std::string current_value;


	/* Constructor */
	coption(void){};

	coption(bool optional, 
		int value_type, 
		char shortcut, 
		std::string full_name, 
		std::string description, 
		std::string default_value);

	/* Atempt to parse argument from optarg provided by getopt*/
	std::string parse_argument(char* optarg);
	bool is_present(void){return present;}
	std::string value(void){return current_value;}
};

/* Forward declaration */

/* 
* Unix parser class 
*/
class cunixenv
{
private:
/*
	 int debug;			// verbosity
	 int n_buffers;			// #buffers for cfwdelem
	 int block_size; 		// block size in units of getpagesize() for cmmapport (default: 8)
	 int n_blocks; 			// #blocks for cmmapport instances (default: 8)
	 int frame_size; 		// frame size for cmmapport in bytes (default: 2048)
	 int n_fwdengines;		// #fwdengines
	 std::string controller_ip;	// ip address of the controller
	 u_int16_t controller_port;	// port number of the controller
	 std::string config_filename;
	 std::string hw_driver;		//HAL driver 
*/
	bool parsed;
	std::vector<coption> arguments;

	/**
	 * Usage method
	 */
	void usage(char *argv0);

	/**
	 * Fork process
	 */
	void detach(void);


public:

	/*
	* Constructor 
	*/
	cunixenv(std::vector<coption>* args=NULL);

	/*
	* Add argument to current list of arguments to parse 
	*/
	void add_option(coption arg);
	
	/**
	 * Parse arguments using getopt
	 */
	void parse_args(int argc, char** argv);

	/*
	 * Get value methods
	 */
	std::string get_arg(std::string name);	

	std::string get_arg(char shortcut);	
	
};

#endif
