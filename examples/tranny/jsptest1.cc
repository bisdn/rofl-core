#include <rofl/platform/unix/cunixenv.h>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/openflow/cofdpt.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/openflow10.h>
#include <rofl/common/openflow/cofaction.h>
#include <rofl/common/cerror.h>
#include <rofl/common/openflow/openflow_rofl_exceptions.h>
#include <rofl/common/utils/c_logger.h>
#include <unistd.h>

// Simple test program to check the OFP10_VERSION of rofl::cflowentry and related functions work

// g++ jsptest1.cc  -lrofl -lpthread -lrt -I.

class jsptest1 : public rofl::crofbase {

protected:

public:
jsptest1():crofbase(1 << OFP10_VERSION) { }

void handle_dpath_open(rofl::cofdpt *dpt) {
	std::cout << __FUNCTION__ << "handle_dpath_open called with dpt (" << dpt->c_str() << ")" << std::endl;
}

void handle_packet_in( rofl::cofdpt *dpt, rofl::cofmsg_packet_in *msg ) {
	std::cout << std::endl << __FUNCTION__ << ": From  " << dpt->c_str() << ": " << msg->c_str() << "." << std::endl;
	std::cout << "packet came in on port " << (unsigned) msg->get_in_port() << std::endl;
	std::cout << "dpt->get_version() =   " << (unsigned) dpt->get_version() << std::endl;
	uint32_t inport, outport;
//	inport = msg->get_in_port();
//	outport = (inport==2)?1:2;
	inport = 69;
	outport = 42;
	std::cout << __FUNCTION__ << ": About to connect port " << inport << " to " << outport << "." << std::endl;
	rofl::cflowentry fe(dpt->get_version());
	std::cout << "TP" << __LINE__ << std::endl;
	fe.set_command(OFPFC_ADD);
	std::cout << "TP" << __LINE__ << std::endl;
	fe.match.clear();
	std::cout << "TP" << __LINE__ << std::endl;
	fe.match.set_in_port(inport);
	std::cout << "TP" << __LINE__ << std::endl;
	fe.actions.next() = rofl::cofaction_output(dpt->get_version(), outport);
	std::cout << "fe.actions length is " << fe.actions.length() << "." << std::endl;
	std::cout << "There are now " << fe.actions.size() << " actions." << std::endl;
	std::cout << "TP" << __LINE__ << std::endl;
//	fe.instructions.next() = rofl::cofinst_apply_actions(dpt->get_version());
//	fe.set_out_port(outport);
	std::cout << "TP" << __LINE__ << std::endl;
//	fe.instructions.back().actions.next() = cofaction_output(m_dpt->get_version(), OFPP12_CONTROLLER);
	std::cout << "TP" << __LINE__ << std::endl;
	std::cout << "About to send cflowentry: " << fe.c_str() << "." << std::endl;
	std::cout << "TP" << __LINE__ << std::endl;
//	fe.pack();
	fe.set_cookie(0x1234567890ABCDEFULL);
	std::cout << "TP" << __LINE__ << std::endl;
	std::cout << "inport is " << (unsigned) fe.match.get_in_port() << std::endl;
	std::cout << "match is " << fe.match << std::endl;
	std::cout << "match is " << fe.match.c_str() << std::endl;
	send_flow_mod_message(dpt, fe);
	std::cout << "send_flow_mod_message() called successfully." << std::endl;

	delete (msg);
}

void handle_error ( rofl::cofdpt * dpt, rofl::cofmsg_error * msg ) {	// Annoying, this is ignored I think. Needs to be added to cofdptimpl.cc
	std::cout << __FUNCTION__ << ": From  " << dpt->c_str() << ": " << msg->c_str() << "." << std::endl;
	delete msg;
}

};

int main(int argc, char** argv) {
	/* update defaults */
	rofl::csyslog::initlog( rofl::csyslog::LOGTYPE_STDERR, rofl::csyslog::DBG, std::string("/home/alien/jsptest1.log"), "jsptest1: ");

	rofl::csyslog::set_all_debug_levels(rofl::csyslog::DBG);
	rofl::csyslog::set_debug_level("ciosrv", "emergency");
	rofl::csyslog::set_debug_level("cthread", "emergency");



	rofl::ciosrv::init();

	jsptest1 t;

	t.rpc_listen_for_dpts(rofl::caddress(AF_INET, "0.0.0.0", 6633));

	rofl::ciosrv::run();

	rofl::ciosrv::destroy();
	
	ROFL_INFO("House cleaned!\nGoodbye\n");
	
	exit(EXIT_SUCCESS);
}

