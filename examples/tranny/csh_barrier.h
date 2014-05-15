
#ifndef UCL_EE_CSH_BARRIER
#define UCL_EE_CSH_BARRIER 1

#include "morpheus.h"
#include "morpheus_nested.h"

class morpheus::csh_barrier : public morpheus::chandlersession_base {

protected:
uint32_t m_request_xid;

public:
csh_barrier(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_barrier_request * const msg);
bool process_barrier_request ( const rofl::cofctl * const src, const rofl::cofmsg_barrier_request * const msg );
bool process_barrier_reply ( const rofl::cofdpt * const src, rofl::cofmsg_barrier_reply * const msg );
~csh_barrier();
std::string asString() const;

};

#endif
