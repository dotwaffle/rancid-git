/*
 * $Id: hpuifilter.c,v 1.61 2008/12/05 20:53:17 heas Exp $
 *
 * Copyright (c) 1997-2008 by Terrapin Communications, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to and maintained by
 * Terrapin Communications, Inc. by Henry Kilmer, John Heasley, Andrew Partan,
 * Pete Whiting, Austin Schutz, and Andrew Fort.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by Terrapin Communications,
 *        Inc. and its contributors for RANCID.
 * 4. Neither the name of Terrapin Communications, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 5. It is requested that non-binding fixes and modifications be contributed
 *    back to Terrapin Communications, Inc.
 *
 * THIS SOFTWARE IS PROVIDED BY Terrapin Communications, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COMPANY OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Modified openpty() from NetBSD:
 * Copyright (c) 1990, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include "config.h"
#include "version.h"

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_CTYPE_H
# include <ctype.h>
#endif
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <poll.h>
#if HAVE_PTY_H
# include <pty.h>
#endif
#include <regex.h>
#include <signal.h>
#if HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#if HAVE_STRINGS_H
# include <strings.h>
#endif
#if HAVE_PTMX && HAVE_STROPTS_H
# include <stropts.h>
#endif
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <termios.h>
#if HAVE_UTIL_H
# include <util.h>
#endif

#define	BUFSZ	(LINE_MAX * 2)
#define	ESC	0x1b

char		**environ,
		*progname;
int		debug,
		sigrx,
		timeo = 5;				/* default timeout   */
pid_t		child;

int		expectmore(char *buf, int len);
int		filter(char *, int);
RETSIGTYPE	reapchild(int);
#if !HAVE_OPENPTY
int		openpty(int *, int *, char *, struct termios *,
			struct winsize *);
#endif
RETSIGTYPE	sighdlr(int);
#if !HAVE_UNSETENV
int		unsetenv(const char *);
#endif
void		usage(void);
void		vers(void);

