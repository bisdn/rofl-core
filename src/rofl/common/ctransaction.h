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
	cclock		since;		// time this transaction was created
	cclock		expires;	// time this transaction expires

public:

	/**
	 *
	 */
	ctransaction(uint32_t xid, cclock const& delta);

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

public:

	friend std::ostream&
	operator<< (std::ostream& os, ctransaction const& ta) {
		os << indent(0) << "<transaction ";
			os << "xid:" << (int)ta.xid << " >" << std::endl;
		os << indent(2) << "<since: >" << std::endl;
		{ indent i(4); os << ta.since; }
		os << indent(2) << "<expires: >" << std::endl;
		{ indent i(4); os << ta.expires; }
		return os;
	};
};

}; /* namespace openflow */
}; /* namespace rofl */

#endif /* CTRANSACTION_H_ */
