/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "cofactions.h"

using namespace rofl;

cofactions::cofactions(
		uint8_t ofp_version) :
				ofp_version(ofp_version)
{

}


cofactions::cofactions(
		uint8_t ofp_version,
		struct openflow::ofp_action_header *achdr,
		size_t aclen) :
				ofp_version(ofp_version)
{
	unpack(achdr, aclen);
}



cofactions::cofactions(
		cofactions const& aclist)
{
	*this = aclist;
}



cofactions&
cofactions::operator= (
		cofactions const& aclist)
{
	if (this == &aclist)
		return *this;

	this->ofp_version = aclist.ofp_version;
	coflist<cofaction>::operator= (aclist);

	return *this;
}



cofactions::~cofactions()
{

}


std::vector<cofaction>*
cofactions::find_action(uint8_t type,
		std::vector<cofaction> *result)
{
	result->clear();

	// ah, C++ 0x with copy_if(), let us use it ...
	cofactions::iterator it;
	for (it = elems.begin(); it != elems.end(); ++it)
	{
		cofaction& action = (*it);

		if (action.oac_header->type == type)
		{
			result->push_back(action);
		}
	}

	if (result->empty())
	{
		throw eAcListNotFound();
	}

	return result;
}



std::vector<cofaction>&
cofactions::unpack(
		struct openflow::ofp_action_header *achdr,
		size_t aclen)
{
	clear(); // clears elems

	// sanity check: aclen must be of size at least of ofp_action_header
	switch (ofp_version) {
	case openflow10::OFP_VERSION: {
		if (aclen < (int)sizeof(struct openflow10::ofp_action_header))
			return elems;

	} break;
	case openflow12::OFP_VERSION: {
		if (aclen < (int)sizeof(struct openflow12::ofp_action_header))
			return elems;

	} break;
	case openflow13::OFP_VERSION: {
		if (aclen < (int)sizeof(struct openflow13::ofp_action_header))
			return elems;

	} break;
	default:
		throw eBadVersion();
	}


	while (aclen > 0)
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {
			if (be16toh(achdr->len) < sizeof(struct openflow10::ofp_action_header))
				throw eBadActionBadLen();

		} break;
		case openflow12::OFP_VERSION: {
			if (be16toh(achdr->len) < sizeof(struct openflow12::ofp_action_header))
				throw eBadActionBadLen();

		} break;
		case openflow13::OFP_VERSION: {
			if (be16toh(achdr->len) < sizeof(struct openflow13::ofp_action_header))
				throw eBadActionBadLen();

		} break;
		default:
			logging::warn << "[cofactions] method unpack() failed, bad ofp version" << std::endl;
			throw eBadVersion();
		}

		if (0 == be16toh(achdr->len)) {
			throw eBadActionBadLen();
		}

		next() = cofaction(ofp_version, achdr, be16toh(achdr->len) );

		aclen -= be16toh(achdr->len);
		achdr = (struct openflow::ofp_action_header*)(((uint8_t*)achdr) + be16toh(achdr->len));
	}

	return elems;
}


struct openflow::ofp_action_header*
cofactions::pack(
		struct openflow::ofp_action_header *achdr,
		size_t aclen) const
{
	if (aclen < length())
		throw eAcListInval();

	cofactions::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it) {
		achdr = (struct openflow::ofp_action_header*)
				((uint8_t*)((*it).pack(achdr, (*it).length())) + (*it).length());
	}
	return achdr;
}


size_t
cofactions::length() const
{
	size_t len = 0;
	cofactions::const_iterator it;
	for (it = elems.begin(); it != elems.end(); ++it) {
		len += (*it).length();
	}
	return len;
}


cofaction&
cofactions::find_action(uint8_t type)
{
	cofactions::iterator it;
	if ((it = find_if(elems.begin(), elems.end(),
			cofaction_find_type((uint16_t)type))) == elems.end()) {
		throw eAcListNotFound();
	}
	return ((*it));
}


int
cofactions::count_action_type(
		uint16_t type)
{
	int action_cnt = count_if(elems.begin(), elems.end(), cofaction_find_type(type));

	WRITELOG(COFINST, DBG, "cofinst::actions_count_action_type(): %d", (action_cnt));

	return (action_cnt);
}


int
cofactions::count_action_output(
		uint32_t port_no) const
{
	int action_cnt = 0;

	for (cofactions::const_iterator
			it = elems.begin(); it != elems.end(); ++it)
	{
		cofaction action(*it);


		uint32_t out_port = 0;

		switch (ofp_version) {
		case openflow10::OFP_VERSION: {

			if (openflow10::OFPAT_OUTPUT != action.get_type()) {
				continue;
			}
			out_port = be16toh(action.oac_10output->port);

		} break;
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {

			if (openflow12::OFPAT_OUTPUT != action.get_type()) {
				continue;
			}
			out_port = be32toh(action.oac_12output->port);

		} break;
		default:
			throw eBadVersion();
		}

		if ((0 == port_no) || (out_port == port_no))
		{
			action_cnt++;
		}
	}

	return action_cnt;
}


std::list<uint32_t>
cofactions::actions_output_ports()
{
	std::list<uint32_t> outports;

	for (cofactions::iterator
			it = elems.begin(); it != elems.end(); ++it)
	{
		switch (ofp_version) {
		case openflow10::OFP_VERSION: {

			if ((*it).get_type() != openflow10::OFPAT_OUTPUT) {
				continue;
			}
			outports.push_back(be16toh((*it).oac_10output->port));

		} break;
		case openflow12::OFP_VERSION:
		case openflow13::OFP_VERSION: {

			if ((*it).get_type() != openflow12::OFPAT_OUTPUT) {
				continue;
			}
			outports.push_back(be32toh((*it).oac_12output->port));

		} break;
		default:
			throw eBadVersion();
		}
	}
	return outports;
}


