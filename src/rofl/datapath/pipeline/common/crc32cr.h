/*
 * crc32cr.h
 *
 *  Created on: 11.07.2014
 *      Author: andreas
 *
 * This is the reference implementation from RFC 3309.
 * Please check RFC 3309 for more details and the references mentioned
 * in the comment below.
 */

#ifndef CRC32CR_H_
#define CRC32CR_H_ 1

#include <inttypes.h>
#include <sys/types.h>

#define CRC32C_POLY 0x1EDC6F41
#define CRC32C(c,d) (c=(c>>8)^cpc_crc_c[(c^(d))&0xFF])

uint32_t generate_crc32c(uint8_t* buf, size_t buflen);

#endif /* CRC32CR_H_ */
