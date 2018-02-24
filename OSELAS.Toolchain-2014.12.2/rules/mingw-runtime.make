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
PACKAGES-$(PTXCONF_MINGW_RUNTIME) += mingw-runtime

#
# Paths and names
#
MINGW_RUNTIME_VERSION	:= $(call remove_quotes,$(PTXCONF_MINGW_RUNTIME_VERSION))
MINGW_RUNTIME_MD5	:= $(call remove_quotes,$(PTXCONF_MINGW_RUNTIME_MD5))
MINGW_RUNTIME		:= mingwrt-$(MINGW_RUNTIME_VERSION)-mingw32
MINGW_RUNTIME_SUFFIX	:= tar.gz
MINGW_RUNTIME_URL	:= $(PTXCONF_SETUP_SFMIRROR)/mingw/$(MINGW_RUNTIME)-src.$(MINGW_RUNTIME_SUFFIX)
MINGW_RUNTIME_SOURCE	:= $(SRCDIR)/$(MINGW_RUNTIME)-src.$(MINGW_RUNTIME_SUFFIX)
MINGW_RUNTIME_DIR	:= $(BUILDDIR)/$(MINGW_RUNTIME)
MINGW_RUNTIME_BUILDDIR	:= $(BUILDDIR)/$(MINGW_RUNTIME)-build

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(MINGW_RUNTIME_SOURCE):
	@$(call targetinfo)
	@$(call get, MINGW_RUNTIME)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/mingw-runtime.extract:
	@$(call targetinfo)
	@$(call clean, $(MINGW_RUNTIME_DIR))
	@$(call extract, MINGW_RUNTIME)
	@$(call patchin, MINGW_RUNTIME, $(MINGW_RUNTIME_DIR))
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

MINGW_RUNTIME_PATH := PATH=$(CROSS_PATH)
MINGW_RUNTIME_ENV := $(CROSS_ENV)

MINGW_RUNTIME_AUTOCONF := \
	--prefix=$(SYSROOT)/mingw \
	--host=$(PTXCONF_GNU_TARGET) \
	--target=$(PTXCONF_GNU_TARGET)

$(STATEDIR)/mingw-runtime.prepare:
	@$(call targetinfo)
	@$(call clean, $(MINGW_RUNTIME_BUILDDIR))
	mkdir -p $(MINGW_RUNTIME_BUILDDIR)
	cd $(MINGW_RUNTIME_BUILDDIR) && $(MINGW_RUNTIME_ENV) $(MINGW_RUNTIME_PATH) \
		$(MINGW_RUNTIME_DIR)/configure $(MINGW_RUNTIME_AUTOCONF)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/mingw-runtime.compile:
	@$(call targetinfo)
	cd $(MINGW_RUNTIME_BUILDDIR) && $(MINGW_RUNTIME_PATH) \
		$(MAKE) $(PARALLELMFLAGS) W32API_INCLUDE=-I$(SYSROOT)/mingw/include
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mingw-runtime.install:
	@$(call targetinfo)
	cd $(MINGW_RUNTIME_BUILDDIR) && \
		$(MINGW_RUNTIME_PATH) $(MAKE) \
		install_root=$(SYSROOT) install
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mingw-runtime.targetinstall:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

mingw-runtime_clean:
	rm -rf $(STATEDIR)/mingw-runtime.*
	rm -rf $(MINGW_RUNTIME_DIR)

# vim: syntax=make
