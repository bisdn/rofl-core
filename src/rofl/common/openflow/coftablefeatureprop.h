/*
 * coftablefeatureprop.h
 *
 *  Created on: 02.02.2014
 *      Author: andreas
 */

#ifndef COFTABLEFEATUREPROP_H_
#define COFTABLEFEATUREPROP_H_

#include <inttypes.h>

#include <vector>
#include <algorithm>

#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/cofaction.h"
#include "rofl/common/openflow/cofinstruction.h"

namespace rofl {
namespace openflow {

class eOFTableFeaturePropBase		: public RoflException {};
class eOFTableFeaturePropInval		: public eOFTableFeaturePropBase {};

class coftable_feature_prop :
	public cmemory
{
	uint8_t		ofp_version;

	union {
		uint8_t 											*ofhu_ofp_tfp;
		struct openflow13::ofp_table_feature_prop_header	*ofhu_ofp_tfphdr;
	} ofh_ofhu;

#define ofp_tfp 	ofh_ofhu.ofhu_ofp_tfp
#define ofp_tfphdr 	ofh_ofhu.ofhu_ofp_tfphdr

public:

	/**
	 *
	 */
	coftable_feature_prop();


	/**
	 *
	 */
	coftable_feature_prop(
			uint8_t ofp_version,
			uint16_t type,
			size_t len = sizeof(struct openflow13::ofp_table_feature_prop_header));

	/**
	 *
	 */
	virtual
	~coftable_feature_prop();

	/**
	 *
	 */
	coftable_feature_prop(
			coftable_feature_prop const& tfp);

	/**
	 *
	 */
	coftable_feature_prop&
	operator= (
			coftable_feature_prop const& tfp);

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	uint8_t
	get_version() const;

	/**
	 *
	 */
	uint16_t
	get_type() const;

	/**
	 *
	 */
	void
	set_type(uint16_t type);

	/**
	 *
	 */
	uint16_t
	get_length() const;

	/**
	 *
	 */
	void
	set_length(uint16_t len);

protected:

	/**
	 *
	 */
	uint8_t*
	resize(size_t size);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftable_feature_prop const& tfp) {
		os << indent(0) << "<coftable_feature_prop type:" << tfp.get_type() << " len:" << tfp.get_length() << " >" << std::endl;
		indent i(2);
		os << dynamic_cast<rofl::cmemory const&>( tfp );
		return os;
	};
};



class coftable_feature_prop_instructions :
		public coftable_feature_prop
{
	std::vector<struct rofl::openflow::ofp_instruction> 		instructions_ids;

	union {
		uint8_t													*ofhu_ofp_tfpi;
		struct openflow13::ofp_table_feature_prop_instructions	*ofhu_ofp_tfpihdr;
	} ofh_ofhu;

#define ofh_tfpi	ofh_ofhu.ofhu_ofp_tfpi
#define ofh_tfpihdr	ofh_ofhu.ofhu_ofp_tfpihdr

public:

	/**
	 *
	 */
	coftable_feature_prop_instructions(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN,
			uint16_t type = 0);

	/**
	 *
	 */
	virtual
	~coftable_feature_prop_instructions();

	/**
	 *
	 */
	coftable_feature_prop_instructions(
			coftable_feature_prop_instructions const& tfpi);

	/**
	 *
	 */
	coftable_feature_prop_instructions&
	operator= (
			coftable_feature_prop_instructions const& tfpi);

public:

	/**
	 *
	 */
	void
	clear();

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	std::vector<struct rofl::openflow::ofp_instruction>&
	set_instruction_ids() { return instructions_ids; };

	/**
	 *
	 */
	const std::vector<struct rofl::openflow::ofp_instruction>&
	get_instruction_ids() const { return instructions_ids; };


	/**
	 *
	 */
	void
	add_instruction(
			uint16_t type, uint16_t len = sizeof(struct rofl::openflow::ofp_instruction));


protected:

	/**
	 *
	 */
	uint8_t*
	resize(size_t size);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftable_feature_prop_instructions const& tfp) {
		os << dynamic_cast<coftable_feature_prop const&>( tfp );
		os << indent(2) << "<coftable_feature_prop_instructions size:" << tfp.instructions_ids.size() << " ";
		switch (tfp.get_type()) {
		case rofl::openflow13::OFPTFPT_INSTRUCTIONS: {
			os << "INSTRUCTIONS";
		} break;
		case rofl::openflow13::OFPTFPT_INSTRUCTIONS_MISS: {
			os << "INSTRUCTIONS-MISS";
		} break;
		default: {
			os << "type:" << (int)tfp.get_type();
		};
		}
		os << " >" << std::endl;

		for (std::vector<struct rofl::openflow::ofp_instruction>::const_iterator
				it = tfp.instructions_ids.begin(); it != tfp.instructions_ids.end(); ++it) {
			switch ((*it).type) {
			case rofl::openflow::OFPIT_GOTO_TABLE: {
				os << indent(4) << "<instruction-id GOTO-TABLE >" << std::endl;
			} break;
			case rofl::openflow::OFPIT_WRITE_METADATA: {
				os << indent(4) << "<instruction-id WRITE-METADATA >" << std::endl;
			} break;
			case rofl::openflow::OFPIT_WRITE_ACTIONS: {
				os << indent(4) << "<instruction-id WRITE-ACTIONS >" << std::endl;
			} break;
			case rofl::openflow::OFPIT_APPLY_ACTIONS: {
				os << indent(4) << "<instruction-id APPLY-ACTIONS >" << std::endl;
			} break;
			case rofl::openflow::OFPIT_CLEAR_ACTIONS: {
				os << indent(4) << "<instruction-id CLEAR-ACTIONS >" << std::endl;
			} break;
			case rofl::openflow::OFPIT_METER: {
				os << indent(4) << "<instruction-id METER >" << std::endl;
			} break;
			case rofl::openflow::OFPIT_EXPERIMENTER: {
				os << indent(4) << "<instruction-id EXPERIMENTER >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<instruction-id type:" << (*it).type << " len:" << (*it).len << " >" << std::endl;
			};
			}
		}
		return os;
	};
};



