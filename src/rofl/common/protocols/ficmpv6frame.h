/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ficmpv6frame_H
#define ficmpv6frame_H 1

#include <map>
#include <endian.h>
#ifndef htobe16
	#include "../endian_conversion.h"
#endif

#include "../fframe.h"
#include "../caddress.h"

namespace rofl
{

class eICMPv6FrameBase 				: public eFrameBase {};
class eICMPv6FrameNotFound			: public eICMPv6FrameBase {};
class eICMPv6FrameInvalType			: public eICMPv6FrameBase {};
class eICMPv6FrameInvalCode			: public eICMPv6FrameBase {};
class eICMPv6FrameInvalidSyntax 	: public eICMPv6FrameBase, public eFrameInvalidSyntax {};
class eICMPv6FrameTooShort			: public eICMPv6FrameInvalidSyntax {};







class ficmpv6opt : public fframe {

	#define IPV6_ADDR_LEN		16
	#define ETHER_ADDR_LEN		6

public: // static definitions and constants

	/*
	 * ICMPv6 NDP options
	 */

	enum icmpv6_option_type_t {
		ICMPV6_OPT_LLADDR_SOURCE 		= 1,
		ICMPV6_OPT_LLADDR_TARGET 		= 2,
		ICMPV6_OPT_PREFIX_INFO			= 3,
		ICMPV6_OPT_REDIRECT				= 4,
		ICMPV6_OPT_MTU					= 5,
	};

	/* ICMPv6 generic option header */
	struct icmpv6_option_hdr_t {
		uint8_t 						type;
		uint8_t							len;
		uint8_t 						data[0];
	} __attribute__((packed));


	/* ICMPv6 link layer address option */
	struct icmpv6_lla_option_t {
		struct icmpv6_option_hdr_t		hdr;
		uint8_t							addr[ETHER_ADDR_LEN]; // len=1 (in 8-octets wide blocks) and we assume Ethernet here
	} __attribute__((packed));

	/* ICMPv6 prefix information option */
	struct icmpv6_prefix_info_t {
		struct icmpv6_option_hdr_t		hdr;
		uint8_t							pfxlen;
		uint8_t							flags;
		uint32_t						valid_lifetime;
		uint32_t						preferred_lifetime;
		uint32_t						reserved;
		uint8_t							prefix[IPV6_ADDR_LEN];
	} __attribute__((packed));

	/* ICMPv6 redirected option header */
	struct icmpv6_redirected_hdr_t {
		struct icmpv6_option_hdr_t		hdr;
		uint8_t							reserved[6];
		uint8_t							data[0];
	} __attribute__((packed));

	/* ICMPv6 MTU option */
	struct icmpv6_mtu_t {
		struct icmpv6_option_hdr_t		hdr;
		uint8_t							reserved[2];
		uint32_t						mtu;
	} __attribute__((packed));

public:

	union {
		struct icmpv6_option_hdr_t		*optu;
		struct icmpv6_lla_option_t		*optu_lla;
		struct icmpv6_prefix_info_t		*optu_pfx;
		struct icmpv6_redirected_hdr_t	*optu_rdr;
		struct icmpv6_mtu_t				*optu_mtu;
	} icmpv6optu;

#define icmpv6_opt			icmpv6optu.optu
#define icmpv6_opt_lla		icmpv6optu.optu_lla
#define icmpv6_opt_pfx		icmpv6optu.optu_pfx
#define icmpv6_opt_rdr		icmpv6optu.optu_rdr
#define icmpv6_opt_mtu		icmpv6optu.optu_mtu

public:
	/**
	 */
	ficmpv6opt(uint8_t *data = (uint8_t*)0, size_t datalen = 0);
	/**
	 */
	ficmpv6opt(struct icmpv6_option_hdr_t *data, size_t datalen);
	/**
	 */
	virtual ~ficmpv6opt();
	/** copy constructor
	 */
	ficmpv6opt(ficmpv6opt const& opt);
	/** assignment operator
	 */
	ficmpv6opt& operator= (ficmpv6opt const& opt);
public:
	/**
	 */
	uint8_t get_opt_type() const;
	/**
	 */
	void set_opt_type(uint8_t type);
	/**
	 */
	cmacaddr get_ll_taddr() const;
	/**
	 */
	void set_ll_taddr(cmacaddr const& addr);
	/**
	 */
	cmacaddr get_ll_saddr() const;
	/**
	 */
	void set_ll_saddr(cmacaddr const& addr);
	/**
	 */
	uint8_t get_pfx_on_link_flag() const;
	/**
	 */
	void set_pfx_on_link_flag(uint8_t flag);
	/**
	 */
	uint8_t get_pfx_aac_flag() const;
	/**
	 */
	void set_pfx_aac_flag(uint8_t flag);

public:

