#include "cofmsg_features.h"

using namespace rofl;

cofmsg_features_request::cofmsg_features_request(
		uint8_t of_version,
		uint32_t xid) :
	cofmsg(sizeof(struct openflow::ofp_header))
{
	set_version(of_version);
	set_length(sizeof(struct openflow::ofp_header));
	set_xid(xid);

	switch (of_version) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_FEATURES_REQUEST);
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_FEATURES_REQUEST);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_FEATURES_REQUEST);
	} break;
	}
}



cofmsg_features_request::cofmsg_features_request(
		cmemory *memarea) :
	cofmsg(memarea)
{

}



cofmsg_features_request::cofmsg_features_request(
		cofmsg_features_request const& features_request)
{
	*this = features_request;
}



cofmsg_features_request&
cofmsg_features_request::operator= (
		cofmsg_features_request const& features_request)
{
	if (this == &features_request)
		return *this;

	cofmsg::operator =(features_request);

	return *this;
}



cofmsg_features_request::~cofmsg_features_request()
{

}



void
cofmsg_features_request::reset()
{
	cofmsg::reset();
}



size_t
cofmsg_features_request::length()
{
	return (sizeof(struct openflow::ofp_header));
}



void
cofmsg_features_request::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eInval();

	cofmsg::pack(buf, buflen);
}



void
cofmsg_features_request::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_features_request::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	switch (get_version()) {
	case openflow10::OFP_VERSION: {

	} break;
	case openflow12::OFP_VERSION: {

	} break;
	case openflow13::OFP_VERSION: {

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}










cofmsg_features_reply::cofmsg_features_reply(
		uint8_t of_version,
		uint32_t xid,
		uint64_t dpid,
		uint32_t n_buffers,
		uint8_t  n_tables,
		uint32_t capabilities,
		uint32_t of10_actions_bitmap,
		uint8_t  of13_auxiliary_id,
		cofportlist const& ports) :
	cofmsg(sizeof(struct openflow::ofp_header)),
	ports(ports)
{
	ofh_switch_features = soframe();

	set_version(of_version);
	set_xid(xid);

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		set_type(openflow10::OFPT_FEATURES_REPLY);
		resize(sizeof(struct openflow10::ofp_switch_features));
		set_length(sizeof(struct openflow10::ofp_switch_features));

		ofh10_switch_features->datapath_id 		= htobe64(dpid);
		ofh10_switch_features->n_buffers 		= htobe32(n_buffers);
		ofh10_switch_features->n_tables 		= n_tables;
		ofh10_switch_features->capabilities 	= htobe32(capabilities);
		ofh10_switch_features->actions			= htobe32(of10_actions_bitmap);
	} break;
	case openflow12::OFP_VERSION: {
		set_type(openflow12::OFPT_FEATURES_REPLY);
		resize(sizeof(struct openflow12::ofp_switch_features));
		set_length(sizeof(struct openflow12::ofp_switch_features));

		ofh12_switch_features->datapath_id 		= htobe64(dpid);
		ofh12_switch_features->n_buffers 		= htobe32(n_buffers);
		ofh12_switch_features->n_tables 		= n_tables;
		ofh12_switch_features->capabilities 	= htobe32(capabilities);
	} break;
	case openflow13::OFP_VERSION: {
		set_type(openflow13::OFPT_FEATURES_REPLY);
		resize(sizeof(struct openflow13::ofp_switch_features));
		set_length(sizeof(struct openflow13::ofp_switch_features));

		ofh13_switch_features->datapath_id 		= htobe64(dpid);
		ofh13_switch_features->n_buffers 		= htobe32(n_buffers);
		ofh13_switch_features->n_tables 		= n_tables;
		ofh13_switch_features->auxiliary_id		= of13_auxiliary_id;
		ofh13_switch_features->capabilities 	= htobe32(capabilities);
	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_features_reply::cofmsg_features_reply(
		cmemory *memarea) :
	cofmsg(memarea)
{
	ofh_switch_features = soframe();
}



cofmsg_features_reply::cofmsg_features_reply(
		cofmsg_features_reply const& features_reply)
{
	*this = features_reply;
}



cofmsg_features_reply&
cofmsg_features_reply::operator= (
		cofmsg_features_reply const& features_reply)
{
	if (this == &features_reply)
		return *this;

	cofmsg::operator =(features_reply);

	ofh_switch_features = soframe();

	ports = features_reply.ports;

	return *this;
}



cofmsg_features_reply::~cofmsg_features_reply()
{

}



void
cofmsg_features_reply::reset()
{
	cofmsg::reset();
	ports.clear();
}



void
cofmsg_features_reply::resize(size_t len)
{
	cofmsg::resize(len);
	ofh_switch_features = soframe();
}



size_t
cofmsg_features_reply::length() const
{
	switch (ofh_header->version) {
	case openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_switch_features) + ports.length());
	} break;
	case openflow12::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_switch_features) + ports.length());
	} break;
	case openflow13::OFP_VERSION: {
		return (sizeof(struct openflow13::ofp_switch_features));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_features_reply::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		memcpy(buf, soframe(), framelen());
		ports.pack((struct openflow10::ofp_port*)(buf + sizeof(struct openflow10::ofp_switch_features)), ports.length());
	} break;
	case openflow12::OFP_VERSION: {
		memcpy(buf, soframe(), framelen());
		ports.pack((struct openflow12::ofp_port*)(buf + sizeof(struct openflow12::ofp_switch_features)), ports.length());
	} break;
	case openflow13::OFP_VERSION: {
		memcpy(buf, soframe(), framelen());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_features_reply::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_features_reply::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	ofh_switch_features = soframe();

	ports.clear();

	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow10::ofp_switch_features))
			throw eBadSyntaxTooShort();
		if (get_length() > sizeof(struct openflow10::ofp_switch_features)) {
			ports.unpack(ofh10_switch_features->ports, get_length() - sizeof(struct openflow10::ofp_switch_features));
		}
	} break;
	case openflow12::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow12::ofp_switch_features))
			throw eBadSyntaxTooShort();
		if (get_length() > sizeof(struct openflow12::ofp_switch_features)) {
			ports.unpack(ofh12_switch_features->ports, get_length() - sizeof(struct openflow12::ofp_switch_features));
		}
	} break;
	case openflow13::OFP_VERSION: {
		if (get_length() < sizeof(struct openflow13::ofp_switch_features))
			throw eBadSyntaxTooShort();
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}




