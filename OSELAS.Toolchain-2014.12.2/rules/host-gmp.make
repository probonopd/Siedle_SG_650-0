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
HOST_PACKAGES-$(PTXCONF_HOST_GMP) += host-gmp

#
# Paths and names
#
HOST_GMP_VERSION	:= 6.0.0a
HOST_GMP_MD5		:= 1e6da4e434553d2811437aa42c7f7c76
HOST_GMP		:= gmp-$(HOST_GMP_VERSION)
HOST_GMP_SUFFIX		:= tar.xz
HOST_GMP_URL		:= $(call ptx/mirror, GNU, gmp/$(HOST_GMP).$(HOST_GMP_SUFFIX))
HOST_GMP_SOURCE		:= $(SRCDIR)/$(HOST_GMP).$(HOST_GMP_SUFFIX)
HOST_GMP_DIR		:= $(HOST_BUILDDIR)/$(HOST_GMP)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_GMP_DEVPKG	:= NO

#
# autoconf
#
HOST_GMP_CONF_TOOL	:= autoconf
HOST_GMP_CONF_OPT	:= \
	$(PTX_HOST_AUTOCONF) \
	--disable-shared \
	--enable-static \
	--without-readline

# vim: syntax=make
