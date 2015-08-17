/*
 * Copyright (C) 2015 BullSoft
 * Copyright (C) 2015 Shang Yuanchun <idealities@gmail.com>
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

#include <ifaddrs.h>

static int is_addr_valid(int af, const char *addr)
{
    struct ifaddrs *addrs, *p;
    u_char binaddr[16];

    if (inet_pton(af, addr, (void *)binaddr) < 1) {
        return FC_ERROR;
    }

    if (getifaddrs(&addrs) <  0) {
        return FC_ERROR;
    }
    for (p = addrs; p != NULL; p = p->ifa_next) {
        switch(p->ifa_addr->sa_family) {
        case AF_INET:
            if (((struct sockaddr_in *)p->ifa_addr)->sin_addr.s_addr
                    == *(in_addr_t *)binaddr) {
                freeifaddrs(addrs);
                return FC_OK;
            }
            break;
        case AF_INET6:
            if (IN6_ARE_ADDR_EQUAL(&((struct sockaddr_in6 *)p->ifa_addr)->sin6_addr,
                        (struct in6_addr*)binaddr)) {
                freeifaddrs(addrs);
                return FC_OK;
            }
            break;
        default:
            continue;
        }
    }
    freeifaddrs(addrs);
    return FC_ERROR;
}
