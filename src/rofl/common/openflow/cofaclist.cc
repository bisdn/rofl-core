/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cofaclist.h"

using namespace rofl;

cofaclist::cofaclist()
{

}


cofaclist::cofaclist(
		struct ofp_action_header *achdr,
		size_t aclen)
{
	unpack(achdr, aclen);
}


cofaclist::~cofaclist()
{

}


std::vector<cofaction>*
cofaclist::find_action(enum ofp_action_type type,
		std::vector<cofaction> *result) throw (eAcListNotFound)
{
	result->clear();

	// ah, C++ 0x with copy_if(), let us use it ...
	cofaclist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		cofaction& action = (*it);

		if (action.oac_header->type == type)
		{
			result->push_back(action);
		}
	}

	if (result->empty())
	{
		throw eAcListNotFound();
	}

	return result;
}



std::vector<cofaction>&
cofaclist::unpack(
		struct ofp_action_header *achdr,
		size_t aclen)
throw (eBadActionBadLen, eBadActionBadOutPort)
{
	clear(); // clears elems

	WRITELOG(COFACTION, DBG, "cofaclist(%p)::unpack() aclen:%d", this, aclen);

	// sanity check: aclen must be of size at least of ofp_action_header
	if (aclen < (int)sizeof(struct ofp_action_header))
		return elems;


	while (aclen > 0)
	{
		if (be16toh(achdr->len) < sizeof(struct ofp_action_header))
			throw eBadActionBadLen();

		next() = cofaction(achdr, be16toh(achdr->len) );

		WRITELOG(COFACTION, DBG, "cofaclist(%p)::unpack() new action: %s", this, back().c_str());

		aclen -= be16toh(achdr->len);
		achdr = (struct ofp_action_header*)(((uint8_t*)achdr) + be16toh(achdr->len));
	}

	return elems;
}


struct ofp_action_header*
cofaclist::pack(
	struct ofp_action_header *achdr,
	size_t aclen)
throw (eAcListInval)
{
	if (aclen < length())
		throw eAcListInval();

	cofaclist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		achdr = (struct ofp_action_header*)
				((uint8_t*)((*it).pack(achdr, (*it).length())) + (*it).length());
	}
	return achdr;
}


size_t
cofaclist::length() const
{
	size_t len = 0;
	cofaclist::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		len += (*it).length();
	}
	return len;
}


cofaction&
cofaclist::find_action(enum ofp_action_type type) throw (eAcListNotFound)
{
	cofaclist::iterator it;
	if ((it = find_if(elems.begin(), elems.end(),
			cofaction_find_type((uint16_t)type))) == elems.end())
	{
		throw eAcListNotFound();
	}
	return ((*it));
}


int
cofaclist::count_action_type(
		uint16_t type)
{
	int action_cnt = count_if(elems.begin(), elems.end(), cofaction_find_type(type));

	WRITELOG(COFINST, DBG, "cofinst::actions_count_action_type(): %d", (action_cnt));

	return (action_cnt);
}


int
cofaclist::count_action_output(
		uint32_t port_no) const
{
	int action_cnt = 0;

	for (cofaclist::const_iterator
			it = elems.begin(); it != elems.end(); ++it)
	{
		cofaction action(*it);

		if (OFPAT_OUTPUT != action.get_type())
		{
			continue;
		}

		uint32_t out_port = be32toh(action.oac_output->port);

		if ((0 == port_no) || (out_port == port_no))
		{
			action_cnt++;
		}
	}

	return action_cnt;
}


std::list<uint32_t>
cofaclist::actions_output_ports()
{
	std::list<uint32_t> outports;

	for (cofaclist::iterator
			it = elems.begin(); it != elems.end(); ++it)
	{
		if ((*it).get_type() != OFPAT_OUTPUT)
		{
			continue;
		}
		outports.push_back(be32toh((*it).oac_output->port));
	}
	return outports;
}


const char*
cofaclist::c_str()
{
	cvastring vas(4096);
	info.assign(vas("cofaclist(%p) %d action(s):", this, elems.size()));
	cofaclist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		info.append(vas("\n  %s", (*it).c_str()));
	}
	return info.c_str();
}

