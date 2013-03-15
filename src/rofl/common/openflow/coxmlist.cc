/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * coxmlist.cc
 *
 *  Created on: 10.07.2012
 *      Author: andreas
 */


#include "coxmlist.h"

using namespace rofl;

coxmlist::coxmlist() :
	area(OFPXMT_OFB_MAX * sizeof(coxmatch*)),
	oxmvec((coxmatch**)area.somem())
{
	clear();
	//WRITELOG(COXMLIST, DBG, "coxmlist(%p)::coxmlist()", this);
}


coxmlist::~coxmlist()
{
	clear();
	//WRITELOG(COXMLIST, DBG, "coxmlist(%p)::~coxmlist()", this);
}


coxmlist::coxmlist(
		coxmlist const& oxmlist) :
	area(OFPXMT_OFB_MAX * sizeof(coxmatch*)),
	oxmvec((coxmatch**)area.somem())
{
	*this = oxmlist;
}


coxmlist&
coxmlist::operator= (
		coxmlist const& oxmlist)
{
	if (this == &oxmlist)
		return *this;

	clear();

	for (unsigned int i = 0; i < OFPXMT_OFB_MAX; i++)
	{
             if ((coxmatch*)0 == oxmlist.oxmvec[i])
             {
                    continue;
             }
             oxmvec[i] = new coxmatch(*(oxmlist.oxmvec[i]));
	}

	return *this;
}



bool
coxmlist::operator< (
		coxmlist const& oxm) const
{
	for (unsigned int i = 0; i < OFPXMT_OFB_MAX; i++)
	{
		if ((0 == oxmvec[i]) && (0 == oxm.oxmvec[i]))
		{
			continue;
		}
		else if ((0 != oxmvec[i]) && (0 != oxm.oxmvec[i]))
		{
			if (*(oxmvec[i]) == *(oxm.oxmvec[i]))
			{
				continue;
			}
			else
			{
				return (*(oxmvec[i]) < *(oxm.oxmvec[i]));
			}
		}
		else if (0 != oxmvec[i])
		{
			return false;
		}
		else if (0 != oxm.oxmvec[i])
		{
			return true;
		}
	}
	return false;
}



void
coxmlist::clear()
{
	for (unsigned int i = 0; i < OFPXMT_OFB_MAX; ++i)
	{
		if ((coxmatch*)0 != oxmvec[i])
		{
			delete oxmvec[i];
			oxmvec[i] = (coxmatch*)0;
		}
	}
}


size_t
coxmlist::size() const
{
	size_t cnt = 0;
	for (unsigned int i = 0; i < OFPXMT_OFB_MAX; ++i)
	{
		if ((coxmatch*)0 == oxmvec[i])
		{
			continue;
		}
		cnt++;
	}
	return cnt;
}


bool
coxmlist::operator== (coxmlist& oxmlist)
{
	return overlap(oxmlist, true /* strict */);
}


coxmatch&
coxmlist::operator[] (
		size_t index) throw (eOxmListOutOfRange)
{
	if (index >= OFPXMT_OFB_MAX)
	{
		throw eOxmListOutOfRange();
	}

	if ((coxmatch*)0 == oxmvec[index])
	{
		oxmvec[index] = new coxmatch();
	}

	return *oxmvec[index];
}



