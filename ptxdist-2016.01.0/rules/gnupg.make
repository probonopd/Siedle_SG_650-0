# -*-makefile-*-
#
# Copyright (C) 2005 by Jiri Nesladek
#          
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GNUPG) += gnupg

#
# Paths and names
#
GNUPG_VERSION	:= 2.0.26
GNUPG_MD5	:= fa7e704aad33eb114d1840164455aec1
GNUPG		:= gnupg-$(GNUPG_VERSION)
GNUPG_SUFFIX	:= tar.bz2
GNUPG_URL	:= ftp://ftp.gnupg.org/gcrypt/gnupg/$(GNUPG).$(GNUPG_SUFFIX)
GNUPG_SOURCE	:= $(SRCDIR)/$(GNUPG).$(GNUPG_SUFFIX)
GNUPG_DIR	:= $(BUILDDIR)/$(GNUPG)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

GNUPG_CONF_TOOL := autoconf
GNUPG_CONF_OPT := $(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-gpg \
	--disable-gpgsm \
	--disable-agent \
	--disable-scdaemon \
	--disable-tools \
	--disable-doc \
	--disable-gpgtar \
	--disable-exec \
	--disable-exec \
	--disable-photo-viewers \
	--disable-keyserver-helpers \
	--disable-ldap \
	--disable-hkp \
	--disable-finger \
	--disable-keyserver-path \
	--disable-standard-socket \
	--disable-dns-srv \
	--disable-nls \
	--disable-rpath \
	--disable-regex \
	--disable-bzip2

ifndef PTXCONF_ICONV
GNUPG_CONF_OPT += --without-libiconv-prefix
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/gnupg.targetinstall:
	@$(call targetinfo)

	@$(call install_init, gnupg)
	@$(call install_fixup, gnupg,PRIORITY,optional)
	@$(call install_fixup, gnupg,SECTION,base)
	@$(call install_fixup, gnupg,AUTHOR,"Jiri Nesladek <nesladek@2n.cz>")
	@$(call install_fixup, gnupg,DESCRIPTION,missing)

ifdef PTXCONF_GNUPG_GPG
	@$(call install_copy, gnupg, 0, 0, 0755, -, /usr/bin/gpg2)
	@$(call install_link, gnupg, gpg2, /usr/bin/gpg)
endif
ifdef PTXCONF_GNUPG_GPGV
	@$(call install_copy, gnupg, 0, 0, 0755, -, /usr/bin/gpgv2)
endif

	@$(call install_finish, gnupg)

	@$(call touch)

# vim: syntax=make
