/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFLIST_H
#define COFLIST_H 1

#include <set>
#include <vector>
#include <algorithm>

#include "cvastring.h"
#include "cerror.h"

#include "rofl/platform/unix/csyslog.h"

namespace rofl
{

class eOFlistBase : public cerror {}; // base error class cofinlist
class eOFlistInval : public eOFlistBase {}; // invalid parameter
class eOFlistNotFound : public eOFlistBase {}; // element not found
class eOFlistOutOfRange : public eOFlistBase {}; // out of range


template<class T>
class coflist : public csyslog {
public: // static methods + data structures


public: // iterators

	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::const_iterator const_iterator;
	iterator begin() { return elems.begin(); }
	iterator end() { return elems.end(); }
	const_iterator begin() const { return elems.begin(); }
	const_iterator end() const { return elems.end(); }

	typedef typename std::vector<T>::reverse_iterator reverse_iterator;
	typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;
	reverse_iterator rbegin() { return elems.rbegin(); }
	reverse_iterator rend() { return elems.rend(); }

protected: // data structures

	std::vector<T> elems; // list of cofinst instances

public: // methods

	/** constructor
	 */
	coflist();

	/** destructor
	 */
	virtual
	~coflist();

	/** copy constructor
	 */
	coflist(coflist<T> const& ofl);

	/** assignment operator
	 */
	coflist<T>&
	operator= (coflist<T> const& ofl);

	/** index operator
	 */
	T&
	operator[] (size_t i) throw (eOFlistOutOfRange);

	/** return size
	 */
	size_t
	size() const;

	/** create new and return this next element
	 */
	T&
	next();

	/** return first element
	 *
	 */
	T&
	front() throw (eOFlistNotFound);

	/** return last element
	 *
	 */
	T&
	back() throw (eOFlistNotFound);

	/** return element as const
	 *
	 */
	T
	elem(size_t i) const throw (eOFlistOutOfRange);

	/** reset list (=deletes all elements)
	 */
	virtual void
	clear();

	/** insert an element at position i
	 */
	T&
	insert(size_t i) throw (eOFlistOutOfRange);

	/** return boolean flag
	 */
	bool
	empty() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, coflist<T> const& oflist) {
		os << "<coflist " << std::endl;
			for (coflist<T>::const_iterator
					it = oflist.begin(); it != oflist.end(); ++it) {
				os << (*it);
				os << std::endl;
			}
		os << ">";
		return os;
	};
};


template <class T>
coflist<T>::coflist()
{

}



template <class T>
coflist<T>::~coflist()
{
	clear();
}


template <class T>
coflist<T>::coflist(coflist<T> const& ofl)
{
	*this = ofl;
}


template <class T>
void
coflist<T>::clear()
{
#if 0
	typename std::vector<T>::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		delete (*it);
	}
#endif
	elems.clear();
}


template <class T>
coflist<T>&
coflist<T>::operator= (coflist<T> const& ofl)
{
	if (this == &ofl)
		return *this;

	this->clear();

	typename std::vector<T>::const_iterator it;
	for (it = ofl.elems.begin(); it != ofl.elems.end(); ++it)
	{
		//this->elems.push_back(new T(*(*it)));
		this->elems.push_back(*it);
	}

	return *this;
}


template <class T>
T&
coflist<T>::operator[] (size_t i) throw (eOFlistOutOfRange)
{
	if (i > elems.size())
	{
		throw eOFlistOutOfRange();
	}
	else if (i == elems.size())
	{
#if 0
		elems.push_back(new T());
#endif
		elems.push_back(T());
	}
#if 0
	return *(elems[i]);
#endif
	return elems[i];
}


template <class T>
size_t
coflist<T>::size() const
{
	return elems.size();
}


template <class T>
T&
coflist<T>::next()
{
	return this->operator[] (elems.size());
}


template <class T>
T&
coflist<T>::front() throw (eOFlistNotFound)
{
	if (elems.empty())
	{
		throw eOFlistNotFound();
	}
	return this->operator[] (0);
}


template <class T>
T&
coflist<T>::back() throw (eOFlistNotFound)
{
	if (elems.empty())
	{
		throw eOFlistNotFound();
	}
	return this->operator[] (elems.size() - 1);
}


template <class T>
T
coflist<T>::elem(size_t i) const throw (eOFlistOutOfRange)
{
	if (i > elems.size())
	{
		throw eOFlistOutOfRange();
	}
#if 0
	return *(elems[i]);
#endif
	return elems[i];
}


template <class T>
T&
coflist<T>::insert(size_t i) throw (eOFlistOutOfRange)
{
	for (size_t j = elems.size(); j > i; j--)
	{
		(*this)[j] = (*this)[j-1];
	}
#if 0
	return *(elems[i]);
#endif
	return elems[i];
}


template <class T>
bool
coflist<T>::empty() const
{
	return elems.empty();
}

}; // end of namespace

#endif //COFLIST_H
