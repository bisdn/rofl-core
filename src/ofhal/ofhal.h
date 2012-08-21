/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * ofhal.h
 *
 *  Created on: 16.08.2012
 *      Author: andreas
 */

#ifndef OFHAL_H
#define OFHAL_H 1

#include <inttypes.h>
#include <sys/uio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>




/*
 * data structures
 */

/*
 * return values
 */
enum ofhal_result_t {
	OFHAL_SUCCESS 		= 0,
	OFHAL_FAILURE		= 1,
};


#define OFHAL_IDX_NONE	0xffffffff


/* OpenFlow message types
 *
 * We redefine these numbers and the succeeding headers here in order to avoid
 * including the OpenFlow protocol specification itself via openflow.h.
 * However, we have to adjust this enumeration based on new message types
 * probably defined in upcoming OpenFlow versions.
 */
enum ofhal_ofp_types {
    OFHAL_OFPT_ERROR 						= 1,	/* Symmetric message */
    OFHAL_OFPT_EXPERIMENTER					= 4,    /* Symmetric message */

    OFHAL_OFPT_FEATURES_REQUEST				= 5,    /* Controller/switch message */
    OFHAL_OFPT_FEATURES_REPLY				= 6,    /* Controller/switch message */
    OFHAL_OFPT_GET_CONFIG_REQUEST			= 7,    /* Controller/switch message */
    OFHAL_OFPT_GET_CONFIG_REPLY				= 8,    /* Controller/switch message */
    OFHAL_OFPT_SET_CONFIG					= 9,    /* Controller/switch message */

    OFHAL_OFPT_PACKET_IN					= 10,   /* Async message */
    OFHAL_OFPT_FLOW_REMOVED					= 11,   /* Async message */
    OFHAL_OFPT_PORT_STATUS					= 12,   /* Async message */

    OFHAL_OFPT_PACKET_OUT					= 13,   /* Controller/switch message */
    OFHAL_OFPT_FLOW_MOD						= 14,   /* Controller/switch message */
    OFHAL_OFPT_GROUP_MOD					= 15,   /* Controller/switch message */
    OFHAL_OFPT_PORT_MOD						= 16,   /* Controller/switch message */
    OFHAL_OFPT_TABLE_MOD					= 17,   /* Controller/switch message */

    OFHAL_OFPT_STATS_REQUEST				= 18,   /* Controller/switch message */
    OFHAL_OFPT_STATS_REPLY					= 19,   /* Controller/switch message */

    OFHAL_OFPT_BARRIER_REQUEST				= 20,   /* Controller/switch message */
    OFHAL_OFPT_BARRIER_REPLY				= 21,   /* Controller/switch message */

    OFHAL_OFPT_QUEUE_GET_CONFIG_REQUEST		= 22,  /* Controller/switch message */
    OFHAL_OFPT_QUEUE_GET_CONFIG_REPLY		= 23,  /* Controller/switch message */

    OFHAL_OFPT_ROLE_REQUEST    				= 24, /* Controller/switch message */
    OFHAL_OFPT_ROLE_REPLY					= 25, /* Controller/switch message */
};

/*
 * data structures
 */


/*
 * common OpenFlow headers
 */

/* struct ofp_header */
struct ofhal_ofp_header {
	uint8_t version;
	uint8_t type;
	uint16_t length;
	uint32_t xid;
};

/* struct ofp_instruction */
struct ofhal_ofp_instruction {
	uint16_t type;
	uint16_t len;
	uint8_t  data[0];
};

/* struct ofp_action_header */
struct ofhal_ofp_action {
	uint16_t type;
	uint16_t len;
	uint8_t  data[0];
};

/* struct ofp_bucket */
struct ofhal_ofp_bucket {
	uint16_t len;
	uint8_t  data[0];
};

/* struct OXM TLV */
struct ofhal_ofp_oxm {
	uint16_t oxm_class;
	uint8_t  oxm_hasmask:1;
	uint8_t  oxm_field:7;
	uint8_t  oxm_len;
	uint8_t  data[0];
};


/* auxiliary lists for parsing OF messages
 * - list of instructions
 * - list of actions
 * - list of buckets
 * - list of OXM TLVs
 */


struct ofhal_oxm {
	struct ofhal_oxm				*next; // next OXM TLV or 0
	struct ofhal_oxm				*prev; // previous OXM TLV or 0
	struct ofhal_ofp_oxm			*oxm;  // pointer to OF OXM TLV
};

struct ofhal_action {
	struct ofhal_action				*next; // next action or 0
	struct ofhal_action				*prev; // previous action or 0
	struct ofhal_ofp_action			*action; // pointer to OF action
};

