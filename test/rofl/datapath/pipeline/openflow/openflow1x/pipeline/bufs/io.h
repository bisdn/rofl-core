/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FAKE_IO
#define FAKE_IO

#include <rofl/datapath/pipeline/common/datapacket.h>

extern unsigned int replicas;
extern unsigned int drops;
extern unsigned int outputs;
extern unsigned int allocated;
extern unsigned int released;


void init_io();
void destroy_io();
void reset_io_state();
datapacket_t* allocate_buffer();
void release_buffer(datapacket_t* pkt);

#endif
