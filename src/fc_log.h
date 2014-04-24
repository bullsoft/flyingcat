/*
 * Copyright (C) 2014 BullSoft
 * Copyright (C) 2014 Shang Yuanchun <idealities@gmail.com>
 *
 *
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * FlyingCat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FlyingCat. If not, write to:
 * The Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301, USA.
 */

#ifndef FC_LOG_H
#define FC_LOG_H

#include "config.h"
#include "fc_util.h"

#define FC_LOG_EMERG     0
#define FC_LOG_ALERT     1
#define FC_LOG_CRIT      2
#define FC_LOG_ERR       3
#define FC_LOG_ERROR     FC_LOG_ERR
#define FC_LOG_WARN      4
#define FC_LOG_WARNING   FC_LOG_WARN
#define FC_LOG_NOTICE    5
#define FC_LOG_INFO      6
#define FC_LOG_DEBUG     7
#define FC_LOG_VERB      8

#define FC_MAX_ERR_STR   1024

struct fc_log_s {
    int    log_level;
    char  *file;
    int    fd;
    size_t nerr;
};

typedef struct fc_log_s fc_log_t;

#ifdef FC_DEBUG_LOG

#define fc_log_debug(log, level, ...) do {          \
    if ((log)->log_level >= level)                  \
        _log(log, __FILE__, __LINE__, level,        \
             __VA_ARGS__);                          \
} while(0)

#else

#define fc_log_debug(log, level, ...)

#endif

#define fc_log_stderr(...)                          \
    _log_stderr(__VA_ARGS__)

#define fc_log(log, level, ...) do {                \
    if ((log)->log_level >= level)                  \
        _log(log, __FILE__, __LINE__, level,        \
             __VA_ARGS__);                          \
} while(0)

#define fc_log_warn(log, ...) do {                  \
    if ((log)->log_level >= FC_LOG_WARN)            \
        _log(log, __FILE__, __LINE__, FC_LOG_WARN,  \
             __VA_ARGS__);                          \
} while(0)
 
#define fc_log_error(log, ...) do {                 \
    if ((log)->log_level >= FC_LOG_ERROR)           \
        _log(log, __FILE__, __LINE__,FC_LOG_ERR,    \
             __VA_ARGS__);                          \
} while(0)

#define fc_abort(log, ...) do {                     \
    if ((log)->log_level >= FC_LOG_EMERG)           \
        _log(log, __FILE__, __LINE__, level,        \
             __VA_ARGS__);                          \
    fc_log_backtrace(log, FC_LOG_EMERG);            \
    abort();                                        \
} while(0)

#if defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS)
  void fc_log_backtrace(fc_log_t *log, int level);
#else
# define fc_log_backtrace(log, level)
#endif

fc_log_t *fc_log_create(int level, const char *filename);
void  fc_log_close(fc_log_t  *log);
void  fc_log_reopen(fc_log_t *log);
void _log(fc_log_t *log, const char *file, int line, int level,
          const char *fmt, ...) __attribute__((format(printf, 5, 6)));
void _log_stderr(const char *fmt, ...) __attribute((format(printf, 1, 2)));

#endif
