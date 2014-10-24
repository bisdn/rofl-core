/*
 * crofshim.h
 *
 *  Created on: 22.01.2014
 *      Author: andi
 */

#ifndef CROFSHIM_H_
#define CROFSHIM_H_

#include <map>

#include "rofl/common/croflexception.h"
#include "rofl/common/crofsock.h"
#include "rofl/common/csocket.h"
#include "rofl/common/caddress.h"
#include "rofl/common/ctspaddress.h"

namespace rofl {
namespace common {

class eRofShimBase : public RoflException {
public:
	eRofShimBase(const std::string& __arg) : RoflException(__arg) {};
};
class eRofShimNotFound : public eRofShimBase {
public:
	eRofShimNotFound(const std::string& __arg) : eRofShimBase(__arg) {};
};


class crofshim :
		public crofsock_env,
		public csocket_owner
{
public:

	/**
	 *
	 */
	crofshim() {};

	/**
	 *
	 */
	virtual
	~crofshim() {
		clear_active_rofsockets_in4();
		clear_active_rofsockets_in6();
		clear_listening_sockets_in4();
		clear_listening_sockets_in6();
	};

public:

	/**
	 *
	 */
	void
	connect(enum rofl::csocket::socket_type_t socket_type, cparams const& socket_params) {
		(new crofsock(this))->connect(socket_type, socket_params);
	};

public:

	/**
	 *
	 */
	void
	clear_active_rofsockets_in4() {
		for (std::map<rofl::common::ctspaddress_in4, crofsock*>::iterator
				it = rofsockets_in4.begin(); it != rofsockets_in4.end(); ++it) {
			delete it->second;
		}
	};

	/**
	 *
	 */
	crofsock&
	set_active_rofsocket_in4(const rofl::common::ctspaddress_in4& addr) {
		if (rofsockets_in4.find(addr) == rofsockets_in4.end()) {
			throw eRofShimNotFound("crofshim::set_active_rofsocket_in4()");
		}
		return *(rofsockets_in4[addr]);
	};

	/**
	 *
	 */
	const crofsock&
	get_active_rofsocket_in4(const rofl::common::ctspaddress_in4& addr) const {
		if (rofsockets_in4.find(addr) == rofsockets_in4.end()) {
			throw eRofShimNotFound("crofshim::set_active_rofsocket_in4()");
		}
		return *(rofsockets_in4.at(addr));
	};

	/**
	 *
	 */
	void
	drop_active_rofsocket_in4(const rofl::common::ctspaddress_in4& addr) {
		if (rofsockets_in4.find(addr) == rofsockets_in4.end()) {
			return;
		}
		delete rofsockets_in4[addr];
		rofsockets_in4.erase(addr);
	};

	/**
	 *
	 */
	bool
	has_active_rofsocket_in4(const rofl::common::ctspaddress_in4& addr) const {
		return (not (rofsockets_in4.find(addr) == rofsockets_in4.end()));
	};

public:

	/**
	 *
	 */
	void
	clear_active_rofsockets_in6() {
		for (std::map<rofl::common::ctspaddress_in6, crofsock*>::iterator
				it = rofsockets_in6.begin(); it != rofsockets_in6.end(); ++it) {
			delete it->second;
		}
	};

	/**
	 *
	 */
	crofsock&
	set_active_rofsocket_in6(const rofl::common::ctspaddress_in6& addr) {
		if (rofsockets_in6.find(addr) == rofsockets_in6.end()) {
			throw eRofShimNotFound("crofshim::set_active_rofsocket_in6()");
		}
		return *(rofsockets_in6[addr]);
	};

	/**
	 *
	 */
	const crofsock&
	get_active_rofsocket_in6(const rofl::common::ctspaddress_in6& addr) const {
		if (rofsockets_in6.find(addr) == rofsockets_in6.end()) {
			throw eRofShimNotFound("crofshim::set_active_rofsocket_in6()");
		}
		return *(rofsockets_in6.at(addr));
	};

	/**
	 *
	 */
	void
	drop_active_rofsocket_in6(const rofl::common::ctspaddress_in6& addr) {
		if (rofsockets_in6.find(addr) == rofsockets_in6.end()) {
			return;
		}
		delete rofsockets_in6[addr];
		rofsockets_in6.erase(addr);
	};

	/**
	 *
	 */
	bool
	has_active_rofsocket_in6(const rofl::common::ctspaddress_in6& addr) const {
		return (not (rofsockets_in6.find(addr) == rofsockets_in6.end()));
	};

public:

	/**
	 *
	 */
	void
	clear_listening_sockets_in4() {
		for (std::map<rofl::common::ctspaddress_in4, csocket*>::iterator
				it = sockets_in4.begin(); it != sockets_in4.end(); ++it) {
			delete it->second;
		}
	};

	/**
	 *
	 */
	rofl::csocket&
	add_listening_socket_in4(
			enum rofl::csocket::socket_type_t socket_type,
			const rofl::cparams& socket_params) {

		rofl::common::ctspaddress_in4 addr(
				rofl::caddress_in4(socket_params.get_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string()),
				atoi(socket_params.get_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).get_string().c_str()));

		if (sockets_in4.find(addr) != sockets_in4.end()) {
			delete sockets_in4[addr];
		}
		(sockets_in4[addr] = csocket::csocket_factory(socket_type, this))->listen(socket_params);
		return *(sockets_in4[addr]);
	};

