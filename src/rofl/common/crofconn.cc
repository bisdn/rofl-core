/*
 * crofconn.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "crofconn.h"

using namespace rofl;

crofconn::crofconn(
		crofconn_env *env,
		rofl::openflow::cofhello_elem_versionbitmap const& versionbitmap) :
				env(env),
				dpid(0), // will be determined later via Features.request
				auxiliary_id(0), // same as dpid
				rofsock(NULL),
				versionbitmap(versionbitmap),
				ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN),
				fragmentation_threshold(DEFAULT_FRAGMENTATION_THRESHOLD),
				max_backoff(ctimespec(16, 0)),
				reconnect_start_timeout(ctimespec(0, CROFCONN_RECONNECT_START_TIMEOUT_IN_NSECS)),
				reconnect_timespec(ctimespec(0, CROFCONN_RECONNECT_START_TIMEOUT_IN_NSECS)),
				reconnect_variance(ctimespec(0, CROFCONN_RECONNECT_VARIANCE_IN_NSECS)),
				reconnect_counter(0),
				flavour(FLAVOUR_UNSPECIFIED),
				socket_type(rofl::csocket::SOCKET_TYPE_UNKNOWN),
				newsd(0),
				state(STATE_INIT),
				hello_timeout(DEFAULT_HELLO_TIMEOUT),
				echo_timeout(DEFAULT_ECHO_TIMEOUT),
				echo_interval(DEFAULT_ECHO_INTERVAL * (1 + crandom::draw_random_number()))
{
	for (unsigned int i = 0; i < QUEUE_MAX; i++) {
		rxqueues.push_back(crofqueue());
	}
	// scheduler weights for transmission
	weights[QUEUE_OAM ] = 4;
	weights[QUEUE_MGMT] = 8;
	weights[QUEUE_FLOW] = 4;
	weights[QUEUE_PKT ] = 2;
	//rofl::logging::debug << "[rofl][crofconn] constructor " << std::hex << this << std::dec << std::endl;
}



crofconn::~crofconn()
{
	env = NULL;
	//rofl::logging::debug << "[rofl][crofconn] destructor " << std::hex << this << std::dec << std::endl;
	if (STATE_DISCONNECTED != state) {
		run_engine(EVENT_DISCONNECTED);
	}
}



void
crofconn::set_max_backoff(
		const ctimespec& timespec)
{
	if (timespec < reconnect_start_timeout) {
		max_backoff = reconnect_start_timeout + reconnect_start_timeout;
	} else {
		max_backoff = timespec;
	}
}



void
crofconn::init_thread()
{
	rofl::logging::debug << "[rofl-common][rofconn] init thread" << std::endl;
	if (NULL == rofsock) {
		rofl::logging::debug << "[rofl-common][rofconn] creating new crofsock instance" << std::endl;
		rofsock = new crofsock(this);
	}

	//usleep(500000); // FIXME: do not ask why ...

	if (flags.test(FLAGS_PASSIVE)) {
		rofsock->accept(socket_type, socket_params, newsd);
		// notify main thread about established passive TCP connection to peer
		rofl::ciosrv::notify(rofl::cevent(EVENT_TCP_CONNECTED));
	} else {
		rofsock->connect(socket_type, socket_params);
	}
}



void
crofconn::release_thread()
{
	rofl::logging::debug << "[rofl-common][rofconn] release thread" << std::endl;
	if (NULL != rofsock) {
		rofl::logging::debug << "[rofl-common][rofconn] destroying crofsock instance" << std::endl;
		delete rofsock; rofsock = NULL;
	}
}



void
crofconn::accept(enum rofl::csocket::socket_type_t socket_type, cparams const& socket_params, int newsd, enum crofconn_flavour_t flavour)
{
	flags.reset();
	flags.set(FLAGS_PASSIVE);

	state = STATE_ACCEPT_PENDING;
	this->flavour = flavour;
	this->socket_type = socket_type;
	this->socket_params = socket_params;
	this->newsd = newsd;

	if (not cthread::is_running()) {
		cthread::start();
	}
}



void
crofconn::reconnect(
		bool reset_backoff_timer)
{
	backoff_reconnect(reset_backoff_timer);
}



void
crofconn::connect(
		const cauxid& aux_id,
		enum rofl::csocket::socket_type_t socket_type,
		const cparams& socket_params)
{
	if ((STATE_INIT != state) && (STATE_DISCONNECTED != state)) {
		throw eRofConnBusy();
	}

	flags.reset();
	//flags.reset(FLAGS_PASSIVE);

	state = STATE_CONNECT_PENDING;
	auxiliary_id = aux_id;
	this->socket_type = socket_type;
	this->socket_params = socket_params;

	backoff_reconnect(false);
}



void
crofconn::close()
{

	flags.set(FLAGS_LOCAL_DISCONNECT);
	run_engine(EVENT_DISCONNECTED);
}



void
crofconn::handle_timeout(
		int opaque, void *data)
{
	switch (opaque) {
	case TIMER_NEXT_RECONNECT: {
		run_engine(EVENT_RECONNECT);
	} break;
	case TIMER_WAIT_FOR_HELLO: {
		run_engine(EVENT_HELLO_EXPIRED);
	} break;
	case TIMER_NEED_LIFE_CHECK: {
		run_engine(EVENT_NEED_LIFE_CHECK);
	} break;
	case TIMER_WAIT_FOR_ECHO: {
		run_engine(EVENT_ECHO_EXPIRED);
	} break;
	case TIMER_WAIT_FOR_FEATURES: {
		run_engine(EVENT_FEATURES_EXPIRED);
	} break;
	default: {
		rofl::logging::warn << "[rofl-common][rofconn] unknown timer type:" << opaque << " rcvd" << std::endl << *this;
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
		case EVENT_RECONNECT:		event_reconnect();			break;
		case EVENT_TCP_CONNECTED: 	event_tcp_connected(); 		break;
		case EVENT_DISCONNECTED:	event_disconnected();		return; // might call this object's destructor
		case EVENT_HELLO_RCVD:		event_hello_rcvd();			break;
		case EVENT_HELLO_EXPIRED:	event_hello_expired();		return;
		case EVENT_FEATURES_RCVD:	event_features_rcvd();		break;
		case EVENT_FEATURES_EXPIRED:event_features_expired();	return;
		case EVENT_ECHO_RCVD:		event_echo_rcvd();			break;
		case EVENT_ECHO_EXPIRED:	event_echo_expired();		return;
		case EVENT_NEED_LIFE_CHECK: event_need_life_check();	break;
		default: {
			rofl::logging::error << "[rofl-common][rofconn] unknown event seen, internal error" << std::endl << *this;
		};
		}
	}
}



void
crofconn::event_reconnect()
{
	switch (state) {
	case STATE_CONNECTED: {
		rofl::logging::debug << "[rofl-common][rofconn] connection in state -established-" << std::endl;
		// do nothing
	} return;
	default: {
		state = STATE_CONNECT_PENDING;
		rofl::logging::debug << "[rofl-common][rofconn] reconnect: entering state -connect-pending-" << std::endl;
		if (not cthread::is_running()) {
			cthread::start();
		}
	};
	}
}



void
crofconn::event_tcp_connected()
{
	switch (state) {
	case STATE_INIT:
	case STATE_DISCONNECTED:
	case STATE_CONNECT_PENDING:
	case STATE_ACCEPT_PENDING:
	case STATE_WAIT_FOR_HELLO:
	case STATE_CONNECTED: {
		rofl::logging::debug << "[rofl-common][rofconn] TCP connection established." << std::endl;
		state = STATE_WAIT_FOR_HELLO;
		reconnect_timespec = reconnect_start_timeout;
		timer_start_wait_for_hello();
		timer_stop_next_reconnect();
		action_send_hello_message();
	} break;
	default: {
		rofl::logging::error << "[rofl-common][rofconn] event -CONNECTED- invalid state reached, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_disconnected()
{
	if (cthread::is_running()) {
		if (rofsock) rofsock->close();
		cthread::stop();
	}

	while (not timer_ids.empty()) {
		timer_stop(timer_ids.begin()->first);
	}

	// purge delay queue
	dlqueue.clear();

	switch (state) {
	case STATE_DISCONNECTED: {
		rofl::logging::debug << "[rofl-common][rofconn] connection in state -disconnected-" << std::endl;
	} break;
	case STATE_CONNECT_PENDING: {
		rofl::logging::debug << "[rofl-common][rofconn] entering state -disconnected-" << std::endl;
		state = STATE_DISCONNECTED;

		if (flags.test(FLAGS_CONNECT_REFUSED)) {
			if (env) env->handle_connect_refused(this); flags.reset(FLAGS_CONNECT_REFUSED);
		}
		if (flags.test(FLAGS_CONNECT_FAILED)) {
			if (env) env->handle_connect_failed(this); flags.reset(FLAGS_CONNECT_FAILED);
		}

	} break;
	case STATE_ACCEPT_PENDING:
	case STATE_WAIT_FOR_HELLO:
	case STATE_CONNECTED:
	default: {
		rofl::logging::debug << "[rofl-common][rofconn] entering state -disconnected-" << std::endl;
		state = STATE_DISCONNECTED;
		timer_stop_wait_for_echo();
		timer_stop_wait_for_hello();

		if (flags.test(FLAGS_LOCAL_DISCONNECT)) {
			flags.reset(FLAGS_LOCAL_DISCONNECT); //if (env) env->handle_closed(this); return; // this object may have been destroyed here
		}

		if (flags.test(FLAGS_PEER_DISCONNECTED)) {
			flags.reset(FLAGS_PEER_DISCONNECTED); if (env) env->handle_closed(this); return; // this object may have been destroyed here
		}
	};
	}
}



void
crofconn::event_hello_rcvd()
{
	switch (state) {
	case STATE_DISCONNECTED: {
		rofl::logging::debug << "[rofl-common][rofconn] entering state -wait-for-hello-" << std::endl;
		state = STATE_WAIT_FOR_HELLO;
		action_send_hello_message();
	} // FALLTHROUGH
	//case STATE_ACCEPT_PENDING:
	case STATE_WAIT_FOR_HELLO: {
		rofl::logging::debug << "[rofl-common][rofconn] negotiated OFP version:" << (int)ofp_version << std::endl;
		timer_stop_wait_for_hello();

		if (flags.test(FLAGS_PASSIVE)) {
			rofl::logging::debug << "[rofl-common][rofconn] entering state -wait-for-features-" << std::endl;
			state = STATE_WAIT_FOR_FEATURES;
			action_send_features_request();
			timer_start_wait_for_features();
		} else {
			rofl::logging::debug << "[rofl-common][rofconn] entering state -established-" << std::endl;
			state = STATE_CONNECTED;
			if (env) env->handle_connected(this, ofp_version);
		}

	} break;
	case STATE_CONNECTED: {
		rofl::logging::debug << "[rofl-common][rofconn] connection is in state -established-" << std::endl;
		// do nothing
	} break;
	default: {
		rofl::logging::error << "[rofl-common][rofconn] event -HELLO-RCVD- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_hello_expired()
{
	switch (state) {
	case STATE_WAIT_FOR_HELLO: {
		rofl::logging::debug << "[rofl-common][rofconn] event -HELLO-EXPIRED- occured in state -WAIT-FOR-HELLO-" << std::endl << *this;
		flags.set(FLAGS_PEER_DISCONNECTED);
		run_engine(EVENT_DISCONNECTED);

	} break;
	default: {
		rofl::logging::error << "[rofl-common][rofconn] event -HELLO-EXPIRED- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_features_rcvd()
{
	switch (state) {
	case STATE_WAIT_FOR_FEATURES: {
		if (flags.test(FLAGS_PASSIVE)) {
			rofl::logging::debug << "[rofl-common][rofconn] entering state -connected-" << std::endl;
			state = STATE_CONNECTED;
			cancel_timer(timer_ids[TIMER_WAIT_FOR_FEATURES]);
			timer_ids.erase(TIMER_WAIT_FOR_FEATURES);

			if (env) env->handle_connected(this, ofp_version);
		}

		// send all postponed messages to higher layers
		while (not dlqueue.empty()) {
			if (env) env->recv_message(this, dlqueue.retrieve());
		}

	} break;
	case STATE_CONNECTED: {
		// do nothing
	} break;
	default: {
		rofl::logging::error << "[rofl-common][rofconn] event -FEATURES-RCVD- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_features_expired()
{
	switch (state) {
	case STATE_WAIT_FOR_FEATURES: {
		rofl::logging::warn << "[rofl-common][rofconn] event -FEATURES-EXPIRED- occured in state -WAIT-FOR-FEATURES-" << std::endl << *this;
		flags.set(FLAGS_PEER_DISCONNECTED);
		run_engine(EVENT_DISCONNECTED);

	} break;
	default: {
		rofl::logging::debug << "[rofl-common][rofconn] event -FEATURES-EXPIRED- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}


void
crofconn::event_echo_rcvd()
{
	switch (state) {
	case STATE_CONNECTED: {
		timer_stop_wait_for_echo();
		rofl::logging::debug << "[rofl-common][rofconn] event-echo-rcvd: OFP transport connection is good." << std::endl << *this;
		// do not restart TIMER_NEED_LIFE_CHECK here => ::recv()

	} break;
	default: {
		rofl::logging::error << "[rofl-common][rofconn] event -ECHO.reply-RCVD- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_echo_expired()
{
	switch (state) {
	case STATE_CONNECTED: {
		rofl::logging::warn << "[rofl-common][rofconn] event-echo-expired: OFP transport connection is congested or dead. Closing. " << std::endl << *this;
		flags.set(FLAGS_PEER_DISCONNECTED);
		run_engine(EVENT_DISCONNECTED);

	} break;
	default: {
		rofl::logging::error << "[rofl-common][rofconn] event -ECHO.reply-EXPIRED- occured in invalid state, internal error" << std::endl << *this;
	};
	}
}



void
crofconn::event_need_life_check()
{
	rofl::logging::debug << "[rofl-common][rofconn] event-need-life-check" << std::endl;
	action_send_echo_request();
}



void
crofconn::action_send_hello_message()
{
	try {
		if (versionbitmap.get_highest_ofp_version() == rofl::openflow::OFP_VERSION_UNKNOWN) {
			rofl::logging::warn << "[rofl-common][rofconn] unable to send HELLO message, as no OFP versions are currently configured" << std::endl << *this;
			return;
		}


		cmemory body(0);

		switch (versionbitmap.get_highest_ofp_version()) {
		case rofl::openflow10::OFP_VERSION: {
			// do nothing, as there should be no padding to prevent NOX from crashing

		} break;
		default: {
			body.resize(versionbitmap.length());
			versionbitmap.pack(body.somem(), body.memlen());

		};
		}

		rofl::openflow::cofmsg_hello *hello =
				new rofl::openflow::cofmsg_hello(
						versionbitmap.get_highest_ofp_version(),
						env->get_async_xid(this),
						body.somem(), body.memlen());

		rofl::logging::debug << "[rofl-common][rofconn] sending HELLO.message:" << std::endl << *hello;

		if (rofsock) rofsock->send_message(hello);

	} catch (eRofConnXidSpaceExhausted& e) {

		rofl::logging::error << "[rofl-common][rofconn] sending HELLO.message failed: no idle xid available" << std::endl << *this;

	} catch (RoflException& e) {

		rofl::logging::error << "[rofl-common][rofconn] sending HELLO.message failed " << std::endl << *this;

	}
}



void
crofconn::action_send_features_request()
{
	try {
		cmemory body(versionbitmap.length());
		versionbitmap.pack(body.somem(), body.memlen());

		rofl::openflow::cofmsg_features_request *request =
				new rofl::openflow::cofmsg_features_request(ofp_version, env->get_async_xid(this));

		rofl::logging::debug << "[rofl-common][rofconn] sending FEATURES.request:" << std::endl << *request;

		if (rofsock) rofsock->send_message(request);

	} catch (eRofConnXidSpaceExhausted& e) {

		rofl::logging::error << "[rofl-common][rofconn] sending FEATURES.request failed: no idle xid available" << std::endl << *this;

	} catch (RoflException& e) {

		rofl::logging::error << "[rofl-common][rofconn] sending FEATURES.request failed " << std::endl << *this;

	}
}



void
crofconn::action_send_echo_request()
{
	try {
		rofl::openflow::cofmsg_echo_request *echo =
				new rofl::openflow::cofmsg_echo_request(
						ofp_version,
						env->get_sync_xid(this, OFPT_ECHO_REQUEST));

		rofl::logging::debug << "[rofl-common][rofconn] sending Echo.request" << std::endl;

		if (rofsock) rofsock->send_message(echo);

		timer_start_wait_for_echo();

	} catch (eRofConnXidSpaceExhausted& e) {

		rofl::logging::error << "[rofl-common][rofconn] sending ECHO.request failed: no idle xid available" << std::endl << *this;
		timer_start_life_check();

	} catch (eSocketTxAgain& e) {

		rofl::logging::error << "[rofl-common][rofconn] sending ECHO.request failed: socket full" << std::endl << *this;
		timer_start_life_check();

	} catch (RoflException& e) {

		rofl::logging::error << "[rofl-common][rofconn] sending ECHO.request failed " << std::endl << *this;
	}
}



void
crofconn::recv_message(
		crofsock *rofsock,
		rofl::openflow::cofmsg *msg) {
	rofl::logging::debug << "[rofl-common][rofconn][recv_message] received message" << std::endl << *msg;

	switch (msg->get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		switch (msg->get_type()) {
		case rofl::openflow10::OFPT_PACKET_IN:
		case rofl::openflow10::OFPT_PACKET_OUT: {
			rxqueues[QUEUE_PKT].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_PKT]:" << std::endl << rxqueues[QUEUE_PKT];
		} break;
		case rofl::openflow10::OFPT_FLOW_MOD:
		case rofl::openflow10::OFPT_FLOW_REMOVED: {
			rxqueues[QUEUE_FLOW].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_FLOW]:" << std::endl << rxqueues[QUEUE_FLOW];
		} break;
		case rofl::openflow10::OFPT_ECHO_REQUEST:
		case rofl::openflow10::OFPT_ECHO_REPLY: {
			rxqueues[QUEUE_OAM].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_OAM]:" << std::endl << rxqueues[QUEUE_OAM];
		} break;
		default: {
			rxqueues[QUEUE_MGMT].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_MGMT]:" << std::endl << rxqueues[QUEUE_MGMT];
		};
		}
	} break;
	case rofl::openflow12::OFP_VERSION: {
		switch (msg->get_type()) {
		case rofl::openflow12::OFPT_PACKET_IN:
		case rofl::openflow12::OFPT_PACKET_OUT: {
			rxqueues[QUEUE_PKT].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_PKT]:" << std::endl << rxqueues[QUEUE_PKT];
		} break;
		case rofl::openflow12::OFPT_FLOW_MOD:
		case rofl::openflow12::OFPT_FLOW_REMOVED:
		case rofl::openflow12::OFPT_GROUP_MOD:
		case rofl::openflow12::OFPT_PORT_MOD:
		case rofl::openflow12::OFPT_TABLE_MOD: {
			rxqueues[QUEUE_FLOW].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_FLOW]:" << std::endl << rxqueues[QUEUE_FLOW];
		} break;
		case rofl::openflow12::OFPT_ECHO_REQUEST:
		case rofl::openflow12::OFPT_ECHO_REPLY: {
			rxqueues[QUEUE_OAM].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_OAM]:" << std::endl << rxqueues[QUEUE_OAM];
		} break;
		default: {
			rxqueues[QUEUE_MGMT].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_MGMT]:" << std::endl << rxqueues[QUEUE_MGMT];
		};
		}
	} break;
	case rofl::openflow13::OFP_VERSION: {
		switch (msg->get_type()) {
		case rofl::openflow13::OFPT_PACKET_IN:
		case rofl::openflow13::OFPT_PACKET_OUT: {
			rxqueues[QUEUE_PKT].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_PKT]:" << std::endl << rxqueues[QUEUE_PKT];
		} break;
		case rofl::openflow13::OFPT_FLOW_MOD:
		case rofl::openflow13::OFPT_FLOW_REMOVED:
		case rofl::openflow13::OFPT_GROUP_MOD:
		case rofl::openflow13::OFPT_PORT_MOD:
		case rofl::openflow13::OFPT_TABLE_MOD: {
			rxqueues[QUEUE_FLOW].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_FLOW]:" << std::endl << rxqueues[QUEUE_FLOW];
		} break;
		case rofl::openflow13::OFPT_ECHO_REQUEST:
		case rofl::openflow13::OFPT_ECHO_REPLY: {
			rxqueues[QUEUE_OAM].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_OAM]:" << std::endl << rxqueues[QUEUE_OAM];
		} break;
		default: {
			rxqueues[QUEUE_MGMT].store(msg);
			rofl::logging::debug << "[rofl-common][rofconn][recv_message] rxqueues[QUEUE_MGMT]:" << std::endl << rxqueues[QUEUE_MGMT];
		};
		}
	} break;
	default: {
		rofl::logging::alert << "[rofl-common][rofsock] dropping message with unsupported OpenFlow version" << std::endl;
		delete msg; return;
	};
	}

	rofl::logging::debug << "[rofl-common][rofconn][recv_message] -EVENT-RXQUEUE-" << std::endl;
	rofl::ciosrv::notify(rofl::cevent(EVENT_RXQUEUE));
}



void
crofconn::handle_messages()
{
	flags.set(FLAGS_RXQUEUE_CONSUMING);

	for (unsigned int queue_id = 0; queue_id < QUEUE_MAX; ++queue_id) {

		rofl::logging::debug << "[rofl-common][rofconn][handle_messages] rxqueue:" << std::endl << rxqueues[queue_id];

		for (unsigned int num = 0; num < weights[queue_id]; ++num) {

			if (rxqueues[queue_id].empty()) {
				continue;
			}

			rofl::openflow::cofmsg* msg = (rofl::openflow::cofmsg*)0;

			if ((msg = rxqueues[queue_id].retrieve()) == NULL) {
				continue;
			}

			rofl::logging::debug << "[rofl-common][rofconn][handle_messages] reading message from rxqueue:" << std::endl << *msg;

			if (rofl::openflow::OFP_VERSION_UNKNOWN == msg->get_version()) {
				send_message(new rofl::openflow::cofmsg_error_bad_request_bad_version(
						get_version(), msg->get_xid(), msg->soframe(), msg->framelen()));
				rofl::logging::error << "[rofl-common][rofconn] received message with unknown version, dropping." << std::endl;
				delete msg; continue;
			}

			// reset timer for transmitting next Echo.request, if we have seen a life signal from our peer
			timer_start_life_check();

			switch (msg->get_type()) {
			case OFPT_HELLO: {
				hello_rcvd(msg);
			} break;
			case OFPT_ERROR: {
				error_rcvd(msg);
			} break;
			case OFPT_ECHO_REQUEST: {
				echo_request_rcvd(msg);
			} break;
			case OFPT_ECHO_REPLY: {
				echo_reply_rcvd(msg);
			} break;
			case OFPT_FEATURES_REPLY: {
				features_reply_rcvd(msg);
			} break;
			case OFPT_MULTIPART_REQUEST:
			case OFPT_MULTIPART_REPLY: {
				/*
				 * add multipart support here for receiving messages
				 */
				switch (msg->get_version()) {
				case rofl::openflow13::OFP_VERSION: {
					rofl::openflow::cofmsg_stats *stats = dynamic_cast<rofl::openflow::cofmsg_stats*>( msg );

					if (NULL == stats) {
						rofl::logging::warn << "[rofl-common][rofconn] dropping multipart message, invalid message type." << std::endl << *this;
						delete msg; continue;
					}

					// start new or continue pending transaction
					if (stats->get_stats_flags() & rofl::openflow13::OFPMPF_REQ_MORE) {

						sar.set_transaction(msg->get_xid()).store_and_merge_msg(dynamic_cast<rofl::openflow::cofmsg_stats const&>(*msg));
						delete msg; // delete msg here, we store a copy in the transaction

					// end pending transaction or multipart message with single message only
					} else {

						if (sar.has_transaction(msg->get_xid())) {

							sar.set_transaction(msg->get_xid()).store_and_merge_msg(dynamic_cast<rofl::openflow::cofmsg_stats const&>(*msg));

							rofl::openflow::cofmsg* reassembled_msg = sar.set_transaction(msg->get_xid()).retrieve_and_detach_msg();

							sar.drop_transaction(msg->get_xid());

							delete msg; // delete msg here, we may get an exception from the next line

							if (env) env->recv_message(this, reassembled_msg);

						} else {
							// do not delete msg here, will be done by higher layers
							if (env) env->recv_message(this, msg);

						}

					}

				} break;
				default: {
					// no segmentation and reassembly below OF13, so send message directly to rofchan
					if (env) env->recv_message(this, msg);
				};
				}


			} break;
			default: {
				if (state != STATE_CONNECTED) {
					rofl::logging::warn << "[rofl-common][rofconn] delaying message, connection not fully established." << std::endl << *this;
					dlqueue.store(msg); return;
				} else {
					if (env) env->recv_message(this, msg);
				}
			} break;
			}

		}
	}

	flags.reset(FLAGS_RXQUEUE_CONSUMING);
}