	friend std::ostream&
	operator<< (std::ostream& os, ficmpv6opt const& opt) {
		os << "<ficmvp6opt ";
			os << "type:";
			switch (opt.get_opt_type()) {
			case ICMPV6_OPT_LLADDR_SOURCE:	os << "LLADDR-SOURCE "; 	break;
			case ICMPV6_OPT_LLADDR_TARGET:	os << "LLADDR-TARGET ";		break;
			case ICMPV6_OPT_PREFIX_INFO:	os << "PREFIX-INFO ";		break;
			case ICMPV6_OPT_REDIRECT:		os << "REDIRECT ";			break;
			case ICMPV6_OPT_MTU:			os << "MTU ";				break;
			default:						os << "UNKNOWN ";			break;
			}
			os << std::endl << dynamic_cast<fframe const&>( opt ) << std::endl;
		os << ">";
		return os;
	};
};








class ficmpv6frame : public fframe {

#define IPV6_ADDR_LEN		16
#define ETHER_ADDR_LEN		6

public:	/* ICMPv6 constants and definitions */

	enum icmpv6_ip_proto_t {
		ICMPV6_IP_PROTO = 58,
	};

	enum icmpv6_type_t {
		ICMPV6_TYPE_DESTINATION_UNREACHABLE 							= 1,
		ICMPV6_TYPE_PACKET_TOO_BIG										= 2,
		ICMPV6_TYPE_TIME_EXCEEDED										= 3,
		ICMPV6_TYPE_PARAMETER_PROBLEM									= 4,
		ICMPV6_TYPE_ECHO_REQUEST										= 128,
		ICMPV6_TYPE_ECHO_REPLY											= 129,
		ICMPV6_TYPE_MULTICAST_LISTENER_QUERY							= 130,
		ICMPV6_TYPE_MULTICAST_LISTENER_REPORT							= 131,
		ICMPV6_TYPE_MULTICAST_LISTENER_DONE								= 132,
		ICMPV6_TYPE_ROUTER_SOLICATION									= 133,
		ICMPV6_TYPE_ROUTER_ADVERTISEMENT								= 134,
		ICMPV6_TYPE_NEIGHBOR_SOLICITATION								= 135,
		ICMPV6_TYPE_NEIGHBOR_ADVERTISEMENT								= 136,
		ICMPV6_TYPE_REDIRECT_MESSAGE									= 137,
		ICMPV6_TYPE_ROUTER_RENUMBERING									= 138,
		ICMPV6_TYPE_ICMP_NODE_INFORMATION_QUERY							= 139,
		ICMPV6_TYPE_ICMP_NODE_INFORMATION_RESPONSE						= 140,
		ICMPV6_TYPE_INVERSE_NEIGHBOR_DISCOVERY_SOLICITATION_MESSAGE 	= 141,
		ICMPV6_TYPE_INVERSE_NEIGHBOR_DISCOVERY_ADVERTISEMENT_MESSAGE 	= 142,
		ICMPV6_TYPE_MULTICAST_LISTENER_DISCOVERY_REPORT					= 143,
		ICMPV6_TYPE_HOME_AGENT_ADDRESS_DISCOVERY_REQUEST_MESSAGE		= 144,
		ICMPV6_TYPE_HOME_AGENT_ADDRESS_DISCOVERY_REPLY_MESSAGE			= 145,
		ICMPV6_TYPE_MOBILE_PREFIX_SOLICITATION							= 146,
		ICMPV6_TYPE_MOBILE_PREFIX_ADVERTISEMENT							= 147,
	};

