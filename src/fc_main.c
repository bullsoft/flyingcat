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

#include <getopt.h>
#include <sys/utsname.h>

#include "fc_core.h"

#define FC_LOG_DEFAULT   FC_LOG_INFO
#define FC_LOG_PATH      "logs/fc.log"
#define FC_PIDFILE_PATH  "logs/fc.pid"
#define FC_CONF_PATH     "conf/fc.conf"

static int show_help;
static int show_version;
static int test_conf;
static int daemonize;

static struct option long_options[] = {
    { "help",           no_argument,        NULL,   'h' },
    { "version",        no_argument,        NULL,   'V' },
    { "test-conf",      no_argument,        NULL,   't' },
    { "not-daemonize",  no_argument,        NULL,   'D' },
    { "prefix",         required_argument,  NULL,   'P' },
    { "verbose",        required_argument,  NULL,   'v' },
    { "conf-file",      required_argument,  NULL,   'c' },
    { "log-file",       required_argument,  NULL,   'l' },
    { "pid-file",       required_argument,  NULL,   'p' },
    { NULL,             0,                  NULL,    0  }
};

static char short_options[] = "hVtDP:v:c:l:p:";

static int fc_get_options(int argc, char *argv[], struct flyingcat_s *fc)
{
    int c, value;

    opterr = 0;

    // default it is a daemon
    daemonize = 1;

    for (;;) {
        c = getopt_long(argc, argv, short_options, long_options, NULL);
        if (c == -1) {
            break;
        }

        switch (c) {
        case 'h':
            show_help    = 1;
            break;

        case 'V':
            show_version = 1;
            break;

        case 't':
            test_conf = 1;
            break;

        case 'D':
            daemonize = 0;
            break;

        case 'P':
            fc->prefix = optarg;
            if (access(fc->prefix, X_OK) < 0) {
                fc_log_stderr("access prefix directory (%s) failed: %s",
                              fc->prefix, strerror(errno));
                return FC_ERROR;
            }
            break;

        case 'v':
            value = atoi(optarg);
            if (value < 0) {
                fc_log_stderr(FLYINGCAT_NAME ": option -v requires a number");
                return FC_ERROR;
            }
            fc->log_level = value;
            break;

        case 'c':
            fc->conf_file = optarg;
            break;

        case 'l':
            fc->log_file  = optarg;
            break;

        case 'p':
            fc->pid_file  = optarg;
            break;

        case '?':
            switch (optopt) {
            case 'P':
                fc_log_stderr(FLYINGCAT_NAME ": option -%c requires a "
                              "directory name", optopt);
                break;

            case 'c':
            case 'l':
            case 'p':
                fc_log_stderr(FLYINGCAT_NAME ": option -%c requires a file name",
                           optopt);
                break;

            case 'v':
                fc_log_stderr(FLYINGCAT_NAME ": option -%c requires a number",
                              optopt);
                break;

            default:
                fc_log_stderr(FLYINGCAT_NAME ": invalid option '-%c'", optopt);
                break;
            }
            return FC_ERROR;

        default:
            fc_log_stderr(FLYINGCAT_NAME ": invalid option '-%c'", optopt);
            return FC_ERROR;
        }
    }

    return FC_OK;
}

static void fc_show_usage()
{
}

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
        fc_log_error(fc->log, "open(\"%s\") failed: %s",
                     fc->log_file, strerror(errno));
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
        fc_log_error(fc->log, "close(\"%s\") failed: %s",
                     fc->log_file, strerror(errno));
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
    fc->prefix    = FC_INSTALL_PREFIX;
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

static void fc_print_sysinfo(struct flyingcat_s *fc)
{
    struct utsname uts;

    fc_log(fc->log, FC_LOG_INFO, "flyingcat version: " FLYINGCAT_VERSION);

    if (uname(&uts) < 0) {
        return;
    }
    fc_log(fc->log, FC_LOG_INFO, "OS: %s %s %s", uts.sysname,
           uts.release, uts.machine);
}

static int fc_init_instance(struct flyingcat_s *fc)
{
    fc->log = fc_log_create(fc->log_level, fc->log_file);
    if (!fc->log) {
        return FC_ERROR;
    }

    if (daemonize && fc_daemonize(fc->log) != FC_OK) {
        return FC_ERROR;
    }

    fc->pid = getpid();

    fc_print_sysinfo(fc);
    return FC_OK;
}

static void fc_post_run(struct flyingcat_s *fc)
{
    if (fc->log) {
        fc_log(fc->log, FC_LOG_INFO, "exit");
        fc_log_close(fc->log);
    }
}

int main(int argc, char *argv[])
{
    int status;
    struct flyingcat_s fc;

    fc_set_default_instance(&fc);

    if (fc_get_options(argc, argv, &fc) != FC_OK) {
        fc_show_usage();
        exit(1);
    }

    status = fc_init_instance(&fc);
    if (status != FC_OK) {
        fc_post_run(&fc);
        exit(1);
    }

    return 0;
}
