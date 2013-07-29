/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "cpacket.h"

using namespace rofl;

bool				cpacket::cpacket_init = false;
std::string			cpacket::s_cpacket_info;
pthread_rwlock_t 	cpacket::cpacket_lock;
std::set<cpacket*> 	cpacket::cpacket_list;



const char*
cpacket::cpacket_info()
{
	RwLock lock(&cpacket::cpacket_lock, RwLock::RWLOCK_READ);
	cvastring vas(2048);
	s_cpacket_info.assign(vas("cpackets: #%lu ", cpacket::cpacket_list.size()));
	for (std::set<cpacket*>::iterator
			it = cpacket::cpacket_list.begin();
				it != cpacket::cpacket_list.end(); ++it)
	{
		s_cpacket_info.append(vas("  %s", (*it)->c_str()));
	}
	return s_cpacket_info.c_str();
}


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
			size_t size,
			uint32_t in_port,
			bool do_classify) :
					total_len(size),
					head(0),
					tail(0),
					hspace(CPACKET_DEFAULT_HSPACE),
					tspace(CPACKET_DEFAULT_TSPACE),
					mem(size + hspace + tspace),
					data(std::pair<uint8_t*, size_t>(mem.somem() + hspace, size)),
					packet_receive_time(time(NULL)),
					in_port(0)
{
	pthread_rwlock_init(&ac_rwlock, NULL);

	match.set_in_port(in_port);
	match.set_in_phy_port(in_port);

	if (do_classify)
	{
		classify(in_port);
	}

	cpacket_list_insert();
}



cpacket::cpacket(
		cmemory *mem,
		uint32_t in_port,
		bool do_classify) :
				total_len(mem->memlen()),
				head(0),
				tail(0),
				hspace(CPACKET_DEFAULT_HSPACE),
				tspace(0),
				mem(mem->memlen() + hspace + tspace),
				data(std::pair<uint8_t*, size_t>(this->mem.somem() + hspace, mem->memlen())),
				packet_receive_time(time(NULL)),
				in_port(in_port)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::cpacket()", this);

	memcpy(soframe(), mem->somem(), mem->memlen());

	pthread_rwlock_init(&ac_rwlock, NULL);

	match.set_in_port(OFPP_CONTROLLER);
	match.set_in_phy_port(OFPP_CONTROLLER);

	if (do_classify)
	{
		classify(in_port);
	}

	cpacket_list_insert();
}



cpacket::cpacket(
		uint8_t *buf,
		size_t buflen,
		uint32_t in_port,
		bool do_classify) :
				total_len(buflen),
				head(0),
				tail(0),
				hspace(CPACKET_DEFAULT_HSPACE),
				tspace(0),
				mem(buflen + hspace + tspace),
				//mem(buf, buflen, CPACKET_HEAD_ROOM, CPACKET_TAIL_ROOM),
				data(std::pair<uint8_t*, size_t>(mem.somem() + hspace, buflen)),
				packet_receive_time(time(NULL)),
				in_port(in_port)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::cpacket()", this);

	pthread_rwlock_init(&ac_rwlock, NULL);

	memcpy(soframe(), buf, buflen);



	match.set_in_port(OFPP_CONTROLLER);
	match.set_in_phy_port(OFPP_CONTROLLER);

	if (do_classify)
	{
		classify(in_port);
	}

	cpacket_list_insert();
}



cpacket::cpacket(
		cpacket const& pack) :
				total_len(0),
				head(0),
				tail(0),
				hspace(CPACKET_DEFAULT_HSPACE),
				tspace(0),
				mem(pack.framelen() + hspace),
				//mem(buf, buflen, CPACKET_HEAD_ROOM, CPACKET_TAIL_ROOM),
				data(std::pair<uint8_t*, size_t>(mem.somem() + hspace, pack.framelen())),
				packet_receive_time(time(NULL)),
				in_port(0)
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::cpacket()", this);

	pthread_rwlock_init(&ac_rwlock, NULL);

	*this = pack;

	cpacket_list_insert();
}



cpacket::~cpacket()
{
	WRITELOG(CPACKET, DBG, "cpacket(%p)::~cpacket()", this);
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

	while (head != 0)
	{
		fframe *curr = head;

		head = curr->next;

		curr->next = 0;
		curr->prev = 0;

		delete curr;
	}

	head = 0;
	tail = 0;
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

	total_len			= p.total_len;
	match				= p.match;

	mem_resize(p.framelen());
	memcpy(soframe(), p.soframe(), p.framelen());
	packet_receive_time = p.packet_receive_time;

	classify(match.get_in_port());

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
	if (data.second == p.data.second)
	{

		return (!memcmp(data.first, p.data.first, data.second));
	}
	return false;
}



