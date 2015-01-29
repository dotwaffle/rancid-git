## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am 2334 2011-11-17 21:09:37Z heas $
##
## Copyright (c) 1997-2008 by Terrapin Communications, Inc.
## All rights reserved.
##
## This code is derived from software contributed to and maintained by
## Terrapin Communications, Inc. by Henry Kilmer, John Heasley, Andrew Partan,
## Pete Whiting, Austin Schutz, and Andrew Fort.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 3. All advertising materials mentioning features or use of this software
##    must display the following acknowledgement:
##        This product includes software developed by Terrapin Communications,
##        Inc. and its contributors for RANCID.
## 4. Neither the name of Terrapin Communications, Inc. nor the names of its
##    contributors may be used to endorse or promote products derived from
##    this software without specific prior written permission.
## 5. It is requested that non-binding fixes and modifications be contributed
##    back to Terrapin Communications, Inc.
##
## THIS SOFTWARE IS PROVIDED BY Terrapin Communications, INC. AND CONTRIBUTORS
## ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
## TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COMPANY OR CONTRIBUTORS
## BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
## CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.
# 
#  The expect login scripts were based on Erik Sherk's gwtn, by permission.
# 
#  The original looking glass software was written by Ed Kern, provided by
#  permission and modified beyond recognition.

#AUTOMAKE_OPTIONS=foreign no-dependencies
AUTOMAKE_OPTIONS=foreign

bin_PROGRAMS = hpuifilter
bin_SCRIPTS = agmrancid alogin arancid arrancid avologin avorancid blogin \
	brancid brocaderancid cat5rancid clogin control_rancid cssrancid \
	elogin erancid f5rancid f10rancid flogin fnlogin fnrancid francid \
	hlogin hrancid htlogin htrancid jerancid jlogin jrancid mrancid \
	mrvlogin mrvrancid mtlogin mtrancid nlogin nrancid nslogin nsrancid \
	nxrancid rancid_par prancid rancid rivlogin rivrancid rrancid srancid \
	telcorancid tlogin tntlogin tntrancid trancid ucsrancid xrancid xrrancid zrancid \
	dlogin drancid shelllogin shellrancid

bin_SCRIPTS += lg.cgi lgform.cgi rancid-cvs rancid-fe rancid-run control_rancid
EXTRA_DIST= lg.cgi.in lgform.cgi.in rancid-cvs.in rancid-fe.in rancid-run.in \
	control_rancid.in
#dist_bin_SCRIPTS= $(bin_SCRIPTS:%=%.in)

CLEANFILES= lg.cgi lgform.cgi rancid-cvs rancid-fe rancid-run control_rancid
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
	-e 's,@bindir\@,$(bindir),g' \
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
	-e 's,@SVN_FSTYPE\@,$(SVN_FSTYPE),g' \
	-e 's,@DIFF_CMD\@,$(DIFF_CMD),g' \
	-e 's,@VERSION\@,$(VERSION),g' \
	-e 's,@COPYYEARS\@,$(COPYYEARS),g'

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

control_rancid: Makefile $(srcdir)/control_rancid.in
	rm -f control_rancid control_rancid.tmp; \
	$(auto_edit) $(srcdir)/control_rancid.in >control_rancid.tmp; \
	chmod +x control_rancid.tmp; \
	mv control_rancid.tmp control_rancid

$(bin_PROGRAMS) $(bin_SCRIPTS): ../include/version.h
