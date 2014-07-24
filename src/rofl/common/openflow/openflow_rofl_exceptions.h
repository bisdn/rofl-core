/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef OPENFLOW_ROFL_EXCEPTIONS_H
#define OPENFLOW_ROFL_EXCEPTIONS_H 1

#include <assert.h>

#include "rofl/common/croflexception.h"


namespace rofl {

class eOpenFlowBase					: public RoflException {
public:
	eOpenFlowBase(
			const std::string& __arg = std::string("eOpenFlowBase")) :
				RoflException(__arg) {};
	virtual ~eOpenFlowBase() throw() {};
};

/* exceptions for hello according to OpenFlow */
class eHelloBase					: public eOpenFlowBase {
public:
	eHelloBase(
			const std::string& __arg = std::string("eHelloBase")) :
				eOpenFlowBase(__arg) {};
};
class eHelloIncompatible			: public eHelloBase {			// OFPHFC_INCOMPATIBLE
public:
	eHelloIncompatible(
			const std::string& __arg = std::string("eHelloIncompatible")) :
				eHelloBase(__arg) {};
};
class eHelloEperm					: public eHelloBase {			// OFPHFC_EPERM
public:
	eHelloEperm(
			const std::string& __arg = std::string("eHelloEperm")) :
				eHelloBase(__arg) {};
};

/* exceptions for requests according to OpenFlow */
class eBadRequestBase				: public eOpenFlowBase {
public:
	eBadRequestBase(
			const std::string& __arg = std::string("eBadRequestBase")) :
				eOpenFlowBase(__arg) {};
};
class eBadRequestBadVersion			: public eBadRequestBase {		// OFPBRC_BAD_VERSION
public:
	eBadRequestBadVersion(
			const std::string& __arg = std::string("eBadRequestBadVersion")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestBadType			: public eBadRequestBase {		// OFPBRC_BAD_TYPE
public:
	eBadRequestBadType(
			const std::string& __arg = std::string("eBadRequestBadType")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestBadStat			: public eBadRequestBase {		// OFPBRC_BAD_STAT
public:
	eBadRequestBadStat(
			const std::string& __arg = std::string("eBadRequestBadStat")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestBadExperimenter	: public eBadRequestBase {		// OFPBRC_BAD_EXPERIMENTER
public:
	eBadRequestBadExperimenter(
			const std::string& __arg = std::string("eBadRequestBadExperimenter")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestBadExpType			: public eBadRequestBase {		// OFPBRC_BAD_EXP_TYPE
public:
	eBadRequestBadExpType(
			const std::string& __arg = std::string("eBadRequestBadExpType")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestEperm				: public eBadRequestBase {		// OFPBRC_EPERM
public:
	eBadRequestEperm(
			const std::string& __arg = std::string("eBadRequestEperm")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestBadLen				: public eBadRequestBase {		// OFPBRC_BAD_LEN
public:
	eBadRequestBadLen(
			const std::string& __arg = std::string("eBadRequestBadLen")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestBufferEmpty		: public eBadRequestBase {		// OFPBRC_BUFFER_EMPTY
public:
	eBadRequestBufferEmpty(
			const std::string& __arg = std::string("eBadRequestBufferEmpty")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestBufferUnknown		: public eBadRequestBase {		// OFPBRC_BUFFER_UNKNOWN
public:
	eBadRequestBufferUnknown(
			const std::string& __arg = std::string("eBadRequestBufferUnknown")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestBadTableId			: public eBadRequestBase {		// OFPBRC_BAD_TABLE_ID
public:
	eBadRequestBadTableId(
			const std::string& __arg = std::string("eBadRequestBadTableId")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestIsSlave			: public eBadRequestBase {		// OFPBRC_IS_SLAVE
public:
	eBadRequestIsSlave(
			const std::string& __arg = std::string("eBadRequestIsSlave")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestBadPort			: public eBadRequestBase {		// OFPBRC_BAD_PORT
public:
	eBadRequestBadPort(
			const std::string& __arg = std::string("eBadRequestBadPort")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestBadPacket			: public eBadRequestBase {		// OFPBRC_BAD_PACKET
public:
	eBadRequestBadPacket(
			const std::string& __arg = std::string("eBadRequestBadPacket")) :
				eBadRequestBase(__arg) {};
};
class eBadRequestMultipartBufferOverflow : public eBadRequestBase { // OFPBRC_MULTIPART_BUFFER_OVERFLOW
public:
	eBadRequestMultipartBufferOverflow(
			const std::string& __arg = std::string("eBadRequestMultipartBufferOverflow")) :
				eBadRequestBase(__arg) {};
};

/* exceptions for action-containing requests according to OpenFlow */
class eBadActionBase				: public eOpenFlowBase {
public:
	eBadActionBase(
			const std::string& __arg = std::string("eBadActionBase")) :
				eOpenFlowBase(__arg) {};
};
class eBadActionBadType				: public eBadActionBase {		// OFPBAC_BAD_TYPE
public:
	eBadActionBadType(
			const std::string& __arg = std::string("eBadActionBadType")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadLen				: public eBadActionBase {		// OFPBAC_BAD_LEN
public:
	eBadActionBadLen(
			const std::string& __arg = std::string("eBadActionBadLen")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadExperimenter		: public eBadActionBase {		// OFPBAC_BAD_EXPERIMENTER
public:
	eBadActionBadExperimenter(
			const std::string& __arg = std::string("eBadActionBadExperimenter")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadExperimenterType	: public eBadActionBase {		// OFPBAC_BAD_EXPERIMENTER_TYPE
public:
	eBadActionBadExperimenterType(
			const std::string& __arg = std::string("eBadActionBadExperimenterType")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadOutPort			: public eBadActionBase {		// OFPBAC_BAD_OUT_PORT
public:
	eBadActionBadOutPort(
			const std::string& __arg = std::string("eBadActionBadOutPort")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadArgument			: public eBadActionBase {		// OFPBAC_BAD_ARGUMENT
public:
	eBadActionBadArgument(
			const std::string& __arg = std::string("eBadActionBadArgument")) :
				eBadActionBase(__arg) {};
};
class eBadActionEperm				: public eBadActionBase {		// OFPBAC_EPERM
public:
	eBadActionEperm(
			const std::string& __arg = std::string("eBadActionEperm")) :
				eBadActionBase(__arg) {};
};
class eBadActionTooMany				: public eBadActionBase {		// OFPBAC_TOO_MANY
public:
	eBadActionTooMany(
			const std::string& __arg = std::string("eBadActionTooMany")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadQueue			: public eBadActionBase {		// OFPBAC_BAD_QUEUE
public:
	eBadActionBadQueue(
			const std::string& __arg = std::string("eBadActionBadQueue")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadOutGroup			: public eBadActionBase {		// OFPBAC_BAD_OUT_GROUP
public:
	eBadActionBadOutGroup(
			const std::string& __arg = std::string("eBadActionBadOutGroup")) :
				eBadActionBase(__arg) {};
};
class eBadActionMatchInconsistent	: public eBadActionBase {		// OFPBAC_MATCH_INCONSISTENT
public:
	eBadActionMatchInconsistent(
			const std::string& __arg = std::string("eBadActionMatchInconsistent")) :
				eBadActionBase(__arg) {};
};
class eBadActionUnsupportedOrder	: public eBadActionBase {		// OFPBAC_UNSUPPORTED_ORDER
public:
	eBadActionUnsupportedOrder(
			const std::string& __arg = std::string("eBadActionUnsupportedOrder")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadTag				: public eBadActionBase {		// OFPBAC_BAD_TAG
public:
	eBadActionBadTag(
			const std::string& __arg = std::string("eBadActionBadTag")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadSetType			: public eBadActionBase {		// OFPBAC_BAD_SET_TYPE
public:
	eBadActionBadSetType(
			const std::string& __arg = std::string("eBadActionBadSetType")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadSetLen			: public eBadActionBase {		// OFPBAC_BAD_SET_LEN
public:
	eBadActionBadSetLen(
			const std::string& __arg = std::string("eBadActionBadSetLen")) :
				eBadActionBase(__arg) {};
};
class eBadActionBadSetArgument		: public eBadActionBase { 		// OFPBAC_BAD_SET_ARGUMENT
public:
	eBadActionBadSetArgument(
			const std::string& __arg = std::string("eBadActionBadSetArgument")) :
				eBadActionBase(__arg) {};
};

/* exceptions for action-containing requests according to OpenFlow */
class eBadInstBase					: public eOpenFlowBase {
public:
	eBadInstBase(
			const std::string& __arg = std::string("eBadInstBase")) :
				eOpenFlowBase(__arg) {};
};
class eBadInstUnknownInst			: public eBadInstBase {			// OFPBIC_UNKNOWN_INST
public:
	eBadInstUnknownInst(
			const std::string& __arg = std::string("eBadInstUnknownInst")) :
				eBadInstBase(__arg) {};
};
class eBadInstUnsupInst				: public eBadInstBase {			// OFPBIC_UNSUP_INST
public:
	eBadInstUnsupInst(
			const std::string& __arg = std::string("eBadInstUnsupInst")) :
				eBadInstBase(__arg) {};
};
class eBadInstBadTableId			: public eBadInstBase {			// OFPBIC_BAD_TABLE_ID
public:
	eBadInstBadTableId(
			const std::string& __arg = std::string("eBadInstBadTableId")) :
				eBadInstBase(__arg) {};
};
class eBadInstUnsupMetadata			: public eBadInstBase {			// OFPBIC_UNSUP_METADATA
public:
	eBadInstUnsupMetadata(
			const std::string& __arg = std::string("eBadInstUnsupMetadata")) :
				eBadInstBase(__arg) {};
};
class eBadInstUnsupMetadataMask		: public eBadInstBase {			// OFPBIC_UNSUP_METADATA_MASK
public:
	eBadInstUnsupMetadataMask(
			const std::string& __arg = std::string("eBadInstUnsupMetadataMask")) :
				eBadInstBase(__arg) {};
};
class eBadInstBadExperimenter		: public eBadInstBase {			// OFPBIC_BAD_EXPERIMENTER
public:
	eBadInstBadExperimenter(
			const std::string& __arg = std::string("eBadInstBadExperimenter")) :
				eBadInstBase(__arg) {};
};
class eBadInstBadExpType			: public eBadInstBase {			// OFPBIC_BAD_EXP_TYPE
public:
	eBadInstBadExpType(
			const std::string& __arg = std::string("eBadInstBadExpType")) :
				eBadInstBase(__arg) {};
};
class eBadInstBadLen				: public eBadInstBase {			// OFPBIC_BAD_LEN
public:
	eBadInstBadLen(
			const std::string& __arg = std::string("eBadInstBadLen")) :
				eBadInstBase(__arg) {};
};
class eBadInstEperm					: public eBadInstBase {			// OFPBIC_EPERM
public:
	eBadInstEperm(
			const std::string& __arg = std::string("eBadInstEPerm")) :
				eBadInstBase(__arg) {};
};

/* exceptions for action-containing requests according to OpenFlow */
class eBadMatchBase					: public eOpenFlowBase {
public:
	eBadMatchBase(
			const std::string& __arg = std::string("eBadMatchBase")) :
				eOpenFlowBase(__arg) {};
};
class eBadMatchBadType				: public eBadMatchBase { 		// OFPBMC_BAD_TYPE
public:
	eBadMatchBadType(
			const std::string& __arg = std::string("eBadMatchBadType")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchBadLen				: public eBadMatchBase {		// OFPBMC_BAD_LEN
public:
	eBadMatchBadLen(
			const std::string& __arg = std::string("eBadMatchBadLen")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchBadTag				: public eBadMatchBase {		// OFPBMC_BAD_TAG
public:
	eBadMatchBadTag(
			const std::string& __arg = std::string("eBadMatchBadTag")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchBadDlAddrMask		: public eBadMatchBase {		// OFPBMC_BAD_DL_ADDR_MASK
public:
	eBadMatchBadDlAddrMask(
			const std::string& __arg = std::string("eBadMatchBadDlAddrMask")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchBadNwAddrMask		: public eBadMatchBase {		// OFPBMC_BAD_NW_ADDR_MASK
public:
	eBadMatchBadNwAddrMask(
			const std::string& __arg = std::string("eBadMatchBadNwAddrMask")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchBadWildcards			: public eBadMatchBase {		// OFPBMC_BAD_WILDCARDS
public:
	eBadMatchBadWildcards(
			const std::string& __arg = std::string("eBadMatchBadWildcards")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchBadField				: public eBadMatchBase {		// OFPBMC_BAD_FIELD
public:
	eBadMatchBadField(
			const std::string& __arg = std::string("eBadMatchBadField")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchBadValue				: public eBadMatchBase {		// OFPBMC_BAD_VALUE
public:
	eBadMatchBadValue(
			const std::string& __arg = std::string("eBadMatchBadValue")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchBadMask				: public eBadMatchBase {		// OFPBMC_BAD_MASK
public:
	eBadMatchBadMask(
			const std::string& __arg = std::string("eBadMatchBadMask")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchBadPrereq			: public eBadMatchBase {		// OFPBMC_BAD_PREREQ
public:
	eBadMatchBadPrereq(
			const std::string& __arg = std::string("eBadMatchBadPrereq")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchDupField				: public eBadMatchBase {		// OFPBMC_DUP_FIELD
public:
	eBadMatchDupField(
			const std::string& __arg = std::string("eBadMatchDupField")) :
				eBadMatchBase(__arg) {};
};
class eBadMatchEperm				: public eBadMatchBase {		// OFPBMC_EPERM
public:
	eBadMatchEperm(
			const std::string& __arg = std::string("eBadMatchEperm")) :
				eBadMatchBase(__arg) {};
};

/* exceptions for flow-mod tables and flow-mod entries according to OpenFlow */
class eFlowModBase					: public eOpenFlowBase {
public:
	eFlowModBase(
			const std::string& __arg = std::string("eFlowModBase")) :
				eOpenFlowBase(__arg) {};
};
class eFlowModUnknown				: public eFlowModBase {			// OFPFMFC_UNKNOWN
public:
	eFlowModUnknown(
			const std::string& __arg = std::string("eFlowModUnknown")) :
				eFlowModBase(__arg) {};
};
class eFlowModTableFull				: public eFlowModBase {			// OFPFMFC_TABLE_FULL
public:
	eFlowModTableFull(
			const std::string& __arg = std::string("eFlowModTableFull")) :
				eFlowModBase(__arg) {};
};
class eFlowModBadTableId			: public eFlowModBase {			// OFPFMFC_BAD_TABLE_ID
public:
	eFlowModBadTableId(
			const std::string& __arg = std::string("eFlowModBadTableId")) :
				eFlowModBase(__arg) {};
};
class eFlowModOverlap				: public eFlowModBase {			// OFPFMFC_OVERLAP
public:
	eFlowModOverlap(
			const std::string& __arg = std::string("eFlowModOverlap")) :
				eFlowModBase(__arg) {};
};
class eFlowModEperm					: public eFlowModBase {			// OFPFMFC_EPERM
public:
	eFlowModEperm(
			const std::string& __arg = std::string("eFlowModEperm")) :
				eFlowModBase(__arg) {};
};
class eFlowModBadTimeout			: public eFlowModBase {			// OFPFMFC_BAD_TIMEOUT
public:
	eFlowModBadTimeout(
			const std::string& __arg = std::string("eFlowModBadTimeout")) :
				eFlowModBase(__arg) {};
};
class eFlowModBadCommand			: public eFlowModBase {			// OFPFMFC_BAD_COMMAND
public:
	eFlowModBadCommand(
			const std::string& __arg = std::string("eFlowModBadCommand")) :
				eFlowModBase(__arg) {};
};
class eFlowModBadFlags				: public eFlowModBase {			// OFPFMFC_BAD_FLAGS
public:
	eFlowModBadFlags(
			const std::string& __arg = std::string("eFlowModBadFlags")) :
				eFlowModBase(__arg) {};
};

/* exceptions for group table and group entries according to OpenFlow */
class eGroupModBase					: public eOpenFlowBase {
public:
	eGroupModBase(
			const std::string& __arg = std::string("eGroupModBase")) :
				eOpenFlowBase(__arg) {};
};
class eGroupModExists 				: public eGroupModBase {		// OFPGMFC_GROUP_EXISTS
public:
	eGroupModExists(
			const std::string& __arg = std::string("eGroupModExists")) :
				eGroupModBase(__arg) {};
};
class eGroupModInvalGroup			: public eGroupModBase {		// OFPGMFC_INVALID_GROUP
public:
	eGroupModInvalGroup(
			const std::string& __arg = std::string("eGroupModInvalGroup")) :
				eGroupModBase(__arg) {};
};
class eGroupModWeightUnsupported	: public eGroupModBase {		// OFPGMFC_WEIGHT_UNSUPPORTED
public:
	eGroupModWeightUnsupported(
			const std::string& __arg = std::string("eGroupModWeightUnsupported")) :
				eGroupModBase(__arg) {};
};
class eGroupModOutOfGroups			: public eGroupModBase {		// OFPGMFC_OUT_OF_GROUPS
public:
	eGroupModOutOfGroups(
			const std::string& __arg = std::string("eGroupModOutOfGroups")) :
				eGroupModBase(__arg) {};
};
class eGroupModOutOfBuckets			: public eGroupModBase { 		// OFPGMFC_OUT_OF_BUCKETS
public:
	eGroupModOutOfBuckets(
			const std::string& __arg = std::string("eGroupModOutOfBuckets")) :
				eGroupModBase(__arg) {};
};
class eGroupModChainingUnsupported	: public eGroupModBase {		// OFPGMFC_CHAINING_UNSUPPORTED
public:
	eGroupModChainingUnsupported(
			const std::string& __arg = std::string("eGroupModChainingUnsupported")) :
				eGroupModBase(__arg) {};
};
class eGroupModWatchUnsupported		: public eGroupModBase {		// OFPGMFC_WATCH_UNSUPPORTED
public:
	eGroupModWatchUnsupported(
			const std::string& __arg = std::string("eGroupModWatchUnsupported")) :
				eGroupModBase(__arg) {};
};
class eGroupModLoop					: public eGroupModBase {		// OFPGMFC_LOOP
public:
	eGroupModLoop(
			const std::string& __arg = std::string("eGroupModLoop")) :
				eGroupModBase(__arg) {};
};
class eGroupModUnknownGroup			: public eGroupModBase {		// OFPGMFC_UNKNOWN_GROUP
public:
	eGroupModUnknownGroup(
			const std::string& __arg = std::string("eGroupModUnknownGroup")) :
				eGroupModBase(__arg) {};
};
class eGroupModChainedGroup			: public eGroupModBase {		// OFPGMFC_CHAINED_GROUP
public:
	eGroupModChainedGroup(
			const std::string& __arg = std::string("eGroupModChainedGroup")) :
				eGroupModBase(__arg) {};
};
class eGroupModBadType				: public eGroupModBase { 		// OFPGMFC_BAD_TYPE
public:
	eGroupModBadType(
			const std::string& __arg = std::string("eGroupModBadType")) :
				eGroupModBase(__arg) {};
};
class eGroupModBadCommand			: public eGroupModBase {		// OFPGMFC_BAD_COMMAND
public:
	eGroupModBadCommand(
			const std::string& __arg = std::string("eGroupModBadCommand")) :
				eGroupModBase(__arg) {};
};
class eGroupModBadBucket			: public eGroupModBase {		// OFPGMFC_BAD_BUCKET
public:
	eGroupModBadBucket(
			const std::string& __arg = std::string("eGroupModBadBucket")) :
				eGroupModBase(__arg) {};
};
class eGroupModBadWatch				: public eGroupModBase { 		// OFPGMFC_BAD_WATCH
public:
	eGroupModBadWatch(
			const std::string& __arg = std::string("eGroupModBadWatch")) :
				eGroupModBase(__arg) {};
};
class eGroupModEperm				: public eGroupModBase {		// OFPGMFC_EPERM
public:
	eGroupModEperm(
			const std::string& __arg = std::string("eGroupModEperm")) :
				eGroupModBase(__arg) {};
};

/* exceptions for port-mod commands according to OpenFlow */
class ePortModBase					: public eOpenFlowBase {
public:
	ePortModBase(
			const std::string& __arg = std::string("ePortModBase")) :
				eOpenFlowBase(__arg) {};
};
class ePortModBadPort				: public ePortModBase {			// OFPPMFC_BAD_PORT
public:
	ePortModBadPort(
			const std::string& __arg = std::string("ePortModBadPort")) :
				ePortModBase(__arg) {};
};
class ePortModBadHwAddr				: public ePortModBase {			// OFPPMFC_BAD_HW_ADDR
public:
	ePortModBadHwAddr(
			const std::string& __arg = std::string("ePortModBadHwAddr")) :
				ePortModBase(__arg) {};
};
class ePortModBadConfig				: public ePortModBase {			// OFPPMFC_BAD_CONFIG
public:
	ePortModBadConfig(
			const std::string& __arg = std::string("ePortModBadConfig")) :
				ePortModBase(__arg) {};
};
class ePortModBadAdvertise			: public ePortModBase {			// OFPPMFC_BAD_ADVERTISE
public:
	ePortModBadAdvertise(
			const std::string& __arg = std::string("ePortModBadAdvertise")) :
				ePortModBase(__arg) {};
};
class ePortModEperm					: public ePortModBase {			// OFPPMFC_EPERM
public:
	ePortModEperm(
			const std::string& __arg = std::string("ePortModEperm")) :
				ePortModBase(__arg) {};
};

/* exceptions for table-mod commands according to OpenFlow */
class eTableModBase					: public eOpenFlowBase {
public:
	eTableModBase(
			const std::string& __arg = std::string("eTableModBase")) :
				eOpenFlowBase(__arg) {};
};
class eTableModBadTable				: public eTableModBase {		// OFPTMFC_BAD_TABLE
public:
	eTableModBadTable(
			const std::string& __arg = std::string("eTableModBadTable")) :
				eTableModBase(__arg) {};
};
class eTableModBadConfig			: public eTableModBase {		// OFPTMFC_BAD_CONFIG
public:
	eTableModBadConfig(
			const std::string& __arg = std::string("eTableModBadConfig")) :
				eTableModBase(__arg) {};
};
class eTableModEperm				: public eTableModBase {		// OFPTMFC_EPERM
public:
	eTableModEperm(
			const std::string& __arg = std::string("eTableModEperm")) :
				eTableModBase(__arg) {};
};

/* exceptions for queue-op commands according to OpenFlow */
class eQueueOpBase					: public eOpenFlowBase {
public:
	eQueueOpBase(
			const std::string& __arg = std::string("eQueueOpBase")) :
				eOpenFlowBase(__arg) {};
};
class eQueueOpBadPort				: public eQueueOpBase {			// OFPQOFC_BAD_PORT
public:
	eQueueOpBadPort(
			const std::string& __arg = std::string("eQueueOpBadPort")) :
				eQueueOpBase(__arg) {};
};
class eQueueOpBadQueue				: public eQueueOpBase {			// OFPQOFC_BAD_QUEUE
public:
	eQueueOpBadQueue(
			const std::string& __arg = std::string("eQueueOpBadQueue")) :
				eQueueOpBase(__arg) {};
};
class eQueueOpEperm					: public eQueueOpBase {			// OFPQOFC_EPERM
public:
	eQueueOpEperm(
			const std::string& __arg = std::string("eQueueOpEperm")) :
				eQueueOpBase(__arg) {};
};

/* exceptions for switch-config commands according to OpenFlow */
class eSwitchConfigBase				: public eOpenFlowBase {
public:
	eSwitchConfigBase(
			const std::string& __arg = std::string("eSwitchConfigBase")) :
				eOpenFlowBase(__arg) {};
};
class eSwitchConfigBadFlags			: public eSwitchConfigBase {	// OFPSCFC_BAD_FLAGS
public:
	eSwitchConfigBadFlags(
			const std::string& __arg = std::string("eSwitchConfigBadFlags")) :
				eSwitchConfigBase(__arg) {};
};
class eSwitchConfigBadLen			: public eSwitchConfigBase {	// OFPSCFC_BAD_LEN
public:
	eSwitchConfigBadLen(
			const std::string& __arg = std::string("eSwitchConfigBadLen")) :
				eSwitchConfigBase(__arg) {};
};
class eSwitchConfigEPerm			: public eSwitchConfigBase {	// OFPSCFC_EPERM
public:
	eSwitchConfigEPerm(
			const std::string& __arg = std::string("eSwitchConfigEPerm")) :
				eSwitchConfigBase(__arg) {};
};

/* exceptions for role-request commands according to OpenFlow */
class eRoleRequestBase				: public eOpenFlowBase {
public:
	eRoleRequestBase(
			const std::string& __arg = std::string("eRoleRequestBase")) :
				eOpenFlowBase(__arg) {};
};
class eRoleRequestStale				: public eRoleRequestBase {		// OFPRRFC_STALE
public:
	eRoleRequestStale(
			const std::string& __arg = std::string("eRoleRequestStale")) :
				eRoleRequestBase(__arg) {};
};
class eRoleRequestUnsupported		: public eRoleRequestBase {		// OFPRRFC_UNSUPPORTED
public:
	eRoleRequestUnsupported(
			const std::string& __arg = std::string("eRoleRequestUnsupported")) :
				eRoleRequestBase(__arg) {};
};
class eRoleRequestBadRole			: public eRoleRequestBase {		// OFPRRFC_BAD_ROLE
public:
	eRoleRequestBadRole(
			const std::string& __arg = std::string("eRoleRequestBadRole")) :
				eRoleRequestBase(__arg) {};
};

/* exceptions for meter-mod commands according to OpenFlow */
class eMeterModBase					: public eOpenFlowBase {
public:
	eMeterModBase(
			const std::string& __arg = std::string("eMeterModBase")) :
				eOpenFlowBase(__arg) {};
};
class eMeterModUnknown				: public eMeterModBase {		// OFPMMFC_UNKNOWN
public:
	eMeterModUnknown(
			const std::string& __arg = std::string("eMeterModUnknown")) :
				eMeterModBase(__arg) {};
};
class eMeterModMeterExists			: public eMeterModBase {		// OFPMMFC_METER_EXISTS
public:
	eMeterModMeterExists(
			const std::string& __arg = std::string("eMeterModMeterExists")) :
				eMeterModBase(__arg) {};
};
class eMeterModInvalidMeter			: public eMeterModBase {		// OFPMMFC_INVALID_METER
public:
	eMeterModInvalidMeter(
			const std::string& __arg = std::string("eMeterModInvalidMeter")) :
				eMeterModBase(__arg) {};
};
class eMeterModUnknownMeter			: public eMeterModBase {		// OFPMMFC_UNKNOWN_METER
public:
	eMeterModUnknownMeter(
			const std::string& __arg = std::string("eMeterModUnknownMeter")) :
				eMeterModBase(__arg) {};
};
class eMeterModBadCommand			: public eMeterModBase {		// OFPMMFC_BAD_COMMAND
public:
	eMeterModBadCommand(
			const std::string& __arg = std::string("eMeterModBadCommand")) :
				eMeterModBase(__arg) {};
};
class eMeterModBadFlags				: public eMeterModBase {		// OFPMMFC_BAD_FLAGS
public:
	eMeterModBadFlags(
			const std::string& __arg = std::string("eMeterModBadFlags")) :
				eMeterModBase(__arg) {};
};
class eMeterModBadRate				: public eMeterModBase {		// OFPMMFC_BAD_RATE
public:
	eMeterModBadRate(
			const std::string& __arg = std::string("eMeterModBadRate")) :
				eMeterModBase(__arg) {};
};
class eMeterModBadBurst				: public eMeterModBase {		// OFPMMFC_BAD_BURST
public:
	eMeterModBadBurst(
			const std::string& __arg = std::string("eMeterModBadBurst")) :
				eMeterModBase(__arg) {};
};
class eMeterModBadBand				: public eMeterModBase {		// OFPMMFC_BAD_BAND
public:
	eMeterModBadBand(
			const std::string& __arg = std::string("eMeterModBadBand")) :
				eMeterModBase(__arg) {};
};
class eMeterModBadBandValue			: public eMeterModBase {		// OFPMMFC_BAD_BAND_VALUE
public:
	eMeterModBadBandValue(
			const std::string& __arg = std::string("eMeterModBadBandValue")) :
				eMeterModBase(__arg) {};
};
class eMeterModOutOfMeters			: public eMeterModBase {		// OFPMMFC_OUT_OF_METERS
public:
	eMeterModOutOfMeters(
			const std::string& __arg = std::string("eMeterModOutOfMeters")) :
				eMeterModBase(__arg) {};
};
class eMeterModOutOfBands			: public eMeterModBase {		// OFPMMFC_OUT_OF_BANDS
public:
	eMeterModOutOfBands(
			const std::string& __arg = std::string("eMeterModOutOfBands")) :
				eMeterModBase(__arg) {};
};

/* exceptions for table-features commands according to OpenFlow */
class eTableFeaturesReqBase			: public eOpenFlowBase {
public:
	eTableFeaturesReqBase(
			const std::string& __arg = std::string("eTableFeaturesReqBase")) :
				eOpenFlowBase(__arg) {};
};
class eTableFeaturesReqBadTable		: public eTableFeaturesReqBase {	//OFPTFFC_BAD_TABLE
public:
	eTableFeaturesReqBadTable(
			const std::string& __arg = std::string("eTableFeaturesReqBadTable")) :
				eTableFeaturesReqBase(__arg) {};
};
class eTableFeaturesReqBadMetaData	: public eTableFeaturesReqBase {	//OFPTFFC_BAD_METADATA
public:
	eTableFeaturesReqBadMetaData(
			const std::string& __arg = std::string("eTableFeaturesReqBadMetaData")) :
				eTableFeaturesReqBase(__arg) {};
};
class eTableFeaturesReqBadType		: public eTableFeaturesReqBase {	//OFPTFFC_BAD_TYPE
public:
	eTableFeaturesReqBadType(
			const std::string& __arg = std::string("eTableFeaturesReqBadType")) :
				eTableFeaturesReqBase(__arg) {};
};
class eTableFeaturesReqBadLen		: public eTableFeaturesReqBase {	//OFPTFFC_BAD_LEN
public:
	eTableFeaturesReqBadLen(
			const std::string& __arg = std::string("eTableFeaturesReqBadLen")) :
				eTableFeaturesReqBase(__arg) {};
};
class eTableFeaturesReqBadArgument	: public eTableFeaturesReqBase {	//OFPTFFC_BAD_ARGUMENT
public:
	eTableFeaturesReqBadArgument(
			const std::string& __arg = std::string("eTableFeaturesReqBadArgument")) :
				eTableFeaturesReqBase(__arg) {};
};
class eTableFeaturesReqEperm		: public eTableFeaturesReqBase {	//OFPTFFC_EPERM
public:
	eTableFeaturesReqEperm(
			const std::string& __arg = std::string("eTableFeaturesReqEperm")) :
				eTableFeaturesReqBase(__arg) {};
};

}; // end of namespace

#endif
