
#ifndef UCL_EE_CSH_TABLE_MOD
#define UCL_EE_CSH_TABLE_MOD 1

#include "morpheus.h"
#include "morpheus_nested.h"
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/cofdpt.h>

// TODO translation check
class morpheus::csh_table_mod : public morpheus::chandlersession_base {
public:
csh_table_mod(morpheus * parent, rofl::cofctl * const src, rofl::cofmsg_table_mod * const msg );
bool process_table_mod ( rofl::cofctl * const src, rofl::cofmsg_table_mod * const msg );
~csh_table_mod();
std::string asString() const;
};

#endif