bool
cpacket::operator== (
		cmemory const& m)
{
	if (data.second == m.memlen())
	{
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

	if (f.framelen() > tspace) // requiring resize
	{
		mem_resize(framelen() + f.framelen());
		// mem_resize() sets framelen() to old-framelen() + f.framelen()
	}
	else
	{
		data.second = len + f.framelen();
	}

	memcpy(soframe() + len, f.soframe(), f.framelen());

	uint32_t in_port = OFPP_CONTROLLER;
	try {
		in_port = match.get_in_port();
	} catch (eOFmatchNotFound& e) {}
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



const char*
cpacket::c_str()
{
	cvastring vas(4096);

	info.assign(vas("cpacket(%p) \n", this));

	info.append(vas("  match: %s\n", match.c_str()));

	info.append(vas("  head: %p  tail: %p\n", head, tail));

	for (fframe* curr = head; curr != 0; curr = curr->next)
	{
		info.append(vas("  %s\n", curr->c_str()));
	}

	info.append(vas("  soframe(): %p framelen(): %lu\n", soframe(), framelen()));

	info.append(data_c_str());



	return info.c_str();
}



const char*
cpacket::data_c_str()
{
	d_info.clear();

	cvastring vas;

	d_info.assign(vas("data:\n%p : ", data.first));

	for (int i = 0; i < (int)data.second; ++i)
	{
		char t[8];
		memset(t, 0, sizeof(t));

		snprintf(t, sizeof(t)-1, "%02x ", (data.first[i]));
		d_info.append(t);
		if ((0 == ((i+1) % 16)))
		{
			char ptr[32]; memset(ptr, 0x00, sizeof(ptr));
			snprintf(ptr, sizeof(ptr)-1, "\n%p : ", data.first + (i+1));
			d_info.append(ptr);
		}
		else if ((0 == ((i+1) % 4)))
		{
			d_info.append("  ");
		}
	}

	return d_info.c_str();
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



fpppoeframe*
cpacket::pppoe(int i) throw (ePacketNotFound)
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
			if (dynamic_cast<fpppoeframe*>( frame ))
			{
				return (dynamic_cast<fpppoeframe*>( frame ));
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
			if (dynamic_cast<fpppoeframe*>( frame ))
			{
				return (dynamic_cast<fpppoeframe*>( frame ));
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



fpppframe*
cpacket::ppp(int i) throw (ePacketNotFound)
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
			if (dynamic_cast<fpppframe*>( frame ))
			{
				return (dynamic_cast<fpppframe*>( frame ));
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
			if (dynamic_cast<fpppframe*>( frame ))
			{
				return (dynamic_cast<fpppframe*>( frame ));
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
	case OFPXMC_OPENFLOW_BASIC: {
		set_field_basic_class(oxm);
	} break;
	case OFPXMC_EXPERIMENTER: {
		set_field_experimenter_class(oxm);
	} break;
	default: {
		WRITELOG(CPACKET, WARN, "cpacket(%p)::set_field() "
				"don't know how to handle class:0x%x field:%d, ignoring",
				this, oxm.get_oxm_class(), oxm.get_oxm_field());
	} break;
	}
}



void
cpacket::set_field_basic_class(coxmatch const& oxm)
{
	switch (oxm.get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC: {

		switch (oxm.get_oxm_field()) {
		case OFPXMT_OFB_ETH_DST: {
			cmacaddr maddr(oxm.u48addr());
			ether()->set_dl_dst(maddr);
			match.set_eth_dst(maddr);
		} break;
		case OFPXMT_OFB_ETH_SRC: {
			cmacaddr maddr(oxm.u48addr());
			ether()->set_dl_src(maddr);
			match.set_eth_src(maddr);
		} break;
		case OFPXMT_OFB_ETH_TYPE: {
			uint16_t eth_type = oxm.u16value();
			ether()->set_dl_type(eth_type);
			match.set_eth_type(eth_type);
		} break;
		case OFPXMT_OFB_VLAN_VID: {
			uint16_t vid = oxm.u16value();
			vlan()->set_dl_vlan_id(vid);
			match.set_vlan_vid(vid);
		} break;
		case OFPXMT_OFB_VLAN_PCP: {
			uint8_t pcp = oxm.u8value();
			vlan()->set_dl_vlan_pcp(pcp);
			match.set_vlan_pcp(pcp);
		} break;
		case OFPXMT_OFB_IP_DSCP: {
			uint8_t dscp = oxm.u8value();
			ipv4()->set_ipv4_dscp(dscp);
			ipv4()->ipv4_calc_checksum();
			match.set_ip_dscp(dscp);
		} break;
		case OFPXMT_OFB_IP_ECN: {
			uint8_t ecn = oxm.u8value();
			ipv4()->set_ipv4_ecn(ecn);
			ipv4()->ipv4_calc_checksum();
			match.set_ip_ecn(ecn);
		} break;
		case OFPXMT_OFB_IP_PROTO: {
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
		case OFPXMT_OFB_IPV4_SRC: {
			caddress src(oxm.u32addr());
			ipv4()->set_ipv4_src(src);
			ipv4()->ipv4_calc_checksum();
			match.set_ipv4_src(src);
		} break;
		case OFPXMT_OFB_IPV4_DST: {
			caddress dst(oxm.u32addr());
			ipv4()->set_ipv4_dst(dst);
			ipv4()->ipv4_calc_checksum();
			match.set_ipv4_dst(dst);
		} break;
		case OFPXMT_OFB_TCP_SRC: {
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
		case OFPXMT_OFB_TCP_DST: {
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
		case OFPXMT_OFB_UDP_SRC: {
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
		case OFPXMT_OFB_UDP_DST: {
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
		case OFPXMT_OFB_SCTP_SRC: {
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
		case OFPXMT_OFB_SCTP_DST: {
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
		case OFPXMT_OFB_ICMPV4_TYPE: {
			uint16_t type = oxm.u16value();
			icmpv4()->set_icmp_type(type);
			icmpv4()->icmpv4_calc_checksum(
				icmpv4()->framelen());
			ipv4()->ipv4_calc_checksum();
			match.set_icmpv4_type(type);
		} break;
		case OFPXMT_OFB_ICMPV4_CODE: {
			uint16_t code = oxm.u16value();
			icmpv4()->set_icmp_code(code);
			icmpv4()->icmpv4_calc_checksum(
				icmpv4()->framelen());
			ipv4()->ipv4_calc_checksum();
			match.set_icmpv4_code(code);
		} break;
		case OFPXMT_OFB_ARP_OP: {
			uint16_t opcode = oxm.u16value();
			arpv4()->set_opcode(opcode);
			match.set_arp_opcode(opcode);
		} break;
		case OFPXMT_OFB_ARP_SPA: {
			caddress spa(oxm.u32addr());
			arpv4()->set_nw_src(spa);
			match.set_arp_spa(spa);
		} break;
		case OFPXMT_OFB_ARP_TPA: {
			caddress tpa(oxm.u32addr());
			arpv4()->set_nw_dst(tpa);
			match.set_arp_tpa(tpa);
		} break;
		case OFPXMT_OFB_ARP_SHA: {
			cmacaddr sha(oxm.u48addr());
			arpv4()->set_dl_src(sha);
			match.set_arp_sha(sha);
		} break;
		case OFPXMT_OFB_ARP_THA: {
			cmacaddr tha(oxm.u48addr());
			arpv4()->set_dl_dst(tha);
			match.set_arp_tha(tha);
		} break;
		case OFPXMT_OFB_IPV6_SRC: {
			caddress addr(oxm.u128addr());
			ipv6()->set_ipv6_src(addr);
			match.set_ipv6_src(addr);
		} break;
		case OFPXMT_OFB_IPV6_DST: {
			caddress addr(oxm.u128addr());
			ipv6()->set_ipv6_dst(addr);
			match.set_ipv6_dst(addr);
		} break;
		case OFPXMT_OFB_IPV6_FLABEL: {
			uint32_t flabel = oxm.u32value();
			ipv6()->set_flow_label(flabel);
			match.set_ipv6_flabel(flabel);
		} break;
		case OFPXMT_OFB_ICMPV6_TYPE: {
			uint8_t type = oxm.u8value();
			icmpv6()->set_icmpv6_type(type);
			match.set_icmpv6_type(type);
		} break;
		case OFPXMT_OFB_ICMPV6_CODE: {
			uint8_t code = oxm.u8value();
			icmpv6()->set_icmpv6_code(code);
			match.set_icmpv6_code(code);
		} break;
		case OFPXMT_OFB_IPV6_ND_TARGET: {
			caddress addr(oxm.u128addr());
			icmpv6()->set_icmpv6_neighbor_taddr(addr);
			match.set_icmpv6_neighbor_taddr(addr);
		} break;
		case OFPXMT_OFB_IPV6_ND_SLL: {
			cmacaddr maddr(oxm.u48addr());
			icmpv6()->get_option(ficmpv6opt::ICMPV6_OPT_LLADDR_SOURCE).set_ll_saddr(maddr);
			match.set_icmpv6_neighbor_source_lladdr(maddr);
		} break;
		case OFPXMT_OFB_IPV6_ND_TLL: {
			cmacaddr maddr(oxm.u48addr());
			icmpv6()->get_option(ficmpv6opt::ICMPV6_OPT_LLADDR_TARGET).set_ll_taddr(maddr);
			match.set_icmpv6_neighbor_target_lladdr(maddr);
		} break;
		case OFPXMT_OFB_MPLS_LABEL: {
			uint32_t label = oxm.u32value();
			mpls()->set_mpls_label(label);
			match.set_mpls_label(label);
		} break;
		case OFPXMT_OFB_MPLS_TC: {
			uint8_t tc = oxm.u8value();
			mpls()->set_mpls_tc(tc);
			match.set_mpls_tc(tc);
		} break;
		default: {
			WRITELOG(CPACKET, WARN, "cpacket(%p)::set_field() "
					"don't know how to handle class:0x%x field:%d, ignoring",
					this, oxm.get_oxm_class(), oxm.get_oxm_field());
		} break;
		}
	} break;

	default: {
		WRITELOG(CPACKET, WARN, "cpacket(%p)::set_field() "
				"unsupported OXM TLV class:0x%x, ignoring",
				this, oxm.get_oxm_class());
	} break;
	}
}




void
cpacket::set_field_experimenter_class(coxmatch const& oxm)
{
	switch (oxm.get_oxm_class()) {
	case OFPXMC_EXPERIMENTER: {

		switch (oxm.get_oxm_field()) {
		/*
		 * PPP/PPPoE related extensions
		 */
		case OFPXMT_OFX_PPPOE_CODE: {
			uint8_t code = oxm.u8value();
			pppoe()->set_pppoe_code(code);
			match.set_pppoe_code(code);
		} break;
		case OFPXMT_OFX_PPPOE_TYPE: {
			uint8_t type = oxm.u8value();
			pppoe()->set_pppoe_type(type);
			match.set_pppoe_type(type);
		} break;
		case OFPXMT_OFX_PPPOE_SID: {
			uint16_t sid = oxm.u16value();
			pppoe()->set_pppoe_sessid(sid);
			match.set_pppoe_sessid(sid);
		} break;
		case OFPXMT_OFX_PPP_PROT: {
			uint16_t prot = oxm.u16value();
			ppp()->set_ppp_prot(prot);
			match.set_ppp_prot(prot);
		} break;
		default: {
			WRITELOG(CPACKET, WARN, "cpacket(%p)::set_field() "
					"don't know how to handle class:0x%x field:%d, ignoring",
					this, oxm.get_oxm_class(), oxm.get_oxm_field());
		} break;
		}

	} break;

	default: {
		WRITELOG(CPACKET, WARN, "cpacket(%p)::set_field() "
				"unsupported OXM TLV class:0x%x, ignoring",
				this, oxm.get_oxm_class());
	} break;
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

		WRITELOG(CPACKET, DBG, "cpacket(%p)::push_vlan() "
				"mem: %s", this, mem.c_str());

	} catch (eMemAllocFailed& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::push_vlan() "
				"memory allocation failed", this);
	}
}



void
cpacket::pop_vlan()
{
	try {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_vlan() ", this);

		fvlanframe *p_vlan = vlan();

		ether()->set_dl_type(p_vlan->get_dl_type());

		tag_remove(p_vlan);

		frame_pop(p_vlan);

		delete p_vlan;

		//classify(match.get_in_port());

#if 1
		match.remove(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_VID);
		match.remove(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_PCP);
		match.set_eth_type(ether()->get_dl_type());
#endif

	} catch (ePacketNotFound& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_vlan() "
				"no vlan tag found, mem: %s", this, mem.c_str());
	} catch (ePacketInval& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_vlan() "
				"vlan tag is not outer tag, ignoring pop command, mem: %s", this, mem.c_str());
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

		WRITELOG(CPACKET, DBG, "cpacket(%p)::push_mpls() "
				"mem: %s", this, mem.c_str());


	} catch (eMemAllocFailed& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::push_mpls() "
				"memory allocation failed", this);
	}
}



void
cpacket::pop_mpls(uint16_t ethertype)
{
	try {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_mpls() ", this);

		if (0 == dynamic_cast<fmplsframe*>( frame(1) ))
		{
			fprintf(stderr, "\n\nTTTT\n\n");
			return;
		}

		fmplsframe *p_mpls = mpls();

		ether()->set_dl_type(ethertype);

		tag_remove(p_mpls);

		frame_pop(p_mpls);

		delete p_mpls;

		//classify(match.get_in_port());

#if 1
		match.remove(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_LABEL);
		match.remove(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_TC);
		match.set_eth_type(ether()->get_dl_type());
#endif

	} catch (ePacketNotFound& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_mpls() "
				"no mpls tag found, mem: %s", this, mem.c_str());
	} catch (ePacketInval& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_mpls() "
				"mpls tag is not outer tag, ignoring pop command, mem: %s", this, mem.c_str());
	}
}



void
cpacket::push_pppoe(uint16_t ethertype)
{
	try {
		uint8_t code = 0;
		uint16_t sid = 0;
		uint16_t len = data.second - sizeof(struct fetherframe::eth_hdr_t);

		ether()->set_dl_type(ethertype);

		fpppoeframe *pppoe = (fpppoeframe*)0;
		fpppframe *ppp = (fpppframe*)0;

		switch (ethertype) {
		case fpppoeframe::PPPOE_ETHER_DISCOVERY:
		{
		    pppoe = new fpppoeframe(
				tag_insert(sizeof(struct fpppoeframe::pppoe_hdr_t)),
				            sizeof(struct fpppoeframe::pppoe_hdr_t));

		    frame_push(pppoe);
		}
		    break;
		case fpppoeframe::PPPOE_ETHER_SESSION:
		{
                    ppp = new fpppframe(
                                tag_insert(sizeof(struct fpppframe::ppp_hdr_t)),
                                            sizeof(struct fpppframe::ppp_hdr_t));

                    frame_push(ppp);

                    ppp->set_ppp_prot(0x0000);
                    match.set_ppp_prot(0x0000);

                    pppoe = new fpppoeframe(
                              tag_insert(sizeof(struct fpppoeframe::pppoe_hdr_t)),
                                          sizeof(struct fpppoeframe::pppoe_hdr_t));

                    frame_push(pppoe);

                    len += sizeof(uint16_t);
		}
		    break;
		}

		pppoe->set_pppoe_vers(fpppoeframe::PPPOE_VERSION);
		pppoe->set_pppoe_type(fpppoeframe::PPPOE_TYPE);
		pppoe->set_pppoe_code(code);
		pppoe->set_pppoe_sessid(sid);
		pppoe->set_hdr_length(len);

		match.set_eth_type(ethertype);
		match.set_pppoe_type(fpppoeframe::PPPOE_TYPE);
		match.set_pppoe_code(code);
		match.set_pppoe_sessid(sid);

		WRITELOG(CPACKET, DBG, "cpacket(%p)::push_pppoe() "
				"mem: %s", this, mem.c_str());

	} catch (eMemAllocFailed& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::push_pppoe() "
				"memory allocation failed", this);
	}
}



void
cpacket::pop_pppoe(uint16_t ethertype)
{
	try {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_pppoe() ", this);

		try {
			ppp(); // throws exception when there is no ppp frame (e.g. for pppoe discovery frames)

			pop_ppp();
		} catch (ePacketNotFound& e) {}

		fpppoeframe *p_pppoe = pppoe();

		ether()->set_dl_type(ethertype);

		tag_remove(p_pppoe);

		frame_pop(p_pppoe);

		delete p_pppoe;

		//classify(match.get_in_port());

#if 1
		match.remove(OFPXMC_EXPERIMENTER, OFPXMT_OFX_PPPOE_TYPE);
		match.remove(OFPXMC_EXPERIMENTER, OFPXMT_OFX_PPPOE_CODE);
		match.remove(OFPXMC_EXPERIMENTER, OFPXMT_OFX_PPPOE_SID);
		match.set_eth_type(ether()->get_dl_type());
#endif

	} catch (ePacketNotFound& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_pppoe() "
				"no pppoe tag found, mem: %s", this, mem.c_str());
	} catch (ePacketInval& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_mpls() "
				"pppoe tag is not outer tag, ignoring pop command, mem: %s", this, mem.c_str());
	}
}


#if 0
void
cpacket::push_ppp(uint16_t code)
{
	try {
		fpppframe *ppp = new fpppframe(
									tag_insert(sizeof(struct fpppframe::ppp_hdr_t)),
										sizeof(struct fpppframe::ppp_hdr_t));

		frame_push(ppp);

		ppp->set_ppp_prot(code);

		match.set_ppp_prot(code);

		WRITELOG(CPACKET, DBG, "cpacket(%p)::push_ppp() "
				"mem: %s", this, mem.c_str());

	} catch (eMemAllocFailed& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::push_ppp() "
				"memory allocation failed", this);
	}
}
#endif


void
cpacket::pop_ppp()
{
	try {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_ppp() ", this);

		fpppframe *p_ppp = ppp();

		tag_remove(p_ppp);

		frame_pop(p_ppp);

		delete p_ppp;

		//classify(match.get_in_port());

#if 1
		match.remove(OFPXMC_EXPERIMENTER, OFPXMT_OFX_PPP_PROT);
#endif

	} catch (ePacketNotFound& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_ppp() "
				"no ppp tag found, mem: %s", this, mem.c_str());
	} catch (ePacketInval& e) {

		WRITELOG(CPACKET, DBG, "cpacket(%p)::pop_mpls() "
				"ppp tag is not outer tag, ignoring pop command, mem: %s", this, mem.c_str());
	}
}



void
cpacket::classify(uint32_t in_port /* host byte order */)
{
	reset();

	match.set_in_port(in_port);

	try {

	    parse_ether(data.first, data.second);

	} catch (cerror& e) {
	    // catch all exceptions here
	}
}



void
cpacket::frame_append(
		fframe *frame)
{

	if ((0 == head) && (0 == tail))
	{
		head = tail = frame;
		frame->next = 0;
		frame->prev = 0;
	}
	else if ((head != 0) && (tail != 0))
	{
		tail->next = frame;
		frame->prev = tail;
		tail = frame;
	}
	else
	{
		// internal list management invalid
		throw eInternalError();
	}

}


void
cpacket::frame_push(
		fframe *frame)
{
	if ((0 == head) || (0 == tail))
	{
		head = tail = frame;
	}
	else if ((0 != head) && (0 != tail))
	{
		frame->next 	= head->next;
		head->next 		= frame;
		frame->prev 	= head;

		if (0 != frame->next)
		{
			frame->next->prev = frame;
		}
		else
		{
			tail = frame;
		}
	}
	else
	{
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

	//fprintf(stderr, "cpacket(%p)::frame_pop() "
	//		"frame: %p %s\n", this, frame, frame->c_str());

#if 0
	if ((0 == head) || (head->next != frame))
	{
		throw ePacketInval();
	}
#endif

	if (0 != frame->next)
	{
		frame->next->prev = frame->prev;
	}
	else
	{
		tail = frame->prev;
	}

	if (0 != frame->prev)
	{
		frame->prev->next = frame->next;
	}
	else
	{
		head = frame->next;
	}

	frame->next = (fframe*)0;
	frame->prev = (fframe*)0;
}



void
cpacket::parse_ether(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fetherframe::eth_hdr_t))
	{
		return;
	}

	fetherframe *ether = new fetherframe(p_ptr, sizeof(struct fetherframe::eth_hdr_t));

	match.set_eth_dst(ether->get_dl_dst());
	match.set_eth_src(ether->get_dl_src());
	match.set_eth_type(ether->get_dl_type());

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_ether() "
			"ether: %s\nmatch: %s", this, ether->c_str(), match.c_str());

	frame_append(ether);

	p_ptr += sizeof(struct fetherframe::eth_hdr_t);
	p_len -= sizeof(struct fetherframe::eth_hdr_t);


	switch (ether->get_dl_type()) {
	case fvlanframe::VLAN_CTAG_ETHER:
	case fvlanframe::VLAN_STAG_ETHER:
	case fvlanframe::VLAN_ITAG_ETHER:
		{
			parse_vlan(p_ptr, p_len);
		}
		break;
	case fmplsframe::MPLS_ETHER:
	case fmplsframe::MPLS_ETHER_UPSTREAM:
		{
			parse_mpls(p_ptr, p_len);
		}
		break;
	case fpppoeframe::PPPOE_ETHER_DISCOVERY:
	case fpppoeframe::PPPOE_ETHER_SESSION:
		{
			parse_pppoe(p_ptr, p_len);
		}
		break;
	case farpv4frame::ARPV4_ETHER:
		{
			parse_arpv4(p_ptr, p_len);
		}
		break;
	case fipv4frame::IPV4_ETHER:
		{
			parse_ipv4(p_ptr, p_len);
		}
		break;
	case fipv6frame::IPV6_ETHER:
		{
			parse_ipv6(p_ptr, p_len);
		}
		break;
	default:
		{
			if (p_len > 0)
			{
				fframe *payload = new fframe(p_ptr, p_len);

				frame_append(payload);
			}
		}
		break;
	}
}



