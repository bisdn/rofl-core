/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFACLIST_H
#define COFACLIST_H 1

#include <vector>
#include <algorithm>

#include "../cerror.h"
#include "../coflist.h"
#include "../cvastring.h"

#include "cofaction.h"

class eAcListBase : public cerror {}; // base error class for cofaclist
class eAcListInval : public eAcListBase {}; // parameter is invalid
class eAcListNotFound : public eAcListBase {}; // element not found
class eAcListOutOfRange : public eAcListBase {}; // index out of range in operator[]


class cofaclist : public coflist<cofaction> {
public: // static structures and methods



public: // methods

	/** constructor
	 */
	cofaclist();

	/** constructor
	 */
	cofaclist(
			struct ofp_action_header *achdr,
			size_t aclen);

	/**
	 */
	cofaclist(cofaclist const& aclist)
	{
		*this = aclist;
	};

	/**
	 */
	cofaclist& operator= (cofaclist const& aclist)
	{
		if (this == &aclist)
			return *this;
		coflist<cofaction>::operator= (aclist);
		return *this;
	};

	/** destructor
	 */
	virtual
	~cofaclist();

	/** get list of cofaction instances of a specific type
	 *  result must be defined by the calling function and its content will be overwritten
	 */
	std::vector<cofaction>*
	find_action(enum ofp_action_type type,
			std::vector<cofaction> *result) throw (eAcListNotFound);

	cofaction&
	find_action(enum ofp_action_type type) throw (eAcListNotFound);


	/** create a std::list<cofaction*> from a struct ofp_flow_mod
	 */
	std::vector<cofaction>&
	unpack(
			struct ofp_action_header *actions,
			size_t aclen)
		throw (eActionBadLen, eActionBadOutPort);

	/** builds an array of struct ofp_instructions
	 * from a std::vector<cofinst*>
	 */
	struct ofp_action_header*
	pack(
			struct ofp_action_header *actions,
			size_t aclen)
		throw (eAcListInval);

	/** returns required length for array of struct ofp_action_headers
	 * for all actions defined in std::vector<cofaction*>
	 */
	size_t
	length();

	/** counts number of actions output in ActionList acvec of this instruction
	 */
	int
	actions_count_output();

	/** dump info string
	 */
	const char*
	c_str();



private:

	std::string info; // info string
};

#endif
