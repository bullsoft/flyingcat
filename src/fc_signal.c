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

static void fc_signal_handler(int signo);

fc_signal_t signals[] = {
    {
      fc_signal_value(FC_SIGNAL_SHUTDOWN),
      fc_signal_name(FC_SIGNAL_SHUTDOWN),
      "quit",
      fc_signal_handler
    },

    {
      fc_signal_value(FC_SIGNAL_TERMINATE),
      fc_signal_name(FC_SIGNAL_TERMINATE),
      "stop",
      fc_signal_handler
    },

    {
      fc_signal_value(FC_SIGNAL_RECONFIGURE),
      fc_signal_name(FC_SIGNAL_RECONFIGURE),
      "reload",
      fc_signal_handler
    },

    {
      fc_signal_value(FC_SIGNAL_REOPEN),
      fc_signal_name(FC_SIGNAL_REOPEN),
      "reopen",
      fc_signal_handler
    },

    { SIGTTIN, "SIGTTIN", "", fc_signal_handler },
    { SIGTTOU, "SIGTTOU", "", fc_signal_handler },
    { SIGINT,  "SIGINT",  "", fc_signal_handler },
    { SIGPIPE, "SIGPIPE", "", SIG_IGN },

    { 0, NULL, "", NULL }
};

int fc_signal_init(fc_log_t *log)
{
    fc_signal_t *sig;
    struct sigaction sa;

    for (sig = signals; sig->signo != 0; sig++) {
        memset(&sa, 0, sizeof(sa));

        sa.sa_handler = sig->handler;
        sigemptyset(&sa.sa_mask);

        fc_log_debug(log, FC_LOG_DEBUG, "sigaction(%s)", sig->signame);
        if (sigaction(sig->signo, &sa, NULL) < 0) {
            fc_log_error(log, "sigaction(%s) failed: %s",
                         sig->signame, strerror(errno));
            return FC_ERROR;
        }
    }

    return FC_OK;
}

void fc_signal_close(fc_log_t *log)
{
}

void fc_signal_handler(int signo)
{
    char *action;
    fc_signal_t *sig;

    for (sig = signals; sig->signo != 0; sig++) {
        if (sig->signo == signo) {
            break;
        }
    }

    switch(signo) {
    case fc_signal_value(FC_SIGNAL_SHUTDOWN):
        fc_quit = 1;
        action  = "shutting down";
        break;
    default:
        break;
    }

    fc_log(fc_context->log, FC_LOG_NOTICE, "signal %d(%s) received, %s",
           signo, sig->signame, action);

}

