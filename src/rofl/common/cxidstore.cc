/*
 * cxidstore.cc
 *
 *  Created on: 11.10.2012
 *      Author: andreas
 */

#include <rofl/common/cxidstore.h>


void
cxidstore::xid_add(
		cxidowner* owner,
		uint32_t xid)
	throw (eXidStoreXidBusy)
{
	if (transactions.find(xid) != transactions.end())
	{
		throw eXidStoreXidBusy();
	}
	transactions[xid] = cxidtrans(owner, xid);
}



void
cxidstore::xid_rem(
		uint32_t xid)
{
	if (XIDSTORE_ALL_XIDS == xid)
	{
		for (std::map<uint32_t, cxidtrans>::iterator
				it = transactions.begin(); it != transactions.end(); ++it)
		{
			cxidtrans& xidt = it->second;

			xidt.owner->xid_removed(xidt.xid);
		}
		transactions.clear();
	}
	else
	{
		if (transactions.find(xid) == transactions.end())
		{
			return;
		}
		transactions[xid].owner->xid_removed(xid);
		transactions.erase(xid);
	}
}



void
cxidstore::handle_timeout(
		int opaque)
{

}


