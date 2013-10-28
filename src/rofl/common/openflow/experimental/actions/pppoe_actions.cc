#include "pppoe_actions.h"

using namespace rofl;


cofaction_push_pppoe::cofaction_push_pppoe(
		uint8_t ofp_version,
		uint16_t ethertype) :
				cofaction_experimenter(ofp_version, ROFL_EXPERIMENTER_ID, OFXAT_PUSH_PPPOE, sizeof(struct ofp12_action_push))
{
	switch (ofp_version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		eoac_header = cofaction_experimenter::soaction();

		eoac_push_pppoe->expbody.type		= htobe16(OFXAT_PUSH_PPPOE);
		eoac_push_pppoe->expbody.len		= htobe16(sizeof(struct ofp12_action_push));
		eoac_push_pppoe->expbody.ethertype 	= htobe16(ethertype);
	} break;
	}
}



cofaction_push_pppoe::cofaction_push_pppoe(cofaction const& action) :
		cofaction_experimenter(action)
{
	if ((sizeof(struct ofp12_action_experimenter_header) + sizeof(struct ofp12_action_push)) <
			be16toh(action.oac_header->len))
		throw eBadActionBadLen();

	cofaction::operator= (action);

	eoac_header = soaction();

	if (OFXAT_PUSH_PPPOE != be32toh(eoac_push_pppoe->exphdr.exp_type))
		throw eBadActionBadExperimenterType();
}



cofaction_push_pppoe::~cofaction_push_pppoe()
{

}



uint16_t
cofaction_push_pppoe::get_ethertype() const
{
	return be16toh(eoac_push_pppoe->expbody.ethertype);
}



cofaction_pop_pppoe::cofaction_pop_pppoe(
		uint8_t ofp_version,
		uint16_t ethertype) :
			cofaction_experimenter(ofp_version, ROFL_EXPERIMENTER_ID, OFXAT_POP_PPPOE, sizeof(struct ofx_action_pop_pppoe))
{
	switch (ofp_version) {
	case OFP12_VERSION:
	case OFP13_VERSION: {
		eoac_header = cofaction_experimenter::soaction();

		eoac_pop_pppoe->expbody.type		= htobe16(OFXAT_POP_PPPOE);
		eoac_pop_pppoe->expbody.len			= htobe16(sizeof(struct ofx_action_pop_pppoe_header));
		eoac_pop_pppoe->expbody.ethertype 	= htobe16(ethertype);
	} break;
	}
}




cofaction_pop_pppoe::cofaction_pop_pppoe(cofaction const& action) :
	cofaction_experimenter(action)
{
	if ((sizeof(struct ofp12_action_experimenter_header) + sizeof(struct ofx_action_pop_pppoe)) <
			be16toh(action.oac_header->len))
		throw eBadActionBadLen();

	cofaction::operator= (action);

	eoac_header = soaction();

	if (OFXAT_POP_PPPOE != be32toh(eoac_pop_pppoe->exphdr.exp_type))
		throw eBadActionBadExperimenterType();
}



cofaction_pop_pppoe::~cofaction_pop_pppoe()
{

}



uint16_t
cofaction_pop_pppoe::get_ethertype() const
{
	return be16toh(eoac_pop_pppoe->expbody.ethertype);
}


