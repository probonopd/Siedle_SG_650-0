# -*-makefile-*-
#
# Copyright (C) 2011 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_AT91BOOTSTRAP2) += at91bootstrap2

#
# Paths and names
#
AT91BOOTSTRAP2_VERSION	:= $(call remove_quotes, $(PTXCONF_AT91BOOTSTRAP2_VERSION))
AT91BOOTSTRAP2_MD5	:= $(call remove_quotes, $(PTXCONF_AT91BOOTSTRAP2_MD5))
AT91BOOTSTRAP2		:= at91bootstrap-$(AT91BOOTSTRAP2_VERSION)
AT91BOOTSTRAP2_SUFFIX	:= tar.bz2
AT91BOOTSTRAP2_URL	:= ftp://www.at91.com/pub/buildroot/$(AT91BOOTSTRAP2).$(AT91BOOTSTRAP2_SUFFIX)
AT91BOOTSTRAP2_SOURCE	:= $(SRCDIR)/$(AT91BOOTSTRAP2).$(AT91BOOTSTRAP2_SUFFIX)
AT91BOOTSTRAP2_DIR	:= $(BUILDDIR)/$(AT91BOOTSTRAP2)
AT91BOOTSTRAP2_CONFIG	:= $(call remove_quotes, $(PTXDIST_PLATFORMCONFIGDIR)/$(PTXCONF_AT91BOOTSTRAP2_CONFIG))
AT91BOOTSTRAP2_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

AT91BOOTSTRAP2_PATH	:= PATH=$(CROSS_PATH)
AT91BOOTSTRAP2_MAKEVARS := \
	HOSTCC=$(HOSTCC) \
	ARCH=$(PTXCONF_BAREBOX_ARCH_STRING) \
	CROSS_COMPILE=$(COMPILER_PREFIX)

ifdef PTXCONF_AT91BOOTSTRAP2
$(AT91BOOTSTRAP2_CONFIG):
	@echo
	@echo "************************************************************************************"
	@echo "* Please generate a at91bootstrap config with 'ptxdist menuconfig 'at91bootstrap2' *"
	@echo "************************************************************************************"
	@echo
	@echo
	@exit 1
endif

$(STATEDIR)/at91bootstrap2.prepare: $(AT91BOOTSTRAP2_CONFIG)
	@$(call targetinfo)

	@echo "Using at91bootstrap config file: $(AT91BOOTSTRAP2_CONFIG)"
	@install -m 644 $(AT91BOOTSTRAP2_CONFIG) $(AT91BOOTSTRAP2_DIR)/.config
	@$(call ptx/oldconfig, AT91BOOTSTRAP2)

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/at91bootstrap2.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/at91bootstrap2.targetinstall:
	@$(call targetinfo)
	install -m644 $(AT91BOOTSTRAP2_DIR)/binaries/*boot-$(AT91BOOTSTRAP2_VERSION).bin \
		$(IMAGEDIR)/at91bootstrap.bin
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/at91bootstrap2.clean:
	@$(call targetinfo)
	@$(call clean_pkg, AT91BOOTSTRAP2)
	@rm -rf $(IMAGEDIR)/at91bootstrap.bin

# ----------------------------------------------------------------------------
# oldconfig / menuconfig
# ----------------------------------------------------------------------------

at91bootstrap2_oldconfig at91bootstrap2_menuconfig: $(STATEDIR)/at91bootstrap2.extract
	@if test -e $(AT91BOOTSTRAP2_CONFIG); then \
		cp $(AT91BOOTSTRAP2_CONFIG) $(AT91BOOTSTRAP2_DIR)/.config; \
	fi
	cd $(AT91BOOTSTRAP2_DIR) && \
		$(AT91BOOTSTRAP2_PATH) $(AT91BOOTSTRAP2_ENV) $(MAKE) $(AT91BOOTSTRAP2_MAKEVARS) $(subst at91bootstrap2_,,$@)
	@if cmp -s $(AT91BOOTSTRAP2_DIR)/.config $(AT91BOOTSTRAP2_CONFIG); then \
		echo "at91bootstrap configuration unchanged"; \
	else \
		cp $(AT91BOOTSTRAP2_DIR)/.config $(AT91BOOTSTRAP2_CONFIG); \
	fi

# vim: syntax=make
