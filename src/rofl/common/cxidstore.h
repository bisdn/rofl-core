/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cxidstore.h
 *
 *  Created on: 11.10.2012
 *      Author: andreas
 */

#ifndef CXIDSTORE__H
#define CXIDSTORE__H 1

#include <map>

#include "rofl/common/ciosrv.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/cclock.h"
#include "rofl/common/cvalue.h"

namespace rofl
{

#define XIDSTORE_ALL_XIDS	0xffffffff

class eXidStoreBase 			: public RoflException {};
class eXidStoreXidBusy			: public eXidStoreBase {};		// xid is already in use
class eXidStoreNotFound			: public eXidStoreBase {};		// xid not found



class cxidowner
{
protected:

	friend class cxidstore;

	/** virtual destructor for pure virtual base class
	 *
	 */
	virtual ~cxidowner() {};
	/** must be overwritten by owner, if owner wants receive a notification
	 *
	 */
	virtual void
	xid_removed(uint32_t xid) {};
};


class eXidTransBase				: public RoflException {};
class eXidTransNotFound			: public eXidTransBase {};


class cxidtrans
{
/*
 * data structures
 */
public:
	cxidowner							*owner;
	uint32_t 		 					xid;
	cclock			 					timeout;

	std::map<unsigned int, cvalue>		values;

/*
 * methods
 */
public:
	cxidtrans(cxidowner *owner = 0, uint32_t xid = 0, int timeout_secs = 0) :
		owner(owner), xid(xid), timeout(timeout_secs, 0) {};
	cxidtrans(cxidtrans const& xidtrans) {
		*this = xidtrans;
	};
	cxidtrans& operator= (cxidtrans const& xidtrans) {
		if (this == &xidtrans)
			return *this;
		owner 	= xidtrans.owner;
		xid 	= xidtrans.xid;
		timeout	= xidtrans.timeout;
		values	= xidtrans.values;
		return *this;
	};
	cvalue& set(unsigned int key) {
		return values[key];
	};
	cvalue& get(unsigned int key) throw (eXidTransNotFound) {
		if (values.find(key) == values.end())
		{
			throw eXidTransNotFound();
		}
		return values[key];
	};
	void erase(unsigned int key) {
		if (values.find(key) != values.end())
		{
			values.erase(key);
		}
	};
};





class cxidstore :
	public ciosrv
{
/*
 * data structures
 */
private:


		std::map<uint32_t, cxidtrans> transactions;


public:

	typedef std::map<uint32_t, cxidtrans>::iterator iterator;
	typedef std::map<uint32_t, cxidtrans>::const_iterator const_iterator;
	iterator begin() 				{ return transactions.begin(); }
	iterator end() 					{ return transactions.end(); }
	const_iterator begin() const 	{ return transactions.begin(); }
	const_iterator end() const 	{ return transactions.end(); }

	typedef std::map<uint32_t, cxidtrans>::reverse_iterator reverse_iterator;
	typedef std::map<uint32_t, cxidtrans>::const_reverse_iterator const_reverse_iterator;
	reverse_iterator rbegin() 		{ return transactions.rbegin(); }
	reverse_iterator rend() 		{ return transactions.rend(); }


/*
 * methods
 */
public:


		/**
		 *
		 */
		cxidstore() {};


		/**
		 *
		 */
		virtual ~cxidstore() {};


		/**
		 *
		 */
		void
		xid_add(
				cxidowner *owner,
				uint32_t xid,
				int timeout_secs = 0)
					throw (eXidStoreXidBusy);


		/**
		 *
		 */
		void
		xid_rem(
				uint32_t xid = XIDSTORE_ALL_XIDS);


		/**
		 *
		 */
		cxidtrans&
		xid_find(
				uint32_t xid)
					throw (eXidStoreNotFound);


		/**
		 *
		 */
		bool
		empty() const;


protected:


		/**
		 *
		 */
		virtual void
		handle_timeout(
				int opaque);


};

}; // end of namespace

#endif /* CXIDSTORE_H */
