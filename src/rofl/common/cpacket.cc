/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cpacket.h"

cpacket::cpacket(
			size_t size) :
		packet_receive_time(time(NULL)),
		out_port(0)
{
	init();
	piobuf.push_back(new cmemory(size));
	classify(OFPP_CONTROLLER);
}


cpacket::cpacket(
		cmemory *mem,
		uint32_t in_port,
		bool do_classify) :
		plength(0),
		packet_receive_time(time(NULL)),
		in_port(in_port),
		out_port(0)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::cpacket()", this);
	init();
	if (0 != mem)
	{
		piobuf.push_back(mem);
		plength += mem->memlen();
	}
	if (do_classify)
	{
		classify(in_port);
	}
	else
	{
		piovec.push_back(new fframe(mem->somem(), mem->memlen()));
	}
}

cpacket::cpacket(
		uint8_t *buf,
		size_t buflen,
		uint32_t in_port,
		bool do_classify) :
		plength(buflen),
		packet_receive_time(time(NULL)),
		in_port(in_port),
		out_port(0)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::cpacket()", this);
	init();
	cmemory *mem = new cmemory(buf, buflen);
	piobuf.push_back(mem);
	if (do_classify)
	{
		classify(in_port);
	}
	else
	{
		piovec.push_back(new fframe(mem->somem(), mem->memlen()));
	}
}


cpacket::cpacket(const cpacket& pack)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::cpacket()", this);
	init();
	*this = pack;
	classify(in_port);
}


cpacket::~cpacket()
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::~cpacket()", this);
	clear(); // removes all cmemory and ciovec instances from heap

	pthread_rwlock_destroy(&ac_rwlock);
}


void
cpacket::init()
{
	pthread_rwlock_init(&ac_rwlock, NULL);

	for (unsigned int i = 0; i < MAX_FRAME; i++)
	{
		anchors.push_back(std::deque<fframe*>());
	}
}


void
cpacket::clear()
{
	cleanup(); // clean up anchors and piovec

	for (std::deque<cmemory*>::iterator
				it = piobuf.begin(); it != piobuf.end(); ++it)
	{
		delete (*it);
	}

	plength = 0;
}


cpacket&
cpacket::operator=(const cpacket &p)
{
	if (this == &p)
		return *this;

	clear();

	for (std::deque<cmemory*>::const_iterator
				it = p.piobuf.begin(); it != p.piobuf.end(); ++it)
	{
		piobuf.push_back(new cmemory(*(*it)));
	}

	packet_receive_time = p.packet_receive_time;

#if 0
	for (std::list<ciovec*>::const_iterator
			it = p.piovec.begin(); it != p.piovec.end(); ++it)
	{
		// TODO: ...
		throw eNotImplemented();
	}
#endif

	plength = p.plength;

	return *this;
}


uint8_t&
cpacket::operator[] (size_t index) throw (ePacketOutOfRange)
{
	if (index >= plength)
	{
		throw ePacketOutOfRange();
	}

	size_t i = 0;
	uint8_t* j = 0;

	for (std::list<fframe*>::iterator
			it = piovec.begin(); it != piovec.end(); ++it)
	{
		fframe *frame = (*it);

		if ((i + frame->framelen()) < index)
		{
			i += frame->framelen();
			continue;
		}

		j = ((uint8_t*)frame->framelen() + (index - i));
	}
	return *j;
}


void
cpacket::set_flag_no_packet_in()
{
	flags.set(FLAG_NO_PACKET_IN);
}


bool
cpacket::get_flag_no_packet_in()
{
	return flags.test(FLAG_NO_PACKET_IN);
}


cmemory*
cpacket::to_mem()
{
	size_t p_len = 0;

	for (std::list<fframe*>::iterator
			it = piovec.begin(); it != piovec.end(); ++it)
	{
		p_len += (*it)->framelen();
	}

	cmemory *mem = new cmemory(p_len);

	p_len = 0;

	for (std::list<fframe*>::iterator
			it = piovec.begin(); it != piovec.end(); ++it)
	{
		fframe& frame = *(*it);
		memcpy(mem->somem() + p_len, frame.soframe(), frame.framelen());
		p_len += frame.framelen();
	}

	return mem;
}


void
cpacket::pack(
		uint8_t *dest,
		size_t len) throw (ePacketInval)
{
	uint8_t *ptr = dest;

	for (std::list<fframe*>::iterator
			it = piovec.begin(); it != piovec.end(); ++it)
	{
		fframe *frame = (*it);

		if (len >= frame->framelen())
		{
			memcpy(ptr, frame->soframe(), frame->framelen());
			len -= frame->framelen();
			ptr += frame->framelen();
		}
		else
		{
			memcpy(ptr, frame->soframe(), len);
			break;
		}
	}
}


void
cpacket::unpack(
		uint32_t in_port,
		uint8_t *src,
		size_t len)
{
	cmemory *mem = new cmemory(src, len);
	unpack(in_port, mem);
}



void
cpacket::unpack(
		uint32_t in_port,
		cmemory *mem)
{
	clear();

	this->in_port = in_port;
	piobuf.push_back(mem);
	plength += mem->memlen();

	classify(in_port);
}


size_t
cpacket::length()
{
	return plength;
}


uint8_t*
cpacket::insert(
		size_t offset,
		size_t len) throw (ePacketOutOfRange)
{
	cmemory *mem = new cmemory(len);
	piobuf.push_back(mem);
	plength += mem->memlen();
	std::vector<fframe*> n_iov;
	n_iov.push_back(new fframe(mem->somem(), mem->memlen()));

	unsigned int i = 0;

	for (std::list<fframe*>::iterator
			it = piovec.begin(); it != piovec.end(); ++it)
	{
		fframe *frame = (*it);

		if ((i + frame->framelen()) > offset)
		{
			std::list<fframe*>::iterator jt = it;
			std::advance(jt, 1);

			n_iov.push_back(new fframe(
					(uint8_t*)frame->soframe() 	+ (offset - i),
							  frame->framelen() - (offset - i)));

			piovec.insert(jt, n_iov.begin(), n_iov.end());

			throw eNotImplemented();
#if 0
			frame->reset() -= (offset - i);
#endif

			return mem->somem();
		}
		else if ((i + frame->framelen()) == offset)
		{
			std::list<fframe*>::iterator jt = it;
			std::advance(jt, 1);

			piovec.insert(jt, n_iov[0]);

			return mem->somem();
		}

		i += frame->framelen();
	}

	return mem->somem();
}


uint8_t*
cpacket::remove(
		size_t offset,
		size_t len) throw (ePacketOutOfRange)
{
	throw eNotImplemented();

	return 0;
}



const char*
cpacket::c_str()
{
	cvastring vas(4096);

	info.assign(vas("cpacket(%p) \n", this));

	info.append(vas("  %s", oxmlist.c_str()));

	for(unsigned int i = 0; i < anchors[ETHER_FRAME].size(); ++i)
	{
		info.append(vas("  ether[%d]:%s\n", i, ether(i).c_str()));
	}
	for(unsigned int i = 0; i < anchors[VLAN_FRAME].size(); ++i)
	{
		info.append(vas("   vlan[%d]:%s\n", i, vlan(i).c_str()));
	}
	for(unsigned int i = 0; i < anchors[MPLS_FRAME].size(); ++i)
	{
		info.append(vas("   mpls[%d]:%s\n", i, mpls(i).c_str()));
	}
	for(unsigned int i = 0; i < anchors[PPPOE_FRAME].size(); ++i)
	{
		info.append(vas("  pppoe[%d]:%s\n", i, pppoe(i).c_str()));
	}
	for(unsigned int i = 0; i < anchors[PPP_FRAME].size(); ++i)
	{
		info.append(vas("    ppp[%d]:%s\n", i, ppp(i).c_str()));
	}
	for(unsigned int i = 0; i < anchors[IPV4_FRAME].size(); ++i)
	{
		info.append(vas("   ipv4[%d]:%s\n", i, ipv4(i).c_str()));
	}
	for(unsigned int i = 0; i < anchors[ICMPV4_FRAME].size(); ++i)
	{
		info.append(vas(" icmpv4[%d]:%s\n", i, icmpv4(i).c_str()));
	}
	for(unsigned int i = 0; i < anchors[ARPV4_FRAME].size(); ++i)
	{
		info.append(vas("  arpv4[%d]:%s\n", i, arpv4(i).c_str()));
	}
	for(unsigned int i = 0; i < anchors[UDP_FRAME].size(); ++i)
	{
		info.append(vas("    udp[%d]:%s\n", i, udp(i).c_str()));
	}
	for(unsigned int i = 0; i < anchors[TCP_FRAME].size(); ++i)
	{
		info.append(vas("    tcp[%d]:%s\n", i, tcp(i).c_str()));
	}

	for(std::list<fframe*>::iterator
			it = piovec.begin(); it != piovec.end(); ++it)
	{
		info.append(vas("  fframe:%s\n", (*it)->fframe::c_str()));
	}

	for(std::deque<cmemory*>::iterator
			it = piobuf.begin(); it != piobuf.end(); ++it)
	{
		info.append(vas(" cmemory:%s\n", (*it)->c_str()));
	}

	return info.c_str();
}


