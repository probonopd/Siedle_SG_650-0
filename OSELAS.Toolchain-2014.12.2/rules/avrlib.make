# -*-makefile-*-
# $Id: template 6655 2007-01-02 12:55:21Z rsc $
#
# Copyright (C) 2007 by Juergen Beisert
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_AVRLIB) += avrlib

#
# Paths and names
#
AVRLIB_VERSION	:= $(call remove_quotes,$(PTXCONF_AVRLIB_VERSION))
AVRLIB_MD5	:= $(call remove_quotes,$(PTXCONF_AVRLIB_MD5))
AVRLIB		:= avr-libc-$(AVRLIB_VERSION)
AVRLIB_SUFFIX	:= tar.bz2
AVRLIB_URL	:= http://download.savannah.gnu.org/releases/avr-libc/$(AVRLIB).$(AVRLIB_SUFFIX)
AVRLIB_SOURCE	:= $(SRCDIR)/$(AVRLIB).$(AVRLIB_SUFFIX)
AVRLIB_DIR	:= $(BUILDDIR)/$(AVRLIB)
AVRLIB_BUILDDIR	:= $(BUILDDIR)/$(AVRLIB)-build

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(AVRLIB_SOURCE):
	@$(call targetinfo)
	@$(call get, AVRLIB)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/avrlib.extract:
	@$(call targetinfo)
	@$(call clean, $(AVRLIB_DIR))
	@$(call extract, AVRLIB)
	@$(call patchin, AVRLIB)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

AVRLIB_PATH	:= PATH=$(CROSS_PATH)
AVRLIB_ENV 	:= \
	$(CROSS_ENV) \
	CC_FOR_BUILD=$(HOSTCC)

#
# autoconf
#
AVRLIB_AUTOCONF := \
	--prefix=$(PTXCONF_SYSROOT_TARGET) \
	--host=$(PTXCONF_GNU_TARGET) \
	--target=$(PTXCONF_GNU_TARGET)

$(STATEDIR)/avrlib.prepare:
	@$(call targetinfo)
	@$(call clean, $(AVRLIB_BUILDDIR))
	mkdir -p $(AVRLIB_BUILDDIR)
	cd $(AVRLIB_BUILDDIR) && \
		$(AVRLIB_PATH) $(AVRLIB_ENV) \
		$(AVRLIB_DIR)/configure $(AVRLIB_AUTOCONF)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/avrlib.compile:
	@$(call targetinfo)
	cd $(AVRLIB_BUILDDIR) && $(AVRLIB_PATH) $(MAKE) $(PARALLELMFLAGS)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/avrlib.install:
	@$(call targetinfo)
	cd $(AVRLIB_BUILDDIR) && \
		$(AVRLIB_PATH) $(MAKE) install
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/avrlib.targetinstall:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

avrlib_clean:
	rm -rf $(STATEDIR)/avrlib.*
	rm -rf $(AVRLIB_DIR)
	rm -rf $(AVRLIB_BUILDDIR)

# vim: syntax=make

