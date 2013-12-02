#include <rofl/common/openflow/experimental/matches/gtp_matches.h>

using namespace rofl;


coxmatch_ofx_gtp_teid::coxmatch_ofx_gtp_teid(
		uint32_t teid) :
			coxmatch(sizeof(struct openflow::ofp_oxm_hdr) + sizeof(uint32_t))
{
	set_oxm_class(openflow::OFPXMC_EXPERIMENTER);
	set_oxm_field(openflow::experimental::OFPXMT_OFX_GTP_TEID);
	set_oxm_length(sizeof(uint32_t));
	set_teid(teid);
}



coxmatch_ofx_gtp_teid::coxmatch_ofx_gtp_teid(
		uint32_t teid,
		uint32_t mask) :
			coxmatch(sizeof(struct openflow::ofp_oxm_hdr) + 2 * sizeof(uint32_t))
{
	set_oxm_class(openflow::OFPXMC_EXPERIMENTER);
	set_oxm_field(openflow::experimental::OFPXMT_OFX_GTP_TEID);
	set_oxm_hasmask(true);
	set_oxm_length(2 * sizeof(uint32_t));
	set_teid(teid, mask);
}



coxmatch_ofx_gtp_teid::coxmatch_ofx_gtp_teid(
		coxmatch const& oxm) :
			coxmatch(oxm)
{

}



coxmatch_ofx_gtp_teid::~coxmatch_ofx_gtp_teid()
{

}



uint32_t
coxmatch_ofx_gtp_teid::get_teid() const
{
	return u32value();
}



uint32_t
coxmatch_ofx_gtp_teid::get_teid_value() const
{
	return uint32_value();
}



uint32_t
coxmatch_ofx_gtp_teid::get_teid_mask() const
{
	return uint32_mask();
}



void
coxmatch_ofx_gtp_teid::set_teid(uint32_t teid, uint32_t mask)
{
	oxm_uint32t->dword = htobe32(teid);
	if (mask != 0xffffffff) {
		if (not get_oxm_hasmask())
			throw eOxmInval();
		oxm_uint32t->mask = htobe32(mask);
	}
}







coxmatch_ofx_gtp_msg_type::coxmatch_ofx_gtp_msg_type(
		uint8_t msg_type) :
			coxmatch(sizeof(struct openflow::ofp_oxm_hdr) + sizeof(uint8_t))
{
	set_oxm_class(openflow::OFPXMC_EXPERIMENTER);
	set_oxm_field(openflow::experimental::OFPXMT_OFX_GTP_MSG_TYPE);
	set_oxm_length(sizeof(uint8_t));
	set_msg_type(msg_type);
}



coxmatch_ofx_gtp_msg_type::coxmatch_ofx_gtp_msg_type(
		coxmatch const& oxm) :
			coxmatch(oxm)
{

}



coxmatch_ofx_gtp_msg_type::~coxmatch_ofx_gtp_msg_type()
{

}



uint8_t
coxmatch_ofx_gtp_msg_type::get_msg_type() const
{
	return u8value();
}



void
coxmatch_ofx_gtp_msg_type::set_msg_type(uint8_t msg_type)
{
	oxm_uint8t->byte = msg_type;
}


