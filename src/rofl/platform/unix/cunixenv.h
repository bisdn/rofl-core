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

#include <string>

#include "csyslog.h"

class cunixenv
{
public:

	static int debug;			// verbosity
	static int n_buffers;		// #buffers for cfwdelem
	static int block_size; 	// block size in units of getpagesize() for cmmapport (default: 8)
	static int n_blocks; 		// #blocks for cmmapport instances (default: 8)
	static int frame_size; 	// frame size for cmmapport in bytes (default: 2048)
	static int n_fwdengines;	// #fwdengines
	static std::string config_filename;

public:

	/**
	 *
	 */
	static void usage(char *argv0);

	/**
	 *
	 */
	static void detach();

	/**
	 *
	 */
	static void parse_args(int argc, char** argv);
};

#endif