struct ofhal_bucket {
	struct ofhal_bucket 			*next; // next bucket or 0
	struct ofhal_bucket				*prev; // previous bucket or 0
	struct ofhal_ofp_bucket			*bucket; // pointer to OF bucket
	struct ofhal_action				*actions; // head of action list or 0
};

struct ofhal_instruction {
	struct ofhal_instruction 		*next; // next instruction or 0
	struct ofhal_instruction 		*prev; // previous instruction or 0
	struct ofhal_ofp_instruction 	*inst; // pointer to OF instruction
	struct ofhal_action				*actions; // head of action list or 0
};



/*
 * handle for coupling a common entity and a hardware driver in the HAL library
 */
struct ofhal_handle_t {
	struct ofhal_handle_t	*next;
	struct ofhal_handle_t	*prev;
	uint64_t 				 dpid;
	pthread_mutex_t			 mutex;			/* mutex locking this handle */

	pthread_mutex_t 		 hcl_mutex;		/* HCL queue mutex */
	pthread_cond_t			 hcl_cond;		/* HCL queue condition variable */
	int						 hcl_count;		/* HCL queue msg count */
	struct ofhcl_ops		*hclops;
	struct ofhal_msg_t     *h_hcl_queue; 	/* head of msg queue HCL to HAL */
	struct ofhal_msg_t     *t_hcl_queue; 	/* tail of msg queue HCL to HAL */

	pthread_mutex_t 		 hal_mutex;		/* HAL queue mutex */
	pthread_cond_t			 hal_cond;		/* HAL queue condition variable */
	int						 hal_count;		/* HAL queue msg count */
	struct ofhal_ops 		*halops;
	struct ofhal_msg_t     *h_hal_queue; 	/* head of msg queue HAL to HCL */
	struct ofhal_msg_t     *t_hal_queue; 	/* tail of msg queue HAL to HCL */

};






/*
 * Any OF message will be stored within an ofhal_msg_t structure.
 * This structure will be exposed to the common entity and the hardware driver.
 */
struct ofhal_msg_t {
	struct ofhal_msg_t 				*next;
	struct ofhal_msg_t				*prev;

	uint32_t 						 msgid; /* message id: will be assigned by library */
	struct iovec 					 iov;	/* iovec pointing to message buffer
							 	 	 	 	 * including OF header (and thus version
							 	 	 	 	 */
	struct ofhal_ofp_header 		*ofp_hdr;

	/* head of list of instructions
	 * set to 0 when no instructions are present */
	struct ofhal_instruction 		*head_insts;
	struct ofhal_instruction		*tail_insts;

	/* head of list of actions (e.g. in Packet-Out)
	 * set to 0 when no actions are present */
	struct ofhal_action				*head_actions;
	struct ofhal_action				*tail_actions;

	/* head of list of buckets
	 * set to 0 when no buckets are present */
	struct ofhal_bucket				*head_buckets;
	struct ofhal_bucket				*tail_buckets;

	/* head of list of OXM TLVs
	 * set to 0 when no OXM TLV list is present */
	struct ofhal_oxm				*head_oxms;
	struct ofhal_oxm				*tail_oxms;
};


/*
 * file operations for hardware driver
 */
struct ofhal_ops {
	/*
	 * management events
	 */
	int (*hcl_open)			(uint64_t dpid);	/* indicates that a common entity has connected to the library */
	int (*hcl_close)		(uint64_t dpid);	/* indicates that a common entity has disconnected from the library */

	/*
	 * unidirectional commands
	 */
	int (*packet_out)		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*flow_mod) 		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*group_mod) 		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*port_mod) 		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*table_mod)		(uint64_t dpid, struct ofhal_msg_t *msg);

	/*
	 * bidirectional requests
	 */
	int (*feature_req)		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*get_config_req)	(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*state_req)		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*queue_conf_req) 	(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*barrier_req) 		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*expr_req)			(uint64_t dpid, struct ofhal_msg_t *msg);
};


/*
 * file operations for hardware common layer (HCL)
 */
struct ofhcl_ops {
	/*
	 * management events
	 */
	int (*hal_open)			(uint64_t dpid);	/* indicates that a hardware driver has connected to the library */
	int (*hal_close)		(uint64_t dpid);	/* indicates that a hardware driver has disconnected from the library */

	/*
	 * asynchronous messages
	 */
	int (*packet_in)		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*flow_rmvd)		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*port_status)		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*error)			(uint64_t dpid, struct ofhal_msg_t *msg);

	/*
	 * bidirectional requests
	 */
	int (*feature_rep)		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*get_config_rep)	(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*state_rep)		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*queue_conf_rep)	(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*barrier_rep)		(uint64_t dpid, struct ofhal_msg_t *msg);
	int (*expr_rep)			(uint64_t dpid, struct ofhal_msg_t *msg);
};






