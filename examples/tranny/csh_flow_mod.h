
#ifndef UCL_EE_CSH_FLOW_MOD
#define UCL_EE_CSH_FLOW_MOD 1

#include "morpheus.h"
#include "morpheus_nested.h"
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/openflow/cofctl.h>

void dumpBytes (std::ostream & os, uint8_t * bytes, size_t n_bytes);

// TODO make sure that incoming VLAN is stripped
class morpheus::csh_flow_mod : public morpheus::chandlersession_base {

public:

csh_flow_mod(morpheus * parent, rofl::cofctl * const src, rofl::cofmsg_flow_mod * const msg );
bool process_flow_mod ( rofl::cofctl * const src, rofl::cofmsg_flow_mod * const msg );
~csh_flow_mod();
std::string asString() const;

};

#endif
