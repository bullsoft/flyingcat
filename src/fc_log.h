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

#define FC_LOG_EMERG     1
#define FC_LOG_ALERT     2
#define FC_LOG_CRIT      3
#define FC_LOG_ERR       4
#define FC_LOG_ERROR     FC_LOG_ERR
#define FC_LOG_WARN      5
#define FC_LOG_NOTICE    6
#define FC_LOG_INFO      7
#define FC_LOG_DEBUG     8
#define FC_LOG_VERB      9

#define FC_MAX_ERR_STR   1024

struct fc_log {
    int   log_level;
    char *file;
    int   fd;
};

extern fc_log log;

#ifdef FC_DEBUG_LOG

#define fc_log_debug(level, ...) do {              \
    if (log.log_level >= level)                    \
        _log(__FILE__, __LINE__, __VA_ARGS__);     \
} while(0)

#else

#define fc_log_debug(level, ...)

#endif

#define fc_log_stderr(...)                         \
    _log_stderr(__VA_ARGS__)

#define log(...) do {                              \
    _log(__FILE__, __LINE__, __VA_ARGS__);         \
} while(0)

#define fc_log_warn(...) do {                      \
    if (log.log_level >= FC_LOG_WARN)              \
        _log(__FILE__, __LINE, __VA_ARGS__);       \
} while(0)
 
#define fc_log_error(...) do {                     \
    if (log.log_level >= FC_LOG_ERROR)             \
        _log(__FILE__, __LINE, __VA_ARGS__);       \
} while(0)

int   log_init(int level, const char *filename);
void  log_close();
void  log_reopen();
void _log(const char *file, int line, const char *fmt, ...);
void _log_stderr();

#endif
