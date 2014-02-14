/*
 * $Id: par.c 4910 2014-01-16 20:12:45Z heas $
 *
 * Copyright (C) 2002-2008 by Henry Kilmer, Andrew Partan, John Heasley
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
 * perl version of par is Copyright (C) 1997-2002 by Henry Kilmer, Erik
 * Sherk and Pete Whiting.
 *
 * PAR - parallel processing of command
 *
 * par runs a command N times in parallel.  It will accept a list of arguments
 * for command-line replacement in the command.  If the list entry begins
 * with a ":" the remainder of the line is the command to run ("{}" will be
 * replaced with each subsequent item in the list).  If the list entry begins
 * with a "#", the entry is ignored.  If a command is defined (either with
 * the -c or with a : line) any entry thereafter will be applied to the
 * command by replacing the {} brackets.  If no cammand is defined, then each
 * line is assumed to be a command to be run.
 *
 * differences from perl version:
 * - when par rx's a hup/int/term/quit signal, it does not print out the cmds
 *   that will not be run.
 * - when par rx's a hup/int/term/quit signal, it does not exit immediately
 *   after sending kill to running jobs.  it waits for them to exit so that
 *   they are cleaned-up properly.  if a second signal is rx'd, it dies
 *   immediately.
 */
#include "config.h"
#if HAVE_STDARG_H
# include <stdarg.h>
#endif
#if HAVE_STDINT_H
# include <stdint.h>
#endif
#if HAVE_STDLIB_H
# include <stdlib.h>
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#ifdef HAVE_STRING_H
# include <string.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#include <sys/time.h>
#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#include <sysexits.h>
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif
#include <termios.h>
#include <time.h>

#include "version.h"

extern char	**environ;

typedef struct {
	int	n;				/* proc n of n_opt processes */
	pid_t	pid;				/* child pid */
	char	*logfname;			/* logfile name */
	FILE	*logfile;
	int	logfd;				/* logfile FD */
	pid_t	xpid;				/* xterm child pid */
} child;

char		*progname;
child		*progeny;
int		debug = 0,
		chld_wait = 0,			/* there are children */
		signaled = 0;			/* kill signal rx'd */
int		devnull;			/* /dev/null */

/* args */
int		e_opt = 0,
		f_opt = 0,
		i_opt = 0,
		n_opt = 3,
		p_opt = 0,
		q_opt = 0,
		x_opt = 0,
		ifile = 0;			/* argv index to input files */
char		*c_opt = NULL,
		*l_opt = "par.log";

FILE		*errfp,				/* stderr fp */
		*logfile;			/* logfile fp */
sigset_t	set_chld;			/* SIGCHLD {un}blocking */

void		arg_free(char ***);
int		arg_mash(char **, char **);
int		arg_replace(char **, char **, char **, char ***);
int		dispatch_cmd(char **, char **);
int		execcmd(child *, char **);
int		line_split(const char *, char ***);
int		read_input(char *, FILE **, int *, char ***, char ***);
void		reopenfds(child *);
int		run_cmd(child *, char **, char **);
int		shcmd(child *, char **);
void		usage(void);
void		vers(void);
int		xtermcmd(child *, char **);
void		xtermlog(child *);

RETSIGTYPE	handler(int);
RETSIGTYPE	reapchild(int);

