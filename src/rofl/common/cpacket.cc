/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cpacket.h"

using namespace rofl;

bool				cpacket::cpacket_init = false;
std::string			cpacket::s_cpacket_info;
pthread_rwlock_t 	cpacket::cpacket_lock;
std::set<cpacket*> 	cpacket::cpacket_list;





void
cpacket::cpacket_list_insert()
{
	return;
	if (not cpacket::cpacket_init)
	{
		pthread_rwlock_init(&cpacket::cpacket_lock, 0);
	}
	RwLock(&cpacket::cpacket_lock, RwLock::RWLOCK_WRITE);
	cpacket::cpacket_list.insert(this);
}


void
cpacket::cpacket_list_erase()
{
	return;
	{
		RwLock(&cpacket::cpacket_lock, RwLock::RWLOCK_WRITE);
		cpacket::cpacket_list.erase(this);
	}
}


cpacket::cpacket(
		uint8_t ofp_version,
		size_t size,
		uint32_t in_port,
		bool do_classify) :
				ofp_version(ofp_version),
				total_len(size),
				head(0),
				tail(0),
				hspace(CPACKET_DEFAULT_HSPACE),
				tspace(CPACKET_DEFAULT_TSPACE),
				mem(size + hspace + tspace),
				data(std::pair<uint8_t*, size_t>(mem.somem() + hspace, size)),
				match(ofp_version),
				packet_receive_time(time(NULL)),
				in_port(in_port)
{
	pthread_rwlock_init(&ac_rwlock, NULL);

	match.set_in_port(in_port);
	if (ofp_version >= rofl::openflow12::OFP_VERSION) {
		match.set_in_phy_port(in_port);
	}

	if (do_classify) {
		classify(in_port);
	}

	cpacket_list_insert();
}



cpacket::cpacket(
		uint8_t ofp_version,
		cmemory *mem,
		uint32_t in_port,
		bool do_classify) :
				ofp_version(ofp_version),
				total_len(mem->memlen()),
				head(0),
				tail(0),
				hspace(CPACKET_DEFAULT_HSPACE),
				tspace(0),
				mem(mem->memlen() + hspace + tspace),
				data(std::pair<uint8_t*, size_t>(this->mem.somem() + hspace, mem->memlen())),
				match(ofp_version),
				packet_receive_time(time(NULL)),
				in_port(in_port)
{
	memcpy(soframe(), mem->somem(), mem->memlen());

	pthread_rwlock_init(&ac_rwlock, NULL);

	match.set_in_port(in_port);
	match.set_in_phy_port(in_port);

	if (do_classify) {
		classify(in_port);
	}

	cpacket_list_insert();
}



cpacket::cpacket(
		uint8_t ofp_version,
		uint8_t *buf,
		size_t buflen,
		uint32_t in_port,
		bool do_classify) :
				ofp_version(ofp_version),
				total_len(buflen),
				head(0),
				tail(0),
				hspace(CPACKET_DEFAULT_HSPACE),
				tspace(0),
				mem(buflen + hspace + tspace),
				//mem(buf, buflen, CPACKET_HEAD_ROOM, CPACKET_TAIL_ROOM),
				data(std::pair<uint8_t*, size_t>(mem.somem() + hspace, buflen)),
				match(ofp_version),
				packet_receive_time(time(NULL)),
				in_port(in_port)
{
	pthread_rwlock_init(&ac_rwlock, NULL);

	memcpy(soframe(), buf, buflen);

	match.set_in_port(in_port);
	if (ofp_version >= rofl::openflow12::OFP_VERSION) {
		match.set_in_phy_port(in_port);
	}

	if (do_classify) {
		classify(in_port);
	}

	cpacket_list_insert();
}



cpacket::cpacket(
		cpacket const& pack) :
				ofp_version(rofl::openflow::OFP_VERSION_UNKNOWN),
				total_len(0),
				head(0),
				tail(0),
				hspace(CPACKET_DEFAULT_HSPACE),
				tspace(0),
				mem(pack.framelen() + hspace),
				//mem(buf, buflen, CPACKET_HEAD_ROOM, CPACKET_TAIL_ROOM),
				data(std::pair<uint8_t*, size_t>(mem.somem() + hspace, pack.framelen())),
				match(ofp_version),
				packet_receive_time(time(NULL)),
				in_port(0)
{
	pthread_rwlock_init(&ac_rwlock, NULL);

	*this = pack;

	cpacket_list_insert();
}



cpacket::~cpacket()
{
	reset(); // removes all cmemory and ciovec instances from heap

	pthread_rwlock_destroy(&ac_rwlock);

	cpacket_list_erase();
}




void
cpacket::reset()
{
	flags.reset(FLAG_VLAN_PRESENT);
	flags.reset(FLAG_MPLS_PRESENT);

	match.clear();

	while (head != 0) {
		fframe *curr = head;
		head = curr->next;
		curr->next = curr->prev = 0;
		delete curr;
	}

	head = tail = 0;
}



void
cpacket::clear()
{
	reset();
	mem.clear();
}



void
cpacket::mem_resize(
		size_t size)
{
	mem.resize(size + hspace + tspace);
	data.first 	= mem.somem() + hspace;
	data.second = size;
}



cpacket&
cpacket::operator=(
		cpacket const& p)
{
	if (this == &p)
		return *this;

	reset();

	ofp_version 		= p.ofp_version;
	head = tail 		= 0;
	hspace = tspace 	= 0;

	total_len			= p.total_len;
	match				= p.match;
	flags				= p.flags;

	mem_resize(p.framelen());
	memcpy(soframe(), p.soframe(), p.framelen());
	packet_receive_time = p.packet_receive_time;

	//classify(match.get_in_port());

	return *this;
}


uint8_t&
cpacket::operator[] (size_t index)
{
	return mem[index];
}



bool
cpacket::operator== (
		cpacket const& p)
{
	if (data.second == p.data.second) {
		return (!memcmp(data.first, p.data.first, data.second));
	}
	return false;
}



bool
cpacket::operator== (
		cmemory const& m)
{
	if (data.second == m.memlen()) {
		return (!memcmp(data.first, m.somem(), data.second));
	}
	return false;
}



bool
cpacket::operator!= (
		cpacket const& p)
{
	return not operator== (p);
}



bool
cpacket::operator!= (
		cmemory const& m)
{
	return not operator== (m);
}



void
cpacket::operator+= (
		fframe const& f)
{
	*this = *this + f;
}



cpacket&
cpacket::operator+ (
		fframe const& f)
{
	size_t tspace = mem.memlen() - (hspace + framelen());

	size_t len = framelen();

	if (f.framelen() > tspace) { // requiring resize
		mem_resize(framelen() + f.framelen());
		// mem_resize() sets framelen() to old-framelen() + f.framelen()
	} else {
		data.second = len + f.framelen();
	}

	memcpy(soframe() + len, f.soframe(), f.framelen());

	classify(in_port);

	return *this;
}



void
cpacket::set_flag_no_packet_in(bool no_packet_in)
{
	if (no_packet_in)
		flags.set(FLAG_NO_PACKET_IN);
	else
		flags.reset(FLAG_NO_PACKET_IN);
}



