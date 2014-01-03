/*
 * crofchan.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CROFCHAN_H_
#define CROFCHAN_H_

#include <map>

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include "croflexception.h"
#include "ciosrv.h"
#include "crofconn.h"
#include "openflow/messages/cofmsg.h"
#include "openflow/cofhelloelemversionbitmap.h"

namespace rofl {
namespace openflow {

class eRofChanBase			: public RoflException {};
class eRofChanNotFound		: public eRofChanBase {};

class crofchan; // forward declaration

class crofchan_env {
public:
	virtual ~crofchan_env() {};
	virtual void handle_close(crofchan *chan) = 0;
	virtual void recv_message(crofchan *chan, cofmsg *msg) = 0;
	virtual uint32_t get_async_xid(crofchan *chan) = 0;
	virtual uint32_t get_sync_xid(crofchan *chan) = 0;
	virtual void release_sync_xid(crofchan *chan, uint32_t xid) = 0;
};

class crofchan :
		public crofconn_env,
		public ciosrv
{
	crofchan_env						*env;
	std::map<uint8_t, crofconn*>		conns;				// main and auxiliary connections
	cofhello_elem_versionbitmap			versionbitmap;		// supported OFP versions

public:

	/**
	 *
	 */
	crofchan(
			crofchan_env *env,
			cofhello_elem_versionbitmap const& versionbitmap);

	/**
	 *
	 */
	virtual
	~crofchan();

public:

	virtual void
	handle_close(crofconn *conn);

	virtual void
	recv_message(crofconn *conn, cofmsg *msg);

	virtual uint32_t
	get_async_xid(crofconn *conn);

	virtual uint32_t
	get_sync_xid(crofconn *conn);

	virtual void
	release_sync_xid(crofconn *conn, uint32_t xid);

public:

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	crofconn&
	get_conn(uint8_t aux_id);

public:

	friend std::ostream&
	operator<< (std::ostream& os, crofchan const& chan) {
		os << indent(0) << "<crofchan >" << std::endl;
		return os;
	};
};

}; /* namespace openflow */
}; /* namespace rofl */




#endif /* CROFCHAN_H_ */
