/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "coflist.h"

using namespace rofl;

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


//GCC seems not to detect and instantiatie #FIXME:?

#include "openflow/cofinst.h"
#include "openflow/cofaction.h"
#include "openflow/cofbucket.h"
#include "openflow/cofport.h"
#include "openflow/coxmatch.h"
#include "protocols/cpppoetlv.h"
//#include "experimental/protocols/clldptlv.h"
#include "openflow/cofqueueprop.h"

#if 1 
template class coflist<cofinst>;
template class coflist<cofaction>;
template class coflist<cofbucket>;
template class coflist<cpppoetlv>;
template class coflist<cofport>;
template class coflist<int>;
template class coflist<coflist<int> >;
template class coflist<coxmatch>;
//template class coflist<clldptlv>;
template class coflist<cofqueue_prop>;
#endif

