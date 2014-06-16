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

#ifndef FC_LOG_PATH
# define FC_LOG_PATH     "logs/fc.log"
#endif

#ifndef FC_PID_PATH
# define FC_PID_PATH     "logs/fc.pid"
#endif

#ifndef FC_CONF_PATH
# define FC_CONF_PATH    "conf/fc.conf"
#endif

static int show_help;
static int show_version;
static int test_conf;
static int daemonize;

static struct option long_options[] = {
    { "help",           no_argument,        NULL,   'h' },
    { "version",        no_argument,        NULL,   'v' },
    { "test-conf",      no_argument,        NULL,   't' },
    { "not-daemonize",  no_argument,        NULL,   'D' },
    { "prefix",         required_argument,  NULL,   'p' },
    { "verbose",        required_argument,  NULL,   'l' },
    { "conf-file",      required_argument,  NULL,   'c' },
    { "log-file",       required_argument,  NULL,   'L' },
    { "pid-file",       required_argument,  NULL,   'P' },
    { NULL,             0,                  NULL,    0  }
};

static char short_options[] = "hvtDp:l:c:L:P:";

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
            show_version = 1;
            show_help    = 1;
            break;

        case 'v':
            show_version = 1;
            break;

        case 't':
            test_conf = 1;
            break;

        case 'D':
            daemonize = 0;
            break;

        case 'p':
            fc->prefix = optarg;
            if (access(fc->prefix, X_OK) < 0) {
                fc_log_stderr("access prefix directory (%s) failed: %s",
                              fc->prefix, strerror(errno));
                return FC_ERROR;
            }
            break;

        case 'l':
            value = atoi(optarg);
            if (value < 0) {
                fc_log_stderr(FLYINGCAT_NAME ": option -l requires a number");
                return FC_ERROR;
            }
            fc->log_level = value;
            break;

        case 'c':
            fc->conf_file = optarg;
            break;

        case 'L':
            fc->log_file  = optarg;
            break;

        case 'P':
            fc->pid_file  = optarg;
            break;

        case '?':
            switch (optopt) {
            case 'p':
                fc_log_stderr(FLYINGCAT_NAME ": option -%c requires a "
                              "directory name", optopt);
                break;

            case 'c':
            case 'L':
            case 'P':
                fc_log_stderr(FLYINGCAT_NAME ": option -%c requires a file name",
                           optopt);
                break;

            case 'l':
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
    fc_log_stderr(
        "Usage: " FLYINGCAT_NAME " [-hvtD] [-p prefix] [-l log_level] "
        "[-L log_file] [-P pid_file]" FC_LINEFEED
    );
    fc_log_stderr(
        "Options:" FC_LINEFEED
        "  -h, --help              : this help" FC_LINEFEED
        "  -v, --version           : show version and exit" FC_LINEFEED
        "  -t, --test-conf         : test configuration and exit" FC_LINEFEED
        "  -D, --not-daemonize     : do not daemonize" FC_LINEFEED
        "  -p, --prefix            : set prefix path (default: "
                                     FC_INSTALL_PREFIX ")" FC_LINEFEED
        "  -l, --verbose           : set log level (default: %d, min: %d, max: %d)"
                                     FC_LINEFEED
        "  -c, --conf-file         : set configuration file (default: "
                                     FC_CONF_PATH ")" FC_LINEFEED
        "  -L, --log-file          : set log file (default: "
                                     FC_LOG_PATH ")" FC_LINEFEED
        "  -P, --pid-file          : set pid file (default: "
                                     FC_PID_PATH ")"
        , FC_LOG_DEFAULT, FC_LOG_EMERG, FC_LOG_VERB
    );
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

    fd = open(fc->log_file, O_RDWR | O_APPEND);
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

static int fc_write_pidfile(struct flyingcat_s *fc)
{
    int fd, len;
    char pid[FC_INT64_LEN + 1];

    fd = open(fc->pid_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        fc_log_error(fc->log, "open pid file '%s' failed: %s",
                     fc->pid_file, strerror(errno));
        return FC_ERROR;
    }

    len = snprintf(pid, FC_INT64_LEN + 1, "%d", fc->pid);
    if (len < 0) {
        close(fd);
        return FC_ERROR;
    }

    pid[len++] = '\n';
    if (write(fd, pid, len) < 0) {
        fc_log_error(fc->log, "write pid file '%s' failed: %s",
                     fc->pid_file, strerror(errno));
        close(fd);
        return FC_ERROR;
    }

    close(fd);
    return FC_OK;
}

static int fc_remove_pidfile(struct flyingcat_s *fc)
{
    return (unlink(fc->pid_file) < 0) ? FC_ERROR : FC_OK;
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
    fc->pid_file = FC_PID_PATH;

    fc->log_file_alloc  = 0;
    fc->conf_file_alloc = 0;
    fc->pid_file_alloc  = 0;
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

#define FC_MEMBER_ALLOC(_member) _member ## _alloc

#define FC_STRCAT_IF_RELATIVE(_instance, _member, _path)   \
do {                                                       \
    if (_instance->_member[0] != FC_PATHSEP) {             \
        plen = strlen(fc->prefix);                         \
        name = malloc(plen + strlen(_path) + 2);           \
        if (name == NULL) {                                \
            return FC_ERROR;                               \
        }                                                  \
        p = memcpy(name, fc->prefix, plen);                \
        p += plen;                                         \
        if (*(p - 1) != FC_PATHSEP) {                      \
            *p++ = FC_PATHSEP;                             \
        }                                                  \
        memcpy(p, _path, strlen(_path) + 1);               \
        _instance->_member = name;                         \
        _instance->FC_MEMBER_ALLOC(_member) = 1;           \
    }                                                      \
} while(0)

static int fc_init_instance(struct flyingcat_s *fc)
{
    size_t plen;
    char *p, *name;

    FC_STRCAT_IF_RELATIVE(fc, log_file,  fc->log_file);
    FC_STRCAT_IF_RELATIVE(fc, conf_file, fc->conf_file);
    FC_STRCAT_IF_RELATIVE(fc, pid_file,  fc->pid_file);

    fc->log = fc_log_create(fc->log_level, fc->log_file);
    if (!fc->log) {
        return FC_ERROR;
    }

    fc_print_sysinfo(fc);

    if (daemonize && fc_daemonize(fc->log) != FC_OK) {
        return FC_ERROR;
    }

    if (daemonize && fc_redirect_io(fc) != FC_OK) {
        return FC_ERROR;
    }

    if (fc_signal_init(fc->log) != FC_OK) {
        return FC_ERROR;
    }

    fc->pid = getpid();
    if (fc_write_pidfile(fc) != FC_OK) {
        return FC_ERROR;
    }

    return FC_OK;
}

#define FC_FREE_IF_ALLOC(_instance, _member)    \
do {                                            \
    if (_instance->FC_MEMBER_ALLOC(_member)) {  \
        free(_instance->_member);               \
        _instance->_member = NULL;              \
    }                                           \
} while(0)

static void fc_post_run(struct flyingcat_s *fc)
{
    fc_signal_close(fc->log);

    if (fc_remove_pidfile(fc) != FC_OK && fc->log) {
        fc_log_error(fc->log, "remove pid file '%s' failed: %s",
                     fc->pid_file, strerror(errno));
    }

    if (fc->log) {
        fc_log(fc->log, FC_LOG_INFO, "exit");
        fc_log_close(fc->log);
    }

    FC_FREE_IF_ALLOC(fc, log_file);
    FC_FREE_IF_ALLOC(fc, conf_file);
    FC_FREE_IF_ALLOC(fc, pid_file);
}

static void fc_run(struct flyingcat_s *fc)
{
    fc_context_t *ctx;

    ctx = fc_context_create(fc);
    if (ctx == NULL) {
        return;
    }

    fc_context_close(ctx);
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

    if (show_version) {
        fc_log_stderr(FLYINGCAT_NAME ": " FLYINGCAT_VERSION);
        if (show_help) {
            fc_show_usage();
        }
        exit(0);
    }

    status = fc_init_instance(&fc);
    if (status != FC_OK) {
        fc_post_run(&fc);
        exit(1);
    }

    fc_run(&fc);

    fc_post_run(&fc);
    exit(1);
}
