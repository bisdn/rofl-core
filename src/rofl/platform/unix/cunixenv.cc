/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cunixenv.h"

extern char* optarg;

/* static */const std::string daemon_dir("/var/tmp");

/* Carg stuff */
coption::coption(bool optional, 
		int value_type,
		char shortcut, 
		std::string full_name, 
		std::string description, 
		std::string default_value):
		optional(optional), 
		value_type(value_type), 
		shortcut(shortcut), 
		full_name(full_name), 
		description(description), 
		default_value(default_value)
{
	current_value = default_value;
	present = false;
	
	if(!(this->value_type == NO_ARGUMENT ||
		this->value_type == REQUIRED_ARGUMENT|| 
		this->value_type == OPTIONAL_ARGUMENT )) 
			throw std::runtime_error("Unknown value type");
}	

std::string coption::parse_argument(char* optarg){
	if(value_type != NO_ARGUMENT)
		this->current_value = std::string(optarg);
	present = true;
	
	return this->current_value;	
}

cunixenv&
cunixenv::getInstance()
{
	static cunixenv instance;
	return instance;
}

/*
*
* cunixenv
*
*/

/* Constructor */ 
cunixenv::cunixenv()
{
	/*Push default arguments */
	arguments.push_back(coption(true,REQUIRED_ARGUMENT,'d',"debug","debug level",std::string(""+(int)csyslog::EMERGENCY)));
	arguments.push_back(coption(true,REQUIRED_ARGUMENT,'l',"logfile","log file",std::string(LOGFILE_DEFAULT)));
	arguments.push_back(coption(true,NO_ARGUMENT,'h',"help","Help message",""));
	arguments.push_back(coption(false,REQUIRED_ARGUMENT,'c',"config-file","Config file","./default-cli.cfg"));
	arguments.push_back(coption(true, NO_ARGUMENT,'D',"daemonize","Daemonize process",""));
	parsed = false;
}

void
cunixenv::usage(
		char *argv0)
{

	using namespace std;
	cerr << "usage: " << std::string(argv0)<<" {parameters}"<<endl; 
	
	string mandatory = "";
	string optional = "";

	for(std::vector<coption>::iterator it = this->arguments.begin(); it != this->arguments.end(); ++it){
		string tmp(""); 
		if(it->optional)
			tmp+="[";
		tmp+="--"+it->full_name+"|-"+it->shortcut;
		
		if(it->optional)
			tmp+="]";
	
		tmp +=" <"+it->description;
		if(it->default_value != "")
			tmp+=". default("+it->default_value+")";
		tmp+=">\n";
		
		if(!it->optional)
			mandatory += tmp; 
		else
			optional += tmp; 
			
	}
	if(mandatory!= "")
		cerr<<"Mandatory parameters:"<<endl<<mandatory;	
	if(optional!= "")
		cerr<<"Optional parameters:"<<endl<<optional;	
	
	exit(0);
}

void
cunixenv::update_default_option(const std::string &option_name, const std::string &value)
{
	for(std::vector<coption>::iterator it = this->arguments.begin(); it != this->arguments.end(); ++it) {
		if ( 0 == (*it).full_name.compare(option_name) ) {
			(*it).current_value = value;
			(*it).default_value = value;
		}
	}
}

void
cunixenv::parse_args(
		int argc,
		char** argv)
{
	int c;
	int option_index;
	std::string format;
	struct option* long_options = (struct option*)calloc(1,sizeof(struct option)*(this->arguments.size()+1));

	//Construct getopts stuff
	for(std::vector<coption>::iterator it = this->arguments.begin(); it != this->arguments.end(); ++it){
		format += it->shortcut;
		if(it->value_type == REQUIRED_ARGUMENT)
			format+=":"; 
		else if(it->value_type == OPTIONAL_ARGUMENT)
			format+="::"; 

		//Fill long_options row
		struct option* tmp = &long_options[std::distance(this->arguments.begin(), it)];
		tmp->has_arg = it->value_type;
		tmp->name = it->full_name.c_str();
		tmp->val = it->shortcut;
	
	}

	bool do_detach = false;

	//fprintf(stderr,"Using the following format-> %s \n",format.c_str());	
	while(true){
	
		c = getopt_long(argc, argv, format.c_str(), long_options, &option_index);
		if (c == -1)
			break;
		//TODO: might be better to have a map	
		for(std::vector<coption>::iterator it = this->arguments.begin(); it != this->arguments.end(); ++it){
			if(it->shortcut == c){
				it->parse_argument(optarg);
				if(c == 'h')
					usage(argv[0]);
				else if(c == 'D')	
					do_detach = true;
				continue;
			}
		}
	}
	
	//free calloc
	free(long_options);	
	parsed = true;

	// call detach when all args parsed
	if (do_detach) {
		detach();
	}
}

