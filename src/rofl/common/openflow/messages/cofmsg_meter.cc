#include "rofl/common/openflow/messages/cofmsg_meter.h"

using namespace rofl::openflow;

cofmsg_meter::cofmsg_meter(
		uint8_t of_version,
		uint32_t xid,
		const rofl::openflow::cofmeter_bands& meter_bands) :
				cofmsg(sizeof(struct rofl::openflow::ofp_header)),
				command(0),
				flags(0),
				meter_id(0),
				meter_bands(meter_bands)
{
	this->meter_bands.set_version(of_version);

	set_version(of_version);
	set_xid(xid);

	switch (of_version) {
	case rofl::openflow13::OFP_VERSION: {
		set_type(rofl::openflow13::OFPT_METER_MOD);

	} break;
	default:
		throw eBadVersion();
	}
}



cofmsg_meter::cofmsg_meter(
		cmemory *memarea) :
	cofmsg(memarea),
	command(0),
	flags(0),
	meter_id(0),
	meter_bands(get_version())
{

}



cofmsg_meter::cofmsg_meter(
		cofmsg_meter const& msg)
{
	*this = msg;
}



cofmsg_meter&
cofmsg_meter::operator= (
		cofmsg_meter const& msg)
{
	if (this == &msg)
		return *this;

	cofmsg::operator= (msg);

	command		= msg.command;
	flags		= msg.flags;
	meter_id	= msg.meter_id;
	meter_bands = msg.meter_bands;

	return *this;
}



cofmsg_meter::~cofmsg_meter()
{

}



void
cofmsg_meter::reset()
{
	command = 0;
	flags = 0;
	meter_id = 0;
	meter_bands.clear();
	cofmsg::reset();
}



size_t
cofmsg_meter::length() const
{
	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct rofl::openflow13::ofp_meter_mod) + meter_bands.length());
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofmsg_meter::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < length())
		throw eInval();

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {

		memcpy(buf, soframe(), sizeof(struct rofl::openflow13::ofp_header));

		struct rofl::openflow13::ofp_meter_mod *mh =
				(struct rofl::openflow13::ofp_meter_mod*)buf;

		mh->command		= htobe16(command);
		mh->flags		= htobe16(flags);
		mh->meter_id	= htobe32(meter_id);

		meter_bands.pack(buf +
				sizeof(struct rofl::openflow13::ofp_meter_mod),
				meter_bands.length());

	} break;
	default:
		throw eBadVersion();
	}
}



void
cofmsg_meter::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_meter::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	meter_bands.clear();

	switch (get_version()) {
	case rofl::openflow13::OFP_VERSION: {

		if (get_length() < sizeof(struct rofl::openflow13::ofp_meter_mod))
			throw eBadSyntaxTooShort();

		if (framelen() < sizeof(struct rofl::openflow13::ofp_meter_mod))
			throw eBadSyntaxTooShort();

		struct rofl::openflow13::ofp_meter_mod *mh =
				(struct rofl::openflow13::ofp_meter_mod*)soframe();

		command 	= be16toh(mh->command);
		flags		= be16toh(mh->flags);
		meter_id	= be32toh(mh->meter_id);


		if (framelen() <= sizeof(struct rofl::openflow13::ofp_meter_mod))
			return;

		unsigned int meter_bands_len =
				framelen() - sizeof(struct rofl::openflow13::ofp_meter_mod);

		meter_bands.unpack(soframe() +
				sizeof(struct rofl::openflow13::ofp_meter_mod),
				meter_bands_len);

	} break;
	default:
		throw eBadRequestBadVersion();
	}
}








