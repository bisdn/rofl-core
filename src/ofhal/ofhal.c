#include <ofhal.h>


/*
 * global variables
 */

struct ofhal_handle_t  *h_regs;					/* head of list of registered dpath entities */
struct ofhal_handle_t 	*t_regs;				/* tail of list of registered dpath entities */
pthread_t 				ofhcl_work_thread;		/* ofhcl_worker thread handle */
pthread_t 				ofhal_work_thread;		/* ofhal_worker thread handle */
int 					ofhcl_work_init = 0;	/* running flag for HCL worker thread */
int 					ofhal_work_init = 0;	/* running flag for HAL worker thread */












/*
 * functions for public OFHAL interface
 */


struct ofhal_handle_t*
ofhcl_open(
		uint64_t dpid,
		struct ofhcl_ops* fops)
{
	struct ofhal_handle_t *handle = (struct ofhal_handle_t*)0;

	ofhcl_work_start();	/* start HCL worker thread */

	/* One handle exists for binding a common entity (HCL) and
	 * a hardware driver (HAL). Both connect to the library
	 * using the same 64-bit identifier (dpid). That means
	 * a handle may exist when either of the two opens the
	 * library or not. Check for this condition.
	 */


	handle = ofhal_handle_find(dpid);

	if ((struct ofhal_handle_t*)0 == handle)
	{
		handle = ofhal_handle_create(dpid);
	}


	if ((struct ofhcl_ops*)0 == handle->hclops)
	{
		/* handle is already allocated */
		return 0;
	}

	pthread_mutex_lock(&(handle->mutex));

	if ((struct ofhcl_ops*)0 ==
			(handle->hclops = (struct ofhcl_ops*)calloc(1, sizeof(struct ofhcl_ops))))
	{
		pthread_mutex_unlock(&(handle->mutex));

		return 0;
	}

	memcpy((void*)handle->hclops, (void*)fops, sizeof(struct ofhcl_ops));

	pthread_mutex_unlock(&(handle->mutex));

	return handle;
}


int
ofhcl_close(
		uint64_t dpid)
{
	struct ofhal_handle_t 	*handle = (struct ofhal_handle_t*)0;
	struct ofhal_msg_t 		*msg	= (struct ofhal_msg_t*)0;

	/* search for handle identified by dpid */
	if ((struct ofhal_handle_t*)0 == (handle = ofhal_handle_find(dpid)))
	{
		return OFHAL_FAILURE;
	}


	/* remove all remaining HCL messages stored in handle */
	ofhal_msglist_free(handle->h_hcl_queue);


	/* remove ofhcl_ops struct */
	if (handle->hclops)
	{
		free(handle->hclops);
		handle->hclops = (struct ofhcl_ops*)0;
	}


	/* no hardware driver and no common entity attached, remove handle */
	if ((0 == handle->hclops) && (0 == handle->halops))
	{
		ofhal_handle_remove(dpid);
	}


	 /* stop HCL worker thread, if no handles at all are left */
	if ((struct ofhal_handle_t*)0 == h_regs)
	{
		ofhcl_work_stop();
	}

	return OFHAL_SUCCESS;
}


int
ofhcl_message(
		struct ofhal_handle_t* handle,
		uint8_t *buf, size_t buflen)
{
	struct ofhal_msg_t *msg = 0;

	if ((0 == handle) || (0 == ofhal_handle_find(handle->dpid)))
	{
		return OFHAL_FAILURE;
	}

	msg = ofhal_msg_create();

	msg->iov.iov_len  = buflen;
	if (0 == (msg->iov.iov_base = (void*)calloc(1, msg->iov.iov_len)))
	{
		ofhal_msg_remove(msg);
		return OFHAL_FAILURE;
	}

	memcpy(msg->iov.iov_base, buf, buflen);





	pthread_mutex_lock(&(handle->hcl_mutex));

	/*
	 * msg queue is empty
	 */
	if (0 == handle->h_hcl_queue)
	{
		handle->h_hcl_queue = msg;
		handle->t_hcl_queue = msg;
	}
	/*
	 * append msg
	 */
	else
	{
		handle->t_hcl_queue->next = msg;
		msg->prev = handle->t_hcl_queue;
		handle->t_hcl_queue = msg;
	}

	handle->hcl_count++;

	pthread_cond_signal(&(handle->hcl_cond));

	pthread_mutex_unlock(&(handle->hcl_mutex));

	return OFHAL_SUCCESS;
}


