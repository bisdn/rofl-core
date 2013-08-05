#ifndef __TEST_IPV6_H__
#define __TEST_IPV6_H__

#include "rofl.h"
#include "rofl/datapath/pipeline/physical_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow12/of12_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_pipeline.h"
#include "rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_flow_entry.h"
#include "rofl/datapath/pipeline/openflow/openflow12/pipeline/of12_instruction.h"


int ipv6_set_up(void);
int ipv6_tear_down(void);
void ipv6_basic_test(void);
void ipv6_utern_test(void);
void ipv6_install_flow_mod(void);
void ipv6_alike_test_low(void);
void ipv6_alike_test_high(void);
void ipv6_alike_test_wrong(void);
void ipv6_install_flow_mod_complete(void);
void icmpv6_install_flow_mod_complete(void);


#endif //__TEST_IPV6_H__