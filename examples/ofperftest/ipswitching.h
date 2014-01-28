#ifndef IPSWITCHING_H
#define IPSWITCHING_H 1

#include <map>
#include "rofl/common/cmacaddr.h"
#include "rofl/common/caddress.h"
#include "rofl/common/crofbase.h"
#include "rofl/common/crofdpt.h"

#include "ofperftest.h"

using namespace rofl;

class ipswitching :
		public ofperftest
{
private:

	// fibentry means: host with address 'addr' is reachable via port 'port_no' and entry expires in 'timeout' seconds
	struct fibentry_t {
		uint16_t		vid;		// VLAN ID the IP address can be reached via, or 0xffff, if untagged
		uint32_t 		port_no;	// port where a certain is attached
		time_t 			timeout;	// timeout event for this FIB entry
		caddress		addr;		// address assigned to this node
		fibentry_t() :
			vid(0xffff), port_no(0), timeout(0), addr(caddress(AF_INET)) {};
	};

	unsigned int 		n_entries;	// number of competing flowmods to be installed
	// a very simple forwarding information base
	std::map<crofdpt*, std::map<caddress, struct fibentry_t> > fib;

	unsigned int 		fib_check_timeout; 		// periodic timeout for removing expired FIB entries
	unsigned int		fm_delete_all_timeout;	// periodic purging of all FLOW-MODs

	enum ipswitching_timer_t {
		IPSWITCHING_TIMER_BASE = ((0x6271)),
		IPSWITCHING_TIMER_FIB,
		IPSWITCHING_TIMER_FLOW_MOD_DELETE_ALL,
	};

public:

	ipswitching(cofhello_elem_versionbitmap const& versionbitmap, unsigned int n_entries = 0);

	virtual
	~ipswitching();

	virtual void
	handle_timeout(int opaque, void *data = (void*)0);

	virtual void
	handle_dpath_open(crofdpt& dpt);

	virtual void
	handle_dpath_close(crofdpt& dpt);

	virtual void
	handle_packet_in(crofdpt& dpt, cofmsg_packet_in& msg, uint8_t aux_id = 0);

private:

	void
	install_flow_mods(crofdpt *dpt, unsigned int n = 0);

	void
	drop_expired_fib_entries();

	void
	flow_mod_delete_all();

	void
	handle_packet_in_ipv4(crofdpt *dpt, cofmsg_packet_in *msg);

	void
	handle_packet_in_arpv4(crofdpt *dpt, cofmsg_packet_in *msg);

	void
	update_fib_table(crofdpt *dpt, cofmsg_packet_in *msg, caddress ip_src);

	void
	flood_vlans(crofdpt *dpt, cofmsg_packet_in *msg, caddress ip_src);
};

#endif
