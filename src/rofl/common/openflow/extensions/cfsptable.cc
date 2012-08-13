/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cfsptable.h"

cfsptable::cfsptable()
{ }


cfsptable::~cfsptable()
{
	std::set<cfspentry*> fsp_table;
	while (not fsp_table.empty())
	{
		delete *(fsp_table.begin());
	}
}


void
cfsptable::insert_fsp_entry(
		cfspentry_owner *fspowner,
		cofmatch const& m,
		bool strict) throw (eFspEntryOverlap)
{
	cofmatch match(m);

	//fprintf(stderr, "TTT => %s\n", match.c_str());

	cfspsearch fspsearch(m, false/*non-strict*/);

	fspsearch = for_each(fsp_table.begin(), fsp_table.end(), fspsearch);

#if 0
	if (not fspsearch.fsp_list.empty())
	{
		throw eFspEntryOverlap();
	}
#endif

	new cfspentry(fspowner, m, &fsp_table);
}


void
cfsptable::delete_fsp_entry(
		cfspentry_owner *fspowner,
		cofmatch const& m,
		bool strict) throw (eFspEntryNotFound)
{
	cfspsearch fspsearch(m, true/*strict*/);

	fspsearch = for_each(fsp_table.begin(), fsp_table.end(), fspsearch);

	if (not fspsearch.fsp_list.empty())
	{
		std::set<cfspentry*>::iterator it;
		for (it = fspsearch.fsp_list.begin(); it != fspsearch.fsp_list.end(); ++it)
		{
			delete (*it); // removes (*it) implicitely from nspc_table
		}
	}
	else
	{
		throw eFspEntryNotFound();
	}
}


void
cfsptable::delete_fsp_entries(
		cfspentry_owner* fspowner)
{
	std::set<cfspentry*>::iterator it = fsp_table.begin();

	while (it != fsp_table.end())
	{
		if ((*it)->fspowner == fspowner)
		{
			delete (*it); // invalidates iterator, so reset it
			it = fsp_table.begin();
		}
		else
		{
			++it;
		}
	}
}



std::set<cfspentry*>
cfsptable::find_matching_entries(
		uint32_t in_port,
		uint32_t total_len,
		uint8_t *data, size_t datalen) throw (eFspNoMatch, eFrameInvalidSyntax)
{
	std::set<cfspentry*> nse_list;

	// TODO: introduce fdataframe here in order to avoid copying the packet
	cpacket pack(data, datalen, in_port);

	uint16_t exact_hits = 0;
	uint16_t wildcard_hits = 0;

	std::set<cfspentry*>::iterator it;
	for (it = fsp_table.begin(); it != fsp_table.end(); ++it)
	{
		uint16_t __exact_hits = 0;
		uint16_t __wildcard_hits = 0;
		uint16_t __misses = 0;

		pack.calc_hits((*it)->ofmatch, __exact_hits, __wildcard_hits, __misses);

		if ((__exact_hits < exact_hits) || (__misses > 0))
		{
			WRITELOG(CNAMESPACE, DBG, "cfsptable(%p)::find_matching_entries() ignoring flowspace", this);
			continue;
		}
		else if (__exact_hits > exact_hits)
		{
			nse_list.clear();
			nse_list.insert(*it);
			exact_hits = __exact_hits;
			wildcard_hits = __wildcard_hits;
			WRITELOG(CNAMESPACE, DBG, "cfsptable(%p)::find_matching_entries() new best match: %s", this, (*it)->ofmatch.c_str());
		}
		else if (__exact_hits == exact_hits)
		{
			if (__wildcard_hits < wildcard_hits)
			{
				continue;
			}
			else if (__wildcard_hits > wildcard_hits)
			{
				nse_list.clear();
				nse_list.insert(*it);

				wildcard_hits = __wildcard_hits;
				WRITELOG(CNAMESPACE, DBG, "cfsptable(%p)::find_matching_entries() new best match: %s", this, (*it)->ofmatch.c_str());
			}
			else if (__wildcard_hits == wildcard_hits)
			{
				nse_list.insert(*it);
				WRITELOG(CNAMESPACE, DBG, "cfsptable(%p)::find_matching_entries() same best match", this);
			}
		}
	}

	if (nse_list.empty())
	{
		throw eFspNoMatch();
	}

	return nse_list;
}


void
cfsptable::flow_mod_allowed(
		cfspentry_owner *fspowner,
		cofmatch const& m) throw (eFspNotAllowed)
{
	if (!fspowner)
		throw eFspNotAllowed();

	// get all cnsentry instances for ofctrl
	std::set<cfspentry*> ctrl_table;

	// hehe, there is no copy_if() yet ... :D
	std::set<cfspentry*>::iterator it;
	for (it = fsp_table.begin(); it != fsp_table.end(); ++it)
	{
		if ((*it)->fspowner == fspowner)
		{
			ctrl_table.insert((*it));
		}
	}
	// FIXME: replace with copy_if(), when C0x is in available in all platforms

	if (ctrl_table.empty())
	{
		throw eFspNotAllowed();
	}

	cfspsearch fspsearch(m, false/*non-strict*/);

	fspsearch = for_each(ctrl_table.begin(), ctrl_table.end(), fspsearch);

	if (fspsearch.fsp_list.empty())
	{
		throw eFspNotAllowed();
	}

#ifndef NDEBUG
	cofmatch match(m);
	WRITELOG(CNAMESPACE, DBG, "cfsptable(%p)::flow_mod_allowed() nse_list.size()=%d %p => %s <= %s",
			this, fspsearch.fsp_list.size(), fspowner, (*fspsearch.fsp_list.begin())->c_str(), match.c_str());
#endif
}


const char*
cfsptable::c_str()
{
	cvastring vas(2048);

	info.assign(vas("cfsptable(%p): \n", this));

	std::set<cfspentry*>::iterator it;
	for (it = fsp_table.begin(); it != fsp_table.end(); ++it)
	{
		info.append("\t");
		info.append((*it)->c_str());
		info.append(std::string("\n"));
	}

	return info.c_str();
}