void
cpacket::test()
{
#if 0
	cmemory *m = new cmemory(16);

	fetherframe ether(m->somem(), m->memlen(), m->memlen());

	cpacket a(m);


	uint8_t *ptr = a.insert(sizeof(struct fetherframe::eth_hdr_t),
			sizeof(struct fvlanframe::vlan_hdr_t));
#endif
}



fetherframe&
cpacket::ether(int i) throw (ePacketTypeError)
{
	i = (i >= 0) ? i : anchors[ETHER_FRAME].size() + i;
	if ((i < 0) || (i >= (int)anchors[ETHER_FRAME].size()))
		throw ePacketTypeError();
	return *(dynamic_cast<fetherframe*>(anchors[ETHER_FRAME][i]));
}


fvlanframe&
cpacket::vlan(int i) throw (ePacketTypeError)
{
	i = (i >= 0) ? i : anchors[VLAN_FRAME].size() + i;
	if ((i < 0) || (i >= (int)anchors[VLAN_FRAME].size()))
		throw ePacketTypeError();
	return *(dynamic_cast<fvlanframe*>(anchors[VLAN_FRAME][i]));
}


fpppoeframe&
cpacket::pppoe(int i) throw (ePacketTypeError)
{
	i = (i >= 0) ? i : anchors[PPPOE_FRAME].size() + i;
	if ((i < 0) || (i >= (int)anchors[PPPOE_FRAME].size()))
		throw ePacketTypeError();
	return *(dynamic_cast<fpppoeframe*>(anchors[PPPOE_FRAME][i]));
}


fpppframe&
cpacket::ppp(int i) throw (ePacketTypeError)
{
	i = (i >= 0) ? i : anchors[PPP_FRAME].size() + i;
	if ((i < 0) || (i >= (int)anchors[PPP_FRAME].size()))
		throw ePacketTypeError();
	return *(dynamic_cast<fpppframe*>(anchors[PPP_FRAME][i]));
}


fmplsframe&
cpacket::mpls(int i) throw (ePacketTypeError)
{
	i = (i >= 0) ? i : anchors[MPLS_FRAME].size() + i;
	if ((i < 0) || (i >= (int)anchors[MPLS_FRAME].size()))
		throw ePacketTypeError();
	return *(dynamic_cast<fmplsframe*>(anchors[MPLS_FRAME][i]));
}


farpv4frame&
cpacket::arpv4(int i) throw (ePacketTypeError)
{
	i = (i >= 0) ? i : anchors[ARPV4_FRAME].size() + i;
	if ((i < 0) || (i >= (int)anchors[ARPV4_FRAME].size()))
		throw ePacketTypeError();
	return *(dynamic_cast<farpv4frame*>(anchors[ARPV4_FRAME][i]));
}


fipv4frame&
cpacket::ipv4(int i) throw (ePacketTypeError)
{
	i = (i >= 0) ? i : anchors[IPV4_FRAME].size() + i;
	if ((i < 0) || (i >= (int)anchors[IPV4_FRAME].size()))
		throw ePacketTypeError();
	return *(dynamic_cast<fipv4frame*>(anchors[IPV4_FRAME][i]));
}


ficmpv4frame&
cpacket::icmpv4(int i) throw (ePacketTypeError)
{
	i = (i >= 0) ? i : anchors[ICMPV4_FRAME].size() + i;
	if ((i < 0) || (i >= (int)anchors[ICMPV4_FRAME].size()))
		throw ePacketTypeError();
	return *(dynamic_cast<ficmpv4frame*>(anchors[ICMPV4_FRAME][i]));
}


fudpframe&
cpacket::udp(int i) throw (ePacketTypeError)
{
	i = (i >= 0) ? i : anchors[UDP_FRAME].size() + i;
	if ((i < 0) || (i >= (int)anchors[UDP_FRAME].size()))
		throw ePacketTypeError();
	return *(dynamic_cast<fudpframe*>(anchors[UDP_FRAME][i]));
}


ftcpframe&
cpacket::tcp(int i) throw (ePacketTypeError)
{
	i = (i >= 0) ? i : anchors[TCP_FRAME].size() + i;
	if ((i < 0) || (i >= (int)anchors[TCP_FRAME].size()))
		throw ePacketTypeError();
	return *(dynamic_cast<ftcpframe*>(anchors[TCP_FRAME][i]));
}


