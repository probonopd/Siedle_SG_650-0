# -*-makefile-*-
#
# Copyright (C) 2008 by Luotao Fu <l.fu@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_UTTT) += uttt

#
# Paths and names
#
UTTT_VERSION	:= 0.11.0
UTTT_MD5	:= c141c99bfe0327477055b5ecf91f6912
UTTT		:= uttt-$(UTTT_VERSION)
UTTT_SUFFIX	:= tar.gz
UTTT_URL	:= $(call ptx/mirror, SF, ttt/$(UTTT).$(UTTT_SUFFIX))
UTTT_SOURCE	:= $(SRCDIR)/$(UTTT).$(UTTT_SUFFIX)
UTTT_DIR	:= $(BUILDDIR)/$(UTTT)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

UTTT_PATH	:= PATH=$(CROSS_PATH)
UTTT_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
UTTT_AUTOCONF := $(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/uttt.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  uttt)
	@$(call install_fixup, uttt,PRIORITY,optional)
	@$(call install_fixup, uttt,SECTION,base)
	@$(call install_fixup, uttt,AUTHOR,"Luotao Fu <l.fu@pengutronix.de>")
	@$(call install_fixup, uttt,DESCRIPTION,missing)

ifdef PTXCONF_UTTT_TTT
	@$(call install_copy, uttt, 0, 0, 0755, -, /usr/bin/ttt)
endif

ifdef PTXCONF_UTTT_CONNECT4
	@$(call install_copy, uttt, 0, 0, 0755, -, /usr/bin/connect4)
endif

	@$(call install_finish, uttt)

	@$(call touch)

# vim: syntax=make
