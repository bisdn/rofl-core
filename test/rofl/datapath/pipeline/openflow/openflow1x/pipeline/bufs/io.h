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
