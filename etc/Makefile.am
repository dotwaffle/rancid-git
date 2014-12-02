## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am 2813 2014-04-03 04:34:53Z heas $
##
## Copyright (c) 1997-2013 by Terrapin Communications, Inc.
## All rights reserved.
##
## Please see the file COPYING for the text of the license.

#AUTOMAKE_OPTIONS=foreign no-dependencies
AUTOMAKE_OPTIONS=foreign

sysconf_DATA= rancid.types.base
pkgdata_nogen = 
pkgdata_gen = lg.conf.sample rancid.conf.sample
pkgdata_DATA = $(pkgdata_nogen) $(pkgdata_gen)

EXTRA_DIST= $(pkgdata_gen:%=%.in) $(pkgdata_nogen) $(sysconf_DATA)

CLEANFILES= $(pkgdata_gen)

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
	if test ! -f $(DESTDIR)$(sysconfdir)/rancid.types.conf ; then \
	    $(INSTALL_DATA) rancid.types.conf $(DESTDIR)$(sysconfdir)/rancid.types.conf; \
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
	fi; \
	$(INSTALL_DATA) rancid.types.base $(DESTDIR)$(sysconfdir)/rancid.types.base

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
	-e 's,@pkglibdir\@,$(pkglibdir),g' \
	-e 's,@sysconfdir\@,$(sysconfdir),g' \
	-e 's,@EXPECT_PATH\@,$(EXPECT_PATH),g' \
	-e 's,@PERLV_PATH\@,$(PERLV_PATH),g' \
	-e 's,@ENV_PATH\@,$(ENV_PATH),g' \
	-e 's,@RCSSYS\@,$(RCSSYS),g' \
	-e 's,@PACKAGE\@,$(PACKAGE),g' \
	-e 's,@SENDMAIL\@,$(SENDMAIL),g' \
	-e 's,@VERSION\@,$(VERSION),g'

lg.conf.sample: Makefile $(srcdir)/lg.conf.sample.in
	rm -f lg.conf.sample lg.conf.sample.tmp; \
	$(auto_edit) $(srcdir)/lg.conf.sample.in >lg.conf.sample.tmp; \
	mv lg.conf.sample.tmp lg.conf.sample

rancid.conf.sample: Makefile $(srcdir)/rancid.conf.sample.in
	rm -f rancid.conf.sample rancid.conf.sample.tmp; \
	$(auto_edit) $(srcdir)/rancid.conf.sample.in >rancid.conf.sample.tmp; \
	mv rancid.conf.sample.tmp rancid.conf.sample
