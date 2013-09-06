/*
 * clist.h
 *
 *  Created on: 09.05.2013
 *      Author: andi
 */

#ifndef CLIST_H_
#define CLIST_H_ 1

#include <list>
#include <stdlib.h>
#include <inttypes.h>

#include "cerror.h"


namespace rofl
{

class eListBase : public cerror {};
class eListInval : public eListBase {};
class eListTooShort : public eListBase {};
class eListOutOfRange : public eListBase {};


class clistelem
{
public:

	/**
	 *
	 */
	virtual
	~clistelem() {};


public:


	/**
	 *
	 */
	virtual clistelem*
	clone() const = 0;


	/**
	 *
	 */
	virtual size_t
	length() const = 0;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf, size_t len) const = 0;


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t len) = 0;
};



class clist :
		public std::list<clistelem*>
{
public:


	/**
	 *
	 */
	clist(std::list<clistelem*> const& lst = std::list<clistelem*>());


	/*
	 *
	 */
	clist(clist const& lst);


	/**
	 *
	 */
	virtual
	~clist();


	/**
	 *
	 */
	clist&
	operator= (clist const& lst);


	/**
	 *
	 */
	clistelem&
	operator[] (int idx);


public:


	/**
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf, size_t len) const;


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t len) = 0;
};

} // end of namespace

#endif /* CLIST_H_ */