class coftable_feature_prop_next_tables :
		public coftable_feature_prop,
		public std::vector<uint8_t>
{
	union {
		uint8_t													*ofhu_ofp_tfpnxt;
		struct openflow13::ofp_table_feature_prop_next_tables	*ofhu_ofp_tfpnxthdr;
	} ofh_ofhu;

#define ofh_tfpnxt		ofh_ofhu.ofhu_ofp_tfpnxt
#define ofh_tfpnxthdr	ofh_ofhu.ofhu_ofp_tfpnxthdr

public:

	/**
	 *
	 */
	coftable_feature_prop_next_tables(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN,
			uint16_t type = 0);

	/**
	 *
	 */
	virtual
	~coftable_feature_prop_next_tables();

	/**
	 *
	 */
	coftable_feature_prop_next_tables(
			coftable_feature_prop_next_tables const& tfpi);

	/**
	 *
	 */
	coftable_feature_prop_next_tables&
	operator= (
			coftable_feature_prop_next_tables const& tfpi);

public:

	/**
	 *
	 */
	virtual void
	clear();

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	std::vector<uint8_t>&
	get_next_table_ids() { return *this; };

	/**
	 *
	 */
	void
	add_table_id(
			uint8_t table_id);

protected:

	/**
	 *
	 */
	uint8_t*
	resize(size_t size);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftable_feature_prop_next_tables const& tfp) {
		os << dynamic_cast<coftable_feature_prop const&>( tfp );
		os << indent(2) << "<coftable_feature_prop_next_tables size:" << tfp.size() << " ";
		switch (tfp.get_type()) {
		case rofl::openflow13::OFPTFPT_NEXT_TABLES: {
			os << "NEXT-TABLES";
		} break;
		case rofl::openflow13::OFPTFPT_NEXT_TABLES_MISS: {
			os << "NEXT-TABLES-MISS";
		} break;
		default: {
			os << "type:" << (int)tfp.get_type();
		};
		}
		os << " >" << std::endl;

		for (std::vector<uint8_t>::const_iterator it = tfp.begin(); it != tfp.end(); ++it) {
			os << indent(4) << "<table-id:" << (int)(*it) << " >" << std::endl;
		}
		return os;
	};
};



