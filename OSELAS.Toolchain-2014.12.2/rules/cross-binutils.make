# -*-makefile-*-
#
# Copyright (C) 2006 by Robert Schwebel
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
CROSS_PACKAGES-$(PTXCONF_CROSS_BINUTILS) += cross-binutils

#
# Paths and names
#
CROSS_BINUTILS_VERSION	:= $(call remove_quotes,$(PTXCONF_CROSS_BINUTILS_VERSION))
CROSS_BINUTILS_MD5	:= $(call remove_quotes,$(PTXCONF_CROSS_BINUTILS_MD5))
CROSS_BINUTILS		:= binutils-$(CROSS_BINUTILS_VERSION)
CROSS_BINUTILS_SUFFIX	:= tar.bz2
CROSS_BINUTILS_SOURCE	:= $(SRCDIR)/$(CROSS_BINUTILS).$(CROSS_BINUTILS_SUFFIX)
CROSS_BINUTILS_DIR	:= $(CROSS_BUILDDIR)/$(CROSS_BINUTILS)
CROSS_BINUTILS_BUILDDIR	:= $(CROSS_BUILDDIR)/$(CROSS_BINUTILS)-build

CROSS_BINUTILS_URL	:= \
	$(call ptx/mirror, GNU, binutils/$(CROSS_BINUTILS).$(CROSS_BINUTILS_SUFFIX)) \
	ftp://sources.redhat.com/pub/binutils/snapshots/$(CROSS_BINUTILS).$(CROSS_BINUTILS_SUFFIX) \
	http://www.kernel.org/pub/linux/devel/binutils/$(CROSS_BINUTILS).$(CROSS_BINUTILS_SUFFIX)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CROSS_BINUTILS_DEVPKG	:= NO

#
# autoconf
#
CROSS_BINUTILS_CONF_TOOL	:= autoconf
CROSS_BINUTILS_CONF_OPT		:= \
	$(PTX_HOST_CROSS_AUTOCONF) \
	$(PTXCONF_TOOLCHAIN_CONFIG_SYSROOT) \
	\
	--enable-gold \
	--enable-ld=default \
	--disable-werror \
	--disable-nls \
	\
	--enable-threads \
	--enable-plugins

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cross-binutils.install:
	@$(call targetinfo)
	@$(call world/install, CROSS_BINUTILS)

	mkdir -p "$(CROSS_GCC_FIRST_PREFIX)/$(PTXCONF_GNU_TARGET)/bin"
	for file in \
		ar \
		as \
		ld \
		nm \
		objcopy \
		objdump \
		ranlib \
		strip \
		; do \
		ln -sf "../../../$(PTXCONF_GNU_TARGET)/bin/$$file" \
			"$(CROSS_GCC_FIRST_PREFIX)/$(PTXCONF_GNU_TARGET)/bin/$$file"; \
	done

	@$(call touch)

# vim: syntax=make
