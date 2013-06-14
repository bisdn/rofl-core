#ifndef MMAP_TEST_H
#define MMAP_TEST_H 1

ROFL_BEGIN_DECLS
#include <netdb.h>
ROFL_END_DECLS

#include <map>

#include "rofl/common/cmacaddr.h"
#include "rofl/common/caddress.h"
#include "rofl/common/crofbase.h"
#include "rofl/common/openflow/cofdpt.h"

#include "ofperftest.h"

using namespace rofl;

class mmap_test :
		public ofperftest
{
private:

	enum mmap_test_timer_t {
		MMAP_TEST_TIMER_BASE = ((0x6271)),
	};

	csocket				*sock;

public:

	mmap_test(unsigned int n_entries = 0);

	virtual
	~mmap_test();

	virtual void
	handle_timeout(int opaque);

private:

};

#endif
