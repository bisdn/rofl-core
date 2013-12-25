/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cofaction.h"

using namespace rofl;

cofaction::cofaction(uint8_t ofp_version, size_t datalen) :
			ofp_version(ofp_version),
			action(datalen)
{
	oac_header = (struct openflow::ofp_action_header*)action.somem();
}


cofaction::cofaction(uint8_t ofp_version, struct openflow::ofp_action_header* achdr, size_t aclen) :
			ofp_version(ofp_version),
			action(aclen)
{
	oac_header = (struct openflow::ofp_action_header*)action.somem();

	if (be16toh(oac_header->len) > aclen) {
		throw eBadActionBadLen();
	}

	unpack((uint8_t*)achdr, aclen);
}


cofaction::~cofaction()
{

}


cofaction::cofaction(cofaction const& action)
{
	*this = action;
}


cofaction&
cofaction::operator= (const cofaction& ac)
{
	if (this == &ac)
		return *this;

	this->ofp_version 	= ac.ofp_version;
	this->action 		= ac.action;
	this->oac_generic 	= this->action.somem();

	return *this;
}


uint8_t*
cofaction::pack(
		uint8_t* achdr,
		size_t aclen)
{
	if (aclen < this->length())
		throw eActionInval();

	oac_header->len = htobe16(length());

	memcpy((uint8_t*)achdr, (uint8_t*)soaction(), length());

	return achdr;
}


void
cofaction::unpack(uint8_t* achdr, size_t aclen)
{
	if (action.memlen() < aclen) {
		oac_generic = action.resize(aclen);;
	}

	memcpy(oac_generic, achdr, aclen);

	if (be16toh(oac_header->len) < sizeof(struct openflow::ofp_action_header)) {
		throw eBadActionBadLen();
	}

	switch (ofp_version) {
	case openflow10::OFP_VERSION: {
		unpack_of10(achdr, aclen);
	} break;
	case openflow12::OFP_VERSION: {
		unpack_of12(achdr, aclen);
	} break;
	case openflow13::OFP_VERSION: {
		unpack_of13(achdr, aclen);
	} break;
	}
}



void
cofaction::unpack_of10(uint8_t* buf, size_t buflen)
{
	/*
	 * OpenFlow 1.0
	 */

	switch (be16toh(oac_header->type)) {
	case openflow10::OFPAT_OUTPUT: {
		//oac_output = (struct openflow10::ofp_action_output*)oac_header;
		if (action.memlen() < sizeof(struct openflow10::ofp_action_output)) {
			throw eBadActionBadLen();
		}
		uint16_t port_no = be16toh(oac_10output->port);
		if (0 == port_no) {
			throw eBadActionBadOutPort();
		}
	} break;
	case openflow10::OFPAT_SET_VLAN_VID: {
		//oac_vlanvid = (struct openflow10::ofp_action_vlan_vid*)oac_header;
		if (action.memlen() < sizeof(struct openflow10::ofp_action_vlan_vid)) {
			throw eBadActionBadLen();
		}
	} break;
	case openflow10::OFPAT_SET_VLAN_PCP: {
		//oac_vlanpcp = (struct openflow10::ofp_action_vlan_pcp*)oac_header;
		if (action.memlen() < sizeof(struct openflow10::ofp_action_vlan_pcp)) {
			throw eBadActionBadLen();
		}
	} break;
	case openflow10::OFPAT_STRIP_VLAN: {
		if (action.memlen() < sizeof(struct openflow::ofp_action_header)) {
			throw eBadActionBadLen();
		}
	} break;
	case openflow10::OFPAT_SET_DL_SRC:
	case openflow10::OFPAT_SET_DL_DST: {
		//oac_dladdr = (struct openflow10::ofp_action_dl_addr*)oac_header;
		if (action.memlen() < sizeof(struct openflow10::ofp_action_dl_addr)) {
			throw eBadActionBadLen();
		}
	} break;
	case openflow10::OFPAT_SET_NW_SRC:
	case openflow10::OFPAT_SET_NW_DST: {
		//oac_nwaddr = (struct openflow10::ofp_action_nw_addr*)oac_header;
		if (action.memlen() < sizeof(struct openflow10::ofp_action_nw_addr)) {
			throw eBadActionBadLen();
		}
	} break;
	case openflow10::OFPAT_SET_NW_TOS: {
		//oac_nwtos = (struct openflow10::ofp_action_nw_tos*)oac_header;
		if (action.memlen() < sizeof(struct openflow10::ofp_action_nw_tos)) {
			throw eBadActionBadLen();
		}
	} break;
	case openflow10::OFPAT_SET_TP_SRC:
	case openflow10::OFPAT_SET_TP_DST: {
		//oac_tpport = (struct openflow10::ofp_action_tp_port*)oac_header;
		if (action.memlen() < sizeof(struct openflow10::ofp_action_tp_port)) {
			throw eBadActionBadLen();
		}
	} break;
	case openflow10::OFPAT_ENQUEUE: {
		if (action.memlen() < sizeof(struct openflow10::ofp_action_enqueue)) {
			throw eBadActionBadLen();
		}
	} break;
	case openflow10::OFPAT_VENDOR: {
		//oac_10vendor = (struct openflow10::ofp_action_vendor_header*)oac_header;
		if (action.memlen() < sizeof(struct openflow10::ofp_action_vendor_header)) {
			throw eBadActionBadLen();
		}
	} break;
	default: {
		throw eBadActionBadType();
	} break;
	}
}