struct ofhal_handle_t*
ofhal_open(
		uint64_t dpid,
		struct ofhal_ops* fops)
{
	struct ofhal_handle_t *handle = (struct ofhal_handle_t*)0;

	ofhal_work_start();	/* start HAL worker thread */

	/* One handle exists for binding a common entity (HCL) and
	 * a hardware driver (HAL). Both connect to the library
	 * using the same 64-bit identifier (dpid). That means
	 * a handle may exist when either of the two opens the
	 * library or not. Check for this condition.
	 */


	handle = ofhal_handle_find(dpid);

	if ((struct ofhal_handle_t*)0 == handle)
	{
		handle = ofhal_handle_create(dpid);
	}


	if ((struct ofhal_ops*)0 == handle->halops)
	{
		/* handle is already allocated */
		return 0;
	}

	pthread_mutex_lock(&(handle->mutex));

	if ((struct ofhal_ops*)0 ==
			(handle->halops = (struct ofhal_ops*)calloc(1, sizeof(struct ofhal_ops))))
	{
		pthread_mutex_unlock(&(handle->mutex));

		return 0;
	}

	memcpy((void*)handle->halops, (void*)fops, sizeof(struct ofhal_ops));

	pthread_mutex_unlock(&(handle->mutex));

	return handle;
}


int
ofhal_close(
		uint64_t dpid)
{
	struct ofhal_handle_t 	*handle = (struct ofhal_handle_t*)0;
	struct ofhal_msg_t 		*msg	= (struct ofhal_msg_t*)0;

	/* search for handle identified by dpid */
	if ((struct ofhal_handle_t*)0 == (handle = ofhal_handle_find(dpid)))
	{
		return OFHAL_FAILURE;
	}


	/* remove all remaining HCL messages stored in handle */
	ofhal_msglist_free(handle->h_hal_queue);


	/* remove ofhal_ops struct */
	if (handle->halops)
	{
		free(handle->halops);
		handle->halops = (struct ofhal_ops*)0;
	}


	/* no hardware driver and no common entity attached, remove handle */
	if ((0 == handle->hclops) && (0 == handle->halops))
	{
		ofhal_handle_remove(dpid);
	}


	 /* stop HAL worker thread, if no handles at all are left */
	if ((struct ofhal_handle_t*)0 == h_regs)
	{
		ofhal_work_stop();
	}

	return OFHAL_SUCCESS;
}



int
ofhal_message(
		struct ofhal_handle_t* handle,
		uint8_t *buf, size_t buflen)
{
	struct ofhal_msg_t *msg = 0;

	if ((0 == handle) || (0 == ofhal_handle_find(handle->dpid)))
	{
		return OFHAL_FAILURE;
	}

	msg = ofhal_msg_create();

	msg->iov.iov_len  = buflen;
	if (0 == (msg->iov.iov_base = (void*)calloc(1, msg->iov.iov_len)))
	{
		ofhal_msg_remove(msg);
		return OFHAL_FAILURE;
	}

	memcpy(msg->iov.iov_base, buf, buflen);





	pthread_mutex_lock(&(handle->hal_mutex));

	/*
	 * msg queue is empty
	 */
	if (0 == handle->h_hal_queue)
	{
		handle->h_hal_queue = msg;
		handle->t_hal_queue = msg;
	}
	/*
	 * append msg
	 */
	else
	{
		handle->t_hal_queue->next = msg;
		msg->prev = handle->t_hal_queue;
		handle->t_hal_queue = msg;
	}

	handle->hal_count++;

	pthread_cond_signal(&(handle->hal_cond));

	pthread_mutex_unlock(&(handle->hal_mutex));

	return OFHAL_SUCCESS;
}
















/*
 * "handle" related functions
 */



