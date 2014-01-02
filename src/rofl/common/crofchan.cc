/*
 * crofchan.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "crofchan.h"

using namespace rofl;
using namespace rofl::openflow;


crofchan::crofchan(
		crofchan_env *env,
		cofhello_elem_versionbitmap const& versionbitmap) :
				env(env),
				versionbitmap(versionbitmap)
{

}



crofchan::~crofchan()
{

}



void
crofchan::handle_close(crofconn *conn)
{

}

void
crofchan::recv_message(crofconn *conn, cofmsg *msg)
{

}

uint32_t
crofchan::get_async_xid(crofconn *conn)
{
	return env->get_async_xid(this);
}

uint32_t
crofchan::get_sync_xid(crofconn *conn)
{
	return env->get_sync_xid(this);
}

void
crofchan::release_sync_xid(crofconn *conn, uint32_t xid)
{
	env->release_sync_xid(this, xid);
}


