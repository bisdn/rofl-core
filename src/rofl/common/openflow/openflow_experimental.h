/* Copyright (c) 2008 The Board of Trustees of The Leland Stanford
 * Junior University
 *
 * We are making the OpenFlow specification and associated documentation
 * (Software) available for public use and benefit with the expectation
 * that others will use, modify and enhance the Software and contribute
 * those enhancements back to the community. However, since we would
 * like to make the Software available for broadest use, with as few
 * restrictions as possible permission is hereby granted, free of
 * charge, to any person obtaining a copy of this Software to deal in
 * the Software under the copyrights without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * The name and trademarks of copyright holder(s) may NOT be used in
 * advertising or publicity pertaining to the Software or any
 * derivatives without specific, written prior permission.
 */

/* OpenFlow: protocol between controller and datapath. */

#ifndef _OPENFLOW_EXPERIMENTAL_H
#define _OPENFLOW_EXPERIMENTAL_H 1

#include "rofl/common/openflow/openflow_common.h"

namespace rofl {
namespace openflow {
namespace experimental {
	
	/* OXM Flow match field types for OpenFlow Experimental */
	enum oxm_ofx_match_fields {

		//OF1.0 backwards compatibility
		OFPXMT_OFX_NW_SRC	= 0,	/* network layer source address */
		OFPXMT_OFX_NW_DST	= 1,	/* network layer destination address */
		OFPXMT_OFX_NW_PROTO	= 2,	/* network layer proto/arp code... */
		OFPXMT_OFX_TP_SRC	= 3,	/* transport protocol source port */
		OFPXMT_OFX_TP_DST	= 4,	/* transport protocol destination port */

		/* Reserved (until 20) */

		/* PPP/PPPoE related extensions */
		OFPXMT_OFX_PPPOE_CODE 	= 21,	/* PPPoE code */
		OFPXMT_OFX_PPPOE_TYPE 	= 22,	/* PPPoE type */
		OFPXMT_OFX_PPPOE_SID 	= 23,	/* PPPoE session id */
		OFPXMT_OFX_PPP_PROT 	= 24,	/* PPP protocol */

		/* GTP related extensions */
		OFPXMT_OFX_GTP_MSG_TYPE = 25,	/* GTP message type */
		OFPXMT_OFX_GTP_TEID	= 26,	/* GTP tunnel endpoint identifier */

		/* max value */
		OFPXMT_OFX_MAX,
	};

}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* _OPENFLOW_EXPERIMENTAL_H */
