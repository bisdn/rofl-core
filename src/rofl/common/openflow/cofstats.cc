/*
 * cofstats.cc
 *
 *  Created on: 11.10.2012
 *      Author: andreas
 */


#include <rofl/common/openflow/cofstats.h>


cofstats::cofstats(
		size_t bodylen) :
	packed(0),
	body(bodylen)
{

}



cofstats::~cofstats()
{

}



size_t
cofstats::length()
{
	return body.memlen();
}



cmemory&
cofstats::pack()
{
	return body;
}


