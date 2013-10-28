#ifndef IEE80211_ACTIONS_H
#define IEE80211_ACTIONS_H 

#include "../../cofaction.h"

namespace rofl
{

 /** OFPAT_PUSH_IEEE80211
 *
 */
class cofaction_push_ieee80211 : public cofaction_experimenter {
public:
	/** constructor
	 */
	cofaction_push_ieee80211(uint8_t ofp_version) :
				cofaction_experimenter(ofp_version, ROFL_EXPERIMENTER_ID, OFXAT_PUSH_IEEE80211, sizeof(struct ofp12_action_header))
	{
	};

	/** destructor
	 */
	virtual
	~cofaction_push_ieee80211() {};
};

/** OFPAT_POP_IEEE80211
 *
 */
class cofaction_pop_ieee80211 : public cofaction_experimenter {
public:
	/** constructor
	 */
	cofaction_pop_ieee80211(uint8_t ofp_version) :
				cofaction_experimenter(ofp_version, ROFL_EXPERIMENTER_ID, OFXAT_POP_IEEE80211, sizeof(struct ofp12_action_header))
	{
	};

	/** destructor
	 */
	virtual
	~cofaction_pop_ieee80211() {};
};

}; // end of namespace

#endif
