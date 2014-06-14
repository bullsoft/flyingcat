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

#ifndef FC_CORE_H
#define FC_CORE_H

#include "config.h"

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <sys/stat.h>
#include <fcntl.h>

typedef intptr_t     fc_int_t;
typedef uintptr_t    fc_uint_t;
typedef intptr_t     fc_flag_t;

#include "fc_log.h"
#include "fc_util.h"
#include "fc_signal.h"
#include "fc_alloc.h"
#include "fc_pool.h"

struct flyingcat_s {
    int   log_level;
    char *prefix;
    char *log_file;
    char *conf_file;
    pid_t pid;
    char *pid_file;

    unsigned int log_file_alloc:1;
    unsigned int conf_file_alloc:1;
    unsigned int pid_file_alloc:1;

    char hostname[FC_MAXHOSTNAMELEN];
    fc_log_t *log;
};

#define FC_OK      0
#define FC_ERROR  -1
#define FC_AGAIN  -2
#define FC_NOMEM  -3
#define FC_ABORT  -4

#endif