int
main(int argc, char **argv, char **envp)
{
    extern char		*optarg;
    extern int		optind;
    char		ch;
    time_t		t;
    int			i,
			line;
    char		**cmd = NULL,		/* cmd (c_opt or from input) */
			**args = NULL;		/* cmd argv */
    FILE		*F;			/* input file */

    /* get just the basename() of our exec() name and strip .* off the end */
    if ((progname = strrchr(argv[0], '/')) != NULL)
	progname += 1;
    else
	progname = argv[0];
    if (strrchr(progname, '.') != NULL)
	*(strrchr(progname, '.')) = '\0';

    /* make sure stderr points to something */
    if ((devnull = open("/dev/null", O_RDWR, 0666)) == -1) {
	printf("Error: can not open /dev/null: %s\n", strerror(errno));
	exit(EX_OSERR);
    }
    if (stderr == NULL) {
	if ((errfp = fdopen(devnull, "w+")) == NULL) {
	    printf("Error: can not open /dev/null: %s\n", strerror(errno));
	    exit(EX_OSERR);
	}
    } else
	errfp = stderr;

    while ((ch = getopt(argc, argv, "defhiqxvc:e:l:n:p:")) != -1 )
	switch (ch) {
	case 'c':	/* command to run */
	    c_opt = optarg;
	    break;
	case 'd':
	    debug++;
	    break;
	case 'e':	/* exec args split by spaces rather than using sh -c */
	    e_opt = 1;
	    if (i_opt) {
		fprintf(errfp, "Warning: -e non-sensical with -i\n");
	    }
	    break;
	case 'f':	/* no file or stdin, just run quantity of command */
	    f_opt = 1;
	    break;
	case 'l':	/* logfile */
	    if (q_opt) {
		fprintf(errfp, "Warning: -q non-sensical with -x or -l\n");
		q_opt = 0;
	    }
	    l_opt = optarg;
	    break;
	case 'i':	/* run commands through interactive xterms */
	    i_opt = 1;
	    if (e_opt) {
		fprintf(errfp, "Warning: -i non-sensical with -e\n");
	    }
	    break;
	case 'n':	/* number of processes to run to run at once, dflt 3 */
	    n_opt = atoi(optarg);
	    if (n_opt < 1) {
		fprintf(errfp, "Warning: -n < 1 is non-sensical\n");
		n_opt = 3;
	    }
	    break;
	case 'p':	/* pause # seconds between forks, dflt 0 */
	    p_opt = atoi(optarg);
	    break;
	case 'q':	/* quiet mode (dont log anything to logfiles */
	    if (x_opt) {
		fprintf(errfp, "Warning: -q non-sensical with -x or -l\n");
		x_opt = 0;
	    }
	    q_opt = 1;
	    break;
	case 'x':	/* view par logs as they run through an xterm */
	    if (q_opt) {
		fprintf(errfp, "Warning: -q non-sensical with -x or -l\n");
		q_opt = 0;
	    }
	    x_opt = 1;
	    break;
	case 'v':
	    vers();
	    return(EX_OK);
	case 'h':
	default:
	    usage();
	    return(EX_USAGE);
	}

    /* -f requires -c */
    if (f_opt && ! c_opt) {
	fprintf(errfp, "usage: -f requires -c option\n");
	usage();
	return(EX_USAGE);
    } else if (f_opt && (argc - optind) != 0) {
	/* extraneous command-line arguments with -f */
	fprintf(errfp, "usage: extraneous command-line arguments with -f "
		"option\n");
	usage();
	return(EX_USAGE);
    } else if (argc - optind == 0) {
	/* if no -f and no cmd-line input files, read from stdin */
	ifile = -1;
    } else if (c_opt == NULL && (argc - optind) == 0) {
	/* args after options are input file(s) */
	fprintf(errfp, "usage: either -c or a command file must be supplied\n");
	usage();
	return(EX_USAGE);
    } else
	ifile = optind;

    /* grab space to keep track of our children */
    if ((progeny = (child *)malloc(sizeof(child) * n_opt)) == NULL) {
	fprintf(errfp, "Error: memory allocation failed: %s\n",
		strerror(errno));
	exit(EX_TEMPFAIL);
    } else
	memset((void *)progeny, 0, sizeof(child) * n_opt);

    /* set-up child structures */
    t = time(NULL);
    if (! q_opt) {
	for (i = 0; i < n_opt; i++) {
	    /* open the logfile or use stderr */
	    if (asprintf(&progeny[i].logfname,
			 "%s.%lu.%d", l_opt, (unsigned long)t, i) < 1) {
		fprintf(errfp, "Error: could not allocate space for process "
			"%d's log filename\n", i);
		exit(EX_TEMPFAIL);
	    }
	    if ((progeny[i].logfd = open(progeny[i].logfname,
					 O_WRONLY | O_CREAT, 0666)) == -1) {
		fprintf(errfp, "Error: could not open %s for writing: %s\n",
			progeny[i].logfname, strerror(errno));
		exit(EX_CANTCREAT);
	    }
	    if ((progeny[i].logfile = fdopen(progeny[i].logfd, "w")) == NULL) {
		fprintf(errfp, "Error: could not open %s for writing: %s\n",
			progeny[i].logfname, strerror(errno));
		exit(EX_CANTCREAT);
	    }

	    /* start an xterm for log file */
	    if (x_opt)
		xtermlog(&progeny[i]);
	}
    }

    /* prepare to accept signals */
    signal(SIGHUP, handler);
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGQUIT, handler);
    signal(SIGCHLD, reapchild);
    sigemptyset(&set_chld);
    sigaddset(&set_chld, SIGCHLD);

    /* if argv[ifile] is NULL and stdin is closed, then f_opt is implicit */
    if (argv[ifile] == NULL && stdin == NULL)
	f_opt = 1;

    /*
     * command dispatch starts here.
     *
     * -f means just run the -c command -n times.  any args become "".
     */
    if (f_opt) {
	if ((i = line_split(c_opt, &cmd))) {
	    fprintf(errfp, "Error: failed to build command");
	    if (i == ENOMEM)
		fprintf(errfp, ": %s\n", strerror(i));
	    else
		fprintf(errfp, "\n");
	} else {
	    for (i = 0; !signaled && i < n_opt; i++) {
		progeny[i].n = i + 1;
			/* XXX: check retcode from *cmd()? */
		run_cmd(&progeny[i], cmd, args);
	    }
	    if (args != NULL)
		arg_free(&args);
	}
    } else if (ifile > 0) {
	/*
	 * input files were specified on the command-line, read each as input
	 */
	F = NULL; cmd = NULL; args = NULL;
	for ( ; !signaled && ifile < argc; ifile++) {
	    while (!signaled &&
		   (i = read_input(argv[ifile], &F, &line, &cmd, &args)) == 0) {
		while (dispatch_cmd(cmd, args) == EAGAIN)
		    pause();
		if (args != NULL)
		    arg_free(&args);
	    }
	    /* XXX: if (i == EOF) */
	    if (cmd != NULL)
		arg_free(&cmd);
	    if (args != NULL)
		arg_free(&args);
	}
    } else {
	/* read stdin as input */
	F = stdin;
	line = 1; cmd = NULL;  args = NULL;
	while (!signaled &&
		(i = read_input("(stdin)", &F, &line, &cmd, &args)) == 0) {
	    while (dispatch_cmd(cmd, args) == EAGAIN)
		pause();
	    if (*args != NULL)
		arg_free(&args);
	}
	if (cmd != NULL)
	    arg_free(&cmd);
	if (args != NULL)
	    arg_free(&args);
    }

    /*
     * after all the work is assigned we wait for all the children to
     * finish and be reaped.  ie: so all the pid's in the child structure
     * will be zero.
     */
    if (debug)
	fprintf(errfp, "All work assigned.  Waiting for remaining processes."
		"\n");
    while (1) {
	/* block sigchld while we search the child table */
	sigprocmask(SIG_BLOCK, &set_chld, NULL);

	for (i = 0; i < n_opt; i++) {
	    if (progeny[i].pid != 0) {
		chld_wait = 1;
		break;
            }
	}

	sigprocmask(SIG_UNBLOCK, &set_chld, NULL);
	if (chld_wait == 1) {
	    pause();
	} else if (chld_wait == 0)
	    break;
	chld_wait = 0;
    }

    /* close the log files */
    for (i = 0; i < n_opt; i++) {
	if (progeny[i].logfile != NULL)
	    fclose(progeny[i].logfile);
    }

    if (debug)
	fprintf(errfp, "Complete\n");

    return(EX_OK);
}

