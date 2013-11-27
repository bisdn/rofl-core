/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cofaction.h"

using namespace rofl;

cofaction::cofaction(
		uint8_t ofp_version,
		size_t datalen) :
			ofp_version(ofp_version),
			action(datalen)
{
	WRITELOG(COFACTION, DBG, "cofaction(%p)::cofaction()", this);
	oac_header = (struct ofp_action_header*)action.somem();
}


cofaction::cofaction(
		uint8_t ofp_version,
		struct ofp_action_header* achdr,
		size_t aclen) throw (eBadActionBadLen, eBadActionBadOutPort) :
				ofp_version(ofp_version),
				action(aclen)
{
	WRITELOG(COFACTION, DBG, "cofaction(%p)::cofaction()", this);
	oac_header = (struct ofp_action_header*)action.somem();

	if (be16toh(oac_header->len) > aclen) {
		throw eBadActionBadLen();
	}

	unpack(achdr, aclen);

#if 0
	__make_info();
#endif
}


cofaction::~cofaction()
{
	WRITELOG(COFACTION, DBG, "cofaction(%p)::~cofaction()", this);
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
	this->oac_header = (struct ofp_action_header*)this->action.somem();

	return *this;
}


struct ofp_action_header*
cofaction::pack(
		struct ofp_action_header* achdr,
		size_t aclen) const throw (eActionInval)
{
	if (aclen < this->length())
		throw eActionInval();

	//WRITELOG(CSYSLOG_ACTION, "soaction()=%p actionlen()=%d", soaction(), actionlen());

	oac_header->len = htobe16(length());

	memcpy((uint8_t*)achdr, (uint8_t*)soaction(), length());
	return achdr;
}


void
cofaction::unpack(
		struct ofp_action_header *achdr,
		size_t aclen) throw (eBadActionBadLen, eBadActionBadOutPort, eBadActionBadType)
{
	if (action.memlen() < aclen) {
		oac_header = (struct ofp_action_header*)action.resize(aclen);;
	}

	memcpy((uint8_t*)oac_header, (uint8_t*)achdr, aclen);

	if (be16toh(oac_header->len) < sizeof(struct ofp_action_header)) {
		throw eBadActionBadLen();
	}

	switch (ofp_version) {
	case OFP10_VERSION: {
		unpack((struct ofp10_action_header*)achdr, aclen);
	} break;
	case OFP12_VERSION: {
		unpack((struct ofp12_action_header*)achdr, aclen);
	} break;
	case OFP13_VERSION: {
		unpack((struct ofp13_action_header*)achdr, aclen);
	} break;
	}
}



void
cofaction::unpack(
		struct ofp10_action_header *achdr, size_t aclen)
{
	/*
	 * OpenFlow 1.0
	 */

	switch (be16toh(oac_header->type)) {
	case OFP10AT_OUTPUT: {
		//oac_output = (struct ofp10_action_output*)oac_header;
		if (action.memlen() < sizeof(struct ofp10_action_output)) {
			throw eBadActionBadLen();
		}
		uint16_t port_no = be16toh(oac_10output->port);
		if (0 == port_no) {
			throw eBadActionBadOutPort();
		}
	} break;
	case OFP10AT_SET_VLAN_VID: {
		//oac_vlanvid = (struct ofp10_action_vlan_vid*)oac_header;
		if (action.memlen() < sizeof(struct ofp10_action_vlan_vid)) {
			throw eBadActionBadLen();
		}
	} break;
	case OFP10AT_SET_VLAN_PCP: {
		//oac_vlanpcp = (struct ofp10_action_vlan_pcp*)oac_header;
		if (action.memlen() < sizeof(struct ofp10_action_vlan_pcp)) {
			throw eBadActionBadLen();
		}
	} break;
	case OFP10AT_STRIP_VLAN: {
		if (action.memlen() < sizeof(struct ofp_action_header)) {
			throw eBadActionBadLen();
		}
	} break;
	case OFP10AT_SET_DL_SRC:
	case OFP10AT_SET_DL_DST: {
		//oac_dladdr = (struct ofp10_action_dl_addr*)oac_header;
		if (action.memlen() < sizeof(struct ofp10_action_dl_addr)) {
			throw eBadActionBadLen();
		}
	} break;
	case OFP10AT_SET_NW_SRC:
	case OFP10AT_SET_NW_DST: {
		//oac_nwaddr = (struct ofp10_action_nw_addr*)oac_header;
		if (action.memlen() < sizeof(struct ofp10_action_nw_addr)) {
			throw eBadActionBadLen();
		}
	} break;
	case OFP10AT_SET_NW_TOS: {
		//oac_nwtos = (struct ofp10_action_nw_tos*)oac_header;
		if (action.memlen() < sizeof(struct ofp10_action_nw_tos)) {
			throw eBadActionBadLen();
		}
	} break;
	case OFP10AT_SET_TP_SRC:
	case OFP10AT_SET_TP_DST: {
		//oac_tpport = (struct ofp10_action_tp_port*)oac_header;
		if (action.memlen() < sizeof(struct ofp10_action_tp_port)) {
			throw eBadActionBadLen();
		}
	} break;
	case OFP10AT_ENQUEUE: {
		if (action.memlen() < sizeof(struct ofp10_action_enqueue)) {
			throw eBadActionBadLen();
		}
	} break;
	case OFP10AT_VENDOR: {
		//oac_10vendor = (struct ofp10_action_vendor_header*)oac_header;
		if (action.memlen() < sizeof(struct ofp10_action_vendor_header)) {
			throw eBadActionBadLen();
		}
	} break;
	default: {
		throw eBadActionBadType();
	} break;
	}
}



