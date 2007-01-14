## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am,v 1.36 2007/01/13 22:01:53 heas Exp $
##
## Copyright (C) 1997-2006 by Terrapin Communications, Inc.
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

bin_PROGRAMS = hpuifilter
bin_SCRIPTS = cat5rancid control_rancid \
	agmrancid alogin arancid blogin brancid clogin cssrancid \
	elogin erancid f10rancid flogin francid fnrancid \
	hlogin hrancid htlogin htrancid \
	jerancid jlogin jrancid \
	mrancid nlogin nrancid nslogin nsrancid par prancid \
	rancid rancid-fe rivlogin rivrancid rrancid srancid \
	tntlogin tntrancid xrancid zrancid

bin_SCRIPTS += lg.cgi lgform.cgi rancid-cvs rancid-run
EXTRA_DIST= lg.cgi.in lgform.cgi.in rancid-cvs.in rancid-run.in
#dist_bin_SCRIPTS= $(bin_SCRIPTS:%=%.in)

CLEANFILES= lg.cgi lgform.cgi rancid-cvs rancid-run
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
	-e 's,@sysconfdir\@,$(sysconfdir),g' \
	-e 's,@EXPECT_PATH\@,$(EXPECT_PATH),g' \
	-e 's,@PERLV\@,$(PERLV),g' \
	-e 's,@PERLV_PATH\@,$(PERLV_PATH),g' \
	-e 's,@LG_PING_CMD\@,$(LG_PING_CMD),g' \
	-e 's,@ADMINMAILPLUS\@,$(ADMINMAILPLUS),g' \
	-e 's,@MAILPLUS\@,$(MAILPLUS),g' \
	-e 's,@PACKAGE\@,$(PACKAGE),g' \
	-e 's,@VERSION\@,$(VERSION),g'

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

rancid-run: Makefile $(srcdir)/rancid-run.in
	rm -f rancid-run rancid-run.tmp; \
	$(auto_edit) $(srcdir)/rancid-run.in >rancid-run.tmp; \
	chmod +x rancid-run.tmp; \
	mv rancid-run.tmp rancid-run
