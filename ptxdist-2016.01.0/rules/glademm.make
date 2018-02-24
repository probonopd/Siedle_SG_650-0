# -*-makefile-*-
#
# Copyright (C) 2008 by Luotao Fu <l.fu@pengutronix.de>
#               2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GLADEMM) += glademm

#
# Paths and names
#
GLADEMM_VERSION	:= 2.6.0
GLADEMM_MD5	:= e88be4e895ff3b99d8ae39e799b714a2
GLADEMM		:= glademm-$(GLADEMM_VERSION)
GLADEMM_SUFFIX	:= tar.gz
GLADEMM_URL	:= http://home.wtal.de/petig/Gtk/$(GLADEMM).$(GLADEMM_SUFFIX)
GLADEMM_SOURCE	:= $(SRCDIR)/$(GLADEMM).$(GLADEMM_SUFFIX)
GLADEMM_DIR	:= $(BUILDDIR)/$(GLADEMM)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

GLADEMM_PATH	:= PATH=$(CROSS_PATH)
GLADEMM_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
GLADEMM_AUTOCONF := $(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/glademm.targetinstall:
	@$(call targetinfo)

	@$(call install_init, glademm)
	@$(call install_fixup, glademm,PRIORITY,optional)
	@$(call install_fixup, glademm,SECTION,base)
	@$(call install_fixup, glademm,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, glademm,DESCRIPTION,missing)

	@$(call install_copy, glademm, 0, 0, 0755, -, /usr/bin/glade--)
	@$(call install_copy, glademm, 0, 0, 0755, -, /usr/bin/glademm-embed)

	@$(call install_finish, glademm)

	@$(call touch)

# vim: syntax=make
