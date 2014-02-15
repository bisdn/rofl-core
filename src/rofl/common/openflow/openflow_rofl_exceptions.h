/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef OPENFLOW_ROFL_EXCEPTIONS_H
#define OPENFLOW_ROFL_EXCEPTIONS_H 1

#include <assert.h>

#include "rofl/common/croflexception.h"


namespace rofl {

class eOpenFlowBase					: public RoflException {};

/* exceptions for hello according to OpenFlow */
class eHelloBase					: public eOpenFlowBase {};
class eHelloIncompatible			: public eHelloBase {};			// OFPHFC_INCOMPATIBLE
class eHelloEperm					: public eHelloBase {};			// OFPHFC_EPERM

/* exceptions for requests according to OpenFlow */
class eBadRequestBase				: public eOpenFlowBase {};
class eBadRequestBadVersion			: public eBadRequestBase {};	// OFPBRC_BAD_VERSION
class eBadRequestBadType			: public eBadRequestBase {};	// OFPBRC_BAD_TYPE
class eBadRequestBadStat			: public eBadRequestBase {};	// OFPBRC_BAD_STAT
class eBadRequestBadExperimenter	: public eBadRequestBase {};	// OFPBRC_BAD_EXPERIMENTER
class eBadRequestBadExpType			: public eBadRequestBase {};	// OFPBRC_BAD_EXP_TYPE
class eBadRequestEperm				: public eBadRequestBase {};	// OFPBRC_EPERM
class eBadRequestBadLen				: public eBadRequestBase {};	// OFPBRC_BAD_LEN
class eBadRequestBufferEmpty		: public eBadRequestBase {};	// OFPBRC_BUFFER_EMPTY
class eBadRequestBufferUnknown		: public eBadRequestBase {};	// OFPBRC_BUFFER_UNKNOWN
class eBadRequestBadTableId			: public eBadRequestBase {};	// OFPBRC_BAD_TABLE_ID
class eBadRequestIsSlave			: public eBadRequestBase {};	// OFPBRC_IS_SLAVE
class eBadRequestBadPort			: public eBadRequestBase {};	// OFPBRC_BAD_PORT
class eBadRequestBadPacket			: public eBadRequestBase {};	// OFPBRC_BAD_PACKET
class eBadRequestMultipartBufferOverflow : public eBadRequestBase {}; // OFPBRC_MULTIPART_BUFFER_OVERFLOW

/* exceptions for action-containing requests according to OpenFlow */
class eBadActionBase				: public eOpenFlowBase {};
class eBadActionBadType				: public eBadActionBase {};		// OFPBAC_BAD_TYPE
class eBadActionBadLen				: public eBadActionBase {};		// OFPBAC_BAD_LEN
class eBadActionBadExperimenter		: public eBadActionBase {};		// OFPBAC_BAD_EXPERIMENTER
class eBadActionBadExperimenterType	: public eBadActionBase {};		// OFPBAC_BAD_EXPERIMENTER_TYPE
class eBadActionBadOutPort			: public eBadActionBase {};		// OFPBAC_BAD_OUT_PORT
class eBadActionBadArgument			: public eBadActionBase {};		// OFPBAC_BAD_ARGUMENT
class eBadActionEperm				: public eBadActionBase {};		// OFPBAC_EPERM
class eBadActionTooMany				: public eBadActionBase {};		// OFPBAC_TOO_MANY
class eBadActionBadQueue			: public eBadActionBase {};		// OFPBAC_BAD_QUEUE
class eBadActionBadOutGroup			: public eBadActionBase {};		// OFPBAC_BAD_OUT_GROUP
class eBadActionMatchInconsistent	: public eBadActionBase {};		// OFPBAC_MATCH_INCONSISTENT
class eBadActionUnsupportedOrder	: public eBadActionBase {};		// OFPBAC_UNSUPPORTED_ORDER
class eBadActionBadTag				: public eBadActionBase {};		// OFPBAC_BAD_TAG
class eBadActionBadSetType			: public eBadActionBase {};		// OFPBAC_BAD_SET_TYPE
class eBadActionBadSetLen			: public eBadActionBase {};		// OFPBAC_BAD_SET_LEN
class eBadActionBadSetArgument		: public eBadActionBase {}; 	// OFPBAC_BAD_SET_ARGUMENT

/* exceptions for action-containing requests according to OpenFlow */
class eBadInstBase					: public eOpenFlowBase {};
class eBadInstUnknownInst			: public eBadInstBase {};		// OFPBIC_UNKNOWN_INST
class eBadInstUnsupInst				: public eBadInstBase {};		// OFPBIC_UNSUP_INST
class eBadInstBadTableId			: public eBadInstBase {};		// OFPBIC_BAD_TABLE_ID
class eBadInstUnsupMetadata			: public eBadInstBase {};		// OFPBIC_UNSUP_METADATA
class eBadInstUnsupMetadataMask		: public eBadInstBase {};		// OFPBIC_UNSUP_METADATA_MASK
class eBadInstBadExperimenter		: public eBadInstBase {};		// OFPBIC_BAD_EXPERIMENTER
class eBadInstBadExpType			: public eBadInstBase {};		// OFPBIC_BAD_EXP_TYPE
class eBadInstBadLen				: public eBadInstBase {};		// OFPBIC_BAD_LEN
class eBadInstEPerm					: public eBadInstBase {};		// OFPBIC_EPERM

/* exceptions for action-containing requests according to OpenFlow */
class eBadMatchBase					: public eOpenFlowBase {};
class eBadMatchBadType				: public eBadMatchBase {}; 		// OFPBMC_BAD_TYPE
class eBadMatchBadLen				: public eBadMatchBase {};		// OFPBMC_BAD_LEN
class eBadMatchBadTag				: public eBadMatchBase {};		// OFPBMC_BAD_TAG
class eBadMatchBadDlAddrMask		: public eBadMatchBase {};		// OFPBMC_BAD_DL_ADDR_MASK
class eBadMatchBadNwAddrMask		: public eBadMatchBase {};		// OFPBMC_BAD_NW_ADDR_MASK
class eBadMatchBadWildcards			: public eBadMatchBase {};		// OFPBMC_BAD_WILDCARDS
class eBadMatchBadField				: public eBadMatchBase {};		// OFPBMC_BAD_FIELD
class eBadMatchBadValue				: public eBadMatchBase {};		// OFPBMC_BAD_VALUE
class eBadMatchBadMask				: public eBadMatchBase {};		// OFPBMC_BAD_MASK
class eBadMatchBadPrereq			: public eBadMatchBase {};		// OFPBMC_BAD_PREREQ
class eBadMatchDupField				: public eBadMatchBase {};		// OFPBMC_DUP_FIELD
class eBadMatchEPerm				: public eBadMatchBase {};		// OFPBMC_EPERM

/* exceptions for flow-mod tables and flow-mod entries according to OpenFlow */
class eFlowModBase					: public eOpenFlowBase {};
class eFlowModUnknown				: public eFlowModBase {};		// OFPFMFC_UNKNOWN
class eFlowModTableFull				: public eFlowModBase {};		// OFPFMFC_TABLE_FULL
class eFlowModBadTableId			: public eFlowModBase {};		// OFPFMFC_BAD_TABLE_ID
class eFlowModOverlap				: public eFlowModBase {}; 		// OFPFMFC_OVERLAP
class eFlowModEperm					: public eFlowModBase {};		// OFPFMFC_EPERM
class eFlowModBadTimeout			: public eFlowModBase {};		// OFPFMFC_BAD_TIMEOUT
class eFlowModBadCommand			: public eFlowModBase {};		// OFPFMFC_BAD_COMMAND
class eFlowModBadFlags				: public eFlowModBase {};		// OFPFMFC_BAD_FLAGS

/* exceptions for group table and group entries according to OpenFlow */
class eGroupModBase					: public eOpenFlowBase {};
class eGroupModExists 				: public eGroupModBase {};		// OFPGMFC_GROUP_EXISTS
class eGroupModInvalGroup			: public eGroupModBase {};		// OFPGMFC_INVALID_GROUP
class eGroupModWeightUnsupported	: public eGroupModBase {};		// OFPGMFC_WEIGHT_UNSUPPORTED
class eGroupModOutOfGroups			: public eGroupModBase {}; 		// OFPGMFC_OUT_OF_GROUPS
class eGroupModOutOfBuckets			: public eGroupModBase {}; 		// OFPGMFC_OUT_OF_BUCKETS
class eGroupModChainingUnsupported	: public eGroupModBase {};		// OFPGMFC_CHAINING_UNSUPPORTED
class eGroupModWatchUnsupported		: public eGroupModBase {};		// OFPGMFC_WATCH_UNSUPPORTED
class eGroupModLoop					: public eGroupModBase {};		// OFPGMFC_LOOP
class eGroupModUnknownGroup			: public eGroupModBase {};		// OFPGMFC_UNKNOWN_GROUP
class eGroupModChainedGroup			: public eGroupModBase {};		// OFPGMFC_CHAINED_GROUP
class eGroupModBadType				: public eGroupModBase {}; 		// OFPGMFC_BAD_TYPE
class eGroupModBadCommand			: public eGroupModBase {};		// OFPGMFC_BAD_COMMAND
class eGroupModBadBucket			: public eGroupModBase {};		// OFPGMFC_BAD_BUCKET
class eGroupModBadWatch				: public eGroupModBase {}; 		// OFPGMFC_BAD_WATCH
class eGroupModEperm				: public eGroupModBase {};		// OFPGMFC_EPERM

/* exceptions for port-mod commands according to OpenFlow */
class ePortModBase					: public eOpenFlowBase {};
class ePortModBadPort				: public ePortModBase {};		// OFPPMFC_BAD_PORT
class ePortModBadHwAddr				: public ePortModBase {};		// OFPPMFC_BAD_HW_ADDR
class ePortModBadConfig				: public ePortModBase {};		// OFPPMFC_BAD_CONFIG
class ePortModBadAdvertise			: public ePortModBase {};		// OFPPMFC_BAD_ADVERTISE
class ePortModEPerm					: public ePortModBase {};		// OFPPMFC_EPERM

/* exceptions for table-mod commands according to OpenFlow */
class eTableModBase					: public eOpenFlowBase {};
class eTableModBadTable				: public eTableModBase {};		// OFPTMFC_BAD_TABLE
class eTableModBadConfig			: public eTableModBase {};		// OFPTMFC_BAD_CONFIG
class eTableModEPerm				: public eTableModBase {};		// OFPTMFC_EPERM

/* exceptions for queue-op commands according to OpenFlow */
class eQueueOpBase					: public eOpenFlowBase {};
class eQueueOpBadPort				: public eQueueOpBase {};		// OFPQOFC_BAD_PORT
class eQueueOpBadQueue				: public eQueueOpBase {};		// OFPQOFC_BAD_QUEUE
class eQueueOpEperm					: public eQueueOpBase {};		// OFPQOFC_EPERM

/* exceptions for switch-config commands according to OpenFlow */
class eSwitchConfigBase				: public eOpenFlowBase {};
class eSwitchConfigBadFlags			: public eSwitchConfigBase {};	// OFPSCFC_BAD_FLAGS
class eSwitchConfigBadLen			: public eSwitchConfigBase {};	// OFPSCFC_BAD_LEN
class eSwitchConfigEPerm			: public eSwitchConfigBase {};	// OFPSCFC_EPERM

/* exceptions for role-request commands according to OpenFlow */
class eRoleRequestBase				: public eOpenFlowBase {};
class eRoleRequestStale				: public eRoleRequestBase {};	// OFPRRFC_STALE
class eRoleRequestUnsupported		: public eRoleRequestBase {};	// OFPRRFC_UNSUPPORTED
class eRoleRequestBadRole			: public eRoleRequestBase {};	// OFPRRFC_BAD_ROLE

/* exceptions for meter-mod commands according to OpenFlow */
class eMeterModBase					: public eOpenFlowBase {};
class eMeterModUnknown				: public eMeterModBase {};		// OFPMMFC_UNKNOWN
class eMeterModMeterExists			: public eMeterModBase {};		// OFPMMFC_METER_EXISTS
class eMeterModInvalidMeter			: public eMeterModBase {};		// OFPMMFC_INVALID_METER
class eMeterModUnknownMeter			: public eMeterModBase {};		// OFPMMFC_UNKNOWN_METER
class eMeterModBadCommand			: public eMeterModBase {};		// OFPMMFC_BAD_COMMAND
class eMeterModBadFlags				: public eMeterModBase {};		// OFPMMFC_BAD_FLAGS
class eMeterModBadRate				: public eMeterModBase {};		// OFPMMFC_BAD_RATE
class eMeterModBadBurst				: public eMeterModBase {};		// OFPMMFC_BAD_BURST
class eMeterModBadBand				: public eMeterModBase {};		// OFPMMFC_BAD_BAND
class eMeterModBadBandValue			: public eMeterModBase {};		// OFPMMFC_BAD_BAND_VALUE
class eMeterModOutOfMeters			: public eMeterModBase {};		// OFPMMFC_OUT_OF_METERS
class eMeterModOutOfBands			: public eMeterModBase {};		// OFPMMFC_OUT_OF_BANDS

/* exceptions for table-features commands according to OpenFlow */
class eTableFeaturesReqBase			: public eOpenFlowBase {};
class eTableFeaturesReqBadTable		: public eTableFeaturesReqBase {};	//OFPTFFC_BAD_TABLE
class eTableFeaturesReqBadMetaData	: public eTableFeaturesReqBase {};	//OFPTFFC_BAD_METADATA
class eTableFeaturesReqBadType		: public eTableFeaturesReqBase {};	//OFPTFFC_BAD_TYPE
class eTableFeaturesReqBadLen		: public eTableFeaturesReqBase {};	//OFPTFFC_BAD_LEN
class eTableFeaturesReqBadArgument	: public eTableFeaturesReqBase {};	//OFPTFFC_BAD_ARGUMENT
class eTableFeaturesReqEPerm		: public eTableFeaturesReqBase {};	//OFPTFFC_EPERM

/*
 * further rofl internal exceptions
 */
class eBadVersion					: public RoflException {
public:
	eBadVersion() {
		std::cerr << "BAD-WOLF" << std::endl;
		assert(0 == 1);
	};
};
class eBadSyntax					: public RoflException {};
class eBadSyntaxTooShort			: public eBadSyntax {};
}

#endif
