/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cxidstore.cc
 *
 *  Created on: 11.10.2012
 *      Author: andreas
 */

#include <rofl/common/cxidstore.h>

using namespace rofl;

void
cxidstore::xid_add(
		cxidowner* owner,
		uint32_t xid,
		int timeout_secs)
	throw (eXidStoreXidBusy)
{
	if (transactions.find(xid) != transactions.end())
	{
		throw eXidStoreXidBusy();
	}
	transactions[xid] = cxidtrans(owner, xid, timeout_secs);
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



cxidtrans&
cxidstore::xid_find(
		uint32_t xid)
			throw (eXidStoreNotFound)
{
	if (transactions.find(xid) == transactions.end())
	{
		throw eXidStoreNotFound();
	}
	return transactions[xid];
}



bool
cxidstore::empty() const
{
	return transactions.empty();
}



void
cxidstore::handle_timeout(
		int opaque)
{

}


