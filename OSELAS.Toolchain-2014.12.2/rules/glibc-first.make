# -*-makefile-*-
#
# Copyright (C) 2006 by Robert Schwebel
#		2007, 2008 by Marc Kleine-Budde
#               2013 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GLIBC_FIRST) += glibc-first

#
# Paths and names
#
GLIBC_FIRST_BUILDDIR	= $(BUILDDIR)/$(GLIBC)-first-build

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

GLIBC_FIRST_PATH := PATH=$(CROSS_PATH)
GLIBC_FIRST_ENV := \
	CC="$(CROSS_CC) -fuse-ld=bfd" \
	CXX=false \
	BUILD_CC=$(HOSTCC) \
	\
	libc_cv_c_cleanup=yes \
	libc_cv_forced_unwind=yes \
	libc_cv_fpie=yes \
	libc_cv_slibdir='/lib' \
	libc_cv_ssp=yes \
	libc_cv_visibility_attribute=yes \
	libc_cv_broken_visibility_attribute=no \
	libc_cv_z_relro=yes \
	\
	ac_cv_sizeof_long_double=$(PTXCONF_SIZEOF_LONG_DOUBLE)

GLIBC_FIRST_CONF_TOOL	:= autoconf
GLIBC_FIRST_CONF_OPT	 = \
	$(GLIBC_AUTOCONF_COMMON) \
	$(PTXCONF_GLIBC_CONFIG_EXTRA_CROSS) \
	--disable-debug \
	--disable-profile \

$(STATEDIR)/glibc-first.prepare: $(STATEDIR)/glibc.extract
	@$(call targetinfo)
	@$(call clean, $(GLIBC_FIRST_BUILDDIR))
	mkdir -p $(GLIBC_FIRST_BUILDDIR)
	cd $(GLIBC_FIRST_BUILDDIR) && \
		$(GLIBC_FIRST_ENV) $(GLIBC_FIRST_PATH) \
		$(GLIBC_DIR)/configure $(GLIBC_FIRST_CONF_OPT)
	@$(call touch)

GLIBC_FIRST_MAKE_OPT	:= \
	AUTOCONF=no \
	build-programs=no

GLIBC_FIRST_INSTALL_OPT	:= \
	AUTOCONF=no \
	install_root=$(SYSROOT) \
	install-lib-all \
	install-headers

# vim: syntax=make
