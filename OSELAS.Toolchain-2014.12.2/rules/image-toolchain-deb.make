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
IMAGE_PACKAGES-$(PTXCONF_IMAGE_TOOLCHAIN_DEB) += image-toolchain-deb

#
# Paths and names
#
IMAGE_TOOLCHAIN_DEB_VERSION	:= $(shell ./scripts/setlocalversion ./.tarball-version)
IMAGE_TOOLCHAIN_DEB_IMAGE	:= $(PTXDIST_WORKSPACE)/dist/oselas.toolchain-$(IMAGE_TOOLCHAIN_DEB_VERSION)-$(subst _,-,$(PTXCONF_PLATFORM))_$(IMAGE_TOOLCHAIN_DEB_VERSION)_$(PTX_TOOLCHAIN_HOST_ARCH).deb

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

$(IMAGE_TOOLCHAIN_DEB_IMAGE): $(STATEDIR)/world.cleanup
	@$(call targetinfo)
	@mkdir -p $(dir $@)
	@scripts/make_deb.sh -d "$(@)" -s "$(PTXCONF_SYSROOT_CROSS)"
	@$(call finish)

# vim: syntax=make
