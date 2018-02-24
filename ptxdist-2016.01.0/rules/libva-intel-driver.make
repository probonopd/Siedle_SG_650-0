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
PACKAGES-$(PTXCONF_ARCH_X86)-$(PTXCONF_LIBVA_INTEL_DRIVER) += libva-intel-driver

#
# Paths and names
#
LIBVA_INTEL_DRIVER_VERSION	:= 1.6.1
LIBVA_INTEL_DRIVER_MD5		:= ed1b04c1a3c029ad389b7e23822a2762
LIBVA_INTEL_DRIVER		:= libva-intel-driver-$(LIBVA_INTEL_DRIVER_VERSION)
LIBVA_INTEL_DRIVER_SUFFIX	:= tar.bz2
LIBVA_INTEL_DRIVER_URL		:= http://www.freedesktop.org/software/vaapi/releases/libva-intel-driver/$(LIBVA_INTEL_DRIVER).$(LIBVA_INTEL_DRIVER_SUFFIX)
LIBVA_INTEL_DRIVER_SOURCE	:= $(SRCDIR)/$(LIBVA_INTEL_DRIVER).$(LIBVA_INTEL_DRIVER_SUFFIX)
LIBVA_INTEL_DRIVER_DIR		:= $(BUILDDIR)/$(LIBVA_INTEL_DRIVER)
LIBVA_INTEL_DRIVER_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBVA_INTEL_DRIVER_CONF_TOOL	:= autoconf
LIBVA_INTEL_DRIVER_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-drm \
	--$(call ptx/endis, PTXCONF_LIBVA_INTEL_DRIVER_X11)-x11 \
	--$(call ptx/endis, PTXCONF_LIBVA_INTEL_DRIVER_WAYLAND)-wayland \
	$(GLOBAL_LARGE_FILE_OPTION)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libva-intel-driver.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libva-intel-driver)
	@$(call install_fixup, libva-intel-driver,PRIORITY,optional)
	@$(call install_fixup, libva-intel-driver,SECTION,base)
	@$(call install_fixup, libva-intel-driver,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, libva-intel-driver,DESCRIPTION,missing)

	@$(call install_lib, libva-intel-driver, 0, 0, 0644, dri/i965_drv_video)

	@$(call install_finish, libva-intel-driver)

	@$(call touch)

# vim: syntax=make
