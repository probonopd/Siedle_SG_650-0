# -*-makefile-*-
#
# Copyright (C) 2010 by Robert Schwebel <r.schwebel@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBCAP) += libcap

#
# Paths and names
#
LIBCAP_VERSION	:= 2.24
LIBCAP_MD5	:= d43ab9f680435a7fff35b4ace8d45b80
LIBCAP		:= libcap-$(LIBCAP_VERSION)
LIBCAP_SUFFIX	:= tar.xz
LIBCAP_URL	:= \
	$(call ptx/mirror, KERNEL, libs/security/linux-privs/libcap2/$(LIBCAP).$(LIBCAP_SUFFIX)) \
	http://mirror.linux.org.au/linux/libs/security/linux-privs/libcap2/$(LIBCAP).$(LIBCAP_SUFFIX)
LIBCAP_SOURCE	:= $(SRCDIR)/$(LIBCAP).$(LIBCAP_SUFFIX)
LIBCAP_DIR	:= $(BUILDDIR)/$(LIBCAP)
LIBCAP_LICENSE	:= BSD-3-Clause, GPL-2.0
LIBCAP_LICENSE_FILES := file://License;md5=3f84fd6f29d453a56514cb7e4ead25f1

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBCAP_MAKE_OPT	:= \
	prefix= PAM_CAP=no DYNAMIC=yes \
	LIBATTR=$(call ptx/ifdef, PTXCONF_LIBCAP_SETCAP,yes,no) \
	lib=lib \
	CC=$(CROSS_CC) \
	BUILD_CC=$(HOSTCC)

LIBCAP_INSTALL_OPT :=  \
	$(LIBCAP_MAKE_OPT) \
	RAISE_SETFCAP=no \
	install

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libcap.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  libcap)
	@$(call install_fixup, libcap,PRIORITY,optional)
	@$(call install_fixup, libcap,SECTION,base)
	@$(call install_fixup, libcap,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libcap,DESCRIPTION,missing)

	@$(call install_copy, libcap, 0, 0, 0755, -, /sbin/getpcaps)
	@$(call install_copy, libcap, 0, 0, 0755, -, /sbin/capsh)
	@$(call install_lib,  libcap, 0, 0, 0644, libcap)
ifdef PTXCONF_LIBCAP_SETCAP
	@$(call install_copy, libcap, 0, 0, 0755, -, /sbin/setcap)
	@$(call install_copy, libcap, 0, 0, 0755, -, /sbin/getcap)
endif
	@$(call install_finish, libcap)

	@$(call touch)

# vim: syntax=make
