## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am,v 1.45 2004/05/18 15:54:22 heas Exp $
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

AUTOMAKE_OPTIONS=foreign no-dependencies

@SET_MAKE@

man_gen_MANS = lg.conf.5 rancid.conf.5 lg_intro.1
man_nogen_MANS = alogin.1 blogin.1 clogin.1 control_rancid.1 \
		cssrancid.1 rancid.1 \
		rancid_intro.1 cloginrc.5 router.db.5 \
		elogin.1 f10rancid.1 flogin.1 fnrancid.1 \
		hlogin.1 hrancid.1 htlogin.1 htrancid.1 jlogin.1 \
		jrancid.1 jerancid.1 f10rancid.1 francid.1 cat5rancid.1 \
		erancid.1 mrancid.1 nlogin.1 nrancid.1 nslogin.1 nsrancid.1 \
		par.1 prancid.1 \
		rancid-cvs.1 rancid-run.1 rivlogin.1 rivrancid.1 \
		tntlogin.1 tntrancid.1 xrancid.1 zrancid.1

man_MANS = $(man_nogen_MANS) $(man_gen_MANS)

EXTRA_DIST = $(man_nogen_MANS) $(man_gen_MANS:%=%.in)

CLEANFILES = $(man_gen_MANS)

# auto_edit does the autoconf variable substitution.  This allows the
# substitution to have the full expansion of the variables, e.g.: $sysconfdir
# will be /prefix/etc instead of ${prefix}/etc.
#
# This is a bit of a PITA, but is the method recommended by the autoconf
# documentation.
auto_edit = sed \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@bindir\@,$(bindir),g' \
	-e 's,@localstatedir\@,$(localstatedir),g' \
	-e 's,@sysconfdir\@,$(sysconfdir),g' \
	-e 's,@pkgdatadir\@,$(pkgdatadir),g' \
	-e 's,@ADMINMAILPLUS\@,$(ADMINMAILPLUS),g' \
	-e 's,@MAILPLUS\@,$(MAILPLUS),g'

lg.conf.5: Makefile $(srcdir)/lg.conf.5.in
	rm -f lg.conf.5 lg.conf.5.tmp; \
	$(auto_edit) $(srcdir)/lg.conf.5.in >lg.conf.5.tmp; \
	mv lg.conf.5.tmp lg.conf.5

lg_intro.1: Makefile $(srcdir)/lg_intro.1.in
	rm -f lg_intro.1 lg_intro.1.tmp; \
	$(auto_edit) $(srcdir)/lg_intro.1.in >lg_intro.1.tmp; \
	mv lg_intro.1.tmp lg_intro.1

rancid.conf.5: Makefile $(srcdir)/rancid.conf.5.in
	rm -f rancid.conf.5 rancid.conf.5.tmp; \
	$(auto_edit) $(srcdir)/rancid.conf.5.in >rancid.conf.5.tmp; \
	mv rancid.conf.5.tmp rancid.conf.5
