/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CPACKET_H
#define CPACKET_H 1

#include <utility>
#include <iostream>
#include <string>
#include <deque>
#include <list>
#include <set>
#include <algorithm>
#include <typeinfo>

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <sys/uio.h>
#include <assert.h>


#include "openflow/openflow12.h"
#ifdef __cplusplus
}
#endif

#include "cerror.h"
#include "cclock.h"
#include "fframe.h"

#include "cmacaddr.h"
#include "cmemory.h"
#include "cvastring.h"
#include "thread_helper.h"

/* Platform dependant */
#include "rofl/platform/unix/csyslog.h"

/* Openflow stuff */
#include "openflow/cofmatch.h"
#include "openflow/cofaction.h"

/* Protocol stuff */
#include "protocols/fetherframe.h"
#include "protocols/fvlanframe.h"
#include "protocols/fmplsframe.h"
#include "protocols/fpppoeframe.h"
#include "protocols/fpppframe.h"
#include "protocols/fipv4frame.h"
#include "protocols/ficmpv4frame.h"
#include "protocols/farpv4frame.h"
#include "protocols/fudpframe.h"
#include "protocols/ftcpframe.h"
#include "protocols/fetherframe.h"

/* Forward declarations */
class fetherframe;
class fvlanframe;
class fmplsframe;
class fpppoeframe;
class fpppframe;
class fipv4frame;
class ficmpv4frame;
class farpv4frame;
class fudpframe;
class ftcpframe;
class fetherframe;



class ePacket 				: public cerror {}; // base error class for cpacket
class ePacketInval 			: public ePacket {}; // invalid
class ePacketOutOfMem 		: public ePacket {}; // out of memory
class ePacketNotFound 		: public ePacket {}; // value not found
class ePacketOutOfRange 	: public ePacket {}; // index out of range for operator[]
class ePacketTypeError 		: public ePacket {}; // specified frame type not found in packet




/**
 *
 *
 *
 */
