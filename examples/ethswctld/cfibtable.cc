/*
 * cfib.cc
 *
 *  Created on: 15.07.2013
 *      Author: andreas
 */

#include "cfibtable.h"

using namespace etherswitch;

std::map<rofl::cdptid, cfibtable*> cfibtable::fibtables;

