
#ifndef UCL_EE_CSH_TABLE_STATS
#define UCL_EE_CSH_TABLE_STATS 1

#include "morpheus.h"
#include "morpheus_nested.h"

// TODO translation check
class morpheus::csh_table_stats : public morpheus::chandlersession_base {
	
protected:
uint32_t m_request_xid;

public:
csh_table_stats(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_table_stats_request * const msg);
bool process_table_stats_request ( const rofl::cofctl * const src, const rofl::cofmsg_table_stats_request * const msg );
bool process_table_stats_reply ( rofl::cofdpt * const src, rofl::cofmsg_table_stats_reply * const msg );
~csh_table_stats();
std::string asString() const;

};

#endif