void
cpacket::set_field(coxmatch const& oxm)
{
	switch (oxm.get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
	{
		switch (oxm.get_oxm_field()) {
		case OFPXMT_OFB_ETH_DST:
			if (not anchors[ETHER_FRAME].empty())
			{
				cmacaddr maddr(oxm.oxm_uint48t->value, OFP_ETH_ALEN);
				ether().set_dl_dst(maddr);
				oxmlist[OFPXMT_OFB_ETH_DST] = coxmatch_ofb_eth_dst(maddr);
			}
			break;
		case OFPXMT_OFB_ETH_SRC:
			if (not anchors[ETHER_FRAME].empty())
			{
				cmacaddr maddr(oxm.oxm_uint48t->value, OFP_ETH_ALEN);
				ether().set_dl_src(maddr);
				oxmlist[OFPXMT_OFB_ETH_SRC] = coxmatch_ofb_eth_src(maddr);
			}
			break;
		case OFPXMT_OFB_ETH_TYPE:
			if (not anchors[ETHER_FRAME].empty())
			{
				ether().set_dl_type(oxm.uint16());
				oxmlist[OFPXMT_OFB_ETH_TYPE] = oxm;
			}
			break;
		case OFPXMT_OFB_VLAN_VID:
			if (not anchors[VLAN_FRAME].empty())
			{
				vlan().set_dl_vlan_id(oxm.uint16());
				oxmlist[OFPXMT_OFB_VLAN_VID] = oxm;
			}
			break;
		case OFPXMT_OFB_VLAN_PCP:
			if (not anchors[VLAN_FRAME].empty())
			{
				vlan().set_dl_vlan_pcp(oxm.uint8());
				oxmlist[OFPXMT_OFB_VLAN_PCP] = oxm;
			}
			break;
		case OFPXMT_OFB_IP_DSCP:
			if (not anchors[IPV4_FRAME].empty())
			{
				ipv4().set_ipv4_dscp(oxm.uint8());
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_IP_DSCP] = oxm;
			}
			break;
		case OFPXMT_OFB_IP_ECN:
			if (not anchors[IPV4_FRAME].empty())
			{
				ipv4().set_ipv4_ecn(oxm.uint8());
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_IP_ECN] = oxm;
			}
			break;
		case OFPXMT_OFB_IP_PROTO:
			if (not anchors[IPV4_FRAME].empty())
			{
				ipv4().set_ipv4_proto(oxm.uint8());
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_IP_PROTO] = oxm;
			}
			break;
		case OFPXMT_OFB_IPV4_SRC:
			if (not anchors[IPV4_FRAME].empty())
			{
				ipv4().set_ipv4_src(oxm.uint32());
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_IPV4_SRC] = oxm;
			}
			break;
		case OFPXMT_OFB_IPV4_DST:
			if (not anchors[IPV4_FRAME].empty())
			{
				ipv4().set_ipv4_dst(oxm.uint32());
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_IPV4_DST] = oxm;
			}
			break;
		case OFPXMT_OFB_TCP_SRC:
			if (not anchors[TCP_FRAME].empty())
			{
				tcp().set_sport(oxm.uint16());
				tcp().tcp_calc_checksum(
						ipv4(-1).get_ipv4_src(),
						ipv4(-1).get_ipv4_dst(),
						ipv4(-1).get_ipv4_proto(),
						tcp().framelen());
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_TCP_SRC] = oxm;
			}
			break;
		case OFPXMT_OFB_TCP_DST:
			if (not anchors[TCP_FRAME].empty())
			{
				tcp().set_dport(oxm.uint16());
				tcp().tcp_calc_checksum(
						ipv4(-1).get_ipv4_src(),
						ipv4(-1).get_ipv4_dst(),
						ipv4(-1).get_ipv4_proto(),
						tcp().framelen());
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_TCP_DST] = oxm;
			}
			break;
		case OFPXMT_OFB_UDP_SRC:
			if (not anchors[UDP_FRAME].empty())
			{
				udp().set_sport(oxm.uint16());
				udp().udp_calc_checksum(
						ipv4(-1).get_ipv4_src(),
						ipv4(-1).get_ipv4_dst(),
						ipv4(-1).get_ipv4_proto(),
						udp().framelen());
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_UDP_SRC] = oxm;
			}
			break;
		case OFPXMT_OFB_UDP_DST:
			if (not anchors[UDP_FRAME].empty())
			{
				udp().set_dport(oxm.uint16());
				udp().udp_calc_checksum(
						ipv4(-1).get_ipv4_src(),
						ipv4(-1).get_ipv4_dst(),
						ipv4(-1).get_ipv4_proto(),
						udp().framelen());
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_UDP_DST] = oxm;
			}
			break;
		case OFPXMT_OFB_SCTP_SRC:
			if (not anchors[SCTP_FRAME].empty())
			{
#if 0
				sctp().set_sport(oxm.uint16());
				sctp().udp_calc_checksum(
						ipv4(-1).get_ipv4_src(),
						ipv4(-1).get_ipv4_dst(),
						ipv4(-1).get_ipv4_proto(),
						sctp().framelen());
#endif
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_SCTP_SRC] = oxm;
			}
			break;
		case OFPXMT_OFB_SCTP_DST:
			if (not anchors[SCTP_FRAME].empty())
			{
#if 0
				sctp().set_dport(oxm.uint16());
				sctp().udp_calc_checksum(
						ipv4(-1).get_ipv4_src(),
						ipv4(-1).get_ipv4_dst(),
						ipv4(-1).get_ipv4_proto(),
						sctp().framelen());
#endif
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_SCTP_DST] = oxm;
			}
			break;
		case OFPXMT_OFB_ICMPV4_TYPE:
			if (not anchors[ICMPV4_FRAME].empty())
			{
				icmpv4().set_icmp_type(oxm.uint8());
				icmpv4().icmpv4_calc_checksum();
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_ICMPV4_TYPE] = oxm;
			}
			break;
		case OFPXMT_OFB_ICMPV4_CODE:
			if (not anchors[ICMPV4_FRAME].empty())
			{
				icmpv4().set_icmp_code(oxm.uint8());
				icmpv4().icmpv4_calc_checksum();
				ipv4().ipv4_calc_checksum();
				oxmlist[OFPXMT_OFB_ICMPV4_CODE] = oxm;
			}
			break;
		case OFPXMT_OFB_ARP_OP:
		case OFPXMT_OFB_ARP_SPA:
		case OFPXMT_OFB_ARP_TPA:
		case OFPXMT_OFB_ARP_SHA:
		case OFPXMT_OFB_ARP_THA:
		case OFPXMT_OFB_IPV6_SRC:
		case OFPXMT_OFB_IPV6_DST:
		case OFPXMT_OFB_IPV6_FLABEL:
		case OFPXMT_OFB_ICMPV6_TYPE:
		case OFPXMT_OFB_ICMPV6_CODE:
		case OFPXMT_OFB_IPV6_ND_TARGET:
		case OFPXMT_OFB_IPV6_ND_SLL:
		case OFPXMT_OFB_IPV6_ND_TLL:
			WRITELOG(CPACKET, WARN, "cpacket(%p)::set_field() "
					"NOT IMPLEMENTED! => class:0x%x field:%d, ignoring",
					this, oxm.get_oxm_class(), oxm.get_oxm_field());
			break;
		case OFPXMT_OFB_MPLS_LABEL:
			if (not anchors[MPLS_FRAME].empty())
			{
				mpls().set_mpls_label(oxm.uint32());
				oxmlist[OFPXMT_OFB_MPLS_LABEL] = oxm;
			}
			break;
		case OFPXMT_OFB_MPLS_TC:
			if (not anchors[MPLS_FRAME].empty())
			{
				mpls().set_mpls_tc(oxm.uint8());
				oxmlist[OFPXMT_OFB_MPLS_TC] = oxm;
			}
			break;
		/* PPP/PPPoE related extensions */
		case OFPXMT_OFB_PPPOE_CODE:
			if (not anchors[PPPOE_FRAME].empty())
			{
				pppoe().set_pppoe_code(oxm.uint8());
				oxmlist[OFPXMT_OFB_PPPOE_CODE] = oxm;
			}
			break;
		case OFPXMT_OFB_PPPOE_TYPE:
			if (not anchors[PPPOE_FRAME].empty())
			{
				pppoe().set_pppoe_type(oxm.uint8());
				oxmlist[OFPXMT_OFB_PPPOE_TYPE] = oxm;
			}
			break;
		case OFPXMT_OFB_PPPOE_SID:
			if (not anchors[PPPOE_FRAME].empty())
			{
				pppoe().set_pppoe_sessid(oxm.uint16());
				oxmlist[OFPXMT_OFB_PPPOE_SID] = oxm;
			}
			break;
		case OFPXMT_OFB_PPP_PROT:
			if (not anchors[PPP_FRAME].empty())
			{
				ppp().set_ppp_prot(oxm.uint16());
				oxmlist[OFPXMT_OFB_PPP_PROT] = oxm;
			}
			break;
		default:
			WRITELOG(CPACKET, WARN, "cpacket(%p)::set_field() "
					"don't know how to handle class:0x%x field:%d, ignoring",
					this, oxm.get_oxm_class(), oxm.get_oxm_field());
			break;
		}
		break;
	}
	default:
		WRITELOG(CPACKET, WARN, "cpacket(%p)::set_field() "
				"don't know how to handle class:0x%x field:%d, ignoring",
				this, oxm.get_oxm_class(), oxm.get_oxm_field());
		break;
	}
}


void
cpacket::dl_set_dl_src(uint8_t *dl_addr, size_t dl_addrlen)
{
	if (dl_addrlen != OFP_ETH_ALEN)
	{
		return;
	}
	if (anchors[ETHER_FRAME].empty())
	{
		return;
	}

	memcpy(ether().eth_hdr->dl_src, dl_addr, dl_addrlen);
}


void
cpacket::dl_set_dl_dst(uint8_t *dl_addr, size_t dl_addrlen)
{
	if (dl_addrlen != OFP_ETH_ALEN)
	{
		return;
	}
	if (anchors[ETHER_FRAME].empty())
	{
		return;
	}

	memcpy(ether().eth_hdr->dl_dst, dl_addr, dl_addrlen);
}


void
cpacket::vlan_set_vid(uint16_t vid /* host byte order */)
{
	// no VLAN header present? => ignore command!
	if (anchors[VLAN_FRAME].empty())
	{
		return;
	}

	vlan().set_dl_vlan_id(vid);
}


void
cpacket::vlan_set_pcp(uint8_t pcp)
{
	// no VLAN header present? => ignore command!
	if (anchors[VLAN_FRAME].empty())
	{
		return;
	}

	vlan().set_dl_vlan_pcp(pcp);
}


void
cpacket::nw_set_nw_src(uint32_t nw_src)
{
	if (not anchors[IPV4_FRAME].empty())
	{
		ipv4().set_ipv4_src(nw_src);
		ipv4().ipv4_calc_checksum();
	}
	else if (not anchors[ARPV4_FRAME].empty())
	{
		arpv4().set_nw_src(nw_src);
	}
}


void
cpacket::nw_set_nw_dst(uint32_t nw_dst)
{
	if (not anchors[IPV4_FRAME].empty())
	{
		ipv4().set_ipv4_dst(nw_dst);
		ipv4().ipv4_calc_checksum();
	}
	else if (not anchors[ARPV4_FRAME].empty())
	{
		arpv4().set_nw_dst(nw_dst);
	}
}


void
cpacket::nw_set_nw_dscp(uint8_t dscp)
{
	if (anchors[IPV4_FRAME].empty())
	{
		return;
	}

	ipv4().set_ipv4_dscp(dscp);
	ipv4().ipv4_calc_checksum();
}


void
cpacket::nw_set_nw_ecn(uint8_t ecn)
{
	if (not anchors[IPV4_FRAME].empty())
	{
		return;
	}

	ipv4().set_ipv4_ecn(ecn);
	ipv4().ipv4_calc_checksum();
}


void
cpacket::tp_set_tp_src(uint16_t port)
{
	if (not anchors[UDP_FRAME].empty())
	{
		udp().set_sport(port);
		udp().udp_calc_checksum(
				ipv4().get_ipv4_src(),
				ipv4().get_ipv4_dst(),
				ipv4().get_ipv4_proto(),
				ipv4().payloadlen());
		ipv4().ipv4_calc_checksum();
	}
	if (not anchors[TCP_FRAME].empty())
	{
		tcp().set_sport(port);
		tcp().tcp_calc_checksum(
				ipv4().get_ipv4_src(),
				ipv4().get_ipv4_dst(),
				ipv4().get_ipv4_proto(),
				ipv4().payloadlen());
		ipv4().ipv4_calc_checksum();
	}
	if (not anchors[ICMPV4_FRAME].empty())
	{
		icmpv4().set_icmp_type(port & 0xff);
		icmpv4().icmpv4_calc_checksum();
		ipv4().ipv4_calc_checksum();
	}
}


