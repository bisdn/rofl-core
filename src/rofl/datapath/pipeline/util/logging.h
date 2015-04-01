/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ROFL_PIPELINE_LOG_H_
#define ROFL_PIPELINE_LOG_H_

#include "../../../common/utils/c_logger.h"

// add wrappers here

#define ROFL_PIPELINE_ERR(format, ...) ROFL_ERR(PIPELINE, "[rofl-pipeline] "format, ##__VA_ARGS__)

#define ROFL_PIPELINE_WARN(format, ...) ROFL_WARN(PIPELINE, "[rofl-pipeline] "format, ##__VA_ARGS__)

#define ROFL_PIPELINE_INFO(format, ...) ROFL_INFO(PIPELINE, "[rofl-pipeline] "format, ##__VA_ARGS__)

#define ROFL_PIPELINE_INFO_NO_PREFIX(format, ...) ROFL_INFO(PIPELINE, format, ##__VA_ARGS__)

#define ROFL_PIPELINE_DEBUG(format, ...) ROFL_DEBUG(PIPELINE, "[rofl-pipeline] "format, ##__VA_ARGS__)

#endif /* ROFL_PIPELINE_LOG_H_ */
