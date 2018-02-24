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
PACKAGES-$(PTXCONF_MINGW_RUNTIME_HEADERS) += mingw-runtime-headers

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/mingw-runtime-headers.get:  $(STATEDIR)/mingw-runtime.get
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/mingw-runtime-headers.extract: $(STATEDIR)/mingw-runtime.extract
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/mingw-runtime-headers.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/mingw-runtime-headers.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mingw-runtime-headers.install:
	@$(call targetinfo)
	mkdir -p $(SYSROOT)/mingw/include
	cp -r $(MINGW_RUNTIME_DIR)/include/* $(SYSROOT)/mingw/include
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mingw-runtime-headers.targetinstall:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

mingw-runtime-headers_clean:
	rm -rf $(STATEDIR)/mingw-runtime-headers.*

# vim: syntax=make
