/*
 * cofmsg_table_mod.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_TABLE_MOD_H_
#define COFMSG_TABLE_MOD_H_ 1

#include "cofmsg.h"

namespace rofl
{

/**
 *
 */
class cofmsg_table_mod :
	public cofmsg
{
private:

	union {
		uint8_t*					ofhu_table_mod;
		struct ofp12_table_mod*		ofhu12_table_mod;
		struct ofp13_table_mod*		ofhu13_table_mod;
	} ofhu;

#define ofh_table_mod   ofhu.ofhu_table_mod
#define ofh12_table_mod ofhu.ofhu12_table_mod
#define ofh13_table_mod ofhu.ofhu13_table_mod

public:


	/** constructor
	 *
	 */
	cofmsg_table_mod(
			uint8_t of_version = 0,
			uint32_t xid = 0,
			uint8_t  table_id = 0,
			uint32_t config = 0);


	/**
	 *
	 */
	cofmsg_table_mod(
			cofmsg_table_mod const& table_mod);


	/**
	 *
	 */
	cofmsg_table_mod&
	operator= (
			cofmsg_table_mod const& table_mod);


	/** destructor
	 *
	 */
	virtual
	~cofmsg_table_mod() {};


	/**
	 *
	 */
	cofmsg_table_mod(cmemory *memarea);


	/** reset packet content
	 *
	 */
	virtual void
	reset();


	/**
	 *
	 */
	virtual void
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
	uint8_t
	get_table_id() const;

	/**
	 *
	 */
	void
	set_table_id(uint8_t table_id);

	/**
	 *
	 */
	uint32_t
	get_config() const;

	/**
	 *
	 */
	void
	set_config(uint32_t config);
};

} // end of namespace rofl

#endif /* COFMSG_TABLE_MOD_H_ */
