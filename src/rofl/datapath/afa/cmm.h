/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CMM__H
#define CMM__H 

#include "afa.h"
#include "afa_utils.h"

/**
* Non-OF related events... none for the moment
* e.g. port down/up
*/

//C++ extern C
AFA_BEGIN_DECLS

/**
* @name    cmm_notify_port_add
* @brief   Notify port add to CMM 
* @param port_snapshot Snapshot of the current state of a switch port. The snapshot MUST be deleted using switch_port_destroy_snapshot()
* @ingroup cmm_management
*/
afa_result_t cmm_notify_port_add(switch_port_snapshot_t* port_snapshot);

/**
* @name    cmm_notify_port_delete
* @brief   Notifies CMM that port has been deleted from the platform, or cannot be associated to a switch
* @param port_snapshot Snapshot of the current state of a switch port. The snapshot MUST be deleted using switch_port_destroy_snapshot()
* @ingroup cmm_management
*/
afa_result_t cmm_notify_port_delete(switch_port_snapshot_t* port_snapshot);

/**
* @name    cmm_notify_port_status_changed
* @brief   Notify port add to CMM 
* @param port_snapshot Snapshot of the current state of a switch port. The snapshot MUST be deleted using switch_port_destroy_snapshot()
* @ingroup cmm_management
*/
afa_result_t cmm_notify_port_status_changed(switch_port_snapshot_t* port_snapshot);

/**
* @name    cmm_notify_monitoring_state_changed
* @brief   Notify a monitoring state change to CMM 
* @param monitoring_snapshot Snapshot of the current monitoring state. The snapshot MUST be deleted using monitoring_destroy_snapshot()
* @ingroup cmm_management
*/
afa_result_t cmm_notify_monitoring_state_changed(monitoring_snapshot_state_t* monitoring_snapshot);

//C++ extern C
AFA_END_DECLS

/* OpenFlow related events */
#include "openflow/openflow1x/of1x_cmm.h"
//TODO: Add more versions here...

#endif /** CMM__H */
