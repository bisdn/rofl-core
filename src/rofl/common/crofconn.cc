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
		cofhello_elem_versionbitmap const& versionbitmap) :
				env(env),
				dpid(0), // will be determined later via Features.request
				auxiliary_id(0), // same as dpid
				rofsock(this),
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
	//if (STATE_DISCONNECTED != state) {
	//	run_engine(EVENT_DISCONNECTED);
	//}
}



void
crofconn::accept(int newsd)
{
	flags.set(FLAGS_PASSIVE);
	rofsock.accept(newsd);
}



void
crofconn::reconnect()
{
	if (STATE_ESTABLISHED == state) {
		return;
	}
	state = STATE_CONNECT_PENDING;
	rofsock.reconnect();
}



void
crofconn::connect(
		uint8_t aux_id,
		int domain,
		int type,
		int protocol,
		caddress const& raddr)
{
	if (STATE_ESTABLISHED == state) {
		throw eRofConnBusy();
	}
	flags.reset(FLAGS_PASSIVE);
	auxiliary_id = aux_id;
	state = STATE_CONNECT_PENDING;
	rofsock.connect(domain, type, protocol ,raddr);
}



void
crofconn::close()
{
	if (STATE_DISCONNECTED == state) {
		return;
	}
	state = STATE_DISCONNECTED;
	rofsock.close();
}