void
crofconn::hello_rcvd(
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_hello *hello = dynamic_cast<rofl::openflow::cofmsg_hello*>( msg );

	if (NULL == hello) {
		rofl::logging::debug << "[rofl-common][rofconn] invalid message rcvd in method hello_rcvd()" << std::endl << *msg;
		delete msg; return;
	}

	rofl::logging::debug << "[rofl-common][rofconn] received HELLO message:" << std::endl << *hello;

	timer_stop_wait_for_hello();

	try {

		/* Step 1: extract version information from HELLO message */

		versionbitmap_peer.clear();

		switch (hello->get_version()) {
		case openflow10::OFP_VERSION:
		case openflow12::OFP_VERSION: {
			versionbitmap_peer.add_ofp_version(hello->get_version());
		} break;
		default: { // msg->get_version() should contain the highest number of supported OFP versions encoded in versionbitmap
			rofl::openflow::cofhelloelems helloIEs(hello->get_body());
			if (not helloIEs.has_hello_elem_versionbitmap()) {
				rofl::logging::warn << "[rofl-common][rofconn] HELLO message rcvd without HelloIE -VersionBitmap-" << std::endl << *hello << std::endl;
				versionbitmap_peer.add_ofp_version(hello->get_version());
			} else {
				versionbitmap_peer = helloIEs.get_hello_elem_versionbitmap();
				// sanity check
				if (not versionbitmap_peer.has_ofp_version(hello->get_version())) {
					rofl::logging::warn << "[rofl-common][rofconn] malformed HelloIE -VersionBitmap- => " <<
							"does not contain version defined in OFP message header:" <<
							(int)hello->get_version() << std::endl << *hello;
				}
			}
		};
		}

		/* Step 2: select highest supported protocol version on both sides */

		rofl::openflow::cofhello_elem_versionbitmap versionbitmap_common = versionbitmap & versionbitmap_peer;
		if (versionbitmap_common.get_highest_ofp_version() == rofl::openflow::OFP_VERSION_UNKNOWN) {
			rofl::logging::warn << "[rofl-common][rofconn] no common OFP version found for peer" << std::endl;
			rofl::logging::warn << "local version-bitmap:" << std::endl << indent(2) << versionbitmap;
			rofl::logging::warn << "remote version-bitmap:" << std::endl << indent(2) << versionbitmap_peer;
			throw eHelloIncompatible();
		}

		ofp_version = versionbitmap_common.get_highest_ofp_version();

		rofl::logging::debug << "[rofl-common][rofconn] negotiated OFP version:" << (int)ofp_version << std::endl << *this;

		rofl::logging::debug << "[rofl-common][rofconn] local version-bitmap:" << std::endl << versionbitmap;
		rofl::logging::debug << "[rofl-common][rofconn]  peer version-bitmap:" << std::endl << versionbitmap_peer;

		// move on state machine
		if (ofp_version == rofl::openflow::OFP_VERSION_UNKNOWN) {
			rofl::logging::warn << "[rofl-common][rofconn] no common OFP version supported, closing connection." << std::endl << *this;
			run_engine(EVENT_DISCONNECTED);
		} else {
			run_engine(EVENT_HELLO_RCVD);
		}

	} catch (eHelloIncompatible& e) {

		rofl::logging::warn << "[rofl-common][rofconn] eHelloIncompatible " << *msg << std::endl;
		if (rofsock) rofsock->send_message(
				new rofl::openflow::cofmsg_error_hello_failed_incompatible(
						hello->get_version(), hello->get_xid(), hello->soframe(), hello->framelen()));

		run_engine(EVENT_DISCONNECTED);

	} catch (eHelloEperm& e) {

		rofl::logging::warn << "[rofl-common][rofconn] eHelloEperm " << *msg << std::endl;
		if (rofsock) rofsock->send_message(
				new rofl::openflow::cofmsg_error_hello_failed_eperm(
						hello->get_version(), hello->get_xid(), hello->soframe(), hello->framelen()));

		run_engine(EVENT_DISCONNECTED);

	} catch (RoflException& e) {

		rofl::logging::warn << "[rofl-common][rofconn] RoflException " << *msg << std::endl;

		run_engine(EVENT_DISCONNECTED);
	}

	delete msg;
}



