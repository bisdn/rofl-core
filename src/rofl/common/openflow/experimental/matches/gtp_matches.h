#ifndef GTP_MATCHES_H
#define GTP_MATCHES_H 1

#include <rofl/common/openflow/coxmatch.h>

namespace rofl {
namespace openflow {

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
			uint32_t teid);
	/** constructor
	 */
	coxmatch_ofx_gtp_teid(
			uint32_t teid,
			uint32_t mask);
	/**
	 */
	coxmatch_ofx_gtp_teid(
			coxmatch const& oxm);
	/** destructor
	 */
	virtual
	~coxmatch_ofx_gtp_teid();
	/**
	 */
	uint32_t
	get_teid() const;
	/**
	 */
	uint32_t
	get_teid_value() const;
	/**
	 */
	uint32_t
	get_teid_mask() const;
	/**
	 */
	void
	set_teid(uint32_t teid, uint32_t mask = 0xffffffff);
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
			uint8_t msg_type);
	/**
	 */
	coxmatch_ofx_gtp_msg_type(
			coxmatch const& oxm);
	/** destructor
	 */
	virtual
	~coxmatch_ofx_gtp_msg_type();
	/**
	 */
	uint8_t
	get_msg_type() const;
	/**
	 */
	void
	set_msg_type(uint8_t msg_type);
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

}; // end of namespace openflow
}; // end of namespace rofl

#endif
