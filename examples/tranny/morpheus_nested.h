// nested classes

#ifndef UCL_EE_MORPHEUS_NESTED_H
#define UCL_EE_MORPHEUS_NESTED_H 1

#include <sstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <rofl/common/openflow/openflow_rofl_exceptions.h>
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/cerror.h>
#include <rofl/common/openflow/cofaction.h>
#include "morpheus.h"

/**
 * MASTER TODO:
 * secondary messages (either replies or those forwarded) may return errors - session handlers should have error_msg handlers, and morpheus should register a timer after a call to a session method.  If this timer expires (it's set to longer than a reply message timeout) and the session is completed then only then can it be removed.
 * assert that DPE config supports VLAN tagging and stripping.
 * Somewhere the number of bytes of ethernet frame to send for packet-in is set - this must be adjusted for removal of VLAN tag
 * There should be a database of match structs that were received as part of a flow mod, and their translated counter-parts so they could be quickly looked up and swapped back
 * During a packet-in - what to do with the buffer-id? Because we've sent a packet fragment up in the message, but the switch's buffer has a different version of the packet => should be ok, as switch won't be making changes to stored packet and will forward when asked.
 * in Flow-mod - if the incoming virtual port is actually untagged should we set this in match? Can;t really do that in cofmatch (no access to vid_mask). Should we include a vlan_strip action always anyway?
 */

// class morpheus;

void dumpBytes (std::ostream & os, uint8_t * bytes, size_t n_bytes);

class morpheus::chandlersession_base {

protected:

morpheus * m_parent;
bool m_completed;
chandlersession_base( morpheus * parent );

public:

virtual std::string asString() const;
virtual bool isCompleted();
virtual void handle_error (rofl::cofdpt *src, rofl::cofmsg *msg);
virtual void handle_error (rofl::cofctl *src, rofl::cofmsg *msg);
virtual ~chandlersession_base();
void push_features(uint32_t new_capabilities, uint32_t new_actions);

};

/*
class morpheus::cqueue_stats_session : public morpheus::chandlersession_base {
// cqueue_stats_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::cport_stats_session : public morpheus::chandlersession_base {
// cport_stats_session(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};

class morpheus::csh_flow_stats : public morpheus::chandlersession_base {
// csh_flow_stats(morpheus * parent, rofl::cxidowner * originator, rofl::cofmsg * msg):chandlersession_base(parent,originator,msg) { process(originator, msg); }
};



*/

#endif // UCL_EE_MORPHEUS_NESTED_H
