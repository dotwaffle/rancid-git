dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.13)
AC_INIT(CHANGES)

dnl	VERSION needs to be updated such that 'make dist' uses the correct
dnl	filename for the directory name and tarball.
VERSION=`sed -n 's/.*version.*"\(.*\)".*/\1/p' $srcdir/include/version.h.in|tr -d ' '`
PACKAGE=`sed -n 's/.*package.*"\(.*\)".*/\1/p' $srcdir/include/version.h.in|tr -d ' '`
AC_SUBST(VERSION)
AC_SUBST(PACKAGE)
COPYYEARS="1997-2016"
AC_SUBST(COPYYEARS)

AM_INIT_AUTOMAKE($PACKAGE, $VERSION, rancid@shrubbery.net)

dnl default install location
AC_PREFIX_DEFAULT(/usr/local/rancid)

dnl AM_MAINTAINER_MODE()

dnl AC_CONFIG_SUBDIRS(util)

# make sure MAKE sets ${MAKE}
AC_PATH_PROG(MAKE,gmake,no)
if test $MAKE = no; then
    unset ac_cv_path_MAKE
    AC_PATH_PROG(MAKE,make,no)
    if test $MAKE = no; then
        AC_MSG_ERROR([can't locate a make.])
	exit 1
    fi
fi
AC_PROG_MAKE_SET()

# compiler specifics
AC_PROG_CC
#AM_C_PROTOTYPES
AC_PROG_CPP
AC_C_CONST
AC_C_INLINE
AC_C_STRINGIZE

# check includes/headers
AC_HEADER_STDC
AC_CHECK_HEADERS(ctype.h errno.h fcntl.h limits.h pty.h malloc.h memory.h \
		 siginfo.h string.h strings.h stropts.h sys/types.h \
		 sys/wait.h unistd.h util.h)
AC_CHECK_HEADERS(sysexits.h)

# check functions
AC_CHECK_FUNCS(memcpy memmove memset strerror strchr \
		strrchr strstr strtok strrtok index rindex unsetenv)
AC_CHECK_FUNCS(openpty, openpty=1, openpty=0)
# openpty() is not in the default libraries.  See if it is in some other lib.
if test $openpty = 0; then
  for lib in util; do
    AC_CHECK_LIB($lib, openpty, [AC_DEFINE(HAVE_OPENPTY)
		 LIBS="$LIBS -l$lib"; openpty=1; break])
  done
fi
# If we dont have openpty, then look for /dev/ptmx for use by our own
# openpty().
if test $openpty = 0; then
    # This check (partially) comes from expect's configure
    AC_MSG_CHECKING([for SVR4 style pty allocation])
    AH_TEMPLATE(HAVE_PTMX, "define this if your o/s has /dev/ptmx")
    if test -r /dev/ptmx ; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(HAVE_PTMX)
	# Some systems need libpt.a to use /dev/ptmx
	AC_CHECK_FUNCS(ptsname)
	if test $ac_cv_func_ptsname+set != set; then
	    # ptsname is not in the default libraries.
	    for lib in pt; do
		AC_CHECK_LIB($lib, ptsname, [LIBS="$LIBS -l$lib"; break])
	    done
	fi
    else
	AC_MSG_RESULT(no)
    fi
    # In OSF/1 case, SVR4 are somewhat different.
    # Gregory Depp <depp@osf.org> 17Aug93
    AC_MSG_CHECKING([for OSF/1 style pty allocation])
    AH_TEMPLATE(HAVE_PTMX_OSF, "define this for OSF/1 ptmx")
    if test -r /dev/ptmx_bsd ; then
	AC_DEFINE(HAVE_PTMX_OSF)
	AC_MSG_RESULT(yes)
    else
	AC_MSG_RESULT(no)
    fi
fi

dnl AC_FUNC_VPRINTF

# type checks
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T

# Package-specific options/knobs
#
# Check if user wants us to create LOCALSTATEDIR.  If it's a package-system,
# they might create it themselves for book-keeping sake; eg: NetBSD
AC_MSG_CHECKING([whether to create the local state directory at install time])
AC_ARG_ENABLE(mk-localstatedir,
	AS_HELP_STRING([--enable-mk-localstatedir], [enable creation of the local state directory at install time (default: yes)]),
[if test "$enable_mk_localstatedir" = yes; then
    AC_MSG_RESULT(yes)
else
    AC_MSG_RESULT(no)
fi],
[AC_MSG_RESULT(yes)
    enable_mk_localstatedir="yes"
])
AM_CONDITIONAL([MK_LCLSTATEDIR], [test "${enable_mk_localstatedir}" = yes])

# Check if user wants us to install sample configurations into the sysconfdir.
AC_MSG_CHECKING([whether to install sample .conf files in sysconfdir])
AC_ARG_ENABLE(conf-install,
	AS_HELP_STRING([--enable-conf-install], [enable install of sample .conf files in sysconfdir (default: yes)]),
[if test "$enable_conf_install" = yes; then
    AC_MSG_RESULT(yes)
else
    AC_MSG_RESULT(no)
fi],
[AC_MSG_RESULT(yes)
    enable_conf_install="yes"
])
AM_CONDITIONAL([CONF_INSTALL], [test "${enable_conf_install}" = yes])

# Configure for subversion revision control system instead of CVS.
SVN_FSTYPE="--fs-type fsfs"
AC_MSG_CHECKING([whether subversion])
AC_ARG_WITH(svn,
	AS_HELP_STRING([--with-svn=fstype],
			[use subversion instead of cvs, with optional svn fstype (fsfs|bdb)]),
[ case "$withval" in
  yes)
	AC_MSG_RESULT(yes)
	RCSSYS="svn"
	;;
  fsfs)
	AC_MSG_RESULT([yes fstype fsfs])
	RCSSYS="svn"
	SVN_FSTYPE="--fs-type fsfs"
	;;
  bdb)
	AC_MSG_RESULT([yes fstype bdb])
	RCSSYS="svn"
	SVN_FSTYPE="--fs-type bdb"
	;;
  no)
	AC_MSG_RESULT(no)
	RCSSYS="cvs"
	;;
  *)
	AC_MSG_ERROR([unknown svn fs-type $withval])
  esac
], [AC_MSG_RESULT(no)
    RCSSYS="cvs"
])