int
main(int argc, char **argv, char **ev)
{
    extern char		*optarg;
    extern int		optind;
    char		ch,
			hbuf[BUFSZ],		/* hlogin buffer */
			ptyname[FILENAME_MAX + 1],
			tbuf[BUFSZ],		/* telnet/ssh buffer */
			tbufstr[5] = {ESC, '\x07', '\r', '\n', '\0'};
    int			bytes,			/* bytes read/written */
			devnull,
			rval = EX_OK,
			ptym,			/* master pty */
			ptys;			/* slave pty */
    ssize_t		idx,			/* strcspan span */
			hlen = 0,		/* len of hbuf */
			tlen = 0;		/* len of tbuf */
    struct pollfd	pfds[3];
    struct termios	tios;

    environ = ev;

    /* get just the basename() of our exec() name and strip a .* off the end */
    if ((progname = strrchr(argv[0], '/')) != NULL)
	progname += 1;
    else
	progname = argv[0];
    if (strrchr(progname, '.') != NULL)
	*(strrchr(progname, '.')) = '\0';

    while ((ch = getopt(argc, argv, "dhvt:")) != -1 )
	switch (ch) {
	case 'd':
	    debug++;
	    break;
	case 't':
	    timeo = atoi(optarg);
	    if (timeo < 1)
		timeo = 1;
	    break;
	case 'v':
	    vers();
	    return(EX_OK);
	case 'h':
	default:
	    usage();
	    return(EX_USAGE);
	}

    if (argc - optind < 2) {
	usage();
	return(EX_USAGE);
    }

    unsetenv("DISPLAY");

    for (sigrx = 3; sigrx < 10; sigrx++)
	close(sigrx);

    /* allocate pty for telnet/ssh, then fork and exec */
    if (openpty(&ptym, &ptys, ptyname, NULL, NULL)) {
	fprintf(stderr, "%s: could not allocate pty: %s\n", progname,
		strerror(errno));
	return(EX_TEMPFAIL);
    }
    /* make the pty raw */
    if (tcgetattr(ptys, &tios)) {
	fprintf(stderr, "%s: tcgetattr() failed: %s\n", progname,
		strerror(errno));
	return(EX_OSERR);
    }
    tios.c_lflag &= ~ECHO;
    tios.c_lflag &= ~ICANON;
#ifdef VMIN
    tios.c_cc[VMIN] = 1;
    tios.c_cc[VTIME] = 0;
#endif
    if (tcsetattr(ptys, TCSANOW, &tios)) {
	fprintf(stderr, "%s: tcsetattr() failed: %s\n", progname,
		strerror(errno));
	return(EX_OSERR);
    }

    /*
     * if a tty, make it raw as the hp echos _everything_, including
     * passwords.
     */
    if (isatty(fileno(stdin))) {
	if (tcgetattr(fileno(stdin), &tios)) {
	    fprintf(stderr, "%s: tcgetattr() failed: %s\n", progname,
		strerror(errno));
	    return(EX_OSERR);
	}
	tios.c_lflag &= ~ECHO;
	tios.c_lflag &= ~ICANON;
#ifdef VMIN
	tios.c_cc[VMIN] = 1;
	tios.c_cc[VTIME] = 0;
#endif
	if (tcsetattr(fileno(stdin), TCSANOW, &tios)) {
	    fprintf(stderr, "%s: tcsetattr() failed: %s\n", progname,
		strerror(errno));
	    return(EX_OSERR);
	}
    }

    /* zero the buffers */
    memset(hbuf, 0, BUFSZ);
    memset(tbuf, 0, BUFSZ);

    /* reap our children, must be set-up *after* openpty() */
    signal(SIGCHLD, reapchild);

    if ((child = fork()) == -1) {
	fprintf(stderr, "%s: fork() failed: %s\n", progname, strerror(errno));
	return(EX_TEMPFAIL);
    }

    if (child == 0) {
	struct winsize ws;

	/*
	 * Make sure our terminal length and width are something greater
	 * than 1, for pagers on stupid boxes.
	 */
	ioctl(ptys, TIOCGWINSZ, &ws);
	ws.ws_row = 24;
	ws.ws_col = 132;
	ioctl(ptys, TIOCSWINSZ, &ws);

	signal(SIGCHLD, SIG_DFL);
	/* close the master pty & std* inherited from the parent */
	close(ptym);
	if (ptys != 0)
	    close(0);
	if (ptys != 1)
	    close(1);
	if (ptys != 2)
	    close(2);
#ifdef TIOCSCTTY
	setsid();
	if (ioctl(ptys, TIOCSCTTY, NULL) == -1) {
	    snprintf(ptyname, FILENAME_MAX, "%s: could not set controlling "
		     "tty: %s\n", progname, strerror(errno));
	    write(0, ptyname, strlen(ptyname));
	    return(EX_OSERR);
	}
#endif

	/* close stdin/out/err and attach them to the pipes */
	if (dup2(ptys, 0) == -1 || dup2(ptys, 1) == -1 || dup2(ptys, 2) == -1) {
	    snprintf(ptyname, FILENAME_MAX, "%s: dup2() failed: %s\n", progname,
		     strerror(errno));
	    write(0, ptyname, strlen(ptyname));
	    return(EX_OSERR);
	}
	if (ptys > 2)
	    close(ptys);

	/* exec telnet/ssh */
	execvp(argv[optind], argv + optind);
	snprintf(ptyname, FILENAME_MAX, "%s: execvp() failed: %s\n", progname,
		 strerror(errno));
	write(0, ptyname, strlen(ptyname));
	return(EX_TEMPFAIL);
	/*NOTREACHED*/
    }

    /* parent */
    if (debug)
	fprintf(stderr, "child %d\n", (int)child);

    signal(SIGHUP, sighdlr);

    /* close the slave pty */
    close(ptys);

    devnull = open("/dev/null", O_RDWR);

    /* make FDs non-blocking */
    if (fcntl(ptym, F_SETFL, O_NONBLOCK) ||
	fcntl(fileno(stdin), F_SETFL, O_NONBLOCK) ||
	fcntl(fileno(stdout), F_SETFL, O_NONBLOCK)) {
	fprintf(stderr, "%s: fcntl(NONBLOCK) failed: %s\n", progname,
		strerror(errno));
	exit(EX_OSERR);
    }

    /* loop to read on stdin and ptym */
#define	POLLEXP	(POLLERR | POLLHUP | POLLNVAL)
    pfds[0].fd = fileno(stdin);
    pfds[0].events = POLLIN | POLLEXP;
    pfds[1].fd = fileno(stdout);
    pfds[1].events = POLLEXP;
    pfds[2].fd = ptym;
    pfds[2].events = POLLIN | POLLEXP;

    /* shuffle data across the pipes until we see EOF or a read/write error */
    sigrx = 0;
    while (1) {
	bytes = poll(pfds, 3, (timeo * 1000));
	if (bytes == 0) {
	    if (sigrx)
		break;
	    /* timeout */
	    continue;
	}
	if (bytes == -1) {
	    switch (errno) {
	    case EAGAIN:
	    case EINTR:
		break;
	    default:
		rval = EX_IOERR;
		break;
	    }
	    continue;
	}

	/*
	 * write buffers first
	 * write hbuf (aka hlogin/stdin/pfds[0]) -> telnet (aka ptym/pfds[2])
	 */
	if ((pfds[2].revents & POLLOUT) && hlen) {
	    if ((bytes = write(pfds[2].fd, hbuf, hlen)) < 0 &&
		errno != EINTR && errno != EAGAIN) {
		fprintf(stderr, "%s: write() failed: %s\n", progname,
			strerror(errno));
		hlen = 0;
		hbuf[0] = '\0';
		break;
	    } else if (bytes > 0) {
		strcpy(hbuf, hbuf + bytes);
		hlen -= bytes;
		if (hlen < 1)
		     pfds[2].events &= ~POLLOUT;
	    }
	}
	if (pfds[2].revents & POLLEXP) {
	    hlen = 0;
	    hbuf[0] = '\0';
	    break;
	}

	/* write tbuf (aka telnet/ptym/pfds[2]) -> hlogin (stdout/pfds[1]) */
	if ((pfds[1].revents & POLLOUT) && tlen) {
	    /*
	     * if there is an escape char that didnt get filter()'d,
	     * we need to write only up to that point and wait for
	     * the bits that complete the escape sequence.  if at least
	     * two bytes follow it and it doesn't look like we should expect
	     * more data, write it anyway as filter() didnt match it.
	     */
	    bytes = tlen;
	    idx = strcspn(tbuf, tbufstr);
	    if (idx) {
		if (tbuf[idx] == ESC) {
		    if (tlen - idx < 2 || expectmore(&tbuf[idx], tlen - idx)) {
			bytes = idx;
		    }
		}
		if (tbuf[idx] == '\r' || tbuf[idx] == '\n') {
		    bytes = ++idx;
		    if (tbuf[idx] == '\r' || tbuf[idx] == '\n')
			bytes++;
		}
	    } else {
		if (tbuf[0] == ESC) {
		    if (tlen < 2 || expectmore(tbuf, tlen)) {
			bytes = 0;
		    }
		}
		if (tbuf[0] == '\r' || tbuf[0] == '\n') {
		    bytes = 1;
		    if (tbuf[1] == '\r' || tbuf[1] == '\n')
			bytes++;
		}
	    }

	    if ((bytes = write(pfds[1].fd, tbuf, bytes)) < 0 &&
		errno != EINTR && errno != EAGAIN) {
		fprintf(stderr, "%s: write() failed: %s\n", progname,
			strerror(errno));
		/* dont bother trying to flush tbuf */
		tlen = 0;
		tbuf[0] = '\0';
		break;
	    } else if (bytes > 0) {
		strcpy(tbuf, tbuf + bytes);
		tlen -= bytes;
		if (tlen < 1)
		    pfds[1].events &= ~POLLOUT;
	    }
	}
	if (pfds[1].revents & POLLEXP) {
	    /* dont bother trying to flush tbuf */
	    tlen = 0;
	    tbuf[0] = '\0';
	    break;
	}

	/* read hlogin (aka stdin/pfds[0]) -> hbuf */
	if (pfds[0].revents & POLLIN) {
	    if (BUFSZ - hlen > 1) {
		bytes = read(pfds[0].fd, hbuf + hlen, (BUFSZ - 1) - hlen);
		if (bytes > 0) {
		    hlen += bytes;
		    hbuf[hlen] = '\0';
		    pfds[2].events |= POLLOUT;
		} else if (bytes < 0 && errno != EAGAIN && errno != EINTR) {
		    /* read error */
		    break;
		}
	    }
	}
	if (pfds[0].revents & POLLEXP)
	    break;

	/* read telnet/ssh (aka ptym/pfds[2]) -> tbuf, then filter */
	if (pfds[2].revents & POLLIN) {
	    if (BUFSZ - tlen > 1) {
		bytes = read(pfds[2].fd, tbuf + tlen, (BUFSZ - 1) - tlen);
		if (bytes > 0) {
		    tlen += bytes;
		    tbuf[tlen] = '\0';
		    tlen = filter(tbuf, tlen);
		    if (tlen > 0)
			pfds[1].events |= POLLOUT;
		} else if (bytes < 0 && errno != EAGAIN && errno != EINTR) {
		    /* read error */
		    break;
		}
	    }
	}
	if (pfds[2].revents & POLLEXP)
	    break;
    }
    /* try to flush any remaining data from our buffers */
    if (hlen) {
	(void)write(pfds[2].fd, hbuf, hlen);
	hlen = 0;
    }
    if (tlen) {
	(void)write(pfds[1].fd, tbuf, tlen);
	tlen = 0;
    }
    if ((bytes = read(pfds[2].fd, tbuf, (BUFSZ - 1))) > 0) {
	tbuf[bytes] = '\0';
	tlen = filter(tbuf, bytes);
	(void)write(pfds[1].fd, tbuf, tlen);
    }
    tcdrain(pfds[1].fd);
    if ((hlen = read(pfds[0].fd, hbuf, (BUFSZ - 1))) > 0) {
	(void)write(pfds[2].fd, hbuf, hlen);
    }
    tcdrain(pfds[2].fd);

    if (child && ! kill(child, SIGINT))
	reapchild(SIGCHLD);

    return(rval);
}