void
cofaction::unpack(
		struct ofp12_action_header *achdr, size_t aclen)
{
	/*
	 * OpenFlow 1.2
	 */
	switch (be16toh(oac_header->type)) {
	case OFP12AT_OUTPUT: {
		if (action.memlen() < sizeof(struct ofp12_action_output)) {
			throw eBadActionBadLen();
		}
		uint32_t port_no = be32toh(oac_12output->port);
		if ((OFPP12_ANY == port_no) || (0 == port_no)) {
			throw eBadActionBadOutPort();
		}
	} break;
	case OFP12AT_SET_FIELD:
		if (action.memlen() < sizeof(struct ofp12_action_set_field)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_COPY_TTL_OUT:
		if (action.memlen() < sizeof(struct ofp12_action_header)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_COPY_TTL_IN:
		if (action.memlen() < sizeof(struct ofp12_action_header)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_SET_MPLS_TTL:
		if (action.memlen() < sizeof(struct ofp12_action_mpls_ttl)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_DEC_MPLS_TTL:
		if (action.memlen() < sizeof(struct ofp12_action_push)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_PUSH_VLAN:
		if (action.memlen() < sizeof(struct ofp12_action_push)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_POP_VLAN:
		if (action.memlen() < sizeof(struct ofp12_action_header)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_PUSH_MPLS:
		if (action.memlen() < sizeof(struct ofp12_action_push)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_POP_MPLS:
		if (action.memlen() < sizeof(struct ofp12_action_pop_mpls)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_SET_QUEUE:
		if (action.memlen() < sizeof(struct ofp12_action_set_queue)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_GROUP:
		if (action.memlen() < sizeof(struct ofp12_action_group)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_SET_NW_TTL:
		if (action.memlen() < sizeof(struct ofp12_action_nw_ttl)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_DEC_NW_TTL:
		if (action.memlen() < sizeof(struct ofp12_action_header)) {
			throw eBadActionBadLen();
		}
		break;
	case OFP12AT_EXPERIMENTER:
		if (action.memlen() < sizeof(struct ofp12_action_experimenter_header)) {
			throw eBadActionBadLen();
		}
		break;
	default:
		throw eBadActionBadType();
		//throw eActionInvalType();
		break;
	}
}



void
cofaction::unpack(
		struct ofp13_action_header *achdr, size_t aclen)
{
	/*
	 * OpenFlow 1.2
	 */
	unpack((struct ofp12_action_header*)achdr, aclen);
}




struct ofp_action_header*
cofaction::soaction() const
{
	return (struct ofp_action_header*)(action.somem());
}	


size_t
cofaction::length() const
{
	switch (ofp_version) {
	case OFP10_VERSION: {

		switch (be16toh(oac_header->type)) {
		case OFP10AT_OUTPUT:
			return sizeof(struct ofp10_action_output);

		case OFP10AT_SET_VLAN_VID:
			return sizeof(struct ofp10_action_vlan_vid);

		case OFP10AT_SET_VLAN_PCP:
			return sizeof(struct ofp10_action_vlan_pcp);

		case OFP10AT_STRIP_VLAN:
			return sizeof(struct ofp10_action_header);

		case OFP10AT_SET_DL_SRC:
		case OFP10AT_SET_DL_DST:
			return sizeof(struct ofp10_action_dl_addr);

		case OFP10AT_SET_NW_SRC:
		case OFP10AT_SET_NW_DST:
			return sizeof(struct ofp10_action_nw_addr);

		case OFP10AT_SET_TP_SRC:
		case OFP10AT_SET_TP_DST:
			return sizeof(struct ofp10_action_tp_port);

		case OFP10AT_ENQUEUE:
			return sizeof(struct ofp10_action_enqueue);

		case OFP10AT_VENDOR:
			return action.memlen();

		default:
			return action.memlen();
		}

	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {

		switch (be16toh(oac_header->type)) {
		case OFP12AT_OUTPUT:
			return sizeof(struct ofp12_action_output);

		case OFP12AT_COPY_TTL_OUT:
			return sizeof(ofp12_action_header);

		case OFP12AT_COPY_TTL_IN:
			return sizeof(ofp12_action_header);

		case OFP12AT_SET_MPLS_TTL:
			return sizeof(struct ofp12_action_mpls_ttl);

		case OFP12AT_DEC_MPLS_TTL:
			return sizeof(ofp12_action_header);

		case OFP12AT_PUSH_VLAN:
			return sizeof(struct ofp12_action_push);

		case OFP12AT_POP_VLAN:
			return sizeof(ofp12_action_header);

		case OFP12AT_PUSH_MPLS:
			return sizeof(struct ofp12_action_push);

		case OFP12AT_POP_MPLS:
			return sizeof(ofp12_action_pop_mpls);

		case OFP12AT_SET_QUEUE:
			return sizeof(struct ofp12_action_set_queue);

		case OFP12AT_GROUP:
			return sizeof(struct ofp12_action_group);

		case OFP12AT_SET_NW_TTL:
			return sizeof(struct ofp12_action_nw_ttl);

		case OFP12AT_DEC_NW_TTL:
			return sizeof(ofp12_action_header);

		case OFP12AT_EXPERIMENTER:
			return sizeof(struct ofp12_action_experimenter_header);

		case OFP12AT_SET_FIELD:
			return action.memlen();

		default:
			return action.memlen();
		}

	} break;
	default:
		throw eBadVersion();
	}
}


#if 0
const char*
cofaction::c_str()
{
	__make_info();
#if 0
	cvastring vas;
	info.assign(vas("cofaction(%p)", this));
#endif
	return info.c_str();
}


void
cofaction::__make_info()
{
	cvastring vas(4096);

	switch (ofp_version) {
	case OFP10_VERSION: {

		switch (be16toh(oac_header->type)) {
		case OFP10AT_OUTPUT: {
			info.assign(vas("cofaction(%p) OFP10AT_OUTPUT length[%zu] port[0x%x] max_len[%d]",
					 this,
					 length(),
					 be16toh(oac_10output->port),
					 be16toh(oac_10output->max_len)));
		} break;
		// TODO: remaining OF1.0 actions
		default: {
			info.assign(vas("cofaction(%p) unknown action ", this));
		} break;
		}

	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {

		switch (be16toh(oac_header->type)) {
		case OFP12AT_OUTPUT: {
			info.assign(vas("cofaction(%p) OFP12AT_OUTPUT length[%zu] port[0x%x] max_len[%d]",
					 this,
					 length(),
					 be32toh(oac_12output->port),
					 be16toh(oac_12output->max_len)));
		} break;
		case OFP12AT_COPY_TTL_OUT: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_COPY_TTL_OUT length[%zu] ",
					 this,
					 length()));
		} break;
		case OFP12AT_COPY_TTL_IN: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_COPY_TTL_IN length[%zu] ",
					this,
					length()));
		} break;
		case OFP12AT_SET_MPLS_TTL: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_SET_MPLS_TTL length[%zu] mpls_ttl[%d]",
					 this,
					 length(),
					 oac_12mpls_ttl->mpls_ttl));
		} break;
		case OFP12AT_DEC_MPLS_TTL: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_DEC_MPLS_TTL length[%zu] ",
					 this, length()));
		} break;
		case OFP12AT_PUSH_VLAN: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_PUSH_VLAN length[%zu] ethertype[0x%04x]",
					 this,
					 length(),
					 be16toh(oac_12push->ethertype)));
		} break;
		case OFP12AT_POP_VLAN: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_POP_VLAN length[%zu] ",
					 this, length()));
		} break;
		case OFP12AT_PUSH_MPLS: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_PUSH_MPLS length[%zu] ethertype[0x%x]",
					 this,
					 length(),
					 be16toh(oac_12push->ethertype)));
		} break;
		case OFP12AT_POP_MPLS: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_POP_MPLS length[%zu] ethertype[0x%x]",
					 this,
					 length(),
					 be16toh(oac_12pop_mpls->ethertype)));
		} break;
		case OFP12AT_SET_QUEUE: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_SET_QUEUE length[%zu] queue_id[%d]",
					 this,
					 length(),
					 be32toh(oac_12set_queue->queue_id)));
		} break;
		case OFP12AT_GROUP: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_GROUP length[%zu] group_id[%d]",
					 this,
					 length(),
					 be32toh(oac_12group->group_id)));
		} break;
		case OFP12AT_SET_NW_TTL: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_SET_NW_TTL length[%zu] nw_ttl[%d]",
					 this,
					 length(),
					 oac_12nw_ttl->nw_ttl));
		} break;
		case OFP12AT_DEC_NW_TTL: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_DEC_NW_TTL length[%zu] ",
					 this, length()));
		} break;
		case OFP12AT_EXPERIMENTER: {
			info.assign(vas(
					 "cofaction(%p) OFP12AT_EXPERIMENTER length[%zu] experimenter[%d]",
					 this,
					 length(),
					 be32toh(oac_12experimenter->experimenter)));
		} break;
		case OFP12AT_SET_FIELD: {
			coxmatch oxm((struct ofp_oxm_hdr*)oac_12set_field->field,
					be16toh(oac_12set_field->len) - 2 * sizeof(uint16_t));
			info.assign(vas(
					 "cofaction(%p) OFP12AT_SET_FIELD length[%zu] [%s]",
					 this,
					 length(),
					 oxm.c_str()));
		} break;
		default: {
			info.assign(vas("cofaction(%p) unknown action ", this));
		} break;
		}

	} break;
	default:
		throw eBadVersion();
	}

}
#endif


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
	oac_header = (struct ofp_action_header*)action.somem();
}



