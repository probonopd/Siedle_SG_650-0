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
PACKAGES-$(PTXCONF_KERNEL_HEADERS) += kernel-headers

#
# Paths and names
#
KERNEL_HEADERS_VERSION	:= $(call remove_quotes,$(PTXCONF_KERNEL_HEADERS_VERSION))
KERNEL_HEADERS_MD5	:= $(call remove_quotes,$(PTXCONF_KERNEL_HEADERS_MD5))
KERNEL_HEADERS		:= linux-$(KERNEL_HEADERS_VERSION)
KERNEL_HEADERS_SUFFIX	:= tar.xz
KERNEL_HEADERS_URL	:= $(call kernel-url, KERNEL_HEADERS)
KERNEL_HEADERS_SOURCE	:= $(SRCDIR)/$(KERNEL_HEADERS).$(KERNEL_HEADERS_SUFFIX)
KERNEL_HEADERS_DIR	:= $(BUILDDIR)/$(KERNEL_HEADERS)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

KERNEL_HEADERS_PATH	:= PATH=$(HOST_PATH)
KERNEL_HEADERS_ENV 	:= $(HOST_ENV)
KERNEL_HEADERS_MAKEVARS	:= \
	ARCH=$(PTXCONF_ARCH) \
	CROSS_COMPILE=we_dont_have_a_cross_compiler_yet- \
	$(PARALLELMFLAGS)

$(STATEDIR)/kernel-headers.prepare:
	@$(call targetinfo)

	$(KERNEL_HEADERS_ENV) $(KERNEL_HEADERS_PATH) \
		$(MAKE) -C $(KERNEL_HEADERS_DIR) $(KERNEL_HEADERS_MAKEVARS) defconfig
	$(KERNEL_HEADERS_ENV) $(KERNEL_HEADERS_PATH) \
		yes "" | $(MAKE) -C $(KERNEL_HEADERS_DIR) $(KERNEL_HEADERS_MAKEVARS) oldconfig

	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/kernel-headers.compile:
	@$(call targetinfo)
ifndef PTXCONF_KERNEL_HEADERS_SANITIZED
#
# this is used to generate asm and asm/mach links for arm
# but fails on ppc/powerpc, thus the '-' and '-k'
#
	@-$(KERNEL_HEADERS_ENV) $(KERNEL_HEADERS_PATH) \
		$(MAKE) -C $(KERNEL_HEADERS_DIR) $(KERNEL_HEADERS_MAKEVARS) archprepare -k
#
# if the include/asm link is missing, it's really fatal
#
	test -L $(KERNEL_HEADERS_DIR)/include/asm || exit 1
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/kernel-headers.install:
	@$(call targetinfo)

ifdef PTXCONF_KERNEL_HEADERS_SANITIZED
	@$(KERNEL_HEADERS_ENV) $(KERNEL_HEADERS_PATH) \
		$(MAKE) -C $(KERNEL_HEADERS_DIR) $(KERNEL_HEADERS_MAKEVARS) \
		headers_install INSTALL_HDR_PATH=$(SYSROOT)/usr
else
	mkdir -p $(SYSROOT)/usr/include/asm
	cp -r $(KERNEL_HEADERS_DIR)/include/linux $(SYSROOT)/usr/include
	cp -r $(KERNEL_HEADERS_DIR)/include/asm/* $(SYSROOT)/usr/include/asm
	cp -r $(KERNEL_HEADERS_DIR)/include/asm-generic $(SYSROOT)/usr/include
endif

	@$(call touch)

# vim: syntax=make
