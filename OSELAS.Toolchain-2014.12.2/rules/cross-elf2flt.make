# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
CROSS_PACKAGES-$(PTXCONF_CROSS_ELF2FLT) += cross-elf2flt

#
# Paths and names
#
CROSS_ELF2FLT_VERSION	:= $(call remove_quotes,$(PTXCONF_CROSS_ELF2FLT_VERSION))
CROSS_ELF2FLT_MD5	:= $(call remove_quotes,$(PTXCONF_CROSS_ELF2FLT_MD5))
CROSS_ELF2FLT		:= elf2flt-$(CROSS_ELF2FLT_VERSION)
CROSS_ELF2FLT_SUFFIX	:= tar.gz
CROSS_ELF2FLT_URL	:= http://www.pengutronix.de/software/ptxdist/temporary-src/$(CROSS_ELF2FLT).$(CROSS_ELF2FLT_SUFFIX)
CROSS_ELF2FLT_SOURCE	:= $(SRCDIR)/$(CROSS_ELF2FLT).$(CROSS_ELF2FLT_SUFFIX)
CROSS_ELF2FLT_DIR	:= $(CROSS_BUILDDIR)/$(CROSS_ELF2FLT)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CROSS_ELF2FLT_DEVPKG	:= NO

#
# autoconf
#
CROSS_ELF2FLT_CONF_TOOL	:= autoconf
# HACK: add '-lz' because libbfd.a needs it and the Makefile adds it in the wrong order
CROSS_ELF2FLT_CONF_OPT	:= \
	$(PTX_HOST_CROSS_AUTOCONF) \
	--with-binutils-include-dir=$(CROSS_BINUTILS_DIR)/include \
	--with-binutils-build-dir=$(CROSS_BINUTILS_BUILDDIR) \
	--with-libbfd="$(CROSS_BINUTILS_BUILDDIR)/bfd/libbfd.a -lz -ldl"

# vim: syntax=make
