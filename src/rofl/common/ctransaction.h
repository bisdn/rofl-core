/*
 * ctransaction.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CTRANSACTION_H_
#define CTRANSACTION_H_ 1

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include "cclock.h"

namespace rofl {
namespace openflow {

class ctransaction {

	uint32_t 	xid;
	cclock		since;			// time this transaction was created
	cclock		expires;		// time this transaction expires
	uint8_t		msg_type;		// message type
	uint16_t	msg_sub_type;	// message sub-type

public:

	/**
	 *
	 */
	ctransaction();

	/**
	 *
	 */
	ctransaction(uint32_t xid, cclock const& delta, uint8_t msg_type = 0, uint16_t msg_sub_type = 0);

	/**
	 *
	 */
	virtual
	~ctransaction();

	/**
	 *
	 */
	ctransaction(ctransaction const& ta);

	/**
	 *
	 */
	ctransaction&
	operator= (ctransaction const& ta);

public:

	/**
	 *
	 */
	uint32_t
	get_xid() const { return xid; };

	/**
	 *
	 */
	cclock&
	get_since() { return since; };

	/**
	 *
	 */
	cclock&
	get_expires() { return expires; };

	/**
	 *
	 */
	uint8_t
	get_msg_type() const { return msg_type; };

	/**
	 *
	 */
	uint16_t
	get_msg_sub_type() const { return msg_sub_type; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, ctransaction const& ta) {
		os << indent(0) << "<transaction ";
			os << "xid:" << std::hex << (int)ta.xid << std::dec
					<< " msg-type:" << (int)ta.msg_type
					<< " msg-subtype:" << (int)ta.msg_sub_type << " >" << std::endl;
			cclock delta(ta.since); delta -= ta.expires;
			os << indent(2) << "<expires: >" << std::endl;
			{ indent i(4); os << delta; }
#if 0
			os << indent(2) << "<since: >" << std::endl;
			{ indent i(4); os << ta.since; }
			os << indent(2) << "<expires: >" << std::endl;
			{ indent i(4); os << ta.expires; }
#endif
		return os;
	};
};

}; /* namespace openflow */
}; /* namespace rofl */

#endif /* CTRANSACTION_H_ */
