#ifndef GTP_MATCHES_H
#define GTP_MATCHES_H 1

#include <rofl/common/openflow/coxmatch.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace gtp {

static uint16_t const OFPXMC_EXPERIMENTER = 0xffff;

/* OXM Flow match field types for OpenFlow Experimental */
enum oxm_ofx_match_fields {
	/* GTP related extensions */
	OFPXMT_OFX_GTP_MSGTYPE  		= 25,	/* GTP message type */
	OFPXMT_OFX_GTP_TEID				= 26,	/* GTP tunnel endpoint identifier */
};

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_tlv_match_fields {
	OXM_TLV_EXPR_GTP_MSGTYPE		= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GTP_MSGTYPE << 9) |  5,
	OXM_TLV_EXPR_GTP_MSGTYPE_MASK 	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GTP_MSGTYPE << 9) |  5 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_GTP_TEID			= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GTP_TEID    << 9) |  8,
	OXM_TLV_EXPR_GTP_TEID_MASK		= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GTP_TEID    << 9) | 12 | HAS_MASK_FLAG,
};


/** OXM_OFX_GTP_TEID
 *
 */
class coxmatch_ofx_gtp_teid : public coxmatch {
public:
	coxmatch_ofx_gtp_teid(
			uint32_t teid) :
				coxmatch(OXM_TLV_EXPR_GTP_TEID, teid) {};
	coxmatch_ofx_gtp_teid(
			uint32_t teid, uint32_t mask) :
				coxmatch(OXM_TLV_EXPR_GTP_TEID_MASK, teid, mask) {};
	coxmatch_ofx_gtp_teid(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_gtp_teid() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_gtp_teid const& oxm) {
		os << "OXM";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<GTP-TEID: " << (unsigned int)oxm.get_u32value();
			if (oxm.get_oxm_hasmask()) {
				os << ":" << (unsigned int)oxm.get_u32mask();
			}
			os << ">";
		return os;
	};
};


/** OXM_OFX_GTP_MSG_TYPE
 *
 */
class coxmatch_ofx_gtp_msg_type : public coxmatch {
public:
	coxmatch_ofx_gtp_msg_type(
			uint8_t msg_type) :
				coxmatch(OXM_TLV_EXPR_GTP_MSGTYPE, msg_type) {};
	coxmatch_ofx_gtp_msg_type(
			uint8_t msg_type, uint8_t mask) :
				coxmatch(OXM_TLV_EXPR_GTP_MSGTYPE_MASK, msg_type, mask) {};
	coxmatch_ofx_gtp_msg_type(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_gtp_msg_type() {};;
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_gtp_msg_type const& oxm) {
		os << "OXM";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<GTP-MSG-TYPE: " << (unsigned int)oxm.get_u8value() << ">";
		return os;
	};
};

}; // end of namespace gtp
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
