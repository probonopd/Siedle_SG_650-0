# -*-makefile-*-
#
# Copyright (C) 2008 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
CROSS_PACKAGES-$(PTXCONF_CROSS_ECJ) += cross-ecj

#
# gcj or <host>-gcj is mandatory
#
ifeq ($(PTXCONF_CROSS_ECJ)-$(shell which gcj 2>/dev/null)-$(shell which $(GNU_BUILD)-gcj 2>/dev/null),y--)
    $(warning *** gcj is mandatory to build a java cross compiler)
    $(warning *** please install gcj)
    $(error )
endif

#
# Paths and names
#
CROSS_ECJ_VERSION	:= $(call remove_quotes,$(PTXCONF_CROSS_ECJ_VERSION))
CROSS_ECJ_MD5		:= $(call remove_quotes,$(PTXCONF_CROSS_ECJ_MD5))
CROSS_ECJ		:= ecj-$(CROSS_ECJ_VERSION)
CROSS_ECJ_SUFFIX	:= jar
CROSS_ECJ_URL		:= ftp://sourceware.org/pub/java/$(CROSS_ECJ).$(CROSS_ECJ_SUFFIX)
CROSS_ECJ_SOURCE	:= $(SRCDIR)/$(CROSS_ECJ).$(CROSS_ECJ_SUFFIX)
CROSS_ECJ_DIR		:= $(CROSS_BUILDDIR)/$(CROSS_ECJ)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_CROSS_ECJ
$(STATEDIR)/cross-gcc.extract: $(STATEDIR)/cross-ecj.get
endif

$(CROSS_ECJ_SOURCE):
	@$(call targetinfo)
	@$(call get, CROSS_ECJ)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cross-ecj.extract:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/cross-ecj.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/cross-ecj.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cross-ecj.install:
	@$(call targetinfo)
	if [ \! -x "$$(which $(GNU_BUILD)-gcj)" ]; then \
		ln -sf $$(which gcj) $(PTXCONF_SYSROOT_HOST)/bin/$(GNU_BUILD)-gcj; \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

cross-ecj_clean:
	rm -rf $(STATEDIR)/cross-ecj.*
	rm -rf $(CROSS_ECJ_DIR)

# vim: syntax=make
