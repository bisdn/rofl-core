/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "crib.h"

using namespace rofl;

crib::crib()
{

}


crib::crib(crib const& rib)
{
	*this = rib;
}


crib::~crib()
{

}


crib&
crib::operator= (crib const& r)
{
	if (this == &r)
		return *this;

	graph = r.graph;
	distances = r.distances;
	predecessors = r.predecessors;
	rtable = r.rtable;

	return *this;
}


const char*
crib::c_str()
{
	cvastring vas(1024);

	info.assign(vas("crib(%p): =>\n", this));

	info.append(vas("graph =>\n%s", graph.c_str()));
	info.append(vas("distances =>\n%s", distances.c_str()));
	info.append(vas("predecessors =>\n%s", predecessors.c_str()));
	info.append(vas("rtable =>\n%s", rtable.c_str()));

	return info.c_str();
}


void
crib::handle_timeout(int opaque)
{
	switch (opaque) {
	case TIMER_GRAPH_CHANGED:

		break;
	}
}



uint64_t
crib::next_hop(
		uint64_t start,
		uint64_t end) throw (eSpfUnReachable)
{
	try {
		WRITELOG(CRIB, DBG, "crib(%p)::next_hop() from:%llx to:%llx", this, start, end);
		return rtable.edge(start, end);

		//return vertex_unmap(rtable.link(vertex_exists(start), vertex_exists(end)));

	} catch (eGraphNotFound& e) {

		throw eSpfUnReachable();

	}

	return 0;
}


void
crib::add_edge(
		uint64_t from,
		uint64_t to,
		int weight,
		bool bidirectional)
{
	graph.add_edge(from, to, weight, bidirectional);
}


void
crib::rem_edge(
		uint64_t from,
		uint64_t to,
		bool bidirectional)
{
	graph.rem_edge(from, to, bidirectional);
}


