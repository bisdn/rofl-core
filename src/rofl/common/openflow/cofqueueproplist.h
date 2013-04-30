/*
 * cofqueueproplist.h
 *
 *  Created on: 30.04.2013
 *      Author: andi
 */

#ifndef COFQUEUEPROPLIST_H_
#define COFQUEUEPROPLIST_H_ 1

#include "../cvastring.h"
#include "../cerror.h"
#include "../coflist.h"
#include "openflow.h"

#include "cofqueueprop.h"

namespace rofl
{

class cofqueueproplist :
	public coflist<cofqueue_prop>
{
private:

	uint8_t of_version;

public:


	/**
	 *
	 */
	cofqueueproplist(
		uint8_t of_version);



	/**
	 *
	 */
	virtual
	~cofqueueproplist();



	/**
	 *
	 */
	cofqueueproplist(
		cofqueueproplist const& qpl);



	/**
	 */
	cofqueueproplist&
	operator= (
		cofqueueproplist const& qpl);


public:


	/**
	 *
	 */
	virtual size_t
	length() const;



	/**
	 *
	 */
	virtual void
	unpack(
		uint8_t *buf, size_t buflen);



	/**
	 *
	 */
	virtual void
	pack(
		uint8_t *buf,
		size_t buflen) const;


public:


	/**
	 *
	 */
	cofqueue_prop&
	find_queue_prop(
		uint16_t property);

};

}

#endif /* COFQUEUEPROPLIST_H_ */
