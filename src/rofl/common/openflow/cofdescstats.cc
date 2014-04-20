#include "rofl/common/openflow/cofdescstats.h"

using namespace rofl::openflow;


cofdesc_stats_reply::cofdesc_stats_reply(
		uint8_t of_version,
		std::string const& mfr_desc,
		std::string const& hw_desc,
		std::string const& sw_desc,
		std::string const& serial_num,
		std::string const& dp_desc) :
				of_version(of_version),
				mfr_desc(mfr_desc),
				hw_desc(hw_desc),
				sw_desc(sw_desc),
				serial_num(serial_num),
				dp_desc(dp_desc)
{}



cofdesc_stats_reply::~cofdesc_stats_reply()
{}



cofdesc_stats_reply::cofdesc_stats_reply(
		cofdesc_stats_reply const& descstats)
{
	*this = descstats;
}



cofdesc_stats_reply&
cofdesc_stats_reply::operator= (
		cofdesc_stats_reply const& descstats)
{
	if (this == &descstats)
		return *this;

	of_version 	= descstats.of_version;
	mfr_desc	= descstats.mfr_desc;
	hw_desc		= descstats.hw_desc;
	sw_desc		= descstats.sw_desc;
	serial_num	= descstats.serial_num;
	dp_desc		= descstats.dp_desc;

	return *this;
}



void
cofdesc_stats_reply::set_version(uint8_t of_version)
{
	this->of_version = of_version;
}



uint8_t
cofdesc_stats_reply::get_version() const
{
	return of_version;
}



std::string
cofdesc_stats_reply::get_mfr_desc() const
{
	return mfr_desc;
}



std::string
cofdesc_stats_reply::get_hw_desc() const
{
	return hw_desc;
}



std::string
cofdesc_stats_reply::get_sw_desc() const
{
	return sw_desc;
}



std::string
cofdesc_stats_reply::get_serial_num() const
{
	return serial_num;
}



std::string
cofdesc_stats_reply::get_dp_desc() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return dp_desc;
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofdesc_stats_reply::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct openflow10::ofp_desc_stats))
			throw eInval();

		struct openflow10::ofp_desc_stats *desc = (struct openflow10::ofp_desc_stats*)buf;

		snprintf(desc->mfr_desc, 	DESC_STR_LEN, mfr_desc.c_str(), 	mfr_desc.length());
		snprintf(desc->hw_desc,  	DESC_STR_LEN, hw_desc.c_str(), 		hw_desc.length());
		snprintf(desc->sw_desc,  	DESC_STR_LEN, sw_desc.c_str(), 		sw_desc.length());
		snprintf(desc->serial_num, 	SERIAL_NUM_LEN, serial_num.c_str(), serial_num.length());
		snprintf(desc->dp_desc, 	DESC_STR_LEN, dp_desc.c_str(), 		dp_desc.length());
	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct openflow12::ofp_desc_stats))
			throw eInval();

		struct openflow12::ofp_desc_stats *desc = (struct openflow12::ofp_desc_stats*)buf;

		snprintf(desc->mfr_desc, 	DESC_STR_LEN, mfr_desc.c_str(), 	mfr_desc.length());
		snprintf(desc->hw_desc,  	DESC_STR_LEN, hw_desc.c_str(), 		hw_desc.length());
		snprintf(desc->sw_desc,  	DESC_STR_LEN, sw_desc.c_str(), 		sw_desc.length());
		snprintf(desc->serial_num, 	SERIAL_NUM_LEN, serial_num.c_str(), serial_num.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofdesc_stats_reply::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		if (buflen < sizeof(struct openflow10::ofp_desc_stats))
			throw eInval();

		struct openflow10::ofp_desc_stats *desc = (struct openflow10::ofp_desc_stats*)buf;

		mfr_desc.assign(desc->mfr_desc, DESC_STR_LEN);
		hw_desc.assign(desc->hw_desc, DESC_STR_LEN);
		sw_desc.assign(desc->sw_desc, DESC_STR_LEN);
		serial_num.assign(desc->serial_num, SERIAL_NUM_LEN);
		dp_desc.assign(desc->dp_desc, DESC_STR_LEN);

	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		if (buflen < sizeof(struct openflow12::ofp_desc_stats))
			throw eInval();

		struct openflow12::ofp_desc_stats *desc = (struct openflow12::ofp_desc_stats*)buf;

		mfr_desc.assign(desc->mfr_desc, DESC_STR_LEN);
		hw_desc.assign(desc->hw_desc, DESC_STR_LEN);
		sw_desc.assign(desc->sw_desc, DESC_STR_LEN);
		serial_num.assign(desc->serial_num, SERIAL_NUM_LEN);

	} break;
	default:
		throw eBadVersion();
	}
}



size_t
cofdesc_stats_reply::length() const
{
	switch (of_version) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct openflow10::ofp_desc_stats));
	} break;
	case rofl::openflow12::OFP_VERSION:
	case rofl::openflow13::OFP_VERSION: {
		return (sizeof(struct openflow12::ofp_desc_stats));
	} break;
	default:
		throw eBadVersion();
	}
	return 0;
}



void
cofdesc_stats_reply::clear()
{
	mfr_desc.clear();
	hw_desc.clear();
	sw_desc.clear();
	serial_num.clear();
	dp_desc.clear();
}




