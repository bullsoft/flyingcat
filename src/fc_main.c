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

#define FC_LOG_DEFAULT   FC_LOG_INFO
#define FC_LOG_PATH      "logs/fc.log"
#define FC_PIDFILE_PATH  "logs/fc.pid"
#define FC_CONF_PATH     "conf/fc.conf"

static int fc_daemonize(fc_log_t *log)
{
    pid_t pid;

    pid = fork();
    switch(pid) {
    case  0:
        break;
    case -1:
        fc_log_error(log, "fork() failed: %s", strerror(errno));
        return FC_ERROR;
    default:
        // in parent
        _exit(0);
    }

    if (setsid() < 0) {
        fc_log_error(log, "setsid() failed: %s", strerror(errno));
        return FC_ERROR;
    }

    // fork again
    pid = fork();
    switch(pid) {
    case  0:
        break;
    case -1:
        fc_log_error(log, "fork() again failed: %s", strerror(errno));
        return FC_ERROR;
    default:
        _exit(0);
    }

    return FC_OK;
}

static int fc_redirect_io(struct flyingcat_s *fc)
{
    int   fd;
    char *ptr;

    umask(0);

    fd = open(fc->log_file, O_RDWR);
    if (fd < 0) {
        fc_log_error(fc->log, "open(\"%s\") failed: %s", fc->log_file, strerror(errno));
        return FC_ERROR;
    }

    if (dup2(fd, STDIN_FILENO) < 0) {
        ptr = "STDIN";
        goto DUP2FAILED;
    }

    if (dup2(fd, STDOUT_FILENO) < 0) {
        ptr = "STDOUT";
        goto DUP2FAILED;
    }

    if (dup2(fd, STDERR_FILENO) < 0) {
        ptr = "STDERR";
        goto DUP2FAILED;
    }

    if (close(fd) == -1) {
        fc_log_error(fc->log, "close(\"%s\") failed: %s", fc->log_file, strerror(errno));
        return FC_ERROR;
    }
    return FC_OK;

DUP2FAILED:
    fc_log_error(fc->log, "dup2(%d, \"%s\") failed: %s", fd, ptr, strerror(errno));
    close(fd);
    return FC_ERROR;
}

static void fc_set_default_instance(struct flyingcat_s *fc)
{
    fc->log_level = FC_LOG_DEFAULT;
    fc->log_file  = FC_LOG_PATH;

    fc->conf_file = FC_CONF_PATH;

    if (gethostname(fc->hostname, FC_MAXHOSTNAMELEN)) {
        fc_log_warn(fc->log, "gethostname() failed: %s", strerror(errno));
        strncpy(fc->hostname, "unknown", sizeof("unknown")); // no -1 here
    }
    fc->hostname[FC_MAXHOSTNAMELEN - 1] = '\0';

    fc->pid      = (pid_t)-1;
    fc->pid_file = FC_PIDFILE_PATH;
}

static int fc_init_instance(struct flyingcat_s *fc)
{
}

int main(int argc, char *argv[])
{
    struct flyingcat_s fc;

    fc_set_default_instance(&fc);

    return 0;
}