void
crofconn::echo_request_rcvd(
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_echo_request *request = dynamic_cast<rofl::openflow::cofmsg_echo_request*>( msg );

	try {
		if (NULL == request) {
			rofl::logging::debug << "[rofl-common][rofconn] invalid message rcvd in method echo_request_rcvd()" << std::endl << *msg;
			delete msg; return;
		}

		if (request->get_version() != get_version()) {

			if (rofsock) rofsock->send_message(new rofl::openflow::cofmsg_error_bad_request_bad_version(
					get_version(), request->get_xid(), request->soframe(), request->framelen()));
			return;
		}

		rofl::openflow::cofmsg_echo_reply *reply =
				new rofl::openflow::cofmsg_echo_reply(request->get_version(), request->get_xid(),
						request->get_body().somem(), request->get_body().memlen());

		delete msg;

		if (rofsock) rofsock->send_message(reply);

	} catch (RoflException& e) {

		rofl::logging::warn << "[rofl-common][rofconn] RoflException in echo_request_rcvd() " << *request << std::endl;
	}
}



void
crofconn::echo_reply_rcvd(
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_echo_reply *reply = dynamic_cast<rofl::openflow::cofmsg_echo_reply*>( msg );

	try {
		if (NULL == reply) {
			rofl::logging::debug << "[rofl-common][rofconn] invalid message rcvd in method echo_reply_rcvd()" << std::endl << *msg;
			delete msg; return;
		}

		rofl::logging::debug << "[rofl-common][rofconn] received Echo.reply" << std::endl;

		if (env) env->release_sync_xid(this, msg->get_xid());

		if (msg->get_version() != get_version()) {
			rofl::logging::error << "[rofl-common][rofconn] received echo-reply with invalid version field" << std::endl;
			/* this will lead to an expiration event and a disconnect */
			return;
		}

		delete msg;

		run_engine(EVENT_ECHO_RCVD);

	} catch (RoflException& e) {

		rofl::logging::warn << "[rofl-common][rofconn] RoflException in echo_reply_rcvd() " << *reply << std::endl;
	}
}



