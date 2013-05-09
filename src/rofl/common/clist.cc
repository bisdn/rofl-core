/*
 * clist.cc
 *
 *  Created on: 09.05.2013
 *      Author: andi
 */

#include "clist.h"

using namespace rofl;



clist::clist(
		std::list<clistelem*> const& lst)
{
	for (std::list<clistelem*>::const_iterator
			it = lst.begin(); it != lst.end(); ++it) {
		push_back((*(*it)).clone());
	}
}



clist::clist(
		clist const& lst)
{
	*this = lst;
}



clist::~clist()
{
	for (std::list<clistelem*>::iterator
			it = begin(); it != end(); ++it) {
		delete (*it);
	}
}



clist&
clist::operator= (clist const& lst)
{
	if (this == &lst)
		return *this;

	for (std::list<clistelem*>::iterator
			it = begin(); it != end(); ++it) {
		delete (*it);
	}

	for (std::list<clistelem*>::const_iterator
			it = lst.begin(); it != lst.end(); ++it) {
		push_back((*(*it)).clone());
	}

	return *this;
}



clistelem&
clist::operator[] (int idx)
{
	if (idx < 0) {
		idx = (size() - 1) - (abs(idx) - 1);
	}

	if ((idx <  0) || ((size_t)idx >= size()))
		throw eListOutOfRange();

	std::list<clistelem*>::iterator it = begin();
	while (idx > 0) {
		++it;
		--idx;
	}
	return *(*it);
}



size_t
clist::length() const
{
	size_t len = 0;
	for (std::list<clistelem*>::const_iterator
			it = begin(); it != end(); ++it) {
		len += (*(*it)).length();
	}
	return len;
}



void
clist::pack(uint8_t *buf, size_t len) const
{
	if (len < length())
		throw eListTooShort();

	size_t offset = 0;
	for (std::list<clistelem*>::const_iterator
			it = begin(); it != end(); ++it) {
		(*(*it)).pack(buf + offset, (*(*it)).length());
		offset += (*(*it)).length();
	}
}



