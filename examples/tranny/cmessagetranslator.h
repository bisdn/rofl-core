#include <vector>
#include <rofl/common/crofbase.h>
#include <rofl/common/openflow/cofdpt.h>
#include <rofl/common/openflow/cofctl.h>
#include <rofl/common/openflow/openflow10.h>

#ifndef CMESSAGETRANSLATER_H
#define CMESSAGETRANSLATER_H 1

class cmessagetranslator {

public:

virtual std::vector<rofl::cofmsg_features_request> translate_features_request( const rofl::cofctl * , rofl::cofmsg_features_request * ) { return std::vector<rofl::cofmsg_features_request>(); }
virtual std::vector<rofl::cofmsg_features_reply> translate_features_reply( const rofl::cofdpt * , rofl::cofmsg_features_reply *  ) { return std::vector<rofl::cofmsg_features_reply>(); }
virtual std::vector<rofl::cofmsg_get_config_request> translate_get_config_request( const rofl::cofctl * , rofl::cofmsg_get_config_request * ) { return std::vector<rofl::cofmsg_get_config_request>(); }
virtual std::vector<rofl::cofmsg_get_config_reply> translate_get_config_reply( const rofl::cofdpt * , rofl::cofmsg_get_config_reply *  ) { return std::vector<rofl::cofmsg_get_config_reply>(); }

virtual std::vector<rofl::cofmsg> translate_error ( const rofl::cofdpt * , rofl::cofmsg * ) { return std::vector<rofl::cofmsg>(); }

virtual std::vector<rofl::cofmsg_desc_stats_request> translate_desc_stats_request( const rofl::cofctl * , rofl::cofmsg_desc_stats_request * ) { return std::vector<rofl::cofmsg_desc_stats_request>(); }
virtual std::vector<rofl::cofmsg_desc_stats_reply> translate_desc_stats_reply( const rofl::cofdpt * , rofl::cofmsg_desc_stats_reply * ) { return std::vector<rofl::cofmsg_desc_stats_reply>(); }

virtual std::vector<rofl::cofmsg_table_stats_request> translate_table_stats_request( const rofl::cofctl * , rofl::cofmsg_table_stats_request * ) { return std::vector<rofl::cofmsg_table_stats_request>(); }
virtual std::vector<rofl::cofmsg_table_stats_reply> translate_table_stats_reply( const rofl::cofdpt * , rofl::cofmsg_table_stats_reply * ) { return std::vector<rofl::cofmsg_table_stats_reply>(); }

virtual std::vector<rofl::cofmsg_port_stats_request> translate_port_stats_request( const rofl::cofctl * , rofl::cofmsg_port_stats_request * ) { return std::vector<rofl::cofmsg_port_stats_request>(); }
virtual std::vector<rofl::cofmsg_flow_stats_request> translate_flow_stats_request( const rofl::cofctl * , rofl::cofmsg_flow_stats_request * ) { return std::vector<rofl::cofmsg_flow_stats_request>(); }
virtual std::vector<rofl::cofmsg_aggr_stats_request> translate_aggregate_stats_request( const rofl::cofctl * , rofl::cofmsg_aggr_stats_request * ) { return std::vector<rofl::cofmsg_aggr_stats_request>(); }
virtual std::vector<rofl::cofmsg_aggr_stats_reply> translate_aggregate_stats_reply( const rofl::cofdpt * , rofl::cofmsg_aggr_stats_reply * ) { return std::vector<rofl::cofmsg_aggr_stats_reply>(); }

virtual std::vector<rofl::cofmsg_queue_stats_request> translate_queue_stats_request( const rofl::cofctl * , rofl::cofmsg_queue_stats_request * ) { return std::vector<rofl::cofmsg_queue_stats_request>(); }
virtual std::vector<rofl::cofmsg_stats_request> translate_experimenter_stats_request( const rofl::cofctl * , rofl::cofmsg_stats_request * ) { return std::vector<rofl::cofmsg_stats_request>(); }
virtual std::vector<rofl::cofmsg_packet_in> translate_packet_in( const rofl::cofdpt * , rofl::cofmsg_packet_in * ) { return std::vector<rofl::cofmsg_packet_in>(); }
virtual std::vector<rofl::cofmsg_packet_out> translate_packet_out( const rofl::cofctl * , rofl::cofmsg_packet_out * ) { return std::vector<rofl::cofmsg_packet_out>(); }
virtual std::vector<rofl::cofmsg_barrier_request> translate_barrier_request( const rofl::cofctl * , rofl::cofmsg_barrier_request * ) { return std::vector<rofl::cofmsg_barrier_request>(); }
virtual std::vector<rofl::cofmsg_barrier_reply> translate_barrier_reply ( const rofl::cofdpt * , rofl::cofmsg_barrier_reply * ) { return std::vector<rofl::cofmsg_barrier_reply>(); }
virtual std::vector<rofl::cofmsg_table_mod> translate_table_mod( const rofl::cofctl * , rofl::cofmsg_table_mod * ) { return std::vector<rofl::cofmsg_table_mod>(); }
virtual std::vector<rofl::cofmsg_port_mod> translate_port_mod( const rofl::cofctl * , rofl::cofmsg_port_mod * ) { return std::vector<rofl::cofmsg_port_mod>(); }
virtual std::vector<rofl::cofmsg_set_config> translate_set_config( const rofl::cofctl * , rofl::cofmsg_set_config * ) { return std::vector<rofl::cofmsg_set_config>(); }
virtual std::vector<rofl::cofmsg_queue_get_config_request> translate_queue_get_config_request( const rofl::cofctl * , rofl::cofmsg_queue_get_config_request * ) { return std::vector<rofl::cofmsg_queue_get_config_request>(); }
virtual std::vector<rofl::cofmsg_features_request> translate_experimenter_message( const rofl::cofctl * , rofl::cofmsg_features_request * ) { return std::vector<rofl::cofmsg_features_request>(); }
virtual std::vector<rofl::cofmsg_flow_mod> translate_flow_mod( const rofl::cofctl * , rofl::cofmsg_flow_mod * ) { return std::vector<rofl::cofmsg_flow_mod>(); }

};

#endif 
