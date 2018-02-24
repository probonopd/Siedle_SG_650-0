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
IMAGE_PACKAGES-$(PTXCONF_IMAGE_TOOLCHAIN_TGZ) += image-toolchain-tgz

#
# Paths and names
#
IMAGE_TOOLCHAIN_TGZ_VERSION	:= $(shell ./scripts/setlocalversion ./.tarball-version)
IMAGE_TOOLCHAIN_TGZ_IMAGE	:= $(PTXDIST_WORKSPACE)/dist/oselas.toolchain-$(IMAGE_TOOLCHAIN_TGZ_VERSION)-$(subst _,-,$(PTXCONF_PLATFORM))_$(PTX_TOOLCHAIN_HOST_ARCH).tar.xz

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------

$(IMAGE_TOOLCHAIN_TGZ_IMAGE): $(STATEDIR)/world.cleanup
	@$(call targetinfo)
	@mkdir -p $(dir $@)
	@echo 'tar -C "$(PTX_AUTOBUILD_DESTDIR)$(PTXCONF_PREFIX)" \
		--exclude=gcc-first \
		-cJf "$(@)" \
		"${PTXCONF_PROJECT}/$(PTXCONF_GNU_TARGET)/$(PTXCONF_PREFIX_TCID)"' \
		| fakeroot
	@$(call finish)

# vim: syntax=make
