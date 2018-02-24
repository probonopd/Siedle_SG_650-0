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
HOST_PACKAGES-$(PTXCONF_HOST_XZ) += host-xz

#
# Paths and names
#
HOST_XZ_VERSION	:= 5.0.7
HOST_XZ_MD5	:= 5b8c56cc95bee12e4fbd80e8b721773e
HOST_XZ		:= xz-$(HOST_XZ_VERSION)
HOST_XZ_SUFFIX	:= tar.bz2
HOST_XZ_URL	:= http://tukaani.org/xz/$(HOST_XZ).$(HOST_XZ_SUFFIX)
HOST_XZ_SOURCE	:= $(SRCDIR)/$(HOST_XZ).$(HOST_XZ_SUFFIX)
HOST_XZ_DIR	:= $(HOST_BUILDDIR)/$(HOST_XZ)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_XZ_DEVPKG		:= NO

#
# autoconf
#
HOST_XZ_CONF_TOOL	:= autoconf
HOST_XZ_CONF_OPT	:= \
	$(PTX_HOST_AUTOCONF) \
	--enable-assembler \
	--disable-small \
	--enable-threads \
	--disable-xz \
	--disable-xzdec \
	--disable-lzmadec \
	--disable-lzmainfo \
	--disable-lzma-links \
	--disable-scripts \
	--enable-static \
	--disable-shared \
	--disable-nls \
	--disable-rpath \
	--disable-werror

# vim: syntax=make
