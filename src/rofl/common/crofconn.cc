/*
 * crofconn.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "crofconn.h"

using namespace rofl::openflow;

crofconn::crofconn(
		crofconn_env *env,
		uint8_t auxiliary_id,
		int sd,
		caddress const& ra) :
				env(env),
				auxiliary_id(auxiliary_id),
				rofsock(new crofsock(this, sd, ra))
{

}


crofconn::~crofconn()
{

}



void
crofconn::handle_connect_refused(crofsock *endpnt)
{

}



void
crofconn::handle_open (crofsock *endpnt)
{

}



void
crofconn::handle_close(crofsock *endpnt)
{

}



void
crofconn::recv_message(
		crofsock *endpnt,
		cofmsg *msg)
{
	switch (msg->get_type()) {
	case OFPT_HELLO: 		hello_rcvd(msg);		break;
	case OFPT_ERROR: 		error_rcvd(msg);		break;
	case OFPT_ECHO_REQUEST: echo_request_rcvd(msg);	break;
	case OFPT_ECHO_REPLY: 	echo_reply_rcvd(msg);	break;
	default:				env->recv_message(msg);	break;
	}
}



void
crofconn::hello_rcvd(
		cofmsg *msg)
{
#if 0
	logging::debug << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
			<< " Hello message received" << std::endl << *msg << std::endl;

	try {
		// OpenFlow versions not supported on our side, send error, close connection
		if (not rofbase->is_ofp_version_supported(msg->get_version()))
		{
			new_state(STATE_CTL_DISCONNECTED);

			// invalid OFP_VERSION
			char explanation[256];
			bzero(explanation, sizeof(explanation));
			snprintf(explanation, sizeof(explanation) - 1,
							"unsupported OF version (%d), supported version is (%d)",
							(msg->get_version()), openflow12::OFP_VERSION);

			throw eHelloIncompatible();
		}
		else
		{
			// TODO: determine properly version from hello elements in OpenFlow 1.3
			ofp_version = msg->get_version();

			flags.set(COFCTL_FLAG_HELLO_RCVD);

			new_state(STATE_CTL_ESTABLISHED);

			logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec
					<< " Hello exchanged with peer entity, attached. " << *this << std::endl;

			if (flags.test(COFCTL_FLAG_HELLO_SENT)) {
				register_timer(COFCTL_TIMER_SEND_ECHO_REQUEST, rpc_echo_interval);

				logging::info << "[rofl][ctl] ctid:0x" << std::hex << ctid << std::dec << "" << *this << indent(2)
						<< "Hello exchanged with peer entity (disconnected -> connected)" << std::endl;

				rofbase->handle_ctl_open(this);
			}
		}

		delete msg;

	} catch (eHelloIncompatible& e) {

		logging::warn << "eHelloIncompatible " << *msg << std::endl;
		rofbase->send_error_hello_failed_incompatible(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;
		handle_closed(socket, socket->sd);

	} catch (eHelloEperm& e) {

		logging::warn << "eHelloEperm " << *msg << std::endl;
		rofbase->send_error_hello_failed_eperm(this, msg->get_xid(), msg->soframe(), msg->framelen());
		delete msg;
		handle_closed(socket, socket->sd);

	} catch (eHelloBase& e) {

		logging::warn << "eHelloBase " << *msg << std::endl;
		delete msg;
	}
#endif
}



void
crofconn::echo_request_rcvd(
		cofmsg *msg)
{

}



void
crofconn::echo_reply_rcvd(
		cofmsg *msg)
{

}



void
crofconn::error_rcvd(
		cofmsg *msg)
{

}


