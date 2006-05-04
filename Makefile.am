## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am,v 1.21 2005/09/29 01:14:59 heas Exp $
##
## Copyright (C) 1997-2004 by Terrapin Communications, Inc.
## All rights reserved.
##
## This software may be freely copied, modified and redistributed
## without fee for non-commerical purposes provided that this license
## remains intact and unmodified with any RANCID distribution.
##
## There is no warranty or other guarantee of fitness of this software.
## It is provided solely "as is".  The author(s) disclaim(s) all
## responsibility and liability with respect to this software's usage
## or its effect upon hardware, computer systems, other software, or
## anything else.
##
## Except where noted otherwise, rancid was written by and is maintained by
## Henry Kilmer, John Heasley, Andrew Partan, and Pete Whiting.
##

AUTOMAKE_OPTIONS=foreign no-dependencies

@SET_MAKE@

pkgdata_DATA = COPYING CHANGES FAQ README README.lg UPGRADING cloginrc.sample
EXTRA_DIST = BUGS Todo configure install-sh \
	mkinstalldirs Makefile.in Todo $(pkgdata_DATA)
#DIST_COMMON = 

# '.' is here (and at the beginnging of the macro) so that distclean-recursive
# will run make distclean in . after the other dirs (preserving Makefile) and
# mkinstalldirs will create the install destination before descending into
# the subdirs.
SUBDIRS = . include etc bin man share

# sysconfdir is here because etc/Makefile uses a script to be careful about
# installing rancid.conf, not over-writing an existing one.
DIRS2_CREATE=$(DESTDIR)$(prefix) $(DESTDIR)$(sysconfdir)
if MK_LCLSTATEDIR
DIRS2_CREATE+=$(DESTDIR)$(localstatedir)
endif
install-data-hook:
	$(mkinstalldirs) $(DIRS2_CREATE);
