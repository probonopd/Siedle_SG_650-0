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
PACKAGES-$(PTXCONF_XORG_PROTO_EVIEEXT) += xorg-proto-evieext

#
# Paths and names
#
XORG_PROTO_EVIEEXT_VERSION	:= 1.1.1
XORG_PROTO_EVIEEXT_MD5		:= 98bd86a13686f65f0873070fdee6efc7
XORG_PROTO_EVIEEXT		:= evieext-$(XORG_PROTO_EVIEEXT_VERSION)
XORG_PROTO_EVIEEXT_SUFFIX	:= tar.bz2
XORG_PROTO_EVIEEXT_URL		:= $(call ptx/mirror, XORG, individual/proto/$(XORG_PROTO_EVIEEXT).$(XORG_PROTO_EVIEEXT_SUFFIX))
XORG_PROTO_EVIEEXT_SOURCE	:= $(SRCDIR)/$(XORG_PROTO_EVIEEXT).$(XORG_PROTO_EVIEEXT_SUFFIX)
XORG_PROTO_EVIEEXT_DIR		:= $(BUILDDIR)/$(XORG_PROTO_EVIEEXT)
XORG_PROTO_EVIEEXT_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
XORG_PROTO_EVIEEXT_CONF_TOOL := autoconf

# vim: syntax=make