void
cpacket::parse_vlan(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fvlanframe::vlan_hdr_t))
	{
		return;
	}

	fvlanframe *vlan = new fvlanframe(p_ptr, sizeof(struct fvlanframe::vlan_hdr_t));

	if (not flags.test(FLAG_VLAN_PRESENT))
	{
		match.set_vlan_vid(vlan->get_dl_vlan_id());
		match.set_vlan_pcp(vlan->get_dl_vlan_pcp());

		flags.set(FLAG_VLAN_PRESENT);
	}

	// set ethernet type based on innermost vlan tag
	match.set_eth_type(vlan->get_dl_type());

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_vlan() "
			"vlan: %s\nmatch: %s", this, vlan->c_str(), match.c_str());

	frame_append(vlan);

	p_ptr += sizeof(struct fvlanframe::vlan_hdr_t);
	p_len -= sizeof(struct fvlanframe::vlan_hdr_t);


	switch (vlan->get_dl_type()) {
	case fvlanframe::VLAN_CTAG_ETHER:
	case fvlanframe::VLAN_STAG_ETHER:
	case fvlanframe::VLAN_ITAG_ETHER:
		{
			parse_vlan(p_ptr, p_len);
		}
		break;
	case fmplsframe::MPLS_ETHER:
	case fmplsframe::MPLS_ETHER_UPSTREAM:
		{
			parse_mpls(p_ptr, p_len);
		}
		break;
	case fpppoeframe::PPPOE_ETHER_DISCOVERY:
	case fpppoeframe::PPPOE_ETHER_SESSION:
		{
			parse_pppoe(p_ptr, p_len);
		}
		break;
	case farpv4frame::ARPV4_ETHER:
		{
			parse_arpv4(p_ptr, p_len);
		}
		break;
	case fipv4frame::IPV4_ETHER:
		{
			parse_ipv4(p_ptr, p_len);
		}
		break;
	case fipv6frame::IPV6_ETHER:
		{
			parse_ipv6(p_ptr, p_len);
		}
		break;
	default:
		{
			if (p_len > 0)
			{
				fframe *payload = new fframe(p_ptr, p_len);

				frame_append(payload);
			}
		}
		break;
	}
}




