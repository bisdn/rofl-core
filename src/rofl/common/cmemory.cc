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
		hspace(hspace),
		tspace(tspace),
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
		hspace(hspace),
		tspace(tspace),
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
cmemory::resize(
		size_t len) throw (eMemAllocFailed)
{
	if (0 == len)
	{
		mfree();
	}
	else if (len <= area.second)
	{
		return area.first;
	}
	else
	{

		//fprintf(stderr, "U[1] %s\n", c_str());
		uint8_t* p_ptr 	= (uint8_t*)0;
		size_t p_len 	= hspace + len + tspace;

		size_t offset 	= hspace + len - area.second;
#if 0
		fprintf(stderr, "len: %lu area.second: %lu hspace: %lu tspace: %lu offset: %lu p_len: %lu\n",
				len, area.second, hspace, tspace, offset, p_len);
#endif

		if ((p_ptr = (uint8_t*)calloc(1, p_len)) == 0)
		{
			throw eMemAllocFailed();
		}

		memset(p_ptr, 0x00, p_len);

		// copy public area
		memcpy(p_ptr + offset, area.first, area.second);

		// free old memory
		free(data.first);

		// adjust data
		data.first 	= p_ptr;
		data.second = p_len;

		// adjust area
		area.first	= data.first + offset;
		// do not change area.second here
		//fprintf(stderr, "U[2] %s\n", c_str());
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
	area.second = data.second - (hspace + tspace);
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
	if ((area.first - data.first) < len)
	{
		//fprintf(stderr, "S[1] %s\n", c_str());
		resize(area.second + len);
		//fprintf(stderr, "S[2] %s\n", c_str());
	}

	//fprintf(stderr, "S[3] %s\n", c_str());
	memmove(area.first - len, area.first, offset);
	//fprintf(stderr, "S[4] %s\n", c_str());
	memset(area.first + offset - len, 0x00, len);
	//fprintf(stderr, "S[5] %s\n", c_str());

	//fprintf(stderr, "S[6] data[%p:%lu] area[%p:%lu]\n", data.first, data.second, area.first, area.second);

	area.first 	-= len;
	area.second += len;

	//fprintf(stderr, "S[7] data[%p:%lu] area[%p:%lu]\n", data.first, data.second, area.first, area.second);

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

	memmove(area.first + len, area.first, offset);
	memset(area.first, 0x00, len);

	area.first += len;
	area.second -= len;

	//memmove(area.first + offset, area.first + offset + len, area.second - (offset + len));
	//bzero (area.first + area.second - len, 0);
	//area.second -= len;
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
	snprintf(_info, sizeof(_info)-1, "cmemory(%p) somem()[%p] len[%d] data[%p:%lu] area[%p:%lu] data[",
			this, somem(), (int)memlen(), data.first, data.second, area.first, area.second);

	info.assign(_info);
	//for (int i = 0; i < (int)memlen(); ++i)
	for (int i = 0; i < (int)area.second; ++i)
	{
		char t[8];
		memset(t, 0, sizeof(t));
		//snprintf(t, sizeof(t)-1, "%02x ", ((*this)[i]));
		snprintf(t, sizeof(t)-1, "%02x ", (area.first[i]));
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



