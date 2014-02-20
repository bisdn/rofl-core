
#include <memory>

#include <rofl/platform/unix/cunixenv.h>
// #include <rofl/common/cmacaddr.h>
#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/openflow/cofdpt.h>
#include <rofl/common/openflow/cofctl.h>

class ctranslator : public rofl::crofbase {
	public:
	ctranslator();
	virtual ~ctranslator();
	
	protected:
	virtual void handle_dpath_open (rofl::cofdpt *); // from crofbase
	virtual void handle_dpath_close (rofl::cofdpt *); // from crofbase
	virtual void handle_flow_mod(rofl::cofctl *ctl, rofl::cofmsg_flow_mod *msg);  // from crofbase
	void connect_to_slave();	// connects to the slave data path device - must be called before packets from controller are handled
	rofl::cofdpt * m_slave;
};

ctranslator::ctranslator():m_slave(0) {
	// read config file
	//..
	
	// connect to slave device
	connect_to_slave();
}

ctranslator::~ctranslator() {
	// rpc_close_all();
	std::cerr << "ctranslator::~ctranslator() called." << std::endl;	// TODO: proper logging
}

void ctranslator::connect_to_slave() {
	// connect to slave
	rpc_connect_to_dpt(OFP12_VERSION,1,rofl::caddress(AF_INET, "127.0.0.1", 6633));	// TODO slave IP/PORT should be from config
}

void ctranslator::handle_dpath_open (rofl::cofdpt *dpt) {
	// should be called automatically after call to rpc_connect_to_dpt in connect_to_slave
	m_slave = dpt;
}

void ctranslator::handle_dpath_close (rofl::cofdpt *dpt) {
	assert(dpt==m_slave);
	// this socket disconnecting could just be a temporary thing - mark it is dead, but expect a possible auto reconnect
	m_slave=0;
}

void ctranslator::handle_flow_mod(rofl::cofctl *ctl, rofl::cofmsg_flow_mod *msg) {
	/// if(!m_slave) {PANIC MR MAINWARING!}	// could be a permanent lack of m_slave, or just a temporary disconnect and we're waiting for a reconnect
	std::cout << "handle_flow_mod from " << ctl->c_str() << " : " << msg->c_str() << std::endl;
	rofl::cflowentry entry(OFP12_VERSION);
	entry.set_command(msg->get_command());
	entry.set_table_id(msg->get_table_id());
	entry.set_idle_timeout(msg->get_idle_timeout());
	entry.set_hard_timeout(msg->get_hard_timeout());
	entry.set_cookie(msg->get_cookie());
	entry.set_cookie_mask(msg->get_cookie_mask());
	entry.set_priority(msg->get_priority());
	entry.set_buffer_id(msg->get_buffer_id());
	entry.set_out_port(msg->get_out_port());
	entry.set_out_group(msg->get_out_group());
	entry.set_flags(msg->get_flags());
	send_flow_mod_message( m_slave, entry );
}

int main(int, char**) {
        /* update defaults */
        rofl::csyslog::initlog( rofl::csyslog::LOGTYPE_STDERR, rofl::csyslog::EMERGENCY, std::string("tranny.log"), "tranny: ");

        rofl::csyslog::set_debug_level("ciosrv", "emergency");
        rofl::csyslog::set_debug_level("cthread", "emergency");

        rofl::ciosrv::init();

		ctranslator tranny;

		tranny.rpc_listen_for_ctls(rofl::caddress(AF_INET, "0.0.0.0", 6633));
//        tranny.rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6633));
//       tranny.rpc_listen_for_dpts(caddress(AF_INET, "0.0.0.0", 6632));

        rofl::ciosrv::run();

        return 0;
}
