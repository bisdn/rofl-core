/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LLDP_H
#define LLDP_H 1

#ifndef LLDP_ETH_ALEN
#define LLDP_ETH_ALEN 6
#endif

struct mac_hdr_t {
	uint8_t byte[LLDP_ETH_ALEN];
};


// LLDP common TLV header
struct lldp_tlv_hdr_t {
#if __BYTE_ORDER == __BIG_ENDIAN
	uint16_t type:7;
	uint16_t length:9;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	uint16_t length:9;
	uint16_t type:7;
#endif
	uint8_t body[0];
} __attribute__((packed));


// Ethernet + LLDP common header
struct lldp_hdr_t {
	struct mac_hdr_t dst;
	struct mac_hdr_t src;
	uint16_t ethertype; // = 0x88-CC
	struct lldp_tlv_hdr_t body[0];
} __attribute__((packed));


// LLDP TLV chassis ID header
struct lldp_tlv_chassis_id_hdr_t {
	struct lldp_tlv_hdr_t header;
	uint8_t subtype;
	uint8_t body[0];
} __attribute__((packed));


// LLDP TLV port ID header
struct lldp_tlv_port_id_hdr_t {
	struct lldp_tlv_hdr_t header;
	uint8_t subtype;
	uint8_t body[0];
} __attribute__((packed));


// LLDP TLV SYSTEM CAPS header
struct lldp_tlv_caps_hdr_t {
	struct lldp_tlv_hdr_t header;
	uint16_t available_caps;
	uint16_t enabled_caps;
};


// LLDP TLV TTL header
struct lldp_tlv_ttl_hdr_t {
	struct lldp_tlv_hdr_t header;
	uint16_t ttl;
} __attribute__((packed));


/* LLDP TLV types */
enum lldp_tlv_type {
	LLDPTT_END = 0,
	LLDPTT_CHASSIS_ID = 1,
	LLDPTT_PORT_ID = 2,
	LLDPTT_TTL = 3,
	LLDPTT_PORT_DESC = 4,
	LLDPTT_SYSTEM_NAME = 5,
	LLDPTT_SYSTEM_DESC = 6,
	LLDPTT_SYSTEM_CAPS = 7,
	LLDPTT_MGMT_ADDR = 8
};

/* LLDP chassis ID TLV subtypes */
enum lldp_tlv_chassis_id_subtype {
	LLDPCHIDST_RESERVED = 0,
	LLDPCHIDST_CHASSIS = 1,
	LLDPCHIDST_IFALIAS = 2,
	LLDPCHIDST_PORT = 3,
	LLDPCHIDST_MAC_ADDR = 4,
	LLDPCHIDST_NET_ADDR = 5,
	LLDPCHIDST_IFNAME = 6,
	LLDPCHIDST_LOCAL = 7,
};

/* LLDP port ID TLV subtypes */
enum lldp_tlv_port_id_subtype {
	LLDPPRTIDST_RESERVED = 0,
	LLDPPRTIDST_IFALIAS = 1,
	LLDPPRTIDST_PORT = 2,
	LLDPPRTIDST_MAC_ADDR = 3,
	LLDPPRTIDST_NET_ADDR = 4,
	LLDPPRTIDST_IFNAME = 5,
	LLDPPRTIDST_AGENT_CIRCUIT_ID = 6,
	LLDPPRTIDST_LOCAL = 7
};

/* LLDP system capabilities */
enum lldp_sys_caps {
	LLDP_SYSCAPS_OTHER = 0,
	LLDP_SYSCAPS_REPEATER = 1,
	LLDP_SYSCAPS_BRIDGE = 2,
	LLDP_SYSCAPS_WLANAP = 3,
	LLDP_SYSCAPS_ROUTER = 4,
	LLDP_SYSCAPS_PHONE = 5,
	LLDP_SYSCAPS_DOCSIS = 6,
	LLDP_SYSCAPS_STA_ONLY = 7
};

#endif
