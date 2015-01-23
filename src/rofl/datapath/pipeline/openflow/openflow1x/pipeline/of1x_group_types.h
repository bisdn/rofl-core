/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OF1X_GROUP_TYPES_H__
#define __OF1X_GROUP_TYPES_H__

// This file is created to avoid cross dependency of header files

/**
* @ingroup core_of1x
* Group type
*/
typedef enum of1x_group_type{
	OF1X_GROUP_TYPE_ALL 		= 0,	/* All (multicast/broadcast) group.  */
	OF1X_GROUP_TYPE_SELECT 		= 1,   	/* Select group. */
	OF1X_GROUP_TYPE_INDIRECT 	= 2, 	/* Indirect group. */
	OF1X_GROUP_TYPE_FF	 	= 3,	/* Fast failover group. */
}of1x_group_type_t;

#endif //__OF1X_GROUP_TYPES_H__
