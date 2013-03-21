/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cgttable.h"
// This include must be here due to some exception definitions needed by cgtentry #FIXME: define in another file
#include "cgtentry.h"

using namespace rofl;

cgttable::cgttable() :
	lookup_count(0),
	matched_count(0)
{

}


cgttable::~cgttable()
{
	std::map<uint32_t, cgtentry*>::iterator it;
	for (it = grp_table.begin(); it != grp_table.end(); ++it)
	{
		delete it->second;
	}
	grp_table.clear();
}


cgttable::cgttable(const cgttable &gt)
{
	*this = gt;
}


void
cgttable::reset()
{
	std::map<uint32_t, cgtentry*>::iterator it;
	for (it = grp_table.begin(); it != grp_table.end(); ++it)
	{
		delete (it->second);
	}
	grp_table.clear();
}


cgttable&
cgttable::operator= (const cgttable& gt)
{
	if (this == &gt)
		return *this;

	ciosrv::operator =(gt);

	lookup_count = gt.lookup_count;
	matched_count = gt.matched_count;

	std::map<uint32_t, cgtentry*>::const_iterator it;
	for (it = gt.grp_table.begin(); it != gt.grp_table.end(); ++it)
	{
		grp_table[it->first] = new cgtentry(*((*it).second));
	}

	return *this;
}


cgtentry*
cgttable::operator[] (const uint32_t& grp_id) throw(eBadRequestBadTableId)
{
	if (grp_table.find(grp_id) != grp_table.end())
	{
		return grp_table[grp_id];
	}
	throw eBadRequestBadTableId();
}


cgtentry*
cgttable::update_gt_entry(
		cgtentry_owner *owner,
		cofmsg_group_mod *grp_mod)
{
	switch (grp_mod->get_command()) {
	case OFPGC_ADD: // new group
		return add_gt_entry(owner, grp_mod);
		break;
	case OFPGC_MODIFY: // modify group
		return modify_gt_entry(owner, grp_mod);
		break;
	case OFPGC_DELETE: // delete group
		return rem_gt_entry(owner, grp_mod);
		break;
	default:
		WRITELOG(CGTTABLE, WARN, "cgttable(%p)::update_gt_entry() unknown command %d",
				this, grp_mod->get_command());
		break;
	}

	return (cgtentry*)0; // just to make gcc happy
}



cgtentry*
cgttable::add_gt_entry(
		cgtentry_owner* owner,
		cofmsg_group_mod *grp_mod)
throw (eGroupModExists,
		eGroupModInvalGroup,
		eGroupModLoop,
		eBadActionBadOutPort)
{
	WRITELOG(CGTTABLE, DBG, "cgttable(%p)::add_gt_entry() "
			"", this);


	if (OFPG_MAX < grp_mod->get_group_id())
	{
		throw eGroupModInvalGroup();
	}

	if (grp_table.find(grp_mod->get_group_id()) != grp_table.end())
	{
		throw eGroupModExists();
	}


	grp_table[grp_mod->get_group_id()] = new cgtentry(owner, this, grp_mod);

	WRITELOG(CGTTABLE, DBG, "cgttable(%p)::add_gt_entry() gte:%s", this, grp_table[grp_mod->get_group_id()]->c_str());

	loop_check(grp_table[grp_mod->get_group_id()], grp_mod->get_group_id());

	WRITELOG(CGTTABLE, DBG, "cgttable(%p)::add_gt_entry() "
			"group:%s", this, grp_table[grp_mod->get_group_id()]->c_str());

	return grp_table[grp_mod->get_group_id()];
}


cgtentry*
cgttable::modify_gt_entry(
		cgtentry_owner *owner,
		cofmsg_group_mod *grp_mod)
throw (
		eGroupModInvalGroup,
		eGroupModUnknownGroup,
		eGroupModLoop)
{
	if (OFPG_MAX < grp_mod->get_group_id())
	{
		throw eGroupModInvalGroup();
	}

	if (grp_table.find(grp_mod->get_group_id()) == grp_table.end())
	{
		throw eGroupModUnknownGroup();
	}

	cgtentry *gte = grp_table[grp_mod->get_group_id()];

	uint64_t packet_count 	= gte->packet_count;
	uint64_t byte_count 	= gte->byte_count;
	uint32_t ref_count 		= gte->ref_count;

	delete grp_table[grp_mod->get_group_id()];

	grp_table[grp_mod->get_group_id()] = new cgtentry(owner, this, grp_mod);

	grp_table[grp_mod->get_group_id()]->packet_count 	= packet_count;
	grp_table[grp_mod->get_group_id()]->byte_count		= byte_count;
	grp_table[grp_mod->get_group_id()]->ref_count		= ref_count;

	WRITELOG(CGTTABLE, DBG, "cgttable(%p)::modify_gt_entry() gte:%s",
			this, grp_table[grp_mod->get_group_id()]->c_str());

	loop_check(grp_table[grp_mod->get_group_id()], grp_mod->get_group_id());

	return grp_table[grp_mod->get_group_id()];
}


