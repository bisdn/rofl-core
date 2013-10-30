#include "capwap_matches.h"

using namespace rofl;

uint8_t coxmatch_ofx_capwap_rid::get_rid_value() const{
	return uint8_value();
}

uint16_t coxmatch_ofx_capwap_flags::get_flags_value() const{
	return uint16_value();
}

uint16_t coxmatch_ofx_capwap_flags::get_flags_mask() const{
	if(!get_oxm_hasmask())
		return 0xFFFF;
	return uint16_mask();
}

uint8_t coxmatch_ofx_capwap_wbid::get_wbid_value() const{
	return uint8_value();
}
