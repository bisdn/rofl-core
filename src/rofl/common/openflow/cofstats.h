/*
 * cofstats.h
 *
 *  Created on: 11.10.2012
 *      Author: andreas
 */

#ifndef COFSTATS_H
#define COFSTATS_H 1

#include <rofl/common/cerror.h>
#include <rofl/common/cmemory.h>
#include <rofl/common/cvastring.h>
#include <rofl/common/openflow/cofmatch.h>


class eOFstatsBase			: public cerror {};
class eOFstatsTooShort		: public eOFstatsBase {};


class cofstats
{
/*
 * data structures
 */
private:

	std::string		info;

protected:

	cmemory			packed; // packed cofstats body

public:

	cmemory			body;	// fixed part of OpenFlow statistics body
	cofmatch		match;	// match for flow/aggregate statistics requests


	union {
		uint8_t									*ofsu_body;
		/* OFPST_DESC */
		// empty desc stats body in request
		struct ofp_desc_stats					*ofsu_desc_reply;

		/* OFPST_FLOW */
		struct ofp_flow_stats_request			*ofsu_flow_stats_request;
		struct ofp_flow_stats					*ofsu_flow_stats_reply;

		/* OFPST_AGGREGATE */
		struct ofp_aggregate_stats_request		*ofsu_aggr_stats_request;
		struct ofp_aggregate_stats_reply		*ofsu_aggr_stats_reply;

		/* OFPST_TABLE */
		// empty table stats body in request
		struct ofp_table_stats					*ofsu_table_stats_reply;

		/* OFPST_PORT */
		struct ofp_port_stats_request			*ofsu_port_stats_request;
		struct ofp_port_stats					*ofsu_port_stats_reply;

		/* OFPST_QUEUE */
		struct ofp_queue_stats_request			*ofsu_queue_stats_request;
		struct ofp_queue_stats					*ofsu_queue_stats_reply;

		/* OFPST_GROUP */
		struct ofp_group_stats_request			*ofsu_group_stats_request;
		struct ofp_group_stats					*ofsu_group_stats_reply;

		/* OFPST_GROUP_DESC */
		// empty group desc stats body in request
		struct ofp_group_desc_stats				*ofsu_group_desc_stats_reply;

		/* OFPST_GROUP_FEATURES */
		// empty group features stats body in request
		struct ofp_group_features_stats			*ofsu_group_features_stats_reply;

		/* OFPST_EXPERIMENTER */
		struct ofp_experimenter_stats_header	*ofsu_expr_stats_header;
	} ofs_ofsu;

#define ofs_body								ofs_ofsu.ofsu_body

// empty
#define ofs_desc_reply							ofs_ofsu.ofsu_desc_reply
#define ofs_flow_stats_request					ofs_ofsu.ofsu_flow_stats_request
#define ofs_flow_stats							ofs_ofsu.ofsu_flow_stats_reply
#define ofs_aggr_stats_request					ofs_ofsu.ofsu_aggr_stats_request
#define ofs_aggr_stats							ofs_ofsu.ofsu_aggr_stats_reply
// empty
#define ofs_table_stats							ofs_ofsu.ofsu_table_stats_reply
#define ofs_port_stats_request					ofs_ofsu.ofsu_port_stats_request
#define ofs_port_stats							ofs_ofsu.ofsu_port_stats_reply
#define ofs_queue_stats_request					ofs_ofsu.ofsu_queue_stats_request
#define ofs_queue_stats							ofs_ofsu.ofsu_queue_stats_reply
#define ofs_group_stats_request					ofs_ofsu.ofsu_group_stats_request
#define ofs_group_stats							ofs_ofsu.ofsu_group_stats_reply
// empty
#define ofs_group_desc_stats					ofs_ofsu.ofsu_group_desc_stats_reply
// empty
#define ofs_group_features_stats				ofs_ofsu.ofsu_group_features_stats_reply
// empty
#define ofs_experimenter_stats					ofs_ofsu.ofsu_expr_stats_header


#define OFS_FLOW_STATS_STATIC_HDR_LEN			32	// without struct ofp_match
#define OFS_AGGR_STATS_STATIC_HDR_LEN			32	// without struct ofp_match



/*
 * methods
 */
public:

	/**
	 *
	 */
	cofstats(
			size_t bodylen = 0);

	/**
	 *
	 */
	virtual
	~cofstats();

