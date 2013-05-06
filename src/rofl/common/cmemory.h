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

namespace rofl
{

/* error classes */
class eMemBase 				: public cerror {};
class eMemAllocFailed 		: public eMemBase {};
class eMemOutOfRange 		: public eMemBase {};
class eMemNotFound 			: public eMemBase {};
class eMemInval 			: public eMemBase {};



/**
 * @class 	cmemory
 * @brief	C++ abstraction for malloc'ed memory areas.
 *
 * This class provides a set of convenience methods for dealing with
 * memory areas managed by the C-functions malloc/realloc/free.
 * It is not hiding or preventing access from the memory area.
 * The user can get access to the memory addresses via somem()
 * and calculate offsets. However, when resizing the memory area,
 * internal memory addresses may have changed. cmemory tracks
 * such changes and updates its internal variables appropriately.
 * Memory addresses kept outside of cmemory must be updated by
 * the developer explicitly.
 */
class cmemory :
	public csyslog
{
private:

	static std::set<cmemory*> 		cmemory_list;
	static pthread_mutex_t 			memlock;
	static int 						memlockcnt;

	std::pair<uint8_t*, size_t> 	data;		//< memory area including head- and tail-space
	std::string 					info; 		//< info string
	size_t 							occupied; 	//< amount of bytes used in memory area

#define CMEMORY_DEFAULT_SIZE 		1024


public:



	/**
	 * @brief	Constructor. Allocates a new memory area with specified size (default: 1024 bytes).
	 *
	 * @param len length of new memory area to be allocated via malloc.
	 */
	cmemory(
			size_t len = CMEMORY_DEFAULT_SIZE);



	/**
	 * @brief	Constructor. Allocates a new memory area and clones specified buffer.
	 *
	 * @param data pointer to start of memory area to be cloned
	 * @param datalen length of memory area to be cloned
	 */
	cmemory(
			uint8_t *data,
			size_t datalen);



	/**
	 * @brief	Copy constructor.
	 *
	 * @param m cmemory instance
	 */
	cmemory(
			cmemory const& m);



	/**
	 * @brief	Destructor. Calls C-function free() for allocated memory area.
	 *
	 */
	virtual
	~cmemory();



	/**
	 * @brief	Returns a C-string containing an ASCII representation of the memory area.
	 *
	 * @return C-string
	 */
	const char*
	c_str();



public:

	/**
	 * @name Memory operations.
	 */

	/**@{*/

	/**
	 * @brief	Assignment operator.
	 *
	 * @param m cmemory instance for operation
	 */
	cmemory&
	operator= (
			cmemory const& m);


	/**
	 * @brief	Index operator.
	 *
	 * @param index ijndex 0 refers to first byte in memory area
	 * @return reference to byte value as uint8_t
	 * @exception eMemOutOfRange thrown when index points beyond end of allocated memory area
	 */
	uint8_t&
	operator[] (
			size_t index) const;



	/**
	 * @brief	Comparison operator.
	 *
	 * @param m cmemory instance for operation
	 */
	bool
	operator== (
			cmemory const& m) const;



	/**
	 * @brief	Comparison operator (unequal).
	 *
	 * @param m cmemory instance for operation
	 */
	bool
	operator!= (
			cmemory const& m) const;



	/**
	 * @brief	Less than operator.
	 *
	 * @param m cmemory instance for operation
	 */
	bool
	operator< (
			cmemory const& m) const;



	/**
	 * @brief	AND operator
	 *
	 * @param m cmemory instance for operation
	 * @exception eMemInval thrown when memory areas differ in length
	 */
	cmemory
	operator& (
			cmemory const& m) const;



	/**
	 * @brief	Append operator with assignment to this cmemory instance.
	 *
	 * @param m cmemory instance for operation
	 */
	cmemory&
	operator+= (
			cmemory const& m);



	/**
	 * @brief	Append operator with assignment to temporary cmemory instance.
	 *
	 * @param m cmemory instance for operation
	 */
	cmemory
	operator+ (
			cmemory const& m);



	/**
	 * @brief	Returns pointer to start of allocated memory area.
	 *
	 */
	uint8_t*
	somem() const;



	/**
	 * @brief	Returns length of allocated memory area.
	 *
	 */
	size_t
	memlen() const;



	/**
	 * @brief	Overwrites memory area with plain buffer specified. Resizes internal memory as necessary.
	 *
	 * @param buf pointer to start of buffer to be copied
	 * @param buflen length of buffer to be copied
	 */
	void
	assign(
			uint8_t *buf,
			size_t buflen);



	/**
	 * @brief	Resizes allocated memory area by calling C-function realloc().
	 *
	 * This method resizes the internal memory area and adjusts all internal
	 * parameters accordingly, so that any subsequent calls to @see somem()
	 * will yield valid results.
	 *
	 * @param len new length of allocated memory area after resize operation
	 * @exception eMemAllocFailed is thrown when the C-function realloc() fails
	 */
	virtual uint8_t*
	resize(
			size_t len);



	/**
	 * @brief 	Inserts len bytes at pointer ptr into allocated memory area.
	 *
	 * @param ptr start location of new inserted memory block
	 * @param len number of bytes inserted in the allocated memory area
	 * @exception eMemInval is thrown when ptr points to a location outside of the allocated memory area
	 */
	uint8_t*
	insert(
			uint8_t *ptr,
			size_t len);


	/**
	 * @brief	Inserts len bytes at start of allocated memory area plus offset.
	 *
	 * @param offset relative start location of new inserted memory block
	 * @param len number of bytes inserted in the allocated memory area
	 * @exception eMemInval is thrown when offset refers to a location outside of the allocated memory area
	 */
	uint8_t*
	insert(
			unsigned int offset,
			size_t len);



	/**
	 * @brief	Removes len bytes starting at pointer ptr from allocated memory area.
	 *
	 * @param ptr start location of memory block to be removed
	 * @param len length of memory block to be removed
	 * @exception eMemInval is thrown when ptr points to a location outside of the allocated memory area
	 */
	void
	remove(
			uint8_t *ptr,
			size_t len);



	/**
	 * @brief	Removes len bytes starting at offset relative to the allocated memory area.
	 *
	 * @param offset relative start location of memory block to be removed
	 * @param len length of memory block to be removed
	 * @exception eMemInval is thrown when the relative start location points to a location outside of the allocated memory area
	 */
	void
	remove(
			unsigned int offset,
			size_t len);



	/**
	 * @brief	Returns index of first byte with value "value" found in allocated memory area starting at offset "start".
	 *
	 * @param value sought value
	 * @param start relative begin within allocated memory area for search
	 * @exception eMemNotFound is thrown when a byte with the specified value cannot be found
	 */
	unsigned int
	find_first_of(
			uint8_t value,
			unsigned int start);



	/**
	 * @brief	Clears the allocated memory area by setting all bytes to 0.
	 *
	 */
	void
	clear();



	/**@}*/



private: // methods


	/** allocate memory
	 *
	 */
	void
	mallocate(
			size_t len) throw (eMemAllocFailed);


	/** free memory
	 *
	 */
	void mfree();


};

}; // end of namespace

#endif