void
cpacket::tp_set_tp_dst(uint16_t port)
{
	if (not anchors[UDP_FRAME].empty())
	{
		udp().set_dport(port);
		udp().udp_calc_checksum(
				ipv4().get_ipv4_src(),
				ipv4().get_ipv4_dst(),
				ipv4().get_ipv4_proto(),
				ipv4().payloadlen());
		ipv4().ipv4_calc_checksum();
	}
	if (not anchors[TCP_FRAME].empty())
	{
		tcp().set_dport(port);
		tcp().tcp_calc_checksum(
				ipv4().get_ipv4_src(),
				ipv4().get_ipv4_dst(),
				ipv4().get_ipv4_proto(),
				ipv4().payloadlen());
		ipv4().ipv4_calc_checksum();
	}
	if (not anchors[ICMPV4_FRAME].empty())
	{
		icmpv4().set_icmp_code(port & 0xff);
		icmpv4().icmpv4_calc_checksum();
		ipv4().ipv4_calc_checksum();
	}
}


void
cpacket::copy_ttl_out()
{
	throw eNotImplemented();
}


void
cpacket::copy_ttl_in()
{
	throw eNotImplemented();
}


void
cpacket::set_mpls_label(uint32_t mpls_label)
{
	if (anchors[MPLS_FRAME].empty())
	{
		return;
	}

	mpls().set_mpls_label(mpls_label); // mpls_label in host-byte-order
}


void
cpacket::set_mpls_tc(uint8_t mpls_tc)
{
	if (anchors[MPLS_FRAME].empty())
	{
		return;
	}

	mpls().set_mpls_tc(mpls_tc);
}


void
cpacket::set_mpls_ttl(uint8_t mpls_ttl)
{
	if (anchors[MPLS_FRAME].empty())
	{
		return;
	}

	mpls().set_mpls_ttl(mpls_ttl);
}


void
cpacket::dec_mpls_ttl()
{
	if (anchors[MPLS_FRAME].empty())
	{
		return;
	}

	mpls().dec_mpls_ttl();
}


void
cpacket::push_vlan(uint16_t ethertype)
{
	try {
		uint16_t outer_vid = 0;
		uint8_t  outer_pcp = 0;

		// get default values for push actions (OF 1.1 spec section 4.9.1)
		if (not anchors[VLAN_FRAME].empty())
		{
			outer_vid = vlan().get_dl_vlan_id();
			outer_pcp = vlan().get_dl_vlan_pcp();
		}
		else
		{
			outer_vid = 0;
			outer_pcp = 0;
		}


		// insert space for a new vlan tag
		cmemory *vlan_tag = new cmemory(sizeof(struct fvlanframe::vlan_hdr_t));

		piobuf.push_back(vlan_tag);

		fvlanframe *n_vlan = new fvlanframe(
									vlan_tag->somem(),
									vlan_tag->memlen(),
									vlan_tag->memlen());

		anchors[VLAN_FRAME].push_front(n_vlan);

		/* add to piovec vlan is outer most tag immediately after ether(0) == piovec.begin() */
		std::list<fframe*>::iterator it = find_if(piovec.begin(), piovec.end(),
				std::bind2nd(std::equal_to<fframe*>(), &ether(0)));
		piovec.insert(++it, n_vlan);

		n_vlan->set_dl_type(ether().get_dl_type());
		n_vlan->set_dl_vlan_id(outer_vid);
		n_vlan->set_dl_vlan_pcp(outer_pcp);

		ether(-1).set_dl_type(ethertype);

		oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_TYPE, ethertype);
		oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_VID, outer_vid);

		plength += sizeof(struct fvlanframe::vlan_hdr_t);

		WRITELOG(CPACKET, DBG, "cpacket(%p)::push_vlan() pack: %s",
							this,
							c_str());


	} catch (eMemAllocFailed& e) {
		// TODO: log error
	}
}


void
cpacket::pop_vlan()
{
	if (anchors[VLAN_FRAME].empty())
	{
		return;
	}

	ether(-1).set_dl_type(vlan(0).get_dl_type());

	piovec.remove(anchors[VLAN_FRAME].front());

	delete anchors[VLAN_FRAME].front();

	anchors[VLAN_FRAME].pop_front();

	oxmlist.erase(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_VID);
	oxmlist.erase(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_PCP);
	oxmlist.oxm_replace_or_insert(
					OFPXMC_OPENFLOW_BASIC,
					OFPXMT_OFB_ETH_TYPE,
					ether(-1).get_dl_type());

	plength -= sizeof(struct fvlanframe::vlan_hdr_t);
}


void
cpacket::push_mpls(uint16_t ethertype)
{
	try {
		uint32_t outer_label = 0;
		uint8_t  outer_ttl = 0;
		uint8_t  outer_tc  = 0;

		// get default values for push actions (OF 1.1 spec section 4.9.1)
		if (not anchors[MPLS_FRAME].empty())
		{
			outer_label = mpls().get_mpls_label();
			outer_ttl 	= mpls().get_mpls_ttl();
			outer_tc	= mpls().get_mpls_tc();
		}
		else
		{
			outer_label = 0;
			outer_ttl	= 0;
			outer_tc	= 0;
		}


		// insert space for a new mpls tag
		cmemory *mpls_tag = new cmemory(sizeof(struct fmplsframe::mpls_hdr_t));

		piobuf.push_back(mpls_tag);

		fmplsframe *n_mpls = new fmplsframe(
									mpls_tag->somem(),
									mpls_tag->memlen(),
									mpls_tag->memlen());

		anchors[MPLS_FRAME].push_front(n_mpls); // outermost tag

		/* add to piovec: n_mpls  is outer most tag immediately after last vlan or directly after ether */
		std::list<fframe*>::iterator it = piovec.end();
		if (not anchors[VLAN_FRAME].empty())
		{
			it = find_if(piovec.begin(), piovec.end(),
				std::bind2nd(std::equal_to<fframe*>(), &vlan(-1)));
		}
		else
		{
			it = find_if(piovec.begin(), piovec.end(),
				std::bind2nd(std::equal_to<fframe*>(), &ether(-1)));
		}
		piovec.insert(++it, n_mpls);

		n_mpls->set_mpls_label(outer_label);
		n_mpls->set_mpls_ttl(outer_ttl);
		n_mpls->set_mpls_tc(outer_tc);

		// mpls after vlan or ether
		if (not anchors[VLAN_FRAME].empty())
		{
			vlan(-1).set_dl_type(ethertype);
		}
		else
		{
			ether(-1).set_dl_type(ethertype);
		}

		oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_TYPE, ethertype);
		oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_LABEL, outer_label);
		oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_TC, outer_tc);

		plength += sizeof(struct fmplsframe::mpls_hdr_t);

		WRITELOG(CPACKET, DBG, "cpacket(%p)::push_mpls() pack: %s",
							this,
							c_str());


	} catch (eMemAllocFailed& e) {
		// TODO: log error
	}
}


void
cpacket::pop_mpls(uint16_t ethertype)
{
	if (anchors[MPLS_FRAME].empty())
	{
		return;
	}

	piovec.remove(anchors[MPLS_FRAME].front());

	delete anchors[MPLS_FRAME].front();

	anchors[MPLS_FRAME].pop_front();

	// mpls after vlan or ether
	if (not anchors[VLAN_FRAME].empty())
	{
		vlan(-1).set_dl_type(ethertype);
	}
	else
	{
		ether(-1).set_dl_type(ethertype);
	}

	oxmlist.erase(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_LABEL);
	oxmlist.erase(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_TC);
	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_TYPE, ethertype);

	plength -= sizeof(struct fmplsframe::mpls_hdr_t);
}


void
cpacket::set_nw_ttl(uint8_t nw_ttl)
{
	if (anchors[IPV4_FRAME].empty())
	{
		return;
	}

	ipv4().set_ipv4_ttl(nw_ttl);
	ipv4().ipv4_calc_checksum();
}


void
cpacket::dec_nw_ttl()
{
	if (anchors[IPV4_FRAME].empty())
	{
		return;
	}

	ipv4().dec_ipv4_ttl();
	ipv4().ipv4_calc_checksum();
}