/*
 * free space in and of a char[][] from line_split()/arg_replace()
 */
void
arg_free(char ***args)
{
    int		i;

    if (args == NULL || *args == NULL)
	return;

    for (i = 0; (*args)[i] != NULL; i++)
	free((*args)[i]);

    free(*args);
    *args = NULL;

    return;
}

/*
 * arg to sh -c used in shcmd() must be 1 argument.  returns 0 else errno.
 *
 * NOTE: dst[][] is assumed to have 2 spaces of which [0][] will be used.
 *	 dst[1] is, of course, the NULL terminator.  The caller will have to
 *	 free dst[0].
 */
int
arg_mash(char **dst, char **src)
{
    int		i = 0;
    size_t	len = 0;
    char	*ptr;

    while (src[i] != NULL)
	len += strlen(src[i++]);
    len += i + 1;

    if ((dst[0] = (char *)malloc(sizeof(char *) * len)) == NULL)
	return(errno);
    memset(dst[0], 0, len);

    i = 0; ptr = dst[0];
    while (src[i] != NULL) {
	len = strlen(src[i]);
	memcpy(ptr, src[i++], len);
	ptr += len;
	if (src[i] != NULL)
	    *ptr++ = ' ';
    }

    return(0);
}

/*
 * takes a NULL terminated command (cmd[][]) like {"echo", "{}"} whose args
 * are stored in NULL terminated args[][] and sequentially replaces {}s
 * from args[][].  any {}s not matching up to an arg are replaced with "".
 *
 * args found in tail[][] are concatenated to the end new[][] without any
 * interpretation.  ie: if arg_replace() were already called but something
 * needs to be prepended or appended.
 *
 * it returns an argv[][], in new***, which begins with the command followed
 * by the args and is null terminated so that is suitable for execvp(), and
 * 0 or errno to indicate success or an error.
 *
 * NOTE: if arg_replace() succeeds (returns 0), it is the callers
 *	  responsibility to free the space with arg_free().
 */