void
crib::dijkstra()
{
	int dim = (int)graph.vertices.size();

	// for all vertices
	for (int i = 0; i < dim; ++i)
	{
		/*
		 *  initialization dijkstra
		 */

		// set of all nodes to check
		std::set<int> Qset;

		for (int j = 0; j < dim; ++j)
		{
			if (i == j)
			{
				distances.link(i, j) = 0;
				predecessors.link(i, j) = i; // i is its own predecessor
			}
			else
			{
				distances.link(i, j) = -1;
				predecessors.link(i, j) = -1; // no predecessor yet
				Qset.insert(j);
			}
		}

#if 0
		WRITELOG(CROUTE, DBG, "crib(%p)::dijkstra() =>\n", this);
		WRITELOG(CROUTE, DBG, "graph => %s\n", graph.c_str());
		WRITELOG(CROUTE, DBG, "distances => %s\n", distances.c_str());
		WRITELOG(CROUTE, DBG, "predecessors => %s\n", predecessors.c_str());
#endif

#if 0
		fprintf(stderr, "crib(%p)::dijkstra() =>\n", this);
		fprintf(stderr, "graph => %s\n", graph.c_str());
		fprintf(stderr, "distances => %s\n", distances.c_str());
		fprintf(stderr, "predecessors => %s\n", predecessors.c_str());
#endif

		/*
		 *  initialization dijkstra end
		 */

		//fprintf(stderr, "i=%d =================================================\n", i);

		/*
		 * dijsktra itself
		 */

		std::set<int> Uset; // set of nodes with already determined minimal distance
		Uset.insert(i); // initial: only starting node 'i' in Iset

		// for all nodes in Qset
		while (not Qset.empty())
		{
			std::set<int>::iterator ut, qt;

			int dist = -1;
			int u = -1; // next node for Uset
			int p = -1; // predecessor

#if 0
			fprintf(stderr, "Uset: ");
			for (ut = Uset.begin(); ut != Uset.end(); ++ut)
			{
				fprintf(stderr, "%d ", *ut);
			}
			fprintf(stderr, "\n");

			fprintf(stderr, "Qset: ");
			for (qt = Qset.begin(); qt != Qset.end(); ++qt)
			{
				fprintf(stderr, "%d ", *qt);
			}
			fprintf(stderr, "\n");


#endif

			for (ut = Uset.begin(); ut != Uset.end(); ++ut)
			{
#if 0
				fprintf(stderr, "step -I- => *ut=%d: =>\n", *ut);
#endif
				for (qt = Qset.begin(); qt != Qset.end(); ++qt)
				{
#if 0
					fprintf(stderr, "step -I- => *qt=%d[%d] ", *qt, graph.link(*ut, *qt));
#endif

					int q_dist = graph.link(*ut, *qt);

					if (q_dist == -1)
					{
#if 0
						fprintf(stderr, "step -I- => q_dist is -1, skipping\n");
#endif
						continue;
					}

#if 0
					fprintf(stderr, "\n");
#endif

					if ((dist == -1) || (q_dist < dist))
					{
						dist = q_dist;
						u = *qt;
						p = *ut;
					}
				}
			}

#if 0
			fprintf(stderr, "step -I- => new closest node is u=%d  in dist=%d\n", u, dist);
#endif

			if (u == -1)
			{
				goto out; // node 'i' is not connected to anything, skipping
			}

			Uset.insert(u);
			Qset.erase(u);

			if ((distances.link(i, u) == -1) || (distances.link(i, p) + dist) < distances.link(i, p))
			{
				distances.link(i, u) = distances.link(i, p) + dist;
				predecessors.link(i, u) = p;
			}


			// check all neighbors of Uset for new best distances
			for (ut = Uset.begin(); ut != Uset.end(); ++ut)
			{
				int dist_iu = distances.link(i, *ut);

				for (qt = Qset.begin(); qt != Qset.end(); ++qt)
				{
					if (graph.link(*ut, *qt) == -1)
					{
#if 0
						fprintf(stderr, "step -II- => %d -> %d -> %d => skipping, %d and %d not direct neighbours\n",
								i, *ut, *qt, *ut, *qt);
#endif
						continue; // *qt is not direct neighour of *ut
					}

					int dist_uq = graph.link(*ut, *qt);

#if 0
					fprintf(stderr, "step -II- => %d -> %d -> %d => dist[i][u]=%d dist[u][q]=%d sum=%d\n",
							i, *ut, *qt, dist_iu, dist_uq, dist_iu + dist_uq);
#endif

					if ((distances.link(i, *qt) == -1) || ((dist_iu + dist_uq) < distances.link(i, *qt)))
					{
						distances.link(i, *qt) = dist_iu + dist_uq;
						predecessors.link(i, *qt) = *ut;
#if 0
						fprintf(stderr, "step -II- => new better node from i=%d via u=%d => q=%d in dist=%d\n",
								i, *ut, *qt, dist_iu + dist_uq);
#endif
					}
				}
			}

			fprintf(stderr, "crib(%p)::dijkstra() =>\n", this);
			fprintf(stderr, "graph => %s\n", graph.c_str());
			fprintf(stderr, "distances => %s\n", distances.c_str());
			fprintf(stderr, "predecessors => %s\n", predecessors.c_str());
		}
out:
		;;
		/*
		 * dijsktra itself end
		 */

	}

	/*
	 * generate routing table 'rtable' with next hops from 'predecessors' table
	 */

	for (int from = 0; from < dim; ++from)
	{
		uint64_t vtx_from = graph.vertex_unmap(from);
		for (int to = 0; to < dim; ++to)
		{
			uint64_t vtx_to = graph.vertex_unmap(to);

			if (from == to)
			{
				//irtable.link(from, to) = from;
				rtable.edge(vtx_from, vtx_to) = vtx_from;
				continue;
			}

			int a = -1;
			int b = to;

#if 1
			fprintf(stderr, "rtable: from=%d to=%d a=%d b=%d predecessors[%d][%d]=%d\n",
					from, to, a, b, from, b, predecessors.link(from, b));
#endif

			while ((a = predecessors.link(from, b)) != from)
			{
				b = a;
				if (a == -1)
					break;
			}

			//irtable.link(from, to) = b;
			fprintf(stderr, "b: %d\n", b);
			try {
				uint64_t vtx_nhop = graph.vertex_unmap(b);
				rtable.edge(vtx_from, vtx_to) = vtx_nhop;
			} catch (eGraphNotFound& e) {
				rtable.edge(vtx_from, vtx_to) = 0;
			}
		}
	}

	fprintf(stderr, "rtable => %s\n", rtable.c_str());

	/*
	 * rtable generation end
	 */
}