void
cpacket::push_pppoe(uint16_t ethertype)
{
	if (not anchors[PPPOE_FRAME].empty())
	{
		return;
	}

	// insert space for a new pppoe tag
	cmemory *pppoe_tag = new cmemory(sizeof(struct fpppoeframe::pppoe_hdr_t));

	piobuf.push_back(pppoe_tag);

	fpppoeframe *n_pppoe = new fpppoeframe(
								pppoe_tag->somem(),
								pppoe_tag->memlen(),
								pppoe_tag->memlen());

	anchors[PPPOE_FRAME].push_front(n_pppoe); // outermost tag (well, for pppoe there is only a single one anyway)

	/* add to piovec: n_pppoe  is single tag immediately after last vlan or directly after ether */
	std::list<fframe*>::iterator it = piovec.end();
	if (not anchors[VLAN_FRAME].empty())
	{
		it = find_if(piovec.begin(), piovec.end(),
			std::bind2nd(std::equal_to<fframe*>(), &vlan(-1))); // get last vlan tag
	}
	else
	{
		it = find_if(piovec.begin(), piovec.end(),
			std::bind2nd(std::equal_to<fframe*>(), &ether(0))); // get first ether tag
	}
	piovec.insert(++it, n_pppoe);


	n_pppoe->set_pppoe_vers(fpppoeframe::PPPOE_VERSION);
	n_pppoe->set_pppoe_type(fpppoeframe::PPPOE_TYPE);
	n_pppoe->set_pppoe_code(0);
	n_pppoe->set_pppoe_sessid(0);

	// pppoe after vlan or ether (TODO: think about pppoe within mpls?)
	if (not anchors[VLAN_FRAME].empty())
	{
		vlan(-1).set_dl_type(ethertype);
	}
	else
	{
		ether(-1).set_dl_type(ethertype);
	}

	// set PPPoE header length field (costly operation :( )
	std::list<fframe*>::iterator start = find_if(piovec.begin(), piovec.end(),
			std::bind2nd(std::equal_to<fframe*>(), n_pppoe));
	uint16_t p_len = 0;
	for (std::list<fframe*>::iterator
			it = ++start; it != piovec.end(); ++it)
	{
		p_len += (*it)->framelen();
	}

	n_pppoe->set_hdr_length(p_len);

	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_TYPE, ethertype);
#if 0
	// we do this in the appropriate set methods
	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_CODE, (uint8_t)0);
	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_TYPE, (uint8_t)fpppoeframe::PPPOE_TYPE);
	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_SID, (uint16_t)0);
#endif

	plength += sizeof(struct fpppoeframe::pppoe_hdr_t);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::push_pppoe() pack: %s",
						this,
						c_str());
}



void
cpacket::pop_pppoe(uint16_t ethertype)
{
	if (anchors[PPPOE_FRAME].empty())
	{
		return;
	}

	piovec.remove(anchors[PPPOE_FRAME].front());

	delete anchors[PPPOE_FRAME].front();

	anchors[PPPOE_FRAME].pop_front();

	// mpls after vlan or ether
	if (not anchors[VLAN_FRAME].empty())
	{
		vlan(-1).set_dl_type(ethertype);
	}
	else
	{
		ether(-1).set_dl_type(ethertype);
	}

	oxmlist.erase(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_CODE);
	oxmlist.erase(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_TYPE);
	oxmlist.erase(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_SID);

	plength -= sizeof(struct fpppoeframe::pppoe_hdr_t);
}


void
cpacket::set_pppoe_type(uint8_t type)
{
	if (anchors[PPPOE_FRAME].empty())
	{
		return;
	}

	pppoe().set_pppoe_type(type);

	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_TYPE, (uint8_t)fpppoeframe::PPPOE_TYPE);
}


void
cpacket::set_pppoe_code(uint8_t code)
{
	if (anchors[PPPOE_FRAME].empty())
	{
		return;
	}

	pppoe().set_pppoe_code(code);

	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_CODE, (uint8_t)0);
}


void
cpacket::set_pppoe_sessid(uint16_t sessid)
{
	if (anchors[PPPOE_FRAME].empty())
	{
		return;
	}

	pppoe().set_pppoe_sessid(sessid);

	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPPOE_SID, (uint16_t)0);
}



void
cpacket::push_ppp(uint16_t code)
{
	if (not anchors[PPP_FRAME].empty())
	{
		return;
	}

	// insert space for a new ppp tag
	cmemory *ppp_tag = new cmemory(sizeof(struct fpppframe::ppp_hdr_t));

	piobuf.push_back(ppp_tag);

	fpppframe *n_ppp = new fpppframe(
								ppp_tag->somem(),
								ppp_tag->memlen(),
								ppp_tag->memlen());

	anchors[PPP_FRAME].push_front(n_ppp); // outermost tag (well, for ppp there is only a single one anyway)

	/* add to piovec: n_ppp is single tag immediately after pppoe tag */
	std::list<fframe*>::iterator it = piovec.end();
	it = find_if(piovec.begin(), piovec.end(),
			std::bind2nd(std::equal_to<fframe*>(), &pppoe(0))); // get last vlan tag
	piovec.insert(++it, n_ppp);

	ppp(0).set_ppp_prot(code);

	{
		// reset PPPoE header length field (costly operation :( )
		std::list<fframe*>::iterator start = find_if(piovec.begin(), piovec.end(),
				std::bind2nd(std::equal_to<fframe*>(), n_ppp));
		uint16_t p_len = 0;
		for (std::list<fframe*>::iterator
				it = start; it != piovec.end(); ++it)
		{
			p_len += (*it)->framelen();
		}

		pppoe(0).set_hdr_length(p_len);
	}

	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPP_PROT, code);

	plength += sizeof(struct fpppframe::ppp_hdr_t);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::push_ppp() pack: %s",
						this,
						c_str());
}


void
cpacket::pop_ppp()
{
	if (anchors[PPP_FRAME].empty())
	{
		return;
	}

	piovec.remove(anchors[PPP_FRAME].front());

	delete anchors[PPP_FRAME].front();

	anchors[PPP_FRAME].pop_front();

	oxmlist.erase(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPP_PROT);

	plength -= sizeof(struct fpppframe::ppp_hdr_t);
}



void
cpacket::set_ppp_prot(uint16_t prot)
{
	if (anchors[PPP_FRAME].empty())
	{
		return;
	}

	ppp().set_ppp_prot(prot);

	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_PPP_PROT, prot);
}


