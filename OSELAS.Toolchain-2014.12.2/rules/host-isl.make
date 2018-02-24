# -*-makefile-*-
#
# Copyright (C) 2013 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_ISL) += host-isl

#
# Paths and names
#
HOST_ISL_VERSION	:= 0.12.2
HOST_ISL_MD5		:= e039bfcfb6c2ab039b8ee69bf883e824
HOST_ISL		:= isl-$(HOST_ISL_VERSION)
HOST_ISL_SUFFIX		:= tar.bz2
HOST_ISL_URL		:= http://isl.gforge.inria.fr/$(HOST_ISL).$(HOST_ISL_SUFFIX)
HOST_ISL_SOURCE		:= $(SRCDIR)/$(HOST_ISL).$(HOST_ISL_SUFFIX)
HOST_ISL_DIR		:= $(HOST_BUILDDIR)/$(HOST_ISL)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_ISL_DEVPKG	:= NO

#
# autoconf
#
HOST_ISL_CONF_TOOL	:= autoconf
HOST_ISL_CONF_OPT	:= \
	$(PTX_HOST_AUTOCONF) \
	--disable-shared \
	--enable-static

# vim: syntax=make
