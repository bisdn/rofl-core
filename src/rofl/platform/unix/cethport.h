/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CETHPORT_H
#define CETHPORT_H

#include <set>
#include <string>
#include <algorithm>

#include "rofl/common/csocket.h"
#include "rofl/common/caddress.h"
#include "rofl/common/protocols/fetherframe.h"
#include "rofl/common/thread_helper.h"
#include "rofl/common/logging.h"

#include "clinuxport.h"

namespace rofl
{

class cethport_search;

class cethport :
	public csocket,
	public clinuxport
{
public:

#define OUT_QUEUE_MAX_TX_PER_ROUND 128

	//
	// class wide static methods
	//

	// public list of existing cethports
	static std::set<cethport*> cethport_list;

	/** find cethport instance based on devname
	 */
	static cethport* find(std::string devname);

	/** auxiliary class for finding cethport instances
	 */
	class cethport_search {
public:
		cethport_search(std::string& devname) : devname(devname)
		{
		};
		bool
		operator()(const cethport* cp)
		{
			if (cp->devname == this->devname)
				return true;
			return false;
		};
		std::string devname;
	};
	friend class cethport_search;

public:


	/**
	 *
	 */
	cethport(
			cport_owner *owner,
			std::string devname);


	/**
	 *
	 */
	virtual
	~cethport();


protected:

	//
	// socket specific methods
	//

	/** socket connection successful (client mode)
	 */
	virtual void
	handle_connected()
	{
	};
	/** socket connection unsuccessful (client mode)
	 */
	virtual void
	handle_conn_refused()
	{
	};
	/** connection was closed
	 */
	virtual void
	handle_closed(int sd)
	{
	};

	/** handle out queue
	 */
	virtual void handle_out_queue();

	//
	// inherited from ciosrv and overloaded from csocket
	//

	/** handle read events on file descriptor
	 */
	virtual void handle_read(int fd);
#if 0
	/** handle write events on file descriptor
	 */
	virtual void handle_wevent(int sd);
#endif
	//
	// data structures
	//

	// bound interface
	caddress baddr;
};

}; // end of namespace

#endif
