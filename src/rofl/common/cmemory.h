/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CMEMORY_H
#define CMEMORY_H 1

#include <set>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <pthread.h>
#include <stdlib.h>

#ifdef __cplusplus
}
#endif

#include "rofl/platform/unix/csyslog.h"

#include "cerror.h"


/* error classes */
class eMemBase : public cerror {};
class eMemAllocFailed : public eMemBase {};
class eMemOutOfRange : public eMemBase {};
class eMemNotFound : public eMemBase {};
class eMemInval : public eMemBase {};



class cmemory :
	public csyslog
{
public: // static stuff

	static std::set<cmemory*> cmemory_list;
	static pthread_mutex_t memlock;
	static int memlockcnt;

public: // public methods

#define CMEMORY_DEFAULT_SIZE 1024

	/** constructor
	 *
	 */
	cmemory(size_t len = CMEMORY_DEFAULT_SIZE);

	/** constructor
	 *
	 */
	cmemory(uint8_t *data, size_t datalen);

	/** destructor
	 *
	 */
	virtual
	~cmemory();

	/** copy constructor
	 *
	 */
	cmemory(cmemory const& m)
	{
		*this = m;
	}

	/** assignment operator
	 *
	 */
	cmemory& operator= (cmemory const& m);

	/** start of mem area
	 *
	 */
	uint8_t* somem() const;

	/** length of mem area
	 *
	 */
	size_t memlen() const;

	/** operator[]
	 *
	 */
	uint8_t& operator[] (size_t index) const;

	/** operator==
	 *
	 */
	bool operator== (cmemory const& m) const;

	/** operator!=
	 *
	 */
	bool operator!= (cmemory& m) const;

	/** operator<
	 *
	 */
	bool operator< (const cmemory& m) const;

	/** operator&
	 *
	 */
	cmemory operator& (const cmemory& m) const throw (eMemInval);

	/** operator +=
	 *
	 */
	cmemory& operator+= (cmemory const& m);

	/** assign
	 *
	 */
	void
	assign(
			uint8_t *buf,
			size_t buflen);

	/** resize
	 *
	 */
	uint8_t* resize(size_t len) throw (eMemAllocFailed);

	/** insert at offset len bytes
	 *
	 */
	uint8_t*
	insert(
			unsigned int offset,
			size_t len);

	/** remove at offset len bytes
	 *
	 */
	void
	remove(
			unsigned int offset,
			size_t len);

	/** find first occurence of byte "value" starting at offset "start"
	 * returns position relative to start of memory area
	 */
	unsigned int
	find_first_of(
			uint8_t value,
			unsigned int start) throw (eMemNotFound);

	/** sets memory area to 0
	 *
	 */
	void
	clear();

	/** dump memory content
	 *
	 */
	const char* c_str();

public: // data structures

	//< memory area allocated
	std::pair<uint8_t*, size_t> area;

private: // methods

	/** allocate memory
	 *
	 */
	void mallocate(size_t len) throw (eMemAllocFailed);

	/** free memory
	 *
	 */
	void mfree();

	/**
	 *
	 */
	void
	lock()
	{
		pthread_mutex_lock(&cmemory::memlock);
	};

	/**
	 *
	 */
	void
	unlock()
	{
		pthread_mutex_unlock(&cmemory::memlock);
	};

private: // dats structures

	//< info string
	std::string info;
	//< amount of bytes used in memory area
	size_t occupied;
};

#endif
