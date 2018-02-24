# -*-makefile-*-
#
# Copyright (C) 2006 by Robert Schwebel
#               2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
CROSS_PACKAGES-$(PTXCONF_CROSS_GCC) += cross-gcc

#
# Paths and names
#
CROSS_GCC_VERSION	:= $(call remove_quotes,$(PTXCONF_CROSS_GCC_VERSION))
CROSS_GCC_DL_VERSION	:= $(call remove_quotes,$(PTXCONF_CROSS_GCC_DL_VERSION))
CROSS_GCC_MD5		:= $(call remove_quotes,$(PTXCONF_CROSS_GCC_MD5))
CROSS_GCC		:= gcc-$(CROSS_GCC_DL_VERSION)
ifeq ($(CROSS_GCC_VERSION),$(CROSS_GCC_DL_VERSION))
CROSS_GCC_SUFFIX	:= tar.bz2
else
CROSS_GCC_SUFFIX	:= tar.xz
endif
CROSS_GCC_SOURCE	:= $(SRCDIR)/$(CROSS_GCC).$(CROSS_GCC_SUFFIX)
CROSS_GCC_DIR		:= $(BUILDDIR_CROSS_DEBUG)/$(CROSS_GCC)
CROSS_GCC_BUILDDIR	:= $(CROSS_BUILDDIR)/$(CROSS_GCC)-build
CROSS_GCC_BUILD_OOT	:= YES

CROSS_GCC_URL		:= \
	$(call ptx/mirror, GNU, gcc/$(CROSS_GCC)/$(CROSS_GCC).$(CROSS_GCC_SUFFIX)) \
	ftp://sourceware.org/pub/gcc/snapshots/$(CROSS_GCC_DL_VERSION)/$(CROSS_GCC).$(CROSS_GCC_SUFFIX) \
	ftp://sourceware.org/pub/gcc/releases/$(CROSS_GCC)/$(CROSS_GCC).$(CROSS_GCC_SUFFIX) \
	http://launchpad.net/gcc-linaro/4.8/$(subst linaro-,,$(CROSS_GCC_DL_VERSION))/+download/gcc-$(CROSS_GCC_DL_VERSION).$(CROSS_GCC_SUFFIX) \
	http://launchpad.net/gcc-linaro/4.7/$(subst linaro-,,$(CROSS_GCC_DL_VERSION))/+download/gcc-$(CROSS_GCC_DL_VERSION).$(CROSS_GCC_SUFFIX) \
	http://launchpad.net/gcc-linaro/4.6/$(subst linaro-,,$(CROSS_GCC_DL_VERSION))/+download/gcc-$(CROSS_GCC_DL_VERSION).$(CROSS_GCC_SUFFIX)

ptx/abs2rel := $(PTXDIST_WORKSPACE)/scripts/ptxd_abs2rel.sh

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cross-gcc.extract:
	@$(call targetinfo)
	@$(call clean, $(CROSS_GCC_DIR))
	@$(call extract, CROSS_GCC, $(BUILDDIR_CROSS_DEBUG))
	@$(call patchin, CROSS_GCC, $(CROSS_GCC_DIR))
ifdef PTXCONF_CROSS_ECJ
	@cp $(CROSS_ECJ_SOURCE) $(CROSS_GCC_DIR)/ecj.jar
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CROSS_GCC_DEVPKG	:= NO
CROSS_GCC_CONF_ENV	:= $(HOST_ENV) MAKEINFO=:

#
# autoconf
#
CROSS_GCC_AUTOCONF_COMMON := \
	$(PTX_HOST_CROSS_AUTOCONF_BUILD) \
	$(PTX_HOST_CROSS_AUTOCONF_HOST) \
	$(PTX_HOST_CROSS_AUTOCONF_TARGET) \
	\
	$(PTXCONF_TOOLCHAIN_CONFIG_SYSROOT) \
	$(PTXCONF_TOOLCHAIN_CONFIG_MULTILIB) \
	\
	$(PTXCONF_CROSS_GCC_CONFIG_EXTRA) \
	$(PTXCONF_CROSS_GCC_CONFIG_LIBC) \
	$(PTXCONF_CROSS_GCC_CONFIG_CXA_ATEXIT) \
	$(PTXCONF_CROSS_GCC_CONFIG_SJLJ_EXCEPTIONS) \
	\
	--disable-nls \
	--disable-decimal-float \
	--disable-fixed-point \
	--disable-win32-registry \
	\
	--enable-symvers=gnu \
	\
	--with-pkgversion=$(PTXCONF_CROSS_GCC_PKGVERSION) \
	--enable-threads=$(PTXCONF_CROSS_GCC_THREADS) \
	--with-system-zlib

