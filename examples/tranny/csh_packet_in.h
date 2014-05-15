
#ifndef UCL_EE_CSH_PACKET_IN
#define UCL_EE_CSH_PACKET_IN 1

#include "morpheus.h"
#include "morpheus_nested.h"
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/openflow/cofdpt.h>

class morpheus::csh_packet_in : public morpheus::chandlersession_base {
public:
csh_packet_in(morpheus * parent, const rofl::cofdpt * const src, rofl::cofmsg_packet_in * const msg );
bool process_packet_in ( const rofl::cofdpt * const src, rofl::cofmsg_packet_in * const msg );
~csh_packet_in();
std::string asString() const;
};

#endif
