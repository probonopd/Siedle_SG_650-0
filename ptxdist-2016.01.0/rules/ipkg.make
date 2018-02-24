# -*-makefile-*-
#
# Copyright (C) 2005 by Robert Schwebel
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_IPKG) += ipkg

#
# Paths and names
#
IPKG_VERSION	:= 0.99.163
IPKG_MD5	:= 0b10ad2924611bccaea8ddf98481a192
IPKG		:= ipkg-$(IPKG_VERSION)
IPKG_SUFFIX	:= tar.gz
IPKG_URL	:= http://www.pengutronix.de/software/ptxdist/temporary-src/$(IPKG).$(IPKG_SUFFIX)
IPKG_SOURCE	:= $(SRCDIR)/$(IPKG).$(IPKG_SUFFIX)
IPKG_DIR	:= $(BUILDDIR)/$(IPKG)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

IPKG_PATH	:= PATH=$(CROSS_PATH)
IPKG_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
IPKG_AUTOCONF := $(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ipkg.install:
	@$(call targetinfo)
	@$(call install, IPKG)
	@install -m 755 $(IPKG_DIR)/ipkg_extract_test $(IPKG_PKGDIR)/usr/bin/
	@install -m 755 $(IPKG_DIR)/ipkg_hash_test $(IPKG_PKGDIR)/usr/bin/
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ipkg.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ipkg)
	@$(call install_fixup, ipkg,PRIORITY,optional)
	@$(call install_fixup, ipkg,SECTION,base)
	@$(call install_fixup, ipkg,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, ipkg,DESCRIPTION,missing)

	@$(call install_lib, ipkg, 0, 0, 0644, libipkg)

ifdef PTXCONF_IPKG_LOG_WRAPPER
	@$(call install_copy, ipkg, 0, 0, 0755, -, \
		/usr/bin/ipkg-cl)
	@$(call install_alternative, ipkg, 0, 0, 0755, /usr/bin/ipkg_log_wrapper)
else
	@$(call install_copy, ipkg, 0, 0, 0755, \
		$(IPKG_PKGDIR)/usr/bin/ipkg-cl, \
		/usr/bin/ipkg)
endif

ifdef PTXCONF_IPKG_EXTRACT_TEST
	@$(call install_copy, ipkg, 0, 0, 0755, -, \
		/usr/bin/ipkg_extract_test)
endif
ifdef PTXCONF_IPKG_HASH_TEST
	@$(call install_copy, ipkg, 0, 0, 0755, -, \
		/usr/bin/ipkg_hash_test)
endif

ifdef PTXCONF_IPKG_IPKG_CONF
	@$(call install_alternative, ipkg, 0, 0, 0644, /etc/ipkg.conf)
	@$(call install_replace, ipkg, /etc/ipkg.conf, @SRC@, \
		$(PTXCONF_IPKG_IPKG_CONF_URL))
	@$(call install_replace, ipkg, /etc/ipkg.conf, @ARCH@, \
		$(PTXDIST_IPKG_ARCH_STRING))
endif

	@$(call install_finish, ipkg)

	@$(call touch)

# vim: syntax=make
