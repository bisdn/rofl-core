/*
 * cthread.h
 *
 *  Created on: 26.10.2014
 *      Author: andreas
 */

#ifndef CTHREAD_H_
#define CTHREAD_H_

#include <pthread.h>

#include <bitset>

#include <rofl/common/ciosrv.h>
#include <rofl/common/croflexception.h>
#include <rofl/common/logging.h>

namespace rofl {
namespace common {

class eRofThreadBase : public RoflException {
public:
	eRofThreadBase(const std::string& __arg) : RoflException(__arg) {};
};
class eRofThreadFailed : public eRofThreadBase {
public:
	eRofThreadFailed(const std::string& __arg) : eRofThreadBase(__arg) {};
};

class cthread {

	enum cthread_flag_t {
		FLAGS_THREAD_IS_RUNNING = 1,
	};

public:

	/**
	 *
	 */
	cthread() : tid(0), result(0) {};

	/**
	 *
	 */
	virtual
	~cthread() {};

protected:

	/**
	 *
	 */
	bool
	is_running() const
	{ return flags.test(FLAGS_THREAD_IS_RUNNING); };

	/**
	 *
	 */
	void
	set_thread_id(pthread_t tid) { this->tid = tid; };

	/**
	 *
	 */
	pthread_t
	get_thread_id() const { return tid; };

	/**
	 *
	 */
	void
	set_result(int result) { this->result = result; };

	/**
	 *
	 */
	const int&
	get_result() const { return result; };

protected:

	/**
	 *
	 */
	virtual void
	init_thread() {}; // to be overwritten by derived class

	/**
	 *
	 */
	virtual void
	release_thread() {}; // to be overwritten by derived class

	/**
	 *
	 */
	void
	start(const pthread_attr_t* attr = (const pthread_attr_t*)0) {
		LOGGING_DEBUG2 << "[rofl-common][cthread][start_thread] create" << std::endl;
		int rc = pthread_create(&tid, attr, cthread::run_thread, (void*)this);
		LOGGING_DEBUG2 << "[rofl-common][cthread][start_thread] create done tid: 0x" << tid << std::endl;

		if (rc != 0) {
			switch (rc) {
			case EAGAIN: {
				throw eRofThreadFailed("cthread::start_thread() insufficient resources");
			} break;
			case EINVAL: {
				throw eRofThreadFailed("cthread::start_thread() invalid thread attributes");
			} break;
			case EPERM: {
				throw eRofThreadFailed("cthread::start_thread() permission denied");
			} break;
			default: {
				throw eRofThreadFailed("cthread::start_thread() unknown error occured");
			};
			}
		} else {
			flags.set(FLAGS_THREAD_IS_RUNNING);
		}
	};

	/**
	 *
	 */
	void
	stop() {
		LOGGING_DEBUG2 << "[rofl-common][cthread][stop_thread] stop cioloop tid: 0x" << tid << std::endl;
		rofl::cioloop::get_loop(get_thread_id()).stop();
		LOGGING_DEBUG2 << "[rofl-common][cthread][stop_thread] join tid: 0x" << tid << std::endl;
		pthread_cancel(tid);
		void* retval;
		int rc = pthread_join(tid, &retval);
		LOGGING_DEBUG2 << "[rofl-common][cthread][stop_thread] join done tid: 0x" << tid << std::endl;

		if (rc != 0) {
			switch (rc) {
			case EDEADLK: {
				throw eRofThreadFailed("cthread::stop_thread() -join- deadlock");
			} break;
			case EINVAL: {
				throw eRofThreadFailed("cthread::stop_thread() -join- thread not joinable");
			} break;
			case ESRCH: {
				//throw eRofThreadFailed("cthread::stop_thread() -join- thread not found");
			} break;
			default: {
				throw eRofThreadFailed("cthread::stop_thread() -join- unknown error");
			};
			}
		}

		release_thread();

		rofl::cioloop::drop_loop(tid);

		flags.reset(FLAGS_THREAD_IS_RUNNING);
	};

private:

	/**
	 *
	 */
	static void*
	run_thread(void* arg) {
		cthread& thread = *(static_cast<cthread*>( arg ));
		thread.set_thread_id(pthread_self());

		thread.init_thread();

		rofl::cioloop::get_loop(thread.get_thread_id()).run();

		return (void*)&(thread.get_result());
	};

protected:

	std::bitset<32>	flags;
	pthread_t 		tid;
	int				result;
};

}; // end of namespace common
}; // end of namespace rofl

#endif /* CTHREAD_H_ */
