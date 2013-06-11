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

class cofqueue_prop_list :
	public coflist<cofqueue_prop>
{
private:

	uint8_t of_version;

public:


	/**
	 *
	 */
	cofqueue_prop_list(
		uint8_t of_version);



	/**
	 *
	 */
	virtual
	~cofqueue_prop_list();



	/**
	 *
	 */
	cofqueue_prop_list(
		cofqueue_prop_list const& qpl);



	/**
	 */
	cofqueue_prop_list&
	operator= (
		cofqueue_prop_list const& qpl);


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


	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, cofqueue_prop_list const& qpl)
	{
		os << "QueuePropertyList: ";
		for (cofqueue_prop_list::const_iterator
				it = qpl.begin(); it != qpl.end(); ++it) {
			os << (*it) << " ";
		}
		return os;
	};
};

}

#endif /* COFQUEUEPROPLIST_H_ */
