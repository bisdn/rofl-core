/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cgraph.h"

using namespace rofl;

template <class T, class U>
cgraph<T, U>::cgraph()
{
	//resize(1);
}


template <class T, class U>
cgraph<T, U>::cgraph(
		cgraph<T, U> const& graph)
{
	//fprintf(stderr, "cgraph(%p)::cgraph() copy constructor\n", this);
	*this = graph;
}


template <class T, class U>
cgraph<T, U>::~cgraph()
{
	//fprintf(stderr, "cgraph(%p)::~cgraph() destructor\n", this);
	//int i = 0;
	typename std::vector< std::vector<U*>* >::iterator it;
	for (it = g.begin(); it != g.end(); ++it)
	{
		//int j = 0;
		typename std::vector<U*>::iterator jt;
		for (jt = (*it)->begin(); jt != (*it)->end(); ++jt)
		{
			//fprintf(stderr, "cgraph(%p)::~cgraph() [%d][%d]=%3d\n", this, i, j++, *(*jt));
			delete (*jt);
		}
		delete (*it);
		//i++;
	}
	g.clear();
}


template <class T, class U>
void
cgraph<T, U>::clear()
{
	//fprintf(stderr, "cgraph(%p)::reset()\n", this);
	typename std::vector< std::vector<U*>* >::iterator it;
	for (it = g.begin(); it != g.end(); ++it)
	{
		typename std::vector<U*>::iterator jt;
		for (jt = (*it)->begin(); jt != (*it)->end(); ++jt)
		{
			//fprintf(stderr, "%d ", *(*jt));
			delete (*jt);

		}
		//fprintf(stderr, "\n");
		delete (*it);
	}
	g.clear();
}


template <class T, class U>
void
cgraph<T, U>::reset(U u)
{
	for (int i = 0; i < (int)g.size(); i++)
	{
		for (int j = 0; j < (int)g.size(); j++)
		{
			*(g[i]->operator[] (j)) = u;
		}
	}
}


template <class T, class U>
cgraph<T, U>&
cgraph<T, U>::operator= (cgraph<T, U> const& graph)
{
	if (this == &graph)
		return *this;

	reset();

	//fprintf(stderr, "cgraph(%p)::operator=() [1] %s", this, c_str());

	resize(graph.g.size());

	//fprintf(stderr, "cgraph(%p)::operator=() [2] %s", this, c_str());

	for (int i = 0; i < (int)graph.g.size(); i++)
	{
		for (int j = 0; j < (int)graph.g.size(); j++)
		{
			*(g[i]->operator[] (j)) = *(graph.g[i]->operator[] (j));
		}
	}

	//fprintf(stderr, "cgraph(%p)::operator=() [3] %s", this, c_str());

	vertices.clear();

	std::copy(graph.vertices.begin(), graph.vertices.end(),
			std::inserter(vertices, vertices.begin()));

	return *this;
}


template <class T, class U>
bool
cgraph<T, U>::operator== (cgraph<T, U> const& graph)
{
	for (int i = 0; i < (int)graph.g.size(); i++)
	{
		for (int j = 0; j < (int)graph.g.size(); j++)
		{
			if (*(g[i]->operator[] (j)) != *(graph.g[i]->operator[] (j)))
			{
				return false;
			}
		}
	}
	return true;
}


template <class T, class U>
const char*
cgraph<T, U>::c_str()
{
	cvastring vas(512);

	info.assign(vas("cgraph(%p) =>\n", this));

	typename std::map<T, int>::iterator it;
	for (it = vertices.begin(); it != vertices.end(); ++it)
	{
		info.append(vas("vertex: %llx : %d\n", it->first, it->second));
	}

	info.append(vas("    ", this));

	for (int i = 0; i < (int)g.size(); i++)
	{
		info.append(vas("%2d ", i));
	}
	info.append("\n");

	for (int i = 0; i < (int)g.size(); i++)
	{
		info.append(vas("%2d: ", i));

		for (int j = 0; j < (int)g.size(); j++)
		{
			if (*(g[i]->operator[] (j)) >= 0)
				info.append(vas("%x ", *(g[i]->operator[] (j))));
			else
				info.append(vas(" - "));
		}
		info.append("\n");
	}

	return info.c_str();
}


