## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am 2997 2014-12-30 19:20:07Z heas $
##
## Copyright (c) 1997-2015 by Terrapin Communications, Inc.
## All rights reserved.
##
## Please see the file COPYING for the text of the license.

#AUTOMAKE_OPTIONS=foreign no-dependencies
AUTOMAKE_OPTIONS=foreign

pkgdata_SCRIPTS= rancid-cvspurge rtrfilter downreport
dist_pkgdata_SCRIPTS=getipacctg
dist_pkgdata_DATA=README.misc cisco-load.exp cisco-reload.exp \
	index.html lgnotes.html

EXTRA_DIST = rancid-cvspurge.in rancid.spec rtrfilter.in downreport.in

CLEANFILES= rancid-cvspurge downreport

all:

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
	-e 's,@pkgdatadir\@,$(pkgdatadir),g' \
	-e 's,@ADMINMAILPLUS\@,$(ADMINMAILPLUS),g' \
	-e 's,@MAILPLUS\@,$(MAILPLUS),g'

downreport: Makefile $(srcdir)/downreport.in
	rm -f downreport downreport.tmp; \
	$(auto_edit) $(srcdir)/downreport.in >downreport.tmp; \
	chmod +x downreport.tmp; \
	mv downreport.tmp downreport

rancid-cvspurge: Makefile $(srcdir)/rancid-cvspurge.in
	rm -f rancid-cvspurge rancid-cvspurge.tmp; \
	$(auto_edit) $(srcdir)/rancid-cvspurge.in >rancid-cvspurge.tmp; \
	chmod +x rancid-cvspurge.tmp; \
	mv rancid-cvspurge.tmp rancid-cvspurge
