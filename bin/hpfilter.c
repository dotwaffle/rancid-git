/*
 * Copyright (C) 1997-2002 by Henry Kilmer, Erik Sherk and Pete Whiting.
 * All rights reserved.
 *
 * This software may be freely copied, modified and redistributed without
 * fee for non-commerical purposes provided that this copyright notice is
 * preserved intact on all copies and modified copies.
 *
 * There is no warranty or other guarantee of fitness of this software.
 * It is provided solely "as is". The author(s) disclaim(s) all
 * responsibility and liability with respect to this software's usage
 * or its effect upon hardware, computer systems, other software, or
 * anything else.
 *
 *
 * run telnet or ssh to connect to device specified on the command line.  the
 * point of hpfilter is to filter all the bloody vt100 (curses) escape codes
 * that the HP procurve switches belch and make hlogin a real bitch.
 */

#define DFLT_TO	60				/* default timeout */

#include <config.h>
#include <version.h>

#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <regex.h>

#include <termios.h>

char		*progname;
int		debug = 0;

int		filter __P((char *, int));
void		usage __P((void));
void		vers __P((void));
RETSIGTYPE	reapchild __P((void));

int
main(int argc, char **argv)
{
    extern char		*optarg;
    extern int		optind;
    char		ch,
			hbuf[LINE_MAX * 2],	/* hlogin buffer */
			*hbufp,
			tbuf[LINE_MAX * 2],	/* telnet buffer */
			*tbufp;
    int			bytes,			/* bytes read/written */
			child,
			r[2],			/* recv pipe */
			s[2];			/* send pipe */
    ssize_t		hlen = 0,		/* len of hbuf */
			tlen = 0;		/* len of tbuf */
    struct timeval	to = { DFLT_TO, 0 };
    fd_set		rfds,			/* select() */
			wfds;
    struct termios	tios;

    /* get just the basename() of our exec() name and strip a .* off the end */
    if ((progname = strrchr(argv[0], '/')) != NULL)
	progname += 1;
    else
	progname = argv[0];
    if (strrchr(progname, '.') != NULL)
	*(strrchr(progname, '.')) = '\0';

    while ((ch = getopt(argc, argv, "dhv")) != -1 )
	switch (ch) {
	case 'd':
	    debug++;
	    break;
	case 'v':
	    vers();
	    return(EX_OK);
	case 'h':
	default:
	    usage();
	    return(EX_USAGE);
	}

    if (argc - optind != 2) {
	usage();
	return(EX_USAGE);
    }

    /* reap our children */
    signal(SIGCHLD, (void *) reapchild);
    signal(SIGHUP, (void *) reapchild);
    signal(SIGINT, (void *) reapchild);
    signal(SIGKILL, (void *) reapchild);
    signal(SIGTERM, (void *) reapchild);

    /* create 2 pipes for send/recv and then fork and exec telnet */
    for (child = 3; child < 10; child++)
	close(child);
    if (pipe(s) || pipe(r)) {
	fprintf(stderr, "%s: pipe() failed: %s\n", progname,
		strerror(errno));
	return(EX_TEMPFAIL);
    }

    /* if a tty, make it raw as the hp echos _everything_, including
     * passwords.
     */
    if (isatty(0)) {
	if (tcgetattr(0, &tios)) {
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
	if (tcsetattr(0, TCSANOW, &tios)) {
	    fprintf(stderr, "%s: tcsetattr() failed: %s\n", progname,
		strerror(errno));
	    return(EX_OSERR);
	}
    }

    if ((child = fork()) == -1) {
	fprintf(stderr, "%s: fork() failed: %s\n", progname,
		strerror(errno));
	return(EX_TEMPFAIL);
    }

    /* zero the buffers */
    bzero(hbuf, LINE_MAX * 2);
    bzero(tbuf, LINE_MAX * 2);

    if (child == 0) {
	/* close the parent's side of the pipes; we write r[1], read s[0] */
	close(s[1]);
	close(r[0]);
	/* close stdin/out/err and attach them to the pipes */
	if (dup2(s[0], 0) == -1 || dup2(r[1], 1) == -1 || dup2(r[1], 2) == -1) {
	    fprintf(stderr, "%s: dup2() failed: %s\n", progname,
		strerror(errno));
	    return(EX_OSERR);
	}
	close(s[0]);
	close(r[1]);
	/* exec telnet */
	if (execvp(argv[optind], argv + optind)) {
	    fprintf(stderr, "%s: execlp() failed: %s\n", progname,
		strerror(errno));
	    return(EX_TEMPFAIL);
	}
	/* not reached */
    } else {
	/* parent */
	if (debug)
	    fprintf(stderr, "child %d\n", child);

	/* close the child's side of the pipes; we write s[1], read r[0] */
	close(s[0]);
	close(r[1]);

	/* make FDs non-blocking */
	if (fcntl(s[1], F_SETFL, O_NONBLOCK) ||
		fcntl(r[0], F_SETFL, O_NONBLOCK) ||
		fcntl(0, F_SETFL, O_NONBLOCK) ||
		fcntl(1, F_SETFL, O_NONBLOCK)) {
	    fprintf(stderr, "%s: fcntl(NONBLOCK) failed: %s\n", progname,
		strerror(errno));
	    exit(EX_OSERR);
	}

	/* loop to read on stdin and r[0] */
	FD_ZERO(&rfds); FD_ZERO(&wfds);
	hbufp = hbuf; tbufp = tbuf;

	while (1) {
	    FD_SET(0, &rfds); FD_SET(r[0], &rfds);
	    /* if we have stuff in our buffer(s), we select on writes too */
	    FD_ZERO(&wfds);
	    if (hlen) {
		 FD_SET(s[1], &wfds);
	    }
	    if (tlen) {
		 FD_SET(1, &wfds);
	    }

	    switch (select(r[1], &rfds, &wfds, NULL, &to)) {
	    case 0:
		/* timeout */
			/* HEAS: what do i do here? */
		break;
	    case -1:
		switch (errno) {
		case EINTR:		/* interrupted syscall */
		    break;
		default:
		    exit(EX_IOERR);
		}
		break;
	    default:
		/* check exceptions first */

		/* which FD is ready?  write our buffers asap. */
		/* write hbuf (stdin) -> s[1] */
		if (FD_ISSET(s[1], &wfds) && hlen) {
		    if ((hlen = write(s[1], hbuf, hlen)) < 0) {
			fprintf(stderr, "%s: write() failed: %s\n", progname,
				strerror(errno));
			close(s[1]);
		    } else
			strcpy(hbuf, hbuf + hlen);

		    hlen = strlen(hbuf);
		}
		/* write tbuf -> stdout */
		if (FD_ISSET(1, &wfds) && tlen) {
		    /* if there is an escape char that didnt get filter()'d,
		     * we need to only write up to that point and wait for
		     * the bits that complete the escape sequence
		     */
		    if ((tbufp = index(tbuf, 0x1b)) != NULL)
			tlen = tbufp - tbuf;

		    if ((tlen = write(1, tbuf, tlen)) < 0) {
			fprintf(stderr, "%s: write() failed: %s\n", progname,
				strerror(errno));
			close(1);
		    } else
			strcpy(tbuf, tbuf + tlen);

		    tlen = strlen(tbuf);
		}
		if (FD_ISSET(0, &rfds)) {
		    /* read stdin into hbuf */
		    if (LINE_MAX * 2 - hlen > 1) {
			hlen += read(0, hbuf + hlen,
				(LINE_MAX * 2 - 1) - hlen);
			if (hlen > 0) {
			    hbuf[hlen] = '\0';
			} else if (hlen == 0 || errno != EAGAIN)
			    /* EOF or read error */
			    close(0);

			hlen = strlen(hbuf);
		    }
		} else if (FD_ISSET(r[0], &rfds)) {
		    /* read telnet into tbuf, then filter */
		    if (LINE_MAX * 2 - tlen > 1) {
			tlen += read(r[0], tbuf + tlen,
				(LINE_MAX * 2 - 1) - tlen);
			if (tlen > 0) {
			    tbuf[tlen] = '\0';
			    tlen = filter(tbuf, tlen);
			} else if (tlen == 0 || errno != EAGAIN)
			    /* EOF or read error */
			    close(r[0]);

			tlen = strlen(tbuf);
		    }
		}

		break;
	    }
	}
	/* close */
	close(0);
	close(1);
	close(s[1]);
	close(r[0]);

    }

    if (! kill(child, SIGQUIT))
	reapchild();

    return(EX_OK);
}

int
filter(buf, len)
    char	*buf;
    int		len;
{
    static regmatch_t	pmatch[1];
#define	N_REG		11		/* number of regexes in reg[][] */
    static regex_t	preg[N_REG];
    static char		reg[N_REG][50] = {	/* vt100/220 escape codes */
				"\e7\e\\[1;24r\e8",		/* ds */
				"\e8",				/* fs */
	
				"\e\\[2J",
				"\e\\[2K",			/* kE */

				"\e\\[[0-9]+;[0-9]+r",		/* cs */
				"\e\\[[0-9]+;[0-9]+H",		/* cm */

				"\e\\[\\?6l",
				"\e\\[\\?7l",			/* RA */
				"\e\\[\\?25h",			/* ve */
				"\e\\[\\?25l",			/* vi */

				"\eE",			/* replace w/ CR */
			};
    char		ebuf[256];
    size_t		nmatch = 1;
    int			err,
			x;
    static int		init = 0;

    if (index(buf, 0x1b) == 0 || len == 0)
	return(len);

    for (x = 0; x < N_REG - 1; x++) {
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
	    x = 0;
	}
    }

    /* replace \eE w/ CR NL */
    if (! init++) {
	if ((err = regcomp(&preg[N_REG - 1], reg[N_REG - 1], REG_EXTENDED))) {
	    regerror(err, &preg[N_REG - 1], ebuf, 256);
	    fprintf(stderr, "%s: regex compile failed: %s\n", progname,
		ebuf);
	    abort();
	}
    }
    while (1)
	if ((err = regexec(&preg[N_REG - 1], buf, nmatch, pmatch, 0))) {
	    if (err != REG_NOMATCH) {
		regerror(err, &preg[N_REG - 1], ebuf, 256);
		fprintf(stderr, "%s: regexec failed: %s\n", progname, ebuf);
		abort();
	    } else
		break;
	} else {
	    *(buf + pmatch[0].rm_so) = '\n';
	    strcpy(buf + pmatch[0].rm_so + 1, buf + pmatch[0].rm_eo);
	    x = 0;
	}

    return(strlen(buf));
}

RETSIGTYPE 
reapchild(void)
{
    int         status;
    pid_t       pid;
    
    /* HEAS: this needs to deal with/without wait3 via HAVE_WAIT3 */
    while ((pid = wait3(&status, WNOHANG, 0)) > 0)
	if (debug)
            fprintf(stderr, "reap child %d\n", pid);
    
    /*exit(1);*/
return;

    /* not reached */
}   

void
usage(void)
{
    fprintf(stderr,
"usage: %s [-hv] <telnet|ssh> <hostname>
", progname);
    return;
}

void
vers(void)
{
    fprintf(stderr,
"%s: %s version %s
", progname, package, version);
    return;
}