void
cofaction::unpack_of12(uint8_t *buf, size_t buflen)
{
	/*
	 * OpenFlow 1.2
	 */
	switch (be16toh(oac_header->type)) {
	case openflow12::OFPAT_OUTPUT: {
		if (action.memlen() < sizeof(struct openflow12::ofp_action_output)) {
			throw eBadActionBadLen();
		}
		uint32_t port_no = be32toh(oac_12output->port);
		if ((openflow12::OFPP_ANY == port_no) || (0 == port_no)) {
			throw eBadActionBadOutPort();
		}
	} break;
	case openflow12::OFPAT_SET_FIELD:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_set_field)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_COPY_TTL_OUT:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_header)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_COPY_TTL_IN:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_header)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_SET_MPLS_TTL:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_mpls_ttl)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_DEC_MPLS_TTL:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_push)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_PUSH_VLAN:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_push)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_POP_VLAN:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_header)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_PUSH_MPLS:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_push)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_POP_MPLS:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_pop_mpls)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_SET_QUEUE:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_set_queue)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_GROUP:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_group)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_SET_NW_TTL:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_nw_ttl)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_DEC_NW_TTL:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_header)) {
			throw eBadActionBadLen();
		}
		break;
	case openflow12::OFPAT_EXPERIMENTER:
		if (action.memlen() < sizeof(struct openflow12::ofp_action_experimenter_header)) {
			throw eBadActionBadLen();
		}
		break;
	default:
		throw eBadActionBadType();
		break;
	}
}



void
cofaction::unpack_of13(uint8_t *buf, size_t buflen)
{
	/*
	 * OpenFlow 1.2
	 */
	unpack_of12(buf, buflen);
}




struct openflow::ofp_action_header*
cofaction::soaction() const
{
	return (struct openflow::ofp_action_header*)(action.somem());
}	


