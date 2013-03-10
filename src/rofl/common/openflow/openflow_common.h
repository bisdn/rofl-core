/*
 * openflow.h
 *
 *  Created on: 02.03.2013
 *      Author: andi
 */

#ifndef OPENFLOW_COMMON_H_
#define OPENFLOW_COMMON_H_ 1

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#ifdef SWIG
#define OFP_ASSERT(EXPR)        /* SWIG can't handle OFP10_ASSERT. */
#elif !defined(__cplusplus)
/* Build-time assertion for use in a declaration context. */
#define OFP_ASSERT(EXPR)                                                \
        extern int (*build_assert(void))[ sizeof(struct {               \
                    unsigned int build_assert_failed : (EXPR) ? 1 : -1; })]
#else /* __cplusplus */
#define OFP_ASSERT(_EXPR) typedef int build_assert_failed[(_EXPR) ? 1 : -1]
#endif /* __cplusplus */

#ifndef SWIG
#define OFP_PACKED __attribute__((packed))
#else
#define OFP_PACKED              /* SWIG doesn't understand __attribute. */
#endif

#define OFP_MAX_TABLE_NAME_LEN 32
#define OFP_MAX_PORT_NAME_LEN  16

#define OFP_TCP_PORT  6633
#define OFP_SSL_PORT  6633

#define OFP_ETH_ALEN 6          /* Bytes in an Ethernet address. */


/* Header on all OpenFlow packets. */
struct ofp_header {
    uint8_t version;    /* OFP10_VERSION. */
    uint8_t type;       /* One of the OFP10T_ constants. */
    uint16_t length;    /* Length including this ofp10_header. */
    uint32_t xid;       /* Transaction id associated with this packet.
                           Replies use the same id as was in the request
                           to facilitate pairing. */
};
OFP_ASSERT(sizeof(struct ofp_header) == 8);


enum ofp_type {
	/* Immutable messages. */
    OFPT_HELLO 					= 0,    /* Symmetric message */
    OFPT_ERROR 					= 1,	/* Symmetric message */
    OFPT_ECHO_REQUEST 			= 2,	/* Symmetric message */
    OFPT_ECHO_REPLY				= 3,    /* Symmetric message */
    OFPT_EXPERIMENTER			= 4,    /* Symmetric message */

    /* Switch configuration messages. */
    OFPT_FEATURES_REQUEST		= 5,    /* Controller/switch message */
    OFPT_FEATURES_REPLY			= 6,    /* Controller/switch message */
    OFPT_GET_CONFIG_REQUEST		= 7,    /* Controller/switch message */
    OFPT_GET_CONFIG_REPLY		= 8,    /* Controller/switch message */
    OFPT_SET_CONFIG				= 9,    /* Controller/switch message */

    /* Asynchronous messages. */
    OFPT_PACKET_IN				= 10,   /* Async message */
    OFPT_FLOW_REMOVED			= 11,   /* Async message */
    OFPT_PORT_STATUS			= 12,   /* Async message */

    /* Controller command messages. */
    OFPT_PACKET_OUT				= 13,   /* Controller/switch message */
    OFPT_FLOW_MOD				= 14,   /* Controller/switch message */
    OFPT_GROUP_MOD				= 15,   /* Controller/switch message */
    OFPT_PORT_MOD				= 16,   /* Controller/switch message */
    OFPT_TABLE_MOD				= 17,   /* Controller/switch message */

    /* Multipart messages. */
    OFPT_MULTIPART_REQUEST		= 18,   /* Controller/switch message */
    OFPT_MULTIPART_REPLY		= 19,   /* Controller/switch message */

    /* Statistics messages (up to and including OF1.2) */
    OFPT_STATS_REQUEST			= 18,	/* Controller/switch message */
    OFPT_STATS_REPLY			= 19,	/* Controller/switch message */

    /* Barrier messages. */
    OFPT_BARRIER_REQUEST		= 20,   /* Controller/switch message */
    OFPT_BARRIER_REPLY			= 21,   /* Controller/switch message */

    /* Queue Configuration messages. */
    OFPT_QUEUE_GET_CONFIG_REQUEST	= 22,  /* Controller/switch message */
    OFPT_QUEUE_GET_CONFIG_REPLY		= 23,  /* Controller/switch message */

    /* Controller role change request messages. */
    OFPT_ROLE_REQUEST    		= 24, /* Controller/switch message */
    OFPT_ROLE_REPLY				= 25, /* Controller/switch message */

    /* Asynchronous message configuration. */
   	OFPT_GET_ASYNC_REQUEST		= 26, /* Controller/switch message */
    OFPT_GET_ASYNC_REPLY		= 27, /* Controller/switch message */
    OFPT_SET_ASYNC				= 28, /* Controller/switch message */

    /* Meters and rate limiters configuration messages. */
    OFPT_METER_MOD				= 29, /* Controller/switch message */
};


/* Port numbering. Ports are numbered starting from 1. */
enum ofp_port_no {
    /* Maximum number of physical switch ports. */
    OFPP_MAX        = 0xffffff00,

    /* Fake output "ports". */
    OFPP_IN_PORT    = 0xfffffff8,  /* Send the packet out the input port.  This
                                      virtual port must be explicitly used
                                      in order to send back out of the input
                                      port. */
    OFPP_TABLE      = 0xfffffff9,  /* Submit the packet to the first flow table
                                      NB: This destination port can only be
                                      used in packet-out messages. */
    OFPP_NORMAL     = 0xfffffffa,  /* Process with normal L2/L3 switching. */
    OFPP_FLOOD      = 0xfffffffb,  /* All physical ports in VLAN, except input
                                      port and those blocked or link down. */
    OFPP_ALL        = 0xfffffffc,  /* All physical ports except input port. */
    OFPP_CONTROLLER = 0xfffffffd,  /* Send to controller. */
    OFPP_LOCAL      = 0xfffffffe,  /* Local openflow "port". */
    OFPP_ANY        = 0xffffffff   /* Wildcard port used only for flow mod
                                      (delete) and flow stats requests. Selects
                                      all flows regardless of output port
                                      (including flows with no output port). */
};


#endif /* OPENFLOW_COMMON_H_ */
