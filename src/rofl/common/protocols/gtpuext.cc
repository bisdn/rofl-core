/*
 * gtpuext.cc
 *
 *  Created on: 08.07.2013
 *      Author: andreas
 */

#include <rofl/common/protocols/gtpuext.h>

using namespace rofl;

gtpuext::gtpuext(size_t extlen) :
		cmemory(extlen),
		exthdr(0)
{
	exthdr = (struct gtpu_ext_hdr_t*)somem();
}



gtpuext::~gtpuext()
{

}



gtpuext::gtpuext(
		gtpuext const& ext) :
		cmemory(ext.memlen()),
		exthdr(0)
{
	*this = ext;
}



gtpuext&
gtpuext::operator= (
		gtpuext const& ext)
{
	if (this == &ext)
		return *this;

	cmemory::operator= (ext);

	exthdr = (struct gtpu_ext_hdr_t*)somem();

	return *this;
}



gtpuext::gtpuext(uint8_t* buf, size_t buflen) :
		cmemory(buflen),
		exthdr(0)
{
	exthdr = (struct gtpu_ext_hdr_t*)somem();
	memcpy(somem(), buf, buflen);
}



void
gtpuext::pack(uint8_t *buf, size_t buflen) const
{
	cmemory::pack(buf, buflen);
}



void
gtpuext::unpack(uint8_t *buf, size_t buflen)
{
	cmemory::unpack(buf, buflen);
	exthdr = (struct gtpu_ext_hdr_t*)somem();
}



size_t
gtpuext::get_length() const
{
	return (exthdr->extlen * 4);
}



void
gtpuext::set_length(size_t len)
{
	len = (len % 4) ? len : 4 * (len / 4) + 4;
	exthdr->extlen = (len / 4);
}



uint8_t
gtpuext::get_next_hdr_type() const
{
	return (*this)[memlen()-1];
}



void
gtpuext::set_next_hdr_type(uint8_t type)
{
	(*this)[memlen()-1] = type;
}




/*
 * GTP extension: UDP port
 */

gtpuext_udp_port::gtpuext_udp_port(uint16_t udp_port) :
		gtpuext(sizeof(struct gtpu_udp_port_ext_hdr_t)),
		udp_port_exthdr(0)
{
	udp_port_exthdr = (struct gtpu_udp_port_ext_hdr_t*)somem();
	set_length(sizeof(struct gtpu_udp_port_ext_hdr_t));
	set_udp_port(udp_port);
	set_next_hdr_type(0);
}



gtpuext_udp_port::~gtpuext_udp_port()
{

}



gtpuext_udp_port::gtpuext_udp_port(
			gtpuext const& ext) :
		gtpuext(sizeof(struct gtpu_udp_port_ext_hdr_t)),
		udp_port_exthdr(0)
{
	*this = ext;
}



gtpuext_udp_port&
gtpuext_udp_port::operator= (
			gtpuext const& ext)
{
	if (this == &ext)
		return *this;

	gtpuext::operator= (ext);

	udp_port_exthdr = (struct gtpu_udp_port_ext_hdr_t*)somem();

	return *this;
}



gtpuext_udp_port::gtpuext_udp_port(
			uint8_t *buf, size_t buflen) :
			gtpuext(buf, buflen),
			udp_port_exthdr(0)
{
	udp_port_exthdr = (struct gtpu_udp_port_ext_hdr_t*)somem();
}



uint16_t
gtpuext_udp_port::get_udp_port() const
{
	return be16toh(udp_port_exthdr->udpport);
}



void
gtpuext_udp_port::set_udp_port(uint16_t udp_port)
{
	udp_port_exthdr->udpport = htobe16(udp_port);
}








/*
 * GTP extension: PDCP PDU number
 */

