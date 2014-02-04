/*
 * cofdescstats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFDESCSTATS_H_
#define COFDESCSTATS_H_ 1

#include "rofl/common/cmemory.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"

namespace rofl
{

class cofdesc_stats_reply
{
private: // data structures

	uint8_t 		of_version;
	std::string 	mfr_desc;
	std::string		hw_desc;
	std::string		sw_desc;
	std::string		serial_num;
	std::string		dp_desc;

public: // data structures


public:
	/**
	 *
	 */
	cofdesc_stats_reply(
			uint8_t of_version = 0);

	/**
	 *
	 */
	cofdesc_stats_reply(
			uint8_t of_version,
			std::string const& mfr_desc,
			std::string const& hw_desc,
			std::string const& sw_desc,
			std::string const& serial_num,
			std::string const& dp_desc);

	/**
	 *
	 */
	virtual
	~cofdesc_stats_reply();


	/**
	 *
	 */
	cofdesc_stats_reply(
			cofdesc_stats_reply const& descstats);

	/**
	 *
	 */
	cofdesc_stats_reply&
	operator= (
			cofdesc_stats_reply const& descstats);


	/**
	 *
	 */
	size_t
	length() const;


	/**
	 *
	 */
	void
	set_version(uint8_t of_version);


	/**
	 *
	 */
	uint8_t
	get_version() const;


	/**
	 *
	 */
	std::string
	get_mfr_desc() const;

	/**
	 *
	 */
	std::string
	get_hw_desc() const;

	/**
	 *
	 */
	std::string
	get_sw_desc() const;

	/**
	 *
	 */
	std::string
	get_serial_num() const;

	/**
	 *
	 */
	std::string
	get_dp_desc() const;

	/**
	 *
	 */
	void
	pack(uint8_t *buf, size_t buflen) const;

	/**
	 *
	 */
	void
	unpack(uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofdesc_stats_reply const& desc_stats_reply) {
		os << indent(0) << "<cofdesc_stats_reply >" << std::endl;
		os << indent(2) << "<mfr-desc: " << desc_stats_reply.mfr_desc << " >" << std::endl;
		os << indent(2) << "<hw-desc: " << desc_stats_reply.hw_desc << " >" << std::endl;
		os << indent(2) << "<sw-desc: " << desc_stats_reply.sw_desc << " >" << std::endl;
		os << indent(2) << "<serial-num: " << desc_stats_reply.serial_num << " >" << std::endl;
		os << indent(2) << "<dp-desc: " << desc_stats_reply.dp_desc << " >" << std::endl;
		return os;
	};
};

}

#endif /* COFDESCSTATS_H_ */
