EXTRA_DIST =
CLEANFILES =
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
AM_MAKEFLAGS = --no-print-directory

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DLIBEXECDIR=\""$(libexecdir)"\" \
	-I${top_srcdir}/src/kmsfbwrap \
	-I${top_srcdir}/src \
	$(DRM_CFLAGS) \
	$(FUSE_CFLAGS) \
	$(LIBPNG_CFLAGS)

AM_CFLAGS = ${my_CFLAGS} \
	-fvisibility=hidden \
	-ffunction-sections \
	-fdata-sections

AM_LDFLAGS = \
	-Wl,--gc-sections \
	-Wl,--as-needed

SED_PROCESS = \
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && $(SED) \
	-e 's,@VERSION\@,$(VERSION),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@exec_prefix\@,$(exec_prefix),g' \
	-e 's,@libdir\@,$(libdir),g' \
	-e 's,@includedir\@,$(includedir),g' \
	< $< > $@ || rm $@

%.pc: %.pc.in Makefile
	$(SED_PROCESS)

bin_PROGRAMS = kmsfb-manage

kmsfb_manage_SOURCES = \
	src/png.c \
	src/kmsfb-manage.c \
	src/bgi.c

kmsfb_manage_LDADD = \
	$(FUSE_LIBS) \
	$(DRM_LIBS) \
	$(LIBPNG_LIBS)

noinst_HEADERS = \
	src/drm_fourcc_rgb_a.h \
	src/kmsfb.h