void
cpacket::parse_mpls(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fmplsframe::mpls_hdr_t))
	{
		return;
	}

	fmplsframe *mpls = new fmplsframe(p_ptr, sizeof(struct fmplsframe::mpls_hdr_t));

	if (not flags.test(FLAG_MPLS_PRESENT))
	{
		match.set_mpls_label(mpls->get_mpls_label());
		match.set_mpls_tc(mpls->get_mpls_tc());

		flags.set(FLAG_MPLS_PRESENT);
	}

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_mpls() "
			"mpls: %s\nmatch: %s", this, mpls->c_str(), match.c_str());

	frame_append(mpls);

	p_ptr += sizeof(struct fmplsframe::mpls_hdr_t);
	p_len -= sizeof(struct fmplsframe::mpls_hdr_t);


	if (not mpls->get_mpls_bos())
	{
		parse_mpls(p_ptr, p_len);
	}
	else
	{
		if (p_len > 0)
		{
			fframe *payload = new fframe(p_ptr, p_len);

			frame_append(payload);
		}
	}
}



void
cpacket::parse_pppoe(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fpppoeframe::pppoe_hdr_t))
	{
		return;
	}

	fpppoeframe *pppoe = new fpppoeframe(p_ptr, sizeof(struct fpppoeframe::pppoe_hdr_t));

	match.set_pppoe_code(pppoe->get_pppoe_code());
	match.set_pppoe_type(pppoe->get_pppoe_type());
	match.set_pppoe_sessid(pppoe->get_pppoe_sessid());

	frame_append(pppoe);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_pppoe() "
			"pppoe: %s\nmatch: %s", this, pppoe->c_str(), match.c_str());





	switch (match.get_eth_type()) {
	case fpppoeframe::PPPOE_ETHER_DISCOVERY:
		{
			p_len -= sizeof(struct fpppoeframe::pppoe_hdr_t);

			uint16_t pppoe_len = pppoe->get_hdr_length() > p_len ? p_len : pppoe->get_hdr_length();

			/*
			 * parse any pppoe service tags
			 */
			pppoe->unpack(
					p_ptr,
					sizeof(struct fpppoeframe::pppoe_hdr_t) + pppoe_len);


			/*
			 * any remaining bytes after the pppoe tags => padding?
			 */
			if (p_len > pppoe->tags.length())
			{
				fframe *payload = new fframe(p_ptr, p_len - pppoe->tags.length());

				frame_append(payload);
			}
		}
		break;
	case fpppoeframe::PPPOE_ETHER_SESSION:
		{
			p_ptr += sizeof(struct fpppoeframe::pppoe_hdr_t);
			p_len -= sizeof(struct fpppoeframe::pppoe_hdr_t);

			parse_ppp(p_ptr, p_len);
		}
		break;
	default:
		{
			throw eInternalError();
		}
		break;
	}


}