void cunixenv::add_option(const coption &arg){
	arguments.push_back(arg);	
}

std::string cunixenv::get_arg(const std::string &name){
	if(!parsed)
		throw std::runtime_error("Args not yet parsed. use parse_args()");
	std::vector<coption>::iterator it;
	for(it = this->arguments.begin(); it != this->arguments.end(); ++it){
		if(it->full_name == name)
			return it->current_value;
	}
	throw std::runtime_error("Unknown argument");
}

std::string cunixenv::get_arg(char shortcut){

	if(!parsed)
		throw std::runtime_error("Args not yet parsed. use parse_args()");
	
	std::vector<coption>::iterator it;
	for(it = this->arguments.begin(); it != this->arguments.end(); ++it){
		if(it->shortcut == shortcut)
			return it->current_value;
	}
	throw std::runtime_error("Unknown argument");
}

bool
cunixenv::is_arg_set(const std::string &name)
{
	if(!parsed)
		throw std::runtime_error("Args not yet parsed. use parse_args()");

	for(std::vector<coption>::iterator it = this->arguments.begin(); it != this->arguments.end(); ++it){
		if( 0 == (*it).full_name.compare(name)) {
			return (*it).is_present();
		}
	}

	return false;
}

void
cunixenv::update_paths()
{
	char *ptr = getcwd(NULL, 0);
	std::string wd(ptr);
	free(ptr);
	wd.append("/");

	for(std::vector<coption>::iterator it = this->arguments.begin(); it != this->arguments.end(); ++it) {
		switch ((*it).shortcut) {
		case 'c':
			// check if its already an absolute path
			if ((*it).current_value.length() && '/' != (*it).current_value[0]) {
				(*it).current_value.insert(0, wd);
			}
			break;
		case 'l':
			if ( 0 == (*it).current_value.compare((*it).default_value) ) {
				// still the default dir
				std::string tmp(daemon_dir);
				tmp.append("/");
				(*it).current_value.insert(0, tmp);
			} else if ((*it).current_value.length() && '/' != (*it).current_value[0]) {
				// not an absolute path
				(*it).current_value.insert(0, wd);
			} else {
				exit(-1);
			}
			break;
		default:
			break;
		}
	}
}

// todo this belongs rather to ciosrv than here. though needs refactoring
void
cunixenv::detach()
{
	pid_t pid = fork();

	if (pid < 0) // error occured
	{
		fprintf(stderr, "fork() sys-call failed: %d (%s)\n", errno, strerror(errno));
		exit(-1);
	}
	else if (pid > 0) // parent exit
	{
		exit(0);
	}

	// child code
	if ((pid = setsid()) < 0) // detach from controlling terminal
	{
		fprintf(stderr, "setsid() sys-call failed: %d (%s)\n", errno, strerror(errno));
		exit(-1);
	}

	update_paths();

	// set file mask
	umask(027);

	// redirect stdout, stderr
	csyslog::initlog(csyslog::LOGTYPE_FILE,
			static_cast<csyslog::DebugLevel>(atoi(get_arg("debug").c_str())),
			get_arg("logfile")
			); 	// reinitialize logging to logtype file

	fflush(stdout);
	dup2(csyslog::getfd(), STDOUT_FILENO); // redirect stdout

	fflush(stderr);
	dup2(csyslog::getfd(), STDERR_FILENO); // redirect stderr

	// change working directory
	if (chdir(daemon_dir.c_str()) < 0)
	{
		fprintf(stderr, "chdir() sys-call failed: %d (%s)\n", errno, strerror(errno));
		exit(-1);
	}
}

cunixenv::~cunixenv() {
	// everything destroyed anyway...
}
