/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HIVIEWDFX_HILOG_C_H
#define HIVIEWDFX_HILOG_C_H

#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Log domain, indicates the log service domainID. Different YangLogType have different domainID ranges.
 * Log type of YANG_APP: 0-0xFFFF
 * Log type of LOG_CORE & LOG_INIT: 0xD000000-0xD0FFFFF
 */
#ifndef YANG_DOMAIN
#define YANG_DOMAIN 0
#endif

/**
 * Log tag, indicates the log service tag, you can customize the tag as needed, usually the keyword of the module.
 */
#ifndef YANG_LOG_TAG
#define YANG_LOG_TAG NULL
#endif

/* Log type */
typedef enum {
    /* min log type */
    YANG_TYPE_MIN = 0,
    /* Used by app log. */
    YANG_APP = 0,
    /* Used by system log: recommended for important logs during the startup phase. */
    YANG_INIT = 1,
    /* Used by core service, framework. */
    YANG_CORE = 3,
    /* Used by kmsg log. */
    YANG_KMSG = 4,
    /* max log type */
    YANG_TYPE_MAX
} YangLogType;

/* Log level */
typedef enum {
    /* min log level */
    YANG_LEVEL_MIN = 0,
    /* Designates lower priority log. */
    YANG_DEBUG = 3,
    /* Designates useful information. */
    YANG_INFO = 4,
    /* Designates hazardous situations. */
    YANG_WARN = 5,
    /* Designates very serious errors. */
    YANG_ERROR = 6,
    /* Designates major fatal anomaly. */
    YANG_FATAL = 7,
    /* max log level */
    YANG_LEVEL_MAX,
} YangLogLevel;

/**
 * @brief Get log of fatal level
 */
const char* GetLastFatalMessage(void);

/**
 * @brief Print hilog
 *
 * @return If the log is successfully printed, returns the number of bytes written,
 * if failed, returns -1.
 */
int HiLogPrint(YangLogType type, YangLogLevel level, unsigned int domain, const char *tag, const char *fmt, ...)
    __attribute__((__format__(os_log, 5, 6)));

/**
 * @brief Hilog C interface of different log level
 *
 * @param type enum:YangLogType
 */
#define HILOG_DEBUG(type, ...) ((void)HILOG_IMPL((type), YANG_DEBUG, YANG_DOMAIN, YANG_LOG_TAG, __VA_ARGS__))

#define HILOG_INFO(type, ...) ((void)HILOG_IMPL((type), YANG_INFO, YANG_DOMAIN, YANG_LOG_TAG, __VA_ARGS__))

#define HILOG_WARN(type, ...) ((void)HILOG_IMPL((type), YANG_WARN, YANG_DOMAIN, YANG_LOG_TAG, __VA_ARGS__))

#define HIYANG_ERROR(type, ...) ((void)HILOG_IMPL((type), YANG_ERROR, YANG_DOMAIN, YANG_LOG_TAG, __VA_ARGS__))

#define HILOG_FATAL(type, ...) ((void)HILOG_IMPL((type), YANG_FATAL, YANG_DOMAIN, YANG_LOG_TAG, __VA_ARGS__))

/**
 * @brief Hilog C interface implementation
 *
 * @param type enum:YangLogType
 * @param level enum:YangLogLevel
 * @param domain macro:YANG_DOMAIN
 * @param tag macro:YANG_LOG_TAG
 */
#define HILOG_IMPL(type, level, domain, tag, ...) HiLogPrint(type, level, domain, tag, ##__VA_ARGS__)

/**
 * @brief Check whether log of a specified domain, tag and level can be printed.
 *
 * @param domain macro:YANG_DOMAIN
 * @param tag macro:YANG_LOG_TAG
 * @param level enum:YangLogLevel
 */
bool HiLogIsLoggable(unsigned int domain, const char *tag, YangLogLevel level);

#ifdef __cplusplus
}
#endif

#ifdef HILOG_RAWFORMAT
#include "hilog/log_inner.h"
#endif

#endif  // HIVIEWDFX_HILOG_C_H
