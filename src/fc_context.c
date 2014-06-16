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

#include "fc_context.h"

fc_context_t *fc_context;

sig_atomic_t  fc_quit;
sig_atomic_t  fc_terminate;
sig_atomic_t  fc_reconfigure;
sig_atomic_t  fc_reopen;

fc_context_t *fc_context_create(struct flyingcat_s *fc)
{
    fc_pool_t *pool;
    fc_context_t *ctx;

    pool = fc_pool_create(1024, fc->log);
    if (pool == NULL) {
        fc_log_error(fc->log, "create pool for context failed");
        return NULL;
    }

    ctx = fc_palloc(pool, sizeof(fc_context_t));
    if (ctx == NULL) {
        fc_log_error(fc->log, "create context failed");
        fc_pool_close(pool);
        return NULL;
    }

    ctx->instance = fc;
    ctx->pool = pool;
    ctx->log  = fc->log;

    return ctx;
}

void fc_context_close(fc_context_t *ctx)
{
    fc_pool_close(ctx->pool);
}

