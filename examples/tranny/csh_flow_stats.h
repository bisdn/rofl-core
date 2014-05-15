
#ifndef UCL_EE_CSH_FLOW_STATS
#define UCL_EE_CSH_FLOW_STATS 1

#include "morpheus.h"
#include "morpheus_nested.h"

class morpheus::csh_flow_stats : public morpheus::chandlersession_base {

protected:
uint32_t m_request_xid;

public:
csh_flow_stats(morpheus * parent, rofl::cofctl * const src, rofl::cofmsg_flow_stats_request * const msg);
bool process_flow_stats_request ( rofl::cofctl * const src, rofl::cofmsg_flow_stats_request * const msg );
bool process_flow_stats_reply ( rofl::cofdpt * const src, rofl::cofmsg_flow_stats_reply * const msg );
~csh_flow_stats();
std::string asString() const;

};

#endif