void
coxmlist::unpack(
		struct ofp_oxm_hdr* oxm_hdr,
		size_t oxm_len)
{
	clear(); // clears oxmvec

	// sanity check: oxm_len must be of size at least of ofp_oxm_hdr
	if (oxm_len < (int)sizeof(struct ofp_oxm_hdr))
	{
		throw eBadMatchBadLen();
	}

	// first instruction
	struct ofp_oxm_hdr *hdr = oxm_hdr;


	while (oxm_len > 0)
	{
		if ((oxm_len < sizeof(struct ofp_oxm_hdr)) || (0 == hdr->oxm_length))
		{
			return; // not enough bytes to parse an entire ofp_oxm_hdr, possibly padding bytes found
		}

		coxmatch oxm(hdr, sizeof(struct ofp_oxm_hdr) + hdr->oxm_length);

		if (oxm.get_oxm_field() >= OFPXMT_OFB_MAX)
		{
			throw eBadMatchBadField();
		}

		(*this)[oxm.get_oxm_field()] = oxm;

		oxm_len -= (sizeof(struct ofp_oxm_hdr) + hdr->oxm_length);
		hdr = (struct ofp_oxm_hdr*)(((uint8_t*)hdr) + sizeof(struct ofp_oxm_hdr) + hdr->oxm_length);
	}
}


struct ofp_oxm_hdr*
coxmlist::pack(
		struct ofp_oxm_hdr* oxm_hdr,
		size_t oxm_len) const
{
	size_t needed_oxm_len = length();

	if (oxm_len < needed_oxm_len)
	{
		throw eBadMatchBadLen();
	}

	struct ofp_oxm_hdr *hdr = oxm_hdr; // first oxm header

	for (unsigned int i = 0; i < OFPXMT_OFB_MAX; i++)
	{
		if ((coxmatch*)0 == oxmvec[i])
		{
			continue;
		}

		coxmatch& match = *(oxmvec[i]);

		hdr = (struct ofp_oxm_hdr*)
					((uint8_t*)(match.pack(hdr, match.length())) + match.length());
	}

	return oxm_hdr;
}


size_t
coxmlist::length() const
{
	size_t oxm_len = 0;
	for (unsigned int i = 0; i < OFPXMT_OFB_MAX; i++)
	{
		if ((coxmatch*)0 == oxmvec[i])
		{
			continue;
		}
		oxm_len += oxmvec[i]->length();
	}
	return oxm_len;
}


const char*
coxmlist::c_str()
{
	cvastring vas(4096);
	info.assign(vas("coxmlist(%p) length:%d => match(es): ",
			this, length()));
	for (unsigned int i = 0; i < OFPXMT_OFB_MAX; i++)
	{
		if ((coxmatch*)0 == oxmvec[i])
		{
			continue;
		}
		info.append(vas("\n  [%02d] %s ", oxmvec[i]->get_oxm_field(), oxmvec[i]->c_str()));
	}

	return info.c_str();
}


bool
coxmlist::exists(
		uint16_t oxm_class,
		uint8_t oxm_field) const
{
	return ((coxmatch*)0 != oxmvec[oxm_field]);
}


coxmatch&
coxmlist::oxm_find(
		uint16_t oxm_class,
		uint8_t oxm_field)
			const throw (eOxmListNotFound)
{
#if 0
#ifndef NDEBUG
	for (coxmlist::iterator
			it = elems.begin(); it != elems.end(); ++it)
	{
		WRITELOG(COXMLIST, DBG, "coxmlist(%p)::oxm_find() %d => %s",
				this, elems.size(), (*it).c_str());
	}
#endif
#endif


	if (not exists(oxm_class, oxm_field))
	{
		WRITELOG(COXMLIST, DBG, "coxmlist(%p)::oxm_find() class:0x%x field:%d not found",
						this, oxm_class, oxm_field);

		throw eOxmListNotFound();
	}

	coxmatch oxm((*this)[oxm_field]);
	WRITELOG(COXMLIST, DBG, "coxmlist(%p)::oxm_find() class:0x%x field:%d found => %s",
					this, oxm_class, oxm_field, oxm.c_str());

	return (*this)[oxm_field];
}


