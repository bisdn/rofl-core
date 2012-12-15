/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CROFLEXP_H
#define CROFLEXP_H 1

#include <string>

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <inttypes.h>

#include <endian.h>
#ifndef be32toh
#include "endian_conversion.h"
#endif

#ifdef __cplusplus
}
#endif

#include "cmemory.h"
#include "cerror.h"
#include "cvastring.h"
#include "openflow/cofmatch.h"

namespace rofl
{

class eRoflExpBase 		: public cerror {};
class eRoflExpInval 	: public eRoflExpBase {};

#define OFPEXPID_ROFL		0x55b12399

class croflexp
{
/*
 * data structures
 */
protected:

		cmemory mem;

public:

		cofmatch match;

		enum ofp_rofl_ext_version {
			OFP_ROFL_EXT_VERSION1	= 1,
		};

		struct ofp_rofl_ext_header {
			uint8_t 	version;
			uint8_t		type;
			uint16_t 	length;
		};

		enum roflexp_exttype_t {
			OFPRET_NONE 		= 0,
			OFPRET_FLOWSPACE 	= 2,
		};



		struct ofp_rofl_ext_flowspace {
			struct ofp_rofl_ext_header		header;
			uint8_t 						command;
			uint8_t							pad[3];
			struct ofp_match				match[0];		// start of match structure
		};

		enum ofp_rofl_ext_fsp_cmds {
			OFPRET_FSP_ADD		= 1,
			OFPRET_FSP_DELETE	= 2,
		};

// this must be set to largest ofp_vendor_ext_rofl_... structure defined
#define ROFL_EXP_MAX_SIZE (sizeof(struct ofp_vendor_ext_rofl_port_ipv4))


public: // data structures

		union {
			struct ofp_rofl_ext_header				*rehu_header;
			struct ofp_rofl_ext_flowspace			*rehu_flowspace;
		} reh_rehu;

#define rext_header						reh_rehu.rehu_header
#define rext_fsp						reh_rehu.rehu_flowspace

private: // data structures

	std::string info;


public:


	/**
	 *
	 */
	croflexp(
			size_t size = 0);


	/**
	 *
	 */
	croflexp(
			uint8_t *buf,
			size_t buflen);


	/**
	 *
	 */
	virtual
	~croflexp();


	/**
	 *
	 */
	virtual void
	pack(
			uint8_t *body,
			size_t bodylen)
				throw (eRoflExpInval);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *body,
			size_t bodylen);


	/**
	 *
	 */
	virtual const char*
	c_str();


	/**
	 *
	 */
	virtual size_t
	length();


	/**
	 *
	 */
	void
	validate()
			throw (eRoflExpInval);


private:


	/**
	 *
	 */
	void
	validate_flowspace()
			throw (eRoflExpInval);

};





class croflexp_flowspace :
	public croflexp
{
private:

		std::string info;

public:
		/**
		 *
		 */
		croflexp_flowspace(
				uint8_t command,
				cofmatch& m) :
					croflexp(sizeof(struct ofp_rofl_ext_flowspace) + m.length())
		{
			match = m;

			rext_fsp->header.version 	= OFP_ROFL_EXT_VERSION1;
			rext_fsp->header.type 		= OFPRET_FLOWSPACE;
			rext_fsp->header.length 	= htobe16(length());
			rext_fsp->command 			= command;
		};
		/**
		 *
		 */
		virtual
		~croflexp_flowspace()
		{

		};
		/**
		 *
		 */
		virtual void
		pack(
				uint8_t *body,
				size_t bodylen)
					throw (eRoflExpInval)
		{
			if (bodylen < length())
			{
				throw eRoflExpInval();
			}


			memcpy(body, (uint8_t*)rext_fsp, sizeof(struct ofp_rofl_ext_flowspace));
			match.pack((struct ofp_match*)(body + sizeof(struct ofp_rofl_ext_flowspace)),
					bodylen - sizeof(struct ofp_rofl_ext_flowspace));
		};
		/**
		 *
		 */
		virtual const char*
		c_str()
		{
			cvastring vas(2048);

			info.assign(vas("croflexp_flowspace(%p) command: %d match: %s",
					this, rext_fsp->command, match.c_str()));

			return info.c_str();
		};
		/**
		 *
		 */
		virtual size_t
		length()
		{
			return (sizeof(struct ofp_rofl_ext_flowspace) + match.length());
		};
};

}; // end of namespace

#endif

