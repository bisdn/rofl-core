#ifndef PPPOE_MATCHES
#define PPPOE_MATCHES 1

#include <rofl/common/openflow/coxmatch.h>

namespace rofl
{

/** OXM_OF_PPPOE_CODE
 *
 */
class coxmatch_ofx_pppoe_code :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_pppoe_code(
			uint8_t code) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_PPPOE_CODE);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = code;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_pppoe_code() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_pppoe_code const& oxm)
	{
		os << "OXM";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<PPPOE-CODE: " << (unsigned int)oxm.u8value() << ">";
		return os;
	};
};


/** OXM_OF_PPPOE_TYPE
 *
 */
class coxmatch_ofx_pppoe_type :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_pppoe_type(
			uint8_t type) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint8_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_PPPOE_TYPE);
		set_oxm_length(sizeof(uint8_t));
		oxm_uint8t->byte = type;
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_pppoe_type() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_pppoe_type const& oxm)
	{
		os << "OXM";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<PPPOE-CODE: " << (unsigned int)oxm.u8value() << ">";
		return os;
	};
};


/** OXM_OF_PPPOE_SID
 *
 */
class coxmatch_ofx_pppoe_sid :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_pppoe_sid(
			uint16_t sessid) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_PPPOE_SID);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(sessid);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_pppoe_sid() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_pppoe_sid const& oxm)
	{
		os << "OXM";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<PPPOE-SID: " << (unsigned int)oxm.u16value() << ">";
		return os;
	};
};


/** OXM_OF_PPP_PROT
 *
 */
class coxmatch_ofx_ppp_prot :
	public coxmatch
{
public:
	/** constructor
	 */
	coxmatch_ofx_ppp_prot(
			uint16_t prot) :
				coxmatch(sizeof(struct ofp_oxm_hdr) + sizeof(uint16_t))
	{
		set_oxm_class(OFPXMC_EXPERIMENTER);
		set_oxm_field(OFPXMT_OFX_PPP_PROT);
		set_oxm_length(sizeof(uint16_t));
		oxm_uint16t->word = htobe16(prot);
	};
	/** destructor
	 */
	virtual
	~coxmatch_ofx_ppp_prot() {};
	/**
	 */
	friend std::ostream&
	operator<< (std::ostream& os, coxmatch_ofx_ppp_prot const& oxm)
	{
		os << "OXM";
			os << "[" << oxm.get_oxm_class() << ":" << oxm.get_oxm_field() << "]";
			os << "<PPP-PROT: " << (unsigned int)oxm.u16value() << ">";
		return os;
	};
};

}; // end of namespace

#endif
