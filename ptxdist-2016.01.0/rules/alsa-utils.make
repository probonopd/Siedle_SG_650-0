# -*-makefile-*-
#
# Copyright (C) 2006 by Erwin Rol
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_ALSA_UTILS) += alsa-utils

#
# Paths and names
#
ALSA_UTILS_VERSION	:= 1.0.29
ALSA_UTILS_MD5		:= 6b289bf874c4c9a63f4b3973093dd404
ALSA_UTILS		:= alsa-utils-$(ALSA_UTILS_VERSION)
ALSA_UTILS_SUFFIX	:= tar.bz2
ALSA_UTILS_URL		:= \
	http://dl.ambiweb.de/mirrors/ftp.alsa-project.org/utils/$(ALSA_UTILS).$(ALSA_UTILS_SUFFIX) \
	ftp://ftp.alsa-project.org/pub/utils/$(ALSA_UTILS).$(ALSA_UTILS_SUFFIX)
ALSA_UTILS_SOURCE	:= $(SRCDIR)/$(ALSA_UTILS).$(ALSA_UTILS_SUFFIX)
ALSA_UTILS_DIR		:= $(BUILDDIR)/$(ALSA_UTILS)
ALSA_UTILS_LICENSE	:= GPL-2.0+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ALSA_UTILS_PATH	:= PATH=$(CROSS_PATH)
ALSA_UTILS_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
ALSA_UTILS_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	$(CROSS_ENV_AC_NCURSES) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-nls \
	--disable-rpath \
	--disable-alsatest \
	--disable-xmlto \
	--with-asound-state-dir=/etc

ifdef PTXCONF_ALSA_UTILS_SYSTEMD_UNIT
ALSA_UTILS_AUTOCONF += --with-systemdsystemunitdir=/lib/systemd/system
else
ALSA_UTILS_AUTOCONF += --without-systemdsystemunitdir
endif

ifdef PTXCONF_ALSA_UTILS_ALSAMIXER
ALSA_UTILS_AUTOCONF += \
	--enable-alsamixer \
	--with-curses=$(call ptx/ifdef,PTXCONF_NCURSES_WIDE_CHAR,ncursesw,ncurses)
else
ALSA_UTILS_AUTOCONF += \
	--disable-alsamixer \
	--without-curses
endif

ifdef PTXCONF_ALSA_UTILS_ALSALOOP
ALSA_UTILS_AUTOCONF += --enable-alsaloop
else
ALSA_UTILS_AUTOCONF += --disable-alsaloop
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/alsa-utils.targetinstall:
	@$(call targetinfo)

	@$(call install_init, alsa-utils)
	@$(call install_fixup, alsa-utils, PRIORITY, optional)
	@$(call install_fixup, alsa-utils, SECTION, base)
	@$(call install_fixup, alsa-utils, AUTHOR, "Erwin Rol <ero@pengutronix.de>")
	@$(call install_fixup, alsa-utils, DESCRIPTION, missing)

	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/sbin/alsactl)
	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/amidi)
	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/amixer)
	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/aplay)
#	# link arecord aplay
	@$(call install_link, alsa-utils, aplay, /usr/bin/arecord)

	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/iecset)
	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/aconnect)
	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/aplaymidi)
	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/arecordmidi)
	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/aseqdump)
	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/aseqnet)

ifdef PTXCONF_ALSA_UTILS_ALSAMIXER
	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/alsamixer)
endif
ifdef PTXCONF_ALSA_UTILS_ALSALOOP
	@$(call install_copy, alsa-utils, 0, 0, 0755, -, /usr/bin/alsaloop)
endif

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_ALSA_UTILS_STARTSCRIPT
	@$(call install_alternative, alsa-utils, 0, 0, 0755, /etc/init.d/alsa-utils, n)
ifneq ($(call remove_quotes,$(PTXCONF_ALSA_UTILS_BBINIT_LINK)),)
	@$(call install_link, alsa-utils, \
		../init.d/alsa-utils, \
		/etc/rc.d/$(PTXCONF_ALSA_UTILS_BBINIT_LINK))
endif
endif
endif
ifdef PTXCONF_ALSA_UTILS_SYSTEMD_UNIT
	@$(call install_copy, alsa-utils, 0, 0, 0644, -, \
		/lib/systemd/system/alsa-restore.service)
	@$(call install_link, alsa-utils, ../alsa-restore.service, \
		/lib/systemd/system/basic.target.wants/alsa-restore.service)
	@$(call install_copy, alsa-utils, 0, 0, 0644, -, \
		/lib/systemd/system/alsa-store.service)
	@$(call install_link, alsa-utils, ../alsa-store.service, \
		/lib/systemd/system/shutdown.target.wants/alsa-store.service)
endif

ifdef PTXCONF_ALSA_UTILS_ASOUND_STATE
	@$(call install_alternative, alsa-utils, 0, 0, 0644, \
		/etc/asound.state, n)
endif
	@$(call install_finish, alsa-utils)

	@$(call touch)

# vim: syntax=make

