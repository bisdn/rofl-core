/*
 * queuetest.cc
 *
 *  Created on: 04.05.2013
 *      Author: andi
 */

#include "queuetest.h"


queuetest::queuetest()
{

}



queuetest::~queuetest()
{

}



void
queuetest::handle_timeout(
		int opaque)
{
	switch (opaque) {
	case QUEUETEST_TIMER_GET_CONFIG_INTERVAL: {
		for (std::set<cofdpt*>::iterator
				it = dpaths.begin(); it != dpaths.end(); ++it) {
			send_queue_get_config_request((*it), OFPQ_ALL);
		}
		register_timer(QUEUETEST_TIMER_GET_CONFIG_INTERVAL, 15);
	} break;
	default: {
		crofbase::handle_timeout(opaque);
	} break;
	}
}



void
queuetest::handle_dpath_open(
		cofdpt *dpt)
{
	if (dpaths.empty()) {
		reset_timer(QUEUETEST_TIMER_GET_CONFIG_INTERVAL, 60);
		reset_timer(QUEUETEST_TIMER_STATS_INTERVAL, 15);
	}
	dpaths.insert(dpt);
}



void
queuetest::handle_dpath_close(
		cofdpt *dpt)
{
	dpaths.erase(dpt);
}



void
queuetest::handle_queue_get_config_reply(
			cofdpt *dpt,
			cofmsg_queue_get_config_reply *msg)
{
	fprintf(stderr, "queue-get-config-reply: msg:%p\n", msg);

	delete msg;
}


