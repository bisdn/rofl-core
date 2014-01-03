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
		cofhello_elem_versionbitmap const& versionbitmap) :
				env(env),
				auxiliary_id(auxiliary_id),
				rofsock(new crofsock(this, sd)),
				versionbitmap(versionbitmap),
				ofp_version(OFP_VERSION_UNKNOWN),
				state(STATE_DISCONNECTED),
				hello_timeout(DEFAULT_HELLO_TIMEOUT),
				echo_timeout(DEFAULT_ECHO_TIMEOUT),
				echo_interval(DEFAULT_ECHO_INTERVAL)
{
	run_engine(EVENT_CONNECTED); // socket is available
}



crofconn::crofconn(
		crofconn_env *env,
		uint8_t auxiliary_id,
		int domain,
		int type,
		int protocol,
		rofl::caddress const& ra,
		cofhello_elem_versionbitmap const& versionbitmap) :
					env(env),
					auxiliary_id(auxiliary_id),
					rofsock(new crofsock(this, domain, type, protocol, ra)),
					versionbitmap(versionbitmap),
					ofp_version(OFP_VERSION_UNKNOWN),
					state(STATE_DISCONNECTED),
					hello_timeout(DEFAULT_HELLO_TIMEOUT),
					echo_timeout(DEFAULT_ECHO_TIMEOUT),
					echo_interval(DEFAULT_ECHO_INTERVAL)
{

}



crofconn::~crofconn()
{
	run_engine(EVENT_DISCONNECTED);
}



void
crofconn::handle_timeout(
		int opaque)
{
	switch (opaque) {
	case TIMER_WAIT_FOR_HELLO: {
		run_engine(EVENT_HELLO_EXPIRED);
	} break;
	case TIMER_SEND_ECHO: {
		action_send_echo_request();
		register_timer(TIMER_WAIT_FOR_ECHO, echo_timeout);
	} break;
	case TIMER_WAIT_FOR_ECHO: {
		run_engine(EVENT_ECHO_EXPIRED);
	} break;
	default: {
		logging::warn << "[rofl][conn] unknown timer type:" << opaque << " rcvd" << std::endl << *this;
	};
	}
}



void
crofconn::run_engine(crofconn_event_t event)
{
	if (EVENT_NONE != event) {
		events.push_back(event);
	}

	while (not events.empty()) {
		enum crofconn_event_t event = events.front();
		events.pop_front();

		switch (event) {
		case EVENT_CONNECTED: 		event_connected(); 			break;
		case EVENT_DISCONNECTED:	event_disconnected();		break;
		case EVENT_HELLO_RCVD:		event_hello_rcvd();			break;
		case EVENT_HELLO_EXPIRED:	event_hello_expired();		break;
		case EVENT_ECHO_RCVD:		event_echo_rcvd();			break;
		case EVENT_ECHO_EXPIRED:	event_echo_expired();		break;
		default: {
			logging::error << "[rofl][conn] unknown event seen, internal error" << std::endl << *this;
		};
		}
	}
}



