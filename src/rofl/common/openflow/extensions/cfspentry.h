/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFSPENTRY_H
#define CFSPENTRY_H 1

#include <string>

#include "../openflow.h"
#include "../cofmatch.h"
#include "../../cvastring.h"

namespace rofl
{

class cfspentry_owner {
public:
	virtual ~cfspentry_owner() {};
};

class cfspentry {
public: // static

	class find_by_fspowner {
		cfspentry_owner *fspowner;
	public:
		find_by_fspowner(cfspentry_owner *_fspowner) :
			fspowner(_fspowner) { };
		bool operator() (cfspentry* entry) {
			return (entry->fspowner == fspowner);
		};
	};

public: // per instance

	/** constructor
	 *
	 */
	cfspentry(
			cfspentry_owner *fspowner,
			cofmatch match,
			std::set<cfspentry*> *fsp_list = NULL);

	/** destructor
	 *
	 */
	virtual
	~cfspentry();

	/** copy constructor
	 *
	 */
	cfspentry(cfspentry& fspentry);

	/** assignment operator
	 *
	 */
	cfspentry&
	operator= (cfspentry& fspentry);


public: // data structures


	std::set<cfspentry*> 		*fsp_list; 		// set this instance is stored in
	cofmatch					 ofmatch; 		// ofmatch for this namespace entry
	cfspentry_owner 			*fspowner;		// cofctrl instance who has registered this namespace
	std::string 				 info;			// info string

public:

	friend std::ostream&
	operator<< (std::ostream& os, cfspentry const& e) {
		os << "<cfspentry ";
			os << "fsp-owner:" << e.fspowner << " ";
			os << "match:" << e.ofmatch << " ";
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif
