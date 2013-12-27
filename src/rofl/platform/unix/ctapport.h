/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CTAPPORT_H
#define CTAPPORT_H

#include <sys/types.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <algorithm>
#include <string>

#include "rofl/common/cpacket.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/csocket.h"

#include "clinuxport.h"

namespace rofl
{

/** error classes */
class eTapPortInval : public RoflException {}; // invalid parameter
class eTapPortWriteFailed : public RoflException {}; // write failed
class eTapPortOpenFailed : public RoflException {}; // open failed

class ctapport : public clinuxport {
public: // static

	// public list of existing cports
	static std::set<ctapport*> ctapport_list;

	/** find ctapport instance based on devname
	 */
	static ctapport* find(std::string devname);

	/** auxiliary class for finding ctapport instances
	 */
	class ctapport_search {
	public:
		ctapport_search(std::string& devname) :
			devname(devname) { };
		bool
		operator()(const ctapport* ct)
		{
			if (ct->devname == this->devname)
				return true;
			return false;
		};
		std::string devname;
	};
	friend class ctapport_search;

public:


	/** constructor
	 *
	 */
	ctapport(
			cport_owner *owner,
			std::string devname,
			cmacaddr const& ma = cmacaddr("00:00:00:00:00:00"));


	/** destructor
	 *
	 */
	virtual
	~ctapport();

protected:

	/** open tapX device
	 */
	void
	tap_open(std::string devname);

	/** close tapX device
	 *
	 */
	void
	tap_close();

protected: // overloaded from cport

	/** handle out queue
	 */
	virtual void
	handle_out_queue();

protected: // overloaded from ciosrv

	/** handle read events on file descriptor
	 */
	virtual void
	handle_revent(int fd);

	/** handle write events on file descriptor
	 */
	virtual void
	handle_wevent(int fd);


protected: // data structures

	// tap file descriptor
	int fd;
};

}; // end of namespace

#endif
