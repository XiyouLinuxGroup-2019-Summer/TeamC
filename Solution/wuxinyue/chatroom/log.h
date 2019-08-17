/*************************************************************************
	> File Name: log.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 18时36分16秒
 ************************************************************************/
#ifndef LOG_H
#define LOG_H

#include "define.h"

typedef enum log_level {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
} log_level_t;

extern log_level_t curr_log_level;

#define log_debug(args ...) log_message(LOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, args)
#define log_info(args ...) log_message(LOG_INFO, __FILE__, __LINE__, __FUNCTION__, args)
#define log_warn(args ...) log_message(LOG_WARNING, __FILE__, __LINE__, __FUNCTION__, args)
#define log_error(args ...) log_message(LOG_ERROR, __FILE__, __LINE__, __FUNCTION__, args)

#define log_errno_error(msg) do {\
    char errbuf[128];\
        strerror_r(errno, errbuf, sizeof(errbuf));\
        log_error(msg ": %s", errbuf);\
} while(0)

int set_log_filename(const char *filename);

// set log level
void set_log_level(log_level_t level);

// log something
void log_message(log_level_t level, const char *file, int line, const char *func, const char *fmt, ...);

#endif // LOG_H

