/*
 * csocket_plain_test.cc
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
//#undef DEBUG
#endif

void
csocket_test::setUp()
{
#ifdef DEBUG
	rofl::logging::set_debug_level(7);
#endif
}



void
csocket_test::tearDown()
{
	rofl::cioloop::get_loop().stop();
	rofl::cioloop::get_loop().shutdown();
}



void
csocket_test::testSocketImpl()
{
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

#ifdef DEBUG
		std::cerr << "testSocketImpl: init" << std::endl;
#endif
		rofl::cioloop::get_loop().run();
#ifdef DEBUG
		std::cerr << "testSocketImpl: shutdown" << std::endl;
#endif
		delete client;
		delete server;
		delete worker;

		sleep (1);
#ifdef ROFL_HAVE_OPENSSL
	} catch (rofl::eOpenSSL& e) {
		std::cerr << e;
#endif
	} catch (rofl::eSocketBase& e) {
		std::cerr << e;
	} catch (rofl::eSysCall& e) {
		std::cerr << e;
	} catch (rofl::RoflException& e) {
		std::cerr << e;
	}
}


#ifdef ROFL_HAVE_OPENSSL 

void
csocket_test::testSocketOpenSSL()
{
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
#ifdef DEBUG
		std::cerr << "testSocketOpenSSL: init" << std::endl;
#endif
		rofl::cioloop::run();
#ifdef DEBUG
		std::cerr << "testSocketOpenSSL: shutdown" << std::endl;
#endif
		delete client;
		delete server;
		delete worker;

#ifdef ROFL_HAVE_OPENSSL
	} catch (rofl::eOpenSSL& e) {
		std::cerr << e;
#endif
	} catch (rofl::eSocketBase& e) {
		std::cerr << e;
	} catch (rofl::eSysCall& e) {
		std::cerr << e;
	} catch (rofl::RoflException& e) {
		std::cerr << e;
	}
}
#endif


void
csocket_test::handle_timeout(int opaque, void* data)
{
#ifdef DEBUG
	std::cerr << "handle_timeout" << std::endl;
#endif
	switch (opaque) {
	case TIMER_SEND_DATA: {

		if (send_counter >= 4) {
			client->close();
			worker->close();

#ifdef DEBUG
			std::cerr << "handle_closed stopping main loop" << std::endl;
#endif
			rofl::cioloop::get_loop().stop();

			dump_sockets();

			return;
		}

		send_counter++;

		send_timer_id = register_timer(TIMER_SEND_DATA, 1);

		for (unsigned int i = 0; i < cmem.memlen(); i++) {
			cmem[i] = value;
		}
		rofl::cmemory *test = new rofl::cmemory(cmem);

#ifdef DEBUG
		std::cerr << "sending data to worker: " << test->memlen() << std::endl << *test;
#endif
		client->send(test);

		;
		for (unsigned int i = 0; i < wmem.memlen(); i++) {
			wmem[i] = 4 * value;
		}
		rofl::cmemory *test2 = new rofl::cmemory(wmem);
#ifdef DEBUG
		std::cerr << "sending data to client: " << test2->memlen() << std::endl << *test2;
#endif
		worker->send(test2);


		value++;


	} break;
	}
}



void
csocket_test::handle_listen(
		rofl::csocket& socket, int newsd)
{
#ifdef DEBUG
	std::cerr << "HANDLE_NEW_CONNECTION " << std::endl;
#endif
	if (server == &socket) {
#ifdef DEBUG
		std::cerr << "handle_accepted server start" << std::endl;
#endif

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

#ifdef DEBUG
		std::cerr << "handle_accepted server stop" << std::endl;
#endif
		send_timer_id = register_timer(TIMER_SEND_DATA, 1);

	} else
	if (client == &socket) {
#ifdef DEBUG
		std::cerr << "handle_accepted client start" << std::endl;

		std::cerr << "handle_accepted client stop" << std::endl;
#endif
	} else
	if (worker == &socket) {
#ifdef DEBUG
		std::cerr << "handle_accepted worker start" << std::endl;

		std::cerr << "handle_accepted worker stop" << std::endl;
#endif
	}
}



void
csocket_test::handle_accepted(
		rofl::csocket& socket)
{
#ifdef DEBUG
	std::cerr << "handle_accepted" << std::endl;

	try {
		std::cerr << "worker:" << std::endl << dynamic_cast<rofl::csocket_plain&>(*worker);
	} catch (std::bad_cast& e) {};

#ifdef ROFL_HAVE_OPENSSL 
	try {
		std::cerr << "worker:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*worker);
	} catch (std::bad_cast& e) {};
#endif
#endif
}



void
csocket_test::handle_accept_refused(
		rofl::csocket& socket)
{
#ifdef DEBUG
	std::cerr << "handle_accept_refused" << std::endl;
#endif
	CPPUNIT_ASSERT(false);
}



void
csocket_test::handle_connected(
		rofl::csocket& socket)
{
#ifdef DEBUG
	std::cerr << "handle_connected" << std::endl;

	try {
		std::cerr << "client:" << std::endl << dynamic_cast<rofl::csocket_plain&>(*client);
	} catch (std::bad_cast& e) {};


#ifdef ROFL_HAVE_OPENSSL 
	try {
		std::cerr << "client:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*client);
	} catch (std::bad_cast& e) {};
#endif
#endif
}



void
csocket_test::handle_connect_refused(
		rofl::csocket& socket)
{
#ifdef DEBUG
	std::cerr << "handle_connect_refused" << std::endl;
#endif
	CPPUNIT_ASSERT(false);
}



void
csocket_test::handle_connect_failed(
		rofl::csocket& socket)
{
#ifdef DEBUG
	std::cerr << "handle_connect_failed" << std::endl;
#endif
	CPPUNIT_ASSERT(false);
}



void
csocket_test::handle_read(
		rofl::csocket& socket)
{
	if (&socket == client) {
#ifdef DEBUG
		std::cerr << "handle_read (client)" << std::endl;
#endif
		rofl::cmemory mem(64);

		int rc = socket.recv(mem.somem(), mem.memlen());
		(void)rc;
#ifdef DEBUG
		std::cerr << "receiving data from client socket: " << rc << std::endl << mem;
#endif
		CPPUNIT_ASSERT(mem == wmem);

	} else
	if (&socket == worker) {
#ifdef DEBUG
		std::cerr << "handle_read (worker)" << std::endl;
#endif
		rofl::cmemory mem(64);

		int rc = socket.recv(mem.somem(), mem.memlen());
		(void)rc;
#ifdef DEBUG
		std::cerr << "receiving data from worker socket: " << rc << std::endl << mem;
#endif
		CPPUNIT_ASSERT(mem == cmem);
	}
}



void
csocket_test::handle_write(
		rofl::csocket& socket)
{
#ifdef DEBUG
	std::cerr << "handle_write" << std::endl;
#endif
}



void
csocket_test::handle_closed(
		rofl::csocket& socket)
{
#ifdef DEBUG
	std::cerr << "handle_closed" << std::endl << socket;
#endif
}



void
csocket_test::dump_sockets()
{
#ifdef DEBUG
	try {
		std::cerr << "server:" << std::endl << dynamic_cast<rofl::csocket_plain&>(*server);
		std::cerr << "client:" << std::endl << dynamic_cast<rofl::csocket_plain&>(*client);
		std::cerr << "worker:" << std::endl << dynamic_cast<rofl::csocket_plain&>(*worker);
	} catch (std::bad_cast& e) {};


#ifdef ROFL_HAVE_OPENSSL 
	try {
		std::cerr << "server:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*server);
		std::cerr << "client:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*client);
		std::cerr << "worker:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*worker);
	} catch (std::bad_cast& e) {};
#endif
#endif
}

