# -*-makefile-*-
#
# Copyright (C) 2014 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBINPUT) += libinput

#
# Paths and names
#
LIBINPUT_VERSION	:= 0.12.0
LIBINPUT_MD5		:= cc1a8c710a90264d1464c81d657064d2
LIBINPUT		:= libinput-$(LIBINPUT_VERSION)
LIBINPUT_SUFFIX		:= tar.xz
LIBINPUT_URL		:= http://www.freedesktop.org/software/libinput/$(LIBINPUT).$(LIBINPUT_SUFFIX)
LIBINPUT_SOURCE		:= $(SRCDIR)/$(LIBINPUT).$(LIBINPUT_SUFFIX)
LIBINPUT_DIR		:= $(BUILDDIR)/$(LIBINPUT)
LIBINPUT_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBINPUT_CONF_ENV	:= \
	$(CROSS_ENV) \
	ac_cv_path_DOXYGEN=
#
# autoconf
#
LIBINPUT_CONF_TOOL	:= autoconf
LIBINPUT_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-documentation \
	--disable-event-gui \
	--disable-tests \
	--with-udev-dir=/lib/udev

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libinput.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libinput)
	@$(call install_fixup, libinput,PRIORITY,optional)
	@$(call install_fixup, libinput,SECTION,base)
	@$(call install_fixup, libinput,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, libinput,DESCRIPTION,missing)

	@$(call install_lib, libinput, 0, 0, 0644, libinput)

	@$(call install_finish, libinput)

	@$(call touch)

# vim: syntax=make
