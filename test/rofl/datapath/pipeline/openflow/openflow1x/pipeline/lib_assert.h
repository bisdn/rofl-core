/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <assert.h>

/* Useful assertions */

#define ASSERT(condition, message) do { \
unsigned int result;\
if (!(result = condition)) { fprintf(stderr,(message"\n")); } \
assert ((result)); } while(0)


