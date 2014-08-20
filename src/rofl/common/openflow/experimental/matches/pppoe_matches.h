#ifndef PPPOE_MATCHES
#define PPPOE_MATCHES 1

#include <rofl/common/openflow/coxmatch.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace pppoe {

static uint16_t const OFPXMC_EXPERIMENTER = 0xffff;

/* OXM Flow match field types for OpenFlow Experimental */
enum oxm_ofx_match_fields {
	/* PPP/PPPoE related extensions */
	OFPXMT_OFX_PPPOE_CODE 	= 21,	/* PPPoE code */
	OFPXMT_OFX_PPPOE_TYPE 	= 22,	/* PPPoE type */
	OFPXMT_OFX_PPPOE_SID 	= 23,	/* PPPoE session id */
	OFPXMT_OFX_PPP_PROT 	= 24,	/* PPP protocol */
};

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_tlv_match_fields {
	OXM_TLV_EXPR_PPPOE_CODE	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_PPPOE_CODE << 9) | 1,
	OXM_TLV_EXPR_PPPOE_TYPE	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_PPPOE_TYPE << 9) | 1,
	OXM_TLV_EXPR_PPPOE_SID	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_PPPOE_SID  << 9) | 2,
	OXM_TLV_EXPR_PPP_PROT	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_PPP_PROT   << 9) | 2,
};


/** OXM_OF_PPPOE_CODE
 *
 */
class coxmatch_ofx_pppoe_code : public coxmatch {
public:
	coxmatch_ofx_pppoe_code(
			uint8_t code) :
				coxmatch(OXM_TLV_EXPR_PPPOE_CODE, code) {};
	coxmatch_ofx_pppoe_code(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_pppoe_code() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_pppoe_code const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<pppoe-code: "
						<< (int)oxm.get_u8value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_PPPOE_TYPE
 *
 */
class coxmatch_ofx_pppoe_type : public coxmatch {
public:
	coxmatch_ofx_pppoe_type(
			uint8_t type) :
				coxmatch(OXM_TLV_EXPR_PPPOE_TYPE, type) {};
	coxmatch_ofx_pppoe_type(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_pppoe_type() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_pppoe_type const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<pppoe-type: "
						<< (int)oxm.get_u8value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_PPPOE_SID
 *
 */
class coxmatch_ofx_pppoe_sid : public coxmatch {
public:
	coxmatch_ofx_pppoe_sid(
			uint16_t sid) :
				coxmatch(OXM_TLV_EXPR_PPPOE_SID, sid) {};
	coxmatch_ofx_pppoe_sid(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_pppoe_sid() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_pppoe_sid const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<pppoe-sid: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OF_PPP_PROT
 *
 */
class coxmatch_ofx_ppp_prot : public coxmatch {
public:
	coxmatch_ofx_ppp_prot(
			uint16_t prot) :
				coxmatch(OXM_TLV_EXPR_PPP_PROT, prot) {};
	coxmatch_ofx_ppp_prot(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_ppp_prot() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_ppp_prot const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<ppp-prot: "
						<< (int)oxm.get_u16value()
						<< " >" << std::endl;
		return os;
	};
};

}; // end of namespace pppoe
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
