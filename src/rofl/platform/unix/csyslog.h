/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CSYSLOG_H
#define CSYSLOG_H 1

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <stdexcept>

#ifdef __cplusplus
extern "C" {
#endif

#include <syslog.h>
#include <stdarg.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __cplusplus
}
#endif

#if __WORDSIZE == 64
#define UINT64DBGFMT "lu"
#define SIZETDBGFMT "lu"
#else
#define UINT64DBGFMT "u"
#define SIZETDBGFMT "u"
#endif

#ifndef NDEBUG
#define WRITELOG(...) writelog(__VA_ARGS__)
#else
#define WRITELOG(...)
#endif

#ifndef NDEBUG
#define SWRITELOG(...) csyslog::writelog(__VA_ARGS__)
#else
#define SWRITELOG(...)
#endif

class csyslog {

	#define LOGFILE_DEFAULT "./rofld.log"

public:

	enum DebugType {
		LOGTYPE_FILE 	= 	0, /* log to file */
		LOGTYPE_STDERR	= 	1, /* log to stderr */
		LOGTYPE_SYSLOG	= 	2  /* log to syslog */
	};

	/* these are actually the same levels as used by syslog */
	enum DebugLevel {
		UNDEF_DEBUG_LEVEL = -1,		  /* Undefined debug level */
		EMERGENCY = 0	, /* = 0 system is unusable */
		ALERT			, /* = 1 action must be taken immediately */
		CRITICAL		, /* = 2 critical conditions */
		ERROR			, /* = 3 error conditions */
		WARN			, /* = 4 warning conditions */
		NOTICE			, /* = 5 normal but significant condition */
		INFO			, /* = 6 informational */
		DBG				, /* = 7 debug-level messages */
		MAX_DEBUG_LEVEL /* DO NOT USE */
		/* do not put anything beyond MAX_DEBUG_LEVEL! */
	};

	/* all debug classes */
	enum DebugClass {
		/* currently not in use */
		UNDEF_DEBUG_CLASS = -1, /* Undefined debug class */
		UNKNOWN = 0,		/* 0 currently */
		CSOCKET,			/* 1 dump socket details */
		CIOSRV,				/* 2 dump io service details */
		CPORT,				/* 3 dump port activities */
		CPORT_CONFIG,		/* 4 dump port configuration activities */
		COFDPT,				/* 5 dump all cofswitch internals */
		CPCP,				/* 6 dump PCP operations */
		CPACKET,			/* 7 dump all packets */
		COFPACKET,			/* 8 dump PCP packet contents */
		FTE,				/* 9 dump FTE operations */
		CFWD,				/* 10 dump forwarding details */
		FWDTABLE,			/* 11 dump cfwdtable details */
		CFTTABLE,			/* 12 dump all cfttable internals */
		TERMINUS,			/* 13 dump all terminus internals */
		COFRPC,				/* 14 dump all cofrpc internals */
		CDATAPATH,			/* 15 datapath emulator */
		LLDP,				/* 16 LLDP */
		COFACTION,			/* 17 dump cofaction details */
		XID,				/* 18 OpenFlow transactions (session xids) */
		CLI,				/* 19 dump CLI details */
		CTEST,				/* 20 dump test details */
		HARDWARE_GENERAL,	/* 21 hardware specific logging */
		HARDWARE_PORT,		/* 22 hardware-port specific logging */
		COFCTL,				/* 23 dump all cofctrl internals */
		CNAMESPACE,			/* 24 dump namespace table internals */
		CMEMORY,			/* 25 dump cmemory internals */
		CFRAME,				/* 26 dump frame internals */
		COFINST,			/* 27 dump cofinst internals */
		CGTTABLE,			/* 28 dump cgttable internals */
		CUNITTEST,			/* 29 dump cunittest internals */
		CPKBUF,				/* 30 dump cofpkbuf internals */
		CRIB,				/* 31 dump croute internals */
		CFIBENTRY,			/* 32 dump cfibentry internals */
		CLLDPTLV,			/* 33 dump clldptlv internals */
		COFMATCH,			/* 34 dump cofmatch internals */
		CPPPOETLV,			/* 35 dump cpppoetlv internals */
		FFRAME,				/* 36 dump fframe internals */
		CFTSEARCH,			/* 37 dump cftsearch internals */
		FPPPFRAME,			/* 38 dump fpppframe internals */
		CETHCTL,			/* 39 dump cethctl internals */
		CIPCTL,				/* 40 dump cipctl internals */
		CPIPE,				/* 41 dump cpipe internals */
		CFWDENGINE,			/* 42 dump cfwdengine internals */
		CPKBSTORE,			/* 43 dump cpkbstore internals */
		CPKBQUEUE,			/* 44 dump cpkbqueue internals */
		CCTLMOD,			/* 45 dump cctlmod internals */
		CTHREAD ,			/* 46 dump thread internals */
		CMEMPOOL,			/* 47 dump memory pool internals */
		CCLOCK,				/* 48 dump clock internals */
		COXMLIST,			/* 49 dump coxmlist internals */
		COXMATCH,			/* 50 dump coxmatch internals */
		CCONFIG,			/* 51 dump configuration internals */
		CPPP,				/* 52 dump cppp internals */
		COFBUCKET,			/* 53 dump cofbucket internals */
		CGTENTRY,			/* 54 dump cgtentry internals */
		CROFBASE,			/* 55 dump crofbase internals */
		MAX_DEBUG_CLASSES	/* not for debugging use! */
		/* do not put anything beyond MAX_DEBUG_CLASSES! */
	};

