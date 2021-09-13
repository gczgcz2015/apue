/*
 * Our own header, to be included before all standard system headers.
 */
#ifndef    _APUE_H
#define    _APUE_H

#define _POSIX_C_SOURCE 200809L

#if defined(SOLARIS)        /* Solaris 10 */
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 700
#endif

#include <sys/types.h>        /* some systems still require this */
#include <sys/stat.h>
#include <sys/termios.h>    /* for winsize */
#if defined(MACOS) || !defined(TIOCGWINSZ)
#include <sys/ioctl.h>
#endif

#include <stdio.h>        /* for convenience */
#include <stdlib.h>        /* for convenience */
#include <stddef.h>        /* for offsetof */
#include <string.h>        /* for convenience */
#include <unistd.h>        /* for convenience */
#include <signal.h>        /* for SIG_ERR */
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <syslog.h>


#define    MAXLINE    4096            /* max line length */

/*
 * Default file access permissions for new files.
 */
#define    FILE_MODE    (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*
 * Default permissions for new directories.
 */
#define    DIR_MODE    (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

typedef    void    Sigfunc(int);    /* for signal handlers */

#define    min(a,b)    ((a) < (b) ? (a) : (b))
#define    max(a,b)    ((a) > (b) ? (a) : (b))

/*
 * Prototypes for our own functions.
 */
char    *path_alloc(size_t *);                /* {Prog pathalloc} */
long     open_max(void);                    /* {Prog openmax} */

int         set_cloexec(int);                    /* {Prog setfd} */
void     clr_fl(int, int);
void     set_fl(int, int);                    /* {Prog setfl} */

void     pr_exit(int);                        /* {Prog prexit} */

void     pr_mask(const char *);                /* {Prog prmask} */
Sigfunc    *signal_intr(int, Sigfunc *);        /* {Prog signal_intr_function} */

void     daemonize(const char *);            /* {Prog daemoninit} */

void     sleep_us(unsigned int);            /* {Ex sleepus} */
ssize_t     readn(int, void *, size_t);        /* {Prog readn_writen} */
ssize_t     writen(int, const void *, size_t);    /* {Prog readn_writen} */

int         fd_pipe(int *);                    /* {Prog sock_fdpipe} */
int         recv_fd(int, ssize_t (*func)(int,
                 const void *, size_t));    /* {Prog recvfd_sockets} */
int         send_fd(int, int);                    /* {Prog sendfd_sockets} */
int         send_err(int, int,
                  const char *);            /* {Prog senderr} */
int         serv_listen(const char *);            /* {Prog servlisten_sockets} */
int         serv_accept(int, uid_t *);            /* {Prog servaccept_sockets} */
int         cli_conn(const char *);            /* {Prog cliconn_sockets} */
int         buf_args(char *, int (*func)(int,
                  char **));                /* {Prog bufargs} */

int         tty_cbreak(int);                    /* {Prog raw} */
int         tty_raw(int);                        /* {Prog raw} */
int         tty_reset(int);                    /* {Prog raw} */
void     tty_atexit(void);                    /* {Prog raw} */
struct termios    *tty_termios(void);            /* {Prog raw} */

int         ptym_open(char *, int);            /* {Prog ptyopen} */
int         ptys_open(char *);                    /* {Prog ptyopen} */
#ifdef    TIOCGWINSZ
pid_t     pty_fork(int *, char *, int, const struct termios *,
                  const struct winsize *);    /* {Prog ptyfork} */
#endif

int        lock_reg(int, int, int, off_t, int, off_t); /* {Prog lockreg} */

#define    read_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define    readw_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define    write_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define    writew_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define    un_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

pid_t    lock_test(int, int, off_t, int, off_t);        /* {Prog locktest} */

