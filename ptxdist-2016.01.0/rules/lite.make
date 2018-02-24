# -*-makefile-*-
#
# Copyright (C) 2007 by Denis Oliver Kropp
#               2010 by Marc Kleine-Budde <mkl@penugtronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LITE) += lite

#
# Paths and names
#
LITE_VERSION	:= 0.8.10
LITE_MD5	:= b9a28007d38c2b1c51ba1c3841df29ad
LITE		:= LiTE-$(LITE_VERSION)
LITE_SUFFIX	:= tar.gz
LITE_URL	:= http://www.directfb.org/downloads/Libs/$(LITE).$(LITE_SUFFIX)
LITE_SOURCE	:= $(SRCDIR)/$(LITE).$(LITE_SUFFIX)
LITE_DIR	:= $(BUILDDIR)/$(LITE)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LITE_CONF_TOOL := autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lite.targetinstall:
	@$(call targetinfo)

	@$(call install_init, lite)
	@$(call install_fixup, lite,PRIORITY,optional)
	@$(call install_fixup, lite,SECTION,base)
	@$(call install_fixup, lite,AUTHOR,"Denis Oliver Kropp <dok@directfb.org>")
	@$(call install_fixup, lite,DESCRIPTION,missing)

	@$(call install_lib, lite, 0, 0, 0644, liblite)
	@$(call install_lib, lite, 0, 0, 0644, libleck)

	@for i in \
		cursor \
		links \
		obenlinks \
		oben \
		obenrechts \
		rechts \
		untenlinks \
		unten \
		untenrechts; \
	do \
		$(call install_copy, lite, 0, 0, 0644, -, \
			/usr/share/LiTE/$$i.png) \
	done

	@for i in \
		lite_bench		\
		lite_checktest		\
		lite_dfbspy		\
		lite_listtest		\
		lite_msgbox		\
		lite_progressbar	\
		lite_run		\
		lite_scrollbartest	\
		lite_simple		\
		lite_slider		\
		lite_textbuttontest	\
		lite_textlinetest	\
		lite_textlisttest;	\
	do				\
		$(call install_copy, lite, 0, 0, 0755, -, \
			/usr/bin/$$i) \
	done


	@for i in \
		checkbox_images.png		\
		D.png				\
		stop.png			\
		stop_disabled.png		\
		stop_highlighted.png		\
		stop_pressed.png		\
		textbuttonbgnd.png		\
		toggle.png			\
		toggle_disabled.png		\
		toggle_highlighted.png		\
		toggle_pressed.png		\
		toggle_highlighted_on.png	\
		toggle_disabled_on.png		\
		progress.png			\
		progress_bg.png			\
		scrollbar.png;			\
	do					\
						\
		$(call install_copy, lite, 0, 0, 0644, -, \
			/usr/share/LiTE/examples/$$i, n) \
	done


	@$(call install_copy, lite, 0, 0, 0644, -, \
		/usr/share/fonts/truetype/vera.ttf, n)

	@$(call install_copy, lite, 0, 0, 0644, -, \
		/usr/share/fonts/truetype/verabd.ttf, n)

	@$(call install_copy, lite, 0, 0, 0644, -, \
		/usr/share/fonts/truetype/verabi.ttf, n)

	@$(call install_copy, lite, 0, 0, 0644, -, \
		/usr/share/fonts/truetype/verai.ttf, n)

	@$(call install_copy, lite, 0, 0, 0644, -, \
		/usr/share/fonts/truetype/decker.ttf, n)

	@$(call install_copy, lite, 0, 0, 0644, -, \
		/usr/share/fonts/truetype/whiterabbit.ttf, n)

	@$(call install_finish, lite)

	@$(call touch)

# vim: syntax=make
