/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CROFLEXP_H
#define CROFLEXP_H 1

#include <string>
#include <sys/types.h>
#include <inttypes.h>
#include <endian.h>
#ifndef be32toh
	#include "endian_conversion.h"
#endif

#include "rofl/common/cmemory.h"
#include "rofl/common/croflexception.h"
#include "rofl/common/cvastring.h"
#include "rofl/common/openflow/cofmatch.h"

namespace rofl {
namespace openflow {

class eRoflExpBase 		: public RoflException {};
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
			struct openflow::ofp_match		match[0];		// start of match structure
		};

		enum ofp_rofl_ext_fsp_cmds {
			OFPRET_FSP_ADD		= 1,
			OFPRET_FSP_DELETE	= 2,
		};

// this must be set to largest ofp_vendor_ext_rofl_... structure defined
#define ROFL_EXP_MAX_SIZE (sizeof(struct ofp_vendor_ext_rofl_port_ipv4))


public: // data structures

		union {
			uint8_t									*rehu_generic;
			struct ofp_rofl_ext_header				*rehu_header;
			struct ofp_rofl_ext_flowspace			*rehu_flowspace;
		} reh_rehu;

#define rext_generic					reh_rehu.rehu_generic
#define rext_header						reh_rehu.rehu_header
#define rext_fsp						reh_rehu.rehu_flowspace


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
	pack(uint8_t *body, size_t bodylen);

	/**
	 *
	 */
	virtual void
	unpack(uint8_t *body, size_t bodylen);


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

public:

	friend std::ostream&
	operator<< (std::ostream& os, croflexp const& roflexp) {
		os << "<croflexp ";
			os << std::endl << "match:" << roflexp.match << std::endl;
			os << std::endl << "mem:" << roflexp.mem << std::endl;
		os << ">";
		return os;
	};
};





class croflexp_flowspace :
	public croflexp
{
public:
		/**
		 *
		 */
		croflexp_flowspace(
				uint8_t command,
				cofmatch& m) :
					croflexp(sizeof(struct ofp_rofl_ext_flowspace) + m.length()) {
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
		{};


		/**
		 *
		 */
		virtual void
		pack(uint8_t *body, size_t bodylen) {
			if (bodylen < length()) {
				throw eRoflExpInval();
			}
			memcpy(body, rext_generic, sizeof(struct ofp_rofl_ext_flowspace));
			match.pack((body + sizeof(struct ofp_rofl_ext_flowspace)),
					bodylen - sizeof(struct ofp_rofl_ext_flowspace));
		};


		/**
		 *
		 */
		virtual void
		unpack(uint8_t *body, size_t bodylen) {
			if (bodylen < sizeof(struct ofp_rofl_ext_flowspace)) {
				throw eRoflExpInval();
			}
			memcpy(rext_generic, body, sizeof(struct ofp_rofl_ext_flowspace));
			match.unpack(body + sizeof(struct ofp_rofl_ext_flowspace),
					bodylen - sizeof(struct ofp_rofl_ext_flowspace));
		};


		/**
		 *
		 */
		uint8_t
		get_command() const {
			return rext_fsp->command;
		};


		/**
		 *
		 */
		void
		set_command(uint8_t command) {
			rext_fsp->command = command;
		};


		/**
		 *
		 */
		virtual size_t
		length() {
			return (sizeof(struct ofp_rofl_ext_flowspace) + match.length());
		};

public:

		/**
		 *
		 */
		friend std::ostream&
		operator<< (std::ostream& os, croflexp_flowspace const& e) {
			os << "<croflexp_flowspace ";
				switch (e.get_command()) {
				case OFPRET_FSP_ADD: 		os << "FlowSpaceAdd" 	<< " "; break;
				case OFPRET_FSP_DELETE:		os << "FlowSpaceDelete" << " "; break;
				default:					os << "UnknownCommand" 	<< " "; break;
				}
				os << "flowspace: " << e.match << " ";
			os << ">";
			return os;
		};
};

}; // end of namespace openflow
}; // end of namespace rofl

#endif

