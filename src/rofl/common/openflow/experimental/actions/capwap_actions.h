#ifndef CAPWAP_ACTIONS_H
#define CAPWAP_ACTIONS_H 

#include "../../cofaction.h"

namespace rofl
{


/** OFPAT_PUSH_CAPWAP
 *
 */
class cofaction_push_capwap : public cofaction_experimenter {
public:
	/** constructor
	 */
	cofaction_push_capwap(uint8_t ofp_version) :
				cofaction_experimenter(ofp_version, ROFL_EXPERIMENTER_ID, OFXAT_PUSH_CAPWAP, sizeof(struct ofp12_action_header))
	{
	};

	/** destructor
	 */
	virtual
	~cofaction_push_capwap() {};
};

/** OFPAT_POP_CAPWAP
 *
 */
class cofaction_pop_capwap : public cofaction_experimenter {
public:
	/** constructor
	 */
	cofaction_pop_capwap(uint8_t ofp_version) :
				cofaction_experimenter(ofp_version, ROFL_EXPERIMENTER_ID, OFXAT_POP_CAPWAP, sizeof(struct ofp12_action_header))
	{
	};

	/** destructor
	 */
	virtual
	~cofaction_pop_capwap() {};
};
 


}; // end of namespace

#endif
