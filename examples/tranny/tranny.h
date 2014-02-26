
#ifndef UCL_EE_TRANNY_H
#define UCL_EE_TRANNY_H

#include <rofl/common/caddress.h>
#include <rofl/common/crofbase.h>
#include <rofl/common/openflow/cofdpt.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/openflow10.h>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

// a macro to simply copying fields from messages to responses
// e.g. BOOST_PP_SEQ_FOR_EACH( CLONECMD, ((*msg))(entry), (command)(table_id)(idle_timeout)(hard_timeout) (cookie)(cookie_mask)(priority)(buffer_id)(out_port)(out_group)(flags) )
// generates entry.set_command ( (*msg).get_command () ); entry.set_table_id ( (*msg).get_table_id () ); etc..
// if you don;t want to install boost.PP then just - TODO remove CLONECMD just before releasing ver 1.0?
// JSPNOTE: this macro was dropped because identical copies of messages being send to the other proxy end tend to break things a lot
#undef CLONECMD
#undef CLONECMDNOSEQ
#define CLONECMDNOSEQ(CLONEFROM,CLONETO,CLONEWHAT) CLONETO.set_##CLONEWHAT ( CLONEFROM.get_##CLONEWHAT () );
#define CLONECMD(r, CLONESEQ, CLONEWHAT) CLONECMDNOSEQ( BOOST_PP_SEQ_ELEM(0,CLONESEQ) ,  BOOST_PP_SEQ_ELEM(1,CLONESEQ) , CLONEWHAT )

class ctranslator : public rofl::crofbase {
	public:
//	ctranslator(rofl::caddress,uint8_t,??,rofl::caddess);
	ctranslator();
	virtual ~ctranslator();
	
	protected:
	virtual void handle_dpath_open (rofl::cofdpt *); // from crofbase
	virtual void handle_dpath_close (rofl::cofdpt *); // from crofbase
	virtual void handle_ctrl_open (rofl::cofctl *); // from crofbase
	virtual void handle_ctrl_close (rofl::cofctl *); // from crofbase
	virtual void handle_features_request(rofl::cofctl *ctl, rofl::cofmsg_features_request *pack);
	virtual void handle_features_reply(rofl::cofdpt * dpt, rofl::cofmsg_features_reply * msg );
	virtual void handle_get_config_request(rofl::cofctl *ctl, rofl::cofmsg_get_config_request *msg);
	virtual void handle_get_config_reply(rofl::cofdpt * dpt, rofl::cofmsg_get_config_reply * msg );
	
	virtual void handle_error (rofl::cofdpt *, rofl::cofmsg *msg);

	virtual void handle_desc_stats_request(rofl::cofctl *ctl, rofl::cofmsg_desc_stats_request *msg);
	virtual void handle_desc_stats_reply(rofl::cofdpt * dpt, rofl::cofmsg_desc_stats_reply * msg);
	
	virtual void handle_table_stats_request(rofl::cofctl *ctl, rofl::cofmsg_table_stats_request *msg);
	virtual void handle_port_stats_request(rofl::cofctl *ctl, rofl::cofmsg_port_stats_request *pack);
	virtual void handle_flow_stats_request(rofl::cofctl *ctl, rofl::cofmsg_flow_stats_request *pack);
	virtual void handle_aggregate_stats_request(rofl::cofctl *ctl, rofl::cofmsg_aggr_stats_request *pack);
	virtual void handle_queue_stats_request(rofl::cofctl *ctl, rofl::cofmsg_queue_stats_request *pack);
	virtual void handle_experimenter_stats_request(rofl::cofctl *ctl, rofl::cofmsg_stats_request *pack);
	virtual void handle_packet_out(rofl::cofctl *ctl, rofl::cofmsg_packet_out *pack);
	virtual void handle_barrier_request(rofl::cofctl *ctl, rofl::cofmsg_barrier_request *pack);
	virtual void handle_table_mod(rofl::cofctl *ctl, rofl::cofmsg_table_mod *pack);
	virtual void handle_port_mod(rofl::cofctl *ctl, rofl::cofmsg_port_mod *pack);
	virtual void handle_set_config(rofl::cofctl *ctl, rofl::cofmsg_set_config *pack);
	virtual void handle_queue_get_config_request(rofl::cofctl *ctl, rofl::cofmsg_queue_get_config_request *pack);
	virtual void handle_experimenter_message(rofl::cofctl *ctl, rofl::cofmsg_features_request *pack);
	virtual void handle_flow_mod(rofl::cofctl *ctl, rofl::cofmsg_flow_mod *msg);  // from crofbase

	rofl::cofdpt * m_slave;		// the datapath device that we'll be misrepresenting
	rofl::cofctl * m_master;	// the OF controller.

	protected:
	// TODO OF version for master and slave - can be had from m_slave->get_version() and m_master->get_version()
	
	// transaction ID maps - TODO - this may not be a 1:1 mapping
	typedef std::map< uint32_t, uint32_t > xid_map_t;
	xid_map_t m_mxid_sxid;	// maps transaction IDs coming from master to the transaction IDs of the translated messages being sent to the slave
	xid_map_t m_sxid_mxid;	// maps transaction IDs coming in responses from slave to the transaction IDs of the original request from the master
	uint64_t m_slave_dpid;
	uint64_t m_dpid;
//	uint64_t m_master_dpid;
	private:
	uint8_t m_of_version;

};

#endif	// UCL_EE_TRANNY_H
