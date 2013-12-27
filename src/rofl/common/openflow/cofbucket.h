/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COFBUCKET_H
#define COFBUCKET_H 1

#include <string>
#include <vector>
#include <list>
#include <endian.h>
#include <ostream>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "openflow.h"
#include "../cerror.h"
#include "../cmemory.h"
#include "cofactions.h"

namespace rofl
{

/* error classes */
class eBucketBase 	: public cerror {}; // error base class for class cofbucket
class eBucketInval 	: public eBucketBase {}; // parameter is invalid
class eBucketBadLen : public eBucketBase {}; // invalid length



class cofbucket {

	uint8_t ofp_version;

public: // static methods and data structures


public: // data structures

	uint64_t packet_count; // packet count for this bucket
	uint64_t byte_count; // byte count for this bucket
    uint16_t weight;
    uint32_t watch_port;
    uint32_t watch_group;

	cofactions actions; // list of OpenFlow actions

public: // per instance methods

	/** constructor
	 */
	cofbucket(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN,
			uint16_t weigth = 0,
			uint32_t watch_port = 0,
			uint32_t watch_group = 0);

	/** constructor
	 */
	cofbucket(
			uint8_t ofp_version,
			uint8_t *bucket,
			size_t bclen);



	/** destructor
	 */
	virtual
	~cofbucket();

	/** assignment operator
	 */
	cofbucket& operator= (const cofbucket& b);

	/** pack bucket
	 */
	uint8_t*
	pack(uint8_t* bucket, size_t bclen);

	/** unpack bucket
	 */
	void
	unpack(uint8_t* bucket, size_t bclen);

	/** bucket length
	 */
	size_t
	length() const;


	/**
	 *
	 */
	void
	get_bucket_stats(
			cmemory& body);


	/**
	 *
	 */
	cofactions&
	get_actions() { return actions; };


private:

	/** pack bucket
	 */
	uint8_t*
	pack_of12(uint8_t* buf, size_t buflen);

	/** unpack bucket
	 */
	void
	unpack_of12(uint8_t *buf, size_t buflen);

	/** pack bucket
	 */
	uint8_t*
	pack_of13(uint8_t* buf, size_t buflen);

	/** unpack bucket
	 */
	void
	unpack_of13(uint8_t *buf, size_t buflen);

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofbucket const& bucket) {
		os << indent(0) << "<cofbucket ";
			os << "ofp-version:" 	<< (int)bucket.ofp_version 	<< " >" << std::endl;
			os << indent(2) << "<weight:" 		<< (int)bucket.weight 	<< " >" << std::endl;
			os << indent(2) << "<watch-group:" 	<< (int)bucket.watch_group 	<< " >" << std::endl;
			os << indent(2) << "<watch-port:" 	<< (int)bucket.watch_port 	<< " >" << std::endl;
			os << indent(2) << "<packet-count:"	<< (int)bucket.packet_count << " >" << std::endl;
			os << indent(2) << "<byte-count:" 	<< (int)bucket.byte_count 	<< " >" << std::endl;
			os << indent(2) << "<actions: >"	<< std::endl;
			indent i(4);
			os << bucket.actions;
		return os;
	};
};

}; // end of namespace

#endif