/*static*/
void
crib::test()
{
	crib g;

	fprintf(stderr, "g: %s\n", g.c_str());

	crandom vtx0(sizeof(uint64_t));
	crandom vtx1(sizeof(uint64_t));
	crandom vtx2(sizeof(uint64_t));
	crandom vtx3(sizeof(uint64_t));
	crandom vtx4(sizeof(uint64_t));
	crandom vtx5(sizeof(uint64_t));
	crandom vtx6(sizeof(uint64_t));
	crandom vtx7(sizeof(uint64_t));
	crandom vtx8(sizeof(uint64_t));
	crandom vtx9(sizeof(uint64_t));

	g.add_edge(vtx0.uint64(), vtx0.uint64(), 0);
	g.add_edge(vtx1.uint64(), vtx1.uint64(), 0);
	g.add_edge(vtx2.uint64(), vtx2.uint64(), 0);
	g.add_edge(vtx3.uint64(), vtx3.uint64(), 0);
	g.add_edge(vtx4.uint64(), vtx4.uint64(), 0);
	g.add_edge(vtx5.uint64(), vtx5.uint64(), 0);
	g.add_edge(vtx6.uint64(), vtx6.uint64(), 0);
	g.add_edge(vtx7.uint64(), vtx7.uint64(), 0);
	g.add_edge(vtx8.uint64(), vtx8.uint64(), 0);
	g.add_edge(vtx9.uint64(), vtx9.uint64(), 0);

	/* 1 -> 2 */ g.add_edge(vtx1.uint64(), vtx2.uint64(), 7);
	/* 1 -> 3 */ g.add_edge(vtx1.uint64(), vtx3.uint64(), 9);
	/* 1 -> 6 */ g.add_edge(vtx1.uint64(), vtx6.uint64(), 14);

	/* 2 -> 3 */ g.add_edge(vtx2.uint64(), vtx3.uint64(), 10);
	/* 2 -> 4 */ g.add_edge(vtx2.uint64(), vtx4.uint64(), 15);

	/* 3 -> 6 */ g.add_edge(vtx3.uint64(), vtx6.uint64(), 2);
	/* 3 -> 4 */ g.add_edge(vtx3.uint64(), vtx4.uint64(), 11);

	/* 4 -> 5 */ g.add_edge(vtx4.uint64(), vtx5.uint64(), 6);

	/* 5 -> 6 */ g.add_edge(vtx5.uint64(), vtx6.uint64(), 9);

	/* test: second non-connected graph */
	g.add_edge(vtx0.uint64(), vtx7.uint64(), 1);
	g.add_edge(vtx7.uint64(), vtx8.uint64(), 1);
	g.add_edge(vtx0.uint64(), vtx8.uint64(), 1);
	g.add_edge(vtx8.uint64(), vtx9.uint64(), 1);
	/* test end */

	g.dijkstra();

	fprintf(stderr, "g: %s", g.c_str());

	crib h(g);

	fprintf(stderr, "h: %s", h.c_str());

	//g.dijkstra();

	//fprintf(stderr, "h: %s", h->c_str());
}