# Configure for git distributed version control system instead of CVS.
AC_MSG_CHECKING([whether git])
AC_ARG_WITH(git,
	AS_HELP_STRING([--with-git],
			[use git instead of cvs]),
[ case "$withval" in
  yes)
	AC_MSG_RESULT(yes)
	RCSSYS="git"
	;;
  no)
	AC_MSG_RESULT(no)
	RCSSYS="cvs"
  esac
], [AC_MSG_RESULT(no)
    RCSSYS="cvs"
])

AC_SUBST(RCSSYS)
AC_SUBST(SVN_FSTYPE)
rd_cv_RCSSYS=$RCSSYS

# Check for a preference for using mail addresses like rancid+group
# instead of the standard rancid-group
AC_MSG_CHECKING([whether mail addresses should be in the rancid+ form])
AC_ARG_ENABLE(mail-plus,
	AS_HELP_STRING([--enable-mail-plus], [enable mail to rancid+ addresses, instead of rancid-]),
[if test "$enable_mail_plus" = yes; then
    AC_MSG_RESULT(yes)
    MAILPLUS="rancid+"
    AC_SUBST(MAILPLUS)
else
    AC_MSG_RESULT(no)
    MAILPLUS="rancid-"
    AC_SUBST(MAILPLUS)
fi],
[AC_MSG_RESULT(no)
    MAILPLUS="rancid-"
    AC_SUBST(MAILPLUS)
])
rd_cv_MAILPLUS=$MAILPLUS

# Check for a preference for using mail addresses like rancid+admin-group
# instead of the standard rancid-admin-group
AC_MSG_CHECKING([whether admin mail addresses should be in the rancid-admin+ form])
AC_ARG_ENABLE(adminmail-plus,
	AS_HELP_STRING([--enable-adminmail-plus], [enable mail to rancid-admin+ addresses, instead of rancid-admin-]),
[if test "$enable_adminmail_plus" = yes; then
    AC_MSG_RESULT([rancid-admin+])
    ADMINMAILPLUS="rancid-admin+"
    AC_SUBST(ADMINMAILPLUS)
else
    AC_MSG_RESULT([${MAILPLUS}admin-])
    ADMINMAILPLUS="${MAILPLUS}admin-"
    AC_SUBST(ADMINMAILPLUS)
fi],
[AC_MSG_RESULT([${MAILPLUS}admin-])
    ADMINMAILPLUS="${MAILPLUS}admin-"
    AC_SUBST(ADMINMAILPLUS)
])
rd_cv_ADMINMAILPLUS=$ADMINMAILPLUS

AC_PATH_PROG(DIRNAME,dirname,no)

