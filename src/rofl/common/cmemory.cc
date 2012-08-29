/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cmemory.h"


/*static*/std::set<cmemory*> cmemory::cmemory_list;
/*static*/pthread_mutex_t cmemory::memlock;
/*static*/int cmemory::memlockcnt = 0;

cmemory::cmemory(size_t len) :
		area(std::make_pair<uint8_t*, size_t>(NULL,0)),
		occupied(0)
{
	if (0 == cmemory::memlockcnt)
	{
		pthread_mutex_init(&cmemory::memlock, NULL);
	}
	++cmemory::memlockcnt;


	if (len > 0)
		mallocate(len);

#if 0
	WRITELOG(CMEMORY, ROFL_DBG, "cmemory(%p)::cmemory() somem()=%p memlen()=%d",
			this, somem(), memlen());
#endif
}


cmemory::cmemory(uint8_t *data, size_t datalen) :
		area(std::make_pair<uint8_t*, size_t>(NULL,0)),
		occupied(0)
{
	if (0 == cmemory::memlockcnt)
	{
		pthread_mutex_init(&cmemory::memlock, NULL);
	}
	++cmemory::memlockcnt;


	if (datalen > 0)
	{
		mallocate(datalen);
		memcpy(somem(), data, datalen);
	}

#if 0
	WRITELOG(CMEMORY, ROFL_DBG, "cmemory(%p)::cmemory() somem()=%p memlen()=%d",
				this, somem(), memlen());
#endif
}


cmemory::~cmemory()
{
#if 0
	WRITELOG(CMEMORY, ROFL_DBG, "cmemory(%p)::~cmemory() somem()=%p memlen()=%d",
				this, somem(), memlen());
#endif
	lock();
//	cmemory::cmemory_list.erase(this);
	unlock();

	if (area.first)
		mfree();

	--cmemory::memlockcnt;
	if (0 == cmemory::memlockcnt)
	{
		pthread_mutex_destroy(&cmemory::memlock);
	}
}


cmemory&
cmemory::operator= (cmemory const& m)
{
	if (this == &m)
		return *this;
	this->resize(m.memlen());
	clear();
	memcpy(this->somem(), m.somem(), m.memlen());
	return *this;
}


uint8_t*
cmemory::somem() const
{
	return area.first;
}


size_t
cmemory::memlen() const
{
	return area.second;
}


uint8_t&
cmemory::operator[] (size_t index) const
{
	if (index >= area.second)
		throw eMemOutOfRange();
	return *(somem() + index);
}


bool
cmemory::operator< (const cmemory& m) const
{
	if (this->memlen() < m.memlen())
	{
		return true;
	}
	else if (this->memlen() > m.memlen())
	{
		return false;
	}
	// this->memlen() == m.memlen()

	int rc = memcmp(this->somem(), m.somem(), this->memlen());

	if (rc < 0)
	{
		return true;
	}
	else // rc >= 0
	{
		return false;
	}

	return true; // never reached
}


cmemory
cmemory::operator& (const cmemory& m) const throw (eMemInval)
{
	if (memlen() != m.memlen())
	{
		throw eMemInval();
	}

	cmemory tmp(*this);

	for (size_t i = 0; i < memlen(); ++i)
	{
		tmp[i] &= m[i];
	}

	return tmp;
}


cmemory&
cmemory::operator+= (cmemory const& m)
{
	size_t old_len = memlen();

	resize(old_len + m.memlen());

	memcpy(somem() + old_len, m.somem(), m.memlen());

	return *this;
}


bool
cmemory::operator== (cmemory const& m) const
{
	if (this->memlen() == m.memlen()) {
		if (!memcmp(this->somem(), m.somem(), this->memlen()))
			return true;
	}
	return false;
}


bool
cmemory::operator!= (cmemory& m) const
{
	if (this->memlen() == m.memlen()) {
		if (!memcmp(this->somem(), m.somem(), this->memlen()))
			return false;
	}
	return true;
}


