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

#include <string.h>
#include <stdlib.h>

#include "fc_log.h"

#if defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS)
# include <execinfo.h>
#endif

fc_log_t *fc_log_init(int level, const char *filename)
{
    return NULL;
}

void fc_log_close(fc_log_t *log)
{
}

void fc_log_reopen(fc_log_t *log)
{
}

void _log(fc_log_t *log, const char *file, int line, const char *fmt, ...)
{
}

void _log_stderr(const char *fmt, ...)
{
}

#if defined(HAVE_BACKTRACE) && defined(HAVE_BACKTRACE_SYMBOLS)
void fc_log_backtrace(fc_log_t *log){
    void  *bt_buffer[32];
    size_t bt_size, i;
    char **bt_strings;

    bt_size    = backtrace(bt_buffer, sizeof(bt_buffer) / sizeof(bt_buffer[0]));
    bt_strings = backtrace_symbols(bt_buffer, bt_size);

    for (i = 0; i < bt_size; i++) {
        fc_log(log, "%s", bt_strings[i]);
    }

    free(bt_strings);
}
#endif

