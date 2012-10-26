/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFCTRL_H
#define COFCTRL_H 1

#include <map>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#include "openflow12.h"
#ifdef __cplusplus
}
#endif

#include "../cerror.h"
#include "../ciosrv.h"
#include "../cmemory.h"
<<<<<<< HEAD
#include "../crofbase.h"
=======
#include "../cofbase.h"
>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64
#include "../thread_helper.h"
#include "../cvastring.h"

#include "cofmatch.h"
<<<<<<< HEAD
#include "cofbase.h"
#if 0
#include "cfttable.h"
=======
#include "cofiface.h"
>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64
#include "cftentry.h"
#endif
#include "extensions/cfspentry.h"


<<<<<<< HEAD

class crofbase;
=======
class cofbase;
//class cfwdelem;
>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64
class cfttable;

class eOFctrlBase : public cerror {};
class eOFctrlPortNotFound : public eOFctrlBase {};






class cofctrl :
	public ciosrv,
	public cfspentry_owner
{
public: // data structures

<<<<<<< HEAD
	crofbase *rofbase;							// parent crofbase instance
	std::map<cofbase*, cofctrl*> *ofctrl_list;	// pointer to set storing this entity
	cofbase *ctrl;								// pointer to controlling entity
=======
	cofbase *ofbase;							// parent cofbase instance
	std::map<cofiface*, cofctrl*> *ofctrl_list;	// pointer to set storing this entity
	cofiface *ctrl;								// pointer to controlling entity
>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64
	uint16_t flags;								// config: flags
	uint16_t miss_send_len;						// config: miss_send_len
	std::set<cofmatch*> nspaces;				// list of cofmatch structures depicting controlled namespace
	bool role_initialized;						// true, when role values have been initialized properly
	uint16_t role;								// role of this controller instance
	uint64_t cached_generation_id;				// generation-id used by role requests

private: // data structures

	std::string info;							// info string

public: // methods

	/** constructor
	 */
	cofctrl(
<<<<<<< HEAD
			crofbase *fwdelem,
			cofbase* ctrl,
			std::map<cofbase*, cofctrl*> *ofctrl_list);
=======
			cofbase *ofbase,
			cofiface* ctrl,
			std::map<cofiface*, cofctrl*> *ofctrl_list);
>>>>>>> c9b9080ca65f54e59812bb9dcdc201c3c848fb64

	/** destructor
	 */
	virtual
	~cofctrl();

	/** return info string
	 */
	const char*
	c_str();

	/** handle incoming vendor message (ROFL extensions)
	 */
	void
	experimenter_message_rcvd(cofpacket *pack);

	/** handle incoming FEATURE requests
	 */
	void
	features_request_rcvd(cofpacket *pack);

	/** handle incoming PACKET-OUT messages
	 */
	void
	packet_out_rcvd(cofpacket *pack);

	/** handle incoming FLOW-MOD messages
	 */
	void
	flow_mod_rcvd(cofpacket *pack);

	/** handle incoming GROUP-MOD messages
	 */
	void
	group_mod_rcvd(cofpacket *pack);

	/** handle incoming PORT-MOD messages
	 */
	void
	port_mod_rcvd(cofpacket *pack) throw (eOFctrlPortNotFound);

	/** handle incoming TABLE-MOD messages
	 */
	void
	table_mod_rcvd(cofpacket *pack);

	/** handle incoming ROLE-REQUEST messages
	 */
	void
	role_request_rcvd(cofpacket *pack);

	/** handle incoming BARRIER request
	 */
	void
	barrier_request_rcvd(cofpacket *pack);


protected: // methods



};



#endif
