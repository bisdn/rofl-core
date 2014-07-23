/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cofhelloelems.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "cofhelloelems.h"

using namespace rofl::openflow;

cofhelloelems::cofhelloelems()
{

}


cofhelloelems::~cofhelloelems()
{
	clear();
}



cofhelloelems::cofhelloelems(
		uint8_t *buf, size_t buflen)
{
	unpack(buf, buflen);
}



cofhelloelems::cofhelloelems(
		cmemory const& body)
{
	unpack(body.somem(), body.memlen());
}



cofhelloelems::cofhelloelems(
		cofhelloelems const& elems)
{
	*this = elems;
}



cofhelloelems&
cofhelloelems::operator= (
		cofhelloelems const& elems)
{
	if (this == &elems)
		return *this;

	clear();

	for (cofhelloelems::const_iterator
			it = elems.begin(); it != elems.end(); ++it) {
		map_and_insert(*(it->second));
	}

	return *this;
}



void
cofhelloelems::clear()
{
	for (cofhelloelems::iterator
			it = begin(); it != end(); ++it) {
		delete (it->second);
	}
	std::map<uint8_t, cofhello_elem*>::clear();
}



void
cofhelloelems::map_and_insert(
		cofhello_elem const& elem)
{
	if (find(elem.get_type()) != end()) {
		delete operator[](elem.get_type());
		erase(elem.get_type());
	}

	switch (elem.get_type()) {
	case openflow13::OFPHET_VERSIONBITMAP: {
		operator[] (elem.get_type()) = new cofhello_elem_versionbitmap(elem);
	} break;
	default: {
		operator[] (elem.get_type()) = new cofhello_elem(elem);
	} break;
	}
}



size_t
cofhelloelems::length() const
{
	size_t len = 0;
	for (cofhelloelems::const_iterator
			it = begin(); it != end(); ++it) {
		cofhello_elem& elem = *(it->second);
		len += elem.length();
	}
	return len;
}



void
cofhelloelems::pack(
		uint8_t *buf, size_t buflen)
{
	if (buflen < length())
		throw eHelloElemsInval();

	for (cofhelloelems::iterator
			it = begin(); it != end(); ++it) {
		cofhello_elem& elem = *(it->second);
		elem.pack(buf, elem.length());
		buf += elem.length();
	}
}



void
cofhelloelems::unpack(
		uint8_t *buf, size_t buflen)
{
	clear();

	while (buflen > sizeof(struct openflow13::ofp_hello_elem_header)) {
		struct openflow13::ofp_hello_elem_header* hello =
				(struct openflow13::ofp_hello_elem_header*)buf;
		if ((be16toh(hello->length) > buflen) || (be16toh(hello->length) == 0))
			break;
		map_and_insert(cofhello_elem(buf, be16toh(hello->length)));

		/* calculate padded length */
		size_t total_length = be16toh(hello->length);
		size_t pad = (0x7 & total_length);
		/* append padding if not a multiple of 8 */
		if (pad) {
			total_length += 8 - pad;
		}

		/* adjust buf and buflen */
		buf += total_length;
		buflen -= total_length;
	}
}



cofhello_elem_versionbitmap&
cofhelloelems::add_hello_elem_versionbitmap()
{
	std::map<uint8_t, cofhello_elem*>& elems = *this;

	if (elems.find(openflow13::OFPHET_VERSIONBITMAP) != elems.end()) {
		delete elems[openflow13::OFPHET_VERSIONBITMAP];
	}
	elems[openflow13::OFPHET_VERSIONBITMAP] = new cofhello_elem_versionbitmap();
	return *dynamic_cast<cofhello_elem_versionbitmap*>(elems[openflow13::OFPHET_VERSIONBITMAP]);
}



cofhello_elem_versionbitmap&
cofhelloelems::set_hello_elem_versionbitmap()
{
	std::map<uint8_t, cofhello_elem*>& elems = *this;

	if (elems.find(openflow13::OFPHET_VERSIONBITMAP) == elems.end()) {
		elems[openflow13::OFPHET_VERSIONBITMAP] = new cofhello_elem_versionbitmap();
	}
	return *dynamic_cast<cofhello_elem_versionbitmap*>(elems[openflow13::OFPHET_VERSIONBITMAP]);
}



cofhello_elem_versionbitmap&
cofhelloelems::get_hello_elem_versionbitmap()
{
	std::map<uint8_t, cofhello_elem*>& elems = *this;

	if (elems.find(openflow13::OFPHET_VERSIONBITMAP) == elems.end()) {
		throw eHelloElemsNotFound();
	}
	return *dynamic_cast<cofhello_elem_versionbitmap*>(elems.at(openflow13::OFPHET_VERSIONBITMAP));
}



void
cofhelloelems::drop_hello_elem_versionbitmap()
{
	std::map<uint8_t, cofhello_elem*>& elems = *this;

	if (elems.find(openflow13::OFPHET_VERSIONBITMAP) == elems.end()) {
		return;
	}
	delete elems[openflow13::OFPHET_VERSIONBITMAP];
	elems.erase(openflow13::OFPHET_VERSIONBITMAP);
}



bool
cofhelloelems::has_hello_elem_versionbitmap()
{
	std::map<uint8_t, cofhello_elem*>& elems = *this;

	return (elems.find(openflow13::OFPHET_VERSIONBITMAP) != elems.end());
}



