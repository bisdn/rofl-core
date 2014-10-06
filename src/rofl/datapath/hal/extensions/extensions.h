/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HAL_EXTENSIONS_H
#define HAL_EXTENSIONS_H

/**
* @file extensions.h
* @author Marc Sune<marc.sune (at) bisdn.de>
*
* @brief Extensions ops HAL definition
* 
*/

#include "../hal.h"
#include "../hal_utils.h"

//Extension specific includes
#include "nf.h"

//C++ extern C
HAL_BEGIN_DECLS

typedef struct hal_extension_ops{

	//NF ports APIs
	hal_nf_ext_ops_t nf_ports;

}hal_extension_ops_t;

//C++ extern C
HAL_END_DECLS

#endif /* HAL_EXTENSIONS_H */

