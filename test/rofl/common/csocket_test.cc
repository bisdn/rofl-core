/*
 * csocket_impl_test.cc
 *
 *  Created on: 07.04.2014
 *      Author: andreas
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "csocket_test.h"


CPPUNIT_TEST_SUITE_REGISTRATION( csocket_test );

#if defined DEBUG
#undef DEBUG
#endif

void
csocket_test::setUp()
{
	rofl::logging::set_debug_level(7);
}



void
csocket_test::tearDown()
{
	rofl::cioloop::stop();
	rofl::cioloop::shutdown();
}



void
csocket_test::testSocketImpl()
{
	//return;
	try {
		cmem.resize(64);
		wmem.resize(64);
		value = 0;

		socket_type = rofl::csocket::SOCKET_TYPE_PLAIN;

		server = rofl::csocket::csocket_factory(socket_type, this);
		client = rofl::csocket::csocket_factory(socket_type, this);

		sparams = rofl::csocket::get_default_params(rofl::csocket::SOCKET_TYPE_PLAIN);
		sparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("127.0.0.1");
		sparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string("3333");
		sparams.set_param(rofl::csocket::PARAM_KEY_DOMAIN).set_string("inet");
		sparams.set_param(rofl::csocket::PARAM_KEY_TYPE).set_string("stream");
		sparams.set_param(rofl::csocket::PARAM_KEY_PROTOCOL).set_string("tcp");

		server->listen(sparams);

		cparams = rofl::csocket::get_default_params(rofl::csocket::SOCKET_TYPE_PLAIN);
		cparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("127.0.0.1");
		cparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string("4444");
		cparams.set_param(rofl::csocket::PARAM_KEY_REMOTE_HOSTNAME).set_string("127.0.0.1");
		cparams.set_param(rofl::csocket::PARAM_KEY_REMOTE_PORT).set_string("3333");
		cparams.set_param(rofl::csocket::PARAM_KEY_DOMAIN).set_string("inet");
		cparams.set_param(rofl::csocket::PARAM_KEY_TYPE).set_string("stream");
		cparams.set_param(rofl::csocket::PARAM_KEY_PROTOCOL).set_string("tcp");

		client->connect(cparams);

		send_counter = 0;


		std::cerr << "testSocketImpl: init" << std::endl;

		rofl::cioloop::run();

		std::cerr << "testSocketImpl: shutdown" << std::endl;

		delete client;
		delete server;
		delete worker;

		sleep (1);

	} catch (rofl::eOpenSSL& e) {
		std::cerr << e;
	} catch (rofl::eSocketBase& e) {
		std::cerr << e;
	} catch (rofl::eSysCall& e) {
		std::cerr << e;
	} catch (rofl::RoflException& e) {
		std::cerr << e;
	}
}



void
csocket_test::testSocketOpenSSL()
{
	return;
	try {
		cmem.resize(64);
		wmem.resize(64);
		value = 0;

		socket_type = rofl::csocket::SOCKET_TYPE_OPENSSL;

		server = rofl::csocket::csocket_factory(socket_type, this);
		client = rofl::csocket::csocket_factory(socket_type, this);

		sparams = rofl::csocket::get_default_params(rofl::csocket::SOCKET_TYPE_OPENSSL);
		sparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("127.0.0.1");
		sparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string("3334");
		sparams.set_param(rofl::csocket::PARAM_KEY_DOMAIN).set_string("inet");
		sparams.set_param(rofl::csocket::PARAM_KEY_TYPE).set_string("stream");
		sparams.set_param(rofl::csocket::PARAM_KEY_PROTOCOL).set_string("tcp");
		sparams.set_param(rofl::csocket::PARAM_SSL_KEY_CA_FILE).set_string("../../../../../tools/xca/ca.rofl-core.crt.pem");
		sparams.set_param(rofl::csocket::PARAM_SSL_KEY_CERT).set_string("../../../../../tools/xca/server.crt.pem");
		sparams.set_param(rofl::csocket::PARAM_SSL_KEY_PRIVATE_KEY).set_string("../../../../../tools/xca/server.key.pem");

		server->listen(sparams);

		cparams = rofl::csocket::get_default_params(rofl::csocket::SOCKET_TYPE_OPENSSL);
		cparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("127.0.0.1");
		cparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string("6667");
		cparams.set_param(rofl::csocket::PARAM_KEY_REMOTE_HOSTNAME).set_string("127.0.0.1");
		cparams.set_param(rofl::csocket::PARAM_KEY_REMOTE_PORT).set_string("3334");
		cparams.set_param(rofl::csocket::PARAM_KEY_DOMAIN).set_string("inet");
		cparams.set_param(rofl::csocket::PARAM_KEY_TYPE).set_string("stream");
		cparams.set_param(rofl::csocket::PARAM_KEY_PROTOCOL).set_string("tcp");
		cparams.set_param(rofl::csocket::PARAM_SSL_KEY_CA_FILE).set_string("../../../../../tools/xca/ca.rofl-core.crt.pem");
		cparams.set_param(rofl::csocket::PARAM_SSL_KEY_CERT).set_string("../../../../../tools/xca/client.crt.pem");
		cparams.set_param(rofl::csocket::PARAM_SSL_KEY_PRIVATE_KEY).set_string("../../../../../tools/xca/client.key.pem");

		client->connect(cparams);

		send_counter = 0;

		std::cerr << "testSocketOpenSSL: init" << std::endl;

		rofl::cioloop::run();

		std::cerr << "testSocketOpenSSL: shutdown" << std::endl;

		delete client;
		delete server;
		delete worker;

	} catch (rofl::eOpenSSL& e) {
		std::cerr << e;
	} catch (rofl::eSocketBase& e) {
		std::cerr << e;
	} catch (rofl::eSysCall& e) {
		std::cerr << e;
	} catch (rofl::RoflException& e) {
		std::cerr << e;
	}
}



void
csocket_test::handle_timeout(int opaque, void* data)
{
	std::cerr << "handle_timeout" << std::endl;

	switch (opaque) {
	case TIMER_SEND_DATA: {

		if (send_counter >= 16) {
			client->close();
			worker->close();

			std::cerr << "handle_closed stopping main loop" << std::endl;
			rofl::cioloop::stop();

			dump_sockets();

			return;
		}

		send_counter++;

		send_timer_id = register_timer(TIMER_SEND_DATA, 1);

		for (unsigned int i = 0; i < cmem.memlen(); i++) {
			cmem[i] = value;
		}
		rofl::cmemory *test = new rofl::cmemory(cmem);


		std::cerr << "sending data to worker: " << test->memlen() << std::endl << *test;

		client->send(test);

		;
		for (unsigned int i = 0; i < wmem.memlen(); i++) {
			wmem[i] = 4 * value;
		}
		rofl::cmemory *test2 = new rofl::cmemory(wmem);

		std::cerr << "sending data to client: " << test2->memlen() << std::endl << *test2;

		worker->send(test2);


		value++;


	} break;
	}
}



void
csocket_test::handle_listen(
		rofl::csocket& socket, int newsd)
{
	std::cerr << "HANDLE_NEW_CONNECTION " << std::endl;

	if (server == &socket) {
		std::cerr << "handle_accepted server start" << std::endl;

		switch (socket_type) {
		case rofl::csocket::SOCKET_TYPE_PLAIN:
		case rofl::csocket::SOCKET_TYPE_OPENSSL: {
			worker = rofl::csocket::csocket_factory(socket_type, this);
		} break;
		default: {
			throw rofl::eInval();
		};
		}

		worker->accept(server->get_socket_params(), newsd);

		dump_sockets();


		std::cerr << "handle_accepted server stop" << std::endl;

		send_timer_id = register_timer(TIMER_SEND_DATA, 1);

	} else
	if (client == &socket) {
		std::cerr << "handle_accepted client start" << std::endl;

		std::cerr << "handle_accepted client stop" << std::endl;

	} else
	if (worker == &socket) {
		std::cerr << "handle_accepted worker start" << std::endl;

		std::cerr << "handle_accepted worker stop" << std::endl;

	}
}



void
csocket_test::handle_accepted(
		rofl::csocket& socket)
{
	std::cerr << "handle_accepted" << std::endl;

	try {
		std::cerr << "worker:" << std::endl << dynamic_cast<rofl::csocket_impl&>(*worker);
	} catch (std::bad_cast& e) {};


	try {
		std::cerr << "worker:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*worker);
	} catch (std::bad_cast& e) {};

}



void
csocket_test::handle_accept_refused(
		rofl::csocket& socket)
{
	std::cerr << "handle_accept_refused" << std::endl;
	CPPUNIT_ASSERT(false);
}



void
csocket_test::handle_connected(
		rofl::csocket& socket)
{
	std::cerr << "handle_connected" << std::endl;

	try {
		std::cerr << "client:" << std::endl << dynamic_cast<rofl::csocket_impl&>(*client);
	} catch (std::bad_cast& e) {};


	try {
		std::cerr << "client:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*client);
	} catch (std::bad_cast& e) {};
}



void
csocket_test::handle_connect_refused(
		rofl::csocket& socket)
{
	std::cerr << "handle_connect_refused" << std::endl;
	CPPUNIT_ASSERT(false);
}



void
csocket_test::handle_read(
		rofl::csocket& socket)
{
	if (&socket == client) {
		std::cerr << "handle_read (client)" << std::endl;

		rofl::cmemory mem(64);

		int rc = socket.recv(mem.somem(), mem.memlen());

		std::cerr << "receiving data from client socket: " << rc << std::endl << mem;

		CPPUNIT_ASSERT(mem == wmem);

	} else
	if (&socket == worker) {
		std::cerr << "handle_read (worker)" << std::endl;

		rofl::cmemory mem(64);

		int rc = socket.recv(mem.somem(), mem.memlen());

		std::cerr << "receiving data from worker socket: " << rc << std::endl << mem;

		CPPUNIT_ASSERT(mem == cmem);
	}
}



void
csocket_test::handle_write(
		rofl::csocket& socket)
{
	std::cerr << "handle_write" << std::endl;
}



void
csocket_test::handle_closed(
		rofl::csocket& socket)
{
	std::cerr << "handle_closed" << std::endl << socket;
}



void
csocket_test::dump_sockets()
{
	try {
		std::cerr << "server:" << std::endl << dynamic_cast<rofl::csocket_impl&>(*server);
		std::cerr << "client:" << std::endl << dynamic_cast<rofl::csocket_impl&>(*client);
		std::cerr << "worker:" << std::endl << dynamic_cast<rofl::csocket_impl&>(*worker);
	} catch (std::bad_cast& e) {};


	try {
		std::cerr << "server:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*server);
		std::cerr << "client:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*client);
		std::cerr << "worker:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*worker);
	} catch (std::bad_cast& e) {};
}