coxmatch
coxmlist::oxm_copy(
		uint16_t oxm_class,
		uint8_t oxm_field)
			throw (eOxmListNotFound)
{
	if (not exists(oxm_class, oxm_field))
	{
		WRITELOG(COXMLIST, DBG, "coxmlist(%p)::oxm_copy() class:0x%x field:%d not found",
						this, oxm_class, oxm_field);

		throw eOxmListNotFound();
	}

	coxmatch oxm((*this)[oxm_field]);
	WRITELOG(COXMLIST, DBG, "coxmlist(%p)::oxm_copy() class:0x%x field:%d found => %s",
					this, oxm_class, oxm_field, oxm.c_str());

	return oxm;
}


void
coxmlist::erase(
		uint16_t oxm_class,
		uint8_t oxm_field)
{
	if (not exists(oxm_class, oxm_field))
	{
		return;
	}

	delete oxmvec[oxm_field];

	oxmvec[oxm_field] = (coxmatch*)0;
}


void
coxmlist::oxm_replace_or_insert(
		uint16_t oxm_class,
		uint8_t oxm_field,
		uint32_t dword)
{
	(*this)[oxm_field].oxm_uint32t->dword = htobe32(dword);
}


void
coxmlist::oxm_replace_or_insert(
		uint16_t oxm_class,
		uint8_t oxm_field,
		uint16_t word)
{

	(*this)[oxm_field].oxm_uint16t->word = htobe16(word);
}


void
coxmlist::oxm_replace_or_insert(
		uint16_t oxm_class,
		uint8_t oxm_field,
		uint8_t byte)
{
	(*this)[oxm_field].oxm_uint8t->byte = byte;
}


bool
coxmlist::overlap(
	coxmlist const& oxmlist,
	bool strict /* = false (default) */)
{
	/*
	 * strict:
	 * all elemens in *this and oxmlist must be identical (number of and value of elements)
	 *
	 * non-strict:
	 * all elements in oxmlist must be present in *this (value of elements)
	 */

	coxmlist debug(oxmlist);
	WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() COMPARING [%s]\nus:%s vs.\nthem:%s",
			this, (strict) ? "strict" : "non-strict", c_str(), debug.c_str());

	if (strict)
	{
		WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() EEEEEEEEEEEEEEEEEEEEEEE [strict]", this);

		if (size() != oxmlist.size()) // same # of elements?
		{
			return false;
		}

		for (unsigned int i = 0; i < OFPXMT_OFB_MAX; ++i)
		{
			WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() us.oxmvec[%d]:%p "
					"them.oxmvec[%d]:%p", this, i, oxmvec[i], i, oxmlist.oxmvec[i]);
			if (((coxmatch*)0 == oxmvec[i]) && ((coxmatch*)0 == oxmlist.oxmvec[i]))
			{
				WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() i:%d both 0", this, i);
				continue;
			}
			else if ((coxmatch*)0 == oxmvec[i])
			{
				WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() i:%d us 0", this, i);
				return false;
			}
			else if ((coxmatch*)0 == oxmlist.oxmvec[i])
			{
				WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() i:%d them 0", this, i);
				return false;
			}

			coxmatch& m1 = *(oxmvec[i]);

			coxmatch& m2 = *(oxmlist.oxmvec[i]);


			if (not (m1 == m2))
			{
				WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() "
						"m1 != m2 => m1: %s m2: %s", this, m1.c_str(), m2.c_str());
				return false;
			}
			else
			{
				WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() m1 == m2", this);
			}
		}


	}
	else /* non-strict */
	{
		WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() EEEEEEEEEEEEEEEEEEEEEEE [non-strict]", this);

		for (unsigned int i = 0; i < OFPXMT_OFB_MAX; i++)
		{
			if ((coxmatch*)0 == oxmlist.oxmvec[i])
			{
				continue; // wildcard match
			}

			coxmatch& m2 = *(oxmlist.oxmvec[i]);

			if ((coxmatch*)0 == oxmvec[m2.get_oxm_field()])
			{
				return false;
			}

			coxmatch& m1 = *(oxmvec[m2.get_oxm_field()]);

			if (not (m1 == m2))
			{
				WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() "
							"m1 != m2 => m1: %s m2: %s", this, m1.c_str(), m2.c_str());
				return false;
			}
			else
			{
				WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() m1 == m2", this);
			}
		}
	}

	WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() MATCHING!", this);

	return true;
}


