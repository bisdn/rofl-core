/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __PHYSICAL_SWITCH_H__
#define __PHYSICAL_SWITCH_H__

#include <stdlib.h> 
#include <string.h> 
#include "rofl.h"
#include "platform/lock.h"
#include "util/rofl_pipeline_utils.h"
#include "openflow/of_switch.h"
#include "switch_port.h"

/**
* @file physical_switch.h
* @author Marc Sune(marc.sune@bisdn.de)  
* @brief Physical switch API    
*
* The physical switch is the abstraction that contains
* the state of a forwarding device (e.g. a physical switch, 
* or a server).
*
* The physical switch stores fundamentally information about:
*
*    - System ports(net interfaces): the ports availableon the system. This 
*      interfaces may be physical ports, or virtual,
*      depending on the system. The semantic of a 
*      virtual port is strictly bound to the system
*      in which is running (take a look at switch_port.h
*      for more details).
*
*      The discovery of the system ports is up to the library user. It
*      must be done during initialization and before ANY logical switch 
*      has been added to the physical switch.
*
*    - Logical switches instances: a logical switch instance is a runnable
*      instance of a switch that uses one or more of the ports available
*      in the phyisical system. In other words, a logical switch is what
*      OF protocol defines as "datapath", but only concerning forwarding
*      (excluding the OF endpoint).
*
*       Logical switches are of type of_switch, and may be created using
*       the version specific init/destroy functions provided by the library.
*       (e.g. of12_init_switch). Immediately after, they must be added to
*       the physical switch. Note that instances not added to the physical
*       switch are completely meaningless(*).
*
*       Once added the ports can be attached to the logical switch, and 
*       eventually, in the case of the software switches, start to process
*       packets using the call of_process_packet_pipeline.
*
*
* A typical usage of the library in the case of a software switch (that is, it uses
* rofl-pipeline to process packets) would be:
*
* @code
*
* physical_switch_init(); //This must be always the first
* 
* //Here the user MUST fill in the available ports
* //E.g. interfaces
* physical_switch_t* psw = get_physical_switch();
*
* for i in num of interfaces:
*    switch_port_t* port = switch_port_init();
*    //Fill in port struct...(e.g. port name, speed..)
*    port->name = "eth0";
*     psw->physical_ports[i] = port;
*
*
* //Now we create one or more Logical switch instances
* dpid = 0x100;
* of_switch = of12_init_switch(dpid,...);
*
* //Note from this point on if any port is attached
* //The switch will start to process packets so 
* //processing threads may be launched depending on the
* //system.
*
* //Also note I/O system for receiving and sending packets
* //must be ready at least immediately after the attachment 
*
* //Now we attach a port
* port = physical_switch_get_port("eth0");
* sw = physical_switch_get_logical_switch_by_dpid(dpid);
* physical_switch_attach_port_to_logical_switch(port, sw, &port_num);
*
* //Switch should receive packets and process them
*
* @endcode
*
*/


#ifndef PHYSICAL_SWITCH_MAX_LS
    #define PHYSICAL_SWITCH_MAX_LS 64
#endif

#ifndef PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS
    #define PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS 128 
#endif

#ifndef PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS
    #define PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS 128
#endif

#ifndef PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS
    #define PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS 32
#endif
    
#define PHYSICAL_SWITCH_MAX_NUM_META_PORTS 8


//Opaque platform state (to be used, maybe, for platform hooks).
//Currently unused
typedef void platform_physical_switch_state_t;

typedef struct physical_switch{

    /*
    * List of all logical switches in the system
    */
    unsigned int num_of_logical_switches;
    of_switch_t* logical_switches[PHYSICAL_SWITCH_MAX_LS];

    /*
    * Ports
    */
    //physical: index is the physical port of the platform.
    switch_port_t* physical_ports[PHYSICAL_SWITCH_MAX_NUM_PHY_PORTS];

    //tunnel ports
    switch_port_t* tunnel_ports[PHYSICAL_SWITCH_MAX_NUM_VIR_PORTS]; //Not used yet

    //virtual ports (which are not tunnel)
    switch_port_t* virtual_ports[PHYSICAL_SWITCH_MAX_NUM_TUN_PORTS]; //Not used yet

    //meta ports (esoteric ports). This is NOT an array of pointers!
    switch_port_t meta_ports[PHYSICAL_SWITCH_MAX_NUM_META_PORTS]; 

    /* 
    * Other state 
    */
    //Mutex
    platform_mutex_t* mutex;
    
    //Opaque platform specific extra state 
    platform_physical_switch_state_t* platform_state;	
}physical_switch_t; 

//C++ extern C
ROFL_PIPELINE_BEGIN_DECLS