	enum icmpv6_destination_unreachable_code_t {
		ICMPV6_DEST_UNREACH_CODE_NO_ROUTE_TO_DESTINATION					= 0,
		ICMPV6_DEST_UNREACH_CODE_COMMUNICATION_WITH_DESTINATION_ADMINISTRATIVELY_PROHIBITED	= 1,
		ICMPV6_DEST_UNREACH_CODE_BEYOND_SCOPE_OF_SOURCE_ADDRESS				= 2,
		ICMPV6_DEST_UNREACH_CODE_ADDRESS_UNREACHABLE						= 3,
		ICMPV6_DEST_UNREACH_CODE_PORT_UNREACHABLE							= 4,
		ICMPV6_DEST_UNREACH_CODE_SOURCE_ADDRESS_FAILED_INGRESS_EGRESS_POLICY= 5,
		ICMPV6_DEST_UNREACH_CODE_REJECT_ROUTE_TO_DESTINATION				= 6,
		ICMPV6_DEST_UNREACH_CODE_ERROR_IN_SOURCE_ROUTING_HEADER				= 7,
	};





	/*
	 * ICMPv6 message types
	 */

	/* ICMPv6 generic header */
	struct icmpv6_hdr_t {
		uint8_t 						type;
		uint8_t 						code;
		uint16_t 						checksum;
		uint8_t 						data[0];
	} __attribute__((packed));


	/*
	 * ICMPv6 error message types
	 */

	/* ICMPv6 message format for Destination Unreachable */
	struct icmpv6_dest_unreach_hdr_t {
		struct icmpv6_hdr_t				icmpv6_hdr;				// type=133, code=0
		uint32_t						unused;					// a 32bit value
		uint8_t							data[0];				// the IP packet
	} __attribute__((packed));

	/* ICMPv6 message format for Packet Too Big */
	struct icmpv6_pkt_too_big_hdr_t {
		struct icmpv6_hdr_t				icmpv6_hdr;				// type=133, code=0
		uint32_t						unused;					// a 32bit value
		uint8_t							data[0];				// the IP packet
	} __attribute__((packed));

	/* ICMPv6 message format for Time Exceeded */
	struct icmpv6_time_exceeded_hdr_t {
		struct icmpv6_hdr_t				icmpv6_hdr;				// type=133, code=0
		uint32_t						unused;					// a 32bit value
		uint8_t							data[0];				// the IP packet
	} __attribute__((packed));

	/* ICMPv6 message format for Parameter Problem */
	struct icmpv6_param_problem_hdr_t {
		struct icmpv6_hdr_t				icmpv6_hdr;				// type=133, code=0
		uint32_t						pointer;				// a 32bit value
		uint8_t							data[0];				// the IP packet
	} __attribute__((packed));

	/* ICMPv6 echo request message format */
	struct icmpv6_echo_request_hdr_t {
		struct icmpv6_hdr_t				icmpv6_hdr;				// type=133, code=0
		uint16_t						id;
		uint16_t 						seqno;
		uint8_t							data[0];				// arbitrary data
	} __attribute__((packed));

	/* ICMPv6 echo reply message format */
	struct icmpv6_echo_reply_hdr_t {
		struct icmpv6_hdr_t				icmpv6_hdr;				// type=133, code=0
		uint16_t						id;
		uint16_t 						seqno;
		uint8_t							data[0];				// arbitrary data
	} __attribute__((packed));

	/*
	 * ICMPv6 NDP message types
	 */

	/* ICMPv6 router solicitation */
	struct icmpv6_router_solicitation_hdr_t {
		struct icmpv6_hdr_t				icmpv6_hdr;				// type=133, code=0
		uint32_t 						reserved;				// reserved for later use, for now: mbz
		struct ficmpv6opt::icmpv6_option_hdr_t		options[0];
	} __attribute__((packed));

	/* ICMPv6 router advertisement */
	struct icmpv6_router_advertisement_hdr_t {
		struct icmpv6_hdr_t				icmpv6_hdr;				// type=134, code=0
		uint8_t 						cur_hop_limit;
		uint8_t							flags;
		uint16_t 						rtr_lifetime;
		uint32_t						reachable_timer;
		uint32_t 						retrans_timer;
		struct ficmpv6opt::icmpv6_option_hdr_t		options[0];
	} __attribute__((packed));

	/* ICMPv6 neighbor solicitation */
	struct icmpv6_neighbor_solicitation_hdr_t {
		struct icmpv6_hdr_t			icmpv6_hdr;				// type=135, code=0
		uint32_t 					reserved;				// reserved for later use, for now: mbz
		uint8_t						taddr[IPV6_ADDR_LEN]; 	// =target address
		struct ficmpv6opt::icmpv6_option_hdr_t		options[0];
	} __attribute__((packed));