void
coxmlist::calc_hits(
		coxmlist& other,
		uint16_t& exact_hits,
		uint16_t& wildcard_hits,
		uint16_t& missed)
{
	WRITELOG(COXMLIST, DBG, "coxmlist(%p)::calc_hits()", this);

	for (unsigned int i = 0; i < OFPXMT_OFB_MAX; i++)
	{
		if ((coxmatch*)0 == other.oxmvec[i])
		{
			wildcard_hits++; continue; // wildcard match
		}

		coxmatch& m2 = *(other.oxmvec[i]);

		if ((coxmatch*)0 == oxmvec[m2.get_oxm_field()])
		{
			missed++; return;
		}

		coxmatch& m1 = *(oxmvec[m2.get_oxm_field()]);

		if (not (m1 == m2))
		{
			WRITELOG(COXMLIST, DBG, "coxmlist(%p)::calc_hits() "
						"m1 != m2 => m1: %s m2: %s", this, m1.c_str(), m2.c_str());
			missed++; return;
		}
		else
		{
			WRITELOG(COXMLIST, DBG, "coxmlist(%p)::overlap() m1 == m2", this);
			exact_hits++;
		}
	}
}


void
coxmlist::test()
{
#ifndef NDEBUG
	coxmlist oxmlist;

	oxmlist[OFPXMT_OFB_IN_PORT] = coxmatch_ofb_in_port(48);
	//oxmlist.next() = coxmatch_ofb_in_port(48);
	fprintf(stderr, "IN-PORT added: %s\n\n", oxmlist.c_str());
	//oxmlist.next() = coxmatch_ofb_eth_dst(cmacaddr("11:11:11:11:11:11"));
	oxmlist[OFPXMT_OFB_ETH_DST] = coxmatch_ofb_eth_dst(cmacaddr("11:11:11:11:11:11"));
	fprintf(stderr, "ETH-DST added: %s\n\n", oxmlist.c_str());
	//oxmlist.next() = coxmatch_ofb_eth_src(cmacaddr("22:22:22:22:22:22"), cmacaddr("00:ff:00:ff:00:ff"));
	oxmlist[OFPXMT_OFB_ETH_SRC] = coxmatch_ofb_eth_src(cmacaddr("22:22:22:22:22:22"), cmacaddr("00:ff:00:ff:00:ff"));
	fprintf(stderr, "ETH-SRC added: %s\n\n", oxmlist.c_str());
	//oxmlist.next() = coxmatch_ofb_udp_dst(80);
	oxmlist[OFPXMT_OFB_UDP_DST] = coxmatch_ofb_udp_dst(80);
	fprintf(stderr, "UDP-DST added: %s\n\n", oxmlist.c_str());
	//oxmlist.next() = coxmatch_ofb_ipv4_dst(caddress(AF_INET, "1.1.1.1"));
	oxmlist[OFPXMT_OFB_IPV4_DST] = coxmatch_ofb_ipv4_dst(caddress(AF_INET, "1.1.1.1"));
	fprintf(stderr, "IPV4-DST added: %s\n\n", oxmlist.c_str());
	//oxmlist.next() = coxmatch_ofb_vlan_pcp(3);
	oxmlist[OFPXMT_OFB_VLAN_PCP] = coxmatch_ofb_vlan_pcp(3);
	fprintf(stderr, "VLAN-PCP added: %s\n\n", oxmlist.c_str());

	oxmlist.erase(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_SRC);
	fprintf(stderr, "ETH-SRC removed: %s\n\n", oxmlist.c_str());

	oxmlist.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_DST, (uint32_t)0x51515151);
	fprintf(stderr, "IPV4-DST replaced: %s\n\n", oxmlist.c_str());

	size_t oxm_len = oxmlist.length();
	cmemory mem(oxm_len);

	oxmlist.pack((struct ofp_oxm_hdr*)mem.somem(), mem.memlen());
	fprintf(stderr, "mem: %s\n\n", mem.c_str());

	coxmlist unpacked;
	unpacked.unpack((struct ofp_oxm_hdr*)mem.somem(), mem.memlen());
	fprintf(stderr, "unpacked: %s\n\n", unpacked.c_str());

	coxmlist oxmtest(oxmlist);
	fprintf(stderr, "COPY: oxmlist: %s\n\n", oxmlist.c_str());
	fprintf(stderr, "COPY: oxmtest: %s\n\n", oxmtest.c_str());

	fprintf(stderr, "COPY: oxmlist.overlap(oxmtest): non-strict:%d strict:%d \n\n",
			oxmlist.overlap(oxmtest, false), oxmlist.overlap(oxmtest, true));
	fprintf(stderr, "COPY: oxmtest.overlap(oxmlist): non-strict:%d strict:%d \n\n",
			oxmtest.overlap(oxmlist, false), oxmtest.overlap(oxmlist, true));

	oxmtest.oxm_replace_or_insert(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IN_PORT, (uint32_t)47);
	fprintf(stderr, "PORT: oxmlist: %s\n\n", oxmlist.c_str());
	fprintf(stderr, "PORT: oxmtest: %s\n\n", oxmtest.c_str());

	fprintf(stderr, "PORT: oxmlist.overlap(oxmtest): non-strict:%d strict:%d \n\n",
			oxmlist.overlap(oxmtest, false), oxmlist.overlap(oxmtest, true));
	fprintf(stderr, "PORT: oxmtest.overlap(oxmlist): non-strict:%d strict:%d \n\n",
			oxmtest.overlap(oxmlist, false), oxmtest.overlap(oxmlist, true));

	//oxmtest.next() = coxmatch_ofb_mpls_label(7777);
	oxmtest[OFPXMT_OFB_MPLS_LABEL] = coxmatch_ofb_mpls_label(7777);
	fprintf(stderr, "MPLS: oxmlist: %s\n\n", oxmlist.c_str());
	fprintf(stderr, "MPLS: oxmtest: %s\n\n", oxmtest.c_str());

	fprintf(stderr, "MPLS: oxmlist.overlap(oxmtest): non-strict:%d strict:%d \n\n",
			oxmlist.overlap(oxmtest, false), oxmlist.overlap(oxmtest, true));
	fprintf(stderr, "MPLS: oxmtest.overlap(oxmlist): non-strict:%d strict:%d \n\n",
			oxmtest.overlap(oxmlist, false), oxmtest.overlap(oxmlist, true));

	oxmtest = oxmlist;

	uint16_t exact_hits = 0;
	uint16_t wildcard_hits = 0;
	uint16_t missed = 0;

	oxmtest.calc_hits(oxmlist, exact_hits, wildcard_hits, missed);

	fprintf(stderr, "CALC: ALL-COPY: exact_hits:%d wildcard_hits:%d missed:%d\n\n",
			exact_hits, wildcard_hits, missed);

	oxmtest.erase(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_DST);
	fprintf(stderr, "IPV4-DST-RMVD: oxmlist: %s\n\n", oxmlist.c_str());
	fprintf(stderr, "IPV4-DST-RMVD: oxmtest: %s\n\n", oxmtest.c_str());

	exact_hits = 0; wildcard_hits = 0; missed = 0;
	oxmtest.calc_hits(oxmlist, exact_hits, wildcard_hits, missed);

	fprintf(stderr, "CALC: IPV4-DST removed: exact_hits:%d wildcard_hits:%d missed:%d\n\n",
			exact_hits, wildcard_hits, missed);

#endif
}

