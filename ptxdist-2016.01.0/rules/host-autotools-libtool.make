# -*-makefile-*-
#
# Copyright (C) 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
LAZY_PACKAGES-$(PTXCONF_HOST_AUTOTOOLS_LIBTOOL) += host-autotools-libtool

#
# Paths and names
#
HOST_AUTOTOOLS_LIBTOOL_VERSION	:= 2.4.2
HOST_AUTOTOOLS_LIBTOOL_MD5	:= d2f3b7d4627e69e13514a40e72a24d50
HOST_AUTOTOOLS_LIBTOOL		:= libtool-$(HOST_AUTOTOOLS_LIBTOOL_VERSION)
HOST_AUTOTOOLS_LIBTOOL_SUFFIX	:= tar.gz
HOST_AUTOTOOLS_LIBTOOL_URL	:= $(call ptx/mirror, GNU, libtool/$(HOST_AUTOTOOLS_LIBTOOL).$(HOST_AUTOTOOLS_LIBTOOL_SUFFIX))
HOST_AUTOTOOLS_LIBTOOL_SOURCE	:= $(SRCDIR)/$(HOST_AUTOTOOLS_LIBTOOL).$(HOST_AUTOTOOLS_LIBTOOL_SUFFIX)
HOST_AUTOTOOLS_LIBTOOL_DIR	:= $(HOST_BUILDDIR)/$(HOST_AUTOTOOLS_LIBTOOL)
HOST_AUTOTOOLS_LIBTOOL_DEVPKG	:= NO
HOST_AUTOTOOLS_LIBTOOL_LICENSE	:= GPL-2.0+

$(STATEDIR)/autogen-tools: $(STATEDIR)/host-autotools-libtool.install.post

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_AUTOTOOLS_LIBTOOL_CONF_TOOL	:= autoconf
HOST_AUTOTOOLS_LIBTOOL_CONF_OPT		:= \
	$(HOST_AUTOCONF_SYSROOT) \
	--disable-static

# vim: syntax=make