# locate GNU diff (one supporting the -u option)
AC_MSG_CHECKING([for a diff(1) that supports -u])
_DIFF_PATH=`echo $PATH:${prefix}/bin:/usr/bin:/usr/pkg/bin:/usr/local/bin:/usr/gnu/bin:/usr/contrib/bin | sed -e 's/:/ /g'`
for _diff in ${_DIFF_PATH} ;
do
    if test -e ${_diff}/diff ; then
	${_diff}/diff -u -4 /dev/null /dev/null > /dev/null 2>&1
	if test $? -ne 0; then
	    # linux/gnu shit has to change options that have been around
	    # since organized religion; see if -U 4 works.
	    ${_diff}/diff -U 4 /dev/null /dev/null > /dev/null 2>&1
	    if test $? -ne 0; then
		if test "${ADIFF}" = "" ; then
		    ADIFF="${_diff}/diff"
		    ADIFF_CMD="diff -c -4"
		fi
	    else
		DIFF="${_diff}/diff"
		DIFF_CMD="diff -U 4"
		break
	    fi
	else
	    DIFF="${_diff}/diff"
	    DIFF_CMD="diff -u -4"
	    break
	fi
    fi
done
if test "$DIFF" = "" ; then
    DIFF=$ADIFF
    DIFF_CMD=$ADIFF_CMD