bool
cpacket::get_flag_no_packet_in()
{
	return flags.test(FLAG_NO_PACKET_IN);
}



uint8_t*
cpacket::soframe() const
{
	return data.first;
}



size_t
cpacket::framelen() const
{
	return data.second;
}



bool
cpacket::empty() const
{
	return (0 == framelen());
}



void
cpacket::pack(
		uint8_t *dest,
		size_t len)
{
	size_t p_len = (len < framelen()) ? len : framelen();
	memcpy(dest, soframe(), p_len);
}



void
cpacket::unpack(
		uint32_t in_port,
		uint8_t *src,
		size_t len)
{
	if (len > framelen())
	{
		mem_resize(len);
	}
	memcpy(soframe(), src, len);
	data.first = soframe();
	data.second = len;
	classify(in_port);
}




size_t
cpacket::length()
{
	return framelen();
}



uint8_t*
cpacket::tag_insert(
		size_t len) throw (ePacketOutOfRange)
{
	size_t space = soframe() - mem.somem(); // remaining head space

	if (len > space)
	{
		throw ePacketOutOfRange();
	}

	memmove(soframe() - len, soframe(), sizeof(struct fetherframe::eth_hdr_t));
	memset(soframe() + sizeof(struct fetherframe::eth_hdr_t) - len, 0x00, len);

	data.first  -= len;
	data.second += len;

	ether()->reset(data.first, sizeof(struct fetherframe::eth_hdr_t));

	return (soframe() + sizeof(struct fetherframe::eth_hdr_t));
}



void
cpacket::tag_remove(
		fframe *frame) throw (ePacketOutOfRange)
{
	memmove(soframe() + frame->framelen(), soframe(), sizeof(struct fetherframe::eth_hdr_t));

	data.first 	+= frame->framelen();
	data.second -= frame->framelen();

	ether()->reset(data.first, sizeof(struct fetherframe::eth_hdr_t));
}




size_t
cpacket::get_payload_len(fframe *from, fframe *to)
{
	fframe *curr = (from != 0) ? from : head;
	fframe *last = (to != 0) ? to->next : tail;

	size_t len = 0;

	while ((curr != 0) && (curr != last)) {
		len += curr->framelen();
		curr = curr->next;
	}
	return len;
}



void
cpacket::set_total_len(uint16_t total_len)
{
	this->total_len = total_len;
}



size_t
cpacket::get_total_len()
{
	return total_len;
}




fframe*
cpacket::frame(
		int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		return (fframe*)0;
	}


	if (i >= 0)
	{
		fframe *curr = head;

		for (int j = 0; j < (i + 1); j++)
		{
			if (j == i)
			{
				return curr;
			}

			if (0 == curr->next)
			{
				throw ePacketNotFound();
			}
			else
			{
				curr = curr->next;
			}
		}
	}
	else
	{
		fframe *curr = tail;

		for (int j = 1; j < (abs(i) + 1); j++)
		{
			if (j == abs(i))
			{
				return curr;
			}

			if (0 == curr->prev)
			{
				throw ePacketNotFound();
			}
			else
			{
				curr = curr->prev;
			}
		}
	}
	return (fframe*)0;
}



fetherframe*
cpacket::ether(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	if (i >= 0)
	{
		fframe *frame = (fframe*)head;

		while (true)
		{
			if (dynamic_cast<fetherframe*>( frame ))
			{
				return (dynamic_cast<fetherframe*>( frame ));
			}
			else
			{
				if (0 == frame->next)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->next;
				}
			}
		}
	}
	else
	{
		fframe *frame = (fframe*)tail;

		while (true)
		{
			if (dynamic_cast<fetherframe*>( frame ))
			{
				return (dynamic_cast<fetherframe*>( frame ));
			}
			else
			{
				if (0 == frame->prev)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->prev;
				}
			}
		}
	}

	throw ePacketNotFound();
}


fvlanframe*
cpacket::vlan(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	i = (i < 0) ? cnt_vlan_tags() + i : i;


	fframe *vlan0 = head;

	while (0 == dynamic_cast<fvlanframe*>( vlan0 ))
	{
		if (0 == vlan0->next)
		{
			throw ePacketNotFound();
		}
		vlan0 = vlan0->next;
	}

	for (int j = 0; j < i; j++)
	{
		if (0 == vlan0->next)
		{
			throw ePacketNotFound();
		}

		vlan0 = vlan0->next;

		if (0 == dynamic_cast<fvlanframe*>( vlan0 ))
		{
			throw ePacketNotFound();
		}
	}

	return dynamic_cast<fvlanframe*>( vlan0 );
}





fmplsframe*
cpacket::mpls(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	i = (i < 0) ? cnt_mpls_tags() + i : i;


	fframe *mpls0 = head;

	while (0 == dynamic_cast<fmplsframe*>( mpls0 ))
	{
		if (0 == mpls0->next)
		{
			throw ePacketNotFound();
		}
		mpls0 = mpls0->next;
	}

	for (int j = 0; j < i; j++)
	{
		if (0 == mpls0->next)
		{
			throw ePacketNotFound();
		}

		mpls0 = mpls0->next;

		if (0 == dynamic_cast<fmplsframe*>( mpls0 ))
		{
			throw ePacketNotFound();
		}
	}

	return dynamic_cast<fmplsframe*>( mpls0 );
}



farpv4frame*
cpacket::arpv4(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	if (i >= 0)
	{
		fframe *frame = (fframe*)head;

		while (true)
		{
			if (dynamic_cast<farpv4frame*>( frame ))
			{
				return (dynamic_cast<farpv4frame*>( frame ));
			}
			else
			{
				if (0 == frame->next)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->next;
				}
			}
		}
	}
	else
	{
		fframe *frame = (fframe*)tail;

		while (true)
		{
			if (dynamic_cast<farpv4frame*>( frame ))
			{
				return (dynamic_cast<farpv4frame*>( frame ));
			}
			else
			{
				if (0 == frame->prev)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->prev;
				}
			}
		}
	}

	throw ePacketNotFound();
}


fipv4frame*
cpacket::ipv4(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	if (i >= 0)
	{
		fframe *frame = (fframe*)head;

		while (true)
		{
			if (dynamic_cast<fipv4frame*>( frame ))
			{
				return (dynamic_cast<fipv4frame*>( frame ));
			}
			else
			{
				if (0 == frame->next)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->next;
				}
			}
		}
	}
	else
	{
		fframe *frame = (fframe*)tail;

		while (true)
		{
			if (dynamic_cast<fipv4frame*>( frame ))
			{
				return (dynamic_cast<fipv4frame*>( frame ));
			}
			else
			{
				if (0 == frame->prev)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->prev;
				}
			}
		}
	}

	throw ePacketNotFound();
}



