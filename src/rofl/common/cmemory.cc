/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cmemory.h"


/*static*/std::set<cmemory*> cmemory::cmemory_list;
/*static*/pthread_mutex_t cmemory::memlock;
/*static*/int cmemory::memlockcnt = 0;



cmemory::cmemory(
		size_t len,
		size_t hspace,
		size_t tspace) :
		data(std::make_pair<uint8_t*, size_t>(NULL,0)),
		area(std::make_pair<uint8_t*, size_t>(NULL,0)),
		occupied(0)
{
	if (0 == cmemory::memlockcnt)
	{
		pthread_mutex_init(&cmemory::memlock, NULL);
	}
	++cmemory::memlockcnt;


	if (len > 0)
	{
		mallocate(len, hspace, tspace);
	}

#if 0
	WRITELOG(CMEMORY, ROFL_DBG, "cmemory(%p)::cmemory() somem()=%p memlen()=%d",
			this, somem(), memlen());
#endif
}



cmemory::cmemory(
		uint8_t *data,
		size_t datalen,
		size_t hspace,
		size_t tspace) :
		data(std::make_pair<uint8_t*, size_t>(NULL,0)),
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
		mallocate(datalen, hspace, tspace);
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

	mfree();
}



cmemory&
cmemory::operator= (
		cmemory const& m)
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
cmemory::operator[] (
		size_t index) const
{
	if (index >= area.second)
	{
		throw eMemOutOfRange();
	}
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
		size_t offset = area.first - data.first;

		data.second = len + CMEMORY_DEFAULT_TAIL_SPACE;

		if ((data.first = (uint8_t*)realloc(data.first, data.second)) == NULL)
		{
			throw eMemAllocFailed();
		}

		area.first = data.first + offset;
		area.second = len;
	}
	return area.first;
}



void
cmemory::clear()
{
	if (!data.first || !data.second)
	{
		return;
	}

	memset(data.first, 0, data.second);
}



void
cmemory::mallocate(
		size_t len,
		size_t hspace,
		size_t tspace)
		throw (eMemAllocFailed)
{
	if (data.first)
	{
		mfree();
	}
	data.second = hspace + len + tspace;


	if ((data.first = (uint8_t*)calloc(1, data.second)) == 0)
	{
		throw eMemAllocFailed();
	}

	area.first = data.first + hspace;
	area.second = area.second - (hspace + tspace);
}



void
cmemory::mfree()
{
	if (data.first)
	{
		memset(data.first, 0, data.second);
		free(data.first);
	}
	data = std::make_pair<uint8_t*, size_t>(NULL, 0);
	area = std::make_pair<uint8_t*, size_t>(NULL, 0);
}



uint8_t*
cmemory::insert(
		uint8_t *ptr,
		size_t len) throw (eMemInval)
{
	if (not ((ptr >= area.first) && (ptr < (area.first + area.second))))
	{
		throw eMemInval();
	}

	return insert(ptr - area.first, len);
}



uint8_t*
cmemory::insert(
		unsigned int offset,
		size_t len) throw (eMemInval)
{
	size_t hspace = area.first - data.first;
	size_t tspace = data.second - (hspace + area.second);

	if (offset < (area.second / 2))
	{
		if (len > hspace)
		{
			resize(data.second + len);
		}

		memmove(area.first, area.first - len, (offset + len));
		area.first 	-= len;
		area.second += len;
	}
	else
	{
		if (len > tspace)
		{
			resize(data.second + len);
		}

		memmove(area.first + offset, area.first + offset + len,
												area.second - offset);
		area.second += len;
	}

	return (somem() + offset);
}



void
cmemory::remove(
		uint8_t *ptr,
		size_t len) throw (eMemInval)
{
	if (not ((ptr >= area.first) && (ptr < (area.first + area.second))))
	{
		throw eMemInval();
	}

	if (not (((ptr + len) >= area.first) && ((ptr + len) < (area.first + area.second))))
	{
		throw eMemInval();
	}

	remove(ptr - area.first, len);
}



void
cmemory::remove(
		unsigned int offset,
		size_t len) throw (eMemInval)
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