void
cpacket::classify(uint32_t in_port /* host byte order */)
{
	uint8_t* p_ptr = (uint8_t*)0;
	size_t p_len = 0;
	fframe *pred = (fframe*)0;
	uint16_t dl_type = 0;

	/*
	 * initial assumption: we have a single cmemory instance stored in piobuf
	 * that contains the entire frame
	 * this frame may be fragmented later (due to push and pop operations)
	 * It is safe to run method classify() with a single consecutive cmemory
	 * buffer.
	 * It must not be called later. Therefore, classify() is declared as private method.
	 */

	WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() in_port:%d", this, in_port);

	cleanup();

	if (piobuf.empty())
	{
		return;
	}

	p_ptr 	= piobuf[0]->somem();
	p_len	= piobuf[0]->memlen();
	uint16_t total_len 	= piobuf[0]->memlen();

	RwLock lock(&ac_rwlock, RwLock::RWLOCK_WRITE);

	// initialize pseudo header: set input port, default vlan id, all others to zero
	{
		oxmlist.clear();
		oxmlist[OFPXMT_OFB_IN_PORT] 	= coxmatch_ofb_in_port(in_port);
		oxmlist[OFPXMT_OFB_IN_PHY_PORT] = coxmatch_ofb_in_phy_port(in_port);
								// TODO: get real physical port for trunks, etc.
	}

	fetherframe *ether = NULL;

	/*
	 * ethernet header
	 */
	if (true)
	{
		if (p_len < sizeof(struct fetherframe::eth_hdr_t))
		{
			return;
		}

		ether = new fetherframe(p_ptr, sizeof(struct fetherframe::eth_hdr_t), total_len);
		anchors[ETHER_FRAME].push_back(ether);
		piovec.push_back(ether);

		WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() ether:%s", this, ether->c_str());

		// initialize header: set ethernet src, dst, type
		oxmlist[OFPXMT_OFB_ETH_DST] 	= coxmatch_ofb_eth_dst(ether->get_dl_dst());
		oxmlist[OFPXMT_OFB_ETH_SRC] 	= coxmatch_ofb_eth_src(ether->get_dl_src());
		dl_type = ether->get_dl_type();


		total_len -= sizeof(struct fetherframe::eth_hdr_t);

		p_ptr += sizeof(struct fetherframe::eth_hdr_t);
		p_len -= sizeof(struct fetherframe::eth_hdr_t);

		pred = ether;

		// TODO: ether
	}

	/*
	 * vlan header(s)
	 */

	if ((fvlanframe::VLAN_ETHER == ether->get_dl_type()) ||
		(fvlanframe::QINQ_ETHER == ether->get_dl_type()))
	{
		while (true)
		{
			// sanity check
			if (p_len < sizeof(struct fvlanframe::vlan_hdr_t))
			{
				return;
			}

			fvlanframe *vlan = new fvlanframe(
										p_ptr,
										sizeof(struct fvlanframe::vlan_hdr_t),
										total_len,
										pred);

			WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() vlan:%s", this, vlan->c_str());

			anchors[VLAN_FRAME].push_back(vlan);
			piovec.push_back(vlan);

			if (anchors[VLAN_FRAME].size() == 1) // first (=outermost) tag?
			{
				oxmlist[OFPXMT_OFB_VLAN_VID] = coxmatch_ofb_vlan_vid(vlan->get_dl_vlan_id());
				oxmlist[OFPXMT_OFB_VLAN_PCP] = coxmatch_ofb_vlan_pcp(vlan->get_dl_vlan_pcp());
			}
			dl_type = vlan->get_dl_type();

			total_len -= sizeof(struct fvlanframe::vlan_hdr_t);

			p_ptr += sizeof(struct fvlanframe::vlan_hdr_t);
			p_len -= sizeof(struct fvlanframe::vlan_hdr_t);

			pred = vlan;

			// next header is again a vlan header?
			if ((fvlanframe::VLAN_ETHER != vlan->get_dl_type()) &&
				(fvlanframe::QINQ_ETHER != vlan->get_dl_type()))
			{
				break; // no => break out of this loop
			}
		}
	}

	oxmlist[OFPXMT_OFB_ETH_TYPE] = coxmatch_ofb_eth_type(dl_type); // either from ether or innermost vlan


	/*
	 * handle individual ethernet types (payloads) except vlans
	 */
	switch (dl_type) {

	/*
	 *  ethernet type == MPLS
	 */
	case fmplsframe::MPLS_ETHER:
	case fmplsframe::MPLS_ETHER_UPSTREAM:
	{
		while (true)
		{
			// sanity check
			if (p_len < sizeof(struct fmplsframe::mpls_hdr_t))
			{
				return;
			}

			fmplsframe *mpls = new fmplsframe(
										p_ptr,
										sizeof(struct fmplsframe::mpls_hdr_t),
										total_len,
										pred);

			WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() mpls:%s", this, mpls->c_str());

			anchors[MPLS_FRAME].push_back(mpls);
			piovec.push_back(mpls);

			if (anchors[MPLS_FRAME].size() == 1)
			{
				oxmlist[OFPXMT_OFB_MPLS_LABEL] 	= coxmatch_ofb_mpls_label(mpls->get_mpls_label());
				oxmlist[OFPXMT_OFB_MPLS_TC] 	= coxmatch_ofb_mpls_tc(mpls->get_mpls_tc());
			}

			total_len -= sizeof(struct fmplsframe::mpls_hdr_t);

			p_ptr += sizeof(struct fmplsframe::mpls_hdr_t);
			p_len -= sizeof(struct fmplsframe::mpls_hdr_t);

			pred = mpls;

			// bottom of stack bit is set?
			if (mpls->get_mpls_bos())
			{
				break; // yes => break out of this loop
			}
		}
		return; // end of parsing, as there is no way to determine the type of an MPLS payload
	}



	/*
	 *  ethernet type == PPPoE discovery
	 */
	case fpppoeframe::PPPOE_ETHER_DISCOVERY:
	{
		if (p_len < sizeof(struct fpppoeframe::pppoe_hdr_t))
		{
			return;
		}

		fpppoeframe *pppoe = new fpppoeframe(
								p_ptr,
								p_len, // end of parding: length is entire remaining packet (think of PPPoE tags!)
								total_len,
								pred);

		WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() pppoe:%s", this, pppoe->c_str());

		anchors[PPPOE_FRAME].push_back(pppoe);
		piovec.push_back(pppoe);

		oxmlist[OFPXMT_OFB_PPPOE_CODE] 	= coxmatch_ofb_pppoe_code(pppoe->get_pppoe_code());
		oxmlist[OFPXMT_OFB_PPPOE_TYPE] 	= coxmatch_ofb_pppoe_type(pppoe->get_pppoe_type());
		oxmlist[OFPXMT_OFB_PPPOE_SID] 	= coxmatch_ofb_pppoe_sid (pppoe->get_pppoe_sessid());

		return; // end of parsing
	}



	/*
	 *  ethernet type == PPPoE session
	 */
	case fpppoeframe::PPPOE_ETHER_SESSION:
	{
		if (p_len < sizeof(struct fpppoeframe::pppoe_hdr_t))
		{
			return;
		}

		fpppoeframe *pppoe = new fpppoeframe(
									p_ptr,
									sizeof(struct fpppoeframe::pppoe_hdr_t),
									total_len,
									pred);

		WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() pppoe:%s", this, pppoe->c_str());

		anchors[PPPOE_FRAME].push_back(pppoe);
		piovec.push_back(pppoe);

		oxmlist[OFPXMT_OFB_PPPOE_CODE] 	= coxmatch_ofb_pppoe_code(pppoe->get_pppoe_code());
		oxmlist[OFPXMT_OFB_PPPOE_TYPE] 	= coxmatch_ofb_pppoe_type(pppoe->get_pppoe_type());
		oxmlist[OFPXMT_OFB_PPPOE_SID] 	= coxmatch_ofb_pppoe_sid (pppoe->get_pppoe_sessid());

		total_len -= sizeof(struct fpppoeframe::pppoe_hdr_t);

		p_ptr += sizeof(struct fpppoeframe::pppoe_hdr_t);
		p_len -= sizeof(struct fpppoeframe::pppoe_hdr_t);

		pred = pppoe;

		/*
		 * PPP payload
		 */
		try {

			// there is pppoe, than there will be ppp as well, parse it
			if (p_len < sizeof(struct fpppframe::ppp_hdr_t))
			{
				return;
			}

			fpppframe *ppp = new fpppframe(
									p_ptr,
									sizeof(fpppframe::ppp_hdr_t),
									total_len,
									pred);

			WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() ppp:%s", this, ppp->c_str());

			anchors[PPP_FRAME].push_back(ppp);
			piovec.push_back(ppp);

			oxmlist[OFPXMT_OFB_PPP_PROT] = coxmatch_ofb_ppp_prot(ppp->get_ppp_prot());

			// IPv4 within PPP
			if (ppp->get_ppp_prot() == fpppframe::PPP_PROT_IPV4)
			{
				total_len -= sizeof(uint16_t); // PPP-PROT IPv4 is 2 bytes long

				p_ptr +=  sizeof(uint16_t);
				p_len -=  sizeof(uint16_t);

				pred = ppp;

				if (p_len < sizeof(struct fipv4frame::ipv4_hdr_t))
				{
					return;
				}

				fipv4frame *ipv4 = new fipv4frame(
											p_ptr,
											p_len, // end of parsing, ipv4 length comprises entire remaining packet
											total_len,
											pred);

				anchors[IPV4_FRAME].push_back(ipv4);
				piovec.push_back(ipv4);

				oxmlist[OFPXMT_OFB_IPV4_DST] 	= coxmatch_ofb_ipv4_dst(be32toh(ipv4->get_ipv4_dst().s4addr->sin_addr.s_addr));
				oxmlist[OFPXMT_OFB_IPV4_SRC] 	= coxmatch_ofb_ipv4_src(be32toh(ipv4->get_ipv4_src().s4addr->sin_addr.s_addr));
				oxmlist[OFPXMT_OFB_IP_PROTO] 	= coxmatch_ofb_ip_proto(ipv4->get_ipv4_proto());
				oxmlist[OFPXMT_OFB_IP_DSCP] 	= coxmatch_ofb_ip_dscp(ipv4->get_ipv4_dscp());
				oxmlist[OFPXMT_OFB_IP_ECN] 		= coxmatch_ofb_ip_ecn(ipv4->get_ipv4_ecn());

			}
			else
			{
				p_ptr +=  sizeof(uint16_t);
				p_len -=  sizeof(uint16_t);

				pred = ppp;

				piovec.push_back(new fframe(p_ptr, p_len)); // PPP-LCP, PPP-NCP, ... frames
			}

		} catch (eFrameNoPayload& e) {

		}

		return; // end of parsing, we do not allow access to IP frames within ppp (for now)
	}

	/*
	 *  ethernet type == ARPv4
	 */
	case farpv4frame::ARPV4_ETHER:
	{
		if (p_len < sizeof(struct farpv4frame::arpv4_hdr_t))
		{
			return;
		}

		farpv4frame *arpv4 = new farpv4frame(
									p_ptr,
									sizeof(struct farpv4frame::arpv4_hdr_t),
									total_len,
									pred);

		WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() arpv4:%s", this, arpv4->c_str());

		anchors[ARPV4_FRAME].push_back(arpv4);
		piovec.push_back(arpv4);

		oxmlist[OFPXMT_OFB_ARP_SPA] = coxmatch_ofb_arp_spa(be32toh(arpv4->arp_hdr->ip_src));
		oxmlist[OFPXMT_OFB_ARP_TPA] = coxmatch_ofb_arp_tpa(be32toh(arpv4->arp_hdr->ip_dst));
		oxmlist[OFPXMT_OFB_ARP_SHA] = coxmatch_ofb_arp_sha(arpv4->get_dl_src());
		oxmlist[OFPXMT_OFB_ARP_THA] = coxmatch_ofb_arp_tha(arpv4->get_dl_dst());
		oxmlist[OFPXMT_OFB_ARP_OP] 	= coxmatch_ofb_arp_op (arpv4->get_opcode());

		return; // end of parsing
	}



	/*
	 * ethernet type == IPv4
	 */
	case fipv4frame::IPV4_ETHER:
	{
		//WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() ZZZZZZZ framelen()[%d] __ether.payloadlen()[%d] ether: %s",
		//		this, framelen(), __ether->payloadlen(), __ether->c_str());

		if (p_len < sizeof(struct fipv4frame::ipv4_hdr_t))
		{
			return;
		}

		fipv4frame *ipv4 = new fipv4frame(
									p_ptr,
									sizeof(struct fipv4frame::ipv4_hdr_t),
									total_len,
									pred);

		WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() ipv4:%s", this, ipv4->c_str());

		anchors[IPV4_FRAME].push_back(ipv4);
		piovec.push_back(ipv4);

		oxmlist[OFPXMT_OFB_IPV4_DST] 	= coxmatch_ofb_ipv4_dst(be32toh(ipv4->ipv4_hdr->dst));
		oxmlist[OFPXMT_OFB_IPV4_SRC] 	= coxmatch_ofb_ipv4_src(be32toh(ipv4->ipv4_hdr->src));
		oxmlist[OFPXMT_OFB_IP_PROTO] 	= coxmatch_ofb_ip_proto(ipv4->get_ipv4_proto());
		oxmlist[OFPXMT_OFB_IP_DSCP] 	= coxmatch_ofb_ip_dscp (ipv4->get_ipv4_dscp());
		oxmlist[OFPXMT_OFB_IP_ECN] 		= coxmatch_ofb_ip_ecn  (ipv4->get_ipv4_ecn());

		if (ipv4->has_MF_bit_set())
		{
			WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() IPv4 fragment found", this);

			return;
		}

		// FIXME: IP header with options
		total_len -= sizeof(struct fipv4frame::ipv4_hdr_t);

		p_ptr += sizeof(struct fipv4frame::ipv4_hdr_t);
		p_len -= sizeof(struct fipv4frame::ipv4_hdr_t);

		pred = ipv4;

		/* transport protocols
		 *
		 */
		switch (ipv4->ipv4_hdr->proto) {

		/*
		 *  UDP
		 */
		case fudpframe::UDP_IP_PROTO:
		{
			if (p_len < sizeof(struct fudpframe::udp_hdr_t))
			{
				return;
			}

			fudpframe *udp = new fudpframe(
									p_ptr,
									p_len, // end of parsing: udp contains header and payload
									total_len,
									pred);

			WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() udp:%s", this, udp->c_str());

			anchors[UDP_FRAME].push_back(udp);
			piovec.push_back(udp);

			oxmlist[OFPXMT_OFB_UDP_DST] = coxmatch_ofb_udp_dst(udp->get_dport());
			oxmlist[OFPXMT_OFB_UDP_SRC] = coxmatch_ofb_udp_src(udp->get_sport());

			return;
		}

		/*
		 *  TCP
		 */
		case ftcpframe::TCP_IP_PROTO:
		{
			if (p_len < sizeof(struct ftcpframe::tcp_hdr_t))
			{
				return;
			}

			ftcpframe *tcp = new ftcpframe(
									p_ptr,
									p_len, // end of parsing: tcp contains header and payload
									total_len,
									pred);

			WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() tcp:%s", this, tcp->c_str());

			anchors[TCP_FRAME].push_back(tcp);
			piovec.push_back(tcp);

			oxmlist[OFPXMT_OFB_TCP_DST] = coxmatch_ofb_tcp_dst(tcp->get_dport());
			oxmlist[OFPXMT_OFB_TCP_SRC] = coxmatch_ofb_tcp_src(tcp->get_sport());

			return;
		}

		/*
		 *  ICMPv4
		 */
		case ficmpv4frame::ICMPV4_IP_PROTO:
		{
			if (p_len < sizeof(struct ficmpv4frame::icmpv4_hdr_t))
			{
				return;
			}

			ficmpv4frame *icmpv4 = new ficmpv4frame(
										p_ptr,
										p_len, // end of parsing: tcp contains header and payload
										total_len,
										pred);

			WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() icmpv4:%s", this, icmpv4->c_str());

			anchors[ICMPV4_FRAME].push_back(icmpv4);
			piovec.push_back(icmpv4);

			oxmlist[OFPXMT_OFB_ICMPV4_TYPE] = coxmatch_ofb_icmpv4_type(icmpv4->get_icmp_type());
			oxmlist[OFPXMT_OFB_ICMPV4_CODE] = coxmatch_ofb_icmpv4_code(icmpv4->get_icmp_code());

			return;
		}

		default:
			WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() unknown ip proto [%d]",
					this, ipv4->ipv4_hdr->proto);
			break;
		} // end transport protocols

		break;
	} // end IPv4

	default:
		WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() unknown ethernet type [%d] %s",
				this, dl_type, this->ether().c_str());

		break;
	} // end ethernet types
}