ficmpv4frame*
cpacket::icmpv4(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	if (i >= 0)
	{
		fframe *frame = (fframe*)head;

		while (true)
		{
			if (dynamic_cast<ficmpv4frame*>( frame ))
			{
				return (dynamic_cast<ficmpv4frame*>( frame ));
			}
			else
			{
				if (0 == frame->next)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->next;
				}
			}
		}
	}
	else
	{
		fframe *frame = (fframe*)tail;

		while (true)
		{
			if (dynamic_cast<ficmpv4frame*>( frame ))
			{
				return (dynamic_cast<ficmpv4frame*>( frame ));
			}
			else
			{
				if (0 == frame->prev)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->prev;
				}
			}
		}
	}

	throw ePacketNotFound();
}



fipv6frame*
cpacket::ipv6(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	if (i >= 0)
	{
		fframe *frame = (fframe*)head;

		while (true)
		{
			if (dynamic_cast<fipv6frame*>( frame ))
			{
				return (dynamic_cast<fipv6frame*>( frame ));
			}
			else
			{
				if (0 == frame->next)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->next;
				}
			}
		}
	}
	else
	{
		fframe *frame = (fframe*)tail;

		while (true)
		{
			if (dynamic_cast<fipv6frame*>( frame ))
			{
				return (dynamic_cast<fipv6frame*>( frame ));
			}
			else
			{
				if (0 == frame->prev)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->prev;
				}
			}
		}
	}

	throw ePacketNotFound();
}



ficmpv6frame*
cpacket::icmpv6(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	if (i >= 0)
	{
		fframe *frame = (fframe*)head;

		while (true)
		{
			if (dynamic_cast<ficmpv6frame*>( frame ))
			{
				return (dynamic_cast<ficmpv6frame*>( frame ));
			}
			else
			{
				if (0 == frame->next)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->next;
				}
			}
		}
	}
	else
	{
		fframe *frame = (fframe*)tail;

		while (true)
		{
			if (dynamic_cast<ficmpv6frame*>( frame ))
			{
				return (dynamic_cast<ficmpv6frame*>( frame ));
			}
			else
			{
				if (0 == frame->prev)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->prev;
				}
			}
		}
	}

	throw ePacketNotFound();
}



fudpframe*
cpacket::udp(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	if (i >= 0)
	{
		fframe *frame = (fframe*)head;

		while (true)
		{
			if (dynamic_cast<fudpframe*>( frame ))
			{
				return (dynamic_cast<fudpframe*>( frame ));
			}
			else
			{
				if (0 == frame->next)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->next;
				}
			}
		}
	}
	else
	{
		fframe *frame = (fframe*)tail;

		while (true)
		{
			if (dynamic_cast<fudpframe*>( frame ))
			{
				return (dynamic_cast<fudpframe*>( frame ));
			}
			else
			{
				if (0 == frame->prev)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->prev;
				}
			}
		}
	}

	throw ePacketNotFound();
}



ftcpframe*
cpacket::tcp(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	if (i >= 0)
	{
		fframe *frame = (fframe*)head;

		while (true)
		{
			if (dynamic_cast<ftcpframe*>( frame ))
			{
				return (dynamic_cast<ftcpframe*>( frame ));
			}
			else
			{
				if (0 == frame->next)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->next;
				}
			}
		}
	}
	else
	{
		fframe *frame = (fframe*)tail;

		while (true)
		{
			if (dynamic_cast<ftcpframe*>( frame ))
			{
				return (dynamic_cast<ftcpframe*>( frame ));
			}
			else
			{
				if (0 == frame->prev)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->prev;
				}
			}
		}
	}

	throw ePacketNotFound();
}



fsctpframe*
cpacket::sctp(int i) throw (ePacketNotFound)
{
	if ((0 == head) || (0 == tail))
	{
		throw ePacketNotFound();
	}


	if (i >= 0)
	{
		fframe *frame = (fframe*)head;

		while (true)
		{
			if (dynamic_cast<fsctpframe*>( frame ))
			{
				return (dynamic_cast<fsctpframe*>( frame ));
			}
			else
			{
				if (0 == frame->next)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->next;
				}
			}
		}
	}
	else
	{
		fframe *frame = (fframe*)tail;

		while (true)
		{
			if (dynamic_cast<fsctpframe*>( frame ))
			{
				return (dynamic_cast<fsctpframe*>( frame ));
			}
			else
			{
				if (0 == frame->prev)
				{
					throw ePacketNotFound();
				}
				else
				{
					frame = frame->prev;
				}
			}
		}
	}

	throw ePacketNotFound();
}






fframe*
cpacket::payload() throw (ePacketNotFound)
{
	/*
	 * only the tail element can be a payload
	 * a payload is always of type fframe, not a derived class
	 */
	if (0 == tail)
	{
		throw ePacketNotFound();
	}

	if (typeid(*tail) == typeid(fframe))
	{
		return tail;
	}

	throw ePacketNotFound();
}



void
cpacket::set_field(coxmatch const& oxm)
{
	switch (oxm.get_oxm_class()) {
	case openflow::OFPXMC_OPENFLOW_BASIC: {
		set_field_basic_class(oxm);
	} break;
	case openflow::OFPXMC_EXPERIMENTER: {
		set_field_experimenter_class(oxm);
	} break;
	default: {
		logging::warn << "unable to handle OXM class " << (int)oxm.get_oxm_class() << ", ignoring command" << std::endl;
	} break;
	}
}



