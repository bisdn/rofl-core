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
			uint8_t code);
	/** destructor
	 */
	virtual
	~coxmatch_ofx_pppoe_code();
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
			uint8_t type);
	/** destructor
	 */
	virtual
	~coxmatch_ofx_pppoe_type();
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
			uint16_t sessid);
	/** destructor
	 */
	virtual
	~coxmatch_ofx_pppoe_sid();
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
			uint16_t prot);
	/** destructor
	 */
	virtual
	~coxmatch_ofx_ppp_prot();
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
