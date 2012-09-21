/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cgtentry.h"
//#include "cgttable.h" //FIXME: This should NOT be here

cgtentry::cgtentry(cgtentry_owner *owner) :
	owner(owner),
	grp_table(NULL),
	group_id(0),
	group_type(0),
	ref_count(0),
	packet_count(0),
	byte_count(0)
{

}


cgtentry::cgtentry(
		cgtentry_owner *owner,
		cgttable *_grp_table,
		struct ofp_group_mod *grp_mod) throw (eGroupEntryInval, eGroupEntryBadType, eActionBadOutPort) :
		owner(owner),
		grp_table(_grp_table),
		ref_count(0),
		packet_count(0),
		byte_count(0)
{
	this->group_id = be32toh(grp_mod->group_id);
	this->group_type = grp_mod->type;
	if (be16toh(grp_mod->header.length) > sizeof(struct ofp_group_mod))
	{
		size_t bclen = be16toh(grp_mod->header.length) - sizeof(struct ofp_group_mod);
		buckets.unpack(grp_mod->buckets, bclen);
	}

	switch (group_type) {
	case OFPGT_ALL:
		{
			for (cofbclist::iterator
					it = buckets.begin(); it != buckets.end(); ++it)
			{
				cofbucket& bucket = (*it);
				if (bucket.weight != 0)
				{
					throw eGroupEntryInval();
				}
			}
		}
		break;
	case OFPGT_FF:
		break;
	case OFPGT_INDIRECT:
		{
			if (buckets.size() > 1)
			{
				throw eGroupEntryInval();
			}
			cofbucket& bucket = buckets.front();
			if (bucket.weight != 0)
			{
				throw eGroupEntryInval();
			}
		}
		break;
	case OFPGT_SELECT:
		{
			for (cofbclist::iterator
					it = buckets.begin(); it != buckets.end(); ++it)
			{
				cofbucket& bucket = (*it);
				if (bucket.weight == 0)
				{
					throw eGroupEntryInval();
				}
			}
		}
		break;
	default:
		throw eGroupEntryBadType();
	}
}


cgtentry::~cgtentry()
{
	if (NULL != grp_table)
	{
		grp_table->grp_table.erase(this->group_id);
	}
}


void
cgtentry::get_group_stats(
		cmemory& body)
{
	cmemory gstats(OFP_GROUP_STATS_REPLY_STATIC_BODY_LEN); // 32 bytes in OF1.2
	struct ofp_group_stats* group_stats = (struct ofp_group_stats*)gstats.somem();

	group_stats->length			= htobe16(OFP_GROUP_STATS_REPLY_STATIC_BODY_LEN +
								 	 buckets.size() * sizeof(struct ofp_bucket_counter));
	group_stats->group_id 		= htobe32(group_id);
	group_stats->ref_count		= htobe32(ref_count);
	group_stats->packet_count	= htobe64(packet_count);
	group_stats->byte_count		= htobe64(byte_count);

	for (cofbclist::iterator
			it = buckets.begin(); it != buckets.end(); ++it)
	{
		cofbucket& bucket = (*it);
		bucket.get_bucket_stats(body);
	}

	body += gstats;
}


void
cgtentry::get_group_desc_stats(
		cmemory& body)
{
	cmemory gstats(OFP_GROUP_DESC_STATS_REPLY_STATIC_BODY_LEN + buckets.length()); // 8 bytes in OF1.2
	struct ofp_group_desc_stats* group_desc_stats =
						(struct ofp_group_desc_stats*)gstats.somem();

	group_desc_stats->length	= htobe16(OFP_GROUP_DESC_STATS_REPLY_STATIC_BODY_LEN +
								 	 buckets.length());
	group_desc_stats->type 		= htobe32(group_type);
	group_desc_stats->group_id	= htobe32(group_id);

	buckets.pack(group_desc_stats->buckets, buckets.length());

	WRITELOG(CGTENTRY, DBG, "cgtentry(%p)::get_group_desc_stats() %s", this, c_str());

	WRITELOG(CGTENTRY, DBG, "cgtentry(%p)::get_group_desc_stats() body:%s", this, gstats.c_str());

	body += gstats;
}


const char*
cgtentry::c_str()
{
	cvastring vas(4096);

	std::string s_type;

	switch (group_type) {
	case OFPGT_ALL:
		s_type.assign("OFPGT_ALL");
		break;
	case OFPGT_FF:
		s_type.assign("OFPGT_FF");
		break;
	case OFPGT_INDIRECT:
		s_type.assign("OFPGT_INDIRECT");
		break;
	case OFPGT_SELECT:
		s_type.assign("OFPGT_SELECT");
		break;
	default:
		break;
	}

	info.assign(vas("cgtentry(%p): group-id[%d] group-type[%s] packetcount[%d] bytecount[%d] refcount[%d] \n  %s",
			this, group_id, s_type.c_str(), packet_count, byte_count, ref_count, buckets.c_str()));

	return info.c_str();
}

