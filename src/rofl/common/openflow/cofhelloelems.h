/*
 * cofhelloelems.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef COFHELLOELEMS_H_
#define COFHELLOELEMS_H_

#include <list>

#include "cofhelloelem.h"

namespace rofl {
namespace openflow {

class cofhelloelems :
		public std::list<cofhello_elem*>
{
public: // iterators

	typedef typename std::list<cofhello_elem*>::iterator iterator;
	typedef typename std::list<cofhello_elem*>::const_iterator const_iterator;

	typedef typename std::list<cofhello_elem*>::reverse_iterator reverse_iterator;
	typedef typename std::list<cofhello_elem*>::const_reverse_iterator const_reverse_iterator;

public:

	/**
	 *
	 */
	cofhelloelems();

	/**
	 *
	 */
	virtual
	~cofhelloelems();

	/**
	 *
	 */
	cofhelloelems(uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	cofhelloelems(cofhelloelems const& elems);

public:

	/** create a std::list<cofaction*> from a struct ofp_flow_mod
	 */
	void
	unpack(uint8_t *buf, size_t buflen);



	/** builds an array of struct ofp_instructions
	 * from a std::vector<cofinst*>
	 */
	uint8_t*
	pack(uint8_t* buf, size_t buflen);



	/** returns required length for array of struct ofp_action_headers
	 * for all actions defined in std::vector<cofaction*>
	 */
	size_t
	length() const;
};

}; /* namespace openflow */
}; /* namespace rofl */



#endif /* COFHELLOELEMS_H_ */
