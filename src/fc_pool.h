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

#ifndef FC_POOL_H
#define FC_POOL_H

#include "fc_core.h"

struct fc_pool_data_s {
    u_char *last;
    u_char *end;
    fc_uint_t failed;
};

struct fc_pool_s {
    struct fc_pool_data_s d;
    size_t max;
    struct fc_pool_s *current;
    struct fc_pool_s *next;
    struct fc_large_data_s *large;
    fc_log_t *log;
};

struct fc_large_data_s {
    void *data;
    struct fc_large_data_s *next;
};

typedef struct fc_pool_s fc_pool_t;

fc_pool_t *fc_pool_create(size_t size, fc_log_t *log);
void *fc_palloc(fc_pool_t *pool, size_t size);
void *fc_pcalloc(fc_pool_t *pool, size_t size);
void *fc_pmemalign(fc_pool_t *pool, size_t size, size_t alignment);
void  fc_pfree(fc_pool_t *pool, void *ptr);
void  fc_pool_close(fc_pool_t *pool);

#endif