fi
if test "$DIFF" = "" ; then
    AC_MSG_ERROR([can't locate diff.])
fi
AC_MSG_RESULT([${DIFF_CMD}])
rd_cv_DIFF=$DIFF
AC_SUBST(DIFF)
rd_cv_DIFF_CMD=$DIFF_CMD
AC_SUBST(DIFF_CMD)

AC_PATH_PROG(SENDMAIL,sendmail,no, /usr/sbin:/usr/bin:/usr/lib)

# Find an appropriate tar for use in "dist" targets.  A "best guess"
# is good enough -- if we can't find GNU tar, we don't really care.
AC_CHECK_PROGS(TAR, gnutar gtar tar)

AC_CHECK_PROGS(AUTOMAKE, automake)

dnl locate perl 5 and expect.
AC_PROG_INSTALL
AC_PATH_PROG(PERLV_PATH,perl5,no)
if test $PERLV_PATH = no; then
    unset ac_cv_path_PERLV_PATH
    AC_PATH_PROG(PERLV_PATH,perl,no)
    if test $PERLV_PATH = no; then
        AC_MSG_ERROR([can't locate a suitable perl5.])
	exit 1
    else
	$PERLV_PATH -e 'require 5;'
	if test $? -ne 0 ; then
           AC_MSG_ERROR([can't locate a suitable perl5.])
	   exit 1
	fi
    fi
fi
AC_SUBST(PERLV_PATH)
PERLV=`basename $PERLV_PATH`
AC_SUBST(PERLV)

AC_PATH_PROG(EXPECT_PATH,expect,no)
if test $EXPECT_PATH = no; then
    AC_MSG_ERROR([can't locate expect.])
fi
AC_SUBST(EXPECT_PATH)

# locate ping and it's syntax
AC_PATH_PROG(PING_PATH,[ping],[no],
	     [$PATH$PATH_SEPARATOR/sbin$PATH_SEPARATOR/usr/sbin])
if test $PING_PATH = no; then
    AC_MSG_ERROR([can't locate ping.])
    exit 1
fi
# ping seems to take one of two formats for count (N)
# BSD: ping -c N host
# SVR: ping host N
$PING_PATH -c 1 -v 127.0.0.1 > /dev/null 2>&1
if test $? -eq 0 ; then
    LG_PING_CMD="$PING_PATH -c 1"
else
    $PING_PATH 127.0.0.1 56 1 > /dev/null 2>&1
    if test $? -eq 0 ; then
	LG_PING_CMD="$PING_PATH"
    else
	# cygwin using windows ping?
	$PING_PATH -n 1 127.0.0.1 > /dev/null 2>&1
	if test $? -eq 0 ; then
	    LG_PING_CMD="$PING_PATH -n 1"
	else
            AC_MSG_ERROR([can't figure out how to pass count == 1 to $PING_PATH.])
	    exit 1
	fi
    fi
fi
AC_SUBST(LG_PING_CMD)
rd_cv_lg_ping_cmd=$LG_PING_CMD

# locate tools to build $PATH for rancid.conf.  order is significant.  want
# to be sure that we pick up the the proper diff and ucbmail in
# etc/rancid.conf.
ENV_PATH="`dirname $PERLV_PATH`:`dirname $EXPECT_PATH`:`dirname $SENDMAIL`"
ENV_PATH="$ENV_PATH:`dirname $DIRNAME`:`dirname $DIFF`"

if test $RCSSYS = "cvs" ; then
    AC_PATH_PROG(CVS,cvs,no)
    ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_CVS`"
elif test $RCSSYS = "svn" ; then
    AC_PATH_PROG(SVN,svn,no)
    ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_SVN`"
elif test $RCSSYS = "git" ; then
    AC_PATH_PROG(GIT,git,no)
    ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_GIT`"
fi
AC_PATH_PROG(COMM,comm,no)
ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_COMM`"
AC_PATH_PROG(FIND,find,no)
ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_FIND`"
AC_PATH_PROG(GREP,grep,no)
ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_GREP`"
AC_PATH_PROG(ID,id,no)
ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_ID`"
AC_PATH_PROG(MKDIR,mkdir,no)
ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_MKDIR`"
AC_PATH_PROG(RSH,rsh,no)
ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_RSH`"
AC_PATH_PROG(SORT,sort,no)
ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_SORT`"
AC_PATH_PROG(SSH,ssh,no)
ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_SSH`"
AC_PATH_PROG(TELNET,telnet,no)
ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_TELNET`"
AC_PATH_PROG(TOUCH,touch,no)
ENV_PATH="$ENV_PATH:`dirname $ac_cv_path_TOUCH`"
ENV_PATH="$ENV_PATH:/usr/local/bin:/usr/bin"
AC_SUBST(ENV_PATH)

ENV_PATH=`echo $ENV_PATH | $PERLV_PATH -e 'foreach $x(split(":",<>)){next unless (length($x));push(@F, $x),$seen{$x}=1 unless (defined $seen{$x});}print join(":",@F);'`
ac_cv_env_path=$ENV_PATH
AC_SUBST(ENV_PATH)


AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(bin/Makefile)
AC_CONFIG_FILES(etc/Makefile)
AC_CONFIG_FILES(include/Makefile)
AC_CONFIG_FILES(man/Makefile)
AC_CONFIG_FILES(share/Makefile)
AC_CONFIG_FILES(include/version.h)	# this is not a header in the sense of
					# AC_CONFIG_HEADERS will cause
					# autoheader to overwrite it, while all
					# want is simple variable replacement
# autoheader bits
AH_TOP([
#ifndef	CONFIG_H
#define	CONFIG_H	1
])
AH_BOTTOM([

/* damned linux... */
#ifdef LINUX
# define _GNU_SOURCE
#endif

#ifndef __P
# if STDC_HEADERS
#  define __P(a)	a
# else
#  define __P(a)	()
# endif
#endif

#if HAVE_STDLIB_H
# include <stdlib.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
# include <sys/types.h>
#elif HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if HAVE_ERRNO_H
# include <errno.h>
#endif
extern int		errno;

#if HAVE_STRING_H
# include <string.h>
#endif
#if HAVE_STRINGS_H
# include <strings.h>
#endif

#if ! HAVE_STRERROR
# define strerror(n)	sys_errlist[n];
#endif

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

#if HAVE_MEMSET
# define bzero(p,s)	memset(p, 0, s)
# define bcopy(s,d,l)	memcpy(d, s, l)
#endif

#if HAVE_INDEX && ! HAVE_STRCHR
# define index(s,c)	strchr(s,c)
#endif

#if HAVE_SYSEXITS_H
# include <sysexits.h>
#else
					/* missing sysexits.h */
# define EX_OK		0
# define EX_USAGE	64		/* command line usage error */
# define EX_NOINPUT	66		/* cannot open input */
# define EX_TEMPFAIL	75		/* temp failure */
# define EX_OSERR	71		/* system error */
# define EX_CANTCREAT	73		/* can't create (user) output file */
# define EX_IOERR	74		/* input/output error */
# define EX_CONFIG	78		/* configuration error */
#endif

#endif	/* CONFIG_H */
])
AC_CONFIG_HEADERS(include/config.h)

AC_CONFIG_FILES(bin/agmrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/alogin bin/arancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/arrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/avologin bin/avorancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/blogin bin/brancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/bntlogin bin/bntrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/brocaderancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/cat5rancid bin/clogin bin/rancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/cssrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/dlogin bin/drancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/elogin bin/erancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/f5rancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/f10rancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/flogin bin/francid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/fnlogin bin/fnrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/hlogin bin/hrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/htlogin bin/htrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/jlogin bin/jrancid bin/jerancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/mrancid bin/mrvlogin bin/mrvrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/mtrancid bin/mtlogin, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/nlogin bin/nrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/nslogin bin/nsrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/nxrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/pflogin, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/pfrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/prancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/rancid_par, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/rivlogin bin/rivrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/rrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/shellrancid bin/shelllogin, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/srancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/tlogin, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/tntlogin bin/tntrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/telcorancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/trancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/ubnt-es-rancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/ucsrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/urancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/vlogin bin/vrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/xrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/xrrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/zrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/zyrancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/h3clogin, [chmod a+x $ac_file])
AC_CONFIG_FILES(bin/h3crancid, [chmod a+x $ac_file])
AC_CONFIG_FILES(share/rtrfilter, [chmod a+x $ac_file])

AC_OUTPUT()
