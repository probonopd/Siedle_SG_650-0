# -*-makefile-*-
#
# Copyright (C) 2015 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GLIB_NETWORKING) += glib-networking

#
# Paths and names
#
GLIB_NETWORKING_VERSION	:= 2.44.0
GLIB_NETWORKING_MD5	:= 6989b20cf3b26dd5ae272e04a9acb0b3
GLIB_NETWORKING		:= glib-networking-$(GLIB_NETWORKING_VERSION)
GLIB_NETWORKING_SUFFIX	:= tar.xz
GLIB_NETWORKING_URL	:= http://ftp.gnome.org/pub/GNOME/sources/glib-networking/$(basename $(GLIB_NETWORKING_VERSION))/$(GLIB_NETWORKING).$(GLIB_NETWORKING_SUFFIX)
GLIB_NETWORKING_SOURCE	:= $(SRCDIR)/$(GLIB_NETWORKING).$(GLIB_NETWORKING_SUFFIX)
GLIB_NETWORKING_DIR	:= $(BUILDDIR)/$(GLIB_NETWORKING)
GLIB_NETWORKING_LICENSE	:= LGPL-2.0+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

GLIB_NETWORKING_CONF_ENV	:= \
	$(CROSS_ENV) \
	PTXDIST_PKG_CONFIG_VAR_NO_SYSROOT=giomoduledir

#
# autoconf
#
GLIB_NETWORKING_CONF_TOOL	:= autoconf
GLIB_NETWORKING_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--without-libproxy \
	--without-gnome-proxy \
	--with-gnutls \
	--with-ca-certificates=/etc/ssl/certs/ca-certificates.crt \
	--without-pkcs11

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/glib-networking.targetinstall:
	@$(call targetinfo)

	@$(call install_init, glib-networking)
	@$(call install_fixup, glib-networking,PRIORITY,optional)
	@$(call install_fixup, glib-networking,SECTION,base)
	@$(call install_fixup, glib-networking,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, glib-networking,DESCRIPTION,missing)

	@$(call install_lib, glib-networking, 0, 0, 0644, gio/modules/libgiognutls)

	@$(call install_finish, glib-networking)

	@$(call touch)

# vim: syntax=make
