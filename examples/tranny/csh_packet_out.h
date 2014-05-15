
#ifndef UCL_EE_CSH_PACKET_OUT
#define UCL_EE_CSH_PACKET_OUT 1

#include "morpheus.h"
#include "morpheus_nested.h"
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/openflow/cofctl.h>

// TODO translation check
class morpheus::csh_packet_out : public morpheus::chandlersession_base {
public:
csh_packet_out(morpheus * parent, rofl::cofctl * const src, rofl::cofmsg_packet_out * const msg );
bool process_packet_out ( rofl::cofctl * const src, rofl::cofmsg_packet_out * const msg );
~csh_packet_out();
std::string asString() const;
};

#endif