void
crofconn::event_connected()
{
	switch (state) {
	case STATE_DISCONNECTED:
	case STATE_WAIT_FOR_HELLO:
	case STATE_ESTABLISHED: {
		action_send_hello_message();
		reset_timer(TIMER_WAIT_FOR_HELLO, hello_timeout);
		state = STATE_WAIT_FOR_HELLO;

	} break;
	default: {
		logging::error << "[rofl][conn] event -CONNECTED- invalid state reached, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_disconnected()
{
	switch (state) {
	case STATE_DISCONNECTED: {
		// do nothing
	} break;
	case STATE_WAIT_FOR_HELLO:
	case STATE_ESTABLISHED: {
		cancel_timer(TIMER_WAIT_FOR_ECHO);
		cancel_timer(TIMER_WAIT_FOR_HELLO);
		rofsock->get_socket().cclose();
		env->handle_closed(this);
		state = STATE_DISCONNECTED;

	} break;
	default: {
		logging::error << "[rofl][conn] event -DISCONNECTED- invalid state reached, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_hello_rcvd()
{
	switch (state) {
	case STATE_DISCONNECTED: {
		action_send_hello_message();
		reset_timer(TIMER_WAIT_FOR_HELLO, hello_timeout);
		state = STATE_WAIT_FOR_HELLO;

	} break;
	case STATE_WAIT_FOR_HELLO:
	case STATE_ESTABLISHED: {
		cancel_timer(TIMER_WAIT_FOR_HELLO);
		reset_timer(TIMER_SEND_ECHO, echo_interval);
		state = STATE_ESTABLISHED;

	} break;
	default: {
		logging::error << "[rofl][conn] event -HELLO-RCVD- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_hello_expired()
{
	switch (state) {
	case STATE_WAIT_FOR_HELLO: {
		logging::warn << "[rofl][conn] event -HELLO-EXPIRED- occured in state -WAIT-FOR-HELLO-" << std::endl << *this;
		run_engine(EVENT_DISCONNECTED);

	} break;
	default: {
		logging::error << "[rofl][conn] event -HELLO-EXPIRED- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_echo_rcvd()
{
	switch (state) {
	case STATE_ESTABLISHED: {
		cancel_timer(TIMER_WAIT_FOR_ECHO);
		register_timer(TIMER_SEND_ECHO, echo_interval);

	} break;
	default: {
		logging::error << "[rofl][conn] event -ECHO.reply-RCVD- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_echo_expired()
{
	switch (state) {
	case STATE_ESTABLISHED: {
		logging::warn << "[rofl][conn] ECHO.reply timer expired " << std::endl << *this;
		run_engine(EVENT_DISCONNECTED);

	} break;
	default: {
		logging::error << "[rofl][conn] event -ECHO.reply-EXPIRED- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::action_send_hello_message()
{
	try {
		if (versionbitmap.get_highest_ofp_version() == OFP_VERSION_UNKNOWN) {
			logging::warn << "[rofl][conn] unable to send HELLO message, as no OFP versions are currently configured" << std::endl << *this;
			return;
		}

		cmemory body(versionbitmap.length());
		versionbitmap.pack(body.somem(), body.memlen());

		cofmsg_hello *hello =
				new cofmsg_hello(
						versionbitmap.get_highest_ofp_version(),
						env->get_async_xid(this),
						body.somem(), body.memlen());

		rofsock->send_message(hello);

	} catch (eRofConnXidSpaceExhausted& e) {

		logging::error << "[rofl][conn] sending HELLO.message failed: no idle xid available" << std::endl << *this;

	} catch (RoflException& e) {

		logging::error << "[rofl][conn] sending HELLO.message failed " << std::endl << *this;

	}
}



void
crofconn::action_send_echo_request()
{
	try {
		cofmsg_echo_request *echo =
				new cofmsg_echo_request(
						ofp_version,
						env->get_sync_xid(this));

		rofsock->send_message(echo);

	} catch (eRofConnXidSpaceExhausted& e) {

		logging::error << "[rofl][conn] sending ECHO.request failed: no idle xid available" << std::endl << *this;

	} catch (RoflException& e) {

		logging::error << "[rofl][conn] sending ECHO.request failed " << std::endl << *this;
	}
}



void
crofconn::handle_connect_refused(crofsock *endpnt)
{
	run_engine(EVENT_DISCONNECTED);
	logging::warn << "[rofl][conn] OFP socket indicated connection refused." << std::endl << *this;
	env->handle_connect_refused(this);
}



void
crofconn::handle_connected (crofsock *endpnt)
{
	run_engine(EVENT_CONNECTED);
	logging::warn << "[rofl][conn] OFP socket indicated connection established." << std::endl << *this;
}



void
crofconn::handle_closed(crofsock *endpnt)
{
	run_engine(EVENT_DISCONNECTED);
	logging::warn << "[rofl][conn] OFP socket indicated connection closed." << std::endl << *this;
	env->handle_closed(this);
}



void
crofconn::recv_message(
		crofsock *endpnt,
		cofmsg *msg)
{
	switch (msg->get_type()) {
	case OFPT_HELLO: {
		hello_rcvd(msg);
	} break;
	case OFPT_ERROR: {
		error_rcvd(msg);
	} break;
	case OFPT_ECHO_REQUEST: {
		if (state != STATE_ESTABLISHED) {
			logging::warn << "[rofl][conn] dropping message, connection not fully established." << std::endl << *this;
			delete msg; return;
		}
		echo_request_rcvd(msg);
	} break;
	case OFPT_ECHO_REPLY: {
		if (state != STATE_ESTABLISHED) {
			logging::warn << "[rofl][conn] dropping message, connection not fully established." << std::endl << *this;
			delete msg; return;
		}
		echo_reply_rcvd(msg);
	} break;
	default: {
		if (state != STATE_ESTABLISHED) {
			logging::warn << "[rofl][conn] dropping message, connection not fully established." << std::endl << *this;
			delete msg; return;
		}
		env->recv_message(this, msg);
	} break;
	}
}



void
crofconn::hello_rcvd(
		cofmsg *msg)
{
	cofmsg_hello *hello = dynamic_cast<cofmsg_hello*>( msg );

	if (NULL == hello) {
		logging::debug << "[rofl][conn] invalid message rcvd in method hello_rcvd()" << std::endl << *msg;
		delete msg; return;
	}

	try {

		/* Step 1: extract version information from HELLO message */

		versionbitmap_peer.clear();

		switch (hello->get_version()) {
		case openflow10::OFP_VERSION:
		case openflow12::OFP_VERSION: {
			versionbitmap_peer.add_ofp_version(hello->get_version());
		} break;
		default: { // msg->get_version() should contain the highest number of supported OFP versions encoded in versionbitmap
			cofhelloelems helloIEs(hello->get_body());
			if (not helloIEs.has_hello_elem_versionbitmap()) {
				logging::warn << "[rofl][conn] HELLO message rcvd without HelloIE -VersionBitmap-" << std::endl << *hello << std::endl;
				versionbitmap_peer.add_ofp_version(hello->get_version());
			} else {
				versionbitmap_peer = helloIEs.get_hello_elem_versionbitmap();
				// sanity check
				if (not versionbitmap_peer.has_ofp_version(hello->get_version())) {
					logging::warn << "[rofl][conn] malformed HelloIE -VersionBitmap- => " <<
							"does not contain version defined in OFP message header:" <<
							(int)hello->get_version() << std::endl << *hello;
				}
			}
		};
		}

		/* Step 2: select highest supported protocol version on both sides */

		cofhello_elem_versionbitmap versionbitmap_common = versionbitmap & versionbitmap_peer;
		if (versionbitmap_common.get_highest_ofp_version() == OFP_VERSION_UNKNOWN) {
			logging::warn << "[rofl][conn] no common OFP version found for peer" << std::endl;
			logging::warn << "local version-bitmap:" << std::endl << indent(2) << versionbitmap;
			logging::warn << "remote version-bitmap:" << std::endl << indent(2) << versionbitmap_peer;
			throw eHelloIncompatible();
		}

		ofp_version = versionbitmap_common.get_highest_ofp_version();

		logging::info << "[rofl][conn] negotiated OFP version:" << (int)ofp_version << std::endl << *this;

		// move on state machine
		if (ofp_version == OFP_VERSION_UNKNOWN) {
			logging::warn << "[rofl][conn] no common OFP version supported, closing connection." << std::endl << *this;
			run_engine(EVENT_DISCONNECTED);
		} else {
			run_engine(EVENT_HELLO_RCVD);
			logging::info << "[rofl][conn] connection established." << std::endl << *this;
			env->handle_connected(this, ofp_version);
		}

	} catch (eHelloIncompatible& e) {

		logging::warn << "[rofl][conn] eHelloIncompatible " << *msg << std::endl;

		uint16_t type = 0, code = 0;

		switch (hello->get_version()) {
		case openflow10::OFP_VERSION: {
			type = openflow10::OFPET_HELLO_FAILED; code = openflow10::OFPHFC_INCOMPATIBLE;
		} break;
		case openflow12::OFP_VERSION: {
			type = openflow12::OFPET_HELLO_FAILED; code = openflow12::OFPHFC_INCOMPATIBLE;
		} break;
		case openflow13::OFP_VERSION: {
			type = openflow13::OFPET_HELLO_FAILED; code = openflow13::OFPHFC_INCOMPATIBLE;
		} break;
		default: {
			logging::warn << "[rofl][crofbase] cannot send HelloFailed/Incompatible for ofp-version:"
					<< (int)hello->get_version() << std::endl;
		} delete msg; return;
		}

		cofmsg_error *error =
				new cofmsg_error(
						hello->get_version(), hello->get_xid(), type, code,
							msg->soframe(), msg->framelen());

		rofsock->send_message(error);

		run_engine(EVENT_DISCONNECTED);

	} catch (eHelloEperm& e) {

		logging::warn << "eHelloEperm " << *msg << std::endl;

		uint16_t type = 0, code = 0;

		switch (hello->get_version()) {
		case openflow10::OFP_VERSION: {
			type = openflow10::OFPET_HELLO_FAILED; code = openflow10::OFPHFC_EPERM;
		} break;
		case openflow12::OFP_VERSION: {
			type = openflow12::OFPET_HELLO_FAILED; code = openflow12::OFPHFC_EPERM;
		} break;
		case openflow13::OFP_VERSION: {
			type = openflow13::OFPET_HELLO_FAILED; code = openflow13::OFPHFC_EPERM;
		} break;
		default: {
			logging::warn << "[rofl][crofbase] cannot send HelloFailed/EPerm for ofp-version:"
					<< (int)hello->get_version() << std::endl;
		} delete msg; return;
		}

		cofmsg_error *error =
				new cofmsg_error(
						hello->get_version(), hello->get_xid(), type, code,
							msg->soframe(), msg->framelen());

		rofsock->send_message(error);

		run_engine(EVENT_DISCONNECTED);

	} catch (RoflException& e) {

		logging::warn << "[rofl][conn] RoflException " << *msg << std::endl;

		run_engine(EVENT_DISCONNECTED);
	}

	delete msg;
}



void
crofconn::echo_request_rcvd(
		cofmsg *msg)
{
	cofmsg_echo_request *request = dynamic_cast<cofmsg_echo_request*>( msg );

	try {
		if (NULL == request) {
			logging::debug << "[rofl][conn] invalid message rcvd in method echo_request_rcvd()" << std::endl << *msg;
			delete msg; return;
		}

		cofmsg_echo_reply *reply =
				new cofmsg_echo_reply(request->get_version(), request->get_xid(),
						request->get_body().somem(), request->get_body().memlen());

		rofsock->send_message(reply);

	} catch (RoflException& e) {

		logging::warn << "[rofl][conn] RoflException in echo_request_rcvd() " << *request << std::endl;
	}

	delete msg;
}



void
crofconn::echo_reply_rcvd(
		cofmsg *msg)
{
	cofmsg_echo_reply *reply = dynamic_cast<cofmsg_echo_reply*>( msg );

	try {
		if (NULL == reply) {
			logging::debug << "[rofl][conn] invalid message rcvd in method echo_reply_rcvd()" << std::endl << *msg;
			delete msg; return;
		}

		run_engine(EVENT_ECHO_RCVD);

	} catch (RoflException& e) {

		logging::warn << "[rofl][conn] RoflException in echo_reply_rcvd() " << *reply << std::endl;
	}

	delete msg;

}



void
crofconn::error_rcvd(
		cofmsg *msg)
{
	cofmsg_error *error = dynamic_cast<cofmsg_error*>( msg );

	try {
		if (NULL == error) {
			logging::debug << "[rofl][conn] invalid message rcvd in method error_rcvd()" << std::endl << *msg;
			delete msg; return;
		}

		switch (error->get_err_type()) {
		case openflow13::OFPET_HELLO_FAILED: {

			switch (error->get_err_code()) {
			case openflow13::OFPHFC_INCOMPATIBLE: {
				logging::warn << "[rofl][conn] HELLO-INCOMPATIBLE.error rcvd, closing connection." << std::endl << *this;
			} break;
			case openflow13::OFPHFC_EPERM: {
				logging::warn << "[rofl][conn] HELLO-EPERM.error rcvd, closing connection." << std::endl << *this;
			} break;
			default: {
				logging::warn << "[rofl][conn] HELLO.error rcvd, closing connection." << std::endl << *this;
			};
			}

			delete error;

			run_engine(EVENT_DISCONNECTED);
		} break;
		default: {
			env->recv_message(this, error);
		};
		}

	} catch (RoflException& e) {

		logging::warn << "[rofl][conn] RoflException in error_rcvd() " << *error << std::endl;
	}
}


