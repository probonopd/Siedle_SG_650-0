# -*-makefile-*-
#
# Copyright (C) 2003 by Robert Schwebel <r.schwebel@pengutronix.de>
#                       Pengutronix <info@pengutronix.de>, Germany
#               2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBPNG) += libpng

#
# Paths and names
#
LIBPNG_VERSION	:= 1.2.54
LIBPNG_MD5	:= bbb7a7264f1c7d9c444fd16bf6f89832
LIBPNG		:= libpng-$(LIBPNG_VERSION)
LIBPNG_SUFFIX	:= tar.xz
LIBPNG_URL	:= $(call ptx/mirror, SF, libpng/$(LIBPNG).$(LIBPNG_SUFFIX))
LIBPNG_SOURCE	:= $(SRCDIR)/$(LIBPNG).$(LIBPNG_SUFFIX)
LIBPNG_DIR	:= $(BUILDDIR)/$(LIBPNG)
LIBPNG_LICENSE	:= Zlib

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBPNG_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--without-libpng-compat

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libpng.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libpng)
	@$(call install_fixup, libpng,PRIORITY,optional)
	@$(call install_fixup, libpng,SECTION,base)
	@$(call install_fixup, libpng,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libpng,DESCRIPTION,missing)

	@$(call install_lib, libpng, 0, 0, 0644, libpng12)

	@$(call install_finish, libpng)

	@$(call touch)

# vim: syntax=make
