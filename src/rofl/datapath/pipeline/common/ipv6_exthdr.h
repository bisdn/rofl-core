/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __IPV6_EXTHDR_H__
#define __IPV6_EXTHDR_H__

enum ipv6_exthdr_flags {
	IPV6_EH_NONEXT = 	1 << 0,/* "No next header" encountered. */
	IPV6_EH_ESP= 		1 << 1,/* Encrypted Sec Payload header present. */
	IPV6_EH_AUTH= 		1 << 2,/* Authentication header present. */
	IPV6_EH_DEST= 		1 << 3,/* 1 or 2 dest headers present. */
	IPV6_EH_FRAG= 		1 << 4,/* Fragment header present. */
	IPV6_EH_ROUTER = 	1 << 5,/* Router header present. */
	IPV6_EH_HOP =	 	1 << 6,/* Hop-by-hop header present. */
	IPV6_EH_UNREP = 	1 << 7,/* Unexpected repeats encountered. */
	IPV6_EH_UNSEQ = 	1 << 8,/* Unexpected sequencing encountered. */
};


#endif // __IPV6_EXTHDR_H__