void
cpacket::set_field_basic_class(coxmatch const& oxm)
{
	switch (oxm.get_oxm_field()) {
	case openflow::OFPXMT_OFB_ETH_DST: {
		cmacaddr maddr(oxm.u48addr());
		ether()->set_dl_dst(maddr);
		match.set_eth_dst(maddr);
	} break;
	case openflow::OFPXMT_OFB_ETH_SRC: {
		cmacaddr maddr(oxm.u48addr());
		ether()->set_dl_src(maddr);
		match.set_eth_src(maddr);
	} break;
	case openflow::OFPXMT_OFB_ETH_TYPE: {
		uint16_t eth_type = oxm.u16value();
		ether()->set_dl_type(eth_type);
		match.set_eth_type(eth_type);
	} break;
	case openflow::OFPXMT_OFB_VLAN_VID: {
		uint16_t vid = oxm.u16value();
		if (vid & openflow::OFPVID_PRESENT) {
			match.set_vlan_present();
		} else if (vid == openflow::OFPVID_NONE) {
			match.set_vlan_untagged();
		} else {
			vlan()->set_dl_vlan_id(vid);
			match.set_vlan_vid(vid);
		}
	} break;
	case openflow::OFPXMT_OFB_VLAN_PCP: {
		uint8_t pcp = oxm.u8value();
		vlan()->set_dl_vlan_pcp(pcp);
		match.set_vlan_pcp(pcp);
	} break;
	case openflow::OFPXMT_OFB_IP_DSCP: {
		uint8_t dscp = oxm.u8value();
		ipv4()->set_ipv4_dscp(dscp);
		ipv4()->ipv4_calc_checksum();
		match.set_ip_dscp(dscp);
	} break;
	case openflow::OFPXMT_OFB_IP_ECN: {
		uint8_t ecn = oxm.u8value();
		ipv4()->set_ipv4_ecn(ecn);
		ipv4()->ipv4_calc_checksum();
		match.set_ip_ecn(ecn);
	} break;
	case openflow::OFPXMT_OFB_IP_PROTO: {
		uint8_t proto = oxm.u8value();
		switch (match.get_eth_type()) {
		case fipv4frame::IPV4_ETHER: {
			ipv4()->set_ipv4_proto(proto);
			ipv4()->ipv4_calc_checksum();
		} break;
		case fipv6frame::IPV6_ETHER: {
			ipv6()->set_next_header(proto);
			ipv6()->ipv6_calc_checksum();
		} break;
		}
		match.set_ip_proto(proto);
	} break;
	case openflow::OFPXMT_OFB_IPV4_SRC: {
		caddress src(oxm.u32addr());
		ipv4()->set_ipv4_src(src);
		ipv4()->ipv4_calc_checksum();
		match.set_ipv4_src(src);
	} break;
	case openflow::OFPXMT_OFB_IPV4_DST: {
		caddress dst(oxm.u32addr());
		ipv4()->set_ipv4_dst(dst);
		ipv4()->ipv4_calc_checksum();
		match.set_ipv4_dst(dst);
	} break;
	case openflow::OFPXMT_OFB_TCP_SRC: {
		uint16_t port = oxm.u16value();
		tcp()->set_sport(port);
		match.set_tcp_src(port);
		tcp()->tcp_calc_checksum(
				ipv4(-1)->get_ipv4_src(),
				ipv4(-1)->get_ipv4_dst(),
				ipv4(-1)->get_ipv4_proto(),
				tcp()->framelen());
		ipv4()->ipv4_calc_checksum();
		// TODO: handle IPv6
	} break;
	case openflow::OFPXMT_OFB_TCP_DST: {
		uint16_t port = oxm.u16value();
		tcp()->set_dport(port);
		match.set_tcp_dst(port);
		tcp()->tcp_calc_checksum(
				ipv4(-1)->get_ipv4_src(),
				ipv4(-1)->get_ipv4_dst(),
				ipv4(-1)->get_ipv4_proto(),
				tcp()->framelen());
		ipv4()->ipv4_calc_checksum();
		// TODO: handle IPv6
	} break;
	case openflow::OFPXMT_OFB_UDP_SRC: {
		uint16_t port = oxm.u16value();
		udp()->set_sport(port);
		match.set_udp_src(port);
		udp()->udp_calc_checksum(
				ipv4(-1)->get_ipv4_src(),
				ipv4(-1)->get_ipv4_dst(),
				ipv4(-1)->get_ipv4_proto(),
				udp()->framelen());
		ipv4()->ipv4_calc_checksum();
		// TODO: handle IPv6
	} break;
	case openflow::OFPXMT_OFB_UDP_DST: {
		uint16_t port = oxm.u16value();
		udp()->set_dport(port);
		match.set_udp_dst(port);
		udp()->udp_calc_checksum(
				ipv4(-1)->get_ipv4_src(),
				ipv4(-1)->get_ipv4_dst(),
				ipv4(-1)->get_ipv4_proto(),
				udp()->framelen());
		ipv4()->ipv4_calc_checksum();
		// TODO: handle IPv6
	} break;
	case openflow::OFPXMT_OFB_SCTP_SRC: {
#if 0
		sctp().set_sport(oxm.u16value());
		sctp().udp_calc_checksum(
				ipv4(-1).get_ipv4_src(),
				ipv4(-1).get_ipv4_dst(),
				ipv4(-1).get_ipv4_proto(),
				sctp().framelen());
#endif
		ipv4()->ipv4_calc_checksum();
		match.set_sctp_src(oxm.u16value());
		// TODO: handle IPv6
	} break;
	case openflow::OFPXMT_OFB_SCTP_DST: {
#if 0
		sctp().set_dport(oxm.u16value());
		sctp().udp_calc_checksum(
				ipv4(-1).get_ipv4_src(),
				ipv4(-1).get_ipv4_dst(),
				ipv4(-1).get_ipv4_proto(),
				sctp().framelen());
#endif
		ipv4()->ipv4_calc_checksum();
		match.set_sctp_dst(oxm.u16value());
		// TODO: handle IPv6
	} break;
	case openflow::OFPXMT_OFB_ICMPV4_TYPE: {
		uint16_t type = oxm.u16value();
		icmpv4()->set_icmp_type(type);
		icmpv4()->icmpv4_calc_checksum(
			icmpv4()->framelen());
		ipv4()->ipv4_calc_checksum();
		match.set_icmpv4_type(type);
	} break;
	case openflow::OFPXMT_OFB_ICMPV4_CODE: {
		uint16_t code = oxm.u16value();
		icmpv4()->set_icmp_code(code);
		icmpv4()->icmpv4_calc_checksum(
			icmpv4()->framelen());
		ipv4()->ipv4_calc_checksum();
		match.set_icmpv4_code(code);
	} break;
	case openflow::OFPXMT_OFB_ARP_OP: {
		uint16_t opcode = oxm.u16value();
		arpv4()->set_opcode(opcode);
		match.set_arp_opcode(opcode);
	} break;
	case openflow::OFPXMT_OFB_ARP_SPA: {
		caddress spa(oxm.u32addr());
		arpv4()->set_nw_src(spa);
		match.set_arp_spa(spa);
	} break;
	case openflow::OFPXMT_OFB_ARP_TPA: {
		caddress tpa(oxm.u32addr());
		arpv4()->set_nw_dst(tpa);
		match.set_arp_tpa(tpa);
	} break;
	case openflow::OFPXMT_OFB_ARP_SHA: {
		cmacaddr sha(oxm.u48addr());
		arpv4()->set_dl_src(sha);
		match.set_arp_sha(sha);
	} break;
	case openflow::OFPXMT_OFB_ARP_THA: {
		cmacaddr tha(oxm.u48addr());
		arpv4()->set_dl_dst(tha);
		match.set_arp_tha(tha);
	} break;
	case openflow::OFPXMT_OFB_IPV6_SRC: {
		caddress addr(oxm.u128addr());
		ipv6()->set_ipv6_src(addr);
		match.set_ipv6_src(addr);
	} break;
	case openflow::OFPXMT_OFB_IPV6_DST: {
		caddress addr(oxm.u128addr());
		ipv6()->set_ipv6_dst(addr);
		match.set_ipv6_dst(addr);
	} break;
	case openflow::OFPXMT_OFB_IPV6_FLABEL: {
		uint32_t flabel = oxm.u32value();
		ipv6()->set_flow_label(flabel);
		match.set_ipv6_flabel(flabel);
	} break;
	case openflow::OFPXMT_OFB_ICMPV6_TYPE: {
		uint8_t type = oxm.u8value();
		icmpv6()->set_icmpv6_type(type);
		match.set_icmpv6_type(type);
	} break;
	case openflow::OFPXMT_OFB_ICMPV6_CODE: {
		uint8_t code = oxm.u8value();
		icmpv6()->set_icmpv6_code(code);
		match.set_icmpv6_code(code);
	} break;
	case openflow::OFPXMT_OFB_IPV6_ND_TARGET: {
		caddress addr(oxm.u128addr());
		icmpv6()->set_icmpv6_neighbor_taddr(addr);
		match.set_icmpv6_neighbor_taddr(addr);
	} break;
	case openflow::OFPXMT_OFB_IPV6_ND_SLL: {
		cmacaddr maddr(oxm.u48addr());
		icmpv6()->get_option(ficmpv6opt::ICMPV6_OPT_LLADDR_SOURCE).set_ll_saddr(maddr);
		match.set_icmpv6_neighbor_source_lladdr(maddr);
	} break;
	case openflow::OFPXMT_OFB_IPV6_ND_TLL: {
		cmacaddr maddr(oxm.u48addr());
		icmpv6()->get_option(ficmpv6opt::ICMPV6_OPT_LLADDR_TARGET).set_ll_taddr(maddr);
		match.set_icmpv6_neighbor_target_lladdr(maddr);
	} break;
	case openflow::OFPXMT_OFB_MPLS_LABEL: {
		uint32_t label = oxm.u32value();
		mpls()->set_mpls_label(label);
		match.set_mpls_label(label);
	} break;
	case openflow::OFPXMT_OFB_MPLS_TC: {
		uint8_t tc = oxm.u8value();
		mpls()->set_mpls_tc(tc);
		match.set_mpls_tc(tc);
	} break;
	default: {
		logging::error << "[rofl][packet][set-field] unknown OXM:" << std::endl << oxm;
	} break;
	}
}