void
crofconn::error_rcvd(
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_error *error = dynamic_cast<rofl::openflow::cofmsg_error*>( msg );

	try {
		if (NULL == error) {
			rofl::logging::debug << "[rofl-common][rofconn] invalid message rcvd in method error_rcvd()" << std::endl << *msg;
			delete msg; return;
		}

		switch (error->get_err_type()) {
		case openflow13::OFPET_HELLO_FAILED: {

			switch (error->get_err_code()) {
			case openflow13::OFPHFC_INCOMPATIBLE: {
				rofl::logging::warn << "[rofl-common][rofconn] HELLO-INCOMPATIBLE.error rcvd, closing connection." << std::endl << *this;
			} break;
			case openflow13::OFPHFC_EPERM: {
				rofl::logging::warn << "[rofl-common][rofconn] HELLO-EPERM.error rcvd, closing connection." << std::endl << *this;
			} break;
			default: {
				rofl::logging::warn << "[rofl-common][rofconn] HELLO.error rcvd, closing connection." << std::endl << *this;
			};
			}

			delete error;

			run_engine(EVENT_DISCONNECTED);
		} break;
		default: {
			if (env) env->recv_message(this, error);
		};
		}

	} catch (RoflException& e) {

		rofl::logging::warn << "[rofl-common][rofconn] RoflException in error_rcvd() " << std::endl;
	}
}



