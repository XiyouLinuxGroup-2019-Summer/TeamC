/*************************************************************************
	> File Name: log.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月17日 星期六 18时36分54秒
 ************************************************************************/

#include "log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libgen.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>

#define LOG_LINE_MAX  1024

static FILE *logger_fp = NULL;

log_level_t curr_log_level = LOG_INFO;

static const char *log_level_str(log_level_t level)
{
    static const char *level_str[] = {"DEUBG", "INFO", "WARN", "ERROR"};
    return level_str[level];
}

static void close_log_file(void)
{
    if (logger_fp && logger_fp != stderr) {
        fclose(logger_fp);
    }
}

int set_log_filename(const char *filename)
{
    close_log_file();
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        return CR_ERR;
    }
    logger_fp = fp;

    // fclose log file when exit
    atexit(close_log_file);

    return CR_OK;
}

void set_log_level(log_level_t level)
{
    curr_log_level = level;
}

void log_message(log_level_t level, const char *file, int line, const char *func, const char *fmt, ...)
{
    char logger_buf[LOG_LINE_MAX];

    // level filter
    if (level < curr_log_level) {
        return;
    }
    
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(logger_buf, sizeof(logger_buf), fmt, ap);
    va_end(ap);

    struct timeval tv;
    gettimeofday(&tv, NULL);

    time_t time_s = tv.tv_sec;
    struct tm now_tm;
    localtime_r(&time_s, &now_tm);

    char datetime[64];
    int off = strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S.", &now_tm);
    snprintf(datetime + off, sizeof(datetime) - off, "%06d", (int)tv.tv_usec);

    int tid = syscall(SYS_gettid);

    if (!logger_fp) {
        logger_fp = stderr;
    }

    // [datetime] level func(file:line) [tid] ...
    fprintf(logger_fp, "[%s] %s %s(%s:%d) [%d] %s\n",
            datetime, log_level_str(level), func, basename((char *)file), line, tid, logger_buf);
}
