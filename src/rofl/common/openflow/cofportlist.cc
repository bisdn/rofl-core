/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cofportlist.h"

using namespace rofl;

cofportlist::cofportlist()
{
	WRITELOG(COFINST, DBG, "cofportlist(%p)::cofportlist()", this);
}



cofportlist::~cofportlist()
{
	WRITELOG(COFINST, DBG, "cofportlist(%p)::~cofportlist()", this);
}


#if 0
template<class T>
std::vector<cofport>&
cofportlist::unpack(
		T *ports,
		size_t portlen)
throw (ePortListInval)
{
	clear(); // clears bcvec

	// sanity check: bclen must be of size at least of ofp_bucket
	if (portlen < (int)sizeof(T))
		return elems;

	// first port
	T *porthdr = ports;

	while (portlen > 0) {

		if (portlen < sizeof(T))
			throw ePortListInval();

		next() = cofport(porthdr, sizeof(T) );

		portlen -= sizeof(T);
		porthdr++;
	}

	return elems;
}



template<class T>
T*
cofportlist::pack(
	T *ports,
	size_t portlen) throw (ePortListInval)
{
	size_t needed_inlen = length();

	if (portlen < needed_inlen)
		throw ePortListInval();

	T *porthdr = ports; // first ofp_port header

	cofportlist::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		cofport& port = (*it);

		porthdr = (T*)
				((uint8_t*)(port.pack(porthdr, port.length())) + port.length());
	}

	return ports;
}
#endif


std::vector<cofport>&
cofportlist::unpack(
		struct openflow10::ofp_port *ports,
		size_t portlen)
throw (ePortListInval)
{
	clear(); // clears bcvec

	// sanity check: bclen must be of size at least of ofp_bucket
	if (portlen < (int)sizeof(struct openflow10::ofp_port))
		return elems;

	// first port
	struct openflow10::ofp_port *porthdr = ports;

	while (portlen > 0) {

		if (portlen < sizeof(struct openflow10::ofp_port))
			throw ePortListInval();

		next() = cofport(porthdr, sizeof(struct openflow10::ofp_port) );

		portlen -= sizeof(struct openflow10::ofp_port);
		porthdr++;
	}

	return elems;
}




struct openflow10::ofp_port*
cofportlist::pack(
	struct openflow10::ofp_port *ports,
	size_t portlen) const throw (ePortListInval)
{
	size_t needed_inlen = length();

	if (portlen < needed_inlen)
		throw ePortListInval();

	struct openflow10::ofp_port *porthdr = ports; // first ofp_port header

	cofportlist::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		cofport const& port = (*it);

		porthdr = (struct openflow10::ofp_port*)
				((uint8_t*)(port.pack(porthdr, port.length())) + port.length());
	}

	return ports;
}



std::vector<cofport>&
cofportlist::unpack(
		struct openflow12::ofp_port *ports,
		size_t portlen)
throw (ePortListInval)
{
	clear(); // clears bcvec

	// sanity check: bclen must be of size at least of ofp_bucket
	if (portlen < (int)sizeof(struct openflow12::ofp_port))
		return elems;

	// first port
	struct openflow12::ofp_port *porthdr = ports;

	while (portlen > 0) {

		if (portlen < sizeof(struct openflow12::ofp_port))
			throw ePortListInval();

		next() = cofport(porthdr, sizeof(struct openflow12::ofp_port) );

		portlen -= sizeof(struct openflow12::ofp_port);
		porthdr++;
	}

	return elems;
}




struct openflow12::ofp_port*
cofportlist::pack(
	struct openflow12::ofp_port *ports,
	size_t portlen) const throw (ePortListInval)
{
	size_t needed_inlen = length();

	if (portlen < needed_inlen)
		throw ePortListInval();

	struct openflow12::ofp_port *porthdr = ports; // first ofp_port header

	cofportlist::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		cofport const& port = (*it);

		porthdr = (struct openflow12::ofp_port*)
				((uint8_t*)(port.pack(porthdr, port.length())) + port.length());
	}

	return ports;
}



size_t
cofportlist::length() const
{
	size_t inlen = 0;
	cofportlist::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		inlen += (*it).length();
	}
	return inlen;
}




cofport&
cofportlist::find_port(
		uint32_t port_no)
throw (ePortListNotFound)
{
	cofportlist::iterator it;
	if ((it = find_if(elems.begin(), elems.end(),
			cofport_find_port_no(port_no))) == elems.end())
	{
		throw ePortListNotFound();
	}
	return (*it);
}


#if 0
void
cofportlist::test()
{
	cofport p1;

	p1.set_port_no(17);
	p1.set_hwaddr(cmacaddr("00:11:11:11:11:11"));
	p1.set_name(std::string("geb0"));
	p1.set_advertised(0xaaaa);
	p1.set_curr(0xbbbb);
	p1.set_config(0xcccc);

	cofportlist pl1;

	pl1[0] = cofport();
	pl1[1] = cofport();
	pl1[2] = cofport();
	pl1[3] = p1;

	pl1[0].set_port_no(8);
	pl1[0].set_name(std::string("ge1"));
	pl1[0].set_advertised(77777);
	pl1[0].set_hwaddr(cmacaddr("00:22:22:22:22:22"));

	fprintf(stderr, "portlist => %s\n", pl1.c_str());

	cmemory mem(pl1.length());

	pl1.pack((struct openflow10::ofp_port*)mem.somem(), mem.memlen());

	fprintf(stderr, "portlist.packed => %s\n", mem.c_str());

	cofportlist pl2;

	pl2.unpack((struct openflow10::ofp_port*)mem.somem(), mem.memlen());

	fprintf(stderr, "portlist.unpacked => %s\n", pl2.c_str());

#if 0
	inlist[0] = cofinst_write_actions();
	inlist[0].actions[0] = cofaction_set_mpls_label(111111);

	fprintf(stderr, "XXX => %s\n", inlist.c_str());

	fprintf(stderr, "--------------------------\n");

	cofportlist inlist2;

	inlist2[0] = cofinst_apply_actions();
	inlist2[0].actions[0] = cofaction_output(1);
	inlist2[1] = cofinst_clear_actions();
	inlist2[2] = cofinst_write_actions();
	inlist2[2].actions[0] = cofaction_set_vlan_vid(1111);
	inlist2[2].actions[1] = cofaction_set_mpls_tc(7);

	fprintf(stderr, "YYY => %s\n", inlist2.c_str());

	fprintf(stderr, "--------------------------\n");

	inlist2 = inlist;

	fprintf(stderr, "ZZZ => %s\n", inlist2.c_str());

	fprintf(stderr, "--------------------------\n");
#endif
}
#endif