void
cpacket::parse_ppp(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fpppframe::ppp_hdr_t))
	{
		return;
	}

	fpppframe *ppp = new fpppframe(p_ptr, sizeof(struct fpppframe::ppp_hdr_t));

	match.set_ppp_prot(ppp->get_ppp_prot());

	frame_append(ppp);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_ppp() "
			"ppp: %s\nmatch: %s", this, ppp->c_str(), match.c_str());



	switch (match.get_ppp_prot()) {
	case fpppframe::PPP_PROT_IPV4:
		{
			p_ptr += sizeof(struct fpppframe::ppp_hdr_t);
			p_len -= sizeof(struct fpppframe::ppp_hdr_t);

			parse_ipv4(p_ptr, p_len);
		}
		break;
	case fpppframe::PPP_PROT_IPV6:
		{
			p_ptr += sizeof(struct fpppframe::ppp_hdr_t);
			p_len -= sizeof(struct fpppframe::ppp_hdr_t);

			parse_ipv6(p_ptr, p_len);
		}
		break;
	default:
		{
			ppp->unpack(p_ptr, p_len);
		}
		break;
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

	match.set_arp_opcode(arp->get_opcode());
	match.set_arp_spa(arp->get_nw_src());
	match.set_arp_tpa(arp->get_nw_dst());
	match.set_arp_sha(arp->get_dl_src());
	match.set_arp_tha(arp->get_dl_dst());

	frame_append(arp);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_arpv4() "
			"arp: %s\nmatch: %s", this, arp->c_str(), match.c_str());

	p_ptr += sizeof(struct farpv4frame::arpv4_hdr_t);
	p_len -= sizeof(struct farpv4frame::arpv4_hdr_t);

	if (p_len > 0)
	{
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

	if (p_len < sizeof(struct fipv4frame::ipv4_hdr_t))
	{
		return;
	}

	fipv4frame *ip = new fipv4frame(p_ptr, sizeof(struct fipv4frame::ipv4_hdr_t));

	match.set_ip_proto(ip->get_ipv4_proto());
	match.set_ipv4_dst(ip->get_ipv4_dst());
	match.set_ipv4_src(ip->get_ipv4_src());
	match.set_ip_dscp(ip->get_ipv4_dscp());
	match.set_ip_ecn(ip->get_ipv4_ecn());


	frame_append(ip);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_ipv4() "
			"ip: %s\nmatch: %s", this, ip->c_str(), match.c_str());

	p_ptr += sizeof(struct fipv4frame::ipv4_hdr_t);
	p_len -= sizeof(struct fipv4frame::ipv4_hdr_t);

	if (ip->has_MF_bit_set())
	{
		WRITELOG(CFRAME, DBG, "cpacket(%p)::parse_ipv4() "
				"IPv4 fragment found", this);

		return;
	}

	// FIXME: IP header with options



	switch (match.get_ip_proto()) {
	case fipv4frame::IPV4_IP_PROTO:
		{
			parse_ipv4(p_ptr, p_len);
		}
		break;
	case ficmpv4frame::ICMPV4_IP_PROTO:
		{
			parse_icmpv4(p_ptr, p_len);
		}
		break;
	case fipv6frame::IPV6_IP_PROTO:
		{
			parse_ipv6(p_ptr, p_len);
		}
		break;
#if THIS_DOES_NOT_MAKE_SENSE_HERE
	case ficmpv6frame::ICMPV6_IP_PROTO:
		{
			parse_icmpv6(p_ptr, p_len);
		}
		break;
#endif
	case fudpframe::UDP_IP_PROTO:
		{
			parse_udp(p_ptr, p_len);
		}
		break;
	case ftcpframe::TCP_IP_PROTO:
		{
			parse_tcp(p_ptr, p_len);
		}
		break;
	case fsctpframe::SCTP_IP_PROTO:
		{
			parse_sctp(p_ptr, p_len);
		}
		break;
	default:
		{
			if (p_len > 0)
			{
				fframe *payload = new fframe(p_ptr, p_len);

				frame_append(payload);
			}
		}
		break;
	}
}



void
cpacket::parse_icmpv4(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct ficmpv4frame::icmpv4_hdr_t))
	{
		return;
	}

	ficmpv4frame *icmp = new ficmpv4frame(p_ptr, sizeof(struct ficmpv4frame::icmpv4_hdr_t));

	match.set_icmpv4_type(icmp->get_icmp_type());
	match.set_icmpv4_code(icmp->get_icmp_code());

	frame_append(icmp);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_icmpv4() "
			"icmp: %s\nmatch: %s", this, icmp->c_str(), match.c_str());

	p_ptr += sizeof(struct ficmpv4frame::icmpv4_hdr_t);
	p_len -= sizeof(struct ficmpv4frame::icmpv4_hdr_t);


	if (p_len > 0)
	{
		fframe *payload = new fframe(p_ptr, p_len);

		frame_append(payload);
	}
}



