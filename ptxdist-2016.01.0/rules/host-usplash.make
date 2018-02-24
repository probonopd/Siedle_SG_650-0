# -*-makefile-*-
#
# Copyright (C) 2008, 2011 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_USPLASH) += host-usplash

#
# Paths and names
#
HOST_USPLASH_DIR	= $(HOST_BUILDDIR)/$(USPLASH)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_USPLASH_PATH	:= PATH=$(HOST_PATH)
HOST_USPLASH_ENV 	:= $(HOST_ENV)

#
# autoconf
#
HOST_USPLASH_CONF_TOOL	:= autoconf
HOST_USPLASH_CONF_OPT	:= $(HOST_AUTOCONF) \
	--enable-static \
	--disable-svga-backend \
	--enable-convert-tools

$(STATEDIR)/host-usplash.prepare:
	@$(call targetinfo)
	@chmod +x $(HOST_USPLASH_DIR)/configure
	@$(call world/prepare, HOST_USPLASH)
	@$(call touch)

HOST_USPLASH_MAKE_OPT		:= -C bogl
HOST_USPLASH_INSTALL_OPT	:= -C bogl install

# vim: syntax=make