uint64_t
cofmsg_features_reply::get_dpid() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be64toh(ofh10_switch_features->datapath_id);
	} break;
	case openflow12::OFP_VERSION: {
		return be64toh(ofh12_switch_features->datapath_id);
	} break;
	case openflow13::OFP_VERSION: {
		return be64toh(ofh13_switch_features->datapath_id);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_features_reply::set_dpid(uint64_t dpid)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_switch_features->datapath_id = htobe64(dpid);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_switch_features->datapath_id = htobe64(dpid);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_switch_features->datapath_id = htobe64(dpid);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_features_reply::get_n_buffers() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_switch_features->n_buffers);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_switch_features->n_buffers);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_switch_features->n_buffers);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_features_reply::set_n_buffers(uint32_t n_buffers)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_switch_features->n_buffers = htobe32(n_buffers);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_switch_features->n_buffers = htobe32(n_buffers);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_switch_features->n_buffers = htobe32(n_buffers);
	} break;
	default:
		throw eBadVersion();
	}
}



uint8_t
cofmsg_features_reply::get_n_tables() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return (ofh10_switch_features->n_tables);
	} break;
	case openflow12::OFP_VERSION: {
		return (ofh12_switch_features->n_tables);
	} break;
	case openflow13::OFP_VERSION: {
		return (ofh13_switch_features->n_tables);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_features_reply::set_n_tables(uint8_t n_tables)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_switch_features->n_tables = n_tables;
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_switch_features->n_tables = n_tables;
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_switch_features->n_tables = n_tables;
	} break;
	default:
		throw eBadVersion();
	}
}



uint8_t
cofmsg_features_reply::get_auxiliary_id() const
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		return ofh13_switch_features->auxiliary_id;
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_features_reply::set_auxiliary_id(uint8_t auxiliary_id)
{
	switch (get_version()) {
	case openflow13::OFP_VERSION: {
		ofh13_switch_features->auxiliary_id = auxiliary_id;
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_features_reply::get_capabilities() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_switch_features->capabilities);
	} break;
	case openflow12::OFP_VERSION: {
		return be32toh(ofh12_switch_features->capabilities);
	} break;
	case openflow13::OFP_VERSION: {
		return be32toh(ofh13_switch_features->capabilities);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_features_reply::set_capabilities(uint32_t capabilities)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_switch_features->capabilities = htobe32(capabilities);
	} break;
	case openflow12::OFP_VERSION: {
		ofh12_switch_features->capabilities = htobe32(capabilities);
	} break;
	case openflow13::OFP_VERSION: {
		ofh13_switch_features->capabilities = htobe32(capabilities);
	} break;
	default:
		throw eBadVersion();
	}
}



uint32_t
cofmsg_features_reply::get_actions_bitmap() const
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		return be32toh(ofh10_switch_features->actions);
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_features_reply::set_actions_bitmap(uint32_t actions_bitmap)
{
	switch (get_version()) {
	case openflow10::OFP_VERSION: {
		ofh10_switch_features->actions = htobe32(actions_bitmap);
	} break;
	default:
		throw eBadVersion();
	}
}



cofportlist&
cofmsg_features_reply::get_ports()
{
	return ports;
}