#define    is_read_lockable(fd, offset, whence, len) \
            (lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define    is_write_lockable(fd, offset, whence, len) \
            (lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)

void    err_msg(const char *, ...);            /* {App misc_source} */
void    err_dump(const char *, ...) __attribute__((noreturn));
void    err_quit(const char *, ...) __attribute__((noreturn));
void    err_cont(int, const char *, ...);
void    err_exit(int, const char *, ...) __attribute__((noreturn));
void    err_ret(const char *, ...);
void    err_sys(const char *, ...) __attribute__((noreturn));

void    log_msg(const char *, ...);            /* {App misc_source} */
void    log_open(const char *, int, int);
void    log_quit(const char *, ...) __attribute__((noreturn));
void    log_ret(const char *, ...);
void    log_sys(const char *, ...) __attribute__((noreturn));
void    log_exit(int, const char *, ...) __attribute__((noreturn));

void    TELL_WAIT(void);        /* parent/child from {Sec race_conditions} */
void    TELL_PARENT(pid_t);
void    TELL_CHILD(pid_t);
void    WAIT_PARENT(void);
void    WAIT_CHILD(void);


static void err_doit(int, int, const char *, va_list);  
  
/*  
 *  * Nonfatal error related to a system call.  
 *  * Print a message and return.  
 *  */  
void  
err_ret(const char *fmt, ...)  
{  
        va_list     ap;  
          
        va_start(ap, fmt);  
        err_doit(1, errno, fmt, ap);  
        va_end(ap);  
}  
  
/*  
 *  * Fatal error related to a system call.  
 *  * Print a message and terminate.  
 *  */  
void  
err_sys(const char *fmt, ...)  
{  
        va_list     ap;  
          
        va_start(ap, fmt);  
        err_doit(1, errno, fmt, ap);  
        va_end(ap);  
        exit(1);  
}  
  
/*  
 *  * Fatal error unrelated to a system call.  
 *  * Error code passed as explict parameter.  
 *  * Print a message and terminate.  
 *  */  
void  
err_exit(int error, const char *fmt, ...)  
{  
        va_list     ap;  
          
        va_start(ap, fmt);  
        err_doit(1, error, fmt, ap);  
        va_end(ap);  
        exit(1);  
}  
  
/*  
 *  * Fatal error related to a system call.  
 *  * Print a message, dump core, and terminate.  
 *  */  
void  
err_dump(const char *fmt, ...)  
{  
        va_list     ap;  
          
        va_start(ap, fmt);  
        err_doit(1, errno, fmt, ap);  
        va_end(ap);  
        abort();        /* dump core and terminate */  
        exit(1);        /* shouldn't get here */  
}  
  
/*  
 *  * Nonfatal error unrelated to a system call.  
 *  * Print a message and return.  
 *  */  
void  
err_msg(const char *fmt, ...)  
{  
        va_list     ap;  
          
        va_start(ap, fmt);  
        err_doit(0, 0, fmt, ap);  
        va_end(ap);  
}  
  
/*  
 *  * Fatal error unrelated to a system call.  
 *  * Print a message and terminate.  
 *  */  
void  
err_quit(const char *fmt, ...)  
{  
        va_list     ap;  
          
        va_start(ap, fmt);  
        err_doit(0, 0, fmt, ap);  
        va_end(ap);  
        exit(1);  
}  
  
/*  
 *  * Print a message and return to caller.  
 *  * Caller specifies "errnoflag".  
 *  */  
static void  
err_doit(int errnoflag, int error, const char *fmt, va_list ap)  
{  
        char    buf[MAXLINE];  
        vsnprintf(buf, MAXLINE, fmt, ap);  
        if (errnoflag)  
        snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s",strerror(error));  
        strcat(buf, " ");  
        fflush(stdout);     /* in case stdout and stderr are the same */  
        fputs(buf, stderr);  
        fflush(NULL);       /* flushes all stdio output streams */  
}



int
set_cloexec(int fd)
{
	int		val;

	if ((val = fcntl(fd, F_GETFD, 0)) < 0)
		return(-1);

	val |= FD_CLOEXEC;		/* enable close-on-exec */

	return(fcntl(fd, F_SETFD, val));
}


void
daemonize(const char *cmd)
{
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;

	/*
	 * Clear file creation mask.
	 */
	umask(0);

	/*
	 * Get maximum number of file descriptors.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		err_quit("%s: can't get file limit", cmd);

	/*
	 * Become a session leader to lose controlling TTY.
	 */
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if (pid != 0) /* parent */
		exit(0);
	setsid();

	/*
	 * Ensure future opens won't allocate controlling TTYs.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_quit("%s: can't ignore SIGHUP", cmd);
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if (pid != 0) /* parent */
		exit(0);

	/*
	 * Change the current working directory to the root so
	 * we won't prevent file systems from being unmounted.
	 */
	if (chdir("/") < 0)
		err_quit("%s: can't change directory to /", cmd);

	/*
	 * Close all open file descriptors.
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/*
	 * Attach file descriptors 0, 1, and 2 to /dev/null.
	 */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/*
	 * Initialize the log file.
	 */
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
		  fd0, fd1, fd2);
		exit(1);
	}
}


#endif    /* _APUE_H */