# -*-makefile-*-
#
# Copyright (C) 2014 by Juergen Beisert <jbe@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_M4) += host-m4

#
# Paths and names
#
HOST_M4_VERSION	:= 1.4.17
HOST_M4_MD5	:= 12a3c829301a4fd6586a57d3fcf196dc
HOST_M4		:= m4-$(HOST_M4_VERSION)
HOST_M4_SUFFIX	:= tar.xz
HOST_M4_URL	:= http://ftp.gnu.org/gnu/m4/$(HOST_M4).$(HOST_M4_SUFFIX)
HOST_M4_SOURCE	:= $(SRCDIR)/$(HOST_M4).$(HOST_M4_SUFFIX)
HOST_M4_DIR	:= $(HOST_BUILDDIR)/$(HOST_M4)
HOST_M4_LICENSE	:= GPL-3.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_M4_CONF_TOOL	:= autoconf

# vim: syntax=make