//
// Physical switch and logical switch management
//

/**
* @brief    Initializes the physical switch. This call must be done before anyone else. 
*/
void physical_switch_init(void);

/**
* @brief    Returns a pointer to the physical switch. This seldomly needs to be used. 
*/
physical_switch_t* get_physical_switch(void);

/**
* @brief    Destroys the physical switch state. 
*
* This call destroys the physical switch state. This call will NOT destroy any logical switch 
* or any port switch previously placed in the physical switch port list.
*/
void physical_switch_destroy(void);

/**
* @brief    Add a logical switch to the pool. 
*
* This call adds a previously initialized logical switch to the current logical
* switch pool 
*
* @param sw    Pointer to an of_switch_t structure. The switch must have been previously initialized using ofXX_init_switch() call.
*/
rofl_result_t physical_switch_add_logical_switch(of_switch_t* sw);

/**
* @brief Attemps to remove a previously added logical switch from the pool
* by dpid. 
*/
rofl_result_t physical_switch_remove_logical_switch_by_dpid(const uint64_t dpid);

/**
* @brief    Attemps to remove a previously added logical switch from the pool 
*/
rofl_result_t physical_switch_remove_logical_switch(of_switch_t* sw);

//Getters
/**
* @brief    Attemps to retrieve a logical switch from the pool by its dpid 
*/
of_switch_t* physical_switch_get_logical_switch_by_dpid(const uint64_t dpid);

/**
* @brief    Attemps to retrieve the logical switch attached to the port
*/
of_switch_t* physical_switch_get_logical_switch_attached_to_port(const switch_port_t port);


//
// Port management routines
//

/**
* @brief Retrieve a physical switch port by name 
* 
* Attempts to retrieve a port previously added to the phyisical switch by its name.
* The current call ONLY looks up in the physical ports
*/
switch_port_t* physical_switch_get_port_by_name(const char *name);

/**
* @brief Retrieve a port attached to logical switch with dpid at port num
* 
* Attempts to retrieve a port previously added to the phyisical switch and attached
* to the logical switch identified by the dpid and attached to port number
*
* @param dpid     	Datapath ID of the switch 
* @param port_num    Port number 	
*
*/
switch_port_t* physical_switch_get_port_by_num(const uint64_t dpid, unsigned int port_num);


//Physical switch list
/**
* @brief Retrieve the physical port list
* @param num_of_ports    Pointer to an int. Number of ports will be filled by the lib.
*/
void physical_switch_get_physical_ports(switch_port_t* ports, unsigned int* num_of_ports);
/*
TODO: add create virtual link and add/remove tunnel.
//void physical_switch_get_virtual_ports(switch_port_t* ports, unsigned int* num_of_ports);
//void physical_switch_get_tunnel_ports(switch_port_t* ports, unsigned int* num_of_ports);
*/

//Logical switch port management
/**
* @brief Retrieve the physical port list
* @param num_of_ports    Pointer to an int. Number of ports will be filled by the lib.
*/
rofl_result_t get_logical_switch_ports(of_switch_t* sw, logical_switch_port_t** ports, unsigned int* num_of_ports, unsigned int* logical_sw_max_ports);

#if 0
rofl_result_t physical_switch_attach_port_num_to_logical_switch(unsigned int port_num, of_switch_t* sw, unsigned int* logical_switch_port_num);
#endif

/**
* @brief Attaches port to logical switch 
* @param port_num Pointer to an int. The LS port number (OF number), will be filled. 
*/
rofl_result_t physical_switch_attach_port_to_logical_switch(switch_port_t* port, of_switch_t* sw, unsigned int* port_num);

/**
* @brief Attaches port to logical switch at port number port_num 
*
* The attachment may fail if there is already a port attached or
* another port was previously attached there (reuse of port numbers is
* strictly forbidden).
*
* @param port_num The LS port number (OF number) where to attach port.
*/
rofl_result_t physical_switch_attach_port_to_logical_switch_at_port_num(switch_port_t* port, of_switch_t* sw, unsigned int port_num);

/**
* @brief Detaches port located at port_num from logical switch sw. 
*/
rofl_result_t physical_switch_detach_port_num_from_logical_switch(unsigned int port_num, of_switch_t* sw);

/**
* @brief Detaches port from logical switch sw. 
*/
rofl_result_t physical_switch_detach_port_from_logical_switch(switch_port_t* port, of_switch_t* sw);

/**
* @brief Detaches all the ports from the logical switch 
*/
rofl_result_t physical_switch_detach_all_ports_from_logical_switch(of_switch_t* sw);

//C++ extern C
ROFL_PIPELINE_END_DECLS

#endif //PHYSICAL_SWITCH
