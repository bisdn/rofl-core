/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include"csyslog.h"

/* static */ std::string csyslog::ident = std::string("rofl");
/* static */ int csyslog::facility = LOG_USER;
/* static */ int csyslog::option = LOG_CONS;
/* static */ int csyslog::logtype = LOGTYPE_SYSLOG;
///* static */ int csyslog::debugClasses[csyslog::MAX_DEBUG_CLASSES];
/* static */ std::string csyslog::filename(LOGFILE_DEFAULT);
///*static*/std::ofstream csyslog::oflogfile;
/* static */ FILE* csyslog::filestream = NULL;
/* static */ time_t csyslog::start_time = time(NULL);
/* static */ std::vector<int> csyslog::debugClasses(MAX_DEBUG_CLASSES);


void
csyslog::initlog(
		int logtype,
		int verbosity,
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


	//memset(debugClasses, verbosity, MAX_DEBUG_CLASSES * sizeof(int));
	for (int i = 0; i < MAX_DEBUG_CLASSES; ++i)
	{
		debugClasses[i] = verbosity;
	}

	csyslog::ident.assign(ident);
	csyslog::facility = facility;
	csyslog::option = option;
	switch (logtype) {
	case LOGTYPE_FILE:
		csyslog::logtype = LOGTYPE_FILE;
		//oflogfile.open(filename.c_str());
		filename = logfilename;
		//fprintf(stderr, "filename: %s logfilename: %s\n", filename.c_str(), logfilename.c_str());
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
}




void
csyslog::writelog(DebugClass debugClass, DebugLevel level, const char *fmt, ...)
{
#ifdef NDEBUG
	if (level >= ROFL_DBG)
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

#ifdef DEBUG_DEVEL
		fflush(filestream);
#endif


#if 0
			oflogfile << ident << " ";
			oflogfile << "(" << (std::hex) << this << (std::dec) << ") ";
			char dump[512];
			va_list ap;
			va_start(ap, fmt);
			vsnprintf(dump, sizeof(dump)-1, fmt, ap);
			va_end(ap);
			oflogfile << dump << std::endl;
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