gtpuext_pdcp_pdu_number::gtpuext_pdcp_pdu_number(uint16_t pdcp_pdu_number) :
		gtpuext(sizeof(struct gtpu_pdcp_pdu_number_ext_hdr_t)),
		pdcp_pdu_number_exthdr(0)
{
	pdcp_pdu_number_exthdr = (struct gtpu_pdcp_pdu_number_ext_hdr_t*)somem();
	set_length(sizeof(struct gtpu_pdcp_pdu_number_ext_hdr_t));
	set_pdcp_pdu_number(pdcp_pdu_number);
	set_next_hdr_type(0);
}



gtpuext_pdcp_pdu_number::~gtpuext_pdcp_pdu_number()
{

}



gtpuext_pdcp_pdu_number::gtpuext_pdcp_pdu_number(
			gtpuext const& ext) :
		gtpuext(sizeof(struct gtpu_pdcp_pdu_number_ext_hdr_t)),
		pdcp_pdu_number_exthdr(0)
{
	*this = ext;
}



gtpuext_pdcp_pdu_number&
gtpuext_pdcp_pdu_number::operator= (
			gtpuext const& ext)
{
	if (this == &ext)
		return *this;

	gtpuext::operator= (ext);

	pdcp_pdu_number_exthdr = (struct gtpu_pdcp_pdu_number_ext_hdr_t*)somem();

	return *this;
}



gtpuext_pdcp_pdu_number::gtpuext_pdcp_pdu_number(
			uint8_t *buf, size_t buflen) :
			gtpuext(buf, buflen),
			pdcp_pdu_number_exthdr(0)
{
	pdcp_pdu_number_exthdr = (struct gtpu_pdcp_pdu_number_ext_hdr_t*)somem();
}



uint16_t
gtpuext_pdcp_pdu_number::get_pdcp_pdu_number() const
{
	return be16toh(pdcp_pdu_number_exthdr->pdcp_pdu_number);
}



void
gtpuext_pdcp_pdu_number::set_pdcp_pdu_number(uint16_t pdcp_pdu_number)
{
	pdcp_pdu_number &= 0x7fff;
	pdcp_pdu_number_exthdr->pdcp_pdu_number = htobe16(pdcp_pdu_number);
}







/*
 * GTP extension: service class indicator
 */

gtpuext_svc_class_ind::gtpuext_svc_class_ind(uint8_t svc_class_ind) :
		gtpuext(sizeof(struct gtpu_svc_class_ind_ext_hdr_t)),
		svc_class_ind_exthdr(0)
{
	svc_class_ind_exthdr = (struct gtpu_svc_class_ind_ext_hdr_t*)somem();
	set_length(sizeof(struct gtpu_svc_class_ind_ext_hdr_t));
	set_svc_class_ind(svc_class_ind);
	set_next_hdr_type(0);
}



gtpuext_svc_class_ind::~gtpuext_svc_class_ind()
{

}



gtpuext_svc_class_ind::gtpuext_svc_class_ind(
			gtpuext const& ext) :
		gtpuext(sizeof(struct gtpu_svc_class_ind_ext_hdr_t)),
		svc_class_ind_exthdr(0)
{
	*this = ext;
}



gtpuext_svc_class_ind&
gtpuext_svc_class_ind::operator= (
			gtpuext const& ext)
{
	if (this == &ext)
		return *this;

	gtpuext::operator= (ext);

	svc_class_ind_exthdr = (struct gtpu_svc_class_ind_ext_hdr_t*)somem();

	return *this;
}



gtpuext_svc_class_ind::gtpuext_svc_class_ind(
			uint8_t *buf, size_t buflen) :
			gtpuext(buf, buflen),
			svc_class_ind_exthdr(0)
{
	svc_class_ind_exthdr = (struct gtpu_svc_class_ind_ext_hdr_t*)somem();
}



uint8_t
gtpuext_svc_class_ind::get_svc_class_ind() const
{
	return svc_class_ind_exthdr->svc_class_ind;
}



void
gtpuext_svc_class_ind::set_svc_class_ind(uint8_t svc_class_ind)
{
	svc_class_ind_exthdr->svc_class_ind = svc_class_ind;
}



