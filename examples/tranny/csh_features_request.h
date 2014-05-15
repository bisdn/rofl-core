
#ifndef UCL_EE_CSH_FEATURES_REQUEST
#define UCL_EE_CSH_FEATURES_REQUEST 1

#include "morpheus.h"
#include "morpheus_nested.h"
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/cofdpt.h>


class morpheus::csh_features_request : public morpheus::chandlersession_base {

protected:

uint32_t m_request_xid;
bool m_local_request;	// if true then this request originated from morpheus and not as the result of a translated request from a controller

public:

csh_features_request(morpheus * parent);
csh_features_request(morpheus * parent, const rofl::cofctl * const src, const rofl::cofmsg_features_request * const msg);
bool process_features_request ( const rofl::cofctl * const src, const rofl::cofmsg_features_request * const msg );
bool process_features_reply ( const rofl::cofdpt * const src, rofl::cofmsg_features_reply * const msg );
~csh_features_request();
std::string asString() const;

};

#endif
