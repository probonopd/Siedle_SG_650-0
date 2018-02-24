# -*-makefile-*-
#
# Copyright (C) 2009 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LCMS) += lcms

#
# Paths and names
#
LCMS_VERSION	:= 1.19
LCMS_MD5	:= 8af94611baf20d9646c7c2c285859818
LCMS		:= lcms-$(LCMS_VERSION)
LCMS_SUFFIX	:= tar.gz
LCMS_URL	:= $(call ptx/mirror, SF, lcms/$(LCMS).$(LCMS_SUFFIX))
LCMS_SOURCE	:= $(SRCDIR)/$(LCMS).$(LCMS_SUFFIX)
LCMS_DIR	:= $(BUILDDIR)/lcms-1.19

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LCMS_PATH	:= PATH=$(CROSS_PATH)
LCMS_ENV	:= $(CROSS_ENV)

#
# autoconf
#
LCMS_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--without-tiff \
	--without-zlib \
	--without-jpeg \
	--without-python

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lcms.targetinstall:
	@$(call targetinfo)

	@$(call install_init, lcms)
	@$(call install_fixup, lcms,PRIORITY,optional)
	@$(call install_fixup, lcms,SECTION,base)
	@$(call install_fixup, lcms,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, lcms,DESCRIPTION,missing)

	@$(call install_lib, lcms, 0, 0, 0644, liblcms)

	@$(call install_finish, lcms)

	@$(call touch)

# vim: syntax=make
