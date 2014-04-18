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

#ifndef FC_SIGNAL_H
#define FC_SIGNAL_H

#include <signal.h>

#define fc_signal_name(n)       "SIG" #n
#define fc_signal_value(n)      SIG##n

#define FC_SIGNAL_SHUTDOWN      QUIT
#define FC_SIGNAL_TERMINATE     TERM
#define FC_SIGNAL_RECONFIGURE   HUP
#define FC_SIGNAL_REOPEN        USR1

#endif