	/**
	 *
	 */
	virtual size_t
	length();

	/**
	 *
	 */
	virtual cmemory&
	pack();

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf,
			size_t buflen)
				throw (eOFstatsTooShort);

	/**
	 *
	 */
	virtual const char*
	c_str();
};



class cofstats_flow_request :
	public cofstats
{
public:
		/** constructor
		 *
		 */
		cofstats_flow_request() :
			cofstats(OFS_FLOW_STATS_STATIC_HDR_LEN)
		{
			ofs_flow_stats_request = (struct ofp_flow_stats_request*)body.somem();
		};
		/** destructor
		 *
		 */
		virtual
		~cofstats_flow_request() {};
		/**
		 *
		 */
		virtual size_t
		length()
		{
			return (body.memlen() + match.length());
		};
		/**
		 *
		 */
		virtual cmemory&
		pack()
		{
			packed.resize(length());
			memcpy(packed.somem(), body.somem(), body.memlen());
			match.pack((struct ofp_match*)(packed.somem() + body.memlen()), match.length());
			return packed;
		};
};



class cofstats_flow_reply :
	public cofstats
{
public:
		/** constructor
		 *
		 */
		cofstats_flow_reply() :
			cofstats(sizeof(struct ofp_flow_stats))
		{
			ofs_flow_stats = (struct ofp_flow_stats*)body.somem();
		};
		/** destructor
		 *
		 */
		virtual
		~cofstats_flow_reply() {};
};




class cofstats_aggregate_request :
	public cofstats
{
private:
		std::string		info;

public:
		/** constructor
		 *
		 */
		cofstats_aggregate_request() :
			cofstats(OFS_AGGR_STATS_STATIC_HDR_LEN)
		{
			ofs_aggr_stats_request = (struct ofp_aggregate_stats_request*)body.somem();
		};
		cofstats_aggregate_request(uint8_t* buf, size_t buflen) :
			cofstats(OFS_AGGR_STATS_STATIC_HDR_LEN)
		{
			unpack(buf, buflen);
		};
		/** destructor
		 *
		 */
		virtual
		~cofstats_aggregate_request() {};
		/**
		 *
		 */
		virtual size_t
		length()
		{
			return (body.memlen() + match.length());
		};
		/**
		 *
		 */
		virtual cmemory&
		pack()
		{
			packed.resize(length());
			memcpy(packed.somem(), body.somem(), body.memlen());
			match.pack((struct ofp_match*)(packed.somem() + body.memlen()), match.length());
			return packed;
		};
		/**
		 *
		 */
		virtual void
		unpack(uint8_t *buf, size_t buflen) throw (eOFstatsTooShort)
		{
			if (buflen < sizeof(struct ofp_aggregate_stats_request))
			{
				throw eOFstatsTooShort();
			}
			body.assign(buf, OFS_AGGR_STATS_STATIC_HDR_LEN);
			ofs_aggr_stats_request = (struct ofp_aggregate_stats_request*)body.somem();
			match.reset();
			match.unpack((struct ofp_match*)(buf + OFS_AGGR_STATS_STATIC_HDR_LEN),
					buflen - OFS_AGGR_STATS_STATIC_HDR_LEN);
		};
		/**
		 *
		 */
		virtual const char*
		c_str()
		{
			cvastring vas(256);
			info.assign(vas("cofstats_aggregate_request(%p) table-id: %d out_port: 0x%x out_group: 0x%x "
					"cookie: 0x%llx cookie-mask: 0x%llx ",
					this,
					ofs_aggr_stats_request->table_id,
					be32toh(ofs_aggr_stats_request->out_port),
					be32toh(ofs_aggr_stats_request->out_group),
					be64toh(ofs_aggr_stats_request->cookie),
					be64toh(ofs_aggr_stats_request->cookie_mask)));
			info.append(match.c_str());
			return info.c_str();
		};
};



class cofstats_aggregate_reply :
	public cofstats
{
public:
		/** constructor
		 *
		 */
		cofstats_aggregate_reply() :
			cofstats(sizeof(struct ofp_aggregate_stats_reply))
		{
			ofs_aggr_stats = (struct ofp_aggregate_stats_reply*)body.somem();
		};
		/** destructor
		 *
		 */
		virtual
		~cofstats_aggregate_reply() {};
};


#endif /* COFSTATS_H */
