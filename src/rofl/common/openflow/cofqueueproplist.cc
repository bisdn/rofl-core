/*
 * cofqueueproplist.cc
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */

#include "cofqueueproplist.h"


using namespace rofl;


cofqueueproplist::cofqueueproplist(
		uint8_t of_version) :
			of_version(of_version)
{

}



cofqueueproplist::~cofqueueproplist()
{

}



cofqueueproplist::cofqueueproplist(
		cofqueueproplist const& qpl) :
			of_version(qpl.of_version)
{
	*this = qpl;
}



cofqueueproplist&
cofqueueproplist::operator= (
		cofqueueproplist const& qpl)
{
	if (this == &qpl)
		return *this;

	coflist<cofqueue_prop>::operator= (qpl);
	of_version = qpl.of_version;

	return *this;
}



size_t
cofqueueproplist::length() const
{
	size_t len = 0;
	for (coflist<cofqueue_prop>::const_iterator
			it = begin(); it != end(); ++it) {
		len += (*it).length();
	}
	return len;
}



void
cofqueueproplist::unpack(
		uint8_t *buf, size_t buflen)
{
	coflist<cofqueue_prop>::clear();

	switch (of_version) {
	case OFP10_VERSION:
	case OFP12_VERSION: {

		// both OFP10 and OFP12 use the same generic queue property header
		while (buflen > sizeof(struct ofp12_queue_prop_header)) {

			struct ofp12_queue_prop_header *qp = (struct ofp12_queue_prop_header*)buf;

			if (buflen < be16toh(qp->len))
				throw eInval();

			cofqueue_prop queue_prop(of_version);
			queue_prop.unpack(buf, be16toh(qp->len));
			next() = queue_prop;

			buflen -= be16toh(qp->len);
			buf += be16toh(qp->len);
		}
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}




void
cofqueueproplist::pack(
		uint8_t *buf,
		size_t buflen) const
{
	if (buflen < length())
		throw eInval();

	switch (of_version) {
	case OFP10_VERSION:
	case OFP12_VERSION: {

		for (coflist<cofqueue_prop>::const_iterator
				it = begin(); it != end(); ++it) {
			(*it).pack(buf, (*it).length());
			buf += (*it).length();
		}
	} break;
	case OFP13_VERSION: {
		throw eNotImplemented();
	} break;
	default: {
		throw eBadVersion();
	} break;
	}
}



cofqueue_prop&
cofqueueproplist::find_queue_prop(
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



