/*
 * cofmsg_table_stats.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_TABLE_FEATURES_H_
#define COFMSG_TABLE_FEATURES_H_ 1

#include <map>

#include "cofmsg_stats.h"
#include "rofl/common/openflow/coftables.h"

namespace rofl {

/**
 *
 */
class cofmsg_table_features :
	public cofmsg_stats
{
protected:

	rofl::openflow::coftables		tables;

	union {
		uint8_t*											ofhu_generic;
		struct rofl::openflow13::ofp_table_features*		ofhu_table_features;
	} ofh_ofhu;

#define ofh_generic   			ofh_ofhu.ofhu_generic
#define ofh_table_features 		ofh_ofhu.ofhu_table_features

public:


	/** constructor
	 *
	 */
	cofmsg_table_features(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t stats_flags = 0);


	/**
	 *
	 */
	cofmsg_table_features(
			cofmsg_table_features const& msg);


	/**
	 *
	 */
	cofmsg_table_features&
	operator= (
			cofmsg_table_features const& msg);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_table_features();


	/**
	 *
	 */
	cofmsg_table_features(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual uint8_t*
	resize(size_t len);


	/** returns length of packet in packed state
	 *
	 */
	virtual size_t
	length() const;


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf = (uint8_t*)0, size_t buflen = 0);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


	/** parse packet and validate it
	 */
	virtual void
	validate();

public:

	/**
	 *
	 */
	rofl::openflow::coftables const&
	get_tables() const { return tables; };

	/**
	 *
	 */
	rofl::openflow::coftables&
	set_tables() { return tables; };


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_table_features const& msg) {
		os << indent(0) << "<cofmsg_table_features >" << std::endl;
		indent i(2);
		os << msg.tables;
		return os;
	};
};



/**
 *
 */
class cofmsg_table_features_request :
	public cofmsg_table_features
{
public:


	/** constructor
	 *
	 */
	cofmsg_table_features_request(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t flags = 0) :
				cofmsg_table_features(of_version, xid, flags)
	{
		switch (of_version) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {

		} break;
		case rofl::openflow13::OFP_VERSION: {
			set_type(rofl::openflow13::OFPT_MULTIPART_REQUEST);
			set_stats_type(rofl::openflow13::OFPMP_TABLE_FEATURES);
		} break;
		default:
			throw eBadVersion();
		}
	};


	/**
	 *
	 */
	cofmsg_table_features_request(
			cofmsg_table_features_request const& request)
	{
		*this = request;
	};


	/**
	 *
	 */
	cofmsg_table_features_request&
	operator= (
			cofmsg_table_features_request const& request)
	{
		if (this == &request)
			return *this;

		cofmsg_table_features::operator= (request);

		return *this;
	};


	/** destructor
	 *
	 */
	virtual
	~cofmsg_table_features_request() {};


	/**
	 *
	 */
	cofmsg_table_features_request(cmemory *memarea) :
		cofmsg_table_features(memarea) {};


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_table_features_request const& msg) {
		os << dynamic_cast<cofmsg_stats const&>( msg );
		os << indent(4) << "<cofmsg_table_features_request >" << std::endl;
		indent j(6);
		os << dynamic_cast<cofmsg_table_features const&>( msg );
		return os;
	};
};




/**
 *
 */
class cofmsg_table_features_reply :
	public cofmsg_table_features
{
public:


	/** constructor
	 *
	 */
	cofmsg_table_features_reply(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN,
			uint32_t xid = 0,
			uint16_t flags = 0) :
				cofmsg_table_features(of_version, xid, flags)
	{
		switch (of_version) {
		case rofl::openflow::OFP_VERSION_UNKNOWN: {

		} break;
		case rofl::openflow13::OFP_VERSION: {
			set_type(rofl::openflow13::OFPT_MULTIPART_REPLY);
			set_stats_type(rofl::openflow13::OFPMP_TABLE_FEATURES);
		} break;
		default:
			throw eBadVersion();
		}
	};


	/**
	 *
	 */
	cofmsg_table_features_reply(
			cofmsg_table_features_reply const& reply)
	{
		*this = reply;
	};


	/**
	 *
	 */
	cofmsg_table_features_reply&
	operator= (
			cofmsg_table_features_reply const& reply)
	{
		if (this == &reply)
			return *this;

		cofmsg_table_features::operator= (reply);

		return *this;
	};


	/** destructor
	 *
	 */
	virtual
	~cofmsg_table_features_reply() {};


	/**
	 *
	 */
	cofmsg_table_features_reply(cmemory *memarea) :
		cofmsg_table_features(memarea) {};


public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_table_features_reply const& msg) {
		os << dynamic_cast<cofmsg_stats const&>( msg );
		os << indent(4) << "<cofmsg_table_features_reply >" << std::endl;
		indent j(6);
		os << dynamic_cast<cofmsg_table_features const&>( msg );
		return os;
	};
};



} // end of namespace rofl

#endif /* COFMSG_TABLE_STATS_H_ */
