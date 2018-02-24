# -*-makefile-*-
#
# Copyright (C) 2003 Ixia Corporation (www.ixiacom.com), by Milan Bobde
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
PACKAGES-$(PTXCONF_NMAP) += nmap

#
# Paths and names
#
NMAP_VERSION	:= 5.51
NMAP_MD5	:= 0b80d2cb92ace5ebba8095a4c2850275
NMAP		:= nmap-$(NMAP_VERSION)
NMAP_SUFFIX	:= tar.bz2
NMAP_URL	:= http://nmap.org/dist/$(NMAP).$(NMAP_SUFFIX)
NMAP_SOURCE	:= $(SRCDIR)/$(NMAP).$(NMAP_SUFFIX)
NMAP_DIR	:= $(BUILDDIR)/$(NMAP)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

NMAP_PATH := PATH=$(CROSS_PATH)
NMAP_ENV  := \
	$(CROSS_ENV) \
	ac_cv_linux_vers=$(KERNEL_HEADER_VERSION_MAJOR)

#
# autoconf
#
NMAP_AUTOCONF := $(CROSS_AUTOCONF_USR) \
	$(GLOBAL_IPV6_OPTION) \
	--with-libpcre \
	--with-libpcap \
	--without-liblua \
	--without-ndiff \
	--without-zenmap \
	--without-nping \
	--without-ncat \
	\
	--enable-protochain \
	--disable-optimizer-dbg \
	--disable-yydebug

ifdef PTXCONF_NMAP_OPENSSL
NMAP_AUTOCONF += --with-openssl=$(SYSROOT)
else
NMAP_AUTOCONF += --without-openssl
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/nmap.targetinstall:
	@$(call targetinfo)

	@$(call install_init, nmap)
	@$(call install_fixup, nmap,PRIORITY,optional)
	@$(call install_fixup, nmap,SECTION,base)
	@$(call install_fixup, nmap,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, nmap,DESCRIPTION,missing)

	@$(call install_copy, nmap, 0, 0, 0755, -, /usr/bin/nmap)

ifdef PTXCONF_NMAP_SERVICES
	@$(call install_copy, nmap, 0, 0, 0644, -, \
		/usr/share/nmap/nmap-mac-prefixes)
	@$(call install_copy, nmap, 0, 0, 0644, -, \
		/usr/share/nmap/nmap-os-db)
	@$(call install_copy, nmap, 0, 0, 0644, -, \
		/usr/share/nmap/nmap-protocols)
	@$(call install_copy, nmap, 0, 0, 0644, -, \
		/usr/share/nmap/nmap-rpc)
	@$(call install_copy, nmap, 0, 0, 0644, -, \
		/usr/share/nmap/nmap-service-probes)
	@$(call install_copy, nmap, 0, 0, 0644, -, \
		/usr/share/nmap/nmap-services)
endif
	@$(call install_finish, nmap)
	@$(call touch)

# vim: syntax=make