ifdef PTXCONF_HOST_GMP
CROSS_GCC_AUTOCONF_COMMON += --with-gmp=$(PTXCONF_SYSROOT_HOST)
endif

ifdef PTXCONF_HOST_MPFR
CROSS_GCC_AUTOCONF_COMMON += --with-mpfr=$(PTXCONF_SYSROOT_HOST)
endif

ifdef PTXCONF_HOST_MPC
CROSS_GCC_AUTOCONF_COMMON += --with-mpc=$(PTXCONF_SYSROOT_HOST)
endif

ifdef PTXCONF_HOST_CLOOG
CROSS_GCC_AUTOCONF_COMMON += --with-isl=$(PTXCONF_SYSROOT_HOST) --with-cloog=$(PTXCONF_SYSROOT_HOST)
endif

#   --enable-tls            enable or disable generation of tls code
#                           overriding the assembler check for tls support
#   --enable-initfini-array       use .init_array/.fini_array sections
#   --enable-version-specific-runtime-libs
#                           specify that runtime libraries should be
#                           installed in a compiler-specific directory
#   --with-long-double-128  Use 128-bit long double by default.


#
# language selection
#
CROSS_GCC_LANG-$(PTXCONF_CROSS_GCC_LANG_C)		+= c
CROSS_GCC_LANG-$(PTXCONF_CROSS_GCC_LANG_CXX)		+= c++
CROSS_GCC_LANG-$(PTXCONF_CROSS_GCC_LANG_JAVA)		+= java
CROSS_GCC_LANG-$(PTXCONF_CROSS_GCC_LANG_FORTRAN)	+= fortran

CROSS_GCC_CONF_TOOL	:= autoconf
CROSS_GCC_CONF_OPT	:= \
	$(CROSS_GCC_AUTOCONF_COMMON) \
	$(PTX_HOST_CROSS_AUTOCONF_PREFIX) \
	\
	--enable-languages=$(subst $(space),$(comma),$(CROSS_GCC_LANG-y)) \
	--enable-c99 \
	--enable-long-long \
	--enable-libstdcxx-debug \
	--enable-profile \
	\
	$(PTXCONF_CROSS_GCC_CONFIG_SHARED) \
	$(PTXCONF_CROSS_GCC_CONFIG_LIBSSP) \
	\
	$(if $(filter 3.%,$(CROSS_GCC_VERSION)),,--enable-checking=release)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cross-gcc.install:
	@$(call targetinfo)
	@$(call world/install, CROSS_GCC)

	@cd "$(PTXCONF_SYSROOT_CROSS)/$(PTX_TOUPLE_TARGET)/lib"; \
	dst="$(SYSROOT)/usr/lib"; \
	rel="$$($(ptx/abs2rel) "$${dst}" "$${PWD}")" && \
	for file in \
		libg2c.*so* \
		libgcc_s.*so* \
		libgfortran.*so* \
		libgomp.*so* \
		libmudflap.*so* \
		libmudflapth.*so* \
		libssp.*so* \
		libstdc++.*so* \
		; do \
		[ \! -e "$${file}" ] && continue; \
		\
		dst_file="$${dst}/$${file}"; \
		rel_file="$${rel}/$${file}"; \
		if [ -L "$${file}" ]; then \
			cp -vdR "$${file}" "$${dst_file}"; \
		else \
			ln -vfs "$${rel_file}" "$${dst_file}"; \
		fi || exit 1; \
	done

	@find $(PTXCONF_SYSROOT_CROSS) -name "*.la" -print0 | xargs -0 rm -v -f

	@$(call touch)

# vim: syntax=make