void
cmemory::assign(
		uint8_t *buf,
		size_t buflen)
{
	resize(buflen);
	clear();
	memcpy(somem(), buf, buflen);
}


uint8_t*
cmemory::resize(size_t len) throw (eMemAllocFailed)
{
	if (0 == len)
	{

		mfree();

	}
	else
	{

		area.second = len;
#ifdef MEMPOOL
		if ((area.first = (uint8_t*)cmempool::srealloc(area.first, area.second)) == NULL)
		{
			throw eMemAllocFailed();
		}
#else
		if ((area.first = (uint8_t*)realloc(area.first, area.second)) == NULL)
		{
			throw eMemAllocFailed();
		}
#endif

	}
	return area.first;
}


void
cmemory::clear()
{
	if (!area.first || !area.second)
		return;

	bzero(area.first, area.second);
}


void
cmemory::mallocate(size_t len) throw (eMemAllocFailed)
{
	if (area.first)
	{
		mfree();
	}
	area.second = len;

#ifdef MEMPOOL
	if ((area.first = (uint8_t*)cmempool::salloc(area.second)) == 0)
	{
		throw eMemAllocFailed();
	}
#else
	if ((area.first = (uint8_t*)calloc(1, area.second)) == 0)
	{
		throw eMemAllocFailed();
	}
#endif
}


void
cmemory::mfree()
{
	if (area.first)
	{
		bzero(area.first, area.second);
		// for debugging, if something still refers to this already free'd memory, a crash is more likely
#ifdef MEMPOOL
		cmempool::sfree(area.first);
#else
		free(area.first);
#endif
	}
	area = std::make_pair<uint8_t*, size_t>(NULL, 0);
}


uint8_t*
cmemory::insert(
		unsigned int offset,
		size_t len)
{
	std::pair<uint8_t*, size_t> tmparea;

	tmparea.second = area.second + len;

#ifdef MEMPOOL
	if ((tmparea.first = (uint8_t*)cmempool::salloc(tmparea.second)) == 0)
	{
		throw eMemAllocFailed();
	}
#else
	if ((tmparea.first = (uint8_t*)calloc(1, tmparea.second)) == 0)
	{
		throw eMemAllocFailed();
	}
#endif

	memcpy(tmparea.first, area.first, offset);
	bzero (tmparea.first + offset, len);
	memcpy(tmparea.first + offset + len, area.first + offset, area.second - offset);

	mfree();

	area = tmparea;

	return (tmparea.first + offset);
}


void
cmemory::remove(
		unsigned int offset,
		size_t len)
{
	len = ((offset + len) > area.second) ? area.second - offset : len;

	memmove(area.first + offset, area.first + offset + len, area.second - (offset + len));
	bzero (area.first + area.second - len, 0);
	area.second -= len;
}


unsigned int
cmemory::find_first_of(
		uint8_t value,
		unsigned int start) throw (eMemNotFound)
{
	for (unsigned int i = start; i < memlen(); i++)
	{
		if (operator[] (i) == value)
		{
			return i;
		}
	}
	throw eMemNotFound();
}


const char*
cmemory::c_str()
{
#ifdef NDEBUG
	std::stringstream sstr;
	sstr << "cmemory(" << (std::hex) << this << (std::dec) << ") len[" << (int)memlen() << "]";
	return sstr.str().c_str();

#else

	info.clear();
	char _info[256];
	bzero(_info, sizeof(_info));
	snprintf(_info, sizeof(_info)-1, "cmemory(%p) somem()[%p] len[%d] data[", this, somem(), (int)memlen());
	info.assign(_info);
	for (int i = 0; i < (int)memlen(); ++i)
	{
		char t[8];
		memset(t, 0, sizeof(t));
		snprintf(t, sizeof(t)-1, "%02x ", ((*this)[i]));
		info.append(t);
		if ((i > 32) && (0 == (i % 32)))
		{
			info.append("\n");
		}
	}
	info.append("]");
	return info.c_str();
#endif
}



