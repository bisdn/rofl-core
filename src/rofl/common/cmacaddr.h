/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CMACADDR_H
#define CMACADDR_H 1

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include "openflow/openflow12.h"

#ifdef __cplusplus
}
#endif

#include "cvastring.h"
#include "cmemory.h"

class cmacaddr : public cmemory {
public:

	/** constructor
	 *
	 */
	cmacaddr();

	/** constructor
	 *
	 */
	cmacaddr(cmemory const& mem);

	/** constructor
	 *
	 */
	cmacaddr(uint8_t *data, size_t datalen);

	/** constructor
	 *
	 */
	cmacaddr(const std::string& s_addr);

	/** constructor
	 *
	 */
	cmacaddr(const char *s_addr);

	/** assignment operator
	 *
	 */
	cmacaddr& operator= (cmacaddr const& ma);

	/** destructor
	 *
	 */
	virtual
	~cmacaddr() {};

	/** comparison operator
	 *
	 */
	bool
	operator== (
			cmacaddr const& ma) const;

	/** comparison operator
	 *
	 */
	bool
	operator!= (
			cmacaddr const& ma) const;

	/** operator&
	 *
	 */
	cmacaddr&
	operator& (
			cmacaddr const& ma);

	/** is multicast?
	 *
	 */
	bool
	is_multicast() const;

	/** is broadcast?
	 *
	 */
	bool
	is_broadcast() const;

	/** is null?
	 *
	 */
	bool
	is_null() const;

	/**
	 *
	 */
	const char*
	c_str() const;

private:

	std::string info; // info string
};


class cmacbcast : public cmacaddr {
public:
	cmacbcast() :
		cmacaddr("ff:ff:ff:ff:ff:ff") {};
	virtual
	~cmacbcast() {};
};

#endif
