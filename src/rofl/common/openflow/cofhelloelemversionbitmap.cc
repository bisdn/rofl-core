/*
 * cofhelloelemversionbitmap.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "cofhelloelemversionbitmap.h"

using namespace rofl::openflow;

cofhello_elem_versionbitmap::cofhello_elem_versionbitmap() :
		cofhello_elem(sizeof(struct openflow13::ofp_hello_elem_header))
{
	ofh_hello_elem_version_bitmap_generic = somem();
	set_type(openflow13::OFPHET_VERSIONBITMAP);
	set_length(0); // set, when being packed
}



cofhello_elem_versionbitmap::cofhello_elem_versionbitmap(
		uint8_t *buf, size_t buflen) :
				cofhello_elem(buflen)
{
	unpack(buf, buflen);
}



cofhello_elem_versionbitmap::cofhello_elem_versionbitmap(
		cofhello_elem const& elem) :
				cofhello_elem(elem)
{
	ofh_hello_elem_version_bitmap_generic = somem();

	uint32_t *bitmap = (uint32_t*)(somem() + sizeof(struct openflow13::ofp_hello_elem_header));
	for (unsigned int i = 0; i < ((get_length() - 2*sizeof(uint16_t)) / sizeof(uint32_t)); i++) {
		bitmaps.push_back(be32toh(bitmap[i]));
	}
}



cofhello_elem_versionbitmap::cofhello_elem_versionbitmap(
		cofhello_elem_versionbitmap const& elem) :
				cofhello_elem(dynamic_cast<cofhello_elem const&>( elem ))
{
	*this = elem;
}




cofhello_elem_versionbitmap::~cofhello_elem_versionbitmap()
{

}



cofhello_elem_versionbitmap&
cofhello_elem_versionbitmap::operator= (
		cofhello_elem_versionbitmap const& elem)
{
	if (this == &elem)
		return *this;

	cofhello_elem::operator =(dynamic_cast<cofhello_elem const&>( elem ));
	ofh_hello_elem_version_bitmap_generic = somem();

	bitmaps.clear();
	for (std::vector<uint32_t>::const_iterator
			it = elem.bitmaps.begin(); it != elem.bitmaps.end(); ++it) {
		bitmaps.push_back(*it);
	}

	return *this;
}



cofhello_elem_versionbitmap
cofhello_elem_versionbitmap::operator& (
		cofhello_elem_versionbitmap const& versionbitmap) const
{
	size_t size =
			(bitmaps.size() < versionbitmap.bitmaps.size()) ?
						bitmaps.size() : versionbitmap.bitmaps.size();

	cofhello_elem_versionbitmap elem;
	elem.bitmaps.resize(size, 0);

	for (unsigned int i = 0; i < elem.bitmaps.size(); i++) {
		elem.bitmaps[i] = bitmaps[i] & versionbitmap.bitmaps[i];
	}

	return elem;
}



uint8_t*
cofhello_elem_versionbitmap::resize(size_t len)
{
	cofhello_elem::resize(len);
	ofh_hello_elem_version_bitmap_generic = somem();
	return ofh_hello_elem_version_bitmap_generic;
}



size_t
cofhello_elem_versionbitmap::length() const
{
	size_t total_length =
			sizeof(struct openflow13::ofp_hello_elem_header) +
									bitmaps.size() * sizeof(uint32_t);

	size_t pad = (0x7 & total_length);

	/* append padding if not a multiple of 8 */
	if (pad) {
		total_length += 8 - pad;
	}

	return (total_length);
}



void
cofhello_elem_versionbitmap::pack(uint8_t *buf, size_t buflen)
{
	if (buflen < length())
		throw eHelloElemVersionBitmapInval();

	set_length(sizeof(struct openflow13::ofp_hello_elem_header) + bitmaps.size() * sizeof(uint32_t));

	memcpy(buf, somem(), sizeof(struct openflow13::ofp_hello_elem_header));

	uint32_t *bitmap = (uint32_t*)(buf + sizeof(struct openflow13::ofp_hello_elem_header));
	for (unsigned int i = 0; i < bitmaps.size(); i++) {
		bitmap[i] = htobe32(bitmaps[i]);
	}
}



void
cofhello_elem_versionbitmap::unpack(uint8_t *buf, size_t buflen)
{
	if (buflen < sizeof(struct openflow13::ofp_hello_elem_header))
		throw eHelloElemVersionBitmapInval();

	resize(buflen);
	assign(buf, buflen);
	ofh_hello_elem_version_bitmap_generic = somem();

	if (buflen < get_length())
		throw eHelloElemVersionBitmapInval();

	uint32_t *bitmap = (uint32_t*)(somem() + sizeof(struct openflow13::ofp_hello_elem_header));
	for (unsigned int i = 0; i < ((get_length() - 2*sizeof(uint16_t)) / sizeof(uint32_t)); i++) {
		bitmaps.push_back(be32toh(bitmap[i]));
	}
}



void
cofhello_elem_versionbitmap::add_ofp_version(uint8_t ofp_version)
{
	unsigned int index  = ofp_version / 32;
	unsigned int bitpos = ofp_version % 32;

	while (bitmaps.size() < (index + 1)) {
		bitmaps.push_back(0);
	}
	bitmaps[index] |= (1 << bitpos);
}



void
cofhello_elem_versionbitmap::drop_ofp_version(uint8_t ofp_version)
{
	unsigned int index  = ofp_version / 32;
	unsigned int bitpos = ofp_version % 32;

	if (bitmaps.size() < (index + 1))
		return;
	bitmaps[index] &= ~(1 << bitpos);
}



bool
cofhello_elem_versionbitmap::has_ofp_version(uint8_t ofp_version) const
{
	unsigned int index  = ofp_version / 32;
	unsigned int bitpos = ofp_version % 32;

	if (bitmaps.size() < (index + 1))
		return false;

	return (bitmaps[index] & (1 << bitpos));
}



void
cofhello_elem_versionbitmap::clear_ofp_versions()
{
	for (std::vector<uint32_t>::iterator
			it = bitmaps.begin(); it != bitmaps.end(); ++it) {
		(*it) = 0;
	}
}



uint8_t
cofhello_elem_versionbitmap::get_highest_ofp_version() const
{
	uint8_t ofp_version = OFP_VERSION_UNKNOWN;

	unsigned int j = bitmaps.size();
	for (std::vector<uint32_t>::const_reverse_iterator
			it = bitmaps.rbegin(); it != bitmaps.rend(); ++it) {
		uint32_t const& bitmap = (*it); j--;
		for (int i = 31; i >= 0; i--) {
			if (bitmap & (1<<i)) {
				return ((j * 32) + i);
			}
		}
	}

	return ofp_version;
}




