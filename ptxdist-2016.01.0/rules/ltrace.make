# -*-makefile-*-
#
# Copyright (C) 2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LTRACE) += ltrace

#
# Paths and names
#
LTRACE_VERSION	:= 0.5.1
LTRACE_MD5	:= 7dae92a19979e65bbf8ec50c0ed54d9a
LTRACE_SUFFIX	:= orig.tar.gz
LTRACE		:= ltrace-$(LTRACE_VERSION)
LTRACE_TARBALL	:= ltrace_$(LTRACE_VERSION).$(LTRACE_SUFFIX)
LTRACE_URL	:= http://www.pengutronix.de/software/ptxdist/temporary-src/$(LTRACE_TARBALL)
LTRACE_SOURCE	:= $(SRCDIR)/$(LTRACE_TARBALL)
LTRACE_DIR	:= $(BUILDDIR)/$(LTRACE)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LTRACE_PATH	:= PATH=$(CROSS_PATH)
LTRACE_ENV 	:= $(CROSS_ENV)
LTRACE_MAKEVARS	:= ARCH=$(PTXCONF_ARCH_STRING)
LTRACE_MAKE_PAR	:= NO

#
# autoconf
#
LTRACE_AUTOCONF := $(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ltrace.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  ltrace)
	@$(call install_fixup, ltrace,PRIORITY,optional)
	@$(call install_fixup, ltrace,SECTION,base)
	@$(call install_fixup, ltrace,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, ltrace,DESCRIPTION,missing)

	@$(call install_copy, ltrace, 0, 0, 0755, -, /usr/bin/ltrace)

	@$(call install_finish, ltrace)

	@$(call touch)

# vim: syntax=make