	/**
	 *
	 */
	rofl::csocket&
	set_listening_socket_in4(
			enum rofl::csocket::socket_type_t socket_type,
			const rofl::cparams& socket_params) {

		rofl::common::ctspaddress_in4 addr(
				rofl::caddress_in4(socket_params.get_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string()),
				atoi(socket_params.get_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).get_string().c_str()));

		if (sockets_in4.find(addr) == sockets_in4.end()) {
			(sockets_in4[addr] = csocket::csocket_factory(socket_type, this))->listen(socket_params);
		}
		return *(sockets_in4[addr]);
	};

	/**
	 *
	 */
	rofl::csocket&
	set_listening_socket_in4(
			const rofl::common::ctspaddress_in4& addr) {
		if (sockets_in4.find(addr) == sockets_in4.end()) {
			throw eRofShimNotFound("crofshim::set_listening_socket_in4()");
		}
		return *(sockets_in4[addr]);
	};

	/**
	 *
	 */
	const rofl::csocket&
	get_listening_socket_in4(
			const rofl::common::ctspaddress_in4& addr) const {
		if (sockets_in4.find(addr) == sockets_in4.end()) {
			throw eRofShimNotFound("crofshim::get_listening_socket_in4()");
		}
		return *(sockets_in4.at(addr));
	};

	/**
	 *
	 */
	void
	drop_listening_socket_in4(const rofl::common::ctspaddress_in4& addr) {
		if (sockets_in4.find(addr) == sockets_in4.end()) {
			return;
		}
		delete sockets_in4[addr];
		sockets_in4.erase(addr);
	};

	/**
	 *
	 */
	bool
	has_listening_socket_in4(const rofl::common::ctspaddress_in4& addr) const {
		return (not (sockets_in4.find(addr) == sockets_in4.end()));
	};

public:

	/**
	 *
	 */
	void
	clear_listening_sockets_in6() {
		for (std::map<rofl::common::ctspaddress_in6, csocket*>::iterator
				it = sockets_in6.begin(); it != sockets_in6.end(); ++it) {
			delete it->second;
		}
	};

	/**
	 *
	 */
	rofl::csocket&
	add_listening_socket_in6(
			enum rofl::csocket::socket_type_t socket_type,
			const rofl::cparams& socket_params) {

		rofl::common::ctspaddress_in6 addr(
				rofl::caddress_in6(socket_params.get_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).get_string()),
				atoi(socket_params.get_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).get_string().c_str()));

		if (sockets_in6.find(addr) != sockets_in6.end()) {
			delete sockets_in6[addr];
		}
		(sockets_in6[addr] = csocket::csocket_factory(socket_type, this))->listen(socket_params);
		return *(sockets_in6[addr]);
	};

	/**
	 *
	 */
	rofl::csocket&
	set_listening_socket_in6(
			const rofl::common::ctspaddress_in6& addr,
			enum rofl::csocket::socket_type_t socket_type,
			const rofl::cparams& socket_params) {
		if (sockets_in6.find(addr) == sockets_in6.end()) {
			(sockets_in6[addr] = csocket::csocket_factory(socket_type, this))->listen(socket_params);
		}
		return *(sockets_in6[addr]);
	};

	/**
	 *
	 */
	rofl::csocket&
	set_listening_socket_in6(
			const rofl::common::ctspaddress_in6& addr) {
		if (sockets_in6.find(addr) == sockets_in6.end()) {
			throw eRofShimNotFound("crofshim::set_listening_socket_in6()");
		}
		return *(sockets_in6[addr]);
	};

	/**
	 *
	 */
	const rofl::csocket&
	get_listening_socket_in6(
			const rofl::common::ctspaddress_in6& addr) const {
		if (sockets_in6.find(addr) == sockets_in6.end()) {
			throw eRofShimNotFound("crofshim::get_listening_socket_in6()");
		}
		return *(sockets_in6.at(addr));
	};

	/**
	 *
	 */
	void
	drop_listening_socket_in6(const rofl::common::ctspaddress_in6& addr) {
		if (sockets_in6.find(addr) == sockets_in6.end()) {
			return;
		}
		delete sockets_in6[addr];
		sockets_in6.erase(addr);
	};

	/**
	 *
	 */
	bool
	has_listening_socket_in6(const rofl::common::ctspaddress_in6& addr) const {
		return (not (sockets_in6.find(addr) == sockets_in6.end()));
	};

protected:

	/**
	 *
	 */
	virtual void
	handle_message(
			const rofl::common::ctspaddress_in4& addr, rofl::openflow::cofmsg *msg) {
		delete msg; /* to be overwritten */
	};

	/**
	 *
	 */
	virtual void
	handle_message(
			const rofl::common::ctspaddress_in6& addr, rofl::openflow::cofmsg *msg) {
		delete msg; /* to be overwritten */
	};

	/**
	 *
	 */
	virtual void
	send_message(
			crofsock *rofsock, rofl::openflow::cofmsg *msg) {
		try {
			rofsock->send_message(msg);
		} catch (rofl::eSocketTxAgainCongestion& e) {

		} catch (rofl::eSocketTxAgainPacketDropped& e) {

		} catch (rofl::eSocketTxAgainTxQueueFull& e) {

		} catch (rofl::eSocketTxAgain& e) {

		}
	};

