## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS=foreign no-dependencies

@SET_MAKE@

EXTRA_DIST = CHANGES README Todo cloginrc.sample configure install-sh \
	mkinstalldirs Makefile.in Todo
#DIST_COMMON = 

# '.' is here (and at the beginnging of the macro) so that distclean-recursive
# will run make distclean in . after the other dirs (preserving Makefile.inc)
SUBDIRS = . bin util

all:

install-exec-local:
	$(mkinstalldirs) $(prefix) $(prefix)/bin $(prefix)/util

install:
	for file in CHANGES README cloginrc.sample ; do \
	    $(INSTALL_DATA) $$file $(prefix); \
	done

