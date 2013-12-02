#include <rofl/common/openflow/experimental/matches/pppoe_matches.h>

using namespace rofl;


coxmatch_ofx_pppoe_code::coxmatch_ofx_pppoe_code(
		uint8_t code) :
			coxmatch(sizeof(struct openflow::ofp_oxm_hdr) + sizeof(uint8_t))
{
	set_oxm_class(openflow::OFPXMC_EXPERIMENTER);
	set_oxm_field(openflow::experimental::OFPXMT_OFX_PPPOE_CODE);
	set_oxm_length(sizeof(uint8_t));
	oxm_uint8t->byte = code;
}



coxmatch_ofx_pppoe_code::~coxmatch_ofx_pppoe_code()
{

}



coxmatch_ofx_pppoe_code::coxmatch_ofx_pppoe_code(
		coxmatch const& oxm) :
				coxmatch(oxm)
{
	if (openflow::OFPXMC_EXPERIMENTER != get_oxm_class())
		throw eOxmInvalClass();
	if (openflow::experimental::OFPXMT_OFX_PPPOE_CODE != get_oxm_field())
		throw eOxmInvalType();
}



uint8_t
coxmatch_ofx_pppoe_code::get_pppoe_code() const
{
	return u8value();
}



coxmatch_ofx_pppoe_type::coxmatch_ofx_pppoe_type(
		uint8_t type) :
			coxmatch(sizeof(struct openflow::ofp_oxm_hdr) + sizeof(uint8_t))
{
	set_oxm_class(openflow::OFPXMC_EXPERIMENTER);
	set_oxm_field(openflow::experimental::OFPXMT_OFX_PPPOE_TYPE);
	set_oxm_length(sizeof(uint8_t));
	oxm_uint8t->byte = type;
}



coxmatch_ofx_pppoe_type::~coxmatch_ofx_pppoe_type()
{

}



coxmatch_ofx_pppoe_type::coxmatch_ofx_pppoe_type(
		coxmatch const& oxm) :
				coxmatch(oxm)
{
	if (openflow::OFPXMC_EXPERIMENTER != get_oxm_class())
		throw eOxmInvalClass();
	if (openflow::experimental::OFPXMT_OFX_PPPOE_TYPE != get_oxm_field())
		throw eOxmInvalType();
}



uint8_t
coxmatch_ofx_pppoe_type::get_pppoe_type() const
{
	return u8value();
}



coxmatch_ofx_pppoe_sid::coxmatch_ofx_pppoe_sid(
		uint16_t sessid) :
			coxmatch(sizeof(struct openflow::ofp_oxm_hdr) + sizeof(uint16_t))
{
	set_oxm_class(openflow::OFPXMC_EXPERIMENTER);
	set_oxm_field(openflow::experimental::OFPXMT_OFX_PPPOE_SID);
	set_oxm_length(sizeof(uint16_t));
	oxm_uint16t->word = htobe16(sessid);
}



coxmatch_ofx_pppoe_sid::~coxmatch_ofx_pppoe_sid()
{

}



coxmatch_ofx_pppoe_sid::coxmatch_ofx_pppoe_sid(
		coxmatch const& oxm) :
				coxmatch(oxm)
{
	if (openflow::OFPXMC_EXPERIMENTER != get_oxm_class())
		throw eOxmInvalClass();
	if (openflow::experimental::OFPXMT_OFX_PPPOE_SID != get_oxm_field())
		throw eOxmInvalType();
}



uint16_t
coxmatch_ofx_pppoe_sid::get_pppoe_sid() const
{
	return u16value();
}




coxmatch_ofx_ppp_prot::coxmatch_ofx_ppp_prot(
		uint16_t prot) :
			coxmatch(sizeof(struct openflow::ofp_oxm_hdr) + sizeof(uint16_t))
{
	set_oxm_class(openflow::OFPXMC_EXPERIMENTER);
	set_oxm_field(openflow::experimental::OFPXMT_OFX_PPP_PROT);
	set_oxm_length(sizeof(uint16_t));
	oxm_uint16t->word = htobe16(prot);
}



coxmatch_ofx_ppp_prot::~coxmatch_ofx_ppp_prot()
{

}



coxmatch_ofx_ppp_prot::coxmatch_ofx_ppp_prot(
		coxmatch const& oxm) :
				coxmatch(oxm)
{
	if (openflow::OFPXMC_EXPERIMENTER != get_oxm_class())
		throw eOxmInvalClass();
	if (openflow::experimental::OFPXMT_OFX_PPP_PROT != get_oxm_field())
		throw eOxmInvalType();
}



uint16_t
coxmatch_ofx_ppp_prot::get_ppp_prot() const
{
	return u16value();
}