int
arg_replace(char **cmd, char **args, char **tail, char ***new)
{
    int			argn = 0,		/* which arg[] is next */
			nargs = 0,		/* # of entries in args[][] */
			ncmds = 0,		/* # of entries in cmd[][] */
			ntail = 0,		/* # of entries in tail[][] */
			atick = 0,		/* single quoted string toggle
						 * in args[][]
						 */
			aquotes = 0,		/* double quoted string toggle
						 * in args[][]
						 */
			quotes = 0;		/* " quoted string toggle */
    size_t		len = 0;		/* length of cmd - leading sp */
    char		*tick = NULL,		/* ' quoted string */
			*ptr;
    register int	a, b, c, n;
    char		buf[LINE_MAX * 2];	/* temporary space */

    /* if new is null, that is an internal error */
    if (new == NULL || (cmd == NULL && args == NULL))
	abort();

    if (cmd != NULL)
	while (cmd[ncmds] != NULL)
	    ncmds++;
    if (args != NULL)
	while (args[nargs] != NULL)
	    nargs++;
    if (tail != NULL)
	while (tail[ntail] != NULL)
	    ntail++;

    /* create space for ncmds + ntail + NULL terminator */
    if ((*new = (char **)malloc(sizeof(char *) * (ncmds + ntail + 1))) == NULL)
	return(ENOMEM);
    memset(*new, 0, sizeof(char *) * (ncmds + ntail + 1));

    /*
     * now look through each cmd[][] for {}s and replace them, taking care to
     * follow shell quoting/escape semantics
     */
    memset(buf, 0, LINE_MAX * 2);
    for (n = 0; n < ncmds; n++) {
	c = b = 0;
	ptr = cmd[n];
	while (cmd[n][c] != '\0') {
	    /* check buf len to be sure space remains */
	    if ((LINE_MAX * 2) - b < 2)
		return(ENOMEM);

	    switch (ptr[c]) {
	    case '\\':
		if (quotes) {
		    if (ptr[c + 1] == 'n') {
			buf[b++] = '\n';
			c += 2;
		    } else {
			buf[b++] = ptr[c++];
			buf[b++] = ptr[c++];
		    }
		} else {
		    if (ptr[c + 1] == 'n') {
			buf[b++] = '\n';
			c += 2;
		    } else if (ptr[c + 1] == 'r') {
			buf[b++] = '\r';
			c += 2;
		    } else if (ptr[c + 1] == 't') {
			buf[b++] = '\t';
			c += 2;
		    } else {
			buf[b++] = ptr[++c];
			c += 2;
		    }
		}
		break;
	    case '\'':
		/*
		 * shell preserves the meaning of all chars between single
		 * quotes, including backslashes.  so, it is not possible to
		 * put a single quote inside a single quoted string in shell.
		 * just copy everything up to the next '.
		 */
		c++;
		if ((tick = index(ptr + c, '\'')) == NULL) {
		    /* unmatched quotes */
		    return(EX_DATAERR);
		}
		len = tick - (ptr + c);
		if ((b + len + 1) > (LINE_MAX * 2))
		    return(ENOMEM);
		memcpy(&buf[b], &ptr[c], len);
		c += len + 1; b += len;
		break;
	    case '"':
		/*
		 * the shell would recognize <dollar-sign>, <back-tick>, and
		 * <back-slash> in double-quoted strings.  we will do
		 * like-wise, though <back-tick> and <dollar-sign> are
		 * meaningless to us, and include our {}s.
		 */
		quotes ^= 1;
		if (e_opt)
		    c++;
		else
		    buf[b++] = ptr[c++];
		break;
	    case '{':
		/* insert arg[n], if the next char is '}' */
		if (ptr[c + 1] == '}') {
		    if (argn < nargs) {
			len = strlen(args[argn]);
			/* perform shell quoting on the arg */
			a = 0;
			while (args[argn][a] != '\0') {
			    if (b >= (LINE_MAX * 2 - 1)) {
				fprintf(errfp, "Error: buffer space exhausted"
					"\n");
				return(ENOMEM);
			    }
			    switch (args[argn][a]) {
			    case '\'':
				if (!atick)
				    atick ^= 1;
				a++;
				break;
			    case '\\':
				if (atick) {
				    buf[b++] = args[argn][a++];
				} else if (args[argn][a + 1] == 'n') {
				    buf[b++] = '\n';
				    a += 2;
				} else if (args[argn][a + 1] == 'r') {
				    buf[b++] = '\r';
				    a += 2;
				} else if (args[argn][a + 1] == 't') {
				    buf[b++] = '\t';
				    a += 2;
				} else {
				    buf[b++] = args[argn][++a];
				    a++;
				}
				break;
			    case '"':
				if (!atick) {
				    aquotes ^= 1;
				    a++;
				    break;
				}
			    default:
				buf[b++] = args[argn][a++];
			    }
			}
			argn++;
		    }

		    c += 2;
		    break;
		}

		/* fall-through, if not "{}" */
	    default:
		buf[b++] = ptr[c++];
	    }
	}
	buf[b] = '\0';

	if (quotes)
	    return(EX_DATAERR);

	/* copy the full arg */
	if (asprintf(&(*new)[n], "%s", buf) == -1)
	    return(errno);
    }

    /* tack on tail[][], if any exist */
    if (ntail) {
	for (b = 0; b < ntail; b++) {
	    if (asprintf(&(*new)[n], "%s", tail[b]) == -1)
		return(errno);
	    n++;
	}
    }

    return(0);
}

/*
 * find a child/process slot (one of n_opt) to run the command and use
 * run_cmd() to start it.
 *
 * returns EAGAIN if there were no available child slots else the return
 * value from run_cmd.  0 means success in dispatching the command.
 */
int
dispatch_cmd(char **cmd, char **args)
{
    int		i;
    static int	cmd_num = 1;

    /* block sigchld while we search the child table */
    sigprocmask(SIG_BLOCK, &set_chld, NULL);

    for (i = 0; i < n_opt; i++) {
	if (progeny[i].pid != 0)
	    continue;

	progeny[i].n = cmd_num++;

	sigprocmask(SIG_UNBLOCK, &set_chld, NULL);
	return(run_cmd(&progeny[i], cmd, args));
    }

    sigprocmask(SIG_UNBLOCK, &set_chld, NULL);

    return(EAGAIN);
}

/*
 * start a command via an exec, after breaking up the arguments
 */
