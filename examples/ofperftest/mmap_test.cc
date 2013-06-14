#include "mmap_test.h"

#include <inttypes.h>

mmap_test::mmap_test(unsigned int n_entries) :
	sock(0)
{
	new csocket(this, AF_INET, SOCK_DGRAM, getprotobyname("udp").p_name, 10);
}



mmap_test::~mmap_test()
{

}



void
mmap_test::handle_timeout(int opaque)
{
	switch (opaque) {
	default:
		crofbase::handle_timeout(opaque);
	}
}