/*
 * functions for access to the HAL by a common entity
 */

/**
 * @name    ofhcl_open
 * @brief   open HAL library for use by a common entity
 * @ingroup hcl
 *
 * Before using the HAL library, a common entity and a hardware driver must acquire a handle
 * of type ofhal_handle_t. Common entity and hardware driver are bound by
 * a unique identifier dpid. This is the HCL part of the two functions ofhcl_open() and ofhal_open().
 *
 * @param dpid A unique 64bit identified for the datapath
 * @param fops A structure containing function pointers for the OF specific callbacks
 *
 * @retval ofhcl_handle_t handle successfully created, library ready for use
 * @retval NULL Failure
 */
struct ofhal_handle_t*
ofhcl_open(
		uint64_t dpid,
		struct ofhcl_ops* fops);


/**
 * @name    ofhcl_close
 * @brief   close open handle on HAL library by a common entity
 * @ingroup hcl
 *
 * A common entity must close its handle to free library internal resources.
 *
 * @param dpid The handle obtained when opening the library for use.
 *
 * @retval OFHAL_SUCCESS common entity successfully detached from HAL library
 * @retval OFHAL_FAILURE a failure occured
 */
int
ofhcl_close(
		uint64_t dpid);


/**
 * @name	ofhcl_message
 * @brief	send an OpenFlow message to hardware driver
 * @ingroup hcl
 *
 * Function for sending an OpenFlow message to the hardware driver.
 * The message will be copied and stored for later parsing and transmission
 * to the hardware driver and returns immediately. It is safe to
 * free the buffer referred to by msg, msglen afterwards.
 *
 * @param handle The handle obtained when opening the library for use.
 * @param msg Pointer to start of frame of OpenFlow message to be sent to HAL
 * @param msglen Length of OpenFlow message pointed to by msg
 *
 * @retval OFHAL_SUCCESS message successfully stored for transmission to hardware driver
 * @retval OFHAL_FAILURE a failure occured
 */
int
ofhcl_message(
		struct ofhal_handle_t* handle,
		uint8_t *msg, size_t msglen);




/*
 * functions for access to the HAL by a hardware driver
 */

/**
 * @name    ofhal_open
 * @brief   open HAL library for use from a common entity
 * @ingroup hal
 *
 * Before using the HAL library, a hardware driver must acquire a handle
 * of type ofhal_handle_t.
 *
 * @param dpid A unique 64bit identified for the datapath
 * @param fops A structure containing function pointers for the OF specific callbacks
 *
 * @retval ofhal_handle_t handle successfully created, library ready for use
 * @retval NULL Failure
 */
struct ofhal_handle_t*
ofhal_open(
		uint64_t dpid,
		struct ofhal_ops* fops);


/**
 * @name    ofhal_close
 * @brief   close open handle on HAL library by a common entity
 * @ingroup hal
 *
 * A hardware driver must close its handle to free library internal resources.
 *
 * @param dpid The handle obtained when opening the library for use.
 *
 * @retval OFHAL_SUCCESS common entity successfully detached from HAL library
 * @retval OFHAL_FAILURE a failure occured
 */
int
ofhal_close(
		uint64_t dpid);


/**
 * @name	ofhal_message
 * @brief	send an OpenFlow message to common entity
 * @ingroup hal
 *
 * Function for sending an OpenFlow message to the common entity.
 * The message will be copied and stored for later parsing and transmission
 * to the hardware driver and returns immediately. It is safe to
 * free the buffer referred to by msg, msglen afterwards.
 *
 * @param handle The handle obtained when opening the library for use.
 * @param msg Pointer to start of frame of OpenFlow message to be sent to HCL
 * @param msglen Length of OpenFlow message pointed to by msg
 *
 * @retval OFHAL_SUCCESS message successfully stored for transmission to common entity
 * @retval OFHAL_FAILURE a failure occured
 */
int
ofhal_message(
		struct ofhal_handle_t* handle,
		uint8_t *msg, size_t msglen);








/*
 *  struct ofhal_handle_t related functions
 */

/**
 * @name 	ofhal_handle_create
 * @brief 	Allocate a new struct ofhal_handle_t
 * @ingroup	hal
 *
 * Allocates a new structure of type ofhal_handle_t and initializes
 * its values. The dpid parameter must be shared by common entity and
 * hardware driver.
 *
 * @param dpid Unique identifer for this handle, will be copied to handle->dpid
 *
 * @retval struct ofhal_handle_t Pointer to new structure of type ofhal_handle_t on success
 * @retval 0 otherwise
 */
struct ofhal_handle_t*
ofhal_handle_create(
		uint64_t dpid);