class coftable_feature_prop_actions :
		public coftable_feature_prop
{
	union {
		uint8_t												*ofhu_ofp_tfpa;
		struct openflow13::ofp_table_feature_prop_actions	*ofhu_ofp_tfpahdr;
	} ofh_ofhu;

#define ofh_tfpa	ofh_ofhu.ofhu_ofp_tfpa
#define ofh_tfpahdr	ofh_ofhu.ofhu_ofp_tfpahdr

	std::vector<struct rofl::openflow::ofp_action> 			actions;

public:

	/**
	 *
	 */
	coftable_feature_prop_actions(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN,
			uint16_t type = 0);

	/**
	 *
	 */
	virtual
	~coftable_feature_prop_actions();

	/**
	 *
	 */
	coftable_feature_prop_actions(
			coftable_feature_prop_actions const& tfpa);

	/**
	 *
	 */
	coftable_feature_prop_actions&
	operator= (
			coftable_feature_prop_actions const& tfpa);

public:

	/**
	 *
	 */
	virtual void
	clear();

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);


	/**
	 *
	 */
	std::vector<struct rofl::openflow::ofp_action>&
	set_action_ids() { return actions; };

	/**
	 *
	 */
	const std::vector<struct rofl::openflow::ofp_action>&
	get_action_ids() const { return actions; };


	/**
	 *
	 */
	void
	add_action(
			uint16_t type, uint16_t len = sizeof(struct rofl::openflow::ofp_action));


protected:

	/**
	 *
	 */
	uint8_t*
	resize(size_t size);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftable_feature_prop_actions const& tfp) {
		os << dynamic_cast<coftable_feature_prop const&>( tfp );
		os << indent(2) << "<coftable_feature_prop_actions size:" << tfp.actions.size() << " ";
		switch (tfp.get_type()) {
		case rofl::openflow13::OFPTFPT_WRITE_ACTIONS: {
			os << "WRITE-ACTIONS";
		} break;
		case rofl::openflow13::OFPTFPT_WRITE_ACTIONS_MISS: {
			os << "WRITE-ACTIONS-MISS";
		} break;
		case rofl::openflow13::OFPTFPT_APPLY_ACTIONS: {
			os << "APPLY-ACTIONS";
		} break;
		case rofl::openflow13::OFPTFPT_APPLY_ACTIONS_MISS: {
			os << "APPLY-ACTIONS-MISS";
		} break;
		default: {
			os << "type:" << (int)tfp.get_type();
		};
		}
		os << " >" << std::endl;

		for (std::vector<struct rofl::openflow::ofp_action>::const_iterator
				it = tfp.actions.begin(); it != tfp.actions.end(); ++it) {

			switch ((*it).type) {
			case rofl::openflow::OFPAT_OUTPUT: {
				os << indent(4) << "<action-id ACTION-OUTPUT len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_COPY_TTL_OUT: {
				os << indent(4) << "<action-id ACTION-COPY-TTL-OUT len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_COPY_TTL_IN: {
				os << indent(4) << "<action-id ACTION-COPY-TTL-IN len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_SET_MPLS_TTL: {
				os << indent(4) << "<action-id ACTION-SET-MPLS-TTL len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_DEC_MPLS_TTL: {
				os << indent(4) << "<action-id ACTION-DEC-MPLS-TTL len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_PUSH_VLAN: {
				os << indent(4) << "<action-id ACTION-PUSH-VLAN len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_POP_VLAN: {
				os << indent(4) << "<action-id ACTION-POP-VLAN len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_PUSH_MPLS: {
				os << indent(4) << "<action-id ACTION-PUSH-MPLS len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_POP_MPLS: {
				os << indent(4) << "<action-id ACTION-POP-MPLS len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_SET_QUEUE: {
				os << indent(4) << "<action-id ACTION-SET-QUEUE len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_GROUP: {
				os << indent(4) << "<action-id ACTION-GROUP len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_SET_NW_TTL: {
				os << indent(4) << "<action-id ACTION-SET-NW-TTL len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_DEC_NW_TTL: {
				os << indent(4) << "<action-id ACTION-DEC-NW-TTL len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_SET_FIELD: {
				os << indent(4) << "<action-id ACTION-SET-FIELD len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_PUSH_PBB: {
				os << indent(4) << "<action-id ACTION-PUSH-PBB len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_POP_PBB: {
				os << indent(4) << "<action-id ACTION-POP-PBB len:" << (*it).len << " >" << std::endl;
			} break;
			case rofl::openflow::OFPAT_EXPERIMENTER: {
				os << indent(4) << "<action-id ACTION-EXPERIMENTER len:" << (*it).len << " >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<action-id type:" << (*it).type << " len:" << (*it).len << " >" << std::endl;
			};
			}
		}
		return os;
	};
};



