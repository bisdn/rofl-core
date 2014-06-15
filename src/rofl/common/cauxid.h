/*
 * cauxid.h
 *
 *  Created on: 20.05.2014
 *      Author: andreas
 */

#ifndef CAUXID_H_
#define CAUXID_H_

#include <inttypes.h>
#include <iostream>

#include "rofl/common/logging.h"
#include "rofl/common/croflexception.h"

namespace rofl {

class eAuxIdBase 		: public RoflException {
public:
	eAuxIdBase(const std::string __arg) : RoflException(__arg) {};
};

class eAuxIdNotFound 	: public eAuxIdBase {
public:
	eAuxIdNotFound(const std::string __arg) : eAuxIdBase(__arg) {};
};


class cauxid {

	uint8_t id;

public:

	/**
	 *
	 */
	cauxid(
			uint8_t id = 0) :
		id(id) {};

	/**
	 *
	 */
	~cauxid() {};

	/**
	 *
	 */
	cauxid(
			const cauxid& auxid) {
		*this = auxid;
	};

	/**
	 *
	 */
	cauxid&
	operator= (
			const cauxid& auxid) {
		if (this == &auxid)
			return *this;
		id = auxid.id;
		return *this;
	};

	/**
	 *
	 */
	bool
	operator< (
			const cauxid& auxid) const {
		return (id < auxid.id);
	};

	/**
	 *
	 */
	uint8_t const&
	get_id() const { return id; }

public:

	friend std::ostream&
	operator<< (std::ostream& os, const cauxid& auxid) {
		os << rofl::indent(0) << "<cauxid id:" << (int)auxid.id << " >" << std::endl;
		return os;
	};

};

}; // end of namespace rofl

#endif /* CAUXID_H_ */