void
cpacket::set_field_experimenter_class(coxmatch const& oxm)
{
	switch (oxm.get_oxm_field()) {
	default: {
		logging::warn << "unable to handle experimental field " << oxm.get_oxm_field() << ", ignoring command" << std::endl;
		break;
	}
	}
}





void
cpacket::copy_ttl_out()
{
	throw eNotImplemented(); // FIXME
}


void
cpacket::copy_ttl_in()
{
	throw eNotImplemented(); // FIXME
}



void
cpacket::set_mpls_ttl(uint8_t mpls_ttl)
{
	mpls()->set_mpls_ttl(mpls_ttl);
}



void
cpacket::dec_mpls_ttl()
{
	mpls()->dec_mpls_ttl();
}




void
cpacket::set_nw_ttl(uint8_t nw_ttl)
{
	ipv4()->set_ipv4_ttl(nw_ttl);
	ipv4()->ipv4_calc_checksum();
}



void
cpacket::dec_nw_ttl()
{
	ipv4()->dec_ipv4_ttl();
	ipv4()->ipv4_calc_checksum();
}



void
cpacket::push_vlan(uint16_t ethertype)
{
	try {
		uint16_t outer_vid = 0;
		uint8_t  outer_pcp = 0;
		uint16_t vlan_eth_type = ether()->get_dl_type();


		try {
			// get default values for push actions (OF 1.1 spec section 4.9.1)
			outer_vid = vlan()->get_dl_vlan_id();
			outer_pcp = vlan()->get_dl_vlan_pcp();

		} catch (ePacketNotFound& e) {

			outer_vid = 0;
			outer_pcp = 0;
		}

		ether()->set_dl_type(ethertype);

		fvlanframe *vlan = new fvlanframe(
									tag_insert(sizeof(struct fvlanframe::vlan_hdr_t)),
									sizeof(struct fvlanframe::vlan_hdr_t));

		vlan->set_dl_type(vlan_eth_type);
		vlan->set_dl_vlan_id(outer_vid);
		vlan->set_dl_vlan_pcp(outer_pcp);

		frame_push(vlan);

#if 1
		//match.set_eth_type(vlan_eth_type); // do not do this here! we might be on top of a long stack of tags
		match.set_vlan_vid(outer_vid);
		match.set_vlan_pcp(outer_pcp);
#endif

	} catch (eMemAllocFailed& e) {

		logging::error << "[rofl][packet][push-vlan] out-of-memory" << std::endl;
	}
}



void
cpacket::pop_vlan()
{
	try {
		fvlanframe *p_vlan = vlan();

		ether()->set_dl_type(p_vlan->get_dl_type());

		tag_remove(p_vlan);

		frame_pop(p_vlan);

		delete p_vlan;

		match.remove(openflow::OFPXMC_OPENFLOW_BASIC, openflow::OFPXMT_OFB_VLAN_VID);
		match.remove(openflow::OFPXMC_OPENFLOW_BASIC, openflow::OFPXMT_OFB_VLAN_PCP);
		match.set_eth_type(ether()->get_dl_type());

	} catch (ePacketNotFound& e) {
		// no vlan tag found, pop_vlan failed
	} catch (ePacketInval& e) {
		// vlan tag is not outer tag, pop_vlan failed
	}
}



void
cpacket::push_mpls(uint16_t ethertype)
{
	try {
		uint32_t	outer_label 	= 0;
		uint8_t  	outer_ttl 		= 0;
		uint8_t  	outer_tc  		= 0;
		bool 		outer_bos 		= true;

		try {
			// get default values for push actions (OF 1.1 spec section 4.9.1)
			outer_label = mpls()->get_mpls_label();
			outer_ttl	= mpls()->get_mpls_ttl();
			outer_tc	= mpls()->get_mpls_tc();
			outer_bos	= false;

		} catch (ePacketNotFound& e) {

			outer_label		= 0;
			outer_ttl		= 0;
			outer_tc		= 0;
			outer_bos		= true;

			// TODO: get TTL from IP header, if no MPLS tag already exists
		}

		ether()->set_dl_type(ethertype);

		fmplsframe *mpls = new fmplsframe(
									tag_insert(sizeof(struct fmplsframe::mpls_hdr_t)),
									sizeof(struct fmplsframe::mpls_hdr_t));

		frame_push(mpls);

		mpls->set_mpls_label(outer_label);
		mpls->set_mpls_tc(outer_tc);
		mpls->set_mpls_ttl(outer_ttl);
		mpls->set_mpls_bos(outer_bos);

		match.set_eth_type(ethertype);
		match.set_mpls_label(outer_label);
		match.set_mpls_tc(outer_tc);

	} catch (eMemAllocFailed& e) {
		// memory allocation failed
	}
}



void
cpacket::pop_mpls(uint16_t ethertype)
{
	try {
		if (0 == dynamic_cast<fmplsframe*>( frame(1) )) {
			return;
		}

		fmplsframe *p_mpls = mpls();

		ether()->set_dl_type(ethertype);

		tag_remove(p_mpls);

		frame_pop(p_mpls);

		delete p_mpls;


		match.remove(openflow::OFPXMC_OPENFLOW_BASIC, openflow::OFPXMT_OFB_MPLS_LABEL);
		match.remove(openflow::OFPXMC_OPENFLOW_BASIC, openflow::OFPXMT_OFB_MPLS_TC);
		match.set_eth_type(ether()->get_dl_type());

	} catch (ePacketNotFound& e) {
		// no mpls tag found
	} catch (ePacketInval& e) {
		// mpls tag is not outer tag
	}
}



