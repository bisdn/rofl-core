/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CRIB_H
#define CRIB_H 1

#include <string>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif


#include "rofl/common/cerror.h"
#include "rofl/common/ciosrv.h"
#include "rofl/experimental/cgraph.h"
#include "rofl/common/cvastring.h"
#include "rofl/platform/unix/crandom.h"

namespace rofl {

class eSpfBase : public eGraphBase {};
class eSpfNotFound : public eSpfBase {};
class eSpfUnReachable : public eSpfBase {};


class crib : public ciosrv {
public: // static methods

	static void test();

public: // data structures

	cgraph<uint64_t, int> graph; // graph with all link weights
	cgraph<uint64_t, int> distances; // calculated distances to all vertices
	cgraph<uint64_t, int> predecessors; // preceding vertex => predecessors[0][2] = 5 means: from 0 to 2 next hop is 5
	cgraph<uint64_t, uint64_t> rtable; // routing table

public: // methods

	/** constructor
	 *
	 */
	crib();

	/** copy constructor
	 *
	 */
	crib(
			crib const& rib);

	/** destructor
	 *
	 */
	virtual
	~crib();

	/**
	 *
	 */
	crib&
	operator= (crib const& r);

	/**
	 *
	 */
	const char*
	c_str();

public: // methods

	/** return next hop for vertex "end" starting at vertex "start"
	 *
	 */
	uint64_t
	next_hop(uint64_t start, uint64_t end) throw (eSpfUnReachable);

	/** add edge
	 *
	 */
	void
	add_edge(uint64_t from, uint64_t to, int weight = 1, bool bidirectional = true);

	/** remove edge
	 *
	 */
	void
	rem_edge(uint64_t from, uint64_t to, bool bidirectional = true);

	/** calculate shortest path based on dijkstra's algorithm
	 *
	 */
	void
	dijkstra();

public: // overloaded from ciosrv

	/**
	 *
	 */
	void
	handle_timeout(int opaque);



private: // data structures

	enum cspf_timer_t {
		TIMER_GRAPH_CHANGED = 1,
	};

	std::string info;

};

}  // namespace rofl

#endif
