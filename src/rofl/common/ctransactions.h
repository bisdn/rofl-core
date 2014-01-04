/*
 * ctransactions.h
 *
 *  Created on: 04.01.2014
 *      Author: andi
 */

#ifndef CTRANSACTIONS_H_
#define CTRANSACTIONS_H_

#include <map>

#include "ctransaction.h"

namespace rofl {
namespace openflow {

class ctransactions;

class ctransactions_env {
public:
	virtual ~ctransactions_env() {};
	virtual void ta_expired(ctransactions *tas, ctransaction *ta);
};

class ctransactions :
		public std::map<uint32_t, ctransaction>
{
	ctransactions_env			*env;

public:

	ctransactions(ctransactions_env *env);

	virtual
	~ctransactions();


};

}; // namespace openflow
}; // namespace rofl



#endif /* CTRANSACTIONS_H_ */