void
cpacket::cleanup()
{
	for (unsigned int i = 0; i < anchors.size(); ++i)
	{
		anchors[i].clear();
#if 0
		for (std::deque<fframe*>::iterator
				it = anchors[i].begin(); it != anchors[i].end(); ++it)
		{
			delete (*it);
		}
		anchors[i].clear();
#endif
	}

	for (std::list<fframe*>::iterator
			it = piovec.begin(); it != piovec.end(); ++it)
	{
		delete (*it);
	}
	piovec.clear();
}


void
cpacket::calc_checksums()
{
	RwLock lock(&ac_rwlock, RwLock::RWLOCK_READ);

	if (flags.test(FLAG_TCP_CHECKSUM))
	{
		if ((not anchors[IPV4_FRAME].empty()) and
			(not anchors[TCP_FRAME].empty()))
		{
			tcp().tcp_calc_checksum(
				ipv4().get_ipv4_src(),
				ipv4().get_ipv4_dst(),
				ipv4().get_ipv4_proto(),
				ipv4().payloadlen());
		}
	}

	if (flags.test(FLAG_UDP_CHECKSUM))
	{
		if ((not anchors[IPV4_FRAME].empty()) and
			(not anchors[UDP_FRAME].empty()))
		{
			udp().udp_calc_checksum(
				ipv4().get_ipv4_src(),
				ipv4().get_ipv4_dst(),
				ipv4().get_ipv4_proto(),
				ipv4().payloadlen());
		}
	}

	if (flags.test(FLAG_IPV4_CHECKSUM))
	{
		if (not anchors[IPV4_FRAME].empty())
		{
			ipv4().ipv4_calc_checksum();
		}
	}

	if (flags.test(FLAG_ICMPV4_CHECKSUM))
	{
		if (not anchors[ICMPV4_FRAME].empty())
		{
			icmpv4().icmpv4_calc_checksum();
		}
	}

	if (flags.test(FLAG_PPPOE_LENGTH))
	{
		if (not anchors[PPPOE_FRAME].empty())
		{
			pppoe().pppoe_calc_length();
		}
	}

}


void
cpacket::calc_hits(
		cofmatch& ofmatch,
		uint16_t& exact_hits,
		uint16_t& wildcard_hits,
		uint16_t& missed)
{
	oxmlist.calc_hits(ofmatch.oxmlist, exact_hits, wildcard_hits, missed);
}





#if 0
cpacket::cofpseudohdr::cofpseudohdr()
{
	reset();
}


void
cpacket::cofpseudohdr::reset()
{
	fields 			= 0;
	in_port 		= 0;
	dl_dst 			= cmacaddr("00:00:00:00:00:00");
	dl_src 			= cmacaddr("00:00:00:00:00:00");
	dl_type			= 0;
	dl_vlan_id		= 0;
	dl_vlan_pcp		= 0;
	mpls_label		= 0;
	mpls_tc			= 0;
	nw_tos			= 0;
	nw_proto		= 0;
	nw_src			= 0;
	nw_dst			= 0;
	tp_src			= 0;
	tp_dst			= 0;
	metadata		= 0;
	pppoe_type		= 0;
	pppoe_code		= 0;
	pppoe_sessid	= 0;
	ppp_prot		= 0;
}


