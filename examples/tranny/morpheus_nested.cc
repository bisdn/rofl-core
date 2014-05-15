
#include "morpheus.h"
#include "morpheus_nested.h"

morpheus::chandlersession_base::chandlersession_base( morpheus * parent ):m_parent(parent),m_completed(false) {}

std::string morpheus::chandlersession_base::asString() const { return "**chandlersession_base**"; }

bool morpheus::chandlersession_base::isCompleted() { return m_completed; }	// returns true if the session has finished its work and shouldn't be kept alive further

void morpheus::chandlersession_base::handle_error (rofl::cofdpt *src, rofl::cofmsg *msg) { std::cout << "** Received error message from dpt " << src->c_str() << " with xid ( " << msg->get_xid() << " ): no handler implemented. Dropping it." << std::endl; delete(msg); }

void morpheus::chandlersession_base::handle_error (rofl::cofctl *src, rofl::cofmsg *msg) { std::cout << "** Received error message from ctl " << src->c_str() << " with xid ( " << msg->get_xid() << " ): no handler implemented. Dropping it." << std::endl; delete(msg); }

morpheus::chandlersession_base::~chandlersession_base() { std::cout << __FUNCTION__ << " called. Session was " << (m_completed?"":"NOT ") << "completed." << std::endl; assert(m_completed); }

void morpheus::chandlersession_base::push_features(uint32_t new_capabilities, uint32_t new_actions) { m_parent->set_supported_dpe_features( new_capabilities, new_actions ); }
