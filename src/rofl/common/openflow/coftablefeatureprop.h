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
#include "rofl/common/openflow/cofinstructions.h"

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
};

};
};

#endif /* COFTABLEFEATUREPROP_H_ */