/*
 * return non-zero if the escape sequence beginning with buf appears to be
 * incomplete (and the caller should wait for more data).
 */
int
expectmore(char *buf, int len)
{
    int	i;

    if (buf[1] == '[' || isdigit((int)buf[1])) {
	/* look for a char that ends the sequence */
	for (i = 2; i < len; i++) {
	    if (isalpha((int)buf[i]))
		return(0);
	}
	return(1);
    }
    if (buf[1] == '#') {
	/* look for terminating digit */
	for (i = 2; i < len; i++) {
	    if (isdigit((int)buf[i]))
		return(0);
	}
	return(1);
    }

    return(0);
}

/*
 * Remove/replace vt100/220 screen manipulation escape sequences so they do
 * not litter the output.
 */
int
filter(char *buf, int len)
{
    static regmatch_t	pmatch[1];
#define	N_REG		15		/* number of regexes in reg[][] */
#define	N_CRs		2		/* number of CR replacements */
    static regex_t	preg[N_REG];
    static char		reg[N_REG][50] = {	/* vt100/220 escape codes */
				"\x1B""7\x1B\\[1;24r\x1B""8",	/* ds */
				"\x1B""8",			/* fs */

				"\x1B\\[2J",
				"\x1B\\[2K",			/* kE */

				"\x1B\\[[0-9]+;[0-9]+r",	/* cs */
				"\x1B\\[[0-9]+;[0-9]+H",	/* cm */

				"\x1B\\[\\?6l",
				"\x1B\\[\\?7l",			/* RA */
				"\x1B\\[\\?25h",		/* ve */
				"\x1B\\[\\?25l",		/* vi */
				"\x1B\\[K",			/* ce */
				"\x1B\\[7m",			/* mr - ansi */
				"\x07",				/* bell */

				/* replace these with CR */
				"\x1B\\[0m",			/* me */
				"\x1B""E",
			};
    char		bufstr[3] = {ESC, '\x07', '\0'},
			ebuf[256];
    size_t		nmatch = 1;
    int			err,
			x;
    static int		init = 0;

    if (len == 0 || (err = strcspn(buf, bufstr)) >= len)
	return(len);

    for (x = 0; x < N_REG - N_CRs; x++) {
	if (! init) {
	    if ((err = regcomp(&preg[x], reg[x], REG_EXTENDED))) {
		regerror(err, &preg[x], ebuf, 256);
		fprintf(stderr, "%s: regex compile failed: %s\n", progname,
			ebuf);
		abort();
	    }
	}
	if ((err = regexec(&preg[x], buf, nmatch, pmatch, 0))) {
	    if (err != REG_NOMATCH) {
		regerror(err, &preg[x], ebuf, 256);
		fprintf(stderr, "%s: regexec failed: %s\n", progname, ebuf);
		abort();
	    }
	} else {
	    strcpy(buf + pmatch[0].rm_so, buf + pmatch[0].rm_eo);
	    /* start over with the first regex */
	    x = -1;
	}
    }

    /* now the CR NL replacements */
    if (! init++) {
	for (x = N_REG - N_CRs; x < N_REG; x++)
	    if ((err = regcomp(&preg[x], reg[x], REG_EXTENDED))) {
		regerror(err, &preg[x], ebuf, 256);
		fprintf(stderr, "%s: regex compile failed: %s\n", progname,
			ebuf);
		abort();
	    }
    }
    for (x = N_REG - N_CRs; x < N_REG; x++) {
	if ((err = regexec(&preg[x], buf, nmatch, pmatch, 0))) {
	    if (err != REG_NOMATCH) {
		regerror(err, &preg[x], ebuf, 256);
		fprintf(stderr, "%s: regexec failed: %s\n", progname, ebuf);
		abort();
	    }
	} else {
	    *(buf + pmatch[0].rm_so) = '\r';
	    *(buf + pmatch[0].rm_so + 1) = '\n';
	    strcpy(buf + pmatch[0].rm_so + 2, buf + pmatch[0].rm_eo);
	    /* start over with the first CR regex */
	    x = N_REG - 3;
	}
    }

    return(strlen(buf));
}

