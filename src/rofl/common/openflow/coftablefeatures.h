/*
 * coftablestats.h
 *
 *  Created on: 14.03.2013
 *      Author: andi
 */

#ifndef COFTABLEFEATURES_H_
#define COFTABLEFEATURES_H_ 1

#include <inttypes.h>

#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/openflow/openflow.h"
#include "rofl/common/openflow/openflow_rofl_exceptions.h"
#include "rofl/common/openflow/coftablefeatureprops.h"

namespace rofl {
namespace openflow {

class eOFTableFeaturesBase 		: public RoflException {};
class eOFTableFeaturesInval		: public eOFTableFeaturesBase {};
class eOFTableFeaturesNotFound	: public eOFTableFeaturesBase {};

class coftable_features :
		public rofl::cmemory
{
private:

	uint8_t												ofp_version;
	rofl::openflow::coftable_feature_props				table_feature_props;

	union {
		uint8_t*										ofhu_table_features_generic;
		struct rofl::openflow13::ofp_table_features*	ofhu_table_features;
	} ofh_ofhu;

#define ofh_tf_generic   		ofh_ofhu.ofhu_table_features_generic
#define ofh_table_features 		ofh_ofhu.ofhu_table_features


public:

	/**
	 *
	 */
	coftable_features(
			uint8_t of_version = rofl::openflow::OFP_VERSION_UNKNOWN);


	/**
	 *
	 */
	virtual
	~coftable_features();


	/**
	 *
	 */
	coftable_features(
			coftable_features const& table_features);

	/**
	 *
	 */
	coftable_features&
	operator= (
			coftable_features const& table_features);



	/**
	 *
	 */
	size_t
	length() const;

	/**
	 *
	 */
	void
	pack(uint8_t* buf, size_t buflen);

	/**
	 *
	 */
	void
	unpack(uint8_t* buf, size_t buflen);

public:

	/**
	 *
	 */
	uint8_t
	get_version() const { return ofp_version; };

	/**
	 *
	 */
	void
	set_version(uint8_t ofp_version) { this->ofp_version = ofp_version; };

	/**
	 *
	 */
	uint16_t
	get_length() const { return be16toh(ofh_table_features->length); };

	/**
	 *
	 */
	void
	set_length(uint16_t length) { ofh_table_features->length = htobe16(length); };

	/**
	 *
	 */
	uint8_t
	get_table_id() const { return ofh_table_features->table_id; };

	/**
	 *
	 */
	void
	set_table_id(uint8_t table_id) { ofh_table_features->table_id = table_id; };

	/**
	 *
	 */
	std::string
	get_name() const {
		return std::string(ofh_table_features->name, strnlen(ofh_table_features->name, OFP_MAX_TABLE_NAME_LEN));
	};

	/**
	 *
	 */
	void
	set_name(std::string const& name) {
		memset(ofh_table_features->name, 0, OFP_MAX_TABLE_NAME_LEN);
		strncpy(ofh_table_features->name, name.c_str(), OFP_MAX_TABLE_NAME_LEN);
	};

	/**
	 *
	 */
	uint64_t
	get_metadata_match() const { return be64toh(ofh_table_features->metadata_match); };

	/**
	 *
	 */
	void
	set_metadata_match(uint64_t metadata_match) { ofh_table_features->metadata_match = htobe64(metadata_match); };

	/**
	 *
	 */
	uint64_t
	get_metadata_write() const { return be64toh(ofh_table_features->metadata_write); };

	/**
	 *
	 */
	void
	set_metadata_write(uint64_t metadata_write) { ofh_table_features->metadata_write = htobe64(metadata_write); };

	/**
	 *
	 */
	uint32_t
	get_config() const { return be32toh(ofh_table_features->config); };

	/**
	 *
	 */
	void
	set_config(uint32_t config) { ofh_table_features->config = htobe32(config); };

	/**
	 *
	 */
	uint32_t
	get_max_entries() const { return be32toh(ofh_table_features->max_entries); };

	/**
	 *
	 */
	void
	set_max_entries(uint32_t max_entries) { ofh_table_features->max_entries = htobe32(max_entries); };

	/**
	 *
	 */
	rofl::openflow::coftable_feature_props const&
	get_properties() const { return table_feature_props; };

	/**
	 *
	 */
	rofl::openflow::coftable_feature_props&
	set_properties() { return table_feature_props; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, coftable_features const& msg) {
		os << indent(0) << "<coftable_features >" << std::endl;
		os << indent(2) << "<table-id: " 		<< (int)msg.get_table_id() 						<< " >" << std::endl;
		os << indent(2) << "<name: " 			<< msg.get_name() 								<< " >" << std::endl;
		os << indent(2) << "<metadata-match: " 	<< (unsigned long long)msg.get_metadata_match() << " >" << std::endl;
		os << indent(2) << "<metadata-write: " 	<< (unsigned long long)msg.get_metadata_write() << " >" << std::endl;
		os << indent(2) << "<config: " 			<< (unsigned int)msg.get_config() 				<< " >" << std::endl;
		os << indent(2) << "<max-entries: " 	<< (unsigned int)msg.get_max_entries() 			<< " >" << std::endl;
		indent i(2);
		os << msg.get_properties();
		return os;
	};

};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* COFTABLESTATS_H_ */