void
cpacket::parse_ipv6(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct fipv6frame::ipv6_hdr_t)) // base header
	{
		return;
	}

	fipv6frame *ip = new fipv6frame(p_ptr, sizeof(struct fipv6frame::ipv6_hdr_t));

	match.set_ip_proto(ip->get_next_header());
	match.set_ipv6_dst(ip->get_ipv6_dst());
	match.set_ipv6_src(ip->get_ipv6_src());
	match.set_ipv6_flabel(ip->get_flow_label());


	frame_append(ip);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_ipv6() "
			"ip: %s\nmatch: %s", this, ip->c_str(), match.c_str());

	p_ptr += sizeof(struct fipv6frame::ipv6_hdr_t);
	p_len -= sizeof(struct fipv6frame::ipv6_hdr_t);

	switch (match.get_ip_proto()) {
	case fipv4frame::IPV4_IP_PROTO:
		{
			parse_ipv4(p_ptr, p_len);
		}
		break;
	case fipv6frame::IPV6_IP_PROTO:
		{
			parse_ipv6(p_ptr, p_len);
		}
		break;
	case ficmpv6frame::ICMPV6_IP_PROTO:
		{
			parse_icmpv6(p_ptr, p_len);
		}
		break;
	case fudpframe::UDP_IP_PROTO:
		{
			parse_udp(p_ptr, p_len);
		}
		break;
	case ftcpframe::TCP_IP_PROTO:
		{
			parse_tcp(p_ptr, p_len);
		}
		break;
	case fsctpframe::SCTP_IP_PROTO:
		{
			parse_sctp(p_ptr, p_len);
		}
		break;
	default:
		{
			if (p_len > 0)
			{
				fframe *payload = new fframe(p_ptr, p_len);

				frame_append(payload);
			}
		}
		break;
	}
}



void
cpacket::parse_icmpv6(
		uint8_t *data,
		size_t datalen)
{
	uint8_t 	*p_ptr 		= data;
	size_t 		 p_len 		= datalen;

	if (p_len < sizeof(struct ficmpv6frame::icmpv6_hdr_t))
	{
		return;
	}

	ficmpv6frame *icmp = new ficmpv6frame(p_ptr, sizeof(struct ficmpv6frame::icmpv6_hdr_t));

	match.set_icmpv6_type(icmp->get_icmpv6_type());
	match.set_icmpv6_code(icmp->get_icmpv6_code());

	frame_append(icmp);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_icmpv6() "
			"icmp: %s\nmatch: %s", this, icmp->c_str(), match.c_str());

	p_ptr += sizeof(struct ficmpv6frame::icmpv6_hdr_t);
	p_len -= sizeof(struct ficmpv6frame::icmpv6_hdr_t);


	if (p_len > 0)
	{
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

	if (p_len < sizeof(struct fudpframe::udp_hdr_t))
	{
		return;
	}

	fudpframe *udp = new fudpframe(p_ptr, sizeof(struct fudpframe::udp_hdr_t));

	match.set_udp_dst(udp->get_dport());
	match.set_udp_src(udp->get_sport());

	frame_append(udp);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_udp() "
			"udp: %s\nmatch: %s", this, udp->c_str(), match.c_str());

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

	if (p_len < sizeof(struct ftcpframe::tcp_hdr_t))
	{
		return;
	}

	ftcpframe *tcp = new ftcpframe(p_ptr, sizeof(struct ftcpframe::tcp_hdr_t));

	match.set_tcp_dst(tcp->get_dport());
	match.set_tcp_src(tcp->get_sport());

	frame_append(tcp);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_tcp() "
			"tcp: %s\nmatch: %s", this, tcp->c_str(), match.c_str());

	p_ptr += sizeof(struct ftcpframe::tcp_hdr_t);
	p_len -= sizeof(struct ftcpframe::tcp_hdr_t);


	if (p_len > 0)
	{
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

	if (p_len < sizeof(struct fsctpframe::sctp_hdr_t))
	{
		return;
	}

	fsctpframe *sctp = new fsctpframe(p_ptr, sizeof(struct fsctpframe::sctp_hdr_t));

	match.set_sctp_dst(sctp->get_dport());
	match.set_sctp_src(sctp->get_sport());

	frame_append(sctp);

	WRITELOG(CPACKET, DBG, "cpacket(%p)::parse_sctp() "
			"sctp: %s\nmatch: %s", this, sctp->c_str(), match.c_str());

	p_ptr += sizeof(struct fsctpframe::sctp_hdr_t);
	p_len -= sizeof(struct fsctpframe::sctp_hdr_t);


	if (p_len > 0)
	{
		fframe *payload = new fframe(p_ptr, p_len);

		frame_append(payload);
	}


}






