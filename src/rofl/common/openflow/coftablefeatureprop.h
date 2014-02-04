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
	coftable_feature_prop(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN,
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
		return os;
	};
};



class coftable_feature_prop_instructions :
		public coftable_feature_prop,
		public std::vector<cofinst>
{
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
			uint8_t ofp_version = OFP_VERSION_UNKNOWN);

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
	std::vector<cofinst>&
	get_instruction_ids() { return *this; };


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
		os << indent(2) << "<coftable_feature_prop_instructions size:" << tfp.size() << " >" << std::endl;
		for (std::vector<cofinst>::const_iterator it = tfp.begin(); it != tfp.end(); ++it) {
			os << indent(4) << "<instruction-id type:" << (*it).get_type() << " len:" << (*it).get_length() << " >" << std::endl;
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
			uint8_t ofp_version = OFP_VERSION_UNKNOWN);

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
		os << indent(2) << "<coftable_feature_prop_next_tables size:" << tfp.size() << " >" << std::endl;
		for (std::vector<uint8_t>::const_iterator it = tfp.begin(); it != tfp.end(); ++it) {
			os << indent(4) << "<table-id type:" << (*it) << " >" << std::endl;
		}
		return os;
	};
};



class coftable_feature_prop_actions :
		public coftable_feature_prop,
		public std::vector<cofaction>
{
	union {
		uint8_t												*ofhu_ofp_tfpa;
		struct openflow13::ofp_table_feature_prop_actions	*ofhu_ofp_tfpahdr;
	} ofh_ofhu;

#define ofh_tfpa	ofh_ofhu.ofhu_ofp_tfpa
#define ofh_tfpahdr	ofh_ofhu.ofhu_ofp_tfpahdr

public:

	/**
	 *
	 */
	coftable_feature_prop_actions(
			uint8_t ofp_version = OFP_VERSION_UNKNOWN);

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
	std::vector<cofaction>&
	get_action_ids() { return *this; };


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
		os << indent(2) << "<coftable_feature_prop_actions size:" << tfp.size() << " >" << std::endl;
		for (std::vector<cofaction>::const_iterator it = tfp.begin(); it != tfp.end(); ++it) {
			os << indent(4) << "<action-id type:" << (*it).get_type() << " len:" << (*it).get_length() << " >" << std::endl;
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
			uint8_t ofp_version = OFP_VERSION_UNKNOWN);

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
	get_oxm_ids() { return oxm_ids; };


	/**
	 *
	 */
	std::vector<uint64_t>&
	get_oxm_ids_exp() { return oxm_ids_exp; };


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
				<< "oxm-ids-size:" << tfp.oxm_ids.size()
				<< "oxm-ids-exp-size:" << tfp.oxm_ids_exp.size() << " >" << std::endl;
		for (std::vector<uint32_t>::const_iterator it = tfp.oxm_ids.begin(); it != tfp.oxm_ids.end(); ++it) {
			os << indent(4) << "<oxm-id 0x" << std::hex << (*it) << std::dec << " >" << std::endl;
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
