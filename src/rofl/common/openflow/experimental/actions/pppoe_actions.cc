#include <rofl/common/openflow/experimental/actions/pppoe_actions.h>

using namespace rofl;


cofaction_push_pppoe::cofaction_push_pppoe(
			uint16_t ethertype) :
				cofaction_experimenter(ROFL_EXPERIMENTER_ID, sizeof(struct ofp_action_push))
{
	eoac_action = (cofaction_experimenter::oac_experimenter_header)->data; // sets oac_push implicitely
	eoac_push->type = htobe16(OFXAT_PUSH_PPPOE);
	eoac_push->len = htobe16(sizeof(struct ofp_action_push));
	eoac_push->ethertype = htobe16(ethertype);
}



cofaction_push_pppoe::cofaction_push_pppoe(cofaction const& action) :
		cofaction_experimenter(action)
{
	if ((sizeof(struct ofp_action_experimenter_header) + sizeof(struct ofp_action_push)) <
			be16toh(action.oac_header->len))
		throw eBadActionBadLen();

	eoac_action = (cofaction_experimenter::oac_experimenter_header)->data;

	if (OFXAT_PUSH_PPPOE != eoac_push->type)
		throw eBadActionBadExperimenterType();
}



cofaction_push_pppoe::~cofaction_push_pppoe()
{

}



uint16_t
cofaction_push_pppoe::get_ethertype() const
{
	return be16toh(eoac_push->ethertype);
}



cofaction_pop_pppoe::cofaction_pop_pppoe(
		uint16_t ethertype) :
			cofaction_experimenter(ROFL_EXPERIMENTER_ID, sizeof(struct ofx_action_pop_pppoe))
{
	eoac_action = (cofaction_experimenter::oac_experimenter_header)->data;
	eoac_pop_pppoe->type = htobe16(OFXAT_POP_PPPOE);
	eoac_pop_pppoe->len = htobe16(sizeof(struct ofx_action_pop_pppoe));
	eoac_pop_pppoe->ethertype = htobe16(ethertype);
}




cofaction_pop_pppoe::cofaction_pop_pppoe(cofaction const& action) :
	cofaction_experimenter(action)
{
	if ((sizeof(struct ofp_action_experimenter_header) + sizeof(struct ofx_action_pop_pppoe)) <
			be16toh(action.oac_header->len))
		throw eBadActionBadLen();

	eoac_action = (cofaction_experimenter::oac_experimenter_header)->data;

	if (OFXAT_POP_PPPOE != eoac_pop_pppoe->type)
		throw eBadActionBadExperimenterType();
}



cofaction_pop_pppoe::~cofaction_pop_pppoe()
{

}



uint16_t
cofaction_pop_pppoe::get_ethertype() const
{
	return be16toh(eoac_pop_pppoe->ethertype);
}


