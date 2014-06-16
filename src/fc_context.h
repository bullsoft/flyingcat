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

#ifndef FC_CONTEXT_H
#define FC_CONTEXT_H

#include "fc_core.h"

struct fc_context_s {
    void *conf;

    struct flyingcat_s *instance;
    fc_pool_t *pool;
    fc_log_t  *log;
};

typedef struct fc_context_s fc_context_t;

extern fc_context_t *fc_context;

extern sig_atomic_t    fc_quit;
extern sig_atomic_t    fc_terminate;
extern sig_atomic_t    fc_reconfigure;
extern sig_atomic_t    fc_reopen;

fc_context_t *fc_context_create(struct flyingcat_s *fc);
void fc_context_close(fc_context_t *ctx);

#endif