int
execcmd(child *c, char **cmd)
{
    char	*mashed[] = { NULL, NULL };
    int		status;
    time_t	t;

    /* XXX: is this right? */
    if (cmd == NULL)
	return(ENOEXEC);

    /* build cmd string for logs */
    if ((status = arg_mash(mashed, cmd))) {
	/* XXX: is this err msg always proper? will only ret true or ENOMEM? */
	fprintf(errfp, "Error: memory allocation failed: %s\n",
		strerror(errno));
	return(status);
    }

    /* block sigchld so we quickly reap it ourselves */
    sigprocmask(SIG_BLOCK, &set_chld, NULL);

    if (c->logfile) {
	char	*ct;
	t = time(NULL);
		/* XXX: build a complete cmd line */
	ct = ctime(&t); ct[strlen(ct) - 1] = '\0';
	fprintf(c->logfile, "!!!!!!!\n!%s: %s\n!!!!!!!\n", ct, mashed[0]);
	fflush(c->logfile);
    }

    if ((c->pid = fork()) == 0) {
	/* child */
	signal(SIGCHLD, SIG_DFL);
        sigprocmask(SIG_UNBLOCK, &set_chld, NULL);
	if (debug > 1) {
	    fprintf(errfp, "fork(sh -c %s ...) pid %d\n", mashed[0],
		    (uint32_t)getpid());
	}
	/* reassign stdout and stderr to the log file, stdin to /dev/null */
	reopenfds(c);

	execvp(cmd[0], cmd);

	/* not reached, unless exec() fails */
	fprintf(errfp, "Error: exec(%s) failed: %s\n", cmd[0], strerror(errno));
		/* XXX: should we return errno instead? */
	exit(EX_UNAVAILABLE);
    } else {
	if (debug)
	    fprintf(errfp, "\nStarting %d/%d %s: process id=%d\n",
		    c->n, n_opt, mashed[0], (uint32_t)c->pid);
	if (c->pid == -1) {
	    fprintf(errfp, "Error: fork() failed: %s\n", strerror(errno));
		/* XXX: wait on c->pid when its -1?  arg is valid, but
			if the fork failed will have a procstruct to return
			status? */
	    waitpid(c->pid, &status, WNOHANG);
	    c->pid = 0;
	}
    }

    if (mashed[0] != NULL)
	free(mashed[0]);

    sigprocmask(SIG_UNBLOCK, &set_chld, NULL);

    return(0);
}

/*
 * split a line into an arg[][] with shell single/double-quoting semantics.
 * quotes are retained and should be handled by arg_replace();
 */
int
line_split(const char *line, char ***args)
{
    int		argn = 0;			/* current arg */
    size_t	b,
		c,
		llen,				/* length of line */
		nargs = 0;
    int		quotes = 0,			/* track double quotes */
		tick;				/* ptr to single quote */

    if (args == NULL)
	abort();

    /* if line is NULL, just create arg[0][NULL] */
    if (line == NULL) {
        if ((*args = (char **)malloc(sizeof(char **))) == NULL)
	    return(errno);
	memset(*args, 0, sizeof(char **));
    } else {
	/* skip leading/trailing whitespace in line */
        while (*line == ' ' || *line == '\t')
	    line++;
	llen = strlen(line);
        while ((line[llen] == ' ' || line[llen] == '\t') && llen > 0)
	    llen--;

	/*
	 * just count spaces for args[][] malloc, this will waste memory when
	 * spaces are in quoted args, but do not expect it to be a big deal.
	 */
	c = 0;
	while (c < llen) {
	    if (line[c] == ' ' || line[c] == '\t')
		nargs++;
	    c++;
	}
	/* gratuitous last arg and args[][NULL] */
	nargs += 2;

	if ((*args = (char **)malloc(sizeof(char **) * nargs)) == NULL)
	    return(errno);
	memset(*args, 0, sizeof(char **) * nargs);

		/* XXX: do we need checks here for exceeding llen? */
	/*
	 * copy the args into place.
	 * preserve and observe shell style quoting as we go.
	 */
	for (tick = b = c = 0; 1; ) {
	    switch(line[c]) {
	    case '\'':
		tick ^= 1;
		c++;
		break;
	    case '\\':
		if (! tick) {
		    c++;
		    if (line[c] == '\0') {
			fprintf(errfp, "Error: premature end of input\n");
				/* XXX: not the right return code */
			return(ENOMEM);
		    }
		}
		c++;
		break;
	    case '\"':
		if (! tick)
		    quotes ^= 1;
		c++;
		break;
	    case '\0':
		if (tick || quotes) {
		    fprintf(errfp, "Error: unmatched quotes\n");
			/* XXX: not the right return code */
		    return(ENOMEM);
		}
	    case '\t':
	    case ' ':
		if (((*args)[argn] =
		     malloc(sizeof(char) * (c - b + 1))) == NULL)
		    return(ENOMEM);

		memcpy((*args)[argn], &line[b], (c - b));
		(*args)[argn][c - b] = '\0';
		argn++;

		if (line[c] == '\0')
		    goto END;

		/* skip adjacent spaces */
		while (line[c] == ' ' || line[c] == '\t')
		    c++;
		b = c;
		break;
	    default:
		c++;
	    }
	}
    }
END:

    return(0);
}