RETSIGTYPE
reapchild(int sig)
{
    int         status;
    pid_t       pid;

    if (debug)
	fprintf(stderr, "GOT SIGNAL %d\n", sig);

    while ((pid = wait3(&status, WNOHANG, NULL)) > 0) {
	if (debug)
            fprintf(stderr, "reap child %d\n", (int)pid);
	if (pid == child) {
	    child = 0;
	    sigrx = 1;
	    break;
	}
    }
    return;
}

RETSIGTYPE
sighdlr(int sig)
{
    if (debug)
	fprintf(stderr, "GOT SIGNAL %d\n", sig);
    sigrx = 1;
    return;
}

#if !HAVE_UNSETENV
int
unsetenv(const char *name)
{
   char	**victim,
	**end;
   int	len;
   if (environ == NULL)
	return(0);
   len = strlen(name);
   victim = environ;
   while (*victim != NULL) {
	if (strncmp(name, *victim, len) == 0 && victim[0][len] == '=')
	    break;
	victim++;
   }
   if (*victim == NULL)
	return(0);
   end = victim + 1;
   while (*end != NULL)
	end++;
   end--;
   *victim = *end;
   *end = NULL;
   return(0);
}
#endif

void
usage(void)
{
    fprintf(stderr, "usage: %s [-hv] [-t timeout] <telnet|ssh> [<ssh options>]"
	    " <hostname> [<telnet_port>]\n", progname);
    return;
}

