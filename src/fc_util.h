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

#ifndef FC_UTIL_H
#define FC_UTIL_H

#ifdef MAXHOSTNAMELEN
# define FC_MAXHOSTNAMELEN  MAXHOSTNAMELEN
#else
# define FC_MAXHOSTNAMELEN  256
#endif

#ifndef __GNUC__
# define __attribute__(x)
# define __FUNCTION__ "<unknown>"
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define FC_INT32_LEN   (sizeof("-2147483648") - 1)
#define FC_INT64_LEN   (sizeof("-9223372036854775808") - 1)

#ifndef FC_ALIGNMENT
#define FC_ALIGNMENT    sizeof(unsigned long)
#endif

#define fc_align(d, a)    (((d) + (a - 1)) & ~(a - 1))
#define fc_align_ptr(p, a) \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

#endif
