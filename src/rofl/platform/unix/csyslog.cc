/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include"csyslog.h"

/* static */ std::string csyslog::ident = std::string("rofl");
/* static */ int csyslog::facility = LOG_USER;
/* static */ int csyslog::option = LOG_CONS;
/* static */ int csyslog::logtype = LOGTYPE_SYSLOG;
/* static */ std::string csyslog::filename(LOGFILE_DEFAULT);
/* static */ FILE* csyslog::filestream = NULL;
/* static */ time_t csyslog::start_time = time(NULL);
/* static */ std::vector<csyslog::DebugLevel> csyslog::debugClasses(MAX_DEBUG_CLASSES);
/* static */ std::vector<std::string> csyslog::level_names(MAX_DEBUG_LEVEL);
/* static */ std::vector<std::string> csyslog::class_names(MAX_DEBUG_CLASSES);

void
csyslog::initlog(
		enum DebugType logtype,
		enum DebugLevel verbosity,
		std::string logfilename,
		const char *ident,
		int facility,
		int option)
{
	switch (logtype) {
	case LOGTYPE_FILE:
		//oflogfile.close();
		if (NULL != filestream)
		{
			fclose(filestream);
		}
		break;
	case LOGTYPE_STDERR:
		// do nothing
		break;
	case LOGTYPE_SYSLOG:
	default:
		closelog();
		break;
	}

	if (EMERGENCY > verbosity) {
		verbosity = EMERGENCY;
	}
	if (DBG < verbosity) {
		verbosity = DBG;
	}
	set_all_debug_levels(verbosity);

	csyslog::ident.assign(ident);
	csyslog::facility = facility;
	csyslog::option = option;
	switch (logtype) {
	case LOGTYPE_FILE:
		csyslog::logtype = LOGTYPE_FILE;
		filename = logfilename;
		filestream = fopen(filename.c_str(), "w");
		break;
	case LOGTYPE_STDERR:
		csyslog::logtype = LOGTYPE_STDERR;
		break;
	case LOGTYPE_SYSLOG:
	default:
		csyslog::logtype = LOGTYPE_SYSLOG;
		openlog(ident, option, facility);
		break;
	}

	init_level_names();
	init_class_names();
}

void
csyslog::writelog(DebugClass debugClass, DebugLevel level, const char *fmt, ...)
{
#ifdef NDEBUG
	if (level >= DBG)
		return;
#endif

	if (level > debugClasses[debugClass])
	{
		return;
	}

	switch (logtype) {
	case LOGTYPE_FILE:
	{
		if (0 == filestream)
		{
			initlog(LOGTYPE_FILE);
		}
		time_t now = time(NULL);
		std::string s_now(ctime(&now));
		s_now = s_now.substr(0, s_now.size() - 1);
		fprintf(filestream, "%s 0x%x %s ",
				ident.c_str(),
				(unsigned int)pthread_self(),
				s_now.c_str());

		va_list ap;
		va_start(ap, fmt);
		vfprintf(filestream, fmt, ap);
		va_end(ap);

		fprintf(filestream, "\n");

#ifdef FLUSH_ALWAYS
		fflush(filestream);
#endif
	}
	break;
	case LOGTYPE_STDERR:
	{
		std::cerr << ident << " ";
		std::cerr << pthread_self() << " ";
		va_list ap;
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
		std::cerr << std::endl;
	}
	break;
	case LOGTYPE_SYSLOG:
	{
		va_list ap;
		va_start(ap, fmt);
		vsyslog(LOG_INFO, fmt, ap);
		va_end(ap);
	}
	break;
	}
}

void
csyslog::flushlog()
{
	switch (logtype) {
	case LOGTYPE_FILE:
		if (filestream)
		{
			fflush(filestream);
		}
		break;
	case LOGTYPE_STDERR:

		break;
	case LOGTYPE_SYSLOG:
	default:

		break;
	}
}

int
csyslog::getfd()
{
	if (!filestream)
		return -1;
	return fileno(filestream);
}

/* static */void
csyslog::set_debug_level(const char * const class_name, const char * const level_name)
{
	set_debug_level(std::string(class_name), std::string(level_name));
}

/* static */void
csyslog::set_debug_level(const std::string &class_name, const std::string &level_name)
{
	DebugClass class_num = debug_class_pton(class_name);
	if(UNDEF_DEBUG_CLASS == class_num) {
		return;
	}

	DebugLevel level_num = debug_level_pton(level_name);
	if(UNDEF_DEBUG_LEVEL == level_num) {
		return;
	}

	debugClasses[class_num] = level_num;
}

/* static */void
csyslog::set_all_debug_levels(const char * const level_name)
{
	set_all_debug_levels(std::string(level_name));
}

/* static */void
csyslog::set_all_debug_levels(const std::string &level_name)
{
	set_all_debug_levels(debug_level_pton(level_name));
}

/* static */void
csyslog::set_all_debug_levels(const csyslog::DebugLevel level)
{
	for (int i = 0; i < MAX_DEBUG_CLASSES; ++i)
	{
		debugClasses[i] = level;
	}
}