	/* ICMPv6 neighbor advertisement */
	struct icmpv6_neighbor_advertisement_hdr_t {
		struct icmpv6_hdr_t			icmpv6_hdr;				// type=136, code=0
		uint32_t 					flags;
		uint8_t						taddr[IPV6_ADDR_LEN]; 	// =target address
		struct ficmpv6opt::icmpv6_option_hdr_t		options[0];
	} __attribute__((packed));

	/* ICMPv6 redirect message */
	struct icmpv6_redirect_hdr_t {
		struct icmpv6_hdr_t			icmpv6_hdr;				// type=137, code=0
		uint32_t 					reserved;				// reserved for later use, for now: mbz
		uint8_t						taddr[IPV6_ADDR_LEN]; 	// =target address
		uint8_t						daddr[IPV6_ADDR_LEN];	// =destination address
		struct ficmpv6opt::icmpv6_option_hdr_t		options[0];
	} __attribute__((packed));


	/*
	 * ICMPv6 pseudo header
	 */

	/* for ICMPv6 checksum calculation */
	struct icmpv6_pseudo_hdr_t {
		uint8_t 	src[IPV6_ADDR_LEN];
		uint8_t 	dst[IPV6_ADDR_LEN];
		uint32_t 	icmpv6_len;				// payload length (extension headers + ICMPv6 message)
		uint8_t 	zeros[3];				// = 0
		uint8_t 	nxthdr;					// = 58 (=ICMPV6_IP_PROTO, see below)
	} __attribute__((packed));



#define DEFAULT_ICMPV6_FRAME_SIZE sizeof(struct icmpv6_hdr_t)

public: // data structures

	union {
		struct icmpv6_hdr_t 						*icmpv6u_hdr;							// ICMPv6 message header
		struct icmpv6_dest_unreach_hdr_t			*icmpv6u_dst_unreach_hdr;				// ICMPv6 destination unreachable
		struct icmpv6_pkt_too_big_hdr_t				*icmpv6u_pkt_too_big_hdr;				// ICMPv6 packet too big
		struct icmpv6_time_exceeded_hdr_t			*icmpv6u_time_exceeded_hdr;				// ICMPv6 time exceeded
		struct icmpv6_param_problem_hdr_t			*icmpv6u_param_problem_hdr;				// ICMPv6 parameter problem
		struct icmpv6_echo_request_hdr_t			*icmpv6u_echo_request_hdr;				// ICMPv6 echo request
		struct icmpv6_echo_reply_hdr_t				*icmpv6u_echo_reply_hdr;				// ICMPv6 echo reply
		struct icmpv6_router_solicitation_hdr_t		*icmpv6u_rtr_solicitation_hdr;			// ICMPv6 rtr solicitation
		struct icmpv6_router_advertisement_hdr_t	*icmpv6u_rtr_advertisement_hdr;			// ICMPv6 rtr advertisement
		struct icmpv6_neighbor_solicitation_hdr_t	*icmpv6u_neighbor_solication_hdr;		// ICMPv6 NDP solication header
		struct icmpv6_neighbor_advertisement_hdr_t	*icmpv6u_neighbor_advertisement_hdr;	// ICMPv6 NDP advertisement header
		struct icmpv6_redirect_hdr_t				*icmpv6u_redirect_hdr;					// ICMPV6 redirect header
	} icmpv6u;

#define icmpv6_hdr 						icmpv6u.icmpv6u_hdr
#define icmpv6_dest_unreach				icmpv6u.icmpv6u_dst_unreach_hdr
#define icmpv6_pkt_too_big				icmpv6u.icmpv6u_pkt_too_big_hdr
#define icmpv6_time_exceeded			icmpv6u.icmpv6u_time_exceeded_hdr
#define icmpv6_param_problem			icmpv6u.icmpv6u_param_problem_hdr
#define icmpv6_echo_request				icmpv6u.icmpv6u_echo_request_hdr
#define icmpv6_echo_reply				icmpv6u.icmpv6u_echo_reply_hdr
#define icmpv6_rtr_solicitation			icmpv6u.icmpv6u_rtr_solicitation_hdr
#define icmpv6_rtr_advertisement		icmpv6u.icmpv6u_rtr_advertisement_hdr
#define icmpv6_neighbor_solicitation	icmpv6u.icmpv6u_neighbor_solication_hdr
#define icmpv6_neighbor_advertisement	icmpv6u.icmpv6u_neighbor_advertisement_hdr
#define icmpv6_redirect					icmpv6u.icmpv6u_redirect_hdr