void
crofconn::features_reply_rcvd(
		rofl::openflow::cofmsg *msg)
{
	rofl::openflow::cofmsg_features_reply *reply = dynamic_cast<rofl::openflow::cofmsg_features_reply*>( msg );

	try {
		if (NULL == reply) {
			rofl::logging::error << "[rofl-common][rofconn] invalid message rcvd in method features_reply_rcvd()" << std::endl << *msg;
			delete msg; return;
		}

		if (STATE_CONNECTED != state) {
			rofl::logging::debug << "[rofl-common][rofconn] rcvd FEATURES.reply:" << std::endl << *reply;

			dpid 			= reply->get_dpid();
			if (ofp_version >= rofl::openflow13::OFP_VERSION) {
				auxiliary_id 	= reply->get_auxiliary_id();
			} else {
				auxiliary_id 	= 0;
			}

			if (env) env->release_sync_xid(this, msg->get_xid());

			delete msg;

			reconnect_timespec = reconnect_start_timeout + reconnect_variance;

			run_engine(EVENT_FEATURES_RCVD);
		} else {
			if (env) env->recv_message(this, msg);
		}

	} catch (RoflException& e) {

		rofl::logging::warn << "[rofl-common][rofconn] RoflException in features_reply_rcvd() " << std::endl;
	}
}