void
cpacket::calc_checksums()
{
	RwLock lock(&ac_rwlock, RwLock::RWLOCK_READ);

	if (flags.test(FLAG_TCP_CHECKSUM))
	{
		tcp()->tcp_calc_checksum(
			ipv4(-1)->get_ipv4_src(),
			ipv4(-1)->get_ipv4_dst(),
			ipv4(-1)->get_ipv4_proto(),
			get_payload_len(tcp())); // second parameter = 0 => up to tail frame
	}

	if (flags.test(FLAG_UDP_CHECKSUM))
	{
		udp()->udp_calc_checksum(
			ipv4(-1)->get_ipv4_src(),
			ipv4(-1)->get_ipv4_dst(),
			ipv4(-1)->get_ipv4_proto(),
			get_payload_len(udp())); // second parameter = 0 => up to tail frame
	}

	if (flags.test(FLAG_IPV4_CHECKSUM))
	{
		ipv4()->ipv4_calc_checksum();
	}

	if (flags.test(FLAG_ICMPV4_CHECKSUM))
	{
		icmpv4()->icmpv4_calc_checksum(get_payload_len(
            icmpv4()));
	}

	if (flags.test(FLAG_PPPOE_LENGTH))
	{
		pppoe()->pppoe_calc_length();
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
	//match.oxmlist.calc_hits(ofmatch.oxmlist, exact_hits, wildcard_hits, missed);
}



unsigned int
cpacket::cnt_vlan_tags()
{
	unsigned int cnt = 0;

	if ((0 == head) || (0 == tail))
	{
		return cnt;
	}

	fframe *frame = head;

	while (0 != frame)
	{
		if (0 != dynamic_cast<fvlanframe*>( frame ))
		{
			cnt++;
		}

		if (0 == frame->next)
		{
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

	if ((0 == head) || (0 == tail))
	{
		return cnt;
	}

	fframe *frame = head;

	while (0 != frame)
	{
		if (0 != dynamic_cast<fmplsframe*>( frame ))
		{
			cnt++;
		}

		if (0 == frame->next)
		{
			break;
		}

		frame = frame->next;
	}

	return cnt;
}



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

#if 0
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
#endif

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





#if 0
	WRITELOG(CFRAME, DBG, "cpacket(%p)::classify() in_port:%d", this, in_port);

	clear();

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

	if ((fvlanframe::VLAN_CTAG_ETHER == ether->get_dl_type()) ||
		(fvlanframe::VLAN_STAG_ETHER == ether->get_dl_type()))
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
										//sizeof(struct fvlanframe::vlan_hdr_t),
										p_len,
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
			if ((fvlanframe::VLAN_CTAG_ETHER != vlan->get_dl_type()) &&
				(fvlanframe::VLAN_STAG_ETHER != vlan->get_dl_type()))
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
										//sizeof(struct fmplsframe::mpls_hdr_t),
										p_len,
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
								p_len, // end of parsing: length is entire remaining packet (think of PPPoE tags!)
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


			oxmlist[OFPXMT_OFB_PPP_PROT] = coxmatch_ofb_ppp_prot(ppp->get_ppp_prot());

			// IPv4 within PPP
			if (ppp->get_ppp_prot() == fpppframe::PPP_PROT_IPV4)
			{
				anchors[PPP_FRAME].push_back(ppp);
				piovec.push_back(ppp);

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

				oxmlist[OFPXMT_OFB_IPV4_DST] 	= coxmatch_ofb_ipv4_dst(be32toh(ipv4->get_ipv4_dst().ca_s4addr->sin_addr.s_addr));
				oxmlist[OFPXMT_OFB_IPV4_SRC] 	= coxmatch_ofb_ipv4_src(be32toh(ipv4->get_ipv4_src().ca_s4addr->sin_addr.s_addr));
				oxmlist[OFPXMT_OFB_IP_PROTO] 	= coxmatch_ofb_ip_proto(ipv4->get_ipv4_proto());
				oxmlist[OFPXMT_OFB_IP_DSCP] 	= coxmatch_ofb_ip_dscp(ipv4->get_ipv4_dscp());
				oxmlist[OFPXMT_OFB_IP_ECN] 		= coxmatch_ofb_ip_ecn(ipv4->get_ipv4_ecn());

			}
			else
			{
				delete ppp;

				ppp = new fpppframe(
										p_ptr,
										p_len,
										total_len,
										pred);

				anchors[PPP_FRAME].push_back(ppp);
				piovec.push_back(ppp);

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
#endif


#if 0


void
cpacket::set_field(coxmatch const& oxm)
{
	switch (oxm.get_oxm_class()) {
	case OFPXMC_OPENFLOW_BASIC:
	{
		switch (oxm.get_oxm_field()) {
		case OFPXMT_OFB_ETH_DST:
			{
				cmacaddr maddr(oxm.oxm_uint48t->value, OFP_ETH_ALEN);
				ether()->set_dl_dst(maddr);
				match.set_eth_dst(maddr);
			}
			break;
		case OFPXMT_OFB_ETH_SRC:
			{
				cmacaddr maddr(oxm.oxm_uint48t->value, OFP_ETH_ALEN);
				ether()->set_dl_src(maddr);
				match.set_eth_src(maddr);
			}
			break;
		case OFPXMT_OFB_ETH_TYPE:
			{
				uint16_t eth_type = oxm.uint16_value();
				ether()->set_dl_type(eth_type);
				match.set_eth_type(eth_type);
			}
			break;
		case OFPXMT_OFB_VLAN_VID:
			{
				uint16_t vid = oxm.uint16_value();
				vlan()->set_dl_vlan_id(vid);
				match.set_vlan_vid(vid);
			}
			break;
		case OFPXMT_OFB_VLAN_PCP:
			{
				uint8_t pcp = oxm.uint8_value();
				vlan()->set_dl_vlan_pcp(pcp);
				match.set_vlan_pcp(pcp);
			}
			break;
		case OFPXMT_OFB_IP_DSCP:
			{
				uint8_t dscp = oxm.uint8_value();
				ipv4()->set_ipv4_dscp(dscp);
				ipv4()->ipv4_calc_checksum();
				match.set_ip_dscp(dscp);
			}
			break;
		case OFPXMT_OFB_IP_ECN:
			{
				uint8_t ecn = oxm.uint8_value();
				ipv4()->set_ipv4_ecn(ecn);
				ipv4()->ipv4_calc_checksum();
				match.set_ip_ecn(ecn);
			}
			break;
		case OFPXMT_OFB_IP_PROTO:
			{
				uint8_t proto = oxm.uint8_value();
				ipv4()->set_ipv4_proto(proto);
				ipv4()->ipv4_calc_checksum();
				match.set_ip_proto(proto);
			}
			break;
		case OFPXMT_OFB_IPV4_SRC:
			{
				caddress src(AF_INET, "0.0.0.0");
				src.ca_s4addr->sin_addr.s_addr = htobe32(oxm.uint32_value());
				ipv4()->set_ipv4_src(src);
				ipv4()->ipv4_calc_checksum();
				match.set_ipv4_src(src);
			}
			break;
		case OFPXMT_OFB_IPV4_DST:
			{
				caddress dst(AF_INET, "0.0.0.0");
				dst.ca_s4addr->sin_addr.s_addr = htobe32(oxm.uint32_value());
				ipv4()->set_ipv4_dst(dst);
				ipv4()->ipv4_calc_checksum();
				match.set_ipv4_dst(dst);
			}
			break;
		case OFPXMT_OFB_TCP_SRC:
			{
				uint16_t port = oxm.uint16_value();
				tcp()->set_sport(port);
				match.set_tcp_src(port);
				tcp()->tcp_calc_checksum(
						ipv4(-1)->get_ipv4_src(),
						ipv4(-1)->get_ipv4_dst(),
						ipv4(-1)->get_ipv4_proto(),
						tcp()->framelen());
				ipv4()->ipv4_calc_checksum();
			}
			break;
		case OFPXMT_OFB_TCP_DST:
			{
				uint16_t port = oxm.uint16_value();
				tcp()->set_dport(port);
				match.set_tcp_dst(port);
				tcp()->tcp_calc_checksum(
						ipv4(-1)->get_ipv4_src(),
						ipv4(-1)->get_ipv4_dst(),
						ipv4(-1)->get_ipv4_proto(),
						tcp()->framelen());
				ipv4()->ipv4_calc_checksum();
			}
			break;
		case OFPXMT_OFB_UDP_SRC:
			{
				uint16_t port = oxm.uint16_value();
				udp()->set_sport(port);
				match.set_udp_src(port);
				udp()->udp_calc_checksum(
						ipv4(-1)->get_ipv4_src(),
						ipv4(-1)->get_ipv4_dst(),
						ipv4(-1)->get_ipv4_proto(),
						udp()->framelen());
				ipv4()->ipv4_calc_checksum();
			}
			break;
		case OFPXMT_OFB_UDP_DST:
			{
				uint16_t port = oxm.uint16_value();
				udp()->set_dport(port);
				match.set_udp_dst(port);
				udp()->udp_calc_checksum(
						ipv4(-1)->get_ipv4_src(),
						ipv4(-1)->get_ipv4_dst(),
						ipv4(-1)->get_ipv4_proto(),
						udp()->framelen());
				ipv4()->ipv4_calc_checksum();
			}
			break;
		case OFPXMT_OFB_SCTP_SRC:
			{
#if 0
				sctp().set_sport(oxm.uint16_value());
				sctp().udp_calc_checksum(
						ipv4(-1).get_ipv4_src(),
						ipv4(-1).get_ipv4_dst(),
						ipv4(-1).get_ipv4_proto(),
						sctp().framelen());
#endif
				ipv4()->ipv4_calc_checksum();
				match.oxmlist[OFPXMT_OFB_SCTP_SRC] = oxm;
			}
			break;
		case OFPXMT_OFB_SCTP_DST:
			{
#if 0
				sctp().set_dport(oxm.uint16_value());
				sctp().udp_calc_checksum(
						ipv4(-1).get_ipv4_src(),
						ipv4(-1).get_ipv4_dst(),
						ipv4(-1).get_ipv4_proto(),
						sctp().framelen());
#endif
				ipv4()->ipv4_calc_checksum();
				match.oxmlist[OFPXMT_OFB_SCTP_DST] = oxm;
			}
			break;
		case OFPXMT_OFB_ICMPV4_TYPE:
			{
				uint16_t type = oxm.uint16_value();
				icmpv4()->set_icmp_type(type);
				icmpv4()->icmpv4_calc_checksum();
				ipv4()->ipv4_calc_checksum();
				match.set_icmpv4_type(type);
			}
			break;
		case OFPXMT_OFB_ICMPV4_CODE:
			{
				uint16_t code = oxm.uint16_value();
				icmpv4()->set_icmp_code(code);
				icmpv4()->icmpv4_calc_checksum();
				ipv4()->ipv4_calc_checksum();
				match.set_icmpv4_code(code);
			}
			break;
		case OFPXMT_OFB_ARP_OP:
			{
				uint16_t opcode = oxm.uint16_value();
				arpv4()->set_opcode(opcode);
				match.set_arp_opcode(opcode);
			}
			break;
		case OFPXMT_OFB_ARP_SPA:
			{
				caddress spa(AF_INET, "0.0.0.0");
				spa.ca_s4addr->sin_addr.s_addr = htobe32(oxm.uint32_value());
				arpv4()->set_nw_src(spa);
				match.set_arp_spa(spa);
			}
			break;
		case OFPXMT_OFB_ARP_TPA:
			{
				caddress tpa(AF_INET, "0.0.0.0");
				tpa.ca_s4addr->sin_addr.s_addr = htobe32(oxm.uint32_value());
				arpv4()->set_nw_dst(tpa);
				match.set_arp_tpa(tpa);
			}
			break;
		case OFPXMT_OFB_ARP_SHA:
			{
				cmacaddr sha(oxm.oxm_uint48t->value, OFP_ETH_ALEN);
				arpv4()->set_dl_src(sha);
				match.set_arp_sha(sha);
			}
			break;
		case OFPXMT_OFB_ARP_THA:
			{
				cmacaddr tha(oxm.oxm_uint48t->value, OFP_ETH_ALEN);
				arpv4()->set_dl_dst(tha);
				match.set_arp_tha(tha);
			}
			break;
		case OFPXMT_OFB_IPV6_SRC:
			{
				caddress addr(sizeof(struct sockaddr_in6));
				addr.ca_s6addr->sin6_family = AF_INET6;
				if (not oxm.get_oxm_hasmask()) {
					memcpy(addr.ca_s6addr->sin6_addr.s6_addr, oxm.oxm_ipv6addr->addr, 16);
				} else {
#if 0
					memcpy(addr.ca_s6addr->sin6_addr.s6_addr,
							oxm.oxm_ipv6addr->addr & oxm.oxm_ipv6addr->mask, 16);
#endif
				}
				ipv6()->set_ipv6_src(addr);
				match.set_ipv6_src(addr);
			}
			break;
		case OFPXMT_OFB_IPV6_DST:
			{
				caddress addr(sizeof(struct sockaddr_in6));
				addr.ca_s6addr->sin6_family = AF_INET6;
				if (not oxm.get_oxm_hasmask()) {
					memcpy(addr.ca_s6addr->sin6_addr.s6_addr, oxm.oxm_ipv6addr->addr, 16);
				} else {
#if 0
					memcpy(addr.ca_s6addr->sin6_addr.s6_addr,
							oxm.oxm_ipv6addr->addr & oxm.oxm_ipv6addr->mask, 16);
#endif
				}
				ipv6()->set_ipv6_dst(addr);
				match.set_ipv6_dst(addr);
			}
			break;
		case OFPXMT_OFB_IPV6_FLABEL:
			{
				if (not oxm.get_oxm_hasmask()) {
					ipv6()->set_flow_label(oxm.uint32_value());
					match.set_ipv6_flabel(oxm.uint32_value());
				} else {
					ipv6()->set_flow_label(oxm.uint32_value() & oxm.uint32_mask());
					match.set_ipv6_flabel(oxm.uint32_value() & oxm.uint32_mask());
				}
			}
			break;
		case OFPXMT_OFB_ICMPV6_TYPE:
			{
				icmpv6()->set_icmpv6_type(oxm.uint8_value());
				match.set_icmpv6_type(oxm.uint8_value());
			}
			break;
		case OFPXMT_OFB_ICMPV6_CODE:
			{
				icmpv6()->set_icmpv6_code(oxm.uint8_value());
				match.set_icmpv6_code(oxm.uint8_value());
			}
			break;
		case OFPXMT_OFB_IPV6_ND_TARGET:
			{
				caddress addr(sizeof(struct sockaddr_in6));
				addr.ca_s6addr->sin6_family = AF_INET6;
				if (not oxm.get_oxm_hasmask()) {
					memcpy(addr.ca_s6addr->sin6_addr.s6_addr, oxm.oxm_ipv6addr->addr, 16);
				} else {
#if 0
					memcpy(addr.ca_s6addr->sin6_addr.s6_addr,
							oxm.oxm_ipv6addr->addr & oxm.oxm_ipv6addr->mask, 16);
#endif
				}
				icmpv6()->set_icmpv6_neighbor_taddr(addr);
				match.set_icmpv6_neighbor_taddr(addr);
			}
			break;
		case OFPXMT_OFB_IPV6_ND_SLL:
			{
				cmacaddr maddr(oxm.oxm_uint48t->value, OFP_ETH_ALEN);
				icmpv6()->get_option(ficmpv6opt::ICMPV6_OPT_LLADDR_SOURCE).set_ll_saddr(maddr);
			}
			break;
		case OFPXMT_OFB_IPV6_ND_TLL:
			{
				cmacaddr maddr(oxm.oxm_uint48t->value, OFP_ETH_ALEN);
				icmpv6()->get_option(ficmpv6opt::ICMPV6_OPT_LLADDR_TARGET).set_ll_taddr(maddr);
			}
			break;
		case OFPXMT_OFB_MPLS_LABEL:
			{
				uint32_t label = oxm.uint32_value();
				mpls()->set_mpls_label(label);
				match.set_mpls_label(label);
			}
			break;
		case OFPXMT_OFB_MPLS_TC:
			{
				uint8_t tc = oxm.uint8_value();
				mpls()->set_mpls_tc(tc);
				match.set_mpls_tc(tc);
			}
			break;
		/*
		 * PPP/PPPoE related extensions
		 */
		case OFPXMT_OFB_PPPOE_CODE:
			{
				uint8_t code = oxm.uint8_value();
				pppoe()->set_pppoe_code(code);
				match.set_pppoe_code(code);
			}
			break;
		case OFPXMT_OFB_PPPOE_TYPE:
			{
				uint8_t type = oxm.uint8_value();
				pppoe()->set_pppoe_type(type);
				match.oxmlist[OFPXMT_OFB_PPPOE_TYPE] = coxmatch_ofb_pppoe_type(type);
			}
			break;
		case OFPXMT_OFB_PPPOE_SID:
			{
				uint16_t sid = oxm.uint16_value();
				pppoe()->set_pppoe_sessid(sid);
				match.set_pppoe_sessid(sid);
			}
			break;
		case OFPXMT_OFB_PPP_PROT:
			{
				uint16_t prot = oxm.uint16_value();
				ppp()->set_ppp_prot(prot);
				match.set_ppp_prot(prot);
			}
			break;
		default:
			{
				WRITELOG(CPACKET, WARN, "cpacket(%p)::set_field() "
						"don't know how to handle class:0x%x field:%d, ignoring",
						this, oxm.get_oxm_class(), oxm.get_oxm_field());
			}
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


#endif
