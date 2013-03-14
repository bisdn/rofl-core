#include "rofl/common/openflow/cofdescstats.h"

using namespace rofl;



cofdescstats::cofdescstats(
		uint8_t of_version) :
				of_version(of_version)
{}



cofdescstats::cofdescstats(
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



cofdescstats::~cofdescstats()
{}



cofdescstats::cofdescstats(
		cofdescstats const& descstats)
{
	*this = descstats;
}



cofdescstats&
cofdescstats::operator= (
		cofdescstats const& descstats)
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



std::string
cofdescstats::get_mfr_desc() const
{
	return mfr_desc;
}



std::string
cofdescstats::get_hw_desc() const
{
	return hw_desc;
}



std::string
cofdescstats::get_sw_desc() const
{
	return sw_desc;
}



std::string
cofdescstats::get_serial_num() const
{
	return serial_num;
}



std::string
cofdescstats::get_dp_desc() const
{
	return dp_desc;
}



void
cofdescstats::pack(uint8_t *buf, size_t buflen) const
{
	switch (of_version) {
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_desc_stats))
			throw eInval();

		struct ofp10_desc_stats *desc = (struct ofp10_desc_stats*)buf;

		snprintf(desc->mfr_desc, 	DESC_STR_LEN, mfr_desc.c_str(), 	mfr_desc.length());
		snprintf(desc->hw_desc,  	DESC_STR_LEN, hw_desc.c_str(), 		hw_desc.length());
		snprintf(desc->sw_desc,  	DESC_STR_LEN, sw_desc.c_str(), 		sw_desc.length());
		snprintf(desc->serial_num, 	DESC_STR_LEN, serial_num.c_str(), 	serial_num.length());
		snprintf(desc->dp_desc, 	DESC_STR_LEN, dp_desc.c_str(), 		dp_desc.length());
	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_desc_stats))
			throw eInval();

		struct ofp12_desc_stats *desc = (struct ofp12_desc_stats*)buf;

		snprintf(desc->mfr_desc, 	DESC_STR_LEN, mfr_desc.c_str(), 	mfr_desc.length());
		snprintf(desc->hw_desc,  	DESC_STR_LEN, hw_desc.c_str(), 		hw_desc.length());
		snprintf(desc->sw_desc,  	DESC_STR_LEN, sw_desc.c_str(), 		sw_desc.length());
		snprintf(desc->serial_num, 	DESC_STR_LEN, serial_num.c_str(), 	serial_num.length());
		snprintf(desc->dp_desc, 	DESC_STR_LEN, dp_desc.c_str(), 		dp_desc.length());
	} break;
	default:
		throw eBadVersion();
	}
}



void
cofdescstats::unpack(uint8_t *buf, size_t buflen)
{
	switch (of_version) {
	case OFP10_VERSION: {
		if (buflen < sizeof(struct ofp10_desc_stats))
			throw eInval();

		struct ofp10_desc_stats *desc = (struct ofp10_desc_stats*)buf;

		mfr_desc.assign(desc->mfr_desc, DESC_STR_LEN);
		hw_desc.assign(desc->hw_desc, DESC_STR_LEN);
		sw_desc.assign(desc->sw_desc, DESC_STR_LEN);
		serial_num.assign(desc->serial_num, DESC_STR_LEN);
		dp_desc.assign(desc->dp_desc, DESC_STR_LEN);
	} break;
	case OFP12_VERSION: {
		if (buflen < sizeof(struct ofp12_desc_stats))
			throw eInval();

		struct ofp12_desc_stats *desc = (struct ofp12_desc_stats*)buf;

		mfr_desc.assign(desc->mfr_desc, DESC_STR_LEN);
		hw_desc.assign(desc->hw_desc, DESC_STR_LEN);
		sw_desc.assign(desc->sw_desc, DESC_STR_LEN);
		serial_num.assign(desc->serial_num, DESC_STR_LEN);
		dp_desc.assign(desc->dp_desc, DESC_STR_LEN);
	} break;
	default:
		throw eBadVersion();
	}
}


