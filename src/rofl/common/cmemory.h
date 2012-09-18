/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CMEMORY_H
#define CMEMORY_H 1

#include <set>
#include <string>
#include <sstream>

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
#include "cvastring.h"

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



public: // data structures


	std::pair<uint8_t*, size_t> data;		//< memory area including head- and tail-space
	std::pair<uint8_t*, size_t> area;		//< public memory area allocated


private:




#define CMEMORY_DEFAULT_SIZE 			1024
#define CMEMORY_DEFAULT_HEAD_SPACE		64
#define CMEMORY_DEFAULT_TAIL_SPACE		0


public: // public methods



	/** constructor
	 *
	 */
	cmemory(
			size_t len = CMEMORY_DEFAULT_SIZE,
			size_t hspace = CMEMORY_DEFAULT_HEAD_SPACE,
			size_t tspace = CMEMORY_DEFAULT_TAIL_SPACE);


	/** constructor
	 *
	 */
	cmemory(
			uint8_t *data,
			size_t datalen,
			size_t hspace = CMEMORY_DEFAULT_HEAD_SPACE,
			size_t tspace = CMEMORY_DEFAULT_TAIL_SPACE);


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



	/**
	 *
	 */
	uint8_t*
	insert(
			uint8_t *ptr,
			size_t len) throw (eMemInval);


	/** insert at offset len bytes
	 *
	 */
	uint8_t*
	insert(
			unsigned int offset,
			size_t len) throw (eMemInval);


	/** remove at offset len bytes
	 *
	 */
	void
	remove(
			uint8_t *ptr,
			size_t len) throw (eMemInval);


	/** remove at offset len bytes
	 *
	 */
	void
	remove(
			unsigned int offset,
			size_t len) throw (eMemInval);






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


private: // methods


	/** allocate memory
	 *
	 */
	void
	mallocate(
			size_t len,
			size_t hspace = 0,
			size_t tspace = 0) throw (eMemAllocFailed);


	/** free memory
	 *
	 */
	void mfree();


private: // dats structures

	//< info string
	std::string info;
	//< amount of bytes used in memory area
	size_t occupied;
};

#endif
