/*
 * cofhelloelemversionbitmap.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef COFHELLOELEMVERSIONBITMAP_H_
#define COFHELLOELEMVERSIONBITMAP_H_

#include <inttypes.h>
#include <vector>

#include "rofl/common/openflow/cofhelloelem.h"

namespace rofl {
namespace openflow {

class eHelloElemVersionBitmapBase 	: public eHelloElemBase {};
class eHelloElemVersionBitmapInval	: public eHelloElemVersionBitmapBase {};

class cofhello_elem_versionbitmap :
		public cofhello_elem
{
	std::vector<uint32_t>	bitmaps;

	union {
		uint8_t										*ofhu_hello_elem_versionbitmap_generic;
		openflow13::ofp_hello_elem_versionbitmap	*ofhu13_versionbitmap;
	} ofhu;

#define ofh_hello_elem_version_bitmap_generic 		ofhu.ofhu_hello_elem_versionbitmap_generic
#define ofh_versionbitmap 							ofhu.ofhu13_versionbitmap

public:

	/**
	 *
	 */
	cofhello_elem_versionbitmap();

	/**
	 *
	 */
	cofhello_elem_versionbitmap(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	cofhello_elem_versionbitmap(
			cofhello_elem const& elem);

	/**
	 *
	 */
	cofhello_elem_versionbitmap(
			cofhello_elem_versionbitmap const& elem);

	/**
	 *
	 */
	virtual
	~cofhello_elem_versionbitmap();

	/**
	 *
	 */
	cofhello_elem_versionbitmap&
	operator= (
			cofhello_elem_versionbitmap const& elem);

	/**
	 * @brief	Calculates (*this AND versionbitmap) and returns a new copy of cofhello_elem_versionbitmap
	 */
	cofhello_elem_versionbitmap
	operator& (
			cofhello_elem_versionbitmap const& versionbitmap) const;

public:

	/**
	 *
	 */
	virtual uint8_t*
	resize(size_t len);

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	void
	pack(uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	void
	add_ofp_version(uint8_t ofp_version);

	/**
	 *
	 */
	void
	drop_ofp_version(uint8_t ofp_version);

	/**
	 *
	 */
	bool
	has_ofp_version(uint8_t ofp_version) const;

	/**
	 *
	 */
	void
	clear_ofp_versions();

	/**
	 *
	 */
	uint8_t
	get_highest_ofp_version() const;

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofhello_elem_versionbitmap const& elem) {
		os << dynamic_cast<cofhello_elem const&>( elem );
		os << indent(2) << "<cofhello_elem_versionbitmap >" << std::endl;
		for (unsigned int i = 0; i < elem.bitmaps.size(); i++) {
			os << indent(4) << "<bitmap[" << i << "] 0x" <<
					std::setfill('0') << std::setw(8) <<
					std::hex << elem.bitmaps[i] << std::dec <<
					std::setw(0) << std::setfill(' ') << " >" << std::endl;
		}
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << "VersionBitmap ";
		if (has_ofp_version(rofl::openflow10::OFP_VERSION)) {
			ss << "1.0 ";
		}
		if (has_ofp_version(rofl::openflow12::OFP_VERSION)) {
			ss << "1.2 ";
		}
		if (has_ofp_version(rofl::openflow13::OFP_VERSION)) {
			ss << "1.3 ";
		}
		return ss.str();
	};
};

}; /* namespace openflow */
}; /* namespace rofl */



#endif /* COFHELLOELEMVERSIONBITMAP_H_ */
