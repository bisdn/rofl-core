/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmatch.cc
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */

#include "rofl/common/openflow/coxmatch.h"

using namespace rofl::openflow;

coxmatch::coxmatch()
{}


coxmatch::coxmatch(
		uint32_t oxm_id)
{
	rofl::cmemory::resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + (size_t)(oxm_id & 0x000000ff));
	set_oxm_id(oxm_id);
}


coxmatch::coxmatch(
		uint8_t* oxm_hdr, size_t oxm_len) :
				rofl::cmemory(oxm_hdr, oxm_len)
{}


coxmatch::coxmatch(
		const coxmatch& oxm) {
	*this = oxm;
}


coxmatch::~coxmatch()
{}


coxmatch&
coxmatch::operator= (
		const coxmatch& oxm) {
	if (this == &oxm)
		return *this;
	cmemory::operator= (oxm);
	return *this;
}


bool
coxmatch::operator== (
		coxmatch const& oxm) const {
	return cmemory::operator== (oxm);
}


bool
coxmatch::operator!= (
		coxmatch const& oxm) const {
	return cmemory::operator!= (oxm);
}


bool
coxmatch::operator< (
		coxmatch const& oxm)
{
	if (get_oxm_id() != oxm.get_oxm_id()) {
		return (get_oxm_id() < oxm.get_oxm_id());
	}
	size_t len = (memlen() < oxm.memlen()) ? memlen() : oxm.memlen();
	int rc = memcmp(somem(), oxm.somem(), len);
	if (rc < 0) {
		return true;
	} else if (rc > 0) {
		return false;
	}
	return false;
}



/*
 * oxm-id
 */
void
coxmatch::set_oxm_id(uint32_t oxm_id) {
	if (memlen() < sizeof(struct ofp_oxm_tlv_hdr))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_tlv_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_tlv_hdr*)somem();
	oxm->oxm_id = htobe32(oxm_id);
}



uint32_t
coxmatch::get_oxm_id() const {
	if (memlen() < sizeof(struct ofp_oxm_tlv_hdr))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_tlv_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_tlv_hdr*)somem();
	return be32toh(oxm->oxm_id);
}









/*
 * these are special beasts: used for 4bytes and 16bytes as well
 */
coxmatch::coxmatch(
		uint32_t oxm_id, rofl::caddress const& value) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr))
{
	if (value.is_af_inet()) {
		resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) +  4*sizeof(uint8_t));
		set_u32value(value);
	}
	if (value.is_af_inet6()) {
		resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 16*sizeof(uint8_t));
		set_u128value(value);
	}
	set_oxm_id(oxm_id);
}


coxmatch::coxmatch(
		uint32_t oxm_id, rofl::caddress const& value, rofl::caddress const& mask) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t))
{
	if (value.is_af_inet()) {
		resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2* 4*sizeof(uint8_t));
		set_u32value(value);
		set_u32mask(mask);
	}
	if (value.is_af_inet6()) {
		resize(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t));
		set_u128value(value);
		set_u128mask(mask);
	}
	set_oxm_id(oxm_id);
}




/*
 * 1 byte
 */
coxmatch::coxmatch(
		uint32_t oxm_id, uint8_t value) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint8_t))
{
	set_oxm_id(oxm_id);
	set_u8value(value);
}


coxmatch::coxmatch(
		uint32_t oxm_id, uint8_t value, uint8_t mask) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint8_t))
{
	set_oxm_id(oxm_id);
	set_u8value(value);
	set_u8mask(mask);
}


void
coxmatch::set_u8value(uint8_t value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint8_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
	oxm->byte = value;
}


void
coxmatch::set_u8mask(uint8_t mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint8_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
	oxm->mask = mask;
}


uint8_t
coxmatch::get_u8value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint8_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
	return oxm->byte;
}


uint8_t
coxmatch::get_u8mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint8_t))
		return 0xff;
	struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
	return oxm->mask;
}


uint8_t
coxmatch::get_u8masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint8_t))
		return get_u8value();
	struct rofl::openflow::ofp_oxm_ofb_uint8_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint8_t*)somem();
	return (oxm->byte & oxm->mask);
}



/*
 * 2 bytes
 */
coxmatch::coxmatch(
		uint32_t oxm_id, uint16_t value) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint16_t))
{
	set_oxm_id(oxm_id);
	set_u16value(value);
}


coxmatch::coxmatch(
		uint32_t oxm_id, uint16_t value, uint16_t mask) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint16_t))
{
	set_oxm_id(oxm_id);
	set_u16value(value);
	set_u16mask(mask);
}


void
coxmatch::set_u16value(uint16_t value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint16_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
	oxm->word = htobe16(value);
}


void
coxmatch::set_u16mask(uint16_t mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint16_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
	oxm->mask = htobe16(mask);
}


uint16_t
coxmatch::get_u16value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint16_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
	return be16toh(oxm->word);
}


