#ifndef WLAN_MATCHES_H
#define WLAN_MATCHES_H

#include "../../coxmatch.h"

namespace rofl {
namespace openflow {
namespace experimental {
namespace wlan {

static uint16_t const OFPXMC_EXPERIMENTER = 0xffff;

/* OXM Flow match field types for OpenFlow Experimental */
enum oxm_ofx_match_fields {
	/* IEEE 802.11 related extensions */
	OFPXMT_OFX_WLAN_FC			= 30,
	OFPXMT_OFX_WLAN_TYPE		= 31,
	OFPXMT_OFX_WLAN_SUBTYPE		= 32,
	OFPXMT_OFX_WLAN_DIRECTION	= 33,
	OFPXMT_OFX_WLAN_ADDRESS_1	= 34,
	OFPXMT_OFX_WLAN_ADDRESS_2	= 35,
	OFPXMT_OFX_WLAN_ADDRESS_3	= 36,
};

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_tlv_match_fields {
	OXM_TLV_EXPR_WLAN_FC				= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_FC         << 9) |   2,
	OXM_TLV_EXPR_WLAN_FC_MASK			= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_FC         << 9) |   4 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_WLAN_TYPE				= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_TYPE       << 9) |   1,
	OXM_TLV_EXPR_WLAN_TYPE_MASK			= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_TYPE       << 9) |   2 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_WLAN_SUBTYPE			= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_SUBTYPE    << 9) |   1,
	OXM_TLV_EXPR_WLAN_SUBTYPE_MASK		= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_SUBTYPE    << 9) |   2 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_WLAN_DIRECTION			= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_DIRECTION  << 9) |   1,
	OXM_TLV_EXPR_WLAN_DIRECTION_MASK	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_DIRECTION  << 9) |   2 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_WLAN_ADDRESS_1			= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_ADDRESS_1  << 9) |   6,
	OXM_TLV_EXPR_WLAN_ADDRESS_1_MASK	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_ADDRESS_1  << 9) |  12 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_WLAN_ADDRESS_2			= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_ADDRESS_2  << 9) |   6,
	OXM_TLV_EXPR_WLAN_ADDRESS_2_MASK	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_ADDRESS_2  << 9) |  12 | HAS_MASK_FLAG,
	OXM_TLV_EXPR_WLAN_ADDRESS_3			= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_ADDRESS_3  << 9) |   6,
	OXM_TLV_EXPR_WLAN_ADDRESS_3_MASK	= (OFPXMC_EXPERIMENTER << 16) | (OFPXMT_OFX_WLAN_ADDRESS_3  << 9) |  12 | HAS_MASK_FLAG,
};


/** OXM_OFX_WLAN_FC
 *
 */
class coxmatch_ofx_wlan_fc : public coxmatch {
public:
	coxmatch_ofx_wlan_fc(
			uint16_t fc) :
				coxmatch(OXM_TLV_EXPR_WLAN_FC, fc) {};
	coxmatch_ofx_wlan_fc(
			uint16_t fc, uint16_t mask) :
				coxmatch(OXM_TLV_EXPR_WLAN_FC_MASK, fc, mask) {};
	coxmatch_ofx_wlan_fc(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_wlan_fc() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_wlan_fc const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<wlan-fc: 0x" << std::hex
						<< (int)oxm.get_u16value() << "/0x" << (int)oxm.get_u16mask()
						<< std::dec << " >" << std::endl;
		return os;
	};
};



/** OXM_OFX_WLAN_TYPE
 *
 */
class coxmatch_ofx_wlan_type : public coxmatch {
public:
	coxmatch_ofx_wlan_type(
			uint8_t type) :
				coxmatch(OXM_TLV_EXPR_WLAN_TYPE, type) {};
	coxmatch_ofx_wlan_type(
			uint8_t type, uint8_t mask) :
				coxmatch(OXM_TLV_EXPR_WLAN_TYPE_MASK, type, mask) {};
	coxmatch_ofx_wlan_type(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_wlan_type() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_wlan_type const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<wlan-type: 0x" << std::hex
						<< (int)oxm.get_u8value() << "/0x" << (int)oxm.get_u8mask()
						<< std::dec << " >" << std::endl;
		return os;
	};
};



/** OXM_OFX_WLAN_SUBTYPE
 *
 */
