#ifndef GTP_MATCHES_H
#define GTP_MATCHES_H 1

#include <rofl/common/openflow/coxmatch.h>

namespace rofl
{

/** OXM_OFX_GTP_TEID
 *
 */
class coxmatch_ofx_gtp_teid :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_gtp_teid(
			uint32_t teid) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_GTP_TEID);
		set_oxm_length(sizeof(uint32_t));
		set_teid(teid);
	};
	/** constructor
	 */
	coxmatch_ofx_gtp_teid(
			uint32_t teid,
			uint32_t mask) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + 2 * sizeof(uint32_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_GTP_TEID);
		set_oxm_hasmask(true);
		set_oxm_length(2 * sizeof(uint32_t));
		set_teid(teid, mask);
	};
	/**
	 */
	coxmatch_ofx_gtp_teid(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_gtp_teid() {};
	/**
	 */
	uint32_t
	get_teid() const {
		return u32value();
	};
	/**
	 */
	uint32_t
	get_teid_value() const {
		return uint32_value();
	};
	/**
	 */
	uint32_t
	get_teid_mask() const {
		return uint32_mask();
	};
	/**
	 */
	void
	set_teid(uint32_t teid, uint32_t mask = 0xffffffff) {
		oxm_uint32t->dword = htobe32(teid);
		if (mask != 0xffffffff) {
			if (not get_oxm_hasmask())
				throw eOxmInval();
			oxm_uint32t->mask = htobe32(mask);
		}
	};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_gtp_teid const& oxm)
	{
		os << "OXM";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<GTP-TEID: " << (unsigned int)oxm.uint32_value();
			if (oxm.get_oxm_hasmask()) {
				os << ":" << (unsigned int)oxm.uint32_mask();
			}
			os << ">";
		return os;
	};
};


/** OXM_OFX_GTP_MSG_TYPE
 *
 */
class coxmatch_ofx_gtp_msg_type :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_gtp_msg_type(
			uint8_t msg_type) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_GTP_MSG_TYPE);
		set_oxm_length(sizeof(uint8_t));
		set_msg_type(msg_type);
	};
	/**
	 */
	coxmatch_ofx_gtp_msg_type(
			coxmatch const& oxm) :
				coxmatch(oxm) {};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_gtp_msg_type() {};
	/**
	 */
	uint8_t
	get_msg_type() const {
		return u8value();
	};
	/**
	 */
	void
	set_msg_type(uint8_t msg_type) {
		oxm_uint8t->byte = msg_type;
	};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_gtp_msg_type const& oxm)
	{
		os << "OXM";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<GTP-MSG-TYPE: " << (unsigned int)oxm.u8value() << ">";
		return os;
	};
};

}; // end of namespace

#endif
