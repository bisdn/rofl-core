/*
 * cofhelloelem.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef COFHELLO_ELEM_H_
#define COFHELLO_ELEM_H_

#include "rofl/common/cmemory.h"
#include "rofl/common/logging.h"
#include "rofl/common/openflow/openflow.h"

namespace rofl {
namespace openflow {

class eHelloElemBase 		: public RoflException {};
class eHelloElemInval		: public eHelloElemBase {};

class cofhello_elem :
			public cmemory
{

	union {
		uint8_t										*ofhu_generic;
		struct openflow13::ofp_hello_elem_header	*ofhu13_hello_elem_header;
	} ofhu;

#define ofh_generic   			ofhu.ofhu_generic
#define ofh_hello_elem_header 	ofhu.ofhu13_hello_elem_header

public:

	/**
	 *
	 */
	cofhello_elem(
			size_t len);

	/**
	 *
	 */
	cofhello_elem(
			uint8_t *buf, size_t buflen);

	/**
	 *
	 */
	cofhello_elem(
			cofhello_elem const& elem);

	/**
	 *
	 */
	virtual
	~cofhello_elem();

	/**
	 *
	 */
	cofhello_elem&
	operator= (
			cofhello_elem const& elem);

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

	/**
	 *
	 */
	uint16_t
	get_type() const { return be16toh(ofh_hello_elem_header->type); };

	/**
	 *
	 */
	void
	set_type(uint16_t type) { ofh_hello_elem_header->type = htobe16(type); };

	/**
	 *
	 */
	uint16_t
	get_length() const { return be16toh(ofh_hello_elem_header->length); };

	/**
	 *
	 */
	void
	set_length(uint16_t len) { ofh_hello_elem_header->length = htobe16(len); };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofhello_elem const& elem) {
		os << indent(0) << "<cofhello_elem type:" << elem.get_type()
				<< " length:" << elem.get_length() << " padding:" << (elem.length()-elem.get_length()) << " >" << std::endl;
		return os;
	};
};

}; /* namespace openflow */
}; /* namespace rofl */

#endif /* COFHELLO_ELEM_H_ */


