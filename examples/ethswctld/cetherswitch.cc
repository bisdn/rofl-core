#include "cetherswitch.h"

using namespace rofl::examples::ethswctld;

#define ETHSWCTLD_LOG_FILE "/var/log/ethswctld.log"
#define ETHSWCTLD_PID_FILE "/var/run/ethswctld.pid"

bool cetherswitch::keep_on_running = true;


void
signal_handler(int signal) {
	switch (signal) {
	case SIGINT: {
		rofl::cioloop::get_loop().shutdown();
		cetherswitch::stop();
	} break;
	}
}


int
cetherswitch::run(
		int argc, char** argv)
{
	//Capture control+C
	signal(SIGINT, signal_handler);


	/*
	* Parse parameters
	*/
	rofl::cunixenv env_parser(argc, argv);

	/* update defaults */
	env_parser.add_option(rofl::coption(true,REQUIRED_ARGUMENT,'l',"logfile","Log file used when daemonization", ETHSWCTLD_LOG_FILE));
	env_parser.add_option(rofl::coption(true, REQUIRED_ARGUMENT, 'p', "pidfile", "set pid-file", std::string(ETHSWCTLD_PID_FILE)));
#ifdef ROFL_HAVE_OPENSSL
	env_parser.add_option(rofl::coption(true, REQUIRED_ARGUMENT, 't', "cert-and-key-file", "Certificate and key to encrypt control traffic (PEM format)", std::string("")));
#endif
	//Parse
	env_parser.parse_args();

	if (not env_parser.is_arg_set("daemonize")) {
		// only do this in non
		std::string ident(env_parser.get_arg("logfile"));

		rofl::logging::init();
		rofl::logging::set_debug_level(atoi(env_parser.get_arg("debug").c_str()));
	} else {

		rofl::cdaemon::daemonize(env_parser.get_arg("pidfile"), env_parser.get_arg("logfile"));
		rofl::logging::set_debug_level(atoi(env_parser.get_arg("debug").c_str()));
		rofl::logging::notice << "[ethswctld][main] daemonizing successful" << std::endl;
	}

	//Initialize the instance of etherswitch (crofbase based)
	rofl::openflow::cofhello_elem_versionbitmap versionbitmap;
	versionbitmap.add_ofp_version(rofl::openflow10::OFP_VERSION);
	versionbitmap.add_ofp_version(rofl::openflow12::OFP_VERSION);
	versionbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);
	cetherswitch sw(versionbitmap);

	//We must now specify the parameters for allowing datapaths to connect
	rofl::cparams socket_params = rofl::csocket::get_default_params(rofl::csocket::SOCKET_TYPE_PLAIN);
	socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string() = std::string("6653");
	sw.add_dpt_listening(0, rofl::csocket::SOCKET_TYPE_PLAIN, socket_params);

	while (keep_on_running) {
		try {
			//Launch main I/O loop
			rofl::cioloop::get_loop().run();
		} catch (...) {
			std::cerr << "********************************************* UUUUUU" << std::endl;
			throw;
		}
	}

	//Clean-up before exit
	rofl::cioloop::cleanup_on_exit();

	return EXIT_SUCCESS;
}



cetherswitch::cetherswitch(
		const rofl::openflow::cofhello_elem_versionbitmap& versionbitmap) :
		crofbase(versionbitmap),
		dump_fib_interval(DUMP_FIB_DEFAULT_INTERVAL),
		get_flow_stats_interval(GET_FLOW_STATS_DEFAULT_INTERVAL)
{
}



cetherswitch::~cetherswitch()
{
	//Stop listening sockets for datapath elements
	rofl::crofbase::close_dpt_listening();
}



