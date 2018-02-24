# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XZ) += xz

#
# Paths and names
#
XZ_VERSION	:= 5.0.4
XZ_MD5		:= 741cd3a5f64b23b7bac56ec5b2258715
XZ		:= xz-$(XZ_VERSION)
XZ_SUFFIX	:= tar.bz2
XZ_URL		:= http://tukaani.org/xz/$(XZ).$(XZ_SUFFIX)
XZ_SOURCE	:= $(SRCDIR)/$(XZ).$(XZ_SUFFIX)
XZ_DIR		:= $(BUILDDIR)/$(XZ)
XZ_LICENSE	:= public_domain, LGPL-2.1+, GPL-2.0+, GPL-3.0+
XZ_LICENSE_FILES := \
	file://COPYING;md5=c475b6c7dca236740ace4bba553e8e1c \
	file://COPYING.GPLv2;md5=751419260aa954499f7abaabaa882bbe \
	file://COPYING.GPLv3;md5=d32239bcb673463ab874e80d47fae504 \
	file://COPYING.LGPLv2.1;md5=fbc093901857fcd118f065f900982c24

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
XZ_CONF_TOOL	:= autoconf
XZ_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-assembler \
	--disable-small \
	--enable-threads \
	--$(call ptx/endis,PTXCONF_XZ_TOOLS)-xz \
	--$(call ptx/endis,PTXCONF_XZ_TOOLS)-xzdec \
	--disable-lzmadec \
	--disable-lzmainfo \
	--disable-lzma-links \
	--$(call ptx/endis,PTXCONF_XZ_TOOLS)-scripts \
	--disable-static \
	--disable-nls \
	--disable-rpath \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-werror

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xz.targetinstall:
	@$(call targetinfo)

	@$(call install_init, xz)
	@$(call install_fixup, xz,PRIORITY,optional)
	@$(call install_fixup, xz,SECTION,base)
	@$(call install_fixup, xz,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, xz,DESCRIPTION,missing)

	@$(call install_lib, xz, 0, 0, 0644, liblzma)
ifdef PTXCONF_XZ_TOOLS
	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xz)
	@$(call install_link, xz, xz, /usr/bin/unxz)
	@$(call install_link, xz, xz, /usr/bin/xzcat)

	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xzdec)

	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xzdiff)
	@$(call install_link, xz, xzdiff, /usr/bin/xzcmp)

	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xzgrep)
	@$(call install_link, xz, xzgrep, /usr/bin/xzegrep)
	@$(call install_link, xz, xzgrep, /usr/bin/xzfgrep)

	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xzless)
	@$(call install_copy, xz, 0, 0, 0755, -, /usr/bin/xzmore)
endif

	@$(call install_finish, xz)

	@$(call touch)

# vim: syntax=make