uint16_t
coxmatch::get_u16mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint16_t))
		return 0xffff;
	struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
	return be16toh(oxm->mask);
}


uint16_t
coxmatch::get_u16masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint16_t))
		return get_u16value();
	struct rofl::openflow::ofp_oxm_ofb_uint16_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint16_t*)somem();
	return be16toh((oxm->word & oxm->mask));
}



/*
 * 4 bytes
 */
coxmatch::coxmatch(
		uint32_t oxm_id, uint32_t value) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint32_t))
{
	set_oxm_id(oxm_id);
	set_u32value(value);
}


coxmatch::coxmatch(
		uint32_t oxm_id, uint32_t value, uint32_t mask) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
{
	set_oxm_id(oxm_id);
	set_u32value(value);
	set_u32mask(mask);
}


void
coxmatch::set_u32value(uint32_t value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint32_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	oxm->dword = htobe32(value);
}


void
coxmatch::set_u32mask(uint32_t mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	oxm->mask = htobe32(mask);
}


uint32_t
coxmatch::get_u32value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint32_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	return be32toh(oxm->dword);
}


uint32_t
coxmatch::get_u32mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		return 0xffffffff;
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	return be32toh(oxm->mask);
}


uint32_t
coxmatch::get_u32masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		return get_u32value();
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	return be32toh((oxm->dword & oxm->mask));
}


void
coxmatch::set_u32value(rofl::caddress const& addr) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint32_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	if (not addr.is_af_inet())
		throw eOxmInval();
	oxm->dword = addr.ca_s4addr->sin_addr.s_addr;
}


void
coxmatch::set_u32mask(rofl::caddress const& mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	if (not mask.is_af_inet())
		throw eOxmInval();
	oxm->mask = mask.ca_s4addr->sin_addr.s_addr;
}


rofl::caddress
coxmatch::get_u32value_as_addr() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint32_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	caddress addr(AF_INET);
	addr.ca_s4addr->sin_family = AF_INET;
	addr.ca_s4addr->sin_addr.s_addr = oxm->dword;
	return addr;
}


rofl::caddress
coxmatch::get_u32mask_as_addr() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		return rofl::caddress(AF_INET, "255.255.255.255");
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	caddress addr(AF_INET);
	addr.ca_s4addr->sin_family = AF_INET;
	addr.ca_s4addr->sin_addr.s_addr = oxm->mask;
	return addr;
}


rofl::caddress
coxmatch::get_u32masked_value_as_addr() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint32_t))
		return get_u32value_as_addr();
	struct rofl::openflow::ofp_oxm_ofb_uint32_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint32_t*)somem();
	caddress addr(AF_INET);
	addr.ca_s4addr->sin_family = AF_INET;
	addr.ca_s4addr->sin_addr.s_addr = (oxm->dword & oxm->mask);
	return addr;
}




/*
 * 8 bytes
 */
coxmatch::coxmatch(
		uint32_t oxm_id, uint64_t value) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint64_t))
{
	set_oxm_id(oxm_id);
	set_u64value(value);
}


coxmatch::coxmatch(
		uint32_t oxm_id, uint64_t value, uint64_t mask) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint64_t))
{
	set_oxm_id(oxm_id);
	set_u64value(value);
	set_u64mask(mask);
}


void
coxmatch::set_u64value(uint64_t value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint64_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
	oxm->word = htobe64(value);
}


void
coxmatch::set_u64mask(uint64_t mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint64_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
	oxm->mask = htobe64(mask);
}


uint64_t
coxmatch::get_u64value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + sizeof(uint64_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
	return be64toh(oxm->word);
}


uint64_t
coxmatch::get_u64mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint64_t))
		return 0xffffffffffffffffULL;
	struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
	return be64toh(oxm->mask);
}


uint64_t
coxmatch::get_u64masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*sizeof(uint64_t))
		return get_u64value();
	struct rofl::openflow::ofp_oxm_ofb_uint64_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint64_t*)somem();
	return be64toh((oxm->word & oxm->mask));
}



/*
 * 6 bytes
 */
coxmatch::coxmatch(
		uint32_t oxm_id, rofl::cmacaddr const& value) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + OFP_ETH_ALEN)
{
	set_oxm_id(oxm_id);
	set_u48value(value);
}


coxmatch::coxmatch(
		uint32_t oxm_id, rofl::cmacaddr const& value, rofl::cmacaddr const& mask) :
				rofl::cmemory(sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*OFP_ETH_ALEN)
{
	set_oxm_id(oxm_id);
	set_u48value(value);
	set_u48mask(mask);
}


void
coxmatch::set_u48value(rofl::cmacaddr const& value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + OFP_ETH_ALEN)
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	memcpy(oxm->value, value.somem(), OFP_ETH_ALEN);
}


