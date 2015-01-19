## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am 2997 2014-12-30 19:20:07Z heas $
##
## Copyright (C) 1997-2015 by Terrapin Communications, Inc.
## All rights reserved.
##
## Please see the file COPYING for the text of the license.
##

AUTOMAKE_OPTIONS=foreign no-dependencies

@SET_MAKE@

pkgdata_DATA = COPYING CHANGES FAQ README README.lg UPGRADING cloginrc.sample
EXTRA_DIST = BUGS Todo configure configure.vers install-sh \
	mkinstalldirs Makefile.in Todo $(pkgdata_DATA)
#DIST_COMMON = 

# '.' is here (and at the beginnging of the macro) so that distclean-recursive
# will run make distclean in . after the other dirs (preserving Makefile) and
# mkinstalldirs will create the install destination before descending into
# the subdirs.
SUBDIRS = . include etc lib bin man share

# sysconfdir is here because etc/Makefile uses a script to be careful about
# installing rancid.conf, not over-writing an existing one.
DIRS2_CREATE=$(DESTDIR)$(prefix) $(DESTDIR)$(sysconfdir)
if MK_LCLSTATEDIR
DIRS2_CREATE+=$(DESTDIR)$(localstatedir)
endif
install-data-hook:
	$(mkinstalldirs) $(DIRS2_CREATE);

distclean-local:
	-rm -rf $(top_srcdir)/autom4te.cache
