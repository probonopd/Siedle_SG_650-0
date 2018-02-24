# -*-makefile-*-
#
# Copyright (C) 2013 by Sascha Hauer <s.hauer@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DT_UTILS) += dt-utils

#
# Paths and names
#
DT_UTILS_VERSION	:= 2015.10.0
DT_UTILS_MD5		:= 045291fc656dc4d6120618a4180588b9
DT_UTILS		:= dt-utils-$(DT_UTILS_VERSION)
DT_UTILS_SUFFIX		:= tar.xz
DT_UTILS_URL		:= http://pengutronix.de/software/dt-utils/download/$(DT_UTILS).$(DT_UTILS_SUFFIX)
DT_UTILS_SOURCE		:= $(SRCDIR)/$(DT_UTILS).$(DT_UTILS_SUFFIX)
DT_UTILS_DIR		:= $(BUILDDIR)/$(DT_UTILS)
DT_UTILS_LICENSE	:= GPL-2.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

DT_UTILS_CONF_TOOL := autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dt-utils.targetinstall:
	@$(call targetinfo)

	@$(call install_init, dt)
	@$(call install_fixup, dt,PRIORITY,optional)
	@$(call install_fixup, dt,SECTION,base)
	@$(call install_fixup, dt,AUTHOR,"Sascha Hauer <s.hauer@pengutronix.de>")
	@$(call install_fixup, dt,DESCRIPTION,missing)

	@$(call install_lib, dt, 0, 0, 0644, libdt-utils)
	@$(call install_copy, dt, 0, 0, 0755, -, /usr/bin/barebox-state)
	@$(call install_copy, dt, 0, 0, 0755, -, /usr/bin/fdtdump)

	@$(call install_finish, dt)

	@$(call touch)

# vim: syntax=make
