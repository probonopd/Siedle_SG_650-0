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
PACKAGES-$(PTXCONF_LIBBSD) += libbsd

#
# Paths and names
#
LIBBSD_VERSION	:= 0.6.0
LIBBSD_MD5	:= f6c75f0a9818e323a589bcbd560a0eb4
LIBBSD		:= libbsd-$(LIBBSD_VERSION)
LIBBSD_SUFFIX	:= tar.xz
LIBBSD_URL	:= http://libbsd.freedesktop.org/releases/$(LIBBSD).$(LIBBSD_SUFFIX)
LIBBSD_SOURCE	:= $(SRCDIR)/$(LIBBSD).$(LIBBSD_SUFFIX)
LIBBSD_DIR	:= $(BUILDDIR)/$(LIBBSD)
LIBBSD_LICENSE	:= BSD-4-Clause, BSD-3-Clause, BSD-2-Clause-NetBSD, ISC, MIT, BEER-WARE, public_domain

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
LIBBSD_CONF_ENV = \
        $(CROSS_ENV)

#
# autoconf
#
LIBSD_CONF_TOOL := autoconf
LIBSD_CONF_OPT	:= \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-shared \
	--disable-static \
	--with-gnu-ld

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbsd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libbsd)
	@$(call install_fixup, libbsd,PRIORITY,optional)
	@$(call install_fixup, libbsd,SECTION,base)
	@$(call install_fixup, libbsd,AUTHOR,"Alexander Aring <aar@pengutronix.de>")
	@$(call install_fixup, libbsd,DESCRIPTION,missing)

	@$(call install_lib, libbsd, 0, 0, 0644, libbsd)

	@$(call install_finish, libbsd)

	@$(call touch)

# vim: syntax=make