void
cetherswitch::handle_timeout(int opaque, void* data)
{
	try {
		switch (opaque) {
		case TIMER_DUMP_FIB: {

			//re-register timer for next round
			timer_id_dump_fib =
					rofl::ciosrv::register_timer(TIMER_DUMP_FIB,
							rofl::ctimespec(dump_fib_interval));

			rofl::logging::notice << "****************************************" << std::endl;
			rofl::logging::notice << *this;
			rofl::logging::notice << "****************************************" << std::endl;

		} break;
		case TIMER_GET_FLOW_STATS: {

			//re-register timer for next round
			timer_id_get_flow_stats =
					rofl::ciosrv::register_timer(TIMER_GET_FLOW_STATS,
							rofl::ctimespec(get_flow_stats_interval));

			rofl::crofdpt& dpt = rofl::crofdpt::get_dpt(dptid);

			// you have to define the version for each OpenFlow element
			rofl::openflow::cofflow_stats_request req(dpt.get_version_negotiated());

			// since OpenFlow v1.2 => multi-table pipelines
			if (rofl::openflow10::OFP_VERSION < dpt.get_version_negotiated()) {
				req.set_table_id(rofl::openflow12::OFPTT_ALL);
			}

			// no matches specified => all wildcard

			dpt.send_flow_stats_request(rofl::cauxid(0), 0, req);
		} break;
		default: {
			// unhandled timer event
		};
		}

	} catch (rofl::eRofDptNotFound& e) {
		// datapath with internal identifier dptid not found
	} catch (rofl::eRofBaseNotConnected& e) {

	}
}



/*
 * Methods inherited from crofbase
 */
void
cetherswitch::handle_dpt_open(
		rofl::crofdpt& dpt)
{
	// register timer for dumping ethswitch's internal state
	timer_id_dump_fib =
			register_timer(TIMER_DUMP_FIB,
					rofl::ctimespec(dump_fib_interval));

	// start periodic timer for querying datapath for all flow table entries
	timer_id_get_flow_stats =
			rofl::ciosrv::register_timer(TIMER_GET_FLOW_STATS,
					rofl::ctimespec(get_flow_stats_interval));

	dptid = dpt.get_dptid();

	//New connection => cleanup the RIB by re-creating the FIB table
	cfibtable::add_fib(dpt.get_dptid());

	rofl::logging::info << "[cetherswitch] datapath attached, dptid: "
			<< dpt.get_dptid().str() << std::endl
			<< cfibtable::get_fib(dpt.get_dptid());

	//Remove all flows in the table
	dpt.flow_mod_reset();
	
	//Remove all groupmods
	if(dpt.get_version_negotiated() > rofl::openflow10::OFP_VERSION) {
		dpt.group_mod_reset();
	}

	// redirect all traffic not matching any FIB entry to the control plane
	rofl::openflow::cofflowmod flow_table_entry(dpt.get_version_negotiated());
	flow_table_entry.set_command(rofl::openflow::OFPFC_ADD);

	//Now add action
	//OF1.0 has no instructions, so the code here differs
	switch (dpt.get_version_negotiated()) {
	case rofl::openflow10::OFP_VERSION: {
		flow_table_entry.set_actions().
				add_action_output(rofl::cindex(0)).
						set_port_no(rofl::openflow::OFPP_CONTROLLER);
	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		flow_table_entry.set_instructions().set_inst_apply_actions().set_actions().
				add_action_output(rofl::cindex(0)).
						set_port_no(rofl::openflow::OFPP_CONTROLLER);
	} break;
	default: {
		throw rofl::eBadVersion();
	};
	}

	dpt.send_flow_mod_message(rofl::cauxid(0), flow_table_entry);
}



void
cetherswitch::handle_dpt_close(
		const rofl::cdptid& dptid)
{
	rofl::ciosrv::cancel_timer(timer_id_dump_fib);

	rofl::ciosrv::cancel_timer(timer_id_get_flow_stats);

	rofl::logging::info << "[cetherswitch] datapath detached, dptid: "
			<< dptid.str() << std::endl
			<< cfibtable::get_fib(dptid);

	cfibtable::drop_fib(dptid);
}