/*
 * if F is NULL, we open fname.
 *
 * read first line as a command and subsequent lines as either commands or
 * args depending upon c_opt and the first line.  basically;
 * - if the first char of the first line is ':', then what follows it is the
 *   command.
 * - if what follows is null, then the command is c_opt (-c) and subsequent
 *   lines are args
 * - if -c was not specified, then each subsequent line is a command without
 *   arg substitution
 * - if the ':' of the first line was ommitted, then we proceed as if it had
 *   been but the command was empty
 *
 * return 0 on success, EOF at end of file, else errno with an errmsg in cmd.
 * note: any space allocated for cmd or args must be free'd by the caller
 *	 with free() or arg_free().
 */
int
read_input(char *fname, FILE **F, int *line, char ***cmd, char ***args)
{
    int		e;
    char	buf[LINE_MAX + 1];

    if (cmd == NULL || args == NULL)
	abort();

    if (*F == NULL) {
	if (fname == NULL)
	    abort();

	if ((*F = fopen(fname, "r")) == NULL) {
	    e = errno;
	    fprintf(errfp, "Error: open(%s) failed: %s\n", fname, strerror(e));
	    return(e);
	}
	*cmd = NULL;  *args = NULL;
	*line = 1;
    }

    /* first line might be a command */
    if (*line == 1) {
	switch ((buf[0] = fgetc(*F))) {
	case EOF:
	    goto ERR;
	    break;
	case ':':
	    if (fgets(buf, LINE_MAX + 1, *F) == NULL)
		goto ERR;
	    (*line)++;
	    e = strlen(buf);
	    if (buf[e - 1] == '\n') {
		buf[e - 1] = '\0';
	    } /* else
		XXX: finish this, we didnt get the whole line */
	    if ((e = line_split(buf, cmd))) {
			/* XXX: is strerror(e) right? */
		fprintf(errfp, "Error: %s\n", strerror(e));
		fclose(*F); *F = NULL;
		return(e);
	    }
	    break;
	default:
	    ungetc(buf[0], *F);
	    if (*cmd == NULL && c_opt != NULL)
		if ((e = line_split(c_opt, cmd))) {
			/* XXX: is strerror(e) right? */
		    fprintf(errfp, "Error: %s\n", strerror(e));
		    fclose(*F); *F = NULL;
		    return(e);
		}
	}
    }

    /* make sure fname isnt NULL for error printfs */
    if (fname == NULL)
	fname = "(unknown)";

NEXT:
    /* next line */
    if (fgets(buf, LINE_MAX + 1, *F) == NULL)
	goto ERR;
    (*line)++;
    if (buf[0] == '#')
	goto NEXT;

    /*
     * if there isnt a \n on the end, either we lacked buffer space or
     * the last line of the file lacks a \n
     */
    e = strlen(buf);
    if (buf[e - 1] == '\n') {
	buf[e - 1] = '\0';
    } /* else
	XXX: finish this */

    /* split the line into an args[][] */
    if ((e = line_split(buf, args))) {
		/* XXX: is strerror(e) right? */
	fprintf(errfp, "Error: parsing args: %s\n", strerror(e));
	return(e);
    }

    return(0);

ERR:
    /* handle FILE error */
    if (feof(*F)) {
	e = EOF;
    } else {
	e = errno;
	fprintf(errfp, "Error: read(%s) failed: %s\n", fname, strerror(e));
    }
    fclose(*F); *F = NULL;
    return(e);
}

/*
 * reassign stdout and stderr to the log file and stdin to /dev/null.
 * called after fork and before exec() of the child processes.
 */
void
reopenfds(child *c)
{
    int		fd = STDERR_FILENO + 1;

    /* stderr */
    if (! q_opt) {
	if (c->logfd != STDERR_FILENO) {
	    if (dup2(c->logfd, STDERR_FILENO) == -1)
		abort();
	    close(c->logfd);
	    c->logfd = STDERR_FILENO;
	}
	/* stdout */
	if (dup2(c->logfd, STDOUT_FILENO) == -1)
	    abort();
    }
    /* stdin */
    if (dup2(devnull, STDIN_FILENO) == -1)
	abort();

    /* close anything above stderr */
    while (fd < 10)
	close(fd++);

    return;
}

/*
 * start a command.
 */
int
run_cmd(child *c, char **cmd, char **args)
{
    char	**newcmd = NULL;
    int		e;

    if (c == NULL)
	return(0);

    if (cmd == NULL && args == NULL)
	return(ENOEXEC);

    /* if cmd is NULL, swap it with args */
    if (cmd == NULL) {
	cmd = args;
	args = NULL;
    }

    /* preform arg subsitution */
    if ((e = arg_replace(cmd, args, NULL, &newcmd)) == 0) {
	if (i_opt) {
	    e = xtermcmd(c, newcmd);
	} else if (e_opt) {
	    e = execcmd(c, newcmd);
	} else
	    e = shcmd(c, newcmd);

	arg_free(&newcmd);
    }

    if (p_opt) {
	sigprocmask(SIG_BLOCK, &set_chld, NULL);
	sleep(p_opt);
	sigprocmask(SIG_UNBLOCK, &set_chld, NULL);
    }

    return(e);
}

