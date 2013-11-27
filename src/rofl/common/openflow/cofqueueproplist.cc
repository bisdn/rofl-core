/*
 * cofqueueproplist.cc
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */

#include "cofqueueproplist.h"


using namespace rofl;


cofqueue_prop_list::cofqueue_prop_list(
		uint8_t of_version) :
			of_version(of_version)
{

}



cofqueue_prop_list::~cofqueue_prop_list()
{

}



cofqueue_prop_list::cofqueue_prop_list(
		cofqueue_prop_list const& qpl) :
			of_version(qpl.of_version)
{
	*this = qpl;
}



cofqueue_prop_list&
cofqueue_prop_list::operator= (
		cofqueue_prop_list const& qpl)
{
	if (this == &qpl)
		return *this;

	coflist<cofqueue_prop>::operator= (qpl);
	of_version = qpl.of_version;

	return *this;
}



size_t
cofqueue_prop_list::length() const
{
	size_t len = 0;
	for (coflist<cofqueue_prop>::const_iterator
			it = begin(); it != end(); ++it) {
		len += (*it).length();
	}
	return len;
}



void
cofqueue_prop_list::unpack(
		uint8_t *buf, size_t buflen)
{
	coflist<cofqueue_prop>::clear();

	switch (of_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION: {

		// both openflow10::OFP and openflow12::OFP use the same generic queue property header
		while (buflen > sizeof(struct openflow12::ofp_queue_prop_header)) {

			struct openflow12::ofp_queue_prop_header *qp = (struct openflow12::ofp_queue_prop_header*)buf;

			if (buflen < be16toh(qp->len))
				throw eInval();

			cofqueue_prop queue_prop(of_version);
			queue_prop.unpack(buf, be16toh(qp->len));
			next() = queue_prop;

			buflen -= be16toh(qp->len);
			buf += be16toh(qp->len);
		}
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}




void
cofqueue_prop_list::pack(
		uint8_t *buf,
		size_t buflen) const
{
	if (buflen < length())
		throw eInval();

	switch (of_version) {
	case openflow10::OFP_VERSION:
	case openflow12::OFP_VERSION: {

		for (coflist<cofqueue_prop>::const_iterator
				it = begin(); it != end(); ++it) {
			(*it).pack(buf, (*it).length());
			buf += (*it).length();
		}
	} break;
	case openflow13::OFP_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}



cofqueue_prop&
cofqueue_prop_list::find_queue_prop(
		uint16_t property)
{
	for (coflist<cofqueue_prop>::iterator
			it = begin(); it != end(); ++it) {
		if ((*it).get_property() == property) {
			return (*it);
			// use it like this: cofqueue_prop_min_rate min_rate(queue_prop_list.find_queue_prop(OFPQT_MIN_RATE));
			// to get an instance of type cofqueue_prop_min_rate with the appropriate helper methods
		}
	}
	throw eQueuePropNotFound();
}



