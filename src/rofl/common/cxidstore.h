/*
 * cxidstore.h
 *
 *  Created on: 11.10.2012
 *      Author: andreas
 */

#ifndef CXIDSTORE_H
#define CXIDSTORE_H 1

#include <map>

#include <rofl/common/ciosrv.h>
#include <rofl/common/cerror.h>

#define XIDSTORE_ALL_XIDS	0xffffffff

class eXidStoreBase 			: public cerror {};
class eXidStoreXidBusy			: public eXidStoreBase {};		// xid is already in use



class cxidowner
{
public:
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




class cxidtrans
{
/*
 * data structures
 */
public:
	cxidowner	*owner;
	uint32_t 	 xid;

/*
 * methods
 */
public:
	cxidtrans(cxidowner *owner, uint32_t xid) :
		owner(owner), xid(xid) {};
	cxidtrans(cxidtrans const& xidtrans) {
		*this = xidtrans;
	};
	cxidtrans& operator= (cxidtrans const& xidtrans) {
		if (this == &xidtrans)
			return *this;
		owner 	= xidtrans.owner;
		xid 	= xidtrans.xid;
		return *this;
	};
};





class cxidstore :
	public ciosrv
{
/*
 * data structures
 */
public:

		std::map<uint32_t, cxidtrans> transactions;


/*
 * methods
 */
public:

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
				uint32_t xid)
					throw (eXidStoreXidBusy);


		/**
		 *
		 */
		void
		xid_rem(
				uint32_t xid = XIDSTORE_ALL_XIDS);


protected:


		/**
		 *
		 */
		virtual void
		handle_timeout(
				int opaque);


};


#endif /* CXIDSTORE_H */
