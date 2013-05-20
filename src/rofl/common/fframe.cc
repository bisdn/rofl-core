/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fframe.h"

using namespace rofl;

fframe::fframe(
		uint8_t *_data,
		size_t _datalen) :
		mem(0),
		data(_data),
		datalen(_datalen),
		next(0),
		prev(0)
{
	flags.reset(FFRAME_FLAG_MEM);
}



fframe::fframe(
		size_t len) :
		mem(len),
		data(mem.somem()),
		datalen(mem.memlen()),
		next(0),
		prev(0)
{
	flags.set(FFRAME_FLAG_MEM);
}


fframe::~fframe()
{
	if (next)
	{
		next->prev = prev;
	}

	if (prev)
	{
		prev->next = next;
	}
}


void
fframe::reset(uint8_t *_data, size_t _datalen) //, uint16_t _total_len)
{
	if (flags.test(FFRAME_FLAG_MEM))
	{
		//WRITELOG(FFRAME, DBG, "fframe(%p)::reset() clone data", this);

		mem.resize(_datalen);

		memcpy(mem.somem(), _data, mem.memlen());

		data = mem.somem();
		datalen = mem.memlen();
//		total_len = mem.memlen();
	}
	else
	{
		//WRITELOG(FFRAME, DBG, "fframe(%p)::reset() refer to external data", this);

		data = _data;
		datalen = _datalen;
//		total_len = _total_len;
	}
	//WRITELOG(FFRAME, DBG, "fframe(%p)::reset() data:%p datalen:%d", this, data, datalen);

	//Call initialize
	if(data)
		initialize();
}


fframe::fframe(const fframe& frame)
{
	*this = frame;
}


fframe&
fframe::operator= (const fframe& frame)
{
	if (this == &frame)
		return *this;

	flags = frame.flags;

#if 0
	WRITELOG(FFRAME, DBG, "fframe(%p)::operator=() to be copied: data:%p datalen:%d",
			this, frame.data, frame.datalen);
#endif

	// hint: there are two modes:
	// 1. FFRAME_FLAG_MEM is set: fframe's memory area 'mem' contains the data
	// 2. FFRAME_FLAG_MEM is not set: fframe points to some memory area outside
	//
	// reset() can handle both situations as long as flags is correctly set

	// copy frame.mem to this->mem and adjust pointers
	this->reset(frame.data, frame.datalen); //, frame.total_len);

	return *this;
}


uint8_t&
fframe::operator[] (size_t idx) throw (eFrameOutOfRange)
{
	if (idx >= datalen)
		throw eFrameOutOfRange();
	return *(soframe() + idx);
}


const char*
fframe::c_str()
{
	info.clear();
	//return info.c_str(); // performance improvement :D

	char _info[512];
	bzero(_info, sizeof(_info));
	snprintf(_info, sizeof(_info)-1, "[fframe(%p) next:%p prev:%p data:%p datalen:%d [",
			this, next, prev, data, (int)datalen);
	info.assign(_info);
	for (int i = 0; i < (int)datalen; ++i)
	{
		char t[32];
		memset(t, 0, sizeof(t));
		snprintf(t, sizeof(t)-1, "%02x ", data[i]);
		info.append(t);
	}
	info.append("] ]");

	return info.c_str();
}