void
cpacket::classify(uint32_t in_port /* host byte order */)
{
	reset();

	match.set_in_port(in_port);

	try {

	    parse_ether(data.first, data.second);

	} catch (RoflException& e) {
	    // catch all exceptions here
	}
}



void
cpacket::frame_append(
		fframe *frame)
{
	if ((0 == head) && (0 == tail)) {
		head = tail = frame;
		frame->next = 0;
		frame->prev = 0;
	} else if ((head != 0) && (tail != 0)) {
		tail->next = frame;
		frame->prev = tail;
		tail = frame;
	} else {
		throw eInternalError();
	}
}


void
cpacket::frame_push(
		fframe *frame)
{
	if ((0 == head) || (0 == tail)) {
		head = tail = frame;
	} else if ((0 != head) && (0 != tail)) {
		frame->next 	= head->next;
		head->next 		= frame;
		frame->prev 	= head;

		if (0 != frame->next) {
			frame->next->prev = frame;
		} else {
			tail = frame;
		}
	} else {
		throw eInternalError();
	}
}



void
cpacket::frame_pop(
		fframe *frame)
				throw (ePacketInval)
{
	// check whether this is the second fframe (first after ether)
	// if not refuse dropping

	if (0 != frame->next) {
		frame->next->prev = frame->prev;
	} else {
		tail = frame->prev;
	}

	if (0 != frame->prev) {
		frame->prev->next = frame->next;
	} else {
		head = frame->next;
	}

	frame->next = frame->prev = (fframe*)0;
}



void
cpacket::parse_ether(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fetherframe::eth_hdr_t)) {
		return;
	}

	fetherframe *ether = new fetherframe(p_ptr, sizeof(struct fetherframe::eth_hdr_t));

	match.set_eth_dst(ether->get_dl_dst());
	match.set_eth_src(ether->get_dl_src());
	match.set_eth_type(ether->get_dl_type());

	frame_append(ether);

	p_ptr += sizeof(struct fetherframe::eth_hdr_t);
	p_len -= sizeof(struct fetherframe::eth_hdr_t);

	switch (ether->get_dl_type()) {
	case fvlanframe::VLAN_CTAG_ETHER:
	case fvlanframe::VLAN_STAG_ETHER:
	case fvlanframe::VLAN_ITAG_ETHER:		parse_vlan(p_ptr, p_len); 	break;
	case fmplsframe::MPLS_ETHER:
	case fmplsframe::MPLS_ETHER_UPSTREAM:	parse_mpls(p_ptr, p_len); 	break;
	case farpv4frame::ARPV4_ETHER:			parse_arpv4(p_ptr, p_len); 	break;
	case fipv4frame::IPV4_ETHER:			parse_ipv4(p_ptr, p_len); 	break;
	case fipv6frame::IPV6_ETHER:			parse_ipv6(p_ptr, p_len);	break;
	default: {
		if (p_len > 0) {
			fframe *payload = new fframe(p_ptr, p_len);
			frame_append(payload);
		}
	} break;
	}
}



void
cpacket::parse_vlan(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fvlanframe::vlan_hdr_t)) {
		return;
	}

	fvlanframe *vlan = new fvlanframe(p_ptr, sizeof(struct fvlanframe::vlan_hdr_t));

	if (not flags.test(FLAG_VLAN_PRESENT)) {
		match.set_vlan_vid(vlan->get_dl_vlan_id());
		match.set_vlan_pcp(vlan->get_dl_vlan_pcp());

		flags.set(FLAG_VLAN_PRESENT);
	}

	// set ethernet type based on innermost vlan tag
	match.set_eth_type(vlan->get_dl_type());

	frame_append(vlan);

	p_ptr += sizeof(struct fvlanframe::vlan_hdr_t);
	p_len -= sizeof(struct fvlanframe::vlan_hdr_t);

	switch (vlan->get_dl_type()) {
	case fvlanframe::VLAN_CTAG_ETHER:
	case fvlanframe::VLAN_STAG_ETHER:
	case fvlanframe::VLAN_ITAG_ETHER:		parse_vlan(p_ptr, p_len);	break;
	case fmplsframe::MPLS_ETHER:
	case fmplsframe::MPLS_ETHER_UPSTREAM:	parse_mpls(p_ptr, p_len);	break;
	case farpv4frame::ARPV4_ETHER:			parse_arpv4(p_ptr, p_len);	break;
	case fipv4frame::IPV4_ETHER:			parse_ipv4(p_ptr, p_len);	break;
	case fipv6frame::IPV6_ETHER:			parse_ipv6(p_ptr, p_len);	break;
	default: {
		if (p_len > 0) {
			fframe *payload = new fframe(p_ptr, p_len);
			frame_append(payload);
		}
	} break;
	}
}




void
cpacket::parse_mpls(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fmplsframe::mpls_hdr_t)) {
		return;
	}

	fmplsframe *mpls = new fmplsframe(p_ptr, sizeof(struct fmplsframe::mpls_hdr_t));

	if (not flags.test(FLAG_MPLS_PRESENT)) {
		if (ofp_version >= rofl::openflow12::OFP_VERSION) {
			match.set_mpls_label(mpls->get_mpls_label());
			match.set_mpls_tc(mpls->get_mpls_tc());
		}

		flags.set(FLAG_MPLS_PRESENT);
	}

	frame_append(mpls);

	p_ptr += sizeof(struct fmplsframe::mpls_hdr_t);
	p_len -= sizeof(struct fmplsframe::mpls_hdr_t);


	if (not mpls->get_mpls_bos()) {
		parse_mpls(p_ptr, p_len);
	} else {
		if (p_len > 0) {
			fframe *payload = new fframe(p_ptr, p_len);
			frame_append(payload);
		}
	}
}



void
cpacket::parse_arpv4(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct farpv4frame::arpv4_hdr_t))
	{
		return;
	}

	farpv4frame *arp = new farpv4frame(p_ptr, sizeof(struct farpv4frame::arpv4_hdr_t));

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {
		match.set_nw_proto(arp->get_opcode());
		match.set_nw_src(arp->get_nw_src());
		match.set_nw_dst(arp->get_nw_dst());
		match.set_eth_src(arp->get_dl_src());
		match.set_eth_dst(arp->get_dl_dst());

	} break;
	case rofl::openflow12::OFP_VERSION: {
		match.set_arp_opcode(arp->get_opcode());
		match.set_arp_spa(arp->get_nw_src());
		match.set_arp_tpa(arp->get_nw_dst());
		match.set_arp_sha(arp->get_dl_src());
		match.set_arp_tha(arp->get_dl_dst());

	} break;
	case rofl::openflow13::OFP_VERSION: {
		match.set_arp_opcode(arp->get_opcode());
		match.set_arp_spa(arp->get_nw_src());
		match.set_arp_tpa(arp->get_nw_dst());
		match.set_arp_sha(arp->get_dl_src());
		match.set_arp_tha(arp->get_dl_dst());

	} break;
	default: {

	};
	}


	frame_append(arp);

	p_ptr += sizeof(struct farpv4frame::arpv4_hdr_t);
	p_len -= sizeof(struct farpv4frame::arpv4_hdr_t);

	if (p_len > 0) {
		fframe *payload = new fframe(p_ptr, p_len);
		frame_append(payload);
	}
}



