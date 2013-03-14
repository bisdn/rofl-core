/*
 * cofdescstats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFDESCSTATS_H_
#define COFDESCSTATS_H_ 1

#include "../cmemory.h"
#include "../../platform/unix/csyslog.h"
#include "openflow.h"

namespace rofl
{

class cofdescstats
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
	cofdescstats(
			uint8_t of_version);

	/**
	 *
	 */
	cofdescstats(
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
	~cofdescstats();


	/**
	 *
	 */
	cofdescstats(
			cofdescstats const& descstats);

	/**
	 *
	 */
	cofdescstats&
	operator= (
			cofdescstats const& descstats);

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
};

}

#endif /* COFDESCSTATS_H_ */
