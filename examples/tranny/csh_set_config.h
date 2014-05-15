
#ifndef UCL_EE_CSH_SET_CONFIG
#define UCL_EE_CSH_SET_CONFIG 1

#include "morpheus.h"
#include "morpheus_nested.h"

class morpheus::csh_set_config : public morpheus::chandlersession_base {
public:
csh_set_config(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_set_config * const msg );
bool process_set_config ( const rofl::cofctl * const src, const rofl::cofmsg_set_config * const msg );
~csh_set_config();
std::string asString() const;
};

#endif
