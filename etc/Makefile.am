## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am,v 1.13 2005/08/14 23:35:13 heas Exp $
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
## Henry Kilmer, John Heasley, Andrew Partan, Pete Whiting, and Austin Schutz.
##

#AUTOMAKE_OPTIONS=foreign no-dependencies
AUTOMAKE_OPTIONS=foreign

pkgdata_DATA= lg.conf.sample rancid.conf.sample
EXTRA_DIST= $(pkgdata_DATA:%=%.in)

CLEANFILES= $(pkgdata_DATA)

if CONF_INSTALL
DO_CONF_INSTALL=yes
endif

install-data-local: all
	@if test "$(DO_CONF_INSTALL)" = "yes"; then \
	if test -f $(DESTDIR)$(sysconfdir)/rancid.conf ; then \
	    echo ""; \
	    echo "WARNING: *** $(DESTDIR)$(sysconfdir)/rancid.conf exists.  See "; \
	    echo "         *** $(pkgdatadir)/rancid.conf.sample for new"; \
	    echo "         *** examples."; \
	    echo ""; \
	else \
	    $(INSTALL_DATA) rancid.conf.sample $(DESTDIR)$(sysconfdir)/rancid.conf; \
	fi; \
	if test -f $(DESTDIR)$(sysconfdir)/lg.conf ; then \
	    echo ""; \
	    echo "WARNING: *** $(DESTDIR)$(sysconfdir)/lg.conf exists.  See "; \
	    echo "         *** $(pkgdatadir)/lg.conf.sample for new"; \
	    echo "         *** examples."; \
	    echo ""; \
	else \
	    $(INSTALL_DATA) lg.conf.sample $(DESTDIR)$(sysconfdir)/lg.conf; \
	fi; \
	fi

#clean:
#	rm -f Makefile rancid.conf $(BIN_DATAS) $(BIN_PROGS)

# auto_edit does the autoconf variable substitution.  This allows the
# substitution to have the full expansion of the variables, e.g.: $sysconfdir
# will be /prefix/etc instead of ${prefix}/etc.
#
# This is a bit of a PITA, but is the method recommended by the autoconf
# documentation.
auto_edit = sed \
	-e 's,@bindir\@,$(bindir),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@localstatedir\@,$(localstatedir),g' \
	-e 's,@sysconfdir\@,$(sysconfdir),g' \
	-e 's,@EXPECT_PATH\@,$(EXPECT_PATH),g' \
	-e 's,@PERLV_PATH\@,$(PERLV_PATH),g' \
	-e 's,@ENV_PATH\@,$(ENV_PATH),g' \
	-e 's,@RCSSYS\@,$(RCSSYS),g'

lg.conf.sample: Makefile $(srcdir)/lg.conf.sample.in
	rm -f lg.conf.sample lg.conf.sample.tmp; \
	$(auto_edit) $(srcdir)/lg.conf.sample.in >lg.conf.sample.tmp; \
	mv lg.conf.sample.tmp lg.conf.sample

rancid.conf.sample: Makefile $(srcdir)/rancid.conf.sample.in
	rm -f rancid.conf.sample rancid.conf.sample.tmp; \
	$(auto_edit) $(srcdir)/rancid.conf.sample.in >rancid.conf.sample.tmp; \
	mv rancid.conf.sample.tmp rancid.conf.sample