void
cpacket::parse_ipv4(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fipv4frame::ipv4_hdr_t)) {
		return;
	}

	fipv4frame *ip = new fipv4frame(p_ptr, sizeof(struct fipv4frame::ipv4_hdr_t));

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {
		match.set_nw_proto(ip->get_ipv4_proto());
		match.set_nw_dst(ip->get_ipv4_dst());
		match.set_nw_src(ip->get_ipv4_src());
		match.set_ip_dscp(ip->get_ipv4_dscp());
		match.set_ip_ecn(ip->get_ipv4_ecn());

	} break;
	case rofl::openflow12::OFP_VERSION: {
		match.set_ip_proto(ip->get_ipv4_proto());
		match.set_ipv4_dst(ip->get_ipv4_dst());
		match.set_ipv4_src(ip->get_ipv4_src());
		match.set_ip_dscp(ip->get_ipv4_dscp());
		match.set_ip_ecn(ip->get_ipv4_ecn());

	} break;
	case rofl::openflow13::OFP_VERSION: {
		match.set_ip_proto(ip->get_ipv4_proto());
		match.set_ipv4_dst(ip->get_ipv4_dst());
		match.set_ipv4_src(ip->get_ipv4_src());
		match.set_ip_dscp(ip->get_ipv4_dscp());
		match.set_ip_ecn(ip->get_ipv4_ecn());

	} break;
	default: {

	};
	}

	frame_append(ip);

	p_ptr += sizeof(struct fipv4frame::ipv4_hdr_t);
	p_len -= sizeof(struct fipv4frame::ipv4_hdr_t);

	if (ip->has_MF_bit_set()) {
		// no IPv4 fragment
		return;
	}

	// TODO: IP header with options

	switch (match.get_ip_proto()) {
	case fipv4frame::IPV4_IP_PROTO:		parse_ipv4(p_ptr, p_len);	break;
	case ficmpv4frame::ICMPV4_IP_PROTO:	parse_icmpv4(p_ptr, p_len);	break;
	case fipv6frame::IPV6_IP_PROTO:		parse_ipv6(p_ptr, p_len);	break;
#if THIS_DOES_NOT_MAKE_SENSE_HERE
	case ficmpv6frame::ICMPV6_IP_PROTO:	parse_icmpv6(p_ptr, p_len);	break;
#endif
	case fudpframe::UDP_IP_PROTO:		parse_udp(p_ptr, p_len);	break;
	case ftcpframe::TCP_IP_PROTO:		parse_tcp(p_ptr, p_len);	break;
	case fsctpframe::SCTP_IP_PROTO:		parse_sctp(p_ptr, p_len);	break;
	default: {
		if (p_len > 0) {
			fframe *payload = new fframe(p_ptr, p_len);
			frame_append(payload);
		}
	} break;
	}
}



void
cpacket::parse_icmpv4(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct ficmpv4frame::icmpv4_hdr_t)) {
		return;
	}

	ficmpv4frame *icmp = new ficmpv4frame(p_ptr, p_len);

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {
		match.set_tp_src(icmp->get_icmp_type());
		match.set_tp_dst(icmp->get_icmp_code());

	} break;
	case rofl::openflow12::OFP_VERSION: {
		match.set_icmpv4_type(icmp->get_icmp_type());
		match.set_icmpv4_code(icmp->get_icmp_code());

	} break;
	case rofl::openflow13::OFP_VERSION: {
		match.set_icmpv4_type(icmp->get_icmp_type());
		match.set_icmpv4_code(icmp->get_icmp_code());

	} break;
	default: {

	};
	}

	frame_append(icmp);

#if 0
	p_ptr += sizeof(struct ficmpv4frame::icmpv4_hdr_t);
	p_len -= sizeof(struct ficmpv4frame::icmpv4_hdr_t);

	if (p_len > 0) {
		fframe *payload = new fframe(p_ptr, p_len);
		frame_append(payload);
	}
#endif
}



void
cpacket::parse_ipv6(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fipv6frame::ipv6_hdr_t)) { // base header
		return;
	}

	fipv6frame *ip = new fipv6frame(p_ptr, sizeof(struct fipv6frame::ipv6_hdr_t));

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {

	} break;
	case rofl::openflow12::OFP_VERSION: {
		match.set_ip_proto(ip->get_next_header());
		match.set_ipv6_dst(ip->get_ipv6_dst());
		match.set_ipv6_src(ip->get_ipv6_src());
		match.set_ipv6_flabel(ip->get_flow_label());

	} break;
	case rofl::openflow13::OFP_VERSION: {
		match.set_ip_proto(ip->get_next_header());
		match.set_ipv6_dst(ip->get_ipv6_dst());
		match.set_ipv6_src(ip->get_ipv6_src());
		match.set_ipv6_flabel(ip->get_flow_label());

	} break;
	default: {

	};
	}

	frame_append(ip);

	p_ptr += sizeof(struct fipv6frame::ipv6_hdr_t);
	p_len -= sizeof(struct fipv6frame::ipv6_hdr_t);

	switch (match.get_ip_proto()) {
	case fipv4frame::IPV4_IP_PROTO:		parse_ipv4(p_ptr, p_len);	break;
	case fipv6frame::IPV6_IP_PROTO:		parse_ipv6(p_ptr, p_len);	break;
	case ficmpv6frame::ICMPV6_IP_PROTO:	parse_icmpv6(p_ptr, p_len);	break;
	case fudpframe::UDP_IP_PROTO:		parse_udp(p_ptr, p_len);	break;
	case ftcpframe::TCP_IP_PROTO:		parse_tcp(p_ptr, p_len);	break;
	case fsctpframe::SCTP_IP_PROTO:		parse_sctp(p_ptr, p_len);	break;
	default: {
		if (p_len > 0) {
			fframe *payload = new fframe(p_ptr, p_len);
			frame_append(payload);
		}
	} break;
	}
}



void
cpacket::parse_icmpv6(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct ficmpv6frame::icmpv6_hdr_t)) {
		return;
	}

	//ficmpv6frame *icmp = new ficmpv6frame(p_ptr, sizeof(struct ficmpv6frame::icmpv6_hdr_t));
	ficmpv6frame *icmp = new ficmpv6frame(p_ptr, p_len);

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {

	} break;
	case rofl::openflow12::OFP_VERSION: {
		match.set_icmpv6_type(icmp->get_icmpv6_type());
		match.set_icmpv6_code(icmp->get_icmpv6_code());

	} break;
	case rofl::openflow13::OFP_VERSION: {
		match.set_icmpv6_type(icmp->get_icmpv6_type());
		match.set_icmpv6_code(icmp->get_icmpv6_code());

	} break;
	default: {

	};
	}

	frame_append(icmp);

	p_ptr += sizeof(struct ficmpv6frame::icmpv6_hdr_t);
	p_len -= sizeof(struct ficmpv6frame::icmpv6_hdr_t);


	if (p_len > 0) {
		fframe *payload = new fframe(p_ptr, p_len);
		frame_append(payload);
	}
}



