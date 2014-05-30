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

#ifndef FC_ALLOC_H
#define FC_ALLOC_H

#include "fc_core.h"

void *fc_alloc (size_t size, fc_log_t *log);
void *fc_calloc(size_t size, fc_log_t *log);

#define fc_free free

#if defined(HAVE_POSIX_MEMALIGN) || defined(HAVE_MEMALIGN)
  void *fc_memalign(size_t alignment, size_t size, fc_log_t *log);
#else
# define fc_memalign(_alignment, _size, _log) fc_alloc(_size, _log)
#endif

#endif
