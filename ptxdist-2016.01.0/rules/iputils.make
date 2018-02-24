# -*-makefile-*-
#
# Copyright (C) 2014 by Alexander Aring <aar@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_IPUTILS) += iputils

#
# Paths and names
#
IPUTILS_VERSION	:= s20121221
IPUTILS_MD5	:= 6072aef64205720dd1893b375e184171
IPUTILS		:= iputils-$(IPUTILS_VERSION)
IPUTILS_SUFFIX	:= tar.bz2
IPUTILS_URL	:= http://www.skbuff.net/iputils/$(IPUTILS).$(IPUTILS_SUFFIX)
IPUTILS_SOURCE	:= $(SRCDIR)/$(IPUTILS).$(IPUTILS_SUFFIX)
IPUTILS_DIR	:= $(BUILDDIR)/$(IPUTILS)
IPUTILS_LICENSE	:= GPL-2.0
IPUTILS_LICENSE_FILES := file://ninfod/COPYING;md5=5e9a325527978995c41e6d9a83f6e6bd

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

IPUTILS_TOOLS-y					:=
IPUTILS_TOOLS-$(PTXCONF_IPUTILS_ARPING)		+= arping
IPUTILS_TOOLS-$(PTXCONF_IPUTILS_CLOCKDIFF)	+= clockdiff
IPUTILS_TOOLS-$(PTXCONF_IPUTILS_PING)		+= ping
IPUTILS_TOOLS-$(PTXCONF_IPUTILS_PING6)		+= ping6
IPUTILS_TOOLS-$(PTXCONF_IPUTILS_RARPD)		+= rarpd
IPUTILS_TOOLS-$(PTXCONF_IPUTILS_RDISC)		+= rdisc
IPUTILS_TOOLS-$(PTXCONF_IPUTILS_TFTPD)		+= tftpd
IPUTILS_TOOLS-$(PTXCONF_IPUTILS_TRACEPATH)	+= tracepath
IPUTILS_TOOLS-$(PTXCONF_IPUTILS_TRACEPATH6)	+= tracepath6
IPUTILS_TOOLS-$(PTXCONF_IPUTILS_TRACEROUTE6)	+= traceroute6

IPUTILS_CONF_TOOL	:= NO
IPUTILS_MAKEVARS	:= $(CROSS_ENV) TARGETS="$(IPUTILS_TOOLS-y)"

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/iputils.install:
	@$(call targetinfo)
	@$(foreach tool,$(IPUTILS_TOOLS-y), \
		install -D -m755 $(IPUTILS_DIR)/$(tool) \
			$(IPUTILS_PKGDIR)/usr/bin/$(tool);)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/iputils.targetinstall:
	@$(call targetinfo)

	@$(call install_init, iputils)
	@$(call install_fixup, iputils,PRIORITY,optional)
	@$(call install_fixup, iputils,SECTION,base)
	@$(call install_fixup, iputils,AUTHOR,"Alexander Aring <aar@pengutronix.de>")
	@$(call install_fixup, iputils,DESCRIPTION,missing)

	@$(foreach tool,$(IPUTILS_TOOLS-y), \
		$(call install_copy, iputils, 0, 0, 0755, -, /usr/bin/$(tool));)

	@$(call install_finish, iputils)

	@$(call touch)

# vim: syntax=make