/**
 * @name	ofhal_handle_remove
 * @brief	Remove and free a handle of type struct ofhal_handle_t
 * @ingroup hal
 *
 * Checks for existence and deallocates an element of type struct ofhal_handle_t
 *
 * @param dpid Unique identifier for this handle
 */
void
ofhal_handle_remove(
		uint64_t dpid);



/**
 * @name 	ofhal_handle_find
 * @brief 	Find an existing struct ofhal_handle_t
 * @ingroup	hal
 *
 * Searches global list of handles for handle with specified dpid.
 *
 * @param dpid Unique identifer for this handle
 *
 * @retval struct ofhal_handle_t Pointer to existing structure of type ofhal_handle_t on success
 * @retval 0 otherwise
 */

struct ofhal_handle_t*
ofhal_handle_find(
		uint64_t dpid);






/*
 * struct ofhal_msg_t related functions
 */


/**
 * @name	ofhal_msg_create
 * @brief	create (allocate) an ofhal_msg_t structure
 * @ingroup	hal
 *
 * Allocates memory for a new structure of type ofhal_msg_t
 *
 * @retval ptr Pointer to new ifhal_msg_t structure. if successful
 * @retval 0 otherwise
 */
struct ofhal_msg_t*
ofhal_msg_create();


/**
 * @name	ofhal_msg_free
 * @brief	deallocate an ofhal_msg_t structure
 * @ingroup hal
 *
 * Frees memory used by a structure ofhal_msg_t
 *
 * @param msg Pointer to ofhal_msg_t structure
 */
void
ofhal_msg_remove(
		struct ofhal_msg_t *msg);


/**
 * @name 	ofhal_msglist_free
 * @brief	iterate over all elements of a msg list and call ofhal_msg_remove()
 * @ingroup	hal
 *
 * Deallocates all msgs in a msg list by calling ofhal_msg_remove(). Follows
 * next pointer in each msg.
 *
 * @param msglist points to head of msg list
 */
void
ofhal_msglist_free(
		struct ofhal_msg_t *msglist);






/*
 * start/stop routines for worker HCL/HAL threads
 */


/**
 * @name	ofhcl_work_start
 * @brief	Starts the HCL worker thread
 * @ingroup	hcl
 *
 * Starts the worker thread that dequeues msgs from handle->h_hcl_queue
 * and calls the appropriate handle->hal_ops functions.
 *
 */
void
ofhcl_work_start();


/**
 * @name	ofhcl_work_stop
 * @brief	Stops the HCL worker thread
 * @ingroup	hcl
 *
 * Stops the worker thread that dequeues msgs from handle->h_hcl_queue
 * and calls the appropriate handle->hal_ops functions.
 *
 */
void
ofhcl_work_stop();


/**
 * @name	ofhal_work_start
 * @brief	Starts the HAL worker thread
 * @ingroup	hal
 *
 * Starts the worker thread that dequeues msgs from handle->h_hal_queue
 * and calls the appropriate handle->hcl_ops functions.
 *
 */
void
ofhal_work_start();


/**
 * @name	ofhcl_work_stop
 * @brief	Stops the HAL worker thread
 * @ingroup	hal
 *
 * Stops the worker thread that dequeues msgs from handle->h_hal_queue
 * and calls the appropriate handle->hcl_ops functions.
 *
 */
void
ofhal_work_stop();







/*
 * worker threads
 */


/**
 * @name 	ofhcl_work
 * @brief	The HCL main thread that calls the hardware drivers function pointers from handle->halops.
 * @ingroup hcl
 *
 * Runs an indefinite loop with a condition variable handle->hcl_cond and waits for new packets to be queued
 * on handle->h_hcl_queue.
 *
 * @param arg is a void pointer
 *
 * @retval void pointer
 */
void*
ofhcl_work(void *arg);


/**
 * @name 	ofhal_work
 * @brief	The HAL main thread that calls the hardware drivers function pointers from handle->hclops.
 * @ingroup hal
 *
 * Runs an indefinite loop with a condition variable handle->hal_cond and waits for new packets to be queued
 * on handle->h_hal_queue.
 *
 * @param arg is a void pointer
 *
 * @retval void pointer
 */
void*
ofhal_work(void *arg);


/**
 * @name	ofhal_handle_msgs
 * @brief	Iterates over all msgs in a msg queue and calls the appropriate handler functions.
 *
 * Iterates over all msgs in a msg queue and calls the function handlers from handle->halops and
 * handle->hclops depending on the message type.
 *
 * @param handle The handle to be used
 * @param h_msglist Head of the msg queue to be sent to common entity or hardware driver
 */
void
ofhal_handle_msgs(
		struct ofhal_handle_t *handle,
		struct ofhal_msg_t *h_msglist);



void
ofhal_parse_msg(
		struct ofhal_msg_t *msg);


#endif /* OFHAL_H */


