/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CPACKET_H
#define CPACKET_H 1

#include <utility>
#include <iostream>
#include <string>
#include <deque>
#include <list>
#include <set>
#include <algorithm>
#include <typeinfo>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/uio.h>
#include <assert.h>

#include "rofl/common/croflexception.h"
#include "rofl/common/cclock.h"
#include "rofl/common/fframe.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/thread_helper.h"

#include "rofl/common/openflow/openflow.h"

namespace rofl {

class ePacketBase			: public RoflException {
public:
	ePacketBase(const std::string& __arg) : RoflException(__arg) {};
};
class ePacketInval 			: public ePacketBase {
public:
	ePacketInval(const std::string& __arg) : ePacketBase(__arg) {};
};
class ePacketNotFound 		: public ePacketBase {
public:
	ePacketNotFound(const std::string& __arg) : ePacketBase(__arg) {};
};
class ePacketOutOfRange 	: public ePacketBase {
public:
	ePacketOutOfRange(const std::string& __arg) : ePacketBase(__arg) {};
};


/**
 *
 */
class cpacket : public rofl::cmemory {
public:

	/**
	 *
	 */
	cpacket() :
		rofl::cmemory(0), head(0), tail(0), initial_head(0), initial_tail(0) {};

	/**
	 *
	 */
	virtual
	~cpacket() {};

	/**
	 *
	 */
	cpacket(
			size_t size, size_t head = DEFAULT_HSPACE, size_t tail = DEFAULT_TSPACE) :
				rofl::cmemory(head + size + tail), head(head), tail(tail), initial_head(head), initial_tail(tail) {};

	/**
	 *
	 */
	cpacket(
			uint8_t *buf, size_t buflen, size_t head = DEFAULT_HSPACE, size_t tail = DEFAULT_TSPACE) :
				rofl::cmemory(head + buflen + tail), head(head), tail(tail), initial_head(head), initial_tail(tail)
		{ memcpy(somem()+head, buf, buflen); };

	/**
	 *
	 */
	cpacket(const cpacket& pack)
		{ *this = pack; };

	/**
	 *
	 */
	cpacket&
	operator= (const cpacket& pack) {
		if (this == &pack)
			return *this;
		rofl::cmemory::operator= (pack);
		head = pack.head;
		tail = pack.tail;
		initial_head = pack.initial_head;
		initial_tail = pack.initial_tail;
		return *this;
	};

public:

	/**
	 *
	 */
	void
	clear() {
		rofl::cmemory::clear();
		head = initial_head;
		tail = initial_tail;
	};

public:

	/**
	 *
	 */
	uint8_t&
	operator[] (size_t index) {
		if (index >= length()) {
			throw ePacketOutOfRange("cpacket::operator[] index out of range");
		}
		return (rofl::cmemory::operator[] (head + index));
	};

	/**
	 *
	 */
	bool
	operator== (const cpacket& p) {
		if (length() != p.length())
			return false;
		return (not (memcmp(soframe(), p.soframe(), length())));
	};

	/**
	 *
	 */
	bool
	operator== (const cmemory& m) {
		if (length() != m.memlen())
			return false;
		return (not (memcmp(soframe(), m.somem(), length())));
	};

	/**
	 *
	 */
	bool
	operator!= (
			const cpacket& p) {
		if (length() != p.length())
			return true;
		return (memcmp(soframe(), p.soframe(), length()));
	};

	/**
	 *
	 */
	bool
	operator!= (
			const cmemory& m) {
		if (length() != m.memlen())
			return true;
		return (memcmp(soframe(), m.somem(), length()));
	};

	/**
	 *
	 */
	cpacket
	operator+ (
			const cpacket& pack) {
		cpacket p(this->length() + pack.length());
		memcpy(p.soframe(), this->soframe(), this->length());
		memcpy(p.soframe() + this->length(), pack.soframe(), pack.length());
		return p;
	};

