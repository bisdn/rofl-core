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

}



void
csocket_test::testSocketImpl()
{
	return;
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
}



void
csocket_test::testSocketOpenSSL()
{
	//return;
	try {
		socket_type = rofl::csocket::SOCKET_TYPE_OPENSSL;

		server = rofl::csocket::csocket_factory(socket_type, this);
		client = rofl::csocket::csocket_factory(socket_type, this);

		sparams = rofl::csocket::get_default_params(rofl::csocket::SOCKET_TYPE_OPENSSL);
		sparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("127.0.0.1");
		sparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string("443");
		sparams.set_param(rofl::csocket::PARAM_KEY_DOMAIN).set_string("inet");
		sparams.set_param(rofl::csocket::PARAM_KEY_TYPE).set_string("stream");
		sparams.set_param(rofl::csocket::PARAM_KEY_PROTOCOL).set_string("tcp");
		sparams.set_param(rofl::csocket::PARAM_SSL_KEY_CA_FILE).set_string("../../../../../tools/xca/ca.rofl-core.crt.pem");
		sparams.set_param(rofl::csocket::PARAM_SSL_KEY_CERT).set_string("../../../../../tools/xca/server.crt.pem");
		sparams.set_param(rofl::csocket::PARAM_SSL_KEY_PRIVATE_KEY).set_string("../../../../../tools/xca/server.key.pem");

		server->listen(sparams);

		cparams = rofl::csocket::get_default_params(rofl::csocket::SOCKET_TYPE_OPENSSL);
		cparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_HOSTNAME).set_string("127.0.0.1");
		cparams.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string("6666");
		cparams.set_param(rofl::csocket::PARAM_KEY_REMOTE_HOSTNAME).set_string("127.0.0.1");
		cparams.set_param(rofl::csocket::PARAM_KEY_REMOTE_PORT).set_string("443");
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

		if (send_counter > 3) {
			client->close();
			worker->close();

			std::cerr << "handle_closed stopping main loop" << std::endl;
			rofl::cioloop::stop();

			return;
		}

		send_counter++;

		send_timer_id = register_timer(TIMER_SEND_DATA, 0);

		rofl::cmemory *test = new rofl::cmemory(10);

		client->send(test);

	} break;
	}
}



void
csocket_test::handle_new_connection(
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
			fprintf(stderr, "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU [1]\n");
			throw rofl::eInval();
		};
		}

		fprintf(stderr, "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU [2]\n");
		worker->accept(server->get_socket_params(), newsd);
		fprintf(stderr, "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU [3]\n");

		try {
			std::cerr << "client:" << std::endl << dynamic_cast<rofl::csocket_impl&>(*client);
			std::cerr << "worker:" << std::endl << dynamic_cast<rofl::csocket_impl&>(*worker);
		} catch (std::bad_cast& e) {};


		try {
			std::cerr << "client:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*client);
			std::cerr << "worker:" << std::endl << dynamic_cast<rofl::csocket_openssl&>(*worker);
		} catch (std::bad_cast& e) {};


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
}



void
csocket_test::handle_connected(
		rofl::csocket& socket)
{
	std::cerr << "handle_connected" << std::endl;
}



void
csocket_test::handle_connect_refused(
		rofl::csocket& socket)
{

}



void
csocket_test::handle_read(
		rofl::csocket& socket)
{

}



void
csocket_test::handle_write(
		rofl::csocket& socket)
{

}



void
csocket_test::handle_closed(
		rofl::csocket& socket)
{

}



