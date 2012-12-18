/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CGRAPH_H
#define CGRAPH_H 1

#include <map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include "rofl/common/cvastring.h"
#include "rofl/common/cerror.h"
#include "rofl/common/cmemory.h"
#include "rofl/common/ciosrv.h"

namespace rofl {

class eGraphBase : public cerror {};
class eGraphOutOfRange : public eGraphBase {};
class eGraphUnreachable : public eGraphBase {};
class eGraphNotFound : public eGraphBase {};

template <class T, class U>
class cgraph : public ciosrv
{
public: // data structures

	std::map<T, int> vertices; // mapping of node ids (64bit number dpid from openflow) to graph int value

public: // methods

	/** constructor
	 *
	 */
	cgraph();

	/** constructor
	 *
	 */
	cgraph(
			cgraph const& graph);

	/** destructor
	 *
	 */
	virtual
	~cgraph();

	/** assignment operator
	 *
	 */
	cgraph&
	operator= (cgraph const& g);

	/** comparison operator
	 *
	 */
	bool
	operator== (cgraph const& g);

	/**
	 *
	 */
	U&
	link(
			int i,
			int j) throw (eGraphOutOfRange);

	/** return info
	 *
	 */
	const char*
	c_str();

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	void
	reset(U u = 0);

	/**
	 *
	 */
	void
	resize(size_t d);

public: // methods

	/** add edge
	 *
	 */
	void
	add_edge(
			T from,
			T to,
			U weight = 1,
			bool bidirectional = true);

	/** remove edge
	 *
	 */
	void
	rem_edge(
			T from,
			T to,
			bool bidirectional = true);

	/** edge
	 *
	 */
	U&
	edge(
			T from,
			T to);

public: // methods

	/** maps a uint64_t dpid to our internal graph int value
	 *
	 * creates a new mapping for vtxids not seen so far, so this method
	 * never fails
	 * increases graph's dimension if necessary (TODO)
	 */
	int
	vertex_map(
			T vtxid);

	/** checks for existence of a specific vertex mit dpid
	 *
	 */
	int
	vertex_exists(
			T vtxid) throw (eGraphNotFound);


	/** maps an internal graph int value back to a mapped value T
	 *
	 */
	T
	vertex_unmap(
			U i) throw (eGraphNotFound);


protected: // data structures



private: // data structures

	std::string info;	// info string

	std::vector< std::vector<U*>* > g;

private: // auxiliary classes

	template <class V>
	class vertex_graph_id :
		public std::unary_function<std::pair<V, int>, bool>
	{
		int graph_id;
	public:
		vertex_graph_id(int __graph_id) : graph_id(__graph_id) { };
		bool operator() (std::pair<V, int> const& p) {
			return (p.second == graph_id);
		};
	};
};

}  // namespace rofl

#endif