unsigned int
crofconn::fragment_and_send_message(
		rofl::openflow::cofmsg *msg)
{
	unsigned int cwnd_size = 0;

	if (msg->length() <= fragmentation_threshold) {
		cwnd_size = rofsock->send_message(msg); // default behaviour for now: send message directly to rofsock

	} else {

		// fragment the packet
		switch (msg->get_version()) {
		case rofl::openflow12::OFP_VERSION: {

			switch (msg->get_type()) {
			case rofl::openflow12::OFPT_STATS_REPLY: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_reply*>( msg )->get_stats_type()) {
				case rofl::openflow12::OFPST_FLOW: {
					cwnd_size = fragment_flow_stats_reply(dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_TABLE: {
					cwnd_size = fragment_table_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_QUEUE: {
					cwnd_size = fragment_queue_stats_reply(dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_GROUP: {
					cwnd_size = fragment_group_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_stats_reply*>(msg));
				} break;
				case rofl::openflow12::OFPST_GROUP_DESC: {
					cwnd_size = fragment_group_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply*>(msg));
				} break;
				}

			} break;
			}
		} break;
		case rofl::openflow13::OFP_VERSION: {

			switch (msg->get_type()) {
			case rofl::openflow13::OFPT_MULTIPART_REQUEST: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_request*>( msg )->get_stats_type()) {
				case rofl::openflow13::OFPMP_TABLE_FEATURES: {
					cwnd_size = fragment_table_features_stats_request(dynamic_cast<rofl::openflow::cofmsg_table_features_stats_request*>(msg));
				} break;
				}

			} break;
			case rofl::openflow13::OFPT_MULTIPART_REPLY: {

				switch (dynamic_cast<rofl::openflow::cofmsg_stats_reply*>( msg )->get_stats_type()) {
				case rofl::openflow13::OFPMP_FLOW: {
					cwnd_size = fragment_flow_stats_reply(dynamic_cast<rofl::openflow::cofmsg_flow_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_TABLE: {
					cwnd_size = fragment_table_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_PORT_STATS: {
					cwnd_size = fragment_port_stats_reply(dynamic_cast<rofl::openflow::cofmsg_port_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_QUEUE: {
					cwnd_size = fragment_queue_stats_reply(dynamic_cast<rofl::openflow::cofmsg_queue_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_GROUP: {
					cwnd_size = fragment_group_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_GROUP_DESC: {
					cwnd_size = fragment_group_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_group_desc_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_TABLE_FEATURES: {
					cwnd_size = fragment_table_features_stats_reply(dynamic_cast<rofl::openflow::cofmsg_table_features_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_PORT_DESC: {
					cwnd_size = fragment_port_desc_stats_reply(dynamic_cast<rofl::openflow::cofmsg_port_desc_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_METER: {
					cwnd_size = fragment_meter_stats_reply(dynamic_cast<rofl::openflow::cofmsg_meter_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_METER_CONFIG: {
					cwnd_size = fragment_meter_config_stats_reply(dynamic_cast<rofl::openflow::cofmsg_meter_config_stats_reply*>(msg));
				} break;
				case rofl::openflow13::OFPMP_METER_FEATURES: {
					// no array in meter-features, so no need to fragment
					if (rofsock) {
						cwnd_size = rofsock->send_message(msg); // default behaviour for now: send message directly to rofsock
					}
				} break;
				}

			} break;
			}
		} break;
		}
	}

	if (0 == cwnd_size) {
		flags.set(FLAGS_CONGESTED);
	} else {
		flags.reset(FLAGS_CONGESTED);
	}

	return cwnd_size;
}



unsigned int
crofconn::fragment_table_features_stats_request(
		rofl::openflow::cofmsg_table_features_stats_request *msg)
{
	rofl::openflow::coftables tables;
	std::vector<rofl::openflow::cofmsg_table_features_stats_request*> fragments;

	for (std::map<uint8_t, rofl::openflow::coftable_features>::const_iterator
			it = msg->get_tables().get_tables().begin(); it != msg->get_tables().get_tables().end(); ++it) {

		tables.set_table(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::coftable_features elements in tables per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_table_features_stats_request(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REQ_MORE,
						tables));

		tables.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REQ_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_table_features_stats_request*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_flow_stats_reply(
		rofl::openflow::cofmsg_flow_stats_reply *msg)
{
	rofl::openflow::cofflowstatsarray flowstats;
	std::vector<rofl::openflow::cofmsg_flow_stats_reply*> fragments;

	for (std::map<uint32_t, rofl::openflow::cofflow_stats_reply>::const_iterator
			it = msg->get_flow_stats_array().get_flow_stats().begin(); it != msg->get_flow_stats_array().get_flow_stats().end(); ++it) {

		flowstats.set_flow_stats(it->first) = it->second;

		/*
		 * TODO: put more cofflow_stats_reply elements in flowstats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_flow_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						flowstats));

		flowstats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_flow_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_table_stats_reply(
		rofl::openflow::cofmsg_table_stats_reply *msg)
{
	rofl::openflow::coftablestatsarray tablestats;
	std::vector<rofl::openflow::cofmsg_table_stats_reply*> fragments;

	for (std::map<uint8_t, rofl::openflow::coftable_stats_reply>::const_iterator
			it = msg->get_table_stats_array().get_table_stats().begin(); it != msg->get_table_stats_array().get_table_stats().end(); ++it) {

		tablestats.set_table_stats(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::coftable_stats_reply elements in tablestats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_table_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						tablestats));

		tablestats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_table_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_port_stats_reply(
		rofl::openflow::cofmsg_port_stats_reply *msg)
{
	rofl::openflow::cofportstatsarray portstats;
	std::vector<rofl::openflow::cofmsg_port_stats_reply*> fragments;

	for (std::map<uint32_t, rofl::openflow::cofport_stats_reply>::const_iterator
			it = msg->get_port_stats_array().get_port_stats().begin(); it != msg->get_port_stats_array().get_port_stats().end(); ++it) {

		portstats.set_port_stats(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::cofport_stats_reply elements in portstats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_port_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						portstats));

		portstats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_port_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_queue_stats_reply(
		rofl::openflow::cofmsg_queue_stats_reply *msg)
{
	rofl::openflow::cofqueuestatsarray queuestats;
	std::vector<rofl::openflow::cofmsg_queue_stats_reply*> fragments;

	for (std::map<uint32_t, std::map<uint32_t, rofl::openflow::cofqueue_stats_reply> >::const_iterator
			it = msg->get_queue_stats_array().get_queue_stats().begin(); it != msg->get_queue_stats_array().get_queue_stats().end(); ++it) {

		for (std::map<uint32_t, rofl::openflow::cofqueue_stats_reply>::const_iterator
				jt = it->second.begin(); jt != it->second.end(); ++jt) {
			queuestats.set_queue_stats(it->first, jt->first) = jt->second;
		}

		/*
		 * TODO: put more cofqueue_stats_reply elements in queuestats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_queue_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						queuestats));

		queuestats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_queue_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_group_stats_reply(
		rofl::openflow::cofmsg_group_stats_reply *msg)
{
	rofl::openflow::cofgroupstatsarray groupstats;
	std::vector<rofl::openflow::cofmsg_group_stats_reply*> fragments;

	for (std::map<uint32_t, rofl::openflow::cofgroup_stats_reply>::const_iterator
			it = msg->get_group_stats_array().get_group_stats().begin(); it != msg->get_group_stats_array().get_group_stats().end(); ++it) {

		groupstats.set_group_stats(it->first) = it->second;

		/*
		 * TODO: put more cofgroup_stats_reply elements in groupstats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_group_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						groupstats));

		groupstats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_group_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_group_desc_stats_reply(
		rofl::openflow::cofmsg_group_desc_stats_reply *msg)
{
	rofl::openflow::cofgroupdescstatsarray groupdescstats;
	std::vector<rofl::openflow::cofmsg_group_desc_stats_reply*> fragments;

	for (std::map<uint32_t, rofl::openflow::cofgroup_desc_stats_reply>::const_iterator
			it = msg->get_group_desc_stats_array().get_group_desc_stats().begin(); it != msg->get_group_desc_stats_array().get_group_desc_stats().end(); ++it) {

		groupdescstats.set_group_desc_stats(it->first) = it->second;

		/*
		 * TODO: put more cofgroup_desc_stats_reply elements in group_descstats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_group_desc_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						groupdescstats));

		groupdescstats.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_group_desc_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_table_features_stats_reply(
		rofl::openflow::cofmsg_table_features_stats_reply *msg)
{
	rofl::openflow::coftables tables;
	std::vector<rofl::openflow::cofmsg_table_features_stats_reply*> fragments;

	for (std::map<uint8_t, rofl::openflow::coftable_features>::const_iterator
			it = msg->get_tables().get_tables().begin(); it != msg->get_tables().get_tables().end(); ++it) {

		tables.set_table(it->first) = it->second;

		/*
		 * TODO: put more rofl::openflow::coftable_features elements in tables per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_table_features_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						tables));

		tables.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_table_features_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_port_desc_stats_reply(
		rofl::openflow::cofmsg_port_desc_stats_reply *msg)
{
	rofl::openflow::cofports ports;
	std::vector<rofl::openflow::cofmsg_port_desc_stats_reply*> fragments;

	for (std::map<uint32_t, rofl::openflow::cofport*>::const_iterator
			it = msg->get_ports().get_ports().begin(); it != msg->get_ports().get_ports().end(); ++it) {

		ports.add_port(it->first) = *(it->second);

		/*
		 * TODO: put more rofl::openflow::cofport_desc_stats_reply elements in port_descstats per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_port_desc_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						ports));

		ports.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_port_desc_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_meter_stats_reply(
		rofl::openflow::cofmsg_meter_stats_reply *msg)
{
	unsigned int index = 0;
	rofl::openflow::cofmeterstatsarray array;
	std::vector<rofl::openflow::cofmsg_meter_stats_reply*> fragments;

	for (std::map<unsigned int, rofl::openflow::cofmeter_stats_reply>::const_iterator
			it = msg->get_meter_stats_array().get_mstats().begin();
					it != msg->get_meter_stats_array().get_mstats().end(); ++it) {

		array.add_meter_stats(index++) = it->second;

		/*
		 * TODO: put more rofl::openflow::cofmeter_stats_reply elements in array per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_meter_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						array));

		array.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_meter_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



unsigned int
crofconn::fragment_meter_config_stats_reply(
		rofl::openflow::cofmsg_meter_config_stats_reply *msg)
{
	unsigned int index = 0;
	rofl::openflow::cofmeterconfigarray array;
	std::vector<rofl::openflow::cofmsg_meter_config_stats_reply*> fragments;

	for (std::map<unsigned int, rofl::openflow::cofmeter_config_reply>::const_iterator
			it = msg->get_meter_config_array().get_mconfig().begin();
					it != msg->get_meter_config_array().get_mconfig().end(); ++it) {

		array.add_meter_config(index++) = it->second;

		/*
		 * TODO: put more rofl::openflow::cofmeter_config_reply elements in array per round
		 */

		fragments.push_back(
				new rofl::openflow::cofmsg_meter_config_stats_reply(
						msg->get_version(),
						msg->get_xid(),
						msg->get_stats_flags() | rofl::openflow13::OFPMPF_REPLY_MORE,
						array));

		array.clear();
	}

	// clear MORE flag on last fragment
	if (not fragments.empty()) {
		fragments.back()->set_stats_flags(fragments.back()->get_stats_flags() & ~rofl::openflow13::OFPMPF_REPLY_MORE);
	}

	unsigned int cwnd_size = 0;

	for (std::vector<rofl::openflow::cofmsg_meter_config_stats_reply*>::iterator
			it = fragments.begin(); it != fragments.end(); ++it) {
		 if (rofsock) {
			 cwnd_size = rofsock->send_message(*it);
		 }
	}

	delete msg;

	return cwnd_size;
}



void
crofconn::timer_start(
		crofconn_timer_t type, const ctimespec& timespec)
{
	timer_stop(type);
	ctimerid const& tid = register_timer(type, timespec);
	timer_ids[type] = tid;
#if 0
	rofl::logging::debug << "[rofl-common][rofconn] timer-start, registered timer-id: " << std::endl << tid;
	rofl::logging::debug << "[rofl-common][rofconn] timer-start, registered timer-id: " << std::endl << timer_ids[type];

	rofl::logging::debug << "[rofl-common][rofconn] timer-start: " << timer_ids.size() << " <=======================>" << std::endl;
	rofl::indent i(2);
	for (std::map<crofconn_timer_t, ctimerid>::iterator
			it = timer_ids.begin(); it != timer_ids.end(); ++it) {
		rofl::logging::debug << "[rofl-common][rofconn] timer-type: " << it->first << std::endl << it->second;
	}
#endif
}



void
crofconn::timer_stop(
		crofconn_timer_t type)
{
	if (timer_ids.find(type) == timer_ids.end()) {
		return;
	}
#if 0
	rofl::logging::debug << "[rofl-common][rofconn] timer-stop, cancel timer-id: " << std::endl << timer_ids[type];
#endif
	cancel_timer(timer_ids[type]);
	timer_ids.erase(type);
#if 0
	rofl::logging::debug << "[rofl-common][rofconn] timer-stop: " << timer_ids.size() << " <========================>" << std::endl;
	rofl::indent i(2);
	for (std::map<crofconn_timer_t, ctimerid>::iterator
			it = timer_ids.begin(); it != timer_ids.end(); ++it) {
		rofl::logging::debug << "[rofl-common][rofconn] timer-type: " << it->first << std::endl << it->second;
	}
#endif
}



void
crofconn::backoff_reconnect(bool reset_timeout)
{
	timer_stop_next_reconnect();

	//if ((not flags.test(FLAGS_RECONNECTING)) || (reset_timeout)) {
	if (reset_timeout) {

		reconnect_variance.set_timespec().tv_sec *= crandom::draw_random_number();
		reconnect_variance.set_timespec().tv_nsec *= crandom::draw_random_number();
		reconnect_timespec = reconnect_start_timeout + reconnect_variance;
		reconnect_counter = 0;

	} else {
		reconnect_timespec += reconnect_timespec;

		if (reconnect_timespec > max_backoff) {
			reconnect_timespec = max_backoff;
		}
	}

	rofl::logging::debug << "[rofl-common][rofconn][backoff] " << " scheduled reconnect in: " << std::endl << reconnect_timespec;

	timer_start_next_reconnect();

	++reconnect_counter;

	//flags.set(FLAGS_RECONNECTING);
}


