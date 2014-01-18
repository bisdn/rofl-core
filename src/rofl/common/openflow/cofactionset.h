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

#include <inttypes.h>
#include <map>
#include <string>

#include "rofl/common/cvastring.h"
#include "rofl/platform/unix/csyslog.h"
#include "rofl/common/openflow/cofaction.h"
#include "rofl/common/openflow/cofinstruction.h"

namespace rofl
{

class cofactionset : public csyslog
{
public:

	std::map<int, cofaction> 	acset; 			//!< OF1.1 ActionSet, except OFPAT_SET_FIELD (OF1.2)
	std::map<uint16_t, std::map<uint8_t, cofaction> >	acfields;	//!< OF1.2 list of set fields per instruction
	uint64_t 					metadata;
	uint64_t 					metadata_mask;

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


	/**
	 * get the action index of action_type
	 */
	static int
	get_action_index(uint16_t acdtion_type);

protected:

	static void
	init_action_indices();

	static std::map<uint16_t, int>
	action_indices; //!< indexes for priority ordering of actions

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofactionset const& actionset) {
		os << "<cofactionset ";
		os << "metadata:" << (unsigned long long)actionset.metadata << " ";
		os << "metadata-mask:" << (unsigned long long)actionset.metadata_mask << " ";
		os << "actions:" << std::endl;
		for (std::map<int,cofaction>::const_iterator
				it = actionset.acset.begin(); it != actionset.acset.end(); ++it) {
			os << it->first << ":" << it->second << std::endl;
		}
		os << "set-fields:" << std::endl;
		for (std::map<uint16_t, std::map<uint8_t, cofaction> >::const_iterator
				it = actionset.acfields.begin(); it != actionset.acfields.end(); ++it) {

			uint16_t oxm_class = it->first;
			std::map<uint8_t, cofaction> const& acfields_per_class = it->second;

			os << "oxm-class:" << (int)oxm_class << std::endl;
			for (std::map<uint8_t, cofaction>::const_iterator
					jt = acfields_per_class.begin(); jt != acfields_per_class.end(); ++jt) {

				os << (int)jt->first << ": " << jt->second << std::endl;
			}
		}
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif /* COFACTIONSET_H_ */
