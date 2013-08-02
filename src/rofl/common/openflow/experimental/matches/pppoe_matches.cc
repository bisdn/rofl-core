#include <rofl/common/openflow/experimental/matches/pppoe_matches.h>

using namespace rofl;


coxmatch_ofx_pppoe_code::coxmatch_ofx_pppoe_code(
		uint8_t code) :
			coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
{
	set_oxm_class(OFPXMC_EXPERIMENTER);
	set_oxm_field(OFPXMT_OFX_PPPOE_CODE);
	set_oxm_length(sizeof(uint8_t));
	oxm_uint8t->byte = code;
}



coxmatch_ofx_pppoe_code::~coxmatch_ofx_pppoe_code()
{

}



coxmatch_ofx_pppoe_type::coxmatch_ofx_pppoe_type(
		uint8_t type) :
			coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
{
	set_oxm_class(OFPXMC_EXPERIMENTER);
	set_oxm_field(OFPXMT_OFX_PPPOE_TYPE);
	set_oxm_length(sizeof(uint8_t));
	oxm_uint8t->byte = type;
}



coxmatch_ofx_pppoe_type::~coxmatch_ofx_pppoe_type()
{

}



coxmatch_ofx_pppoe_sid::coxmatch_ofx_pppoe_sid(
		uint16_t sessid) :
			coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
{
	set_oxm_class(OFPXMC_EXPERIMENTER);
	set_oxm_field(OFPXMT_OFX_PPPOE_SID);
	set_oxm_length(sizeof(uint16_t));
	oxm_uint16t->word = htobe16(sessid);
}



coxmatch_ofx_pppoe_sid::~coxmatch_ofx_pppoe_sid()
{

}




coxmatch_ofx_ppp_prot::coxmatch_ofx_ppp_prot(
		uint16_t prot) :
			coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
{
	set_oxm_class(OFPXMC_EXPERIMENTER);
	set_oxm_field(OFPXMT_OFX_PPP_PROT);
	set_oxm_length(sizeof(uint16_t));
	oxm_uint16t->word = htobe16(prot);
}



coxmatch_ofx_ppp_prot::~coxmatch_ofx_ppp_prot()
{

}




