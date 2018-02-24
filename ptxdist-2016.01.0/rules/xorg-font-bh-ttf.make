# -*-makefile-*-
#
# Copyright (C) 2006 by Erwin Rol
#           (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XORG_FONT_BH_TTF) += xorg-font-bh-ttf

#
# Paths and names
#
XORG_FONT_BH_TTF_VERSION	:= 1.0.3
XORG_FONT_BH_TTF_MD5		:= e8ca58ea0d3726b94fe9f2c17344be60
XORG_FONT_BH_TTF		:= font-bh-ttf-$(XORG_FONT_BH_TTF_VERSION)
XORG_FONT_BH_TTF_SUFFIX		:= tar.bz2
XORG_FONT_BH_TTF_URL		:= $(call ptx/mirror, XORG, individual/font/$(XORG_FONT_BH_TTF).$(XORG_FONT_BH_TTF_SUFFIX))
XORG_FONT_BH_TTF_SOURCE		:= $(SRCDIR)/$(XORG_FONT_BH_TTF).$(XORG_FONT_BH_TTF_SUFFIX)
XORG_FONT_BH_TTF_DIR		:= $(BUILDDIR)/$(XORG_FONT_BH_TTF)
XORG_FONT_BH_TTF_LICENSE	:= Luxi

ifdef PTXCONF_XORG_FONT_BH_TTF
$(STATEDIR)/xorg-fonts.targetinstall.post: $(STATEDIR)/xorg-font-bh-ttf.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XORG_FONT_BH_TTF_PATH	:= PATH=$(CROSS_PATH)
XORG_FONT_BH_TTF_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
XORG_FONT_BH_TTF_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--with-fontdir=$(XORG_FONTDIR)/truetype

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-font-bh-ttf.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-font-bh-ttf.targetinstall:
	@$(call targetinfo)

	@mkdir -p $(XORG_FONTS_DIR_INSTALL)/truetype

	@find $(XORG_FONT_BH_TTF_DIR) \
		-name "*.ttf" \
		| \
		while read file; do \
		install -m 644 $${file} $(XORG_FONTS_DIR_INSTALL)/truetype; \
	done

	@$(call touch)

# vim: syntax=make
