/*
 * cudpmsg.h
 *
 *  Created on: 01.09.2013
 *      Author: andreas
 */

#ifndef CUDPMSG_H_
#define CUDPMSG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include <rofl/common/cmemory.h>

namespace spray
{

class cudpmsg :
		public rofl::cmemory
{
	struct udp_msg_hdr_t {
		uint8_t type;
		uint8_t pad[3];
		uint32_t seqno;
	};

	struct udp_msg_hdr_t *msg_hdr;

public:

	enum udp_msg_type_t {
		UMT_UNKNOWN = 0,
		UMT_START = 1,
		UMT_DATA = 2,
		UMT_STOP = 3,
	};

public:

	cudpmsg(size_t msglen = 22);

	virtual
	~cudpmsg();

	uint8_t*
	resize(size_t msglen);

public:

	void
	set_type(uint8_t type);

	uint8_t
	get_type() const;

	void
	set_seqno(uint32_t seqno);

	uint32_t
	get_seqno() const;
};

}; // end of namespace

#endif /* CUDPMSG_H_ */