void
cpacket::parse_udp(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fudpframe::udp_hdr_t)) {
		return;
	}

	fudpframe *udp = new fudpframe(p_ptr, sizeof(struct fudpframe::udp_hdr_t));

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {
		match.set_tp_dst(udp->get_dport());
		match.set_tp_src(udp->get_sport());

	} break;
	case rofl::openflow12::OFP_VERSION: {
		match.set_udp_dst(udp->get_dport());
		match.set_udp_src(udp->get_sport());

	} break;
	case rofl::openflow13::OFP_VERSION: {
		match.set_udp_dst(udp->get_dport());
		match.set_udp_src(udp->get_sport());

	} break;
	default: {

	};
	}

	frame_append(udp);

	p_ptr += sizeof(struct fudpframe::udp_hdr_t);
	p_len -= sizeof(struct fudpframe::udp_hdr_t);

	switch (match.get_udp_dst()) {
	default: {
		if (p_len > 0)
		{
			fframe *payload = new fframe(p_ptr, p_len);

			frame_append(payload);
		}
	} break;
	}
}



void
cpacket::parse_tcp(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct ftcpframe::tcp_hdr_t)) {
		return;
	}

	ftcpframe *tcp = new ftcpframe(p_ptr, sizeof(struct ftcpframe::tcp_hdr_t));

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {
		match.set_tp_dst(tcp->get_dport());
		match.set_tp_src(tcp->get_sport());

	} break;
	case rofl::openflow12::OFP_VERSION: {
		match.set_tcp_dst(tcp->get_dport());
		match.set_tcp_src(tcp->get_sport());

	} break;
	case rofl::openflow13::OFP_VERSION: {
		match.set_tcp_dst(tcp->get_dport());
		match.set_tcp_src(tcp->get_sport());

	} break;
	default: {

	};
	}

	frame_append(tcp);

	p_ptr += sizeof(struct ftcpframe::tcp_hdr_t);
	p_len -= sizeof(struct ftcpframe::tcp_hdr_t);

	if (p_len > 0) {
		fframe *payload = new fframe(p_ptr, p_len);
		frame_append(payload);
	}
}



void
cpacket::parse_sctp(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fsctpframe::sctp_hdr_t)) {
		return;
	}

	fsctpframe *sctp = new fsctpframe(p_ptr, sizeof(struct fsctpframe::sctp_hdr_t));

	switch (ofp_version) {
	case rofl::openflow10::OFP_VERSION: {

	} break;
	case rofl::openflow12::OFP_VERSION: {
		match.set_sctp_dst(sctp->get_dport());
		match.set_sctp_src(sctp->get_sport());

	} break;
	case rofl::openflow13::OFP_VERSION: {
		match.set_sctp_dst(sctp->get_dport());
		match.set_sctp_src(sctp->get_sport());

	} break;
	default: {

	};
	}

	frame_append(sctp);

	p_ptr += sizeof(struct fsctpframe::sctp_hdr_t);
	p_len -= sizeof(struct fsctpframe::sctp_hdr_t);

	if (p_len > 0) {
		fframe *payload = new fframe(p_ptr, p_len);
		frame_append(payload);
	}
}



void
cpacket::calc_checksums()
{
	RwLock lock(&ac_rwlock, RwLock::RWLOCK_READ);

	if (flags.test(FLAG_TCP_CHECKSUM)) {
		tcp()->tcp_calc_checksum(
			ipv4(-1)->get_ipv4_src(),
			ipv4(-1)->get_ipv4_dst(),
			ipv4(-1)->get_ipv4_proto(),
			get_payload_len(tcp())); // second parameter = 0 => up to tail frame
	}

	if (flags.test(FLAG_UDP_CHECKSUM)) {
		udp()->udp_calc_checksum(
			ipv4(-1)->get_ipv4_src(),
			ipv4(-1)->get_ipv4_dst(),
			ipv4(-1)->get_ipv4_proto(),
			get_payload_len(udp())); // second parameter = 0 => up to tail frame
	}

	if (flags.test(FLAG_IPV4_CHECKSUM)) {
		ipv4()->ipv4_calc_checksum();
	}

	if (flags.test(FLAG_ICMPV4_CHECKSUM)) {
		icmpv4()->icmpv4_calc_checksum(get_payload_len(
            icmpv4()));
	}
}



void
cpacket::calc_hits(
		cofmatch& ofmatch,
		uint16_t& exact_hits,
		uint16_t& wildcard_hits,
		uint16_t& missed)
{
	this->match.is_part_of(ofmatch, exact_hits, wildcard_hits, missed);
}



unsigned int
cpacket::cnt_vlan_tags()
{
	unsigned int cnt = 0;

	if ((0 == head) || (0 == tail)) {
		return cnt;
	}

	fframe *frame = head;

	while (0 != frame) {
		if (0 != dynamic_cast<fvlanframe*>( frame )) {
			cnt++;
		}
		if (0 == frame->next) {
			break;
		}
		frame = frame->next;
	}

	return cnt;
}



unsigned int
cpacket::cnt_mpls_tags()
{
	unsigned int cnt = 0;

	if ((0 == head) || (0 == tail)) {
		return cnt;
	}

	fframe *frame = head;

	while (0 != frame) {
		if (0 != dynamic_cast<fmplsframe*>( frame )) {
			cnt++;
		}
		if (0 == frame->next) {
			break;
		}
		frame = frame->next;
	}

	return cnt;
}




void
cpacket::action_set_field(
		cofaction& action)
{
	set_field(action.get_oxm());
}


void
cpacket::action_copy_ttl_out(
		cofaction& action)
{
	copy_ttl_out();
}


void
cpacket::action_copy_ttl_in(
		cofaction& action)
{
	copy_ttl_in();
}


void
cpacket::action_set_mpls_ttl(
		cofaction& action)
{
	set_mpls_ttl(action.oac_12mpls_ttl->mpls_ttl);
}


void
cpacket::action_dec_mpls_ttl(
		cofaction& action)
{
	dec_mpls_ttl();
}


void
cpacket::action_push_vlan(
		cofaction& action)
{
	push_vlan(be16toh(action.oac_12push->ethertype));
}


void
cpacket::action_pop_vlan(
		cofaction& action)
{
	pop_vlan();
}


void
cpacket::action_push_mpls(
		cofaction& action)
{
	push_mpls(be16toh(action.oac_12push->ethertype));
}


void
cpacket::action_pop_mpls(
		cofaction& action)
{
	pop_mpls(be16toh(action.oac_12pop_mpls->ethertype));
}


void
cpacket::action_set_nw_ttl(
		cofaction& action)
{
	set_nw_ttl(action.oac_12nw_ttl->nw_ttl);
}


void
cpacket::action_dec_nw_ttl(
		cofaction& action)
{
	dec_nw_ttl();
}


