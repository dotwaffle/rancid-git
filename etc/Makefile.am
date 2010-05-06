## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

## $Id: Makefile.am 1853 2007-11-16 01:33:38Z heas $
##
## Copyright (c) 1997-2007 by Terrapin Communications, Inc.
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
	-e 's,@RCSSYS\@,$(RCSSYS),g' \
	-e 's,@PACKAGE\@,$(PACKAGE),g' \
	-e 's,@VERSION\@,$(VERSION),g'

lg.conf.sample: Makefile $(srcdir)/lg.conf.sample.in
	rm -f lg.conf.sample lg.conf.sample.tmp; \
	$(auto_edit) $(srcdir)/lg.conf.sample.in >lg.conf.sample.tmp; \
	mv lg.conf.sample.tmp lg.conf.sample

rancid.conf.sample: Makefile $(srcdir)/rancid.conf.sample.in
	rm -f rancid.conf.sample rancid.conf.sample.tmp; \
	$(auto_edit) $(srcdir)/rancid.conf.sample.in >rancid.conf.sample.tmp; \
	mv rancid.conf.sample.tmp rancid.conf.sample
