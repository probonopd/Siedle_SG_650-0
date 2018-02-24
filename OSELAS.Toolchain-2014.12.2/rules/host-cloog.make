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
HOST_PACKAGES-$(PTXCONF_HOST_CLOOG) += host-cloog

#
# Paths and names
#
HOST_CLOOG_VERSION	:= 0.18.2
HOST_CLOOG_MD5	:= 69116aa6cd5e73f6b688d871875e1292
HOST_CLOOG		:= cloog-$(HOST_CLOOG_VERSION)
HOST_CLOOG_SUFFIX	:= tar.gz
HOST_CLOOG_URL	:= http://www.bastoul.net/cloog/pages/download/$(HOST_CLOOG).$(HOST_CLOOG_SUFFIX)
HOST_CLOOG_SOURCE	:= $(SRCDIR)/$(HOST_CLOOG).$(HOST_CLOOG_SUFFIX)
HOST_CLOOG_DIR	:= $(HOST_BUILDDIR)/$(HOST_CLOOG)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_CLOOG_DEVPKG	:= NO

#
# autoconf
#
HOST_CLOOG_CONF_TOOL	:= autoconf
HOST_CLOOG_CONF_OPT	:= \
	$(PTX_HOST_AUTOCONF) \
	--disable-shared \
	--enable-static \
	--with-isl=system \
	--with-gmp=system \
	--without-osl

# vim: syntax=make
