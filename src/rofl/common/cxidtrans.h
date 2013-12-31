/*
 * cxidtrans.h
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#ifndef CXIDTRANS_H_
#define CXIDTRANS_H_ 1

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

namespace rofl {

namespace openflow {

class cxidtrans {
	uint32_t xid;
public:
	cxidtrans(uint32_t xid) : xid(xid) {};


};

}; /* namespace openflow */

}; /* namespace rofl */




#endif /* CXIDTRANS_H_ */
