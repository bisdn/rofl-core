/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofhelloelems.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef COFHELLOELEMS_H_
#define COFHELLOELEMS_H_

#include <inttypes.h>
#include <map>

#include "rofl/common/openflow/cofhelloelem.h"
#include "rofl/common/openflow/cofhelloelemversionbitmap.h"
#include "rofl/common/logging.h"

namespace rofl {
namespace openflow {

class eHelloElemsBase 		: public RoflException {};
class eHelloElemsInval		: public eHelloElemsBase {};
class eHelloElemsNotFound	: public eHelloElemsBase {};

class cofhelloelems :
		public std::map<uint8_t, cofhello_elem*>
{
public: // iterators

	typedef std::map<uint8_t, cofhello_elem*>::iterator iterator;
	typedef std::map<uint8_t, cofhello_elem*>::const_iterator const_iterator;

	typedef std::map<uint8_t, cofhello_elem*>::reverse_iterator reverse_iterator;
	typedef std::map<uint8_t, cofhello_elem*>::const_reverse_iterator const_reverse_iterator;

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
	cofhelloelems(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	cofhelloelems(
			cmemory const& body);

	/**
	 *
	 */
	cofhelloelems(
			cofhelloelems const& elems);

	/**
	 *
	 */
	cofhelloelems&
	operator= (
			cofhelloelems const& elems);

public:

	/**
	 *
	 */
	void
	unpack(uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	void
	pack(uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	clear();

public:

	/**
	 *
	 */
	cofhello_elem_versionbitmap&
	add_hello_elem_versionbitmap();

	/**
	 *
	 */
	cofhello_elem_versionbitmap&
	set_hello_elem_versionbitmap();

	/**
	 *
	 */
	cofhello_elem_versionbitmap&
	get_hello_elem_versionbitmap();

	/**
	 *
	 */
	void
	drop_hello_elem_versionbitmap();

	/**
	 *
	 */
	bool
	has_hello_elem_versionbitmap();

private:

	/**
	 *
	 */
	void
	map_and_insert(
			cofhello_elem const& elem);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofhelloelems const& elems) {
		os << indent(0) << "<cofhelloelems #elems:" << (int)elems.size() <<
				" length:" << (int)elems.length() << " >" << std::endl;
		indent i(2);
		for (cofhelloelems::const_iterator
				it = elems.begin(); it != elems.end(); ++it) {
			cofhello_elem const& elem = *(it->second);
			switch (elem.get_type()) {
			case openflow13::OFPHET_VERSIONBITMAP: {
				os << cofhello_elem_versionbitmap(elem);
			} break;
			default: {
				os << *(it->second);
			} break;
			}
		}
		return os;
	};
};

}; /* namespace openflow */
}; /* namespace rofl */



#endif /* COFHELLOELEMS_H_ */