class cpacket :
	public csyslog
{
/*
 * data structures
 */
private:


		std::bitset<32> 				flags;			// flags (e.g. for checksum calculations, NO_PACKET_IN, etc.)
		std::string 					info;			// info string
		pthread_rwlock_t 				ac_rwlock;		// rwlock for anchors

		enum cpacket_flag_t {
			FLAG_PPPOE_LENGTH   		= 1,
			FLAG_IPV4_CHECKSUM  		= 2,
			FLAG_ICMPV4_CHECKSUM  		= 3,
			FLAG_UDP_CHECKSUM   		= 4,
			FLAG_TCP_CHECKSUM   		= 5,
			FLAG_NO_PACKET_IN			= 6,
			FLAG_VLAN_PRESENT			= 7,
			FLAG_MPLS_PRESENT			= 8,
		};


#define CPACKET_DEFAULT_SIZE 			 1526
#define CPACKET_DEFAULT_HSPACE			 64			// head room for push operations
#define CPACKET_DEFAULT_TSPACE			 256		// tail room for appending payload(s)


		fframe							*head;		// head of all frames
		fframe							*tail;		// tail of all frames

		size_t							 hspace;	// head space size: this is used as extra space for pushing tags
		size_t							 tspace;	// tail space size: this is used as extra space for appending payload(s)
		cmemory 						 mem;		// packet data + head space
		std::pair<uint8_t*, size_t>		 data;		// the packet data: defines iov of packet data within cmemory mem
													// we have also some additional headspace



public:


		cofmatch						 match;		// packet header fields stored in ofmatch

		time_t 				packet_receive_time;	// time this packet was received
		uint32_t 			in_port;				// incoming port
		uint32_t			out_port;				// outgoing port when stored within cpktqueue, 0 otherwise


#if 1
		cclock 							time_cport_recv;
		cclock 							time_cdpath_in;
		cclock 							time_cfwdengine_in;
		cclock 							time_cfwdengine_out;
		cclock 							time_cdpath_out;
		cclock 							time_cport_enqueue;
		cclock 							time_cport_send;
#endif




/*
 * methods
 */
public: // static methods


	/** returns an empty packet
	 *
	 */
	static cpacket
	pempty();


public: // methods


	/** constructor for creating new empty packets
	 *
	 */
	cpacket(
			size_t size = CPACKET_DEFAULT_SIZE,
			uint32_t in_port = 0,
			bool do_classify = false);


	/** constructor for storing a memory area specified by (buf, buflen)
	 *  in a cpacket
	 */
	cpacket(
			uint8_t *buf, size_t buflen,
			uint32_t in_port = 0 /*invalid port in OF*/,
			bool do_classify = true);


	/** constructor for storing a memory area stored in cmemory mem
	 *  in a cpacket, mem must not be freed after creating the cpacket instance
	 */
	cpacket(
			cmemory *mem,
			uint32_t in_port = 0 /*invalid port in OF*/,
			bool do_classify = true);


	/** copy constructor
	 *
	 */
	cpacket(
			cpacket const& pack);


	/** destructor
	 */
	virtual
	~cpacket();


public:


	/** assignment operator
	 */
	cpacket&
	operator=(
			cpacket const& p);


	/** return reference to byte at index
	 *
	 */
	uint8_t&
	operator[] (
			size_t index) throw (ePacketOutOfRange);


	/**
	 *
	 */
	bool
	operator== (
			cpacket const& p);


	/**
	 *
	 */
	bool
	operator== (
			cmemory const& m);


	/**
	 *
	 */
	bool
	operator!= (
			cpacket const& p);


	/**
	 *
	 */
	bool
	operator!= (
			cmemory const& m);


	/**
	 *
	 */
	cpacket&
	operator+ (
			fframe const& f);


	/**
	 *
	 */
	void
	operator+= (
			fframe const& f);


	/**
	 *
	 */
	void
	set_flag_no_packet_in();


	/**
	 *
	 */
	bool
	get_flag_no_packet_in();


	/**
	 *
	 */
	uint8_t*
	soframe() const;


	/**
	 *
	 */
	size_t
	framelen() const;



	/**
	 *
	 */
	bool
	empty() const;



	/** copies all piobufs into memory specified by (dest, len)
	 *
	 */
	void
	pack(
			uint8_t *dest,
			size_t len) throw (ePacketInval);


	/**  copies buffer specified by (src, len) into single piobuf
	 *
	 */
	void
	unpack(
			uint32_t in_port,
			uint8_t *src,
			size_t len);


	/**  copies buffer specified by (src, len) into single piobuf
	 *
	 */
	void
	unpack(
			uint32_t in_port);


	/** returns total length of all piobufs
	 *
	 */
	size_t
	length();


	/** insert space with length 'len' at offset
	 *
	 */
	uint8_t*
	tag_insert(
			size_t len) throw (ePacketOutOfRange);


	/** remove 'len' bytes at offset
	 *
	 */
	void
	tag_remove(
			fframe *frame) throw (ePacketOutOfRange);


	/** dump packet
	 */
	virtual const char*
	c_str();


	/** output stream operator
	 *
	 */
	friend std::ostream &
	operator<<(std::ostream& os, const cpacket& cpack)
	{
		std::cerr << "XXX" << std::endl;
		os << "[";
		os << "]";
		std::cerr << "XXX" << std::endl;
		return os;
	};


	/** calculate hit-rate for cofmatch
	 */
	void calc_hits(
			cofmatch& ofmatch,
			uint16_t& exact_hits,
			uint16_t& wildcard_hits,
			uint16_t& missed);


	/** parse and classify data frame
	 *
	 */
	void
	classify(
			uint32_t in_port /* host byte order */);


private:

#if 0
	/** allocate consecutive memory buffer
	 */

	virtual const char*
	c_str();
#endif


	/**
	 *
	 */
	void
	reset();


public:


	/**
	 *
	 */
	fframe*
	frame(
			int i = 0) throw (ePacketNotFound);


	/** return pointer to this->ether frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fetherframe*
	ether(
			int i = 0) throw (ePacketNotFound);

	/** return pointer to outer vlan tag
	 *
	 */
	fvlanframe*
	vlan(
			int i = 0) throw (ePacketNotFound);


	/** return pointer to this->ether frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fpppoeframe*
	pppoe(
			int i = 0) throw (ePacketNotFound);

	/** return pointer to this->ether frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fpppframe*
	ppp(
			int i = 0) throw (ePacketNotFound);


	/** return pointer to this->ether frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fmplsframe*
	mpls(
			int i = 0) throw (ePacketNotFound);

	/** return pointer to this->arpv4 frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	farpv4frame*
	arpv4(
			int i = 0) throw (ePacketNotFound);

	/** return pointer to this->ipv4 frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fipv4frame*
	ipv4(
			int i = 0) throw (ePacketNotFound);

	/** return pointer to this->icmpv4 frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	ficmpv4frame*
	icmpv4(
			int i = 0) throw (ePacketNotFound);

	/** return pointer to this->udp frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fudpframe*
	udp(
			int i = 0) throw (ePacketNotFound);

	/** return pointer to this->tcp frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	ftcpframe*
	tcp(
			int i = 0) throw (ePacketNotFound);

	// TODO: SCTP


	/** return pointer to a payload frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fframe*
	payload(
			int i = 0) throw (ePacketNotFound);



	/** recalculates checksums
	 *
	 */
	void
	calc_checksums();


	/**
	 *
	 */
	unsigned int
	cnt_vlan_tags();


	/**
	 *
	 */
	unsigned int
	cnt_mpls_tags();


public:


	/**
	 */
	void
	set_field(
			coxmatch const& oxm);


	/** copy ttl out
	 */
	void
	copy_ttl_out();

	/** copy ttl in
	 */
	void
	copy_ttl_in();


	/** set mpls ttl
	 */
	void
	set_mpls_ttl(
			uint8_t mpls_ttl);

	/** decrement mpls ttl
	 */
	void
	dec_mpls_ttl();

	/** push vlan
	 */
	void
	push_vlan(
			uint16_t ethertype);

	/** pop vlan
	 */
	void
	pop_vlan();

	/** push mpls
	 */
	void
	push_mpls(
			uint16_t ethertype);

	/** pop mpls
	 */
	void
	pop_mpls(
			uint16_t ethertype);

	/** set nw ttl
	 */
	void
	set_nw_ttl(
			uint8_t nw_ttl);

	/** decrement nw ttl
	 */
	void
	dec_nw_ttl();


	/** push pppoe
	 */
	void
	push_pppoe(
			uint16_t ethertype);

	/** pop pppoe
	 */
	void
	pop_pppoe(
			uint16_t ethertype);


	/** push ppp
	 */
	void
	push_ppp(
			uint16_t code);

	/** pop pppoe
	 */
	void
	pop_ppp();


public: // action related methods

	/**
	 * action entry point
	 */
	void
	handle_action(
		cofaction& action);

	/**
	 * action: set field
	 */
	void action_set_field(
		cofaction& action);


	/**
	 * action: set copy ttl out
	 */
	void action_copy_ttl_out(
		cofaction& action);


	/**
	 * action: set copy ttl in
	 */
	void action_copy_ttl_in(
		cofaction& action);


	/**
	 * action: set mpls ttl
	 */
	void action_set_mpls_ttl(
		cofaction& action);


	/**
	 * action: dec mpls ttl
	 */
	void action_dec_mpls_ttl(
		cofaction& action);


	/**
	 * action: push vlan
	 */
	void action_push_vlan(
		cofaction& action);


	/**
	 * action: pop vlan
	 */
	void action_pop_vlan(
		cofaction& action);


	/**
	 * action: push mpls
	 */
	void action_push_mpls(
		cofaction& action);


	/**
	 * action: pop mpls
	 */
	void action_pop_mpls(
		cofaction& action);


	/**
	 * action: set nw ttl
	 */
	void action_set_nw_ttl(
		cofaction& action);


	/**
	 * action: dec nw ttl
	 */
	void action_dec_nw_ttl(
		cofaction& action);


	/**
	 * action: push pppoe
	 */
	void action_push_pppoe(
		cofaction& action);


	/**
	 * action: pop pppoe
	 */
	void action_pop_pppoe(
		cofaction& action);



	/**
	 * action: push ppp
	 */
	void action_push_ppp(
		cofaction& action);


	/**
	 * action: pop pppoe
	 */
	void action_pop_ppp(
		cofaction& action);




private: // methods


	/**
	 *
	 */
	void
	mem_resize(
			size_t size);


	/**
	 *
	 */
	void
	frame_append(
			fframe *frame);



	/**
	 *
	 */
	void
	frame_push(
			fframe *frame);



	/**
	 *
	 */
	void
	frame_pop(
			fframe *frame)
					throw (ePacketInval);



	/**
	 *
	 */
	void
	parse_ether(
			uint8_t *data,
			size_t datalen);



	/**
	 *
	 */
	void
	parse_vlan(
			uint8_t *data,
			size_t datalen);



	/**
	 *
	 */
	void
	parse_mpls(
			uint8_t *data,
			size_t datalen);


	/**
	 *
	 */
	void
	parse_pppoe(
			uint8_t *data,
			size_t datalen);


	/**
	 *
	 */
	void
	parse_ppp(
			uint8_t *data,
			size_t datalen);


	/**
	 *
	 */
	void
	parse_arpv4(
			uint8_t *data,
			size_t datalen);


	/**
	 *
	 */
	void
	parse_ipv4(
			uint8_t *data,
			size_t datalen);


	/**
	 *
	 */
	void
	parse_icmpv4(
			uint8_t *data,
			size_t datalen);


	/**
	 *
	 */
	void
	parse_udp(
			uint8_t *data,
			size_t datalen);


	/**
	 *
	 */
	void
	parse_tcp(
			uint8_t *data,
			size_t datalen);


	/**
	 *
	 */
	void
	parse_sctp(
			uint8_t *data,
			size_t datalen);



public: // static methods


	/**
	 *
	 */
	static void
	test();
};

#endif
