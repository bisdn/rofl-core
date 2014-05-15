
#ifndef UCL_EE_CSH_DESC_STATS
#define UCL_EE_CSH_DESC_STATS 1

#include "morpheus.h"
#include "morpheus_nested.h"

class morpheus::csh_desc_stats : public morpheus::chandlersession_base {

protected:
uint32_t m_request_xid;

public:
csh_desc_stats(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_desc_stats_request * const msg);
bool process_desc_stats_request ( const rofl::cofctl * const src, const rofl::cofmsg_desc_stats_request * const msg );
bool process_desc_stats_reply ( rofl::cofdpt * const src, rofl::cofmsg_desc_stats_reply * const msg );
~csh_desc_stats();
std::string asString() const;

};

#endif
