#ifndef IEEE80211_MATCHES_H
#define IEEE80211_MATCHES_H 

#include "../../coxmatch.h"

namespace rofl
{


/** OXM_OF_IEEE80211_FC
 *
 */
class coxmatch_ofx_ieee80211_fc :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_ieee80211_fc(
			uint16_t fc) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_FC);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(fc);
	};

	/** constructor
	 */
	coxmatch_ofx_ieee80211_fc(
			coxmatch const& oxm):
			coxmatch(oxm){};

	/** constructor
	 */
	coxmatch_ofx_ieee80211_fc(
			uint16_t fc,
			uint16_t mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_FC);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint16_t));
		oxm_uint16t->word = htobe16(fc);
		oxm_uint16t->mask = htobe16(mask);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_ieee80211_fc() {};

	uint16_t
	get_fc_value(void) const;
	
	uint16_t
	get_fc_mask(void) const;
	
};

/** OXM_OF_IEEE80211_TYPE
 *
 */
class coxmatch_ofx_ieee80211_type :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_ieee80211_type(
			uint8_t type) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_TYPE);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = type;
	};

	/** constructor
	 */
	coxmatch_ofx_ieee80211_type(
			coxmatch const& oxm):
			coxmatch(oxm){};

	/** destructor
	 */
	virtual
	~coxmatch_ofx_ieee80211_type() {};

	uint8_t
	get_type_value(void) const;
};

/** OXM_OF_IEEE80211_SUBTYPE
 *
 */
class coxmatch_ofx_ieee80211_subtype :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_ieee80211_subtype(
			uint8_t subtype) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_SUBTYPE);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = subtype;
	};

	/** constructor
	 */
	coxmatch_ofx_ieee80211_subtype(
			coxmatch const& oxm):
			coxmatch(oxm){};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_ieee80211_subtype() {};

	uint8_t
	get_subtype_value(void) const;
};

/** OXM_OF_IEEE80211_DIRECTION
 *
 */
class coxmatch_ofx_ieee80211_direction :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_ieee80211_direction(
			uint8_t direction) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_DIRECTION);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = direction;
	};

	/** constructor
	 */
	coxmatch_ofx_ieee80211_direction(
			coxmatch const& oxm):
			coxmatch(oxm){};

	/** destructor
	 */
	virtual
	~coxmatch_ofx_ieee80211_direction() {};

	uint8_t
	get_direction_value(void) const;
	
};

/** OXM_OF_IEEE80211_ADDRESS_1
 *
 */
class coxmatch_ofx_ieee80211_address_1 :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_ieee80211_address_1(
			cmacaddr const& maddr) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_ADDRESS_1);
		set_oxm_length(OFP_ETH_ALEN);
		memcpy(oxm_maddr->addr, maddr.somem(), OFP_ETH_ALEN);
	};
	/** constructor
	 */
	coxmatch_ofx_ieee80211_address_1(
			cmacaddr const& maddr,
			cmacaddr const& mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_ADDRESS_1);
		set_oxm_hasmask(true);
		set_oxm_length(2 * OFP_ETH_ALEN);
		memcpy(oxm_maddr->addr, maddr.somem(), OFP_ETH_ALEN);
		memcpy(oxm_maddr->mask, mask.somem(), OFP_ETH_ALEN);
	};

	/** constructor
	 */
	coxmatch_ofx_ieee80211_address_1(
			coxmatch const& oxm):
			coxmatch(oxm){};

	/** destructor
	 */
	virtual
	~coxmatch_ofx_ieee80211_address_1() {};

	cmacaddr
	get_mac_value(void) const;
	
	cmacaddr
	get_mac_mask(void) const;

};

/** OXM_OF_IEEE80211_ADDRESS_2
 *
 */
class coxmatch_ofx_ieee80211_address_2 :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_ieee80211_address_2(
			cmacaddr const& maddr) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_ADDRESS_2);
		set_oxm_length(OFP_ETH_ALEN);
		memcpy(oxm_maddr->addr, maddr.somem(), OFP_ETH_ALEN);
	};
	/** constructor
	 */
	coxmatch_ofx_ieee80211_address_2(
			cmacaddr const& maddr,
			cmacaddr const& mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_ADDRESS_2);
		set_oxm_hasmask(true);
		set_oxm_length(2 * OFP_ETH_ALEN);
		memcpy(oxm_maddr->addr, maddr.somem(), OFP_ETH_ALEN);
		memcpy(oxm_maddr->mask, mask.somem(), OFP_ETH_ALEN);
	};

	/** constructor
	 */
	coxmatch_ofx_ieee80211_address_2(
			coxmatch const& oxm):
			coxmatch(oxm){};

	/** destructor
	 */
	virtual
	~coxmatch_ofx_ieee80211_address_2() {};

	cmacaddr
	get_mac_value(void) const;
	
	cmacaddr
	get_mac_mask(void) const;


};

/** OXM_OF_IEEE80211_ADDRESS_3
 *
 */
class coxmatch_ofx_ieee80211_address_3 :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_ieee80211_address_3(
			cmacaddr const& maddr) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_ADDRESS_3);
		set_oxm_length(OFP_ETH_ALEN);
		memcpy(oxm_maddr->addr, maddr.somem(), OFP_ETH_ALEN);
	};
	/** constructor
	 */
	coxmatch_ofx_ieee80211_address_3(
			cmacaddr const& maddr,
			cmacaddr const& mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * OFP_ETH_ALEN)
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_IEEE80211_ADDRESS_3);
		set_oxm_hasmask(true);
		set_oxm_length(2 * OFP_ETH_ALEN);
		memcpy(oxm_maddr->addr, maddr.somem(), OFP_ETH_ALEN);
		memcpy(oxm_maddr->mask, mask.somem(), OFP_ETH_ALEN);
	};

	/** constructor
	 */
	coxmatch_ofx_ieee80211_address_3(
			coxmatch const& oxm):
			coxmatch(oxm){};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_ieee80211_address_3() {};

	cmacaddr
	get_mac_value(void) const;
	
	cmacaddr
	get_mac_mask(void) const;

};

}; // end of namespace

#endif
