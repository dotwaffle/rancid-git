#ifndef CONFIG_H
#define CONFIG_H        1

@TOP@

@BOTTOM@

#ifndef __P
# if STDC_HEADERS
#  define __P(a)	a
# else
#  define __P(a)	()
# endif
#endif

#define BUF_SZ		LINE_MAX	/* (increments of) size of bufs */

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

#endif /* CONFIG_H */
