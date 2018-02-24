# -*-makefile-*-
#
# Copyright (C) 2004 by Sascha Hauer
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
PACKAGES-$(PTXCONF_FBTEST) += fbtest

#
# Paths and names
#
FBTEST_VERSION	:= 20041102-1
FBTEST_MD5	:= d9dc61e96edb60dc52491ce3a5d5185c
FBTEST		:= fbtest-$(FBTEST_VERSION)
FBTEST_SUFFIX	:= tar.gz
FBTEST_URL	:= http://www.pengutronix.de/software/ptxdist/temporary-src/$(FBTEST).$(FBTEST_SUFFIX)
FBTEST_SOURCE	:= $(SRCDIR)/$(FBTEST).$(FBTEST_SUFFIX)
FBTEST_DIR	:= $(BUILDDIR)/$(FBTEST)
FBTEST_LICENSE	:= GPL-2.0

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

FBTEST_PATH	:= PATH=$(CROSS_PATH)
FBTEST_MAKE_ENV	:= $(CROSS_ENV) CROSS_COMPILE=$(COMPILER_PREFIX)
FBTEST_MAKE_PAR	:= NO

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/fbtest.targetinstall:
	@$(call targetinfo)

	@$(call install_init, fbtest)
	@$(call install_fixup, fbtest,PRIORITY,optional)
	@$(call install_fixup, fbtest,SECTION,base)
	@$(call install_fixup, fbtest,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, fbtest,DESCRIPTION,missing)

	@$(call install_copy, fbtest, 0, 0, 0755, -, /sbin/fbtest)

	@$(call install_finish, fbtest)

	@$(call touch)

# vim: syntax=make
