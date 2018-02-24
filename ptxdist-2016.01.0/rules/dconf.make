# -*-makefile-*-
#
# Copyright (C) 2010 by Robert Schwebel <r.schwebel@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DCONF) += dconf

#
# Paths and names
#
DCONF_VERSION	:= 0.9.0
DCONF_MD5	:= bd59d7ad24a1cb42092f80beddce4632
DCONF		:= dconf-$(DCONF_VERSION)
DCONF_SUFFIX	:= tar.bz2
DCONF_URL	:= http://download.gnome.org/sources/dconf/0.9/$(DCONF).$(DCONF_SUFFIX)
DCONF_SOURCE	:= $(SRCDIR)/$(DCONF).$(DCONF_SUFFIX)
DCONF_DIR	:= $(BUILDDIR)/$(DCONF)
DCONF_LICENSE	:= LGPL-2.1

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
DCONF_CONF_TOOL	:= autoconf

DCONF_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-gtk-doc \
	--disable-gtk-doc-html \
	--disable-gtk-doc-pdf \
	--disable-editor


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dconf.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  dconf)
	@$(call install_fixup, dconf,PRIORITY,optional)
	@$(call install_fixup, dconf,SECTION,base)
	@$(call install_fixup, dconf,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dconf,DESCRIPTION,missing)

	@$(call install_copy, dconf, 0, 0, 0644, -, /usr/lib/gio/modules/libdconfsettings.so)
	@$(call install_lib,  dconf, 0, 0, 0644, libdconf)

	@$(call install_copy, dconf, 0, 0, 0755, -, /usr/libexec/dconf-service)
	@$(call install_copy, dconf, 0, 0, 0755, -, /usr/bin/dconf)

	@$(call install_copy, dconf, 0, 0, 0644, -, /usr/share/dbus-1/services/ca.desrt.dconf.service)
	@$(call install_copy, dconf, 0, 0, 0644, -, /usr/share/dbus-1/system-services/ca.desrt.dconf.service)

	@$(call install_finish, dconf)

	@$(call touch)

# vim: syntax=make
