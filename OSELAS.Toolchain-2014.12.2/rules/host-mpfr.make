# -*-makefile-*-
#
# Copyright (C) 2007-2008 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_MPFR) += host-mpfr

#
# Paths and names
#
HOST_MPFR_VERSION	:= 3.1.2
HOST_MPFR_MD5		:= ee2c3ac63bf0c2359bf08fc3ee094c19
HOST_MPFR		:= mpfr-$(HOST_MPFR_VERSION)
HOST_MPFR_SUFFIX	:= tar.bz2
HOST_MPFR_SOURCE	:= $(SRCDIR)/$(HOST_MPFR).$(HOST_MPFR_SUFFIX)
HOST_MPFR_DIR		:= $(HOST_BUILDDIR)/$(HOST_MPFR)

HOST_MPFR_URL		:= \
	http://www.mpfr.org/mpfr-$(HOST_MPFR_VERSION)/$(HOST_MPFR).$(HOST_MPFR_SUFFIX) \
	http://cross-lfs.org/files/packages/svn/$(HOST_MPFR).$(HOST_MPFR_SUFFIX)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_MPFR_DEVPKG:= NO

#
# autoconf
#
HOST_MPFR_CONF_TOOL	:= autoconf
HOST_MPFR_CONF_OPT	:= \
	$(PTX_HOST_AUTOCONF) \
	--disable-shared \
	--enable-static

# vim: syntax=make
