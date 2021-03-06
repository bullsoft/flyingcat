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

#include "fc_core.h"

static void *fc_palloc_large(fc_pool_t *pool, size_t size);
static void *fc_palloc_block(fc_pool_t *pool, size_t size);

fc_pool_t *fc_pool_create(size_t size, fc_log_t *log)
{
    fc_pool_t *p;

    p = fc_memalign(FC_POOL_ALIGNMENT, size + sizeof(fc_pool_t), log);
    if (p != NULL) {
        p->d.last  = (u_char *)p + sizeof(fc_pool_t);
        p->d.end   = p->d.last + size;
        p->d.fails = 0;
        p->d.next  = NULL;
        p->max     = size;
        p->current = p;
        p->large   = NULL;
        p->log     = log;
    }
    return p;
}

void *fc_palloc(fc_pool_t *pool, size_t size)
{
    u_char *m;
    fc_pool_t *p;

    if (size > pool->max)
        return fc_palloc_large(pool, size);

    p = pool->current;
    do {
        m = fc_align_ptr(p->d.last, FC_ALIGNMENT);

        if ((size_t)(p->d.end - m) >= size) {
            p->d.last = m + size;
            return m;
        }

        p = p->d.next;
    } while(p);

    return fc_palloc_block(pool, size);
}

static void *fc_palloc_large(fc_pool_t *pool, size_t size)
{
    void *p;
    int   n;
    u_char found = 0;
    struct fc_large_data_s *l = NULL;

    n = 0;
    for(l = pool->large; l; l = l->next) {
        if (l->data == NULL) {
            found = 1;
            break;
        }

        if (n++ >= 4) {
            break;
        }
    }
    if (!found) {
        l = fc_palloc(pool, sizeof(struct fc_large_data_s));
    }
    if (l == NULL) {
        return NULL;
    }

    // allocate for requested `size` memory
    p = fc_alloc(size, pool->log);
    if (p == NULL) {
        return NULL;
    }
    l->data = p;
    if (!found) {
        l->next     = pool->large;
        pool->large = l;
    }

    return p;
}

static void *fc_palloc_block(fc_pool_t *pool, size_t size)
{
    u_char *m;
    fc_pool_t *p, *q, *cur;

    p = fc_memalign(FC_POOL_ALIGNMENT,
                    pool->max + sizeof(struct fc_pool_data_s), pool->log);
    if (p == NULL) {
        return NULL;
    }

    p->d.last  = (u_char *)p + sizeof(struct fc_pool_data_s);
    p->d.end   = p->d.last + pool->max;
    p->d.fails = 0;
    p->d.next  = NULL;

    m = fc_align_ptr(pool->d.last, FC_ALIGNMENT);
    p->d.last  = m + size;

    cur = pool->current;
    for (q = cur; q->d.next != NULL; q = q->d.next) {
        if (q->d.fails ++ >= 4) {
            cur = q->d.next;
        }
    }

    q->d.next = p;
    pool->current = cur ? cur : p;

    return m;
}

void *fc_pcalloc(fc_pool_t *pool, size_t size)
{
    void *p = fc_palloc(pool, size);

    return (p != NULL ? memset(p, 0, size) : p);
}

void *fc_pmemalign(fc_pool_t *pool, size_t size, size_t alignment)
{
    void *p;
    struct fc_large_data_s *l;

    p = fc_memalign(alignment, size, pool->log);
    if (p == NULL) {
        return NULL;
    }

    l = fc_palloc(pool, sizeof(struct fc_large_data_s));
    if (l == NULL) {
        fc_free(p);
        return NULL;
    }
    l->data = p;
    l->next = pool->large;
    pool->large = l;

    return p;
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
        tmp = pool->d.next;

        // because pool->log will not be allocated
        // in the pool.
        fc_log_debug(pool->log, FC_LOG_DEBUG,
                     "free pool: %p, unused: %zu", pool,
                     pool->d.end - pool->d.last);
        fc_free(pool);
    }
}