/*
 * start a command whose output is concatenated to the childs logfile via
 * sh -c.
 */
int
shcmd(child *c, char **cmd)
{
    char	*sh[] = { "sh", "-c", NULL },
		*mashed[] = { NULL, NULL },
		**new;
    int		status;
    time_t	t;

    /* XXX: is this right? */
    if (cmd == NULL)
	return(ENOEXEC);

    /* build new command */
    if ((status = arg_mash(mashed, cmd))) {
	/* XXX: is this err msg always proper? will only ret true or ENOMEM? */
	fprintf(errfp, "Error: memory allocation failed: %s\n",
		strerror(errno));
	return(status);
    }
    if ((status = arg_replace(sh, NULL, mashed, &new))) {
	/* XXX: is this err msg always proper? will only ret true or ENOMEM? */
	fprintf(errfp, "Error: memory allocation failed: %s\n",
		strerror(errno));
	return(status);
    }

    /* block sigchld so we quickly reap it ourselves */
    sigprocmask(SIG_BLOCK, &set_chld, NULL);

    if (c->logfile) {
	char	*ct;
	t = time(NULL);
	ct = ctime(&t); ct[strlen(ct) - 1] = '\0';
	fprintf(c->logfile, "!!!!!!!\n!%s: %s\n!!!!!!!\n", ct, mashed[0]);
	fflush(c->logfile);
    }

    if ((c->pid = fork()) == 0) {
	/* child */
	signal(SIGCHLD, SIG_DFL);
        sigprocmask(SIG_UNBLOCK, &set_chld, NULL);
	if (debug > 1) {
	    fprintf(errfp, "fork(sh -c %s ...) pid %d\n", mashed[0],
		    (uint32_t)getpid());
	}
	/* reassign stdout and stderr to the log file, stdin to /dev/null */
	reopenfds(c);

	execvp(new[0], new);

	/* not reached, unless exec() fails */
	fprintf(errfp, "Error: exec(%s) failed: %s\n", new[0], strerror(errno));
	exit(EX_UNAVAILABLE);
    } else {
	if (debug)
	    fprintf(errfp, "\nStarting %d/%d %s: process id=%d\n",
		    c->n, n_opt, mashed[0], (uint32_t)c->pid);
	if (c->pid == -1) {
	    fprintf(errfp, "Error: fork() failed: %s\n", strerror(errno));
	    waitpid(c->pid, &status, WNOHANG);
	    c->pid = 0;
	}
    }

    if (mashed[0] != NULL)
	free(mashed[0]);

    sigprocmask(SIG_UNBLOCK, &set_chld, NULL);

    return(0);
}

/*
 * start a command in an interactive xterm.
 */
int
xtermcmd(child *c, char **cmd)
{
    char	*xterm[] = { "xterm", "-e", NULL },
		*mashed[] = { NULL, NULL },
		**new;
    int		status;
    time_t	t;

    /* XXX: is this right? */
    if (cmd == NULL)
	return(ENOEXEC);

    /* build new command */
    if ((status = arg_mash(mashed, cmd))) {
	/* XXX: is this err msg always proper? will only ret true or ENOMEM? */
	fprintf(errfp, "Error: memory allocation failed: %s\n",
		strerror(errno));
	return(status);
    }
    if ((status = arg_replace(xterm, NULL, cmd, &new))) {
	/* XXX: is this err msg always proper? will only ret true or ENOMEM? */
	fprintf(errfp, "Error: memory allocation failed: %s\n",
		strerror(errno));
	return(status);
    }

    /* block sigchld so we quickly reap it ourselves */
    sigprocmask(SIG_BLOCK, &set_chld, NULL);

    if (c->logfile) {
	char	*ct;
	t = time(NULL);
		/* XXX: build a complete cmd line */
	ct = ctime(&t); ct[strlen(ct) - 1] = '\0';
	fprintf(c->logfile, "!!!!!!!\n!%s: %s\n!!!!!!!\n", ct, mashed[0]);
	fflush(c->logfile);
    }

    if ((c->pid = fork()) == 0) {
	/* child */
	signal(SIGCHLD, SIG_DFL);
        sigprocmask(SIG_UNBLOCK, &set_chld, NULL);
	if (debug > 1) {
	    fprintf(errfp, "fork(sh -c %s ...) pid %d\n", mashed[0],
		    (uint32_t)getpid());
	}
	/* reassign stdout and stderr to the log file, stdin to /dev/null */
	reopenfds(c);

	execvp(new[0], new);

	/* not reached, unless exec() fails */
	fprintf(errfp, "Error: exec(xterm failed): %s\n", strerror(errno));
	exit(EX_UNAVAILABLE);
    } else {
	if (debug)
	    fprintf(errfp, "\nStarting %d/%d %s: process id=%d\n",
		    c->n, n_opt, mashed[0], (uint32_t)c->pid);
	if (c->pid == -1) {
	    fprintf(errfp, "Error: fork() failed: %s\n", strerror(errno));
	    waitpid(c->pid, &status, WNOHANG);
	    c->pid = 0;
	}
    }

    if (mashed[0] != NULL)
	free(mashed[0]);

    sigprocmask(SIG_UNBLOCK, &set_chld, NULL);

    return(0);
}

