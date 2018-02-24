# -*-makefile-*-
#
# Copyright (C) 2003 by Benedikt Spranger
#               2007 by Robert Schwebel
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SLANG) += slang

#
# Paths and names
#
SLANG_MAJOR	:= 2
SLANG_VERSION	:= $(SLANG_MAJOR).1.2
SLANG_MD5	:= 99534ae667ed1a50c863ce9a08912ccc
SLANG		:= slang-$(SLANG_VERSION)
SLANG_SUFFIX	:= tar.bz2
SLANG_URL	:= ftp://space.mit.edu/pub/davis/slang/v2.1/$(SLANG).$(SLANG_SUFFIX)
SLANG_SOURCE	:= $(SRCDIR)/$(SLANG).$(SLANG_SUFFIX)
SLANG_DIR	:= $(BUILDDIR)/$(SLANG)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
SLANG_CONF_TOOL	:= autoconf
SLANG_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--without-png \
	--without-pcre

# FIXME: iconv support is broken (at least for glibc-iconv)
SLANG_CONF_OPT	+= --without-iconv

SLANG_MAKE_OPT		:= -C src elf TERMCAP=
SLANG_INSTALL_OPT	:= -C src install-elf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/slang.targetinstall:
	@$(call targetinfo)

	@$(call install_init, slang)
	@$(call install_fixup, slang,PRIORITY,optional)
	@$(call install_fixup, slang,SECTION,base)
	@$(call install_fixup, slang,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, slang,DESCRIPTION,missing)

	@$(call install_lib, slang, 0, 0, 0644, libslang)

	@$(call install_finish, slang)

	@$(call touch)

# vim: syntax=make
