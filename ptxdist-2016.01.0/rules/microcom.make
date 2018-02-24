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
PACKAGES-$(PTXCONF_MICROCOM) += microcom

#
# Paths and names
#
MICROCOM_VERSION	:= 2009.06
MICROCOM_MD5		:= 68a63c2d56196a28be89d1b4e5d13af1
MICROCOM		:= microcom-$(MICROCOM_VERSION)
MICROCOM_SUFFIX		:= tar.gz
MICROCOM_URL		:= http://www.pengutronix.de/software/microcom/download/$(MICROCOM).$(MICROCOM_SUFFIX)
MICROCOM_SOURCE		:= $(SRCDIR)/$(MICROCOM).$(MICROCOM_SUFFIX)
MICROCOM_DIR		:= $(BUILDDIR)/$(MICROCOM)
MICROCOM_LICENSE	:= GPL-2.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

MICROCOM_PATH := PATH=$(CROSS_PATH)
MICROCOM_COMPILE_ENV := $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/microcom.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/microcom.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  microcom)
	@$(call install_fixup, microcom,PRIORITY,optional)
	@$(call install_fixup, microcom,SECTION,base)
	@$(call install_fixup, microcom,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, microcom,DESCRIPTION,missing)

	@$(call install_copy, microcom, 0, 0, 0755, $(MICROCOM_DIR)/microcom, /usr/bin/microcom)

	@$(call install_finish, microcom)

	@$(call touch)

# vim: syntax=make
