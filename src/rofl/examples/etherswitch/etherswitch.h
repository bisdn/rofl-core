#ifndef ETHERSWITCH_H
#define ETHERSWITCH_H 1

#include <map>

#include "rofl/common/cmacaddr.h"
#include "rofl/common/caddress.h"
#include "rofl/common/crofbase.h"
#include "rofl/common/openflow/cofdpt.h"

using namespace rofl;

class etherswitch :
		public crofbase
{
private:

	// a very simple forwarding information base
	std::map<cofdpt*, std::map<uint16_t, std::map<cmacaddr, uint32_t> > > fib;

public:

	etherswitch();

	virtual
	~etherswitch();

	virtual void
	handle_dpath_open(cofdpt *dpt);

	virtual void
	handle_dpath_close(cofdpt *dpt);

	virtual void
	handle_packet_in(cofdpt *dpt, cofmsg_packet_in *msg);
};

#endif