cgtentry*
cgttable::rem_gt_entry(
		cgtentry_owner *owner,
		cofmsg_group_mod *grp_mod)
throw (
		eGroupModInvalGroup)
{
	if (OFPG_ALL == grp_mod->get_group_id())
	{
		WRITELOG(CGTTABLE, WARN, "cgttable(%p)::rem_gt_entry() "
				"removing all groups", this);

		while (not grp_table.empty())
		{
			std::map<uint32_t, cgtentry*>::iterator it = grp_table.begin();

			delete it->second;
		}

	}
	else
	{
		WRITELOG(CGTTABLE, WARN, "cgttable(%p)::rem_gt_entry() "
				"removing group-id:0x%x", this, grp_mod->get_group_id());

		if (OFPG_MAX < grp_mod->get_group_id())
		{
			throw eGroupModInvalGroup();
		}

		if (grp_table.find(grp_mod->get_group_id()) == grp_table.end())
		{
			//throw eGroupTableNotFound();
			return (cgtentry*)0;
		}

		delete grp_table[grp_mod->get_group_id()];
	}

	return (cgtentry*)0;
}


void
cgttable::get_group_stats(
		cmemory& body)
{
	for (std::map<uint32_t, cgtentry*>::iterator
			it = grp_table.begin(); it != grp_table.end(); ++it)
	{
		cgtentry* gte = it->second;
		gte->get_group_stats(body);
	}
}


void
cgttable::get_group_desc_stats(
		cmemory& body)
{
	for (std::map<uint32_t, cgtentry*>::iterator
			it = grp_table.begin(); it != grp_table.end(); ++it)
	{
		cgtentry* gte = it->second;
		gte->get_group_desc_stats(body);
	}

	WRITELOG(CGTTABLE, DBG, "cgttable(%p)::get_group_desc_stats() body:%s", this, body.c_str());
}



void
cgttable::get_group_features_stats(
		cmemory& body)
{
	cmemory fstats(sizeof(struct ofp_group_features_stats));
	struct ofp_group_features_stats* stats = (struct ofp_group_features_stats*)fstats.somem();

	/*
	 * we do not have support for the various group types yet, so send all 0 back
	 */
	stats->types						= htobe32(0);
	stats->capabilities					= htobe32(0);
	stats->max_groups[OFPGT_ALL]		= htobe32(0);
	stats->max_groups[OFPGT_FF]			= htobe32(0);
	stats->max_groups[OFPGT_INDIRECT]	= htobe32(0);
	stats->max_groups[OFPGT_SELECT]		= htobe32(0);
	stats->actions[OFPGT_ALL]			= htobe32(0);
	stats->actions[OFPGT_FF]			= htobe32(0);
	stats->actions[OFPGT_INDIRECT]		= htobe32(0);
	stats->actions[OFPGT_SELECT]		= htobe32(0);

	body += fstats;
}


void
cgttable::loop_check(
		cgtentry *gte, uint32_t loop_group_id) throw (eGroupModLoop)
{
	if ((cgtentry*)0 == gte)
	{
		return;
	}

	WRITELOG(CGTTABLE, DBG, "cgttable(%p)::loop_check() "
			"loop_group_id:%d gte:%s",
			this, loop_group_id, gte->c_str());

	for (cofbclist::iterator
			it = gte->buckets.begin(); it != gte->buckets.end(); ++it)
	{
		cofbucket& bucket = (*it);

		for (cofaclist::iterator
				at = bucket.actions.begin(); at != bucket.actions.end(); ++at)
		{
			cofaction& action = (*at);

			if (OFPAT_GROUP != action.get_type())
			{
				continue;
			}

			uint32_t group_id = be32toh(action.oac_group->group_id);

			if (group_id == loop_group_id)
			{
				throw eGroupModLoop();
			}
			else
			{
				loop_check(grp_table[group_id], loop_group_id);
			}
		}
	}
}


