#ifndef CAPWAP_MATCHES_H
#define CAPWAP_MATCHES_H 

#include "../../coxmatch.h"

namespace rofl
{

/** OXM_OF_CAPWAP_RID
 *
 */
class coxmatch_ofx_capwap_rid :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_capwap_rid(
			uint8_t rid) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_CAPWAP_RID);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = rid;
	};

	/** constructor
	 */
	coxmatch_ofx_capwap_rid(
			coxmatch const& oxm):
			coxmatch(oxm){};


	/** destructor
	 */
	virtual
	~coxmatch_ofx_capwap_rid() {};
	
		/**
	 */
	uint8_t
	get_rid_value(void) const;
	
};

/** OXM_OF_CAPWAP_FLAGS
 *
 */
class coxmatch_ofx_capwap_flags :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_capwap_flags(
			uint16_t flags) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_CAPWAP_FLAGS);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(flags);
	};
	/** constructor
	 */
	coxmatch_ofx_capwap_flags(
			uint16_t flags,
			uint16_t mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_CAPWAP_FLAGS);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint16_t));
		oxm_uint16t->word = htobe16(flags);
		oxm_uint16t->mask = htobe16(mask);
	};

	/** constructor
	 */
	coxmatch_ofx_capwap_flags(
			coxmatch const& oxm):
			coxmatch(oxm){};


	/** destructor
	 */
	virtual
	~coxmatch_ofx_capwap_flags() {};

	uint16_t
	get_flags_value(void) const;
	
	uint16_t
	get_flags_mask(void) const;
	
};

/** OXM_OF_CAPWAP_WBID
 *
 */
class coxmatch_ofx_capwap_wbid :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_capwap_wbid(
			uint8_t wbid) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_CAPWAP_WBID);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = wbid;
	};

	/** constructor
	 */
	coxmatch_ofx_capwap_wbid(
			coxmatch const& oxm):
			coxmatch(oxm){};


	/** destructor
	 */
	virtual
	~coxmatch_ofx_capwap_wbid() {};


	uint8_t
	get_wbid_value(void) const;
	
};

}; // end of namespace

#endif
