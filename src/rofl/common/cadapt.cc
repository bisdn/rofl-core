/*
 * cadapt.cc
 *
 *  Created on: 30.08.2012
 *      Author: andreas
 */

#include <rofl/common/cadapt.h>




cadapt::cadapt(
		cadapt_owner *base) throw (eAdaptInval) :
	base(base),
	child(0)
{
	if (0 == base)
	{
		throw eAdaptInval();
	}

	base->adapter_open(this);
}


cadapt::~cadapt()
{
	base->adapter_close(this);
}


const char*
cadapt::c_str()
{
	cvastring vas(64);
	info.assign(vas("cadapter(%p) cadapt_owner:%p", this, base));
	return info.c_str();
}


void
cadapt::adapter_open(
		cadapt* adapt)
{
	child = adapt;
}



void
cadapt::handle_packet_in(
		cofpacket *pack)
{
	if (0 == child) // no child adapter => call this->send_packet_in() directly
	{
		if (not pack->has_data())
		{
			WRITELOG(CFWD, DBG, "cadapt(%p)::handle_packet_in() no payload, dropping => pack: %p",
					this, pack);

			return;
		}

		cpacket n_pack(pack->get_data(), pack->get_datalen(),
				be16toh(pack->ofh_packet_in->total_len));

		this->send_packet_in((cadapt*)0,
				be32toh(pack->ofh_packet_in->buffer_id),
				be16toh(pack->ofh_packet_in->total_len),
				pack->ofh_packet_in->table_id,
				pack->ofh_packet_in->reason,
				pack->match,
				n_pack);
	}
	else // child adapter => hand Packet-In over to child it will call our send_packet_in() later, if necessary
	{
		child->handle_packet_in(pack);
	}
}


void
cadapt::handle_error(
		uint16_t type,
		uint16_t code,
		uint8_t *data,
		size_t datalen)
{
	// TODO
}


void
cadapt::handle_port_status(
		uint8_t reason,
		cofport *port)
{
	if (0 == child)	// no child adapter => call this->send_port_status() directly
	{
		this->send_port_status((cadapt*)0, reason, port);
	}
	else	// child adapter => hand Port-Status over to child it will call our send_port_status() later, if necessary
	{
		child->handle_port_status(reason, port);
	}
}


void
cadapt::adapter_close(
		cadapt* adapt)
{
	child = (cadapt*)0;
}



uint32_t
cadapt::get_free_portno()
		throw (eAdaptNotFound)
{
	return base->get_free_portno();
}


void
cadapt::flowspace_open(
			cadapt *adapt,
			cofmatch const& match)
{
	base->flowspace_open(this, match);
}


void
cadapt::flowspace_close(
			cadapt *adapt,
			cofmatch const& match)
{
	base->flowspace_close(this, match);
}


void
cadapt::send_packet_out(
			cadapt *adapt,
			uint32_t buffer_id,
			uint32_t in_port,
			cofaclist& aclist,
			cpacket& pack)
{
	// directly send PACKET-OUT message via our owner
	base->send_packet_out(this, buffer_id, in_port, aclist, pack);
}