struct ofhal_handle_t*
ofhal_handle_create(
		uint64_t dpid)
{
	struct ofhal_handle_t *handle = (struct ofhal_handle_t*)0;

	if ((handle = (struct ofhal_handle_t*)calloc(1, sizeof(struct ofhal_handle_t))) == 0)
	{
		return (struct ofhal_handle_t*)0;
	}

	handle->dpid			= dpid;
	handle->next 			= (struct ofhal_handle_t*)0;
	handle->prev 			= (struct ofhal_handle_t*)0;
	handle->h_hal_queue 	= (struct ofhal_msg_t*)0;
	handle->t_hal_queue 	= (struct ofhal_msg_t*)0;
	handle->h_hcl_queue 	= (struct ofhal_msg_t*)0;
	handle->t_hcl_queue 	= (struct ofhal_msg_t*)0;

	pthread_mutex_init(&(handle->mutex), 0);
	pthread_mutex_init(&(handle->hcl_mutex), 0);
	pthread_cond_init(&(handle->hcl_cond), 0);
	pthread_mutex_init(&(handle->hal_mutex), 0);
	pthread_cond_init(&(handle->hal_cond), 0);

	/*
	 * list is empty
	 */
	if ((struct ofhal_handle_t*)0 == h_regs)
	{
		h_regs = handle;
		t_regs = handle;
	}
	/*
	 * at least one element is in list, append handle to last element
	 */
	else
	{
		t_regs->next = handle;
		handle->prev = t_regs;
		handle->next = (struct ofhal_handle_t*)0;
		t_regs = handle;
	}

	return handle;
}


void
ofhal_handle_remove(
		uint64_t dpid)
{
	/*
	 * list is empty
	 */
	if ((struct ofhal_handle_t*)0 == h_regs)
	{
		return;
	}
	else
	/*
	 * at least one element is in the list
	 */
	{
		struct ofhal_handle_t *curr = h_regs;

		while ((struct ofhal_handle_t*)0 != curr)
		{
			if (curr->dpid == dpid)
			{
				struct ofhal_handle_t *next = curr->next;
				struct ofhal_handle_t *prev = curr->prev;

				if (next)
				{
					if (prev)
					{
						next->prev = prev;
					}
					else
					{
						h_regs = next;
						next->prev = (struct ofhal_handle_t*)0;
					}
				}

				if (prev)
				{
					if (next)
					{
						prev->next = next;
					}
					else
					{
						t_regs = prev;
						prev->next = (struct ofhal_handle_t*)0;
					}
				}

				/* TODO: fix for pending lock first */
				pthread_mutex_destroy(&(curr->mutex));
				pthread_cond_destroy(&(curr->hcl_cond));
				pthread_mutex_destroy(&(curr->hcl_mutex));
				pthread_cond_destroy(&(curr->hal_cond));
				pthread_mutex_destroy(&(curr->hal_mutex));

				free(curr);
			}
			else
			{
				curr = curr->next;
			}
		}
	}
}


struct ofhal_handle_t*
ofhal_handle_find(
		uint64_t dpid)
{
	if ((struct ofhal_handle_t*)0 == h_regs)
	{
		return 0;
	}

	struct ofhal_handle_t *curr = h_regs;

	while ((struct ofhal_handle_t*)0 != curr)
	{
		if (curr->dpid == dpid)
		{
			return curr;
		}

		curr = curr->next;
	}

	return 0;
}


void
ofhal_msglist_free(struct ofhal_msg_t *msg)
{
	while ((struct ofhal_msg_t*)0 != msg)
	{
		struct ofhal_msg_t *tmp = msg;

		msg = msg->next;

		ofhal_msg_remove(tmp);
	}
}


struct ofhal_msg_t*
ofhal_msg_create()
{
	struct ofhal_msg_t *msg = 0;

	if (0 == (msg = (struct ofhal_msg_t*)calloc(1, sizeof(struct ofhal_msg_t))))
	{
		return 0;
	}

	msg->iov.iov_base = 0;
	msg->iov.iov_len = 0;
	msg->next = 0;
	msg->prev = 0;
	msg->ofh_header = 0;
	msg->head_actions = 0;
	msg->tail_actions = 0;
	msg->head_buckets = 0;
	msg->tail_buckets = 0;
	msg->head_insts = 0;
	msg->tail_insts = 0;
	msg->head_oxms = 0;
	msg->tail_oxms = 0;

	return msg;
}