	std::map<ficmpv6opt::icmpv6_option_type_t, ficmpv6opt> 	 icmpv6opts;	// ICMPv6 NDP options

	uint8_t 					*data; 			// ICMPv6 message body
	size_t 						 datalen;		// ICMPv6 message body length

private: // data structures

	std::string 				 info;			// info string

public:


	/** constructor
	 *
	 */
	ficmpv6frame(
			uint8_t *data,
			size_t datalen);


	/** constructor for creating new icmpv4 frame
	 *
	 */
	ficmpv6frame(
			size_t len = DEFAULT_ICMPV6_FRAME_SIZE);


	/** destructor
	 *
	 */
	virtual
	~ficmpv6frame();


	/** calculate TCP header checksum
	 *
	 */
	void
	icmpv6_calc_checksum();


	/** get specific ICMPv6 option
	 *
	 */
	ficmpv6opt&
	get_option(ficmpv6opt::icmpv6_option_type_t type) throw (eICMPv6FrameNotFound);


public: // overloaded from fframe

	/** returns boolean value indicating completeness of the packet
	 */
	virtual bool
	complete();

	/** returns the number of bytes this packet expects from the socket next
	 */
	virtual size_t
	need_bytes();

	/** validate (frame structure)
	 *
	 */
	virtual void
	validate(uint16_t total_len = 0) throw (eICMPv6FrameTooShort);

	/** initialize (set eth_hdr, pppoe_hdr)
	 *
	 */
	virtual void
	initialize();

	/** insert payload
	 *
	 */
	virtual void
	payload_insert(
			uint8_t *data, size_t datalen) throw (eFrameOutOfRange);

	/** get payload
	 *
	 */
	virtual uint8_t*
	payload() const throw (eFrameNoPayload);

	/** get payload length
	 *
	 */
	virtual size_t
	payloadlen() const throw (eFrameNoPayload);

public:

	/**
	 *
	 */
	uint8_t
	get_icmpv6_code() const;

	/**
	 *
	 */
	void
	set_icmpv6_code(uint8_t code);

	/**
	 *
	 */
	uint8_t
	get_icmpv6_type() const;

	/**
	 *
	 */
	void
	set_icmpv6_type(uint8_t type);

	/**
	 *
	 */
	uint8_t
	get_icmpv6_ndp_rtr_flag() const;

	/**
	 *
	 */
	void
	set_icmpv6_ndp_rtr_flag();

	/**
	 *
	 */
	uint8_t
	get_icmpv6_ndp_solicited_flag() const;

	/**
	 *
	 */
	void
	set_icmpv6_ndp_solicited_flag();

	/**
	 *
	 */
	uint8_t
	get_icmpv6_ndp_override_flag() const;

	/**
	 *
	 */
	void
	set_icmpv6_ndp_override_flag();

	/**
	 *
	 */
	caddress_in6
	get_icmpv6_neighbor_taddr() const;

	/**
	 *
	 */
	void
	set_icmpv6_neighbor_taddr(caddress_in6 const& addr);




private: // methods

	/**
	 */
	void
	parse_icmpv6_options(struct ficmpv6opt::icmpv6_option_hdr_t *option, size_t optlen) throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_dest_unreach() throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_pkt_too_big() throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_time_exceeded() throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_param_problem() throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_echo_request() throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_echo_reply() throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_rtr_solicitation() throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_rtr_advertisement() throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_neighbor_solicitation() throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_neighbor_advertisement() throw (eICMPv6FrameTooShort);
	/**
	 */
	void
	parse_icmpv6_redirect() throw (eICMPv6FrameTooShort);

public:

	friend std::ostream&
	operator<< (std::ostream& os, ficmpv6frame const& frame) {
		os << dynamic_cast<fframe const&>( frame );
		os << indent(2) << "<ficmpv6frame ";
			os << "code:" << (int)frame.get_icmpv6_code() << " ";
			os << "type:" << (int)frame.get_icmpv6_type() << " ";
		os << ">" << std::endl;
		return os;
	};
};

}; // end of namespace

#endif
