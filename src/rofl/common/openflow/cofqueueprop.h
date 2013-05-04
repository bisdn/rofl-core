/*
 * cofqueueprop.h
 *
 *  Created on: 29.04.2013
 *      Author: andi
 */

#ifndef COFQUEUEPROP_H_
#define COFQUEUEPROP_H_ 1

#include "../cmemory.h"
#include "../cerror.h"
#include "openflow.h"
#include "openflow_rofl_exceptions.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <endian.h>
#ifndef htobe16
#include "../endian_conversion.h"
#endif
#ifdef __cplusplus
}
#endif

namespace rofl
{

class eQueuePropBase : public cerror {};
class eQueuePropNotFound : public eQueuePropBase {};


class cofqueue_prop :
		public cmemory
{
private:

	union {
		uint8_t							*ofqu_hdr;
		struct ofp10_queue_prop_header	*ofqu10_hdr;
		struct ofp12_queue_prop_header	*ofqu12_hdr;
		//struct ofp13_queue_prop_header	*ofqu13_hdr;
	} ofq_ofqu;

#define ofq_header ofq_ofqu.ofqu_hdr
#define ofq10_header ofq_ofqu.ofqu10_hdr
#define ofq12_header ofq_ofqu.ofqu12_hdr
//#define ofq13_header ofq_ofqu.ofqu13_hdr

protected:

	uint8_t of_version;

public:

	/**
	 *
	 */
	cofqueue_prop(
			uint8_t of_version = OFP12_VERSION);


	/**
	 *
	 */
	virtual
	~cofqueue_prop();


	/**
	 *
	 */
	cofqueue_prop(
			cofqueue_prop const& qp);


	/**
	 *
	 */
	cofqueue_prop&
	operator= (
			cofqueue_prop const& qp);


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
	pack(uint8_t *buf, size_t buflen) const;


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	uint16_t
	get_property() const;


	/**
	 *
	 */
	void
	set_property(
			uint16_t property);


	/**
	 *
	 */
	uint16_t
	get_length() const;


	/**
	 *
	 */
	void
	set_length(uint16_t len);

};



class cofqueue_prop_min_rate :
		public cofqueue_prop
{
private:

	union {
		uint8_t								*ofqu_min_rate;
		struct ofp10_queue_prop_min_rate	*ofqu10_min_rate;
		struct ofp12_queue_prop_min_rate	*ofqu12_min_rate;
		//struct ofp13_queue_prop_min_rate	*ofqu13_min_rate;
	} ofq_ofqu;

#define ofq_min_rate ofq_ofqu.ofqu_min_rate
#define ofq10_min_rate ofq_ofqu.ofqu10_min_rate
#define ofq12_min_rate ofq_ofqu.ofqu12_min_rate
//#define ofqu13_min_rate ofq_ofqu.ofqu13_min_rate

public:

	/**
	 *
	 */
	cofqueue_prop_min_rate(
			uint8_t of_version, uint16_t rate = 0);


	/**
	 *
	 */
	virtual
	~cofqueue_prop_min_rate();


	/**
	 *
	 */
	cofqueue_prop_min_rate(
			cofqueue_prop_min_rate const& qp);



	/**
	 *
	 */
	cofqueue_prop_min_rate&
	operator= (
			cofqueue_prop const& qp);


public:


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf, size_t buflen) const;


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	uint16_t
	get_rate() const;


	/**
	 *
	 */
	void
	set_rate(uint16_t rate);

};




class cofqueue_prop_max_rate :
		public cofqueue_prop
{
private:

	union {
		uint8_t								*ofqu_max_rate;
		struct ofp12_queue_prop_min_rate	*ofqu12_max_rate;
		//struct ofp13_queue_prop_min_rate	*ofqu13_max_rate;
	} ofq_ofqu;

#define ofq_max_rate ofq_ofqu.ofqu_max_rate
#define ofq12_max_rate ofq_ofqu.ofqu12_max_rate
//#define ofqu13_max_rate ofq_ofqu.ofqu13_max_rate

public:

	/**
	 *
	 */
	cofqueue_prop_max_rate(
			uint8_t of_version, uint16_t rate = 0);


	/**
	 *
	 */
	virtual
	~cofqueue_prop_max_rate();



	/**
	 *
	 */
	cofqueue_prop_max_rate(
			cofqueue_prop_max_rate const& qp);



	/**
	 *
	 */
	cofqueue_prop_max_rate&
	operator= (
			cofqueue_prop const& qp);



public:


	/**
	 *
	 */
	virtual void
	pack(uint8_t *buf, size_t buflen) const;


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	uint16_t
	get_rate() const;


	/**
	 *
	 */
	void
	set_rate(uint16_t rate);

};





class cofqueue_prop_expr :
		public cofqueue_prop
{
private:

	union {
		uint8_t									*ofqu_expr;
		struct ofp12_queue_prop_experimenter	*ofqu12_expr;
		//struct ofp13_queue_prop_min_rate		*ofqu13_expr;
	} ofq_ofqu;

#define ofq_expr ofq_ofqu.ofqu_expr
#define ofq12_expr ofq_ofqu.ofqu12_expr
//#define ofqu13_expr ofq_ofqu.ofqu13_expr

	cmemory body;

public:

	/**
	 *
	 */
	cofqueue_prop_expr(
			uint8_t of_version);


	/**
	 *
	 */
	virtual
	~cofqueue_prop_expr();


	/**
	 *
	 */
	cofqueue_prop_expr(
			cofqueue_prop_expr const& qp);


	/**
	 *
	 */
	cofqueue_prop_expr&
	operator= (
			cofqueue_prop_expr const& qp);


	/**
	 *
	 */
	cofqueue_prop_expr&
	operator= (
			cofqueue_prop const& qp);



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
	pack(uint8_t *buf, size_t buflen) const;


	/**
	 *
	 */
	virtual void
	unpack(uint8_t *buf, size_t buflen);


public:


	/**
	 *
	 */
	uint32_t
	get_expr() const;


	/**
	 *
	 */
	void
	set_expr(uint32_t expr);


	/**
	 *
	 */
	cmemory&
	get_body();
};



}

#endif /* COFQUEUEPROP_H_ */