void
crofconn::handle_timeout(
		int opaque, void *data)
{
	switch (opaque) {
	case TIMER_WAIT_FOR_HELLO: {
		run_engine(EVENT_HELLO_EXPIRED);
	} break;
	case TIMER_SEND_ECHO: {
		action_send_echo_request();
		timer_ids[TIMER_WAIT_FOR_ECHO] = register_timer(TIMER_WAIT_FOR_ECHO, echo_timeout);
	} break;
	case TIMER_WAIT_FOR_ECHO: {
		run_engine(EVENT_ECHO_EXPIRED);
	} break;
	case TIMER_WAIT_FOR_FEATURES: {
		run_engine(EVENT_FEATURES_EXPIRED);
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
		case EVENT_DISCONNECTED:	event_disconnected();		return; // might call this object's destructor
		case EVENT_HELLO_RCVD:		event_hello_rcvd();			break;
		case EVENT_HELLO_EXPIRED:	event_hello_expired();		break;
		case EVENT_FEATURES_RCVD:	event_features_rcvd();		break;
		case EVENT_FEATURES_EXPIRED:event_features_expired();	break;
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
	case STATE_CONNECT_PENDING:
	case STATE_WAIT_FOR_HELLO:
	case STATE_ESTABLISHED: {
		state = STATE_WAIT_FOR_HELLO;
		action_send_hello_message();
		if (timer_ids.find(TIMER_WAIT_FOR_HELLO) != timer_ids.end()) {
			cancel_timer(timer_ids[TIMER_WAIT_FOR_HELLO]);
		}
		timer_ids[TIMER_WAIT_FOR_HELLO] = register_timer(TIMER_WAIT_FOR_HELLO, hello_timeout);

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

	} break;
	case STATE_CONNECT_PENDING: {
		if (not flags.test(FLAGS_PASSIVE)) {
			env->handle_connect_refused(this);
		}
	} break;
	case STATE_WAIT_FOR_HELLO:
	case STATE_ESTABLISHED: {
		state = STATE_DISCONNECTED;
		if (timer_ids.find(TIMER_WAIT_FOR_ECHO) != timer_ids.end()) {
			cancel_timer(timer_ids[TIMER_WAIT_FOR_ECHO]);
		}
		if (timer_ids.find(TIMER_WAIT_FOR_HELLO) != timer_ids.end()) {
			cancel_timer(timer_ids[TIMER_WAIT_FOR_HELLO]);
		}
		rofsock.close();
		env->handle_closed(this);

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
		state = STATE_WAIT_FOR_HELLO;
		action_send_hello_message();
		timer_ids[TIMER_WAIT_FOR_HELLO] = register_timer(TIMER_WAIT_FOR_HELLO, hello_timeout);

	} break;
	case STATE_WAIT_FOR_HELLO: {

		cancel_timer(timer_ids[TIMER_WAIT_FOR_HELLO]);

		if (flags.test(FLAGS_PASSIVE)) {
			state = STATE_WAIT_FOR_FEATURES;
			action_send_features_request();

			if (timer_ids.find(TIMER_WAIT_FOR_FEATURES) != timer_ids.end()) {
				cancel_timer(timer_ids[TIMER_WAIT_FOR_FEATURES]);
			}
			timer_ids[TIMER_WAIT_FOR_FEATURES] = register_timer(TIMER_WAIT_FOR_FEATURES, echo_interval);

		} else {
			state = STATE_ESTABLISHED;

			if (timer_ids.find(TIMER_SEND_ECHO) != timer_ids.end()) {
				cancel_timer(timer_ids[TIMER_SEND_ECHO]);
			}
			timer_ids[TIMER_SEND_ECHO] = register_timer(TIMER_SEND_ECHO, echo_interval);
			env->handle_connected(this, ofp_version);
		}

	} break;
	case STATE_ESTABLISHED: {
		// do nothing
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
crofconn::event_features_rcvd()
{
	switch (state) {
	case STATE_WAIT_FOR_FEATURES: {
		if (flags.test(FLAGS_PASSIVE)) {
			state = STATE_ESTABLISHED;
			cancel_timer(timer_ids[TIMER_WAIT_FOR_FEATURES]);

			timer_ids[TIMER_SEND_ECHO] = register_timer(TIMER_SEND_ECHO, echo_interval);

			env->handle_connected(this, ofp_version);
		}

	} break;
	case STATE_ESTABLISHED: {
		// do nothing
	} break;
	default: {
		logging::error << "[rofl][conn] event -FEATURES-RCVD- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_features_expired()
{
	switch (state) {
	case STATE_WAIT_FOR_FEATURES: {
		logging::warn << "[rofl][conn] event -FEATURES-EXPIRED- occured in state -WAIT-FOR-FEATURES-" << std::endl << *this;
		run_engine(EVENT_DISCONNECTED);

	} break;
	default: {
		logging::error << "[rofl][conn] event -FEATURES-EXPIRED- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}


void
crofconn::event_echo_rcvd()
{
	switch (state) {
	case STATE_ESTABLISHED: {
		cancel_timer(timer_ids[TIMER_WAIT_FOR_ECHO]);
		timer_ids[TIMER_SEND_ECHO] = register_timer(TIMER_SEND_ECHO, echo_interval);

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

		logging::error << "[rofl][conn] sending HELLO.message:" << std::endl << *hello;

		rofsock.send_message(hello);

	} catch (eRofConnXidSpaceExhausted& e) {

		logging::error << "[rofl][conn] sending HELLO.message failed: no idle xid available" << std::endl << *this;

	} catch (RoflException& e) {

		logging::error << "[rofl][conn] sending HELLO.message failed " << std::endl << *this;

	}
}



void
crofconn::action_send_features_request()
{
	try {
		cmemory body(versionbitmap.length());
		versionbitmap.pack(body.somem(), body.memlen());

		cofmsg_features_request *request =
				new cofmsg_features_request(ofp_version, env->get_async_xid(this));

		logging::error << "[rofl][conn] sending FEATURES.request:" << std::endl << *request;

		rofsock.send_message(request);

	} catch (eRofConnXidSpaceExhausted& e) {

		logging::error << "[rofl][conn] sending FEATURES.request failed: no idle xid available" << std::endl << *this;

	} catch (RoflException& e) {

		logging::error << "[rofl][conn] sending FEATURES.request failed " << std::endl << *this;

	}
}



void
crofconn::action_send_echo_request()
{
	try {
		cofmsg_echo_request *echo =
				new cofmsg_echo_request(
						ofp_version,
						env->get_sync_xid(this, OFPT_ECHO_REQUEST));

		rofsock.send_message(echo);

	} catch (eRofConnXidSpaceExhausted& e) {

		logging::error << "[rofl][conn] sending ECHO.request failed: no idle xid available" << std::endl << *this;

	} catch (RoflException& e) {

		logging::error << "[rofl][conn] sending ECHO.request failed " << std::endl << *this;
	}
}



void
crofconn::handle_connect_refused(crofsock *endpnt)
{
	logging::warn << "[rofl][conn] OFP socket indicated connection refused." << std::endl << *this;
	run_engine(EVENT_DISCONNECTED);
}



void
crofconn::handle_connected (crofsock *endpnt)
{
	logging::warn << "[rofl][conn] OFP socket indicated connection established." << std::endl << *this;
	run_engine(EVENT_CONNECTED);
}



void
crofconn::handle_closed(crofsock *endpnt)
{
	logging::warn << "[rofl][conn] OFP socket indicated connection closed." << std::endl << *this;
	run_engine(EVENT_DISCONNECTED);
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
	case OFPT_FEATURES_REPLY: {
		features_reply_rcvd(msg);
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

	logging::info << "[rofl][conn] received HELLO message:" << std::endl << *hello;

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
			logging::info << "[rofl][conn] connection established." << std::endl << *this;
			run_engine(EVENT_HELLO_RCVD);
		}

	} catch (eHelloIncompatible& e) {

		logging::warn << "[rofl][conn] eHelloIncompatible " << *msg << std::endl;
		rofsock.send_message(
				new cofmsg_error_hello_failed_incompatible(
						hello->get_version(), hello->get_xid(), hello->soframe(), hello->framelen()));

		run_engine(EVENT_DISCONNECTED);

	} catch (eHelloEperm& e) {

		logging::warn << "[rofl][conn] eHelloEperm " << *msg << std::endl;
		rofsock.send_message(
				new cofmsg_error_hello_failed_eperm(
						hello->get_version(), hello->get_xid(), hello->soframe(), hello->framelen()));

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

		delete msg;

		rofsock.send_message(reply);

	} catch (RoflException& e) {

		logging::warn << "[rofl][conn] RoflException in echo_request_rcvd() " << *request << std::endl;
	}
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

		env->release_sync_xid(this, msg->get_xid());

		delete msg;

		run_engine(EVENT_ECHO_RCVD);

	} catch (RoflException& e) {

		logging::warn << "[rofl][conn] RoflException in echo_reply_rcvd() " << *reply << std::endl;
	}
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



void
crofconn::features_reply_rcvd(
		cofmsg *msg)
{
	cofmsg_features_reply *reply = dynamic_cast<cofmsg_features_reply*>( msg );

	try {
		if (NULL == reply) {
			logging::error << "[rofl][conn] invalid message rcvd in method features_reply_rcvd()" << std::endl << *msg;
			delete msg; return;
		}

		if (STATE_ESTABLISHED != state) {
			logging::debug << "[rofl][conn] rcvd FEATURES.reply:" << std::endl << *reply;

			dpid 			= reply->get_dpid();
			if (ofp_version >= rofl::openflow13::OFP_VERSION) {
				auxiliary_id 	= reply->get_auxiliary_id();
			} else {
				auxiliary_id 	= 0;
			}

			env->release_sync_xid(this, msg->get_xid());

			delete msg;

			run_engine(EVENT_FEATURES_RCVD);
		} else {
			env->recv_message(this, msg);
		}

	} catch (RoflException& e) {

		logging::warn << "[rofl][conn] RoflException in features_reply_rcvd() " << *reply << std::endl;
	}
}



void
crofconn::send_message(
		cofmsg *msg)
{
	rofsock.send_message(msg);
}



