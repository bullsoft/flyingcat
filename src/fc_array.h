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

#ifndef FC_ARRAY_H
#define FC_ARRAY_H

#include "fc_core.h"

struct fc_array_s {
    void      *data;
    size_t     used;
    size_t     nalloc;
    size_t     size;
    fc_pool_t *pool;
};

typedef struct fc_array_s fc_array_t;

fc_array_t *fc_array_create(fc_pool_t *pool, size_t n, size_t size);
void *fc_array_push(fc_array_t *arr);
void *fc_array_pop(fc_array_t *arr);
void *fc_array_get(fc_array_t *arr, size_t idx);
void  fc_array_close(fc_array_t *arr);

static inline int
fc_array_init(fc_array_t *arr, fc_pool_t *pool, size_t n, size_t size)
{
    arr->used = 0;
    arr->size = size;
    arr->pool = pool;
    arr->nalloc = n;

    if ((arr->data = fc_palloc(pool, n * size)) == NULL) {
        return FC_ERROR;
    }

    return FC_OK;
}

#endif
