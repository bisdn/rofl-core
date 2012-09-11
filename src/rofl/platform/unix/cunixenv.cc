/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cunixenv.h"

extern char* optarg;

/* Carg stuff */
carg::carg(bool optional, 
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

std::string carg::parse_argument(char* optarg){
	if(value_type != NO_ARGUMENT)
		this->current_value = std::string(optarg);
	present = true;
	
	return this->current_value;	
}

/*
*
* cunixenv
*
*/

/* Constructor */ 
cunixenv::cunixenv(std::vector<carg>* args){
	
	if(args)
		arguments = *args;
	else{
		/*Push default arguments */
		arguments.push_back(carg(true,REQUIRED_ARGUMENT,'d',"debug","debug level",std::string(""+(int)csyslog::EMERGENCY)));
		arguments.push_back(carg(true,NO_ARGUMENT,'h',"help","Help message",""));
		arguments.push_back(carg(false,REQUIRED_ARGUMENT,'c',"config-file","Config file","./default-cli.cfg"));
		arguments.push_back(carg(true, NO_ARGUMENT,'D',"daemonize","Daemonize process",""));
	}
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

	for(std::vector<carg>::iterator it = this->arguments.begin(); it != this->arguments.end(); it++){
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
cunixenv::parse_args(
		int argc,
		char** argv)
{
	int c;
	int option_index;
	std::string format;
	struct option* long_options = (struct option*)calloc(1,sizeof(struct option)*(this->arguments.size()+1));

	//Construct getopts stuff
	for(std::vector<carg>::iterator it = this->arguments.begin(); it != this->arguments.end(); it++){
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

	//fprintf(stderr,"Using the following format-> %s \n",format.c_str());	
	while(true){
	
		c = getopt_long(argc, argv, format.c_str(), long_options, &option_index);
		if (c == -1)
			break;
		//TODO: might be better to have a map	
		for(std::vector<carg>::iterator it = this->arguments.begin(); it != this->arguments.end(); it++){
			if(it->shortcut == c){
				it->parse_argument(optarg);
				if(c == 'h')
					usage(argv[0]);
				else if(c == 'D')	
					detach();
				continue;
			}
		}
	}
	
	//free calloc
	free(long_options);	
	parsed = true;
}

void cunixenv::add_argument(carg arg){
	arguments.push_back(arg);	
}

std::string cunixenv::get_arg(std::string name){
	if(!parsed)
		throw std::runtime_error("Args not yet parsed. use parse_args()");
	std::vector<carg>::iterator it;
	for(it = this->arguments.begin(); it != this->arguments.end(); it++){
		if(it->full_name == name)
			return it->current_value;
	}
	throw std::runtime_error("Unknown argument");
}

std::string cunixenv::get_arg(char shortcut){

	if(!parsed)
		throw std::runtime_error("Args not yet parsed. use parse_args()");
	
	std::vector<carg>::iterator it;
	for(it = this->arguments.begin(); it != this->arguments.end(); it++){
		if(it->shortcut == shortcut)
			return it->current_value;
	}
	throw std::runtime_error("Unknown argument");
}

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


	// redirect stdout, stderr
	csyslog::initlog(csyslog::LOGTYPE_FILE); 	// reinitialize logging to logtype file

	dup2(STDOUT_FILENO, csyslog::getfd()); // redirect stdout
	dup2(STDERR_FILENO, csyslog::getfd()); // redirect stderr

	// set file mask
	umask(027);

	// change working directory
	if (chdir("/var/tmp") < 0)
	{
		fprintf(stderr, "chdir() sys-call failed: %d (%s)\n", errno, strerror(errno));
		exit(-1);
	}
}

