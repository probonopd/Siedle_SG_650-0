# -*-makefile-*-
#
# Copyright (C) 2011 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LIBELF) += host-libelf

#
# Paths and names
#
HOST_LIBELF_VERSION	:= 0.160
HOST_LIBELF_MD5		:= 7527f22dff8b1ac8c122cfc4d3d3bb1e
HOST_LIBELF		:= elfutils-$(HOST_LIBELF_VERSION)
HOST_LIBELF_SUFFIX	:= tar.bz2
HOST_LIBELF_URL		:= https://fedorahosted.org/releases/e/l/elfutils/$(HOST_LIBELF_VERSION)/$(HOST_LIBELF).$(HOST_LIBELF_SUFFIX)
HOST_LIBELF_SOURCE	:= $(SRCDIR)/$(HOST_LIBELF).$(HOST_LIBELF_SUFFIX)
HOST_LIBELF_DIR		:= $(HOST_BUILDDIR)/$(HOST_LIBELF)
HOST_LIBELF_LICENSE	:= LGPL

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_LIBELF_AUTOCONF := \
	$(PTX_HOST_AUTOCONF) \
	--disable-debugpred \
	--disable-gprof \
	--disable-gcov \
	--disable-valgrind \
	--with-zlib \
	--with-lzma

HOST_LIBELF_DEVPKG	:= NO

HOST_LIBELF_MAKE_OPT	:= -C libelf
HOST_LIBELF_INSTALL_OPT	:= -C libelf install-data-am install-exec-am

# vim: syntax=make
