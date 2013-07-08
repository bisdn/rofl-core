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


#include "openflow/openflow.h"
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
#include "protocols/fipv6frame.h"
#include "protocols/ficmpv6frame.h"
#include "protocols/farpv4frame.h"
#include "protocols/fudpframe.h"
#include "protocols/ftcpframe.h"
#include "protocols/fsctpframe.h"
#include "protocols/fetherframe.h"
#include "protocols/fgtpuframe.h"

namespace rofl
{


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
class fsctpframe;
class fetherframe;
class fipv6frame;
class ficmpv6frame;
class fgtpuframe;



class ePacket 				: public cerror {}; // base error class for cpacket
class ePacketInval 			: public ePacket {}; // invalid
class ePacketOutOfMem 		: public ePacket {}; // out of memory
class ePacketNotFound 		: public ePacket {}; // value not found
class ePacketOutOfRange 	: public ePacket {}; // index out of range for operator[]
class ePacketTypeError 		: public ePacket {}; // specified frame type not found in packet




/**
 * @class	cpacket
 * @brief	A class for storing, querying, and manipulating data packets.
 *
 * cpacket is a container for storing data packets. It follows OpenFlow's convention
 * of Ethernet frames, i.e. a packet's payload starts with an Ethernet frame always.
 * A cpacket instance may contain full-size packets or shortened versions (e.g. used
 * in OpenFlow PACKET-IN messages). cpacket's length() method always returns the
 * actual size of the available amount of user data (probably shortened) stored in cpacket. Check the total_len
 * parameter from cofmsg_packet_in to acquire the true length of the packet stored
 * at the data path element. cpacket is used also in PACKET-OUT messages for
 * sending arbitrarily formed Ethernet frames out towards a data path element.
 *
 * @see cofmsg_packet_in
 * @see cofmsg_packet_out
 *
 * cpacket contains a classifier for parsing a packet's content and creates a match
 * structure during the parse process.
 * Note, that this match structure may differ from the one seen in PACKET-IN
 * messages and is locally created upon reception of the PACKET-IN from the control connection.
 * Use the match structure from cofmsg_packet_in to get the view received from the data path element.
 *
 * @see cofmatch
 *
 * The parser also creates a set of helper classes to simplify access and manipulation of
 * the packet's content. So-called protocol specific frame classes interpret parts of
 * the packet and allow direct access to protocol header fields. Unrecognized payload is
 * referred to by a protocol agnostic fframe instance.
 *
 * @see fframe
 * @see fetherframe
 * @see fvlanframe
 * @see fmplsframe
 * @see farpv4frame
 * @see fipv4frame
 * @see ficmpv4frame
 * @see fipv6frame
 * @see ficmpv6frame
 * @see fudpframe
 * @see ftcpframe
 * @see fsctpframe
 *
 */
class cpacket :
	public csyslog
{
private:

		static bool						cpacket_init;
		static std::string				s_cpacket_info;
		static pthread_rwlock_t			cpacket_lock;
		static std::set<cpacket*> 		cpacket_list;

		std::bitset<32> 				flags;			// flags (e.g. for checksum calculations, NO_PACKET_IN, etc.)
		std::string 					info;			// info string
		std::string						d_info;			// data info string (for use by method data_c_str()
		pthread_rwlock_t 				ac_rwlock;		// rwlock for anchors
		uint16_t						total_len;		// total_len of packet (for shortened frames in Packet-In)

		enum cpacket_flag_t {
			FLAG_PPPOE_LENGTH   		= 1,
			FLAG_IPV4_CHECKSUM  		= 2,
			FLAG_ICMPV4_CHECKSUM  		= 3,
			FLAG_UDP_CHECKSUM   		= 4,
			FLAG_TCP_CHECKSUM   		= 5,
			FLAG_NO_PACKET_IN			= 6,
			FLAG_VLAN_PRESENT			= 7,
			FLAG_MPLS_PRESENT			= 8,
			FLAG_SCTP_CHECKSUM			= 9,
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
		cofmatch						 match;		// packet header fields stored in ofmatch


public:

		/**
		 * @name Deprecated
		 *
		 * These are deprecated variables and methods. To be removed in the next version.
		 */

		/**@{*/

		time_t 				packet_receive_time;	// time this packet was received
		uint32_t 			in_port;				// incoming port

		/**
		 * @brief	Set/Reset NO-PACKET-IN flag for this cpacket instance.
		 *
		 */
		void
		set_flag_no_packet_in(bool no_packet_in = true);



		/**
		 * @brief	Returns NO-PACKET-IN flag for this cpacket instance.
		 *
		 * @return true: NO-PACKET-IN is set, false otherwise
		 */
		bool
		get_flag_no_packet_in();




#if 0
		// used by second generation adpd data path implementation for profiling
		cclock 							time_cport_recv;
		cclock 							time_cdpath_in;
		cclock 							time_cfwdengine_in;
		cclock 							time_cfwdengine_out;
		cclock 							time_cdpath_out;
		cclock 							time_cport_enqueue;
		cclock 							time_cport_send;
#endif

		/**@}*/


public: // methods


	/**
	 * @brief	Constructor. Creates an empty packet with default size of 1526 bytes.
	 *
	 * @param size size of new packet to be created
	 * @param in_port port this packet was received on
	 * @param do_classify run the classifier during construction
	 */
	cpacket(
			size_t size = CPACKET_DEFAULT_SIZE,
			uint32_t in_port = 0,
			bool do_classify = false);



	/**
	 * @brief	Constructor. Creates a new packet and stores the specified memory area in it.
	 *
	 * @param buf pointer to start of memory area to be stored
	 * @param buflen length of memora area to be stored
	 * @param in_port port this packet was received on
	 * @param do_classify run the classifier during construction
	 */
	cpacket(
			uint8_t *buf, size_t buflen,
			uint32_t in_port = 0 /*invalid port in OF*/,
			bool do_classify = true);



	/**
	 * @brief	Constructor. Creates a new packet and stores the specified memory area in it.
	 *
	 * @param mem pointer to cmemory instance containing the memory area to be stored
	 * @param in_port port this packet was received on
	 * @param do_classify run the classifier during construction
	 */
	cpacket(
			cmemory *mem,
			uint32_t in_port = 0 /*invalid port in OF*/,
			bool do_classify = true);



	/**
	 * @brief	Copy constructor.
	 *
	 * @param pack cpacket instance to be copied
	 */
	cpacket(
			cpacket const& pack);




	/**
	 * @brief	Destructor.
	 *
	 */
	virtual
	~cpacket();



	/**
	 * @brief	Returns a C-string containing information about this cpacket instance.
	 *
	 * @return C-string
	 */
	const char*
	c_str();



	/**
	 * @brief	Clears content of this cpacket instance. Sets memory area to 0 length.
	 *
	 */
	void
	clear();



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



public:

	/**
	 * @name	Operators
	 *
	 * cpacket provides a number of operators for assignment of cpacket instances,
	 * direct access to a packet's content, comparison with other packets, and concatenating
	 * multiple packets.
	 */

	/**@{*/

	/**
	 * @brief	Assignment operator.
	 *
	 * @param p cpacket instance to be assigned
	 */
	cpacket&
	operator=(
			cpacket const& p);



	/**
	 * @brief	Index operator. Returns reference to byte at index.
	 *
	 * This method grants direct access to a packet's content.
	 * (*this)[0] refers to the first byte of the Ethernet header,
	 * (*this)[length()-1] refers to the last byte of the payload.
	 *
	 * @param index the index of byte to be retrieved from cpacket
	 */
	uint8_t&
	operator[] (
			size_t index);



	/**
	 * @brief	Comparison operator for cpacket instance.
	 *
	 * @param p cpacket instance to be compared
	 */
	bool
	operator== (
			cpacket const& p);



	/**
	 * @brief	Comparison operator for cmemory instance.
	 *
	 * @param m cmemory instance to be compared
	 */
	bool
	operator== (
			cmemory const& m);



	/**
	 * @brief	Comparison operator for cpacket instance (unequal).
	 *
	 * @param p cpacket instance to be compared
	 */
	bool
	operator!= (
			cpacket const& p);



	/**
	 * @brief	Comparison operator for cmemory instance (unequal).
	 *
	 * @param m cmemory instance to be compared
	 */
	bool
	operator!= (
			cmemory const& m);


	// FIXME: fix the append operators


	/**
	 * @brief	Appends an fframe instance to this cpacket instance.
	 *
	 * @param f fframe instance to be appended
	 */
	cpacket&
	operator+ (
			fframe const& f);



	/**
	 * @brief	Appends an fframe instance to this cpacket instance.
	 *
	 * @param f fframe instance to be appended
	 */
	void
	operator+= (
			fframe const& f);

	/**@}*/


public:



	/**
	 * @name	Management methods for querying cpacket's properties
	 */

	/**@{*/


	/**
	 * @brief	Returns reference to cpacket's internal cofmatch instance.
	 *
	 * @return cpacket's internal cofmatch instance
	 */
	cofmatch&
	get_match() { return match; };



	/**
	 * @brief	Re-calculates all checksums within packet headers (IP, UDP, TCP, ...)
	 *
	 */
	void
	calc_checksums();



	/**
	 * @brief	Returns number of VLAN tags found in packet.
	 *
	 */
	unsigned int
	cnt_vlan_tags();



	/**
	 * @brief	Returns number of MPLS tags found in packet.
	 *
	 */
	unsigned int
	cnt_mpls_tags();


	/**@{*/



public:


	/**
	 * @brief	Returns start of memory area containing the packet.
	 *
	 * @return pointer to start of memory area
	 */
	uint8_t*
	soframe() const;



	/**
	 * @brief	Returns length of memory area containing the packet.
	 *
	 * @return length of memory area
	 */
	size_t
	framelen() const;




	/**
	 * @brief	Checks for an empty packet.
	 *
	 * @return true: length of memory area is 0, false otherwise
	 */
	bool
	empty() const;




	/**
	 * @brief	Copies cpacket's internal memory area into specified buffer.
	 *
	 * @param dest pointer to start of memory area serving as destination buffer
	 * @param len length of memory area serving as destination buffer
	 */
	void
	pack(
			uint8_t *dest,
			size_t len);



	/**
	 * @brief	Copies the specified buffer into cpacket's internal memory area and calls cpacket's classifier.
	 *
	 * @param in_port the OpenFlow in_port for this packet
	 * @param src pointer to start of memory area serving as source buffer
	 * @param len length of memory area serving as source buffer
	 */
	void
	unpack(
			uint32_t in_port,
			uint8_t *src,
			size_t len);



	/**
	 * @brief	Returns the overall length of cpacket. This may differ from total_len in OpenFlow!
	 *
	 * @return overall packet length stored in cpacket, not total_len
	 */
	size_t
	length();




public:


	/**
	 * @name	Frame related methods
	 *
	 * cpacket maintains a doubly-linked list of protocol specific frame classes, once
	 * the packet has been classified. These protocol specific frames can be used to
	 * query or set fields within the supported protocol headers. Within a packet, multiple
	 * headers (tags) of the same type may exist, e.g. multiple stacked MPLS tags.
	 * cpacket's interface allows direct access to all found tags by specifying a separate
	 * index value. Positive indices start at index 0 (fist frame from left side).
	 * Negative indices start at index -1 (first frame from right side). A data packet
	 * typically contains some (unparsed) payload, which is referenced to by a
	 * protocol agnostic fframe instance.
	 *
	 * @see fetherframe
	 * @see fvlanframe
	 * @see fmplsframe
	 * @see farpv4frame
	 * @see fipv4frame
	 * @see fipv6frame
	 * @see ...
	 */

	/**@{*/

	/**
	 * @brief	Returns a protocol specific frame.
	 *
	 * cpacket maintains a doubly linked list of protocol specific frames.
	 * The first frame is always of type fetherframe and refers to the
	 * Ethernet frame located at the start of the packet. The frame()
	 * method provides direct access to the DLL of protocol specific frame instances.
	 * Index 0 refers always to the first Ethernet header.
	 * Index -1 refers always to the packet's last header (usually the packet's payload).
	 *
	 * @param i index of frame to be returned
	 * @return pointer to fframe instance at index i
	 * @exception ePacketNotFound is thrown when the frame at the specified index could not be found
	 */
	fframe*
	frame(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th fetherframe instance found in packet. (0: first from left side, -1: first from right side)
	 * @see fetherframe
	 *
	 * @param i index of fetherframe
	 * @return pointer to fetherframe instance at index i
	 * @exception ePacketNotFound is thrown when no fetherframe at index i could be found
	 */
	fetherframe*
	ether(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th fvlanframe instance found in packet. (0: first from left side, -1: first from right side)
	 * @see fvlanframe
	 *
	 * @param i index of fvlanframe
	 * @return pointer to fvlanframe instance at index i
	 * @exception ePacketNotFound is thrown when no fvlanframe at index i could be found
	 */
	fvlanframe*
	vlan(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th fpppoeframe instance found in packet. (0: first from left side, -1: first from right side)
	 * @see fpppoeframe
	 *
	 * @param i index of fpppoeframe
	 * @return pointer to fpppoeframe instance at index i
	 * @exception ePacketNotFound is thrown when no fpppoeframe at index i could be found
	 */
	fpppoeframe*
	pppoe(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th fpppframe instance found in packet. (0: first from left side, -1: first from right side)
	 * @see fpppframe
	 *
	 * @param i index of fpppframe
	 * @return pointer to fpppframe instance at index i
	 * @exception ePacketNotFound is thrown when no fpppframe at index i could be found
	 */
	fpppframe*
	ppp(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th fmplsframe instance found in packet. (0: first from left side, -1: first from right side)
	 * @see fmplsframe
	 *
	 * @param i index of fmplsframe
	 * @return pointer to fmplsframe instance at index i
	 * @exception ePacketNotFound is thrown when no fmplsframe at index i could be found
	 */
	fmplsframe*
	mpls(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th farpv4frame instance found in packet. (0: first from left side, -1: first from right side)
	 * @see farpv4frame
	 *
	 * @param i index of farpv4frame
	 * @return pointer to farpv4frame instance at index i
	 * @exception ePacketNotFound is thrown when no farpv4frame at index i could be found
	 */
	farpv4frame*
	arpv4(
			int i = 0) throw (ePacketNotFound);




	/**
	 * @brief	Returns the i'th fipv4frame instance found in packet. (0: first from left side, -1: first from right side)
	 * @see fipv4frame
	 *
	 * @param i index of fipv4frame
	 * @return pointer to fipv4frame instance at index i
	 * @exception ePacketNotFound is thrown when no fipv4frame at index i could be found
	 */
	fipv4frame*
	ipv4(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th ficmpv4frame instance found in packet. (0: first from left side, -1: first from right side)
	 * @see ficmpv4frame
	 *
	 * @param i index of ficmpv4frame
	 * @return pointer to ficmpv4frame instance at index i
	 * @exception ePacketNotFound is thrown when no ficmpv4frame at index i could be found	 */
	ficmpv4frame*
	icmpv4(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th fipv6frame instance found in packet. (0: first from left side, -1: first from right side)
	 * @see fipv6frame
	 *
	 * @param i index of fipv6frame
	 * @return pointer to fipv6frame instance at index i
	 * @exception ePacketNotFound is thrown when no fipv6frame at index i could be found
	 */
	fipv6frame*
	ipv6(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th ficmpv6frame instance found in packet. (0: first from left side, -1: first from right side)
	 * @see ficmpv6frame
	 *
	 * @param i index of ficmpv6frame
	 * @return pointer to ficmpv6frame instance at index i
	 * @exception ePacketNotFound is thrown when no ficmpv6frame at index i could be found
	 */
	ficmpv6frame*
	icmpv6(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th fudpframe instance found in packet. (0: first from left side, -1: first from right side)
	 * @see fudpframe
	 *
	 * @param i index of fudpframe
	 * @return pointer to fudpframe instance at index i
	 * @exception ePacketNotFound is thrown when no fudpframe at index i could be found
	 */
	fudpframe*
	udp(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th ftcpframe instance found in packet. (0: first from left side, -1: first from right side)
	 * @see ftcpframe
	 *
	 * @param i index of ftcpframe
	 * @return pointer to ftcpframe instance at index i
	 * @exception ePacketNotFound is thrown when no ftcpframe at index i could be found
	 */
	ftcpframe*
	tcp(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th fsctpframe instance found in packet. (0: first from left side, -1: first from right side)
	 * @see fsctpframe
	 *
	 * @param i index of fsctpframe
	 * @return pointer to fsctpframe instance at index i
	 * @exception ePacketNotFound is thrown when no fsctpframe at index i could be found
	 */
	fsctpframe*
	sctp(
			int i = 0) throw (ePacketNotFound);



	/**
	 * @brief	Returns the i'th fgtpuframe instance found in packet. (0: first from left side, -1: first from right side)
	 * @see fgtpuframe
	 *
	 * @param i index of fgtpuframe
	 * @return pointer to fgtpuframe instance at index i
	 * @exception ePacketNotFound is thrown when no fgtpuframe at index i could be found
	 */
	fgtpuframe*
	gtpu(
			int i = 0) throw (ePacketNotFound);


	/**
	 * @brief	Returns the last fframe instance in cpacket.
	 * @see fframe
	 *
	 * @return pointer to fframe instance at tail
	 * @exception ePacketNotFound is thrown when no fframe could be found
	 */
	fframe*
	payload() throw (ePacketNotFound);


	/**@}*/





public: // action related methods

	/**
	 * @name 	Action related methods
	 *
	 * Each method from this group of methods consumes a cofaction or one of
	 * its derived classes and applies this action on the packet.
	 *
	 * @see cofaction
	 */

	/**@{*/


	/**
	 * @brief	Dispatcher for all actions received.
	 *
	 * If you do not know, how to apply an action, call this method.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	handle_action(
		cofaction& action);



	/**
	 * @brief	Apply a SET-FIELD action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_set_field(
		cofaction& action);



	/**
	 * @brief	Apply a COPY-TTL-OUT action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_copy_ttl_out(
		cofaction& action);



	/**
	 * @brief	Apply a COPY-TTL-IN action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_copy_ttl_in(
		cofaction& action);



	/**
	 * @brief	Apply a SET-MPLS-TTL action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_set_mpls_ttl(
		cofaction& action);




	/**
	 * @brief	Apply a DEC-MPLS-TTL action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_dec_mpls_ttl(
		cofaction& action);




	/**
	 * @brief	Apply a PUSH-VLAN action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_push_vlan(
		cofaction& action);



	/**
	 * @brief	Apply a POP-VLAN action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_pop_vlan(
		cofaction& action);




	/**
	 * @brief	Apply a PUSH-MPLS action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_push_mpls(
		cofaction& action);




	/**
	 * @brief	Apply a POP-MPLS action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_pop_mpls(
		cofaction& action);




	/**
	 * @brief	Apply a SET-NW-TTL action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_set_nw_ttl(
		cofaction& action);




	/**
	 * @brief	Apply a DEC-NW-TTL action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_dec_nw_ttl(
		cofaction& action);




	/**
	 * @brief	Apply a PUSH-PPPOE action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_push_pppoe(
		cofaction& action);




	/**
	 * @brief	Apply a POP-PPPOE action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_pop_pppoe(
		cofaction& action);




#if 0
	/**
	 * action: push ppp
	 */
	void action_push_ppp(
		cofaction& action);
#endif


	/**
	 * @brief	Apply a POP-PPP action.
	 *
	 * @param action cofaction instance to be applied
	 */
	void
	action_pop_ppp(
		cofaction& action);


	/**@}*/



public:


	/**
	 * @name	Packet manipulation methods
	 *
	 * A set of "actions" exists for manipulating a packet's content based on actions
	 * defined by the OpenFlow specification.
	 */

	/**@{*/

	/**
	 * @brief	ActionSetField: Sets a header field specified by an OXM-TLV.
	 *
	 */
	void
	set_field(
			coxmatch const& oxm);



	/**
	 * @brief	ActionCopyTTLout.
	 *
	 */
	void
	copy_ttl_out();



	/**
	 * @brief	ActionCopyTTLin.
	 *
	 */
	void
	copy_ttl_in();



	/**
	 * @brief	ActionSetMplsTTL.
	 *
	 * @param mpls_ttl TTL value set into outermost MPLS header
	 */
	void
	set_mpls_ttl(
			uint8_t mpls_ttl);



	/**
	 * @brief	ActionDecMplsTTL.
	 *
	 *  Decrements TTL field of outermost MPLS tag by 1.
	 */
	void
	dec_mpls_ttl();



	/**
	 * @brief	ActionPushVlan.
	 *
	 * Pushes a new outermost VLAN tag onto the packet.
	 *
	 * @param ethertype new ethernet type value set in Ethernet header
	 */
	void
	push_vlan(
			uint16_t ethertype);



	/**
	 * @brief	ActionPopVlan.
	 *
	 * Removes the outermost VLAN tag from a packet.
	 */
	void
	pop_vlan();



	/**
	 * @brief	ActionPushMpls.
	 *
	 * Pushes a new outermost MPLS tag onto the packet.
	 *
	 * @param ethertype new ethernet type value set in Ethernet header
	 */
	void
	push_mpls(
			uint16_t ethertype);



	/**
	 * @brief	ActionPopMpls.
	 *
	 * Removes the outermost MPLS tag from a packet.
	 *
	 * @param ethertype new ethernet type value set in Ethernet header
	 */
	void
	pop_mpls(
			uint16_t ethertype);



	/**
	 * @brief	ActionSetNwTTL.
	 *
	 * @param nw_ttl new TTL value for network layer protocol
	 */
	void
	set_nw_ttl(
			uint8_t nw_ttl);



	/**
	 * @brief	ActionDecNwTTL.
	 *
	 */
	void
	dec_nw_ttl();



	/**
	 * @brief	ActionPushPPPoE
	 *
	 * Pushes a new outermost PPPoE tag onto the packet.
	 *
	 * @param ethertype new ethernet type value set in Ethernet header
	 */
	void
	push_pppoe(
			uint16_t ethertype);



	/**
	 * @brief	ActionPopPPPoE.
	 *
	 * Removes the outermost PPPoE tag from a packet.
	 *
	 * @param ethertype new ethernet type value set in Ethernet header
	 */
	void
	pop_pppoe(
			uint16_t ethertype);



#if 0
	/** push ppp
	 */
	void
	push_ppp(
			uint16_t code);
#endif



	/**
	 * @brief	ActionPopPPP.
	 *
	 * Removes the outermost PPP tag from a packet.
	 */
	void
	pop_ppp();


	/**@}*/




private:



	/**
	 */
	void
	set_field_basic_class(
			coxmatch const& oxm);



	/**
	 * @brief	Inserts a memory block of len bytes after the Ethernet header.
	 *
	 * The cpacket container assumes that each stored packet starts with an
	 * Ethernet header. tag_insert() inserts between the Ethernet header and
	 * the succeeding payload len bytes, e.g. for VLAN, MPLS, PPPoE tags.
	 *
	 * @return start of new inserted memory block within cpacket's memory area
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



	static const char*
	cpacket_info();


	void
	cpacket_list_insert();

	void
	cpacket_list_erase();




	/**
	 *
	 */
	void
	reset();


	/**
	 *
	 */
	const char*
	data_c_str();


	/** returns length of parsed packet (may be shortened during Packet-In)
	 *
	 */
	size_t
	get_payload_len(
			fframe *from = (fframe*)0,
			fframe *to = (fframe*)0);


	/**
	 *
	 */
	void
	set_total_len(uint16_t total_len);


	/**
	 *
	 */
	size_t
	get_total_len();




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
	parse_ipv6(
			uint8_t *data,
			size_t datalen);


	/**
	 *
	 */
	void
	parse_icmpv6(
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




	/**
	 *
	 */
	void
	parse_gtpu(
			uint8_t *data,
			size_t datalen);




};

}; // end of namespace

#endif
