/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * openflow.h
 *
 *  Created on: 02.03.2013
 *      Author: andi
 */

#ifndef OPENFLOW_H__
#define OPENFLOW_H__ 1

#include <inttypes.h>

#include "rofl/common/croflexception.h"

#include "rofl/common/openflow/openflow10.h"
#include "rofl/common/openflow/openflow12.h"
#include "rofl/common/openflow/openflow13.h"
#include "rofl/common/openflow/openflow_experimental.h"

namespace rofl {
namespace openflow {

class base {
public:
	static uint32_t
	get_ofp_no_buffer(uint8_t ofp_version);

	static uint32_t
	get_ofpp_flood_port(uint8_t ofp_version);

	static uint32_t
	get_ofpp_controller_port(uint8_t ofp_version);

	static uint8_t
	get_ofptt_all(uint8_t ofp_version);
};

};
};

#endif /* OPENFLOW_H_ */