private:

	friend class crofsock_env;

	/**
	 *
	 */
	virtual void
	handle_connect_refused(crofsock *rofsock) {
		delete rofsock;
	};

	/**
	 *
	 */
	virtual void
	handle_connect_failed(crofsock *rofsock) {
		delete rofsock;
	};

	/**
	 *
	 */
	virtual void
	handle_connected(crofsock *rofsock) {
		std::cerr << "RADDR:" << rofsock->get_socket().get_raddr();
		switch (rofsock->get_socket().get_domain()) {
		case AF_INET: {
			rofl::common::ctspaddress_in4 addr(
					rofl::caddress_in4(
							rofsock->get_socket().get_raddr().ca_s4addr, rofsock->get_socket().get_raddr().salen),
							be16toh(rofsock->get_socket().get_raddr().ca_s4addr->sin_port));
			if (rofsockets_in4.find(addr) != rofsockets_in4.end()) {
				delete rofsockets_in4[addr];
			}
			rofsockets_in4[addr] = rofsock;
		} break;
		case AF_INET6: {
			rofl::common::ctspaddress_in6 addr(
					rofl::caddress_in6(
							rofsock->get_socket().get_raddr().ca_s6addr, rofsock->get_socket().get_raddr().salen),
							be16toh(rofsock->get_socket().get_raddr().ca_s6addr->sin6_port));
			if (rofsockets_in6.find(addr) != rofsockets_in6.end()) {
				delete rofsockets_in6[addr];
			}
			rofsockets_in6[addr] = rofsock;
		} break;
		default: {
			delete rofsock;
		};
		}
	};

	virtual void
	handle_closed(crofsock *rofsock) {
		switch (rofsock->get_socket().get_domain()) {
		case AF_INET: {
			rofl::common::ctspaddress_in4 addr(
					rofl::caddress_in4(
							rofsock->get_socket().get_raddr().ca_s4addr, rofsock->get_socket().get_raddr().salen),
							be16toh(rofsock->get_socket().get_raddr().ca_s4addr->sin_port));
			rofsockets_in4.erase(addr);
		} break;
		case AF_INET6: {
			rofl::common::ctspaddress_in6 addr(
					rofl::caddress_in6(
							rofsock->get_socket().get_raddr().ca_s6addr, rofsock->get_socket().get_raddr().salen),
							be16toh(rofsock->get_socket().get_raddr().ca_s6addr->sin6_port));
			rofsockets_in6.erase(addr);
		} break;
		default: {
		};
		}
		delete rofsock;
	};

	virtual void
	handle_write(crofsock *rofsock) {
		/* TODO: handle situation after congestion */
	};

	virtual void
	recv_message(crofsock *rofsock, rofl::openflow::cofmsg *msg) {
		switch (rofsock->get_socket().get_domain()) {
		case AF_INET: {
			rofl::common::ctspaddress_in4 addr(
					rofl::caddress_in4(
							rofsock->get_socket().get_raddr().ca_s4addr, rofsock->get_socket().get_raddr().salen),
							be16toh(rofsock->get_socket().get_raddr().ca_s4addr->sin_port));
			handle_message(addr, msg);
		} break;
		case AF_INET6: {
			rofl::common::ctspaddress_in6 addr(
					rofl::caddress_in6(
							rofsock->get_socket().get_raddr().ca_s6addr, rofsock->get_socket().get_raddr().salen),
							be16toh(rofsock->get_socket().get_raddr().ca_s6addr->sin6_port));
			handle_message(addr, msg);
		} break;
		default: {
			delete msg;
		};
		}
	}

private:

	friend class csocket_env;

	virtual void
	handle_listen(csocket& socket, int newsd) {
		(new crofsock(this))->accept(socket.get_socket_type(), socket.get_socket_params(), newsd);
	};

	virtual void
	handle_accepted(csocket& socket) {};

	virtual void
	handle_accept_refused(csocket& socket) {};

	virtual void
	handle_connected(csocket& socket) {};

	virtual void
	handle_connect_refused(csocket& socket) {};

	virtual void
	handle_connect_failed(csocket& socket) {};

	virtual void
	handle_read(csocket& socket) {};

	virtual void
	handle_write(csocket& socket) {};

	virtual void
	handle_closed(csocket& socket) {};

private:

	std::map<rofl::common::ctspaddress_in4, csocket*>	sockets_in4;	// listening csocket instances
	std::map<rofl::common::ctspaddress_in6, csocket*>	sockets_in6;	// listening csocket instances
	std::map<rofl::common::ctspaddress_in4, crofsock*>	rofsockets_in4;
	std::map<rofl::common::ctspaddress_in6, crofsock*>	rofsockets_in6;
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif /* CROFSHIM_H_ */

