/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CFSM_H
#define CFSM_H 1

#include <list>

#ifndef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <strings.h>
#ifndef __cplusplus
}
#endif


/** interface for finite state machines
 */
class cfsm {
public:
	/** constructor
	 */
	cfsm();
	/** destructor
	*/
	virtual ~cfsm();

	/** return current state
	 */
	int cur_state();

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

	std::list<int> fsm_state_history; // history of states

	int fsm_state; // current state
};

#endif
