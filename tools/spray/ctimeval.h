/*
 * ctimeval.h
 *
 *  Created on: 01.09.2013
 *      Author: andreas
 */

#ifndef CTIMEVAL_H_
#define CTIMEVAL_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#ifdef __cplusplus
}
#endif

namespace spray
{

class ctimeval
{
	struct timeval tv;

public:

	static ctimeval
	now();

public:

	ctimeval();

	virtual
	~ctimeval();

	ctimeval(const ctimeval& tval);

	ctimeval& operator= (const ctimeval& tval);
};

}; // end of namespace

#endif /* CTIMEVAL_H_ */
