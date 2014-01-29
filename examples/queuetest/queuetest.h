/*
 * queuetest.h
 *
 *  Created on: 04.05.2013
 *      Author: andi
 */

#ifndef QUEUETEST_H_
#define QUEUETEST_H_

#include <rofl/common/crofbase.h>

using namespace rofl;

class queuetest :
		public crofbase
{
	enum queuetest_timer_t {
		QUEUETEST_TIMER_GET_CONFIG_INTERVAL = (0xe431 << 16),
		QUEUETEST_TIMER_STATS_INTERVAL,
	};

	std::set<crofdpt*> 	dpaths;

public:

	queuetest(cofhello_elem_versionbitmap const& versionbitmap);

	virtual
	~queuetest();

public:

	virtual void
	handle_timeout(
			int opaque, void *data = (void*)0);

public:

	virtual void
	handle_dpath_open(
			crofdpt *dpt);

	virtual void
	handle_dpath_close(
			crofdpt *dpt);

	virtual void
	handle_queue_get_config_reply(
			crofdpt *dpt,
			cofmsg_queue_get_config_reply *msg);

	virtual void
	handle_queue_stats_reply(
			crofdpt *dpt,
			cofmsg_queue_stats_reply *msg);
};



#endif /* QUEUETEST_H_ */
