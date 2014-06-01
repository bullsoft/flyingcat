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

#include "fc_pool.h"

fc_pool_t *fc_pool_create(size_t size, fc_log_t *log)
{
    fc_pool_t *p;

    p = fc_memalign(FC_POOL_ALIGNMENT, size + sizeof(fc_pool_t), log);
    if (p != NULL) {
        p->d.last  = (u_char *)p + sizeof(fc_pool_t);
        p->d.end   = p->d.last + size;
        p->max     = size;
        p->current = p;
        p->next    = NULL;
        p->large   = NULL;
        p->log     = log;
    }
    return p;
}

void *fc_palloc(fc_pool_t *pool, size_t size)
{
    if (size > pool->max)
        return fc_pmemalign(pool, size, FC_POOL_ALIGNMENT);

    return NULL;
}

void *fc_pcalloc(fc_pool_t *pool, size_t size)
{
    void *p = fc_palloc(pool, size);

    return (p != NULL ? memset(p, 0, size) : p);
}

void *fc_pmemalign(fc_pool_t *pool, size_t size, size_t alignment)
{
    return NULL;
}

void fc_pfree(fc_pool_t *pool, void *ptr)
{
    struct fc_large_data_s *l;

    for (l = pool->large; l != NULL; l = l->next) {
        if (l->data != ptr)
            continue;

        fc_log_debug(pool->log, FC_LOG_DEBUG,
                     "free: %p", l->data);
        fc_free(l->data);
        l->data = NULL;
        return;
    }
}

void fc_pool_close(fc_pool_t *pool)
{
    fc_pool_t *tmp;
    struct fc_large_data_s *l;

    for (l = pool->large; l != NULL; l = l->next) {
        if (l->data == NULL)
            continue;

        fc_log_debug(pool->log, FC_LOG_DEBUG,
                     "free: %p", l->data);
        fc_free(l->data);
    }

    for (; pool != NULL; pool = tmp) {
        tmp = pool->next;

        // because pool->log will not be allocated
        // in the pool.
        fc_log_debug(pool->log, FC_LOG_DEBUG,
                     "free pool: %p, unused: %zu", pool,
                     pool->d.end - pool->d.last);
        fc_free(pool);
    }
}

