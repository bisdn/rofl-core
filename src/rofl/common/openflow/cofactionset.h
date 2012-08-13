/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofactionset.h
 *
 *  Created on: Feb 6, 2012
 *      Author: tobi
 */

#ifndef COFACTIONSET_H_
#define COFACTIONSET_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include <map>
#include <string>
#include "../cvastring.h"

#include "../../platform/unix/csyslog.h"

#include "cofaction.h"
#include "cofinst.h"

class cofactionset : public csyslog
{
public:

	/**
	 * constructor
	 */
	cofactionset();

	/**
	 * destructor
	 */
	~cofactionset();


	/** sort action set
	 *
	 */
	std::vector<cofaction>&
	ordered_action_set();

	/** clear the packets action_set
	 *
	 */
	void
	actionset_clear(cofinst& inst);

	/** clear the packets action_set
	 *
	 */
	void
	actionset_clear();

	/** write actions to the packets action_set
	 *
	 */
	void
	actionset_write_actions(cofinst& inst);

	/** write metadata to the packets action_set
	 *
	 */
	void
	actionset_write_metadata(cofinst& inst);

	std::map<int, cofaction> 	acset; 			//!< OF1.1 ActionSet, except OFPAT_SET_FIELD (OF1.2)
	std::map<uint16_t, std::map<uint8_t, cofaction> >	acfields;	//!< OF1.2 list of set fields per instruction
	uint64_t 					metadata;
	uint64_t 					metadata_mask;

	/**
	 * get the action index of action_type
	 */
	static int
	get_action_index(uint16_t acdtion_type);

	/**
	 * return info
	 */
	const char*
	c_str();

protected:

	static void
	init_action_indices();

	static std::map<uint16_t, int>
	action_indices; //!< indexes for priority ordering of actions

private:

	std::string info; // info string
};

#endif /* COFACTIONSET_H_ */
