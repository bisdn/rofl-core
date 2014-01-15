#include "cofmsg_error.h"

using namespace rofl;

cofmsg_error::cofmsg_error(
		uint8_t of_version,
		uint32_t xid,
		uint16_t err_type,
		uint16_t err_code,
		uint8_t* data,
		size_t datalen) :
	cofmsg(sizeof(struct ofp_error_msg)),
	body(0)
{
	

	cofmsg::resize(sizeof(struct ofp_error_msg) + datalen);
	body.assign(data, datalen);
    err_msg = (struct ofp_error_msg*)soframe();

	set_version(of_version);
	set_length(sizeof(struct ofp_error_msg) + datalen);
	set_xid(xid);

	switch (of_version) {
	case OFP10_VERSION: {
		set_type(OFPT10_ERROR);
	} break;
	case OFP12_VERSION: {
		set_type(OFPT12_ERROR);
	} break;
	case OFP13_VERSION: {
		set_type(OFPT13_ERROR);
	} break;
	}

	set_err_type(err_type);
	set_err_code(err_code);
}



cofmsg_error::cofmsg_error(
		cmemory *memarea) :
	cofmsg(memarea)
{
	err_msg = (struct ofp_error_msg*)soframe();
}



cofmsg_error::cofmsg_error(
		cofmsg_error const& error)
{
	*this = error;
}



cofmsg_error&
cofmsg_error::operator= (
		cofmsg_error const& error)
{
	if (this == &error)
		return *this;

	cofmsg::operator =(error);

	body = error.body;

	err_msg = (struct ofp_error_msg*)soframe();

	return *this;
}



cofmsg_error::~cofmsg_error()
{

}



void
cofmsg_error::reset()
{
	cofmsg::reset();
	body.clear();
}



size_t
cofmsg_error::length()
{
	return (sizeof(struct ofp_error_msg) + body.memlen());
}



void
cofmsg_error::pack(uint8_t *buf, size_t buflen)
{
	set_length(length());

	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eInval();

	cofmsg::pack(buf, buflen);

	memcpy(buf + sizeof(struct ofp_error_msg), body.somem(), body.memlen());
}



void
cofmsg_error::unpack(uint8_t *buf, size_t buflen)
{
	cofmsg::unpack(buf, buflen);

	validate();
}



void
cofmsg_error::validate()
{
	cofmsg::validate(); // check generic OpenFlow header

	err_msg = (struct ofp_error_msg*)soframe();

	switch (get_version()) {
	case OFP10_VERSION:
	case OFP12_VERSION:
	case OFP13_VERSION: {
		if (get_length() < sizeof(struct ofp_error_msg))
			throw eBadSyntaxTooShort();
		if (get_length() > sizeof(struct ofp_error_msg)) {
			body.assign(soframe() + sizeof(struct ofp_error_msg), framelen() - sizeof(struct ofp_error_msg));
		}
	} break;
	default:
		throw eBadRequestBadVersion();
	}
}



uint16_t
cofmsg_error::get_err_type() const
{
	return be16toh(err_msg->type);
}



void
cofmsg_error::set_err_type(uint16_t type)
{
	//FIXME XXX: internally 1.2 types and codes are used
	//this should be properly abstracted to make always an appropiate translation (version agnostic codes)
	//or always use the same identifiers otherwise it can confuse the user
	switch (get_version()) {
		case OFP10_VERSION:
			//Translating it... this is crap
			switch(type){
				case OFPET_HELLO_FAILED:
					err_msg->type=htobe16(OFP10ET_HELLO_FAILED);
					break;
				case OFPET_BAD_REQUEST:
					err_msg->type=htobe16(OFP10ET_BAD_REQUEST);
					break;
				case OFPET_BAD_ACTION:
					err_msg->type=htobe16(OFP10ET_BAD_ACTION); 
					break;
				case OFPET_FLOW_MOD_FAILED:
					err_msg->type=htobe16(OFP10ET_FLOW_MOD_FAILED); 
					break;
				case OFPET_PORT_MOD_FAILED:
					err_msg->type=htobe16(OFP10ET_PORT_MOD_FAILED); 
					break;
				case OFPET_QUEUE_OP_FAILED:
					err_msg->type=htobe16(OFP10ET_QUEUE_OP_FAILED); 
					break;
				default: 
					err_msg->type=htobe16(OFP10ET_BAD_REQUEST); 
					break;
			}
			break;
		case OFP12_VERSION:
		case OFP13_VERSION: 
			err_msg->type = htobe16(type);
			break;
		
		default: //TODO: what to do...
			break;		
	}
}



uint16_t
cofmsg_error::get_err_code() const
{
	return be16toh(err_msg->code);
}



void
cofmsg_error::set_err_code(uint16_t code)
{
	//FIXME XXX: internally 1.2 types and codes are used
	//this should be properly abstracted to make always an appropiate translation (version agnostic codes)
	//or always use the same identifiers otherwise it can confuse the user
	
	switch (get_version()) {
			case OFP10_VERSION:
				//Translating it... this is crap
				//only the ones which differ
				if(err_msg->type == htobe16(OFPET_FLOW_MOD_FAILED)){
					switch(code){
						case OFPFMFC_UNKNOWN:
							err_msg->code = htobe16(OFP10FMFC_BAD_COMMAND);
							break;
						case OFPFMFC_TABLE_FULL:
							err_msg->code = htobe16(OFP10FMFC_ALL_TABLES_FULL);
							break;
						case OFPFMFC_BAD_TABLE_ID:
							err_msg->code = htobe16(OFP10FMFC_BAD_COMMAND);
							break;
						case OFPFMFC_OVERLAP:
							err_msg->code = htobe16(OFP10FMFC_OVERLAP);
							break;
						case OFPFMFC_EPERM:
							err_msg->code = htobe16(OFP10FMFC_EPERM);
							break;
						case OFPFMFC_BAD_TIMEOUT:
							err_msg->code = htobe16(OFP10FMFC_UNSUPPORTED);
							break;
						case OFPFMFC_BAD_COMMAND:
							err_msg->code = htobe16(OFP10FMFC_BAD_COMMAND);
							break;
						default: 
							err_msg->code = htobe16(code);
							break;
					}

					break; //Only break for OF1.0 AND OFPET_FLOW_MOD_FAILED type
				}
			case OFP12_VERSION:
			case OFP13_VERSION: 
				err_msg->code = htobe16(code);
				break;
			
			default: //TODO: what to do...
				break;		
	}
}



cmemory&
cofmsg_error::get_body()
{
	return body;
}