void
ofhal_msg_remove(
		struct ofhal_msg_t *msg)
{
	if (0 == msg)
	{
		return;
	}

	/* TODO: deallocate action, bucket, instruction, OXM TLV lists */

	free(msg);
}



















/*
 * start/stop routines for worker threads
 */


void
ofhcl_work_start()
{
	if (1 == ofhcl_work_init)
	{
		return;
	}

	int rc = 0;

	rc = pthread_create(&ofhcl_work_thread, (pthread_attr_t*)0,
			&(ofhcl_work), (void*)0);

	if (rc < 0)
	{
		fprintf(stderr, "ofhcl_work_run() thread creation failed: %d (%s)\n", errno, strerror(errno));
		return;
	}

	ofhcl_work_init = 1;
}


void
ofhcl_work_stop()
{
	if (0 == ofhcl_work_init)
	{
		return;
	}

	pthread_cancel(ofhcl_work_thread);

	ofhcl_work_init = 0;
}


void
ofhal_work_start()
{
	if (1 == ofhal_work_init)
	{
		return;
	}

	int rc = 0;

	rc = pthread_create(&ofhal_work_thread, (pthread_attr_t*)0,
			&(ofhal_work), (void*)0);

	if (rc < 0)
	{
		fprintf(stderr, "ofhcl_work_run() thread creation failed: %d (%s)\n", errno, strerror(errno));
		return;
	}

	ofhal_work_init = 1;
}


void
ofhal_work_stop()
{
	if (0 == ofhal_work_init)
	{
		return;
	}

	pthread_cancel(ofhal_work_thread);

	ofhal_work_init = 0;
}















/*
 * worker threads
 */


void*
ofhcl_work(void *arg)
{
	while (1)
	{
		struct ofhal_handle_t *handle = 0;

		/* lock condition variable */
		pthread_mutex_lock(&(handle->hcl_mutex));

		pthread_cond_wait(&(handle->hcl_cond), &(handle->hcl_mutex));


		if (((struct ofhal_handle_t*)0 != h_regs) && (handle->hcl_count > 0))
		{
			handle = h_regs;

			while ((struct ofhal_handle_t*)0 != handle)
			{
				struct ofhal_msg_t *h_msglist = 0;

				/* get msg list from handle with lock */
				pthread_mutex_lock(&(handle->mutex));

				h_msglist = handle->h_hcl_queue;

				handle->h_hcl_queue = (struct ofhal_msg_t*)0;
				handle->t_hcl_queue = (struct ofhal_msg_t*)0;

				pthread_mutex_unlock(&(handle->mutex));



				/* handle all messages in list */
				ofhal_handle_msgs(handle, h_msglist);


				/* continue with next handle */
				handle = handle->next;
			}
		}

		handle->hcl_count = 0;

		pthread_mutex_unlock(&(handle->hcl_mutex));
	}

	return 0;
}


void*
ofhal_work(void *arg)
{
	while (1)
	{
		struct ofhal_handle_t *handle = 0;

		/* lock condition variable */
		pthread_mutex_lock(&(handle->hal_mutex));

		pthread_cond_wait(&(handle->hal_cond), &(handle->hal_mutex));


		if (((struct ofhal_handle_t*)0 != h_regs) && (handle->hal_count > 0))
		{
			handle = h_regs;

			while ((struct ofhal_handle_t*)0 != handle)
			{
				struct ofhal_msg_t *h_msglist = 0;

				/* get msg list from handle with lock */

				h_msglist = handle->h_hal_queue;

				handle->h_hal_queue = (struct ofhal_msg_t*)0;
				handle->t_hal_queue = (struct ofhal_msg_t*)0;




				/* handle all messages in list */
				ofhal_handle_msgs(handle, h_msglist);


				/* continue with next handle */
				handle = handle->next;
			}
		}

		handle->hal_count = 0;

		pthread_mutex_unlock(&(handle->hal_mutex));
	}

	return 0;
}


void
ofhal_handle_msgs(
		struct ofhal_handle_t *handle,
		struct ofhal_msg_t *h_msglist)
{
	struct ofhal_msg_t *msg = 0;


	if ((struct ofhal_handle_t*)0 == handle)
	{
		return; /* no handle => return */
	}