size_t
cofaction::length() const
{
	switch (ofp_version) {
	case openflow10::OFP_VERSION: {

		switch (be16toh(oac_header->type)) {
		case openflow10::OFPAT_OUTPUT:
			return sizeof(struct openflow10::ofp_action_output);

		case openflow10::OFPAT_SET_VLAN_VID:
			return sizeof(struct openflow10::ofp_action_vlan_vid);

		case openflow10::OFPAT_SET_VLAN_PCP:
			return sizeof(struct openflow10::ofp_action_vlan_pcp);

		case openflow10::OFPAT_STRIP_VLAN:
			return sizeof(struct openflow10::ofp_action_header);

		case openflow10::OFPAT_SET_DL_SRC:
		case openflow10::OFPAT_SET_DL_DST:
			return sizeof(struct openflow10::ofp_action_dl_addr);

		case openflow10::OFPAT_SET_NW_SRC:
		case openflow10::OFPAT_SET_NW_DST:
			return sizeof(struct openflow10::ofp_action_nw_addr);

		case openflow10::OFPAT_SET_TP_SRC:
		case openflow10::OFPAT_SET_TP_DST:
			return sizeof(struct openflow10::ofp_action_tp_port);

		case openflow10::OFPAT_ENQUEUE:
			return sizeof(struct openflow10::ofp_action_enqueue);

		case openflow10::OFPAT_VENDOR:
			return action.memlen();

		default:
			return action.memlen();
		}

	} break;
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {

		switch (be16toh(oac_header->type)) {
		case openflow12::OFPAT_OUTPUT:
			return sizeof(struct openflow12::ofp_action_output);

		case openflow12::OFPAT_COPY_TTL_OUT:
			return sizeof(openflow12::ofp_action_header);

		case openflow12::OFPAT_COPY_TTL_IN:
			return sizeof(openflow12::ofp_action_header);

		case openflow12::OFPAT_SET_MPLS_TTL:
			return sizeof(struct openflow12::ofp_action_mpls_ttl);

		case openflow12::OFPAT_DEC_MPLS_TTL:
			return sizeof(openflow12::ofp_action_header);

		case openflow12::OFPAT_PUSH_VLAN:
			return sizeof(struct openflow12::ofp_action_push);

		case openflow12::OFPAT_POP_VLAN:
			return sizeof(openflow12::ofp_action_header);

		case openflow12::OFPAT_PUSH_MPLS:
			return sizeof(struct openflow12::ofp_action_push);

		case openflow12::OFPAT_POP_MPLS:
			return sizeof(openflow12::ofp_action_pop_mpls);

		case openflow12::OFPAT_SET_QUEUE:
			return sizeof(struct openflow12::ofp_action_set_queue);

		case openflow12::OFPAT_GROUP:
			return sizeof(struct openflow12::ofp_action_group);

		case openflow12::OFPAT_SET_NW_TTL:
			return sizeof(struct openflow12::ofp_action_nw_ttl);

		case openflow12::OFPAT_DEC_NW_TTL:
			return sizeof(openflow12::ofp_action_header);

		case openflow12::OFPAT_EXPERIMENTER:
			return sizeof(struct openflow12::ofp_action_experimenter_header);

		case openflow12::OFPAT_SET_FIELD:
			return action.memlen();

		default:
			return action.memlen();
		}

	} break;
	default:
		throw eBadVersion();
	}
}



uint8_t
cofaction::get_version() const
{
	return ofp_version;
}


uint16_t
cofaction::get_type() const
{
	return be16toh(oac_header->type);
}



void
cofaction::resize(size_t len)
{
	action.resize(len);
	oac_generic = action.somem();
}



uint32_t
cofaction_output::get_port() const
{
	switch (ofp_version) {
	case openflow10::OFP_VERSION: {
		if (openflow10::OFPAT_OUTPUT != get_type())
			throw eActionInvalType();
		return be16toh(oac_10output->port);
	} break;
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		if (openflow12::OFPAT_OUTPUT != get_type())
			throw eActionInvalType();
		return be32toh(oac_12output->port);
	} break;
	default:
		throw eActionInvalType();
	}
}

void
cofaction_output::set_max_len(uint16_t max_len)
{
	switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			if (openflow10::OFPAT_OUTPUT != get_type())
				throw eActionInvalType();
			oac_10output->max_len = htobe16(max_len);
		} break;
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {
			if (openflow12::OFPAT_OUTPUT != get_type())
				throw eActionInvalType();
			oac_12output->max_len = htobe16(max_len);
		} break;
		default:
			throw eActionInvalType();
	}
}
uint16_t
cofaction_output::get_max_len() const
{
	switch (ofp_version) {
	case openflow10::OFP_VERSION: {
		if (openflow10::OFPAT_OUTPUT != get_type())
			throw eActionInvalType();
		return be16toh(oac_10output->max_len);
	} break;
	case openflow12::OFP_VERSION:
	case openflow13::OFP_VERSION: {
		if (openflow12::OFPAT_OUTPUT != get_type())
			throw eActionInvalType();
		return be16toh(oac_12output->max_len);
	} break;
	default:
		throw eActionInvalType();
	}
}


coxmatch
cofaction::get_oxm() throw (eActionInvalType)
{
	if (openflow12::OFPAT_SET_FIELD != get_type())
	{
		throw eActionInvalType();
	}

	struct openflow::ofp_oxm_hdr* oxm_hdr = (struct openflow::ofp_oxm_hdr*)oac_12set_field->field;
	size_t oxm_len = be16toh(oac_12set_field->len) - 2 * sizeof(uint16_t); // without action header

	coxmatch oxm(oxm_hdr, oxm_len);

	return oxm;
}

template class coflist<cofaction>;