const char*
cpacket::cofpseudohdr::c_str()
{
	cvastring vas(2048);

	info.clear();

	info.append(vas(
			 "classifier(%p) "
			 "\tclassifier.inport : port=0x%x \n"
			 "\tether          : src=%02x:%02x:%02x:%02x:%02x:%02x => dst=%02x:%02x:%02x:%02x:%02x:%02x type=0x%04x \n",
			 this,
			 in_port,
			 dl_src[0],
			 dl_src[1],
			 dl_src[2],
			 dl_src[3],
			 dl_src[4],
			 dl_src[5],
			 dl_dst[0],
			 dl_dst[1],
			 dl_dst[2],
			 dl_dst[3],
			 dl_dst[4],
			 dl_dst[5],
			 dl_type));

	// VLAN
	{
		info.append(vas(
			 "\tvlan           : vid=0x%x(%d) pcp=%d \n",
			 dl_vlan_id,
			 dl_vlan_id,
			 dl_vlan_pcp));
	}

	// MPLS
	{
		info.append(vas(
			 "\tmpls           : label=0x%x tc=%d \n",
			 mpls_label,
			 mpls_tc));
	}

	// PPPoE
	{
		info.append(vas(
			"\tpppoe          : type=%d code=0x%x sessid=0x%x \n",
			pppoe_type,
			pppoe_code,
			pppoe_sessid));
	}

	// PPP
	{
		info.append(vas(
			"\tppp            : prot=0x%04x \n",
			ppp_prot));
	}

	// ARPv4
	{
		info.append(vas(
			"\tarpv4          : opcode=%d %d.%d.%d.%d => %d.%d.%d.%d \n",
			nw_proto,
			((nw_src & 0xff000000) >> 24),
			((nw_src & 0x00ff0000) >> 16),
			((nw_src & 0x0000ff00) >>  8),
			((nw_src & 0x000000ff) >>  0),
			((nw_dst & 0xff000000) >> 24),
			((nw_dst & 0x00ff0000) >> 16),
			((nw_dst & 0x0000ff00) >>  8),
			((nw_dst & 0x000000ff) >>  0)));
	}

	// IPv4
	{
		info.append(vas(
			"\tipv4           : tos=%d proto=%d %d.%d.%d.%d => %d.%d.%d.%d \n",
			(nw_tos & 0xfc),
			nw_proto,
			((nw_src & 0xff000000) >> 24),
			((nw_src & 0x00ff0000) >> 16),
			((nw_src & 0x0000ff00) >>  8),
			((nw_src & 0x000000ff) >>  0),
			((nw_dst & 0xff000000) >> 24),
			((nw_dst & 0x00ff0000) >> 16),
			((nw_dst & 0x0000ff00) >>  8),
			((nw_dst & 0x000000ff) >>  0)));
	}

	// ICMPv4
	{
		info.append(vas(
			"\ticmpv4         : type=%d code=%d \n",
			tp_src,
			tp_dst));
	}

	// UDP
	{
		info.append(vas(
			"\tudp            : sport=0x%x(%d) dport=0x%x(%d) \n",
			tp_src,
			tp_src,
			tp_dst,
			tp_dst));
	}

	// TCP
	{
		info.append(vas(
			"\ttcp            : sport=0x%x(%d) dport=0x%x(%d) \n",
			tp_src,
			tp_src,
			tp_dst,
			tp_dst));
	}

	return info.c_str();
}
#endif




/*
 * action related methods
 */


void
cpacket::handle_action(
		cofaction& action)
{
	switch (action.get_type()) {
	// processing actions on cpkbuf instance
	case OFPAT_SET_FIELD:
		action_set_field(action);
		break;
	case OFPAT_COPY_TTL_OUT:
		action_copy_ttl_out(action);
		break;
	case OFPAT_COPY_TTL_IN:
		action_copy_ttl_in(action);
		break;
	case OFPAT_SET_MPLS_TTL:
		action_set_mpls_ttl(action);
		break;
	case OFPAT_DEC_MPLS_TTL:
		action_dec_mpls_ttl(action);
		break;
	case OFPAT_PUSH_VLAN:
		action_push_vlan(action);
		break;
	case OFPAT_POP_VLAN:
		action_pop_vlan(action);
		break;
	case OFPAT_PUSH_MPLS:
		action_push_mpls(action);
		break;
	case OFPAT_POP_MPLS:
		action_pop_mpls(action);
		break;
	case OFPAT_SET_NW_TTL:
		action_set_nw_ttl(action);
		break;
	case OFPAT_DEC_NW_TTL:
		action_dec_nw_ttl(action);
		break;
	case OFPAT_PUSH_PPPOE:
		action_push_pppoe(action);
		break;
	case OFPAT_POP_PPPOE:
		action_pop_pppoe(action);
		break;
	case OFPAT_PUSH_PPP:
		action_push_ppp(action);
		break;
	case OFPAT_POP_PPP:
		action_pop_ppp(action);
		break;
	default:
		WRITELOG(CPACKET, ERROR, "cpacket(%p)::handle_action() unknown action type %d",
				this, action.get_type());
		break;
	}
}


void
cpacket::action_set_field(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_set_field() [1] pack: %s",
				this,
				c_str());

	set_field(action.get_oxm());

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_set_field() [2] pack: %s",
				this,
				c_str());
}


void
cpacket::action_copy_ttl_out(
		cofaction& action)
{
	copy_ttl_out();

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_copy_ttl_out() ", this);
}


void
cpacket::action_copy_ttl_in(
		cofaction& action)
{
	copy_ttl_in();

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_copy_ttl_in() ", this);
}


void
cpacket::action_set_mpls_ttl(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_set_mpls_ttl() "
			"set to mpls ttl [%d] [1] pack: %s", this, action.oac_mpls_ttl->mpls_ttl, c_str());

	set_mpls_ttl(action.oac_mpls_ttl->mpls_ttl);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_set_mpls_ttl() "
			"set to mpls ttl [%d] [2] pack: %s", this, action.oac_mpls_ttl->mpls_ttl, c_str());
}


void
cpacket::action_dec_mpls_ttl(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_dec_mpls_ttl() [1] pack: %s",
				this, c_str());

	dec_mpls_ttl();

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_dec_mpls_ttl() [2] pack: %s",
				this, c_str());
}


void
cpacket::action_push_vlan(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_push_vlan() "
				 "set to vlan [%d] [1] pack: %s", this, be16toh(action.oac_push->ethertype), c_str());

	push_vlan(be16toh(action.oac_push->ethertype));

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_push_vlan() "
			 	 "set to vlan [%d] [2] pack: %s", this, be16toh(action.oac_push->ethertype), c_str());
}


void
cpacket::action_pop_vlan(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_pop_vlan() [1] pack: %s", this, c_str());

	pop_vlan();

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_pop_vlan() [2] pack: %s", this, c_str());
}


void
cpacket::action_push_mpls(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_push_mpls() "
			"set to mpls [%d] [1] pack: %s", this, be16toh(action.oac_push->ethertype), c_str());

	push_mpls(be16toh(action.oac_push->ethertype));

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_push_mpls() "
			"set to mpls [%d] [2] pack: %s", this, be16toh(action.oac_push->ethertype), c_str());
}


void
cpacket::action_pop_mpls(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_pop_mpls() [1] pack: %s", this, c_str());

	pop_mpls(be16toh(action.oac_pop_mpls->ethertype));

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_pop_mpls() [2] pack: %s", this, c_str());
}


void
cpacket::action_set_nw_ttl(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_set_nw_ttl() [1] "
				 "set nw-ttl [%d] pack: %s", this, action.oac_nw_ttl->nw_ttl, c_str());

	set_nw_ttl(action.oac_nw_ttl->nw_ttl);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_set_nw_ttl() [2] "
			 	 "set tnw-ttl [%d] pack: %s", this, action.oac_nw_ttl->nw_ttl, c_str());
}


void
cpacket::action_dec_nw_ttl(
		cofaction& action)
{
	dec_nw_ttl();

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_dec_nw_ttl() ", this);
}


void
cpacket::action_push_pppoe(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_push_pppoe() "
			"ethertype [0x%x] [1] pack: %s",
			this, be16toh(action.oac_push_pppoe->ethertype), c_str());

	push_pppoe(be16toh(action.oac_push_pppoe->ethertype));

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_push_pppoe() "
			"ethertype [0x%x] [2] pack: %s",
			this, be16toh(action.oac_push_pppoe->ethertype), c_str());
}


void
cpacket::action_pop_pppoe(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_pop_pppoe() "
			"ethertype [%d] [1] pack: %s",
			this, be16toh(action.oac_pop_pppoe->ethertype), c_str());

	pop_pppoe(be16toh(action.oac_pop_pppoe->ethertype));

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_pop_pppoe() "
			"ethertype [%d] [2] pack: %s",
			this, be16toh(action.oac_pop_pppoe->ethertype), c_str());
}


void
cpacket::action_push_ppp(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_push_ppp() "
			"set to ppp [1] pack: %s", this, c_str());

	uint16_t code = 0;

	push_ppp(code);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_push_ppp() "
			"set to ppp [2] pack: %s", this, c_str());
}


void
cpacket::action_pop_ppp(
		cofaction& action)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_pop_ppp() "
			"[1] pack: %s", this, c_str());

	pop_ppp();

	WRITELOG(CPACKET, DBG, "cpacket(%p)::action_pop_ppp() "
			"[2] pack: %s", this, c_str());
}
