## Process this file with automake to produce Makefile.in
## A Makefile.in is supplied, in case you do not have automake.

AUTOMAKE_OPTIONS=foreign no-dependencies

@SET_MAKE@

man_gen_MANS = env.5 lg.conf.5 lg_intro.1
man_nogen_MANS = do-diffs.1 alogin.1 blogin.1 clogin.1 control_rancid.1 \
		create_cvs.1 rancid.1 \
		rancid_intro.1 cloginrc.5 router.db.5 \
		elogin.1 f10rancid.1 flogin.1 hlogin.1 hrancid.1 jlogin.1 \
		jrancid.1 f10rancid.1 francid.1 cat5rancid.1 erancid.1 \
		mrancid.1 par.1 xrancid.1

man_MANS = $(man_gen_MANS) $(man_nogen_MANS)

EXTRA_DIST = $(man_nogen_MANS)

#CLEANFILES = Makefile env.5 lg.conf.5 lg_intro.1

all:

distclean: clean