class coftable_feature_prop_oxm :
		public coftable_feature_prop
{
	std::vector<uint32_t>	oxm_ids;
	std::vector<uint64_t>	oxm_ids_exp;

	union {
		uint8_t											*ofhu_ofp_tfpoxm;
		struct openflow13::ofp_table_feature_prop_oxm	*ofhu_ofp_tfpoxmhdr;
	} ofh_ofhu;

#define ofh_tfpoxm		ofh_ofhu.ofhu_ofp_tfpoxm
#define ofh_tfpoxmhdr	ofh_ofhu.ofhu_ofp_tfpoxmhdr

public:

	/**
	 *
	 */
	coftable_feature_prop_oxm(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN,
			uint16_t type = 0);

	/**
	 *
	 */
	virtual
	~coftable_feature_prop_oxm();

	/**
	 *
	 */
	coftable_feature_prop_oxm(
			coftable_feature_prop_oxm const& tfpoxm);

	/**
	 *
	 */
	coftable_feature_prop_oxm&
	operator= (
			coftable_feature_prop_oxm const& tfpoxm);

public:

	/**
	 *
	 */
	virtual void
	clear();

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t* buf, size_t buflen);


	/**
	 *
	 */
	std::vector<uint32_t>&
	set_oxm_ids() { return oxm_ids; };

	/**
	 *
	 */
	const std::vector<uint32_t>&
	get_oxm_ids() const { return oxm_ids; };

	/**
	 *
	 */
	std::vector<uint64_t>&
	set_oxm_ids_exp() { return oxm_ids_exp; };

	/**
	 *
	 */
	const std::vector<uint64_t>&
	get_oxm_ids_exp() const { return oxm_ids_exp; };


	/**
	 *
	 */
	void
	add_oxm(
			uint32_t oxm_id);


	/**
	 *
	 */
	void
	add_oxm_exp(
			uint64_t oxm_exp_id);



