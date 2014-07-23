/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdlib.h>
#include <sys/time.h>
#include "timers_hard_timeout.h"

int platform_gettimeofday(struct timeval * tval){
#ifdef TIMERS_FAKE_TIME

        time_forward(0,0,tval);

        //ROFL_PIPELINE_DEBUG("NOT usig real system time (%lu:%lu)\n", tval->tv_sec, tval->tv_usec);

        return 0;

#else

        //gettimeofday(tval,tzone);

        //ROFL_PIPELINE_DEBUG("<%s:%d> Time %lu:%lu\n",__func__,__LINE__,tval->tv_sec, tval->tv_usec);

        //return 0;

        return gettimeofday(tval,NULL);

#endif

}