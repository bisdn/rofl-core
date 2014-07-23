/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __TEST_IPV6_H__
#define __TEST_IPV6_H__

#include "rofl.h"
#include "rofl/datapath/pipeline/physical_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/of1x_switch.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_pipeline.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_flow_entry.h"
#include "rofl/datapath/pipeline/openflow/openflow1x/pipeline/of1x_instruction.h"
#include "rofl/datapath/pipeline/common/large_types.h"


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