void
vers(void)
{
    fprintf(stderr, "%s: %s version %s\n", progname, package, version);
    return;
}


#if !HAVE_OPENPTY
#include <grp.h>
#define TTY_LETTERS	"pqrstuvwxyzPQRST"
#define TTY_OLD_SUFFIX	"0123456789abcdef"
#define TTY_NEW_SUFFIX	"ghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

int
openpty(int *amaster, int *aslave, char *name, struct termios *term,
	struct winsize *winp)
{
    static char		line[] = "/dev/XtyXX";
    const char		*cp1, *cp2, *cp, *linep;
    int			master, slave;
    gid_t		ttygid;
    mode_t		mode;
    struct group	*gr;

#if HAVE_PTMX
    if ((master =
#if HAVE_PTMX_BSD
	 open("/dev/ptmx_bsd", O_RDWR))
#else
	 open("/dev/ptmx", O_RDWR))
#endif
				    != -1) {
	linep = ptsname(master);
	grantpt(master);
	unlockpt(master);
#ifndef TIOCSCTTY
	setsid();
#endif
	if ((slave = open(linep, O_RDWR)) < 0) {
	    slave = errno;
	    (void) close(master);
	    errno = slave;
	    return(-1);
	}
#if HAVE_PTMX_OSF
	{
	    char buf[10240];
	    if (ioctl (slave, I_LOOK, buf) != 0)
		if (ioctl (slave, I_PUSH, "ldterm")) {
		    close(slave);
		    close(master);
		    return(-1);
		}
	}
#elif HAVE_STROPTS_H
	ioctl(slave, I_PUSH, "ptem");
	ioctl(slave, I_PUSH, "ldterm");
	ioctl(slave, I_PUSH, "ttcompat");
#endif
	goto gotit;
    }
    if (errno != ENOENT)
	return(-1);