	/**
	 *
	 */
	cpacket&
	operator+= (
			const cpacket& pack) {
		size_t offset = this->length();
		rofl::cmemory::resize(this->length() + pack.length());
		memcpy(this->soframe() + offset, pack.soframe(), pack.length());
		return *this;
	};

public:

	/**
	 *
	 */
	virtual void
	assign(uint8_t *buf, size_t buflen) {
		head = initial_head;
		tail = initial_tail;
		cmemory::resize(head + buflen + tail);
		cmemory::clear();
		memcpy(soframe(), buf, buflen);
	};

	/**
	 *
	 */
	virtual uint8_t*
	resize(size_t len) {
		cmemory::resize(head + len + tail);
		return soframe();
	};

	/**
	 *
	 */
	uint8_t*
	soframe() const
		{ return (rofl::cmemory::somem() + head); };

	/**
	 *
	 */
	bool
	empty() const
		{ return (0 == length()); };

	/**
	 *
	 */
	size_t
	length() const
		{ return (rofl::cmemory::memlen() - head - tail); };

	/**
	 *
	 */
	virtual void
	pack(uint8_t* buf, size_t buflen) {
		if (buflen < length()) {
			throw ePacketInval("cpacket::pack() insufficient space");
		}
		memcpy(buf, soframe(), buflen);
	};

	/**
	 *
	 */
	virtual void
	unpack(uint8_t* buf, size_t buflen) {
		rofl::cmemory::resize(head + buflen + tail);
		memcpy(soframe(), buf, buflen);
	};

private:

	/**
	 *
	 */
	uint8_t*
	tag_insert(
			uint8_t* ptr, size_t len) { throw eNotImplemented(); };

	/**
	 *
	 */
	void
	tag_remove(
			uint8_t* ptr, size_t len) { throw eNotImplemented(); };

public:

	/**
	 *
	 */
	void
	tag_insert(
			size_t len) {
		if (len > head) {
			throw ePacketInval("cpacket::tag_insert() insufficient head space");
		}
		head -= len;
	};

	/**
	 *
	 */
	void
	tag_remove(
			size_t len) {
		if ((len + head) > rofl::cmemory::memlen()) {
			throw ePacketInval("cpacket::tag_insert() invalid tag size");
		}
		head += len;
	};

public:

	/**
	 *
	 */
	friend std::ostream &
	operator<<(std::ostream& os, const cpacket& pack) {
		os << rofl::indent(0) << "<cpacket ";
		os << "data:" << (void*)pack.soframe() << " ";
		os << "datalen:" << (int)pack.length() << " ";
		os << ">" << std::endl;
		rofl::indent i(2);

		unsigned int nbytes = 32;
		if (pack.length() > 0) {
			for (unsigned int i=0; i < pack.length(); i++) {
				if (0 == (i % nbytes)) {
					os << indent(2)
						<< std::setfill('0')
						<< std::setw(4)
						<< std::dec << (i/nbytes) << ": " << std::hex
						<< std::setw(0)
						<< std::setfill(' ');
				}

				os << std::setfill('0')
					<< std::setw(2)
					<< std::hex << (int)(*(pack.soframe() + i)) << std::dec
					<< std::setw(0)
					<< std::setfill(' ')
					<< " ";

				if (0 == ((i+1) % 8))
					os << "  ";
				if (0 == ((i+1) % nbytes))
					os << std::endl;
			}
			os << std::endl;
		}
		return os;
	};

private:

	static const int DEFAULT_SIZE = 0;
	static const int DEFAULT_HSPACE = 64;	// head room for push operations
	static const int DEFAULT_TSPACE = 256;	// tail room for appending payload(s)

	size_t			 head;	// head space size: this is used as extra space for pushing tags
	size_t			 tail;	// tail space size: this is used as extra space for appending payload(s)
	size_t			 initial_head;
	size_t 			 initial_tail;
};

}; // end of namespace rofl

#endif