class coxmatch_ofx_wlan_subtype : public coxmatch {
public:
	coxmatch_ofx_wlan_subtype(
			uint8_t subtype) :
				coxmatch(OXM_TLV_EXPR_WLAN_SUBTYPE, subtype) {};
	coxmatch_ofx_wlan_subtype(
			uint8_t subtype, uint8_t mask) :
				coxmatch(OXM_TLV_EXPR_WLAN_SUBTYPE_MASK, subtype, mask) {};
	coxmatch_ofx_wlan_subtype(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_wlan_subtype() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_wlan_subtype const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<wlan-subtype: 0x" << std::hex
						<< (int)oxm.get_u8value() << "/0x" << (int)oxm.get_u8mask()
						<< std::dec << " >" << std::endl;
		return os;
	};
};



/** OXM_OFX_WLAN_DIRECTION
 *
 */
class coxmatch_ofx_wlan_direction : public coxmatch {
public:
	coxmatch_ofx_wlan_direction(
			uint8_t direction) :
				coxmatch(OXM_TLV_EXPR_WLAN_DIRECTION, direction) {};
	coxmatch_ofx_wlan_direction(
			uint8_t direction, uint8_t mask) :
				coxmatch(OXM_TLV_EXPR_WLAN_DIRECTION_MASK, direction, mask) {};
	coxmatch_ofx_wlan_direction(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_wlan_direction() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_wlan_direction const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<wlan-direction: 0x" << std::hex
						<< (int)oxm.get_u8value() << "/0x" << (int)oxm.get_u8mask()
						<< std::dec << " >" << std::endl;
		return os;
	};
};



/** OXM_OFX_WLAN_ADDRESS_1
 *
 */
class coxmatch_ofx_wlan_address_1 : public coxmatch {
public:
	coxmatch_ofx_wlan_address_1(
			cmacaddr const& maddr) :
				coxmatch(OXM_TLV_EXPR_WLAN_ADDRESS_1, maddr) {};
	coxmatch_ofx_wlan_address_1(
			cmacaddr const& maddr, cmacaddr const& mask) :
				coxmatch(OXM_TLV_EXPR_WLAN_ADDRESS_1_MASK, maddr, mask) {};
	coxmatch_ofx_wlan_address_1(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_wlan_address_1() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_wlan_address_1 const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<wlan-address-1 value: >" << std::endl;
		{ indent i(4); os << oxm.get_u48value();  }
		os << indent(2) << "<wlan-address-1 mask: >" << std::endl;
		{ indent i(4); os << oxm.get_u48mask();  }
		return os;
	};
};



/** OXM_OFX_WLAN_ADDRESS_2
 *
 */
class coxmatch_ofx_wlan_address_2 : public coxmatch {
public:
	coxmatch_ofx_wlan_address_2(
			cmacaddr const& maddr) :
				coxmatch(OXM_TLV_EXPR_WLAN_ADDRESS_2, maddr) {};
	coxmatch_ofx_wlan_address_2(
			cmacaddr const& maddr, cmacaddr const& mask) :
				coxmatch(OXM_TLV_EXPR_WLAN_ADDRESS_2_MASK, maddr, mask) {};
	coxmatch_ofx_wlan_address_2(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_wlan_address_2() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_wlan_address_2 const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<wlan-address-2 value: >" << std::endl;
		{ indent i(4); os << oxm.get_u48value();  }
		os << indent(2) << "<wlan-address-2 mask: >" << std::endl;
		{ indent i(4); os << oxm.get_u48mask();  }
		return os;
	};
};



/** OXM_OFX_WLAN_ADDRESS_3
 *
 */
class coxmatch_ofx_wlan_address_3 : public coxmatch {
public:
	coxmatch_ofx_wlan_address_3(
			cmacaddr const& maddr) :
				coxmatch(OXM_TLV_EXPR_WLAN_ADDRESS_3, maddr) {};
	coxmatch_ofx_wlan_address_3(
			cmacaddr const& maddr, cmacaddr const& mask) :
				coxmatch(OXM_TLV_EXPR_WLAN_ADDRESS_3_MASK, maddr, mask) {};
	coxmatch_ofx_wlan_address_3(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	virtual
	~coxmatch_ofx_wlan_address_3() {};
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_wlan_address_3 const& oxm) {
		os << dynamic_cast<coxmatch const&>(oxm);
		os << indent(2) << "<wlan-address-3 value: >" << std::endl;
		{ indent i(4); os << oxm.get_u48value();  }
		os << indent(2) << "<wlan-address-3 mask: >" << std::endl;
		{ indent i(4); os << oxm.get_u48mask();  }
		return os;
	};
};

}; // end of namespace wlan
}; // end of namespace experimental
}; // end of namespace openflow
}; // end of namespace rofl

#endif
