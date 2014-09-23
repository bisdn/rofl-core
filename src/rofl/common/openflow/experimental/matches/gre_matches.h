/*
 * gre_matches.h
 *
 *  Created on: 23.09.2014
 *      Author: andreas
 */

#ifndef GRE_MATCHES_H_
#define GRE_MATCHES_H_

#include <rofl/common/openflow/coxmatch.h>

namespace rofl {
namespace openflow {
namespace experimental {
namespace gre {

static uint16_t const OFPXMC_EXPERIMENTER = 0xffff;

/* OXM Flow match field types for OpenFlow Experimental */
enum oxm_ofx_match_fields {
	/* GRE related extensions */
	OFPXMT_OFX_GRE_VERSION  		= 37,	/* GRE version */
	OFPXMT_OFX_GRE_PROT_TYPE		= 38,	/* GRE protocol type */
	OFPXMT_OFX_GRE_KEY				= 39,	/* GRE key */
};

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_tlv_match_fields {
	OXM_TLV_EXPR_GRE_VERSION		= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GRE_VERSION << 9) |  2,
	OXM_TLV_EXPR_GRE_VERSION_MASK 	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GRE_VERSION << 9) |  4 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_GRE_PROT_TYPE		= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GRE_PROT_TYPE << 9) |  2,
	OXM_TLV_EXPR_GRE_PROT_TYPE_MASK = (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GRE_PROT_TYPE << 9) |  4 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_GRE_KEY			= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GRE_KEY    << 9) |  4,
	OXM_TLV_EXPR_GRE_KEY_MASK		= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_GRE_KEY    << 9) |  8 | HAS_MASK_FLAG,
};


/** OXM_OFX_GRE_VERSION
 *
 */
class coxmatch_ofx_gre_version : public coxmatch {
public:
	coxmatch_ofx_gre_version(
			uint16_t version) :
				coxmatch(OXM_TLV_EXPR_GRE_VERSION, version) {};
	coxmatch_ofx_gre_version(
			uint16_t version, uint16_t mask) :
				coxmatch(OXM_TLV_EXPR_GRE_VERSION_MASK, version, mask) {};
	coxmatch_ofx_gre_version(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_gre_version() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_gre_version const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<gre-version: "
						<< (int)oxm.get_u16value() << "/" << (int)oxm.get_u16mask()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OFX_GRE_PROT_TYPE
 *
 */
class coxmatch_ofx_gre_prot_type : public coxmatch {
public:
	coxmatch_ofx_gre_prot_type(
			uint16_t prot_type) :
				coxmatch(OXM_TLV_EXPR_GRE_PROT_TYPE, prot_type) {};
	coxmatch_ofx_gre_prot_type(
			uint16_t prot_type, uint16_t mask) :
				coxmatch(OXM_TLV_EXPR_GRE_PROT_TYPE_MASK, prot_type, mask) {};
	coxmatch_ofx_gre_prot_type(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_gre_prot_type() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_gre_prot_type const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<gre-prot-type: "
						<< (int)oxm.get_u16value() << "/" << (int)oxm.get_u16mask()
						<< " >" << std::endl;
		return os;
	};
};


/** OXM_OFX_GRE_KEY
 *
 */
class coxmatch_ofx_gre_key : public coxmatch {
public:
	coxmatch_ofx_gre_key(
			uint32_t key) :
				coxmatch(OXM_TLV_EXPR_GRE_KEY, key) {};
	coxmatch_ofx_gre_key(
			uint32_t key, uint32_t mask) :
				coxmatch(OXM_TLV_EXPR_GRE_KEY_MASK, key, mask) {};
	coxmatch_ofx_gre_key(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_gre_key() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_gre_key const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<gre-key: "
						<< (int)oxm.get_u32value() << "/" << (int)oxm.get_u32mask()
						<< " >" << std::endl;
		return os;
	};
};



}; // end of namespace gre
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif /* GRE_MATCHES_H_ */
