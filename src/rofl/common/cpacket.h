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
#include "openflow/coxmlist.h"
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
private: // static

//#define CPACKET_DEFAULT_SIZE 1518
#define CPACKET_DEFAULT_SIZE 1526


public: // data structures


		fframe							*head;		// head of all frames
		fframe							*tail;		// tail of all frames

		cmemory 						 mem;		// packet data
		cofmatch						 match;		// packet header fields stored in ofmatch

		enum cpacket_frametype_t {
			ETHER_FRAME = 0,
			VLAN_FRAME,
			MPLS_FRAME,
			PPPOE_FRAME,
			PPP_FRAME,
			IPV4_FRAME,
			ICMPV4_FRAME,
			ARPV4_FRAME,
			UDP_FRAME,
			TCP_FRAME,
			SCTP_FRAME,
			MAX_FRAME,
		};

private: // data structures


	std::bitset<32> 		flags;			// flags (e.g. for checksum calculations, NO_PACKET_IN, etc.)
	std::string 			info;			// info string
	pthread_rwlock_t 		ac_rwlock;		// rwlock for anchors

	enum cpacket_flag_t {
		FLAG_PPPOE_LENGTH   	= 1,
		FLAG_IPV4_CHECKSUM  	= 2,
		FLAG_ICMPV4_CHECKSUM  	= 3,
		FLAG_UDP_CHECKSUM   	= 4,
		FLAG_TCP_CHECKSUM   	= 5,
		FLAG_NO_PACKET_IN		= 6,
		FLAG_VLAN_PRESENT		= 7,
		FLAG_MPLS_PRESENT		= 8,
	};


public: // data structures

	time_t 		packet_receive_time;	// time this packet was received
	uint32_t 	in_port;				// incoming port
	uint32_t	out_port;				// outgoing port when stored within cpktqueue, 0 otherwise


#if 1
	cclock time_cport_recv;
	cclock time_cdpath_in;
	cclock time_cfwdengine_in;
	cclock time_cfwdengine_out;
	cclock time_cdpath_out;
	cclock time_cport_enqueue;
	cclock time_cport_send;
#endif

public: // methods

	/** constructor for creating new empty packets
	 *
	 */
	cpacket(
			size_t size = CPACKET_DEFAULT_SIZE);

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
			const cpacket& pack);

	/** destructor
	 */
	virtual
	~cpacket();

	/** assignment operator
	 */
	cpacket&
	operator=(
			const cpacket &p);

	/** return reference to byte at index
	 *
	 */
	uint8_t&
	operator[] (
			size_t index) throw (ePacketOutOfRange);


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
	void
	clear();

	/** copies all piobufs into a newly created cmemory instance
	 *
	 */
	cmemory*
	to_mem();

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
			uint32_t in_port,
			cmemory *mem);

	/** returns total length of all piobufs
	 *
	 */
	size_t
	length();

	/** insert space with length 'len' at offset
	 *
	 */
	uint8_t*
	insert(
			size_t offset,
			size_t len) throw (ePacketOutOfRange);

	/** remove 'len' bytes at offset
	 *
	 */
	uint8_t*
	remove(
			size_t offset,
			size_t len) throw (ePacketOutOfRange);


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
	 * fills in pseudo header structure
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
	cleanup();

public:

	/** return pointer to this->ether frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fetherframe&
	ether(
			int i = 0) throw (ePacketTypeError);

	/** return pointer to outer vlan tag
	 *
	 */
	fvlanframe&
	vlan(
			int i = 0) throw (ePacketTypeError);


	/** return pointer to this->ether frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fpppoeframe&
	pppoe(
			int i = 0) throw (ePacketTypeError);

	/** return pointer to this->ether frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fpppframe&
	ppp(
			int i = 0) throw (ePacketTypeError);


	/** return pointer to this->ether frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fmplsframe&
	mpls(
			int i = 0) throw (ePacketTypeError);

	/** return pointer to this->arpv4 frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	farpv4frame&
	arpv4(
			int i = 0) throw (ePacketTypeError);

	/** return pointer to this->ipv4 frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fipv4frame&
	ipv4(
			int i = 0) throw (ePacketTypeError);

	/** return pointer to this->icmpv4 frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	ficmpv4frame&
	icmpv4(
			int i = 0) throw (ePacketTypeError);

	/** return pointer to this->udp frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	fudpframe&
	udp(
			int i = 0) throw (ePacketTypeError);

	/** return pointer to this->tcp frame
	 * keep in mind: valid only while this instance is alive!
	 *
	 */
	ftcpframe&
	tcp(
			int i = 0) throw (ePacketTypeError);

	// TODO: SCTP


	/** recalculates checksums
	 *
	 */
	void
	calc_checksums();


public:


	/**
	 */
	void
	set_field(
			coxmatch const& oxm);


	/** set dl_src
	 */
	void
	dl_set_dl_src(
			uint8_t *data,
			size_t datalen);


	/** set dl_src
	 */
	void
	dl_set_dl_dst(
			uint8_t *data,
			size_t datalen);


	/** set VLAN vid
	 */
	void
	vlan_set_vid(
			uint16_t vid);


	/** set VLAN pcp
	 */
	void
	vlan_set_pcp(
			uint8_t pcp);

	/** set network source address
	 */
	void
	nw_set_nw_src(
			uint32_t nw_src);

	/** set network source address
	 */
	void
	nw_set_nw_dst(
			uint32_t nw_dst);

	/** set network tos
	 */
	void
	nw_set_nw_dscp(
			uint8_t nw_dscp);

	/** set network tos
	 */
	void
	nw_set_nw_ecn(
			uint8_t nw_ecn);

	/** set transport protocol src port
	 */
	void
	tp_set_tp_src(
			uint16_t port);

	/** set transport protocol src port
	 */
	void
	tp_set_tp_dst(
			uint16_t port);

	/** copy ttl out
	 */
	void
	copy_ttl_out();

	/** copy ttl in
	 */
	void
	copy_ttl_in();

	/** set mpls label
	 */
	void
	set_mpls_label(
			uint32_t mpls_label);

	/** set mpls tc
	 */
	void
	set_mpls_tc(
			uint8_t mpls_tc);

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

	/** set pppoe type
	 */
	void
	set_pppoe_type(
			uint8_t type);

	/** set pppoe code
	 */
	void
	set_pppoe_code(
			uint8_t code);

	/** set pppoe sessid
	 */
	void
	set_pppoe_sessid(
			uint16_t sessid);

	/** push ppp
	 */
	void
	push_ppp(
			uint16_t code);

	/** pop pppoe
	 */
	void
	pop_ppp();

	/** set pppoe code
	 */
	void
	set_ppp_prot(
			uint16_t prot);


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
	frame_append(
			fframe *frame);




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