void
coxmatch::set_u48mask(rofl::cmacaddr const& mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*OFP_ETH_ALEN)
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	memcpy(oxm->mask, mask.somem(), OFP_ETH_ALEN);
}


rofl::cmacaddr
coxmatch::get_u48value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + OFP_ETH_ALEN)
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	cmacaddr addr;
	memcpy(addr.somem(), oxm->value, OFP_ETH_ALEN);
	return addr;
}


rofl::cmacaddr
coxmatch::get_u48mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*OFP_ETH_ALEN)
		return rofl::cmacaddr("ff:ff:ff:ff:ff:ff");
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	cmacaddr mask;
	memcpy(mask.somem(), oxm->mask, OFP_ETH_ALEN);
	return mask;
}


rofl::cmacaddr
coxmatch::get_u48masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*OFP_ETH_ALEN)
		return get_u48value();
	struct rofl::openflow::ofp_oxm_ofb_uint48_t* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_uint48_t*)somem();
	cmacaddr addr;
	memcpy(addr.somem(), oxm->value, OFP_ETH_ALEN);
	cmacaddr mask;
	memcpy(mask.somem(), oxm->mask, OFP_ETH_ALEN);
	return (addr & mask);
}


/*
 * 16 bytes
 */

//
// for caddress based constructors, see above
//

void
coxmatch::set_u128value(rofl::caddress const& value) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 16*sizeof(uint8_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
	memcpy(oxm->addr, value.ca_s6addr->sin6_addr.s6_addr, 16);
}


void
coxmatch::set_u128mask(rofl::caddress const& mask) {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
	memcpy(oxm->mask, mask.ca_s6addr->sin6_addr.s6_addr, 16);
}


rofl::caddress
coxmatch::get_u128value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 16*sizeof(uint8_t))
		throw eOxmInval();
	struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
	caddress addr(AF_INET6);
	addr.ca_s6addr->sin6_family = AF_INET6;
	memcpy(addr.ca_s6addr->sin6_addr.s6_addr, oxm->addr, 16);
	return addr;
}


rofl::caddress
coxmatch::get_u128mask() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t))
		return rofl::caddress(AF_INET6, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
	struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
	caddress mask(AF_INET6);
	mask.ca_s6addr->sin6_family = AF_INET6;
	memcpy(mask.ca_s6addr->sin6_addr.s6_addr, oxm->mask, 16);
	return mask;
}


rofl::caddress
coxmatch::get_u128masked_value() const {
	if (memlen() < sizeof(struct rofl::openflow::ofp_oxm_hdr) + 2*16*sizeof(uint8_t))
		return get_u128value();
	struct rofl::openflow::ofp_oxm_ofb_ipv6_addr* oxm =
			(struct rofl::openflow::ofp_oxm_ofb_ipv6_addr*)somem();
	caddress addr(AF_INET6);
	addr.ca_s6addr->sin6_family = AF_INET6;
	memcpy(addr.ca_s6addr->sin6_addr.s6_addr, oxm->addr, 16);
	caddress mask(AF_INET6);
	mask.ca_s6addr->sin6_family = AF_INET6;
	memcpy(mask.ca_s6addr->sin6_addr.s6_addr, oxm->mask, 16);
	return (addr & mask);
}




size_t
coxmatch::length() const {
	return memlen();
}


void
coxmatch::pack(
		uint8_t* buf,
		size_t buflen) {
	if (buflen < length()) {
		throw eOxmInval();
	}
	memcpy(buf, somem(), memlen());
}


void
coxmatch::unpack(
		uint8_t* buf,
		size_t buflen) {
	clear();
	if (buflen > memlen()) {
		resize(buflen);
	}
	assign(buf, buflen);
}


void
coxmatch::set_oxm_class(
		uint16_t oxm_class) {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	oxm->oxm_class = htobe16(oxm_class);
}


uint16_t
coxmatch::get_oxm_class() const {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	return be16toh(oxm->oxm_class);
}


void
coxmatch::set_oxm_field(
		uint8_t oxm_field) {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	oxm->oxm_field = (oxm->oxm_field & 0x01) | ((oxm_field & 0x7f) << 1);
}


uint8_t
coxmatch::get_oxm_field() const {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	return ((oxm->oxm_field) >> 1);
}


void
coxmatch::set_oxm_hasmask(
		bool hasmask) {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	oxm->oxm_field = (oxm->oxm_field & 0xfe) | hasmask;
}


bool
coxmatch::get_oxm_hasmask() const {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	return (oxm->oxm_field & 0x01);
}


void
coxmatch::set_oxm_length(
		uint8_t oxm_len) {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	oxm->oxm_length = oxm_len;
}


uint8_t
coxmatch::get_oxm_length() {
	struct rofl::openflow::ofp_oxm_hdr* oxm =
			(struct rofl::openflow::ofp_oxm_hdr*)somem();
	return oxm->oxm_length;
}