/* static */const csyslog::DebugLevel
csyslog::debug_level_pton(const std::string &name)
{
	int cur = EMERGENCY;

	for (std::vector<std::string>::const_iterator iter = level_names.begin();
			iter != level_names.end() ; ++iter) {
		if ( (*iter) == name) {
			return static_cast<DebugLevel>(cur);
		}
		++cur;
	}

	return UNDEF_DEBUG_LEVEL;
}

/* static */const csyslog::DebugClass
csyslog::debug_class_pton(const std::string &name)
{
	int cur = UNKNOWN;

	for (std::vector<std::string>::const_iterator iter = class_names.begin();
			iter != class_names.end() ; ++iter) {
		if ( (*iter) == name) {
			return static_cast<DebugClass>(cur);
		}
		++cur;
	}

	return UNDEF_DEBUG_CLASS;
}

/* static */const std::string&
csyslog::debug_level_ntop(DebugLevel l) throw (std::out_of_range)
{
	return level_names.at(l);
}

/* static */const std::string&
csyslog::debug_class_ntop(DebugClass c) throw (std::out_of_range)
{
	return class_names.at(c);
}

/* static */void
csyslog::init_level_names()
{
	level_names[EMERGENCY] = std::string("emergency");
	level_names[ALERT]     = std::string("alert");
	level_names[CRITICAL]  = std::string("critical");
	level_names[ERROR]     = std::string("error");
	level_names[WARN]      = std::string("warn");
	level_names[NOTICE]    = std::string("notice");
	level_names[INFO]      = std::string("info");
	level_names[DBG]       = std::string("dbg");
}

/* static */void
csyslog::init_class_names()
{
	class_names[UNKNOWN]          = std::string("unknown");
	class_names[CSOCKET]          = std::string("csocket");
	class_names[CIOSRV]           = std::string("ciosrv");
	class_names[CPORT]            = std::string("cport");
	class_names[CPORT_CONFIG]     = std::string("cport_config");
	class_names[COFDPT]         = std::string("cofdpath");
	class_names[CPCP]             = std::string("cpcp");
	class_names[CPACKET]          = std::string("cpacket");
	class_names[COFPACKET]        = std::string("cofpacket");
	class_names[FTE]              = std::string("fte");
	class_names[CFWD]             = std::string("cfwd");
	class_names[FWDTABLE]         = std::string("fwdtable");
	class_names[CFTTABLE]         = std::string("cfttable");
	class_names[TERMINUS]         = std::string("terminus");
	class_names[COFRPC]           = std::string("cofrpc");
	class_names[CDATAPATH]        = std::string("cdatapath");
	class_names[LLDP]             = std::string("lldp");
	class_names[COFACTION]        = std::string("cofaction");
	class_names[XID]              = std::string("xid");
	class_names[CLI]              = std::string("cli");
	class_names[CTEST]            = std::string("ctest");
	class_names[HARDWARE_GENERAL] = std::string("hardware_general");
	class_names[HARDWARE_PORT]    = std::string("hardware_port");
	class_names[COFCTL]          = std::string("cofctrl");
	class_names[CNAMESPACE]       = std::string("cnamespace");
	class_names[CMEMORY]          = std::string("cmemory");
	class_names[CFRAME]           = std::string("cframe");
	class_names[COFINST]          = std::string("cofinst");
	class_names[CGTTABLE]         = std::string("cgttable");
	class_names[CUNITTEST]        = std::string("cunittest");
	class_names[CPKBUF]           = std::string("cpkbuf");
	class_names[CRIB]             = std::string("crib");
	class_names[CFIBENTRY]        = std::string("cfibentry");
	class_names[CLLDPTLV]         = std::string("clldptlv");
	class_names[COFMATCH]         = std::string("cofmatch");
	class_names[CPPPOETLV]        = std::string("cpppoetlv");
	class_names[FFRAME]           = std::string("fframe");
	class_names[CFTSEARCH]        = std::string("cftsearch");
	class_names[FPPPFRAME]        = std::string("fpppframe");
	class_names[CETHCTL]          = std::string("cethctl");
	class_names[CIPCTL]           = std::string("cipctl");
	class_names[CPIPE]            = std::string("cpipe");
	class_names[CFWDENGINE]       = std::string("cfwdengine");
	class_names[CPKBSTORE]        = std::string("cpkbstore");
	class_names[CPKBQUEUE]        = std::string("cpkbqueue");
	class_names[CCTLMOD]          = std::string("cctlmod");
	class_names[CTHREAD]          = std::string("cthread");
	class_names[CMEMPOOL]         = std::string("cmempool");
	class_names[CCLOCK]           = std::string("cclock");
	class_names[COXMLIST]         = std::string("coxmlist");
	class_names[COXMATCH]         = std::string("coxmatch");
	class_names[CCONFIG]          = std::string("cconfig");
	class_names[CPPP]             = std::string("cppp");
	class_names[COFBUCKET]        = std::string("cofbucket");
	class_names[CGTENTRY]         = std::string("cgtentry");
}
