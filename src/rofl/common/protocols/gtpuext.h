/*
 * gtpuext.h
 *
 *  Created on: 08.07.2013
 *      Author: andreas
 */

#ifndef GTPUEXT_H_
#define GTPUEXT_H_ 1

#include <ostream>

#include <rofl/common/cmemory.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif


namespace rofl
{

class gtpuext :
		public cmemory
{
public:

	// GTP-Uv1 extension header
	struct gtpu_ext_hdr_t {
		uint8_t		extlen;		// length of extension header in bytes
		uint8_t		data[0];	// pointer to start of data
					/* last byte contains next extension header type */
	};

	struct gtpu_ext_hdr_t		*exthdr;

public:


	/**
	 *
	 */
	gtpuext(
			size_t extlen = 0);


	/**
	 *
	 */
	virtual
	~gtpuext();


	/**
	 *
	 */
	gtpuext(
			gtpuext const& ext);


	/**
	 *
	 */
	gtpuext&
	operator= (
			gtpuext const& ext);


	/**
	 *
	 */
	gtpuext(
			uint8_t* buf,
			size_t buflen);


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf, size_t buflen);


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	virtual size_t
	get_length() const;


	/**
	 *
	 */
	virtual void
	set_length(size_t len);


	/**
	 *
	 */
	virtual uint8_t
	get_next_type() const;


	/**
	 *
	 */
	virtual void
	set_next_type(uint8_t type);


public:


	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, gtpuext const& ext)
	{
		os << "gtpuext{"
				<< "length=" << (unsigned int)ext.get_length() << " "
				<< "nexthdr=" << (unsigned int)ext.get_next_type() << " "
			<< "}";
		return os;
	};
};



/**
 * GTP extension: UDP port
 */
class gtpuext_udp_port :
		public gtpuext
{
public:

	struct gtpu_udp_port_ext_hdr_t {
		uint8_t 	len;
		uint16_t 	udpport;
		uint8_t 	nhtype;
	};

	struct gtpu_udp_port_ext_hdr_t 	*udp_port_exthdr;

public:

	/**
	 *
	 */
	gtpuext_udp_port(
			uint16_t udp_port = 0);


	/**
	 *
	 */
	virtual
	~gtpuext_udp_port();


	/**
	 *
	 */
	gtpuext_udp_port(
			gtpuext const& ext);


	/**
	 *
	 */
	gtpuext_udp_port&
	operator= (
			gtpuext const& ext);


	/**
	 *
	 */
	gtpuext_udp_port(
			uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	uint16_t
	get_udp_port() const;


	/**
	 *
	 */
	void
	set_udp_port(uint16_t udp_port);


public:


	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, gtpuext_udp_port const& ext)
	{
		os << "gtpuext_udp_port{"
				<< "length=" << (unsigned int)ext.get_length() << " "
				<< "udpport=" << (unsigned int)ext.get_udp_port() << " "
				<< "nexthdr=" << (unsigned int)ext.get_next_type() << " "
			<< "}";
		return os;
	};
};


}; // end of namespace

#endif /* GTPUEXT_H_ */
