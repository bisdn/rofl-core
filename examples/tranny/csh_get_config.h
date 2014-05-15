
#ifndef UCL_EE_CSH_GET_CONFIG
#define UCL_EE_CSH_GET_CONFIG 1

#include "morpheus.h"
#include "morpheus_nested.h"
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/cofdpt.h>

class morpheus::csh_get_config : public morpheus::chandlersession_base {

protected:
uint32_t m_request_xid;

public:

csh_get_config(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_get_config_request * const msg);
bool process_config_request ( const rofl::cofctl * const src, const rofl::cofmsg_get_config_request * const msg );
bool process_config_reply ( const rofl::cofdpt * const src, rofl::cofmsg_get_config_reply * const msg );
~csh_get_config();
std::string asString() const;

};

#endif
