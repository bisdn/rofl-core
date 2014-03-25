#include "pp_isolation.h"

//Include all NON-pp headers in the pipeline
#include "rofl/datapath/pipeline/physical_switch.h"
#include "rofl/datapath/pipeline/monitoring.h"
#include "rofl/datapath/pipeline/port_queue.h"
#include "rofl/datapath/pipeline/platform/atomic_operations.h"
#include "rofl/datapath/pipeline/platform/timing.h"
#include "rofl/datapath/pipeline/platform/likely.h"
#include "rofl/datapath/pipeline/platform/memory.h"
#include "rofl/datapath/pipeline/platform/lock.h"
#include "rofl/datapath/pipeline/platform/packet.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_timers.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_instruction.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_statistics.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_match.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_utils.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/matching_algorithms_available.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/loop/of1x_loop_match.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/matching_algorithms.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_group_table.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_action.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_table.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/of1x_async_events_hooks.h"
#include "rofl/datapath/pipeline/openflow/of_switch.h"
#include "rofl/datapath/pipeline/common/protocol_constants.h"
#include "rofl/datapath/pipeline/common/wrap_types.h"
#include "rofl/datapath/pipeline/common/endianness.h"
#include "rofl/datapath/pipeline/common/bitmap.h"
#include "rofl/datapath/pipeline/common/ternary_fields.h"
#include "rofl/datapath/pipeline/common/packet_matches.h"
#include "rofl/datapath/pipeline/common/large_types.h"
#include "rofl/datapath/pipeline/common/datapacket.h"
#include "rofl/datapath/pipeline/common/ipv6_exthdr.h"
#include "rofl/datapath/pipeline/util/logging.h"
#include "rofl/datapath/pipeline/util/time.h"
#include "rofl/datapath/pipeline/switch_port.h"


//Throw compilation error if ROFL_PIPELINE_PP_PRESENT is set
#ifdef ROFL_PIPELINE_PP_PRESENT
	#error Some packet processing API (_pp) header is being include in a non __pp.h file
#endif

void pp_test(void)
{	
	//Do nothing. This is a compilation test only	
}