	if ((struct ofhal_msg_t*)0 == h_msglist)
	{
		return; /* empty msg list? => return */
	}



	/*
	 *
	 */
	msg = h_msglist;

	while ((struct ofhal_msg_t*)0 != msg)
	{
		/* TODO: parse and call appropriate function from target fops */
		ofhal_parse_msg(msg);

		pthread_mutex_lock(&(handle->mutex));

		switch (msg->ofh_header->type) {
		/*
		 * HCL -> HAL
		 */
#if 0
		case OFPT_EXPERIMENTER:
			if (handle->halops && handle->halops->expr_req)
			{
				handle->halops->expr_req(msg);
			}
			break;
#endif
		case OFPT_FEATURES_REQUEST:
			if (handle->halops && handle->halops->feature_req)
			{
				handle->halops->feature_req(msg);
			}
			break;
		case OFPT_GET_CONFIG_REQUEST:
			if (handle->halops && handle->halops->get_config_req)
			{
				handle->halops->get_config_req(msg);
			}
			break;
		case OFPT_STATS_REQUEST:
			if (handle->halops && handle->halops->state_req)
			{
				handle->halops->state_req(msg);
			}
			break;
		case OFPT_QUEUE_GET_CONFIG_REQUEST:
			if (handle->halops && handle->halops->queue_conf_req)
			{
				handle->halops->queue_conf_req(msg);
			}
			break;
		case OFPT_BARRIER_REQUEST:
			if (handle->halops && handle->halops->barrier_req)
			{
				handle->halops->barrier_req(msg);
			}
			break;
		case OFPT_FLOW_MOD:
			if (handle->halops && handle->halops->flow_mod)
			{
				handle->halops->flow_mod(msg);
			}
			break;
		case OFPT_GROUP_MOD:
			if (handle->halops && handle->halops->group_mod)
			{
				handle->halops->group_mod(msg);
			}
			break;
		case OFPT_PORT_MOD:
			if (handle->halops && handle->halops->port_mod)
			{
				handle->halops->port_mod(msg);
			}
			break;
		case OFPT_TABLE_MOD:
			if (handle->halops && handle->halops->table_mod)
			{
				handle->halops->table_mod(msg);
			}
			break;
		case OFPT_PACKET_OUT:
			if (handle->halops && handle->halops->packet_out)
			{
				handle->halops->packet_out(msg);
			}
			break;

			/*
			 * HAL -> HCL
			 */
		case OFPT_ERROR:
			if (handle->hclops && handle->hclops->error)
			{
				handle->hclops->error(msg);
			}
			break;
		case OFPT_PACKET_IN:
			if (handle->hclops && handle->hclops->packet_in)
			{
				handle->hclops->packet_in(msg);
			}
			break;
		case OFPT_FLOW_REMOVED:
			if (handle->hclops && handle->hclops->flow_rmvd)
			{
				handle->hclops->flow_rmvd(msg);
			}
			break;
		case OFPT_PORT_STATUS:
			if (handle->hclops && handle->hclops->port_status)
			{
				handle->hclops->port_status(msg);
			}
			break;
		case OFPT_FEATURES_REPLY:
			if (handle->hclops && handle->hclops->feature_rep)
			{
				handle->hclops->feature_rep(msg);
			}
			break;
		case OFPT_GET_CONFIG_REPLY:
			if (handle->hclops && handle->hclops->get_config_rep)
			{
				handle->hclops->get_config_rep(msg);
			}
			break;
		case OFPT_STATS_REPLY:
			if (handle->hclops && handle->hclops->state_rep)
			{
				handle->hclops->state_rep(msg);
			}
			break;
		case OFPT_QUEUE_GET_CONFIG_REPLY:
			if (handle->hclops && handle->hclops->queue_conf_rep)
			{
				handle->hclops->queue_conf_rep(msg);
			}
			break;
		case OFPT_BARRIER_REPLY:
			if (handle->hclops && handle->hclops->barrier_rep)
			{
				handle->hclops->barrier_rep(msg);
			}
			break;
		}

		pthread_mutex_unlock(&(handle->mutex));

		msg = msg->next;
	}
}


void
ofhal_parse_msg(
		struct ofhal_msg_t *msg)
{
	/* TODO: parse message and construct help pointers */
}