/*
 * start a xterm with the command tail -f on the logfile
 */
void
xtermlog(child *c)
{
    char	*cmd[] = {	"xterm", "-e", "tail", "-f", NULL, NULL };
    int		status;

    if (c->logfname == NULL || c->logfile == NULL) {
	fprintf(errfp, "Error: tried to start xterm to watch NULL log file "
		"for par process\n");
	return;
    }
    cmd[4] = c->logfname;

    if ((c->xpid = fork()) == 0) {
	/* child */
	if (debug > 1) {
	    fprintf(errfp, "fork(xterm tail %s) pid %d\n", c->logfname,
		    (uint32_t)getpid());
	}
	/*
	 * set the process group id so signals are not delivered to the
	 * logging xterms.  in theory, we want them for a reason, so we
	 * dont want them to die when par exits.
	 */
	setpgid(0, getpid());

	execvp(cmd[0], cmd);

	/* not reached, unless exec() fails */
	fprintf(errfp, "Error: exec(xterm) failed: %s\n", strerror(errno));
	exit(EX_UNAVAILABLE);
    } else {
	if (c->xpid == -1) {
	    fprintf(errfp, "Error: fork() failed: %s\n", strerror(errno));
	    waitpid(c->xpid, &status, WNOHANG);
	    c->xpid = 0;
	    return;
	}
    }

    return;
}

/*
 * we are being killed.  kill and reap our sub-processes, except for
 * logging xterms.  if we get a second signal, we give up.
 */
RETSIGTYPE
handler(int sig)
{
    int		i;

    /* block sigchld for the moment, no big deal if it fails */
    sigprocmask(SIG_BLOCK, &set_chld, NULL);

    signaled = 1;

    if (debug > 1)
	fprintf(errfp, "Received signal %d\n", sig);

    for (i = 0; i < n_opt; i++) {
	if (! progeny[i].pid)
	    continue;

	if (debug > 1)
	    fprintf(errfp, "kill(%d, SIGQUIT)\n", (uint32_t)progeny[i].pid);

	if (kill(progeny[i].pid, SIGQUIT))
	    fprintf(errfp, "Error: kill(%d): %s\n", (uint32_t)progeny[i].pid,
		    strerror(errno));
    }

    /* if we rx 2 signals, just give up */
    signal(SIGHUP, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

    sigprocmask(SIG_UNBLOCK, &set_chld, NULL);

    return;
}

RETSIGTYPE
reapchild(int sig)
{
    int		i,
		status;
    char	*str;
    pid_t	pid;
    time_t	t;

    /* block sigchld for the moment, no big deal if it fails */
    sigprocmask(SIG_BLOCK, &set_chld, NULL);

    if (debug > 1)
	fprintf(errfp, "Received signal SIGCHLD\n");

    /*
     * clean-up hook so we know whether we need to walk the child table
     * again at the end of main()
     */
    chld_wait = -1;
    while ((pid = wait3(&status, WNOHANG, NULL)) > 0) {
	if (debug > 1)
	    fprintf(errfp, "reaped %d\n", (uint32_t)pid);

	t = time(NULL);

	/* search for the pid */
	for (i = 0; i < n_opt; i++) {
	    if (progeny[i].pid == pid) {
		if (debug) {
		    if (progeny[i].logfname == NULL)
			fprintf(errfp, "%d finished\n", (uint32_t)pid);
		    else
			fprintf(errfp, "%d finished (logfile %s)\n",
				(uint32_t)pid, progeny[i].logfname);
		}
		if (progeny[i].logfile != NULL) {
		    str = ctime(&t); str[strlen(str) - 1] = '\0';
		    fprintf(progeny[i].logfile, "Ending: %s: pid = %d\n",
			    str, (uint32_t)pid);
		}
		progeny[i].pid = 0;
		break;
	    } else if (i_opt && progeny[i].xpid == pid) {
		progeny[i].xpid = 0;
		if (debug > 1) {
		    if (progeny[i].logfile != NULL)
			fprintf(errfp, "%d log xterm finished\n",
				(uint32_t)pid);
		    else
			fprintf(errfp, "%d log xterm finished (logfile %s)\n",
				(uint32_t)pid, progeny[i].logfname);
		}
		break;
	    }
	}
    }

    signal(SIGCHLD, reapchild);
    sigprocmask(SIG_UNBLOCK, &set_chld, NULL);

    return;
}

void
usage(void)
{
    fprintf(errfp,
"usage: %s [-dfiqx] [-n #] [-p n] [-l logfile] [-c command] [<command file>]\n",
	progname);
    return;
}

void
vers(void)
{
    fprintf(errfp, "%s: %s version %s\n", progname, package, version);
    return;
}
