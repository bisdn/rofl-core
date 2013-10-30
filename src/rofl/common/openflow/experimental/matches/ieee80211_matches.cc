#include "ieee80211_matches.h"

using namespace rofl;

uint16_t coxmatch_ofx_ieee80211_fc::get_fc_value() const{
	return uint16_value();
}
uint16_t coxmatch_ofx_ieee80211_fc::get_fc_mask() const{
	if(!get_oxm_hasmask())
		return 0xFFFF;
	return uint16_mask();
}

uint8_t coxmatch_ofx_ieee80211_type::get_type_value() const{
	return uint8_value();
}

uint8_t coxmatch_ofx_ieee80211_subtype::get_subtype_value() const{
	return uint8_value();
}

uint8_t coxmatch_ofx_ieee80211_direction::get_direction_value() const{
	return uint8_value();
}

cmacaddr coxmatch_ofx_ieee80211_address_1::get_mac_value() const{
	return u48addr();
}

cmacaddr coxmatch_ofx_ieee80211_address_1::get_mac_mask() const{
	if (!get_oxm_hasmask()) {
		return cmacaddr("ff:ff:ff:ff:ff:ff");
	}
	return cmacaddr(oxm_maddr->mask, OFP_ETH_ALEN);
}

cmacaddr coxmatch_ofx_ieee80211_address_2::get_mac_value() const{
	return u48addr();
}

cmacaddr coxmatch_ofx_ieee80211_address_2::get_mac_mask() const{
	if (!get_oxm_hasmask()) {
		return cmacaddr("ff:ff:ff:ff:ff:ff");
	}
	return cmacaddr(oxm_maddr->mask, OFP_ETH_ALEN);
}

cmacaddr coxmatch_ofx_ieee80211_address_3::get_mac_value() const{
	return u48addr();
}

cmacaddr coxmatch_ofx_ieee80211_address_3::get_mac_mask() const{
	if (!get_oxm_hasmask()) {
		return cmacaddr("ff:ff:ff:ff:ff:ff");
	}
	return cmacaddr(oxm_maddr->mask, OFP_ETH_ALEN);
}

