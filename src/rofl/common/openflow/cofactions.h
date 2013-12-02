/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFACLIST_H
#define COFACLIST_H 1

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include <vector>
#include <algorithm>

#include "../cerror.h"
#include "../cvastring.h"

#include "../coflist.h"
#include "openflow.h"
#include "cofaction.h"

namespace rofl
{

class eAcListBase 		: public cerror {}; // base error class for cofactions
class eAcListInval 		: public eAcListBase {}; // parameter is invalid
class eAcListNotFound 	: public eAcListBase {}; // element not found
class eAcListOutOfRange : public eAcListBase {}; // index out of range in operator[]


class cofactions : public coflist<cofaction>
{
public:

	uint8_t 		ofp_version;

	/** constructor
	 */
	cofactions(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN);


	/** constructor
	 */
	cofactions(
			uint8_t ofp_version,
			struct openflow::ofp_action_header *achdr,
			size_t aclen);


	/**
	 */
	cofactions(
			cofactions const& aclist);


	/**
	 */
	cofactions&
	operator= (
			cofactions const& aclist);


	/** destructor
	 */
	virtual
	~cofactions();



	/** get list of cofaction instances of a specific type
	 *  result must be defined by the calling function and its content will be overwritten
	 */
	std::vector<cofaction>*
	find_action(uint8_t type,
			std::vector<cofaction> *result);

	cofaction&
	find_action(uint8_t type);


	/** create a std::list<cofaction*> from a struct ofp_flow_mod
	 */
	std::vector<cofaction>&
	unpack(
			struct openflow::ofp_action_header *actions,
			size_t aclen);

	/** builds an array of struct ofp_instructions
	 * from a std::vector<cofinst*>
	 */
	struct openflow::ofp_action_header*
	pack(
			struct openflow::ofp_action_header *actions,
			size_t aclen)
		const;

	/** returns required length for array of struct ofp_action_headers
	 * for all actions defined in std::vector<cofaction*>
	 */
	size_t
	length() const;

	/** counts number of actions of specific type in ActionList acvec of this instruction
	 */
	int
	count_action_type(
			uint16_t type);


	/** counts number of actions output in ActionList acvec of this instruction
	 */
	int
	count_action_output(
			uint32_t port_no = 0) const;



	/** returns a list of port_no values for each ActionOutput instances
	 *
	 */
	std::list<uint32_t>
	actions_output_ports();



private:

	std::string info; // info string

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofactions const& actions) {
		os << "<cofactions ";
			os << "ofp-version:" << (int)actions.ofp_version << " ";
			os << dynamic_cast<coflist const&>( actions ) << " ";
		os << ">";
		return os;
	};
};

}; // end of namespace

#endif
