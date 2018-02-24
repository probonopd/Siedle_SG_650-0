# -*-makefile-*-
#
# Copyright (C) 2015 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XORG_FONT_TTF_CALADEA) += xorg-font-ttf-caladea

#
# Paths and names
#
XORG_FONT_TTF_CALADEA_VERSION	:= 20130214
XORG_FONT_TTF_CALADEA_MD5	:= 368f114c078f94214a308a74c7e991bc
XORG_FONT_TTF_CALADEA		:= crosextrafonts-$(XORG_FONT_TTF_CALADEA_VERSION)
XORG_FONT_TTF_CALADEA_SUFFIX	:= tar.gz
XORG_FONT_TTF_CALADEA_URL	:= http://commondatastorage.googleapis.com/chromeos-localmirror/distfiles/$(XORG_FONT_TTF_CALADEA).$(XORG_FONT_TTF_CALADEA_SUFFIX)
XORG_FONT_TTF_CALADEA_SOURCE	:= $(SRCDIR)/$(XORG_FONT_TTF_CALADEA).$(XORG_FONT_TTF_CALADEA_SUFFIX)
XORG_FONT_TTF_CALADEA_DIR	:= $(BUILDDIR)/$(XORG_FONT_TTF_CALADEA)
XORG_FONT_TTF_CALADEA_LICENSE	:= Apache-2.0

ifdef PTXCONF_XORG_FONT_TTF_CALADEA
$(STATEDIR)/xorg-fonts.targetinstall.post: $(STATEDIR)/xorg-font-ttf-caladea.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XORG_FONT_TTF_CALADEA_CONF_TOOL	:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-font-ttf-caladea.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-font-ttf-caladea.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-font-ttf-caladea.targetinstall:
	@$(call targetinfo)

	@mkdir -p $(XORG_FONTS_DIR_INSTALL)/truetype

	@find $(XORG_FONT_TTF_CALADEA_DIR) \
		-name "*.ttf" \
		| \
		while read file; do \
		install -m 644 $${file} $(XORG_FONTS_DIR_INSTALL)/truetype; \
	done

	@$(call install_init,  xorg-font-ttf-caladea)
	@$(call install_fixup, xorg-font-ttf-caladea,PRIORITY,optional)
	@$(call install_fixup, xorg-font-ttf-caladea,SECTION,base)
	@$(call install_fixup, xorg-font-ttf-caladea,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, xorg-font-ttf-caladea,DESCRIPTION,missing)

	@$(call install_alternative, xorg-font-ttf-caladea, 0, 0, 644, \
		/etc/fonts/conf.d/30-0-google-crosextra-caladea-fontconfig.conf)
	@$(call install_alternative, xorg-font-ttf-caladea, 0, 0, 644, \
		/etc/fonts/conf.d/62-google-crosextra-caladea-fontconfig.conf)

	@$(call install_finish, xorg-font-ttf-caladea)

	@$(call touch)

# vim: syntax=make
