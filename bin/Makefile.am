## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am 2733 2013-10-30 23:02:46Z heas $
##
## Copyright (c) 1997-2013 by Terrapin Communications, Inc.
## All rights reserved.
##
## Please see the file COPYING for the text of the license.

#AUTOMAKE_OPTIONS=foreign no-dependencies
AUTOMAKE_OPTIONS=foreign

bin_PROGRAMS = hpuifilter
bin_SCRIPTS = agmrancid alogin anlogin anrancid arancid arrancid atrancid \
	avologin avorancid blogin brancid cat5rancid clogin complogin \
	control_rancid cssrancid dllogin elogin erancid f5rancid f10rancid \
	flogin fnlogin fnrancid francid grancid hlogin hrancid htlogin \
	htrancid jerancid jlogin mrancid mrvlogin mrvrancid mtlogin mtrancid \
	nlogin nrancid nslogin nsrancid nxrancid par prancid rivlogin \
	rivrancid rrancid srancid tlogin tntlogin tntrancid trancid xilogin \
	xirancid xrancid zrancid
# XXX hack for the grancid to get pkglibdir, see corresponding XXX below
bin_in = grancid.in

bin_SCRIPTS += lg.cgi lgform.cgi rancid-cvs rancid-fe rancid-run
EXTRA_DIST= lg.cgi.in lgform.cgi.in rancid-cvs.in rancid-fe.in rancid-run.in \
	$(bin_in)
#dist_bin_SCRIPTS= $(bin_SCRIPTS:%=%.in)

CLEANFILES= lg.cgi lgform.cgi rancid-cvs rancid-fe rancid-run
#CLEANFILES= $(bin_SCRIPTS)

hpuifilter_SOURCES = hpuifilter.c

#CPPFLAGS += @PG_CPPFLAGS@
#INCLUDES += -I$(top_srcdir)/include @PG_CPPFLAGS@
#INCLUDES += -I$(top_srcdir)/include

#CFLAGS += -g
CFLAGS = -g -O0

YFLAGS = -d
#LFLAGS = -i

# no idea why automake doesnt clean these targets
#CLEANFILES= y.tab.c y.tab.h lex.yy.c conf.h conf.c conflex.c

#clean:
#	rm -f Makefile env $(BIN_DATAS) $(BIN_PROGS)

# auto_edit does the autoconf variable substitution.  This allows the
# substitution to have the full expansion of the variables, e.g.: $sysconfdir
# will be /prefix/etc instead of ${prefix}/etc.
#
# This is a bit of a PITA, but is the method recommended by the autoconf
# documentation.
auto_edit = sed \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@localstatedir\@,$(localstatedir),g' \
	-e 's,@pkglibdir\@,$(pkglibdir),g' \
	-e 's,@sysconfdir\@,$(sysconfdir),g' \
	-e 's,@EXPECT_PATH\@,$(EXPECT_PATH),g' \
	-e 's,@PERLV\@,$(PERLV),g' \
	-e 's,@PERLV_PATH\@,$(PERLV_PATH),g' \
	-e 's,@LG_PING_CMD\@,$(LG_PING_CMD),g' \
	-e 's,@ADMINMAILPLUS\@,$(ADMINMAILPLUS),g' \
	-e 's,@MAILPLUS\@,$(MAILPLUS),g' \
	-e 's,@PACKAGE\@,$(PACKAGE),g' \
	-e 's,@SVN_FSTYPE\@,$(SVN_FSTYPE),g' \
	-e 's,@VERSION\@,$(VERSION),g' \
	-e 's,@COPYYEARS\@,$(COPYYEARS),g'

# XXX this is the generic version for the rest of bin_scripts that comes from
#     configure.in now.  perhaps change to use this.
#$(bin_SCRIPTS): Makefile $(top_builddir)/config.status
$(bin_in:%.in=%): Makefile $(top_builddir)/config.status
	rm -f $@ $@.tmp; \
	$(auto_edit) $(srcdir)/$@.in >$@.tmp; \
	chmod +x $@.tmp; \
	mv $@.tmp $@
# XXX there must be a better way to produce these dependencies, perhaps in
#     configure.
grancid: $(srcdir)/grancid.in

lg.cgi: Makefile $(srcdir)/lg.cgi.in
	rm -f lg.cgi lg.cgi.tmp; \
	$(auto_edit) $(srcdir)/lg.cgi.in >lg.cgi.tmp; \
	chmod +x lg.cgi.tmp; \
	mv lg.cgi.tmp lg.cgi

lgform.cgi: Makefile $(srcdir)/lgform.cgi.in
	rm -f lgform.cgi lgform.cgi.tmp; \
	$(auto_edit) $(srcdir)/lgform.cgi.in >lgform.cgi.tmp; \
	chmod +x lgform.cgi.tmp; \
	mv lgform.cgi.tmp lgform.cgi

rancid-cvs: Makefile $(srcdir)/rancid-cvs.in
	rm -f rancid-cvs rancid-cvs.tmp; \
	$(auto_edit) $(srcdir)/rancid-cvs.in >rancid-cvs.tmp; \
	chmod +x rancid-cvs.tmp; \
	mv rancid-cvs.tmp rancid-cvs

rancid-fe: Makefile $(srcdir)/rancid-fe.in
	rm -f rancid-fe rancid-fe.tmp; \
	$(auto_edit) $(srcdir)/rancid-fe.in >rancid-fe.tmp; \
	chmod +x rancid-fe.tmp; \
	mv rancid-fe.tmp rancid-fe

rancid-run: Makefile $(srcdir)/rancid-run.in
	rm -f rancid-run rancid-run.tmp; \
	$(auto_edit) $(srcdir)/rancid-run.in >rancid-run.tmp; \
	chmod +x rancid-run.tmp; \
	mv rancid-run.tmp rancid-run

$(bin_PROGRAMS) $(bin_SCRIPTS): ../include/version.h
