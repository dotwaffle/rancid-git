## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am 2997 2014-12-30 19:20:07Z heas $
##
## Copyright (c) 1997-2015 by Terrapin Communications, Inc.
## All rights reserved.
##
## Please see the file COPYING for the text of the license.

AUTOMAKE_OPTIONS=foreign no-dependencies

@SET_MAKE@

man_gen_MANS = lg.conf.5 rancid.3 rancid.conf.5 rancid.types.conf.5 lg_intro.1
man_nogen_MANS = agmrancid.1 alogin.1 anlogin.1 anrancid.1 arancid.1 \
		arrancid.1 avologin.1 avorancid.1 blogin.1 brancid.1 \
		cat5rancid.1 clogin.1 cloginrc.5 control_rancid.1 cssrancid.1 \
		dllogin.1 \
		elogin.1 erancid.1 f5rancid.1 f10rancid.1 flogin.1 fnlogin.1 \
		fnrancid.1 hlogin.1 hrancid.1 htlogin.1 \
		htrancid.1 jerancid.1 jlogin.1 mrancid.1 \
		mrvlogin.1 mtlogin.1 mtrancid.1 nlogin.1 \
		nrancid.1 nslogin.1 nsrancid.1 nxrancid.1 \
		panlogin.1 par.1 prancid.1 \
		rancid-cvs.1 rancid-run.1 rancid.1 \
		rancid.types.base.5 rancid_intro.1 rivlogin.1 rivrancid.1 \
		router.db.5 rrancid.1 slogin.1 srancid.1 tlogin.1 wlogin.1 \
		xilogin.1 xirancid.1 trancid.1 xrancid.1 \
		zrancid.1 \
		complogin.1 comprancid.1

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

rancid.types.conf.5: Makefile $(srcdir)/rancid.types.conf.5.in
	rm -f rancid.types.conf.5 rancid.types.conf.5.tmp; \
	$(auto_edit) $(srcdir)/rancid.types.conf.5.in >rancid.types.conf.5.tmp; \
	mv rancid.types.conf.5.tmp rancid.types.conf.5

rancid.3: Makefile $(srcdir)/rancid.3.in
	rm -f rancid.3 rancid.3.tmp; \
	$(auto_edit) $(srcdir)/rancid.3.in >rancid.3.tmp; \
	mv rancid.3.tmp rancid.3
