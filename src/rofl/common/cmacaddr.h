/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CMACADDR_H
#define CMACADDR_H 1

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include "openflow/openflow.h"

#ifdef __cplusplus
}
#endif

#include "cvastring.h"
#include "cmemory.h"

namespace rofl
{


/**
 * @class	cmacaddr
 * @brief	Helper class for managing hardware addresses.
 *
 * This class provides a convenience interface for managing hardware
 * addresses.
 */
class cmacaddr :
		public cmemory
{
public:

	/**
	 * @brief	Constructor.
	 *
	 */
	cmacaddr();



	/**
	 * @brief	Copy constructor.
	 *
	 */
	cmacaddr(cmacaddr const& ma);



	/**
	 * @brief	Construct from memory instance-
	 *
	 * @param mem memory area containing the hardware address
	 */
	cmacaddr(cmemory const& mem);



	/**
	 * @brief	Constructor from plain memory area.
	 *
	 * @param data pointer to start of memory area
	 * @param datalen length of memory area
	 */
	cmacaddr(uint8_t *data, size_t datalen);



	/**
	 * @brief	Constructor from std::string.
	 *
	 * @param s_addr std::string containing the hardware address defined as "00:00:00:00:00:00"
	 */
	cmacaddr(const std::string& s_addr);



	/**
	 * @brief	Constructor from C-string.
	 *
	 * @param s_addr null-terminated c-string containing the hardware address defined as "00:00:00:00:00:00"
	 */
	cmacaddr(const char *s_addr);

	/**
	 * @brief Constructor from uint64_t (MAC is expected to be in host byte order and in the lower bytes)
	 *
	 * @param mac
	 */
	cmacaddr(const uint64_t mac);


	/**
	 * @brief	Assignment operator.
	 *
	 * @param ma cmacaddr instance to be assigned
	 */
	cmacaddr& operator= (cmacaddr const& ma);



	/**
	 * @brief	Destructor.
	 *
	 */
	virtual
	~cmacaddr() {};



	/**
	 * @brief	Comparison operator (equal).
	 *
	 * @param ma cmacaddr instance to be compared
	 */
	bool
	operator== (
			cmacaddr const& ma) const;



	/**
	 * @brief	Comparison operator (unequal).
	 *
	 * @param ma cmacaddr instance to be compared
	 */
	bool
	operator!= (
			cmacaddr const& ma) const;



	/**
	 * @brief	AND operator.
	 *
	 * @param ma cmacaddr instance for operation
	 */
	cmacaddr&
	operator& (
			cmacaddr const& ma);



	/**
	 * @brief	Check for multicast bit in hardware address.
	 *
	 * @return true: hardware address has multicast bit set, false otherwise
	 */
	bool
	is_multicast() const;



	/**
	 * @brief	Check for broadcast hardware address.
	 *
	 * @return true: hardware address equals "ff:ff:ff:ff:ff:ff", false otherwise
	 */
	bool
	is_broadcast() const;



	/**
	 * @brief	Check for null hardware address.
	 *
	 * @return true: hardware address equals "00:00:00:00:00:00", false otherwise
	 */
	bool
	is_null() const;



	/**
	 * @brief	Returns a C-string containing information about this hardware address.
	 *
	 * @return c-string
	 */
	const char*
	c_str() const;

	/**
	 * @brief	Return the MAC address as a uint64_t value. The MAC address will be transformed to host byte order and resides in the lower bytes.
	 *
	 * @return uint64_t mac
	 */
	uint64_t
	get_mac() const;

private:

	std::string info; // info string
};



/**
 * @class cmacbcast
 * @brief	A class with a pre-defined hardware address of "ff:ff:ff:ff:ff:ff".
 */
class cmacbcast : public cmacaddr {
public:
	cmacbcast() :
		cmacaddr("ff:ff:ff:ff:ff:ff") {};
	virtual
	~cmacbcast() {};
};

}; // end of namespace

#endif
