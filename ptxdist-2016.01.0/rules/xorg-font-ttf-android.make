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
PACKAGES-$(PTXCONF_XORG_FONT_TTF_ANDROID) += xorg-font-ttf-android

#
# Paths and names
#
XORG_FONT_TTF_ANDROID_VERSION		:= 4.4.4r2
XORG_FONT_TTF_ANDROID_MD5		:= 351432aab0853958b28daebf28f8a988
XORG_FONT_TTF_ANDROID			:= fonts-android_$(XORG_FONT_TTF_ANDROID_VERSION)
XORG_FONT_TTF_ANDROID_SUFFIX		:= orig.tar.xz
XORG_FONT_TTF_ANDROID_URL		:= http://snapshot.debian.org/archive/debian/20140920T101110Z/pool/main/f/fonts-android/$(XORG_FONT_TTF_ANDROID).$(XORG_FONT_TTF_ANDROID_SUFFIX)
XORG_FONT_TTF_ANDROID_SOURCE		:= $(SRCDIR)/$(XORG_FONT_TTF_ANDROID).$(XORG_FONT_TTF_ANDROID_SUFFIX)
XORG_FONT_TTF_ANDROID_DIR		:= $(BUILDDIR)/$(XORG_FONT_TTF_ANDROID)
XORG_FONT_TTF_ANDROID_STRIP_LEVEL	:= 0
XORG_FONT_TTF_ANDROID_LICENSE		:= Apache-2.0
XORG_FONT_TTF_ANDROID_LICENSE_FILES	:= \
	file://NOTICE;md5=9645f39e9db895a4aa6e02cb57294595

ifdef PTXCONF_XORG_FONT_TTF_ANDROID
$(STATEDIR)/xorg-fonts.targetinstall.post: $(STATEDIR)/xorg-font-ttf-android.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
XORG_FONT_TTF_ANDROID_CONF_TOOL	:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-font-ttf-android.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-font-ttf-android.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xorg-font-ttf-android.targetinstall:
	@$(call targetinfo)

	@mkdir -p $(XORG_FONTS_DIR_INSTALL)/truetype

	@find $(XORG_FONT_TTF_ANDROID_DIR) \
		-name "*.ttf" \
		| \
		while read file; do \
		install -m 644 $${file} $(XORG_FONTS_DIR_INSTALL)/truetype; \
	done

	@$(call touch)

# vim: syntax=make
