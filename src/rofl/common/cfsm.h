/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFSM_H
#define CFSM_H 1

#include <string>
#include <stdio.h>
#include <strings.h>

namespace rofl
{

/** interface for finite state machines
 */
class cfsm
{
private:

#define FSM_STATE_HISTORY_MAX_SIZE		16	// store at most 16 past states

	std::string		info;

public:
	/** constructor
	 */
	cfsm();
	/** destructor
	*/
	virtual ~cfsm();

	/** return current state
	 */
	int cur_state() const;

protected:

	/** set initial state
	 */
	void init_state(int state);

	/** enter new state
	 */
	int new_state(int state);

protected:

	/** dump internal state
	 */
	const char* c_str();

	int fsm_state; // current state
};

}; // end of namespace

#endif
