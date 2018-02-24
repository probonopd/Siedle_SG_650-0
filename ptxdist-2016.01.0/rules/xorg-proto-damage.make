# -*-makefile-*-
#
# Copyright (C) 2006 by Erwin Rol
#           (C) 2009 by Robert Schwebel
#           (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XORG_PROTO_DAMAGE) += xorg-proto-damage

#
# Paths and names
#
XORG_PROTO_DAMAGE_VERSION	:= 1.2.1
XORG_PROTO_DAMAGE_MD5		:= 998e5904764b82642cc63d97b4ba9e95
XORG_PROTO_DAMAGE		:= damageproto-$(XORG_PROTO_DAMAGE_VERSION)
XORG_PROTO_DAMAGE_SUFFIX	:= tar.bz2
XORG_PROTO_DAMAGE_URL		:= $(call ptx/mirror, XORG, individual/proto/$(XORG_PROTO_DAMAGE).$(XORG_PROTO_DAMAGE_SUFFIX))
XORG_PROTO_DAMAGE_SOURCE	:= $(SRCDIR)/$(XORG_PROTO_DAMAGE).$(XORG_PROTO_DAMAGE_SUFFIX)
XORG_PROTO_DAMAGE_DIR		:= $(BUILDDIR)/$(XORG_PROTO_DAMAGE)
XORG_PROTO_DAMAGE_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XORG_PROTO_DAMAGE_PATH	:= PATH=$(CROSS_PATH)
XORG_PROTO_DAMAGE_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
XORG_PROTO_DAMAGE_AUTOCONF := $(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-proto-damage.targetinstall:
	@$(call targetinfo)
	@$(call touch)

# vim: syntax=make