#endif

    if ((gr = getgrnam("tty")) != NULL) {
	ttygid = gr->gr_gid;
	mode = S_IRUSR|S_IWUSR|S_IWGRP;
    } else {
	ttygid = getgid();
	mode = S_IRUSR|S_IWUSR;
    }

    for (cp1 = TTY_LETTERS; *cp1; cp1++) {
	line[8] = *cp1;
	for (cp = cp2 = TTY_OLD_SUFFIX TTY_NEW_SUFFIX; *cp2; cp2++) {
	    line[5] = 'p';
	    line[9] = *cp2;
	    if ((master = open(line, O_RDWR, 0)) == -1) {
		if (errno != ENOENT)
			continue;	/* busy */
		if (cp2 - cp + 1 < sizeof(TTY_OLD_SUFFIX))
			return -1;	/* out of ptys */
		else	
			break;		/* out of ptys in this group */
	    }
	    line[5] = 't';
	    linep = line;
	    if (chown(line, getuid(), ttygid) == 0 &&
		chmod(line, mode) == 0 &&
		(slave = open(line, O_RDWR, 0)) != -1) {
gotit:
		*amaster = master;
		*aslave = slave;
		if (name)
		    (void)strcpy(name, linep);
		if (term)
		    (void)tcsetattr(slave, TCSAFLUSH, term);
		if (winp)
		    (void)ioctl(slave, TIOCSWINSZ, winp);
		return 0;
	    }
	    (void)close(master);
	}
    }
    errno = ENOENT;	/* out of ptys */
    return -1;
}
#endif
