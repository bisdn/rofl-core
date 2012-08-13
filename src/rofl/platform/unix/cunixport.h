/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef cunixPORT_H
#define cunixPORT_H

#include <set>
#include <string>
#include <algorithm>

#include "../../common/csocket.h"
#include "../../common/caddress.h"
#include "../../common/cpacket.h"
#include "clinuxport.h"

class cunixport_search;

class cunixport : public csocket, public clinuxport {
public:

	//
	// class wide static methods
	//

	// public list of existing cunixports
	static std::set<cunixport*> cunixport_list;

	/** find cunixport instance based on devname
	 */
	static cunixport* find(std::string devname);


	/** auxiliary class for finding cunixport instances
	 */
	class cunixport_search {
public:
		cunixport_search(std::string& devname) : devname(devname)
		{
		};
		bool
		operator()(const cunixport* cp)
		{
			if (cp->devname == this->devname)
				return true;
			return false;
		};
		std::string devname;
	};
	friend class cunixport_search;

public:

	// constructor
	cunixport(std::string devname, int port_no = -1);
	// destructor
	virtual
	~cunixport();

protected:

	//
	// inherited from cport
	//

	/** handle out queue
	 */
	virtual void handle_out_queue();

	//
	// inherited from ciosrv and overloaded from csocket
	//

	/** handle read events on file descriptor
	 */
	virtual void handle_read(int fd);

	//
	// data structures
	//

	// bound interface
	caddress baddr;
};


#endif
