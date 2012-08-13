/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CMEMAREA_H
#define CMEMAREA_H 1

#include <list>
#include <vector>
#include <map>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

//#include <linux/uio.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <strings.h>

#ifdef __cplusplus
}
#endif

#include "cmemory.h"
#include "cerror.h"
#include "cvastring.h"
#include "thread_helper.h"

#include "../platform/unix/csyslog.h"

class eMemPoolBase : public cerror {};
class eMemPoolInval : public eMemPoolBase {};
class eMemPoolOom : public eMemPoolBase {};
class eMemPoolCorrupted : public eMemPoolBase {};


class cmemslot
{
private:

	std::string info;

public:

	void  *iov_base; 		// pointer to start of memory slot
	size_t iov_size; 		// size of memory slot

public:

	/**
	 */
	cmemslot(
			void *__iov_base = 0,
			size_t __iov_size = 0) :
		iov_base(__iov_base),
		iov_size(__iov_size)
	{ };

	/**
	 */
	const char*
	c_str()
	{
		cvastring vas;
		info.assign(vas("cmemslot(%p) iov_base:%p iov_size:%d", this, iov_base, iov_size));
		return info.c_str();
	};
};



class cmempool :
	public csyslog
{

/*
 * static variables and methods
 */

public: // static methods

	/** test method
	 *
	 */
	static void
	test();


private: // variables

	static std::string info;

public: // variables

	static std::vector<cmempool*> pools;	// memory pools with different slot slizes (index*64)
	static unsigned int n_pools;			// number of mempools to create
	static unsigned int n_slots; 			// number of slot entries per pool
	static unsigned int n_step;			// granularity of slot size steps

#define DEFAULT_POOLS_NUM 4					// # of mempools
#define DEFAULT_SLOTS_NUM 1024 				// bytes
#define DEFAULT_SLOT_STEP 512 				// bytes

	static std::map<void*, unsigned int> inuse;	// reverse mapping pointer => cmempool

	static pthread_rwlock_t memlock;		// rwlock for public static methods salloc, srealloc, sfree

public: // static methods

	/**
	 *
	 */
	static
	void
	init(
			unsigned int n_pools = DEFAULT_POOLS_NUM,
			unsigned int n_slots = DEFAULT_SLOTS_NUM,
			unsigned int n_step  = DEFAULT_SLOT_STEP);

	/**
	 *
	 */
	static
	void
	destroy();

	/**
	 *
	 */
	static
	void*
	salloc(
			size_t size);

	/**
	 *
	 */
	static void*
	srealloc(
			void *ptr, size_t size);

	/**
	 *
	 */
	static
	void
	sfree(
			void *ptr);

	/**
	 *
	 */
	static
	const char*
	c_str();




/*
 * per instance variables and methods
 */

public: // data structures

		unsigned int p_num_slots;	// number of slots
		size_t p_slot_size; 		// size of single slot

private: // data structures

#define DEFAULT_SLOT_SIZE 2048 // bytes

		pthread_rwlock_t poollock;		// lock for access to memslots

		std::vector<void*> p_mem; // memory areas, each of size (n_slots * s_slot)

		std::vector<cmemslot*> 	memslots;
		std::set<int> 			freeslots;
		std::map<void*, int> 	usedslots; // maps pointer to index into memslots vector

		std::string p_info;

public: // methods

		/**
		 *
		 */
		cmempool(
				unsigned int n_slots = DEFAULT_SLOTS_NUM,
				size_t s_slot = DEFAULT_SLOT_SIZE)
		throw (eMemPoolOom);

		/**
		 *
		 */
		virtual
		~cmempool();

protected:

		/**
		 *
		 */
		void*
		palloc()
		throw (eMemPoolOom);

		/**
		 *
		 */
		void
		pfree(
				void *ptr);

		/**
		 *
		 */
		const char*
		p_str();

private: // methods

		/**
		 *
		 */
		void
		expand();
};



#endif