protected:

	/**
	 *
	 */
	uint8_t*
	resize(size_t size);

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftable_feature_prop_oxm const& tfp) {
		os << dynamic_cast<coftable_feature_prop const&>( tfp );
		os << indent(2) << "<coftable_feature_prop_oxm "
				<< "#oxm-ids:" << tfp.oxm_ids.size() << " "
				<< "#oxm-ids-exp:" << tfp.oxm_ids_exp.size() << " ";
		switch (tfp.get_type()) {
		case rofl::openflow13::OFPTFPT_MATCH: {
			os << "MATCH";
		} break;
		case rofl::openflow13::OFPTFPT_WILDCARDS: {
			os << "WILDCARDS";
		} break;
		case rofl::openflow13::OFPTFPT_WRITE_SETFIELD: {
			os << "WRITE-SETFIELD";
		} break;
		case rofl::openflow13::OFPTFPT_WRITE_SETFIELD_MISS: {
			os << "WRITE-SETFIELD-MISS";
		} break;
		case rofl::openflow13::OFPTFPT_APPLY_SETFIELD: {
			os << "APPLY-SETFIELD";
		} break;
		case rofl::openflow13::OFPTFPT_APPLY_SETFIELD_MISS: {
			os << "APPLY-SETFIELD-MISS";
		} break;
		default: {
			os << "type:" << (int)tfp.get_type();
		};
		}
		os << " >" << std::endl;

		for (std::vector<uint32_t>::const_iterator it = tfp.oxm_ids.begin(); it != tfp.oxm_ids.end(); ++it) {

			switch ((int32_t)*it) {
			case rofl::openflow::OXM_TLV_BASIC_IN_PORT: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IN-PORT >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IN_PHY_PORT: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IN-PHY-PORT >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_METADATA: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-METADATA >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_METADATA_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-METADATA-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ETH_DST: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ETH-DST >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ETH_DST_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ETH-DST-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ETH_SRC: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ETH-SRC >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ETH_SRC_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ETH-SRC-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ETH_TYPE: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ETH-TYPE >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_VLAN_VID: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-VLAN-VID >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_VLAN_VID_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-VLAN-VID-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_VLAN_PCP: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-VLAN-PCP >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IP_DSCP: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IP-DSCP >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IP_ECN: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IP-ECN >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IP_PROTO: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IP-PROTO >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV4_SRC: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV4-SRC >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV4_SRC_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV4-SRC-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV4_DST: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV4-DST >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV4_DST_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV4-DST-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_TCP_SRC: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-TCP-SRC >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_TCP_DST: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-TCP-DST >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_UDP_SRC: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-UDP-SRC >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_UDP_DST: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-UDP-DST >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_SCTP_SRC: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-SCTP-SRC >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_SCTP_DST: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-SCTP-DST >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ICMPV4_TYPE: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ICMPV4-TYPE >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ICMPV4_CODE: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ICMPV4-CODE >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ARP_OP: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ARP-OP >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ARP_SPA: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ARP-SPA >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ARP_SPA_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ARP-SPA-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ARP_TPA: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ARP-TPA >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ARP_TPA_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ARP-TPA-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ARP_SHA: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ARP-SHA >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ARP_SHA_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ARP-SHA-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ARP_THA: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ARP-THA >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ARP_THA_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ARP-THA-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV6_SRC: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV6-SRC >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV6_SRC_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV6-SRC-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV6_DST: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV6-DST >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV6_DST_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV6-DST-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV6_FLABEL: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV6-FLABEL >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ICMPV6_TYPE: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ICMPV6-TYPE >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_ICMPV6_CODE: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-ICMPV6-CODE >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TARGET: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV6-ND-TARGET >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV6_ND_SLL: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV6-ND-SLL >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV6_ND_TLL: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV6-ND-TLL >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_MPLS_LABEL: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-MPLS-LABEL >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_MPLS_TC: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-MPLS-TC >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_MPLS_BOS: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-MPLS-BOS >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_PBB_ISID: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-PBB-ISID >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_PBB_ISID_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-PBB-ISID-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-TUNNEL-ID >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_TUNNEL_ID_MASK: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-TUNNEL-ID-MASK >" << std::endl;
			} break;
			case rofl::openflow::OXM_TLV_BASIC_IPV6_EXTHDR: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " BASIC-IPV6-EXTHDR >" << std::endl;
			} break;
			default: {
				os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " >" << std::endl;
			};
			}
		}
		for (std::vector<uint64_t>::const_iterator it = tfp.oxm_ids_exp.begin(); it != tfp.oxm_ids_exp.end(); ++it) {
			os << indent(4) << "<oxm-id-exp 0x" << std::hex << (*it) << std::dec << " >" << std::endl;
		}
		return os;
	};
};


/*
 * TODO: coftable_feature_prop_experimenter
 */

};
};

#endif /* COFTABLEFEATUREPROP_H_ */