template <class T, class U>
U&
cgraph<T, U>::link(
		int i,
		int j) throw (eGraphOutOfRange)
{
	int d = ((i > j) ? i : j) + 1;

	if (d > (int)g.size())
	{
		resize(d);
	}

	return *(g[i]->operator[] (j));
}


template <class T, class U>
void
cgraph<T, U>::resize(size_t d)
{
	if (d <= g.size())
		return;

	//fprintf(stderr, "cgraph(%p)::resize() d=%lu\n", this, d);

	// expand existing vectors
	typename std::vector<std::vector<U*>* >::iterator it;
	for (it = g.begin(); it != g.end(); ++it)
	{
		for (int j = g.size(); j < (int)d; j++)
		{
			(*it)->push_back(new U(-1));
		}
	}

	// create new vectors beyond current end
	for (int i = g.size(); i < (int)d; i++)
	{
		typename std::vector<U*> *v = new std::vector<U*>();
		for (int j = 0; j < (int)d; j++)
		{
			v->push_back(new U(-1));
		}
		g.push_back(v);
	}
}



template <class T, class U>
void
cgraph<T, U>::add_edge(
		T from,
		T to,
		U weight,
		bool bidirectional)
{
	link(vertex_map(from), vertex_map(to)) = weight;

	if (bidirectional)
	{
		link(vertex_map(to), vertex_map(from)) = weight;
	}
}


template <class T, class U>
void
cgraph<T, U>::rem_edge(
		T from,
		T to,
		bool bidirectional)
{
	try {
		link(vertex_exists(from), vertex_exists(to)) = -1;
	} catch (eGraphNotFound& e) {}

	try {
		if (bidirectional)
		{
			link(vertex_exists(to), vertex_exists(from)) = -1;
		}
	} catch (eGraphNotFound& e) {}
}


template <class T, class U>
U&
cgraph<T, U>::edge(T from, T to)
{
	return link(vertex_map(from), vertex_map(to));
}


template <class T, class U>
int
cgraph<T, U>::vertex_map(T vtxid)
{
	try {

		return vertex_exists(vtxid);

	} catch (eGraphNotFound& e) {

		int graph_id = 0;
		typename std::map<T, int>::iterator it;
restart:
		for (it = vertices.begin(); it != vertices.end(); ++it)
		{
			if (graph_id == it->second)
			{
				break;
			}
		}
		if (it != vertices.end())
		{
			graph_id++;
			goto restart;
		}

		vertices[vtxid] = graph_id;

		return graph_id;
	}
	return -1; // just to make eclipse happy :)
}


template <class T, class U>
int
cgraph<T, U>::vertex_exists(T vtxid) throw (eGraphNotFound)
{
	WRITELOG(CRIB, DBG, "crib(%p)::vertex_exists() vtxid:%llx", this, vtxid);

	if (vertices.find(vtxid) == vertices.end())
	{
		throw eGraphNotFound();
	}
	return vertices[vtxid];
}


template <class T, class U>
T
cgraph<T, U>::vertex_unmap(U i) throw (eGraphNotFound)
{
	typename std::map<T, int>::iterator it;
	for (it = vertices.begin(); it != vertices.end(); ++it)
	{
		if ((U)(it->second) == i)
		{
			return it->first;
		}
	}
	throw eGraphNotFound();
}


template class cgraph<uint64_t, int>::vertex_graph_id<uint64_t>;
template class cgraph<uint64_t, int>;
template class cgraph<uint64_t, uint64_t>;
template class cgraph<uint64_t, uint16_t>;