uint32_t
cofaction_output::get_port() throw (eActionInvalType)
{
	switch (ofp_version) {
	case OFP10_VERSION: {
		if (OFP10AT_OUTPUT != get_type())
			throw eActionInvalType();
		return be16toh(oac_10output->port);
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		if (OFP12AT_OUTPUT != get_type())
			throw eActionInvalType();
		return be32toh(oac_12output->port);
	} break;
	default:
		throw eActionInvalType();
	}
}

void
cofaction_output::set_max_len(uint16_t max_len) const throw (eActionInvalType)
{
	switch (ofp_version) {
		case OFP10_VERSION: {
			if (OFP10AT_OUTPUT != get_type())
				throw eActionInvalType();
			oac_10output->max_len = htobe16(max_len);
		} break;
		case OFP12_VERSION:
		case OFP13_VERSION: {
			if (OFP12AT_OUTPUT != get_type())
				throw eActionInvalType();
			oac_12output->max_len = htobe16(max_len);
		} break;
		default:
			throw eActionInvalType();
	}
}
uint16_t
cofaction_output::get_max_len() const throw (eActionInvalType)
{
	switch (ofp_version) {
	case OFP10_VERSION: {
		if (OFP10AT_OUTPUT != get_type())
			throw eActionInvalType();
		return be16toh(oac_10output->max_len);
	} break;
	case OFP12_VERSION:
	case OFP13_VERSION: {
		if (OFP12AT_OUTPUT != get_type())
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
	if (OFP12AT_SET_FIELD != get_type())
	{
		throw eActionInvalType();
	}

	struct ofp_oxm_hdr* oxm_hdr = (struct ofp_oxm_hdr*)oac_12set_field->field;
	size_t oxm_len = be16toh(oac_12set_field->len) - 2 * sizeof(uint16_t); // without action header

	coxmatch oxm(oxm_hdr, oxm_len);

	return oxm;
}

template class coflist<cofaction>;
