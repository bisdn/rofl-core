/*
 * cofhelloelem.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "cofhelloelem.h"

using namespace rofl::openflow;

cofhello_elem::cofhello_elem(
		size_t len) :
				cmemory(len)
{
	ofh_generic = somem();
}


cofhello_elem::cofhello_elem(
		uint8_t *buf, size_t buflen) :
				cmemory(buf, buflen)
{
	ofh_generic = somem();
}


cofhello_elem::cofhello_elem(
		cofhello_elem const& elem)
{
	*this = elem;
}


cofhello_elem::~cofhello_elem()
{

}



cofhello_elem&
cofhello_elem::operator= (cofhello_elem const& elem)
{
	if (this == &elem)
		return *this;

	cmemory::operator=(elem);
	ofh_generic = somem();

	return *this;
}



uint8_t*
cofhello_elem::resize(size_t len)
{
	cmemory::resize(len);
	ofh_generic = somem();
	set_length(0); // FIXME
	return ofh_generic;
}



size_t
cofhello_elem::length() const
{
	size_t total_length = memlen();

	size_t pad = (0x7 & total_length);

	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}

	return (total_length);
}



void
cofhello_elem::pack(uint8_t *buf, size_t buflen)
{
	if (buflen < length())
		throw eHelloElemInval();
	memcpy(buf, somem(), memlen());
}



void
cofhello_elem::unpack(uint8_t *buf, size_t buflen)
{
	resize(buflen);
	assign(buf, buflen);
	ofh_generic = somem();
}


