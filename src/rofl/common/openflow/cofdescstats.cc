#include "rofl/common/openflow/cofdescstats.h"

using namespace rofl;



cofdescstats::cofdescstats(
		uint8_t of_version) :
				of_version(of_version)
{
	switch (of_version) {
	case OFP10_VERSION: {
		cmemory::resize(sizeof(struct ofp10_desc_stats));
		ofh10_desc_stats = (struct ofp10_desc_stats*)somem();
	} break;
	case OFP12_VERSION: {
		cmemory::resize(sizeof(struct ofp12_desc_stats));
		ofh12_desc_stats = (struct ofp12_desc_stats*)somem();
	} break;
	default:
		throw eBadVersion();
	}
}



cofdescstats::cofdescstats(
		uint8_t of_version,
		std::string const& mfr_desc,
		std::string const& hw_desc,
		std::string const& sw_desc,
		std::string const& serial_num,
		std::string const& dp_desc) :
				of_version(of_version)
{
	switch (of_version) {
	case OFP10_VERSION: {
		cmemory::resize(sizeof(struct ofp10_desc_stats));
		ofh10_desc_stats = (struct ofp10_desc_stats*)somem();

		snprintf(ofh10_desc_stats->mfr_desc, DESC_STR_LEN, mfr_desc.c_str(), mfr_desc.length());
		snprintf(ofh10_desc_stats->hw_desc, DESC_STR_LEN, hw_desc.c_str(), hw_desc.length());
		snprintf(ofh10_desc_stats->sw_desc, DESC_STR_LEN, sw_desc.c_str(), sw_desc.length());
		snprintf(ofh10_desc_stats->serial_num, DESC_STR_LEN, serial_num.c_str(), serial_num.length());
		snprintf(ofh10_desc_stats->dp_desc, DESC_STR_LEN, dp_desc.c_str(), dp_desc.length());
	} break;
	case OFP12_VERSION: {
		cmemory::resize(sizeof(struct ofp12_desc_stats));
		ofh12_desc_stats = (struct ofp12_desc_stats*)somem();

		snprintf(ofh12_desc_stats->mfr_desc, DESC_STR_LEN, mfr_desc.c_str(), mfr_desc.length());
		snprintf(ofh12_desc_stats->hw_desc, DESC_STR_LEN, hw_desc.c_str(), hw_desc.length());
		snprintf(ofh12_desc_stats->sw_desc, DESC_STR_LEN, sw_desc.c_str(), sw_desc.length());
		snprintf(ofh12_desc_stats->serial_num, DESC_STR_LEN, serial_num.c_str(), serial_num.length());
		snprintf(ofh12_desc_stats->dp_desc, DESC_STR_LEN, dp_desc.c_str(), dp_desc.length());
	} break;
	default:
		throw eBadVersion();
	}
}



cofdescstats::~cofdescstats()
{

}



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

	cmemory::operator= (descstats);

	of_version = descstats.of_version;

	switch (of_version) {
	case OFP10_VERSION: {
		ofh10_desc_stats = (struct ofp10_desc_stats*)somem();
	} break;
	case OFP12_VERSION: {
		ofh12_desc_stats = (struct ofp12_desc_stats*)somem();
	} break;
	default:
		throw eBadVersion();
	}

	return *this;
}



std::string
cofdescstats::get_mfr_desc() const
{
	std::string mfr_desc;

	switch (of_version) {
	case OFP10_VERSION: {
		mfr_desc.assign(ofh10_desc_stats->mfr_desc, DESC_STR_LEN);
	} break;
	case OFP12_VERSION: {
		mfr_desc.assign(ofh12_desc_stats->mfr_desc, DESC_STR_LEN);
	} break;
	default:
		throw eBadVersion();
	}

	return mfr_desc;
}



std::string
cofdescstats::get_hw_desc() const
{
	std::string hw_desc;

	switch (of_version) {
	case OFP10_VERSION: {
		hw_desc.assign(ofh10_desc_stats->hw_desc, DESC_STR_LEN);
	} break;
	case OFP12_VERSION: {
		hw_desc.assign(ofh12_desc_stats->hw_desc, DESC_STR_LEN);
	} break;
	default:
		throw eBadVersion();
	}

	return hw_desc;
}



std::string
cofdescstats::get_sw_desc() const
{
	std::string sw_desc;

	switch (of_version) {
	case OFP10_VERSION: {
		sw_desc.assign(ofh10_desc_stats->sw_desc, DESC_STR_LEN);
	} break;
	case OFP12_VERSION: {
		sw_desc.assign(ofh12_desc_stats->sw_desc, DESC_STR_LEN);
	} break;
	default:
		throw eBadVersion();
	}

	return sw_desc;
}



std::string
cofdescstats::get_serial_num() const
{
	std::string serial_num;

	switch (of_version) {
	case OFP10_VERSION: {
		serial_num.assign(ofh10_desc_stats->serial_num, DESC_STR_LEN);
	} break;
	case OFP12_VERSION: {
		serial_num.assign(ofh12_desc_stats->serial_num, DESC_STR_LEN);
	} break;
	default:
		throw eBadVersion();
	}

	return serial_num;
}



std::string
cofdescstats::get_dp_desc() const
{
	std::string dp_desc;

	switch (of_version) {
	case OFP10_VERSION: {
		dp_desc.assign(ofh10_desc_stats->dp_desc, DESC_STR_LEN);
	} break;
	case OFP12_VERSION: {
		dp_desc.assign(ofh12_desc_stats->dp_desc, DESC_STR_LEN);
	} break;
	default:
		throw eBadVersion();
	}

	return dp_desc;
}