	static time_t start_time;
	static int logtype;
	static int option;     			// syslog option (default: LOG_CONS)
	static int facility;     		// syslog facility (default: LOG_USER)
	static std::string ident;   	// syslog ident
	static std::vector<csyslog::DebugLevel> debugClasses;
	static std::string filename; 	//< filename for logtype LOGTYPE_FILE
	static FILE* filestream; 		//< file descriptor for logtype FILE

public:     // static methods

	/** initialize environmental logging
	 */
	static void
	initlog(
			enum DebugType logtype = LOGTYPE_SYSLOG,
			enum DebugLevel = EMERGENCY,
			std::string logfilename = std::string(LOGFILE_DEFAULT),
			const char *ident = "rofl",
			int facility = LOG_USER,
			int option = LOG_CONS);

	/** write log message (C-interface with variable parameter list)
	 *  if level <= debugClasses[debugClass], the message will be written to log
	 */
	static void
	writelog(
			DebugClass debugClass,
			DebugLevel level,
			const char *format, ...);

	/** flush all logging
	 *
	 */
	static void
	flushlog();

	static void
	set_debug_level(const char * const class_name, const char * const level_name);

	static void
	set_debug_level(const std::string &class_name, const std::string &level_name);

	static void
	set_all_debug_levels(const char * const level_name);

	static void
	set_all_debug_levels(const std::string &level_name);

	static void
	set_all_debug_levels(const csyslog::DebugLevel level);

	/** return file descriptor
	 */
	static int
	getfd();

	static const std::vector<std::string>&
	get_level_names() {
		return level_names;
	}

	static const std::vector<std::string>&
	get_class_names() {
		return class_names;
	}

	static const DebugLevel
	debug_level_pton(const std::string &name);

	static const DebugClass
	debug_class_pton(const std::string &name);

	static const std::string&
	debug_level_ntop(DebugLevel l) throw (std::out_of_range);

	static const std::string&
	debug_class_ntop(DebugClass c) throw (std::out_of_range);

private:

	static void
	init_level_names();

	static void
	init_class_names();

	static std::vector<std::string> level_names;
	static std::vector<std::string> class_names;
};

#endif
