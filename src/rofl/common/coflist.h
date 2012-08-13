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

#include "../platform/unix/csyslog.h"


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
	reset();

	/** insert an element at position i
	 */
	T&
	insert(size_t i) throw (eOFlistOutOfRange);

	/** return boolean flag
	 */
	bool
	empty() const;
};

#endif