void
cetherswitch::handle_packet_in(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_packet_in& msg)
{
	try {
		cfibtable& fib = cfibtable::set_fib(dpt.get_dptid());
		cflowtable& ftb = cflowtable::set_flowtable(dpt.get_dptid());
		rofl::caddress_ll eth_src;
		rofl::caddress_ll eth_dst;
		uint32_t in_port = 0;

		switch (dpt.get_version_negotiated()) {
		case rofl::openflow10::OFP_VERSION: {
			struct eth_hdr_t {
				uint8_t eth_dst[6];
				uint8_t eth_src[6];
				uint16_t eth_type;
			};
			if (msg.get_packet().length() < sizeof(struct eth_hdr_t)) {
				return;
			}
			struct eth_hdr_t* eth_hdr = (struct eth_hdr_t*)msg.get_packet().soframe();
			eth_dst.unpack(eth_hdr->eth_dst, 6);
			eth_src.unpack(eth_hdr->eth_src, 6);
			in_port = msg.get_in_port();
		} break;
		default: {
			eth_src = msg.get_match().get_eth_src();
			eth_dst = msg.set_match().get_eth_dst();
			in_port = msg.get_match().get_in_port();
		};
		}

		//Ignore multi-cast frames (SRC)
		if (eth_src.is_multicast()) {
			rofl::logging::warn << "[cetherswitch][packet-in] eth-src:" << eth_src << " is multicast, ignoring." << std::endl;
			return;
		}

		//SRC is a unicast address => Update RIB: learn the source address and its associated portno
		fib.set_fib_entry(eth_src, in_port).set_port_no(in_port);

		//Drop frames destined to 01:80:c2:00:00:00
		if (eth_dst == rofl::caddress_ll("01:80:c2:00:00:00")) {
			dpt.drop_buffer(auxid, msg.get_buffer_id());
			return;
		}

		//Dump the pkt info
		dump_packet_in(dpt, msg);

		//Flood multicast and yet unknown frames (DST)
		if (eth_dst.is_multicast() || (not fib.has_fib_entry(eth_dst))) {
			rofl::openflow::cofactions actions(dpt.get_version_negotiated());
			actions.add_action_output(rofl::cindex(0)).set_port_no(rofl::crofbase::get_ofp_flood_port(dpt.get_version_negotiated()));
			dpt.send_packet_out_message(auxid, msg.get_buffer_id(), in_port, actions);
			return;
		}

		//SRC and DST are unicast addresses => Create flow entry on data path
		if (fib.has_fib_entry(eth_dst)) {
			ftb.set_flow_entry(eth_src, eth_dst, fib.get_fib_entry(eth_dst).get_port_no());

			if (rofl::openflow::OFP_NO_BUFFER != msg.get_buffer_id()) {
				rofl::openflow::cofactions actions(dpt.get_version_negotiated());
				actions.add_action_output(rofl::cindex(0)).set_port_no(fib.get_fib_entry(eth_dst).get_port_no());
				dpt.send_packet_out_message(auxid, msg.get_buffer_id(), in_port, actions);
			}
		}

	} catch (exceptions::eFibInval& e) {
		rofl::logging::error << "[cetherswitch] hwaddr validation failed" << std::endl << msg;

	} catch (...) {
		rofl::logging::error << "[cetherswitch] caught some exception, use debugger for getting more info" << std::endl << msg;
		assert(0);
		throw;
	}
}



void
cetherswitch::handle_flow_stats_reply(
		rofl::crofdpt& dpt,
		const rofl::cauxid& auxid,
		rofl::openflow::cofmsg_flow_stats_reply& msg)
{
	rofl::logging::info << "Flow-Stats-Reply rcvd:" << std::endl << msg;
}



void
cetherswitch::handle_flow_stats_reply_timeout(
		rofl::crofdpt& dpt,
		uint32_t xid)
{
	rofl::logging::info << "Flow-Stats-Reply timeout" << std::endl;
}
/*
 * End of methods inherited from crofbase
 */




void
cetherswitch::dump_packet_in(
		rofl::crofdpt& dpt,
		rofl::openflow::cofmsg_packet_in& msg)
{
	struct eth_hdr_t {
		uint8_t eth_dst[6];
		uint8_t eth_src[6];
		uint16_t eth_type;
	};

	if (msg.get_packet().length() < sizeof(struct eth_hdr_t)) {
		return;
	}

	struct eth_hdr_t* eth_hdr = (struct eth_hdr_t*)msg.get_packet().soframe();

	rofl::caddress_ll eth_dst(eth_hdr->eth_dst, 6);
	rofl::caddress_ll eth_src(eth_hdr->eth_src, 6);

	//Dump some information
	rofl::logging::info << "[cetherswitch] PACKET-IN => frame seen, "
						<< "buffer-id:0x" << std::hex << msg.get_buffer_id() << std::dec << " "
						<< "eth-src:" << eth_src << " "
						<< "eth-dst:" << eth_dst << " "
						<< "eth-type:0x" << std::hex << (int)be16toh(eth_hdr->eth_type) << std::dec << " "
						<< std::endl;
	rofl::logging::info << dpt.get_dpid();
}




