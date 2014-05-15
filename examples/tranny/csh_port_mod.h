
#ifndef UCL_EE_CSH_PORT_MOD
#define UCL_EE_CSH_PORT_MOD 1

#include "morpheus.h"
#include "morpheus_nested.h"
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/openflow/cofctl.h>

// TODO translation check
class morpheus::csh_port_mod : public morpheus::chandlersession_base {

public:
csh_port_mod(morpheus * parent, rofl::cofctl * const src, rofl::cofmsg_port_mod * const msg );
bool process_port_mod ( rofl::cofctl * const src, rofl::cofmsg_port_mod * const msg );
~csh_port_mod();
std::string asString() const;
};

#endif
