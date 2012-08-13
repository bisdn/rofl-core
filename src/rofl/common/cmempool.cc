/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cmempool.h"


/*
 * cmemmgmt => contains several memory pools of different sizes
 */

// static data from cmemmgmt
std::string cmempool::info;
std::vector<cmempool*> cmempool::pools;
unsigned int cmempool::n_pools = DEFAULT_POOLS_NUM;
unsigned int cmempool::n_slots = DEFAULT_SLOTS_NUM;
unsigned int cmempool::n_step = DEFAULT_SLOT_STEP;
std::map<void*, unsigned int> cmempool::inuse;
pthread_rwlock_t cmempool::memlock;


/*static*/
void
cmempool::init(
		unsigned int __n_pools,
		unsigned int __n_slots,
		unsigned int __n_step)
{
	pthread_rwlock_init(&memlock, NULL);

	n_pools = __n_pools;
	n_slots = __n_slots;
	n_step  = __n_step;

	RwLock lock(&memlock, RwLock::RWLOCK_WRITE);

	for (unsigned int i = 0; i < n_pools; i++)
	{
		pools.push_back(new cmempool(n_slots, (i+1) * n_step)); // yields max: 2k bytes slots
	}
}


/*static*/
void
cmempool::destroy()
{
	{
		RwLock lock(&memlock, RwLock::RWLOCK_WRITE);

		for (std::vector<cmempool*>::iterator
				it = pools.begin(); it != pools.end(); ++it)
		{
			delete ((*it));
		}
	}

	pthread_rwlock_destroy(&memlock);
}


/*static*/
void*
cmempool::salloc(size_t size)
{
	RwLock lock(&memlock, RwLock::RWLOCK_WRITE);

	// find the memory pool that satisfies 'size'
	unsigned int index = ((unsigned int)size / n_step);

	if (index >= pools.size())
	{
		return 0; // or better throw exception?
	}
	void *ptr = pools[index]->palloc();
	inuse[ptr] = index;
	return ptr;
}


/*static*/
void
cmempool::sfree(
		void *ptr)
{
	RwLock lock(&memlock, RwLock::RWLOCK_WRITE);

	std::map<void*, unsigned int>::iterator it;
	if ((it = inuse.find(ptr)) == inuse.end())
	{
		return;
	}
	unsigned int index = it->second;
	pools[index]->pfree(ptr);
	inuse.erase(it);
}


/*static*/
void*
cmempool::srealloc(
		void *ptr, size_t size)
{
	unsigned int index = 0;
	void *n_ptr = 0;

	{
		RwLock lock(&memlock, RwLock::RWLOCK_WRITE);

		std::map<void*, unsigned int>::iterator it;
		if ((it = inuse.find(ptr)) == inuse.end())
		{
			goto direct;
		}

		index = it->second;

		if (pools[index]->p_slot_size >= size) // ignore requests to shrink for now
		{
			return ptr;
		}
	}

	n_ptr = salloc(size);

	if (0 == n_ptr)
	{
		return 0;
	}

	memcpy(n_ptr, ptr, pools[index]->p_slot_size); // old memory is at most s_slot in bytes

	sfree(ptr);

	return n_ptr;

direct:
	return salloc(size);
}


const char*
cmempool::c_str()
{
	cvastring vas(2048);
	info.clear();

	//RwLock lock(&memlock, RwLock::RWLOCK_READ);
	for (std::vector<cmempool*>::iterator
			it = pools.begin(); it != pools.end(); ++it)
	{
		info.append(vas("cmempool()\n%s", (*it)->p_str()));
	}
	return info.c_str();
}





/*
 * memory pools
 */

cmempool::cmempool(
		unsigned int __p_num_slots,
		size_t __p_slot_size)
	throw (eMemPoolOom) :
		p_num_slots(__p_num_slots),
		p_slot_size(__p_slot_size)
{
	WRITELOG(CMEMPOOL, DBG, "cmempool(%p)::cmempool() n_slots:%d s_slot:%d",
			this, p_num_slots, p_slot_size);
	pthread_rwlock_init(&poollock, NULL);

	RwLock lock(&poollock, RwLock::RWLOCK_WRITE);

	expand();
}


cmempool::~cmempool()
{
	WRITELOG(CMEMPOOL, DBG, "cmempool(%p)::~cmempool() #allocated areas:%d",
			this, p_mem.size());

	{
		RwLock lock(&poollock, RwLock::RWLOCK_WRITE);

		for (std::vector<void*>::iterator
				it = p_mem.begin(); it != p_mem.end(); ++it)
		{
			free((*it));
		}
	}

	pthread_rwlock_destroy(&poollock);
}


const char*
cmempool::p_str()
{
	cvastring vas(1024);

	p_info.clear();

	//RwLock lock(&poollock, RwLock::RWLOCK_READ);

	for (unsigned int i = 0; i < memslots.size(); ++i)
	{
		if (freeslots.find(i) == freeslots.end())
		{
			p_info.append(vas("index:%d slot:%s [+]\n", i, memslots[i]->c_str()));
		}
		else
		{
			p_info.append(vas("index:%d slot:%s [-]\n", i, memslots[i]->c_str()));
		}
	}
	return p_info.c_str();
}


void*
cmempool::palloc()
	throw (eMemPoolOom)
{
	if (freeslots.empty())
	{
		expand();
	}

	RwLock lock(&poollock, RwLock::RWLOCK_WRITE);

	// get first free slot from list 'freeslots'
	int i = *(freeslots.begin());

	freeslots.erase(i);

	usedslots[memslots[i]->iov_base] = i;

	WRITELOG(CMEMPOOL, DBG, "cmempool(%p)::salloc() index:%d slot:%s",
			this, i, memslots[i]->c_str());

	return memslots[i]->iov_base;
}


void
cmempool::pfree(
		void* ptr)
{
	std::map<void*, int>::iterator it;

	WRITELOG(CMEMPOOL, DBG, "cmempool(%p)::sfree() ptr:%p",
			this, ptr);

	RwLock lock(&poollock, RwLock::RWLOCK_WRITE);

	if ((it = usedslots.find(ptr)) == usedslots.end())
	{
		WRITELOG(CMEMPOOL, DBG, "cmempool(%p)::sfree() ptr:%p not found",
				this, ptr);

		return;
	}

	WRITELOG(CMEMPOOL, DBG, "cmempool(%p)::sfree() ptr:%p at index:%d",
			this, ptr, it->second);

	freeslots.insert(it->second);

	usedslots.erase(it);
}


void
cmempool::expand()
{
	int i = p_mem.size();

	p_mem.push_back((void*)0);

	if ((p_mem[i] = (void*)calloc(p_num_slots, p_slot_size)) == NULL)
	{
		throw eMemPoolOom();
	}

	unsigned int offset = memslots.size();

	for (unsigned int j = 0; j < p_num_slots; ++j)
	{
		memslots.push_back(new cmemslot((uint8_t*)p_mem[i] + j * p_slot_size, p_slot_size));
		freeslots.insert(offset + j);
	}

	WRITELOG(CMEMPOOL, DBG, "cmempool(%p)::expand()\n%s", this, c_str());
}


void
cmempool::test()
{
	size_t size = 2197;
	void *m = cmempool::salloc(size);


	for (int i = 0; i < 10; i++)
	{
		cmempool::salloc(100);
	}


	cmempool::salloc(2600);

	cmempool::salloc(2800);

	cmempool::salloc(3100);

	cmempool::salloc(5000);

	cmempool::sfree(m);

	WRITELOG(CMEMPOOL, DBG, "cmempool()::test() XXX\n%s", cmempool::c_str());
}


