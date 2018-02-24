# -*-makefile-*-
#
# Copyright (C) 2006 by Roland Hostettler
#               2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2010 Tim Sander <tim.sander@hbm.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DBUS) += dbus

#
# Paths and names
#
DBUS_VERSION	:= 1.10.2
DBUS_MD5	:= 2428919cc77b8d0028d65ee4d5dbef31
DBUS		:= dbus-$(DBUS_VERSION)
DBUS_SUFFIX	:= tar.gz
DBUS_URL	:= http://dbus.freedesktop.org/releases/dbus/$(DBUS).$(DBUS_SUFFIX)
DBUS_SOURCE	:= $(SRCDIR)/$(DBUS).$(DBUS_SUFFIX)
DBUS_DIR	:= $(BUILDDIR)/$(DBUS)
DBUS_LICENSE	:= AFL-2.1, GPL-2.0+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
DBUS_CONF_TOOL	:= autoconf
DBUS_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-silent-rules \
	--disable-static \
	--disable-compiler-coverage \
	--disable-ansi \
	--disable-verbose-mode \
	--disable-asserts \
	--disable-checks \
	--disable-xml-docs \
	--disable-doxygen-docs \
	--disable-ducktype-docs \
	--enable-abstract-sockets=yes \
	--$(call ptx/endis, PTXCONF_DBUS_SELINUX)-selinux \
	--disable-apparmor \
	--disable-libaudit \
	--enable-inotify \
	--disable-kqueue \
	--disable-console-owner-file \
	--$(call ptx/endis, PTXCONF_DBUS_SYSTEMD)-systemd \
	--with-dbus-user=messagebus \
	--disable-embedded-tests \
	--disable-modular-tests \
	--disable-tests \
	--enable-epoll \
	--$(call ptx/endis, PTXCONF_DBUS_X)-x11-autolaunch \
	--disable-stats \
	--$(call ptx/endis, PTXCONF_DBUS_SYSTEMD)-user-session \
	--$(call ptx/wwo, PTXCONF_DBUS_X)-x$(call ptx/ifdef PTXCONF_DBUS_X,=$(SYSROOT)/usr,) \
	--without-valgrind \
	--with-systemdsystemunitdir=/lib/systemd/system \
	--with-systemduserunitdir=/usr/lib/systemd/user

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dbus.targetinstall:
	@$(call targetinfo)

	@$(call install_init, dbus)
	@$(call install_fixup, dbus,PRIORITY,optional)
	@$(call install_fixup, dbus,SECTION,base)
	@$(call install_fixup, dbus,AUTHOR,"Roland Hostettler <r.hostettler@gmx.ch>")
	@$(call install_fixup, dbus,DESCRIPTION,missing)

	@$(call install_copy, dbus, 0, 0, 0755, -, \
		/usr/bin/dbus-daemon)
	@$(call install_copy, dbus, 0, 0, 0755, -, \
		/usr/bin/dbus-cleanup-sockets)
	@$(call install_copy, dbus, 0, 0, 0755, -, \
		/usr/bin/dbus-launch)
	@$(call install_copy, dbus, 0, 0, 0755, -, \
		/usr/bin/dbus-monitor)
	@$(call install_copy, dbus, 0, 0, 0755, -, \
		/usr/bin/dbus-run-session)
	@$(call install_copy, dbus, 0, 0, 0755, -, \
		/usr/bin/dbus-send)
	@$(call install_copy, dbus, 0, 0, 0755, -, \
		/usr/bin/dbus-uuidgen)
	@$(call install_copy, dbus, 0, 104, 4754, -, \
		/usr/libexec/dbus-daemon-launch-helper)

	@$(call install_lib, dbus, 0, 0, 0644, libdbus-1)

#	#
#	# install config files
#	#
	@$(call install_alternative, dbus, 0, 0, 0644, /usr/share/dbus-1/system.conf)
	@$(call install_alternative, dbus, 0, 0, 0644, /usr/share/dbus-1/session.conf)

#	#
#	# busybox init: start script
#	#

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_DBUS_STARTSCRIPT
	@$(call install_alternative, dbus, 0, 0, 0755, /etc/init.d/dbus)

ifneq ($(call remove_quotes,$(PTXCONF_DBUS_BBINIT_LINK)),)
	@$(call install_link, dbus, \
		../init.d/dbus, \
		/etc/rc.d/$(PTXCONF_DBUS_BBINIT_LINK))
endif
endif
endif
ifdef PTXCONF_DBUS_SYSTEMD_UNIT
	@$(call install_copy, dbus, 0, 0, 0644, -, \
		/lib/systemd/system/dbus.socket)
	@$(call install_link, dbus, ../dbus.socket, \
		/lib/systemd/system/sockets.target.wants/dbus.socket)
	@$(call install_link, dbus, ../dbus.socket, \
		/lib/systemd/system/dbus.target.wants/dbus.socket)

	@$(call install_copy, dbus, 0, 0, 0644, -, \
		/lib/systemd/system/dbus.service)
	@$(call install_link, dbus, ../dbus.service, \
		/lib/systemd/system/multi-user.target.wants/dbus.service)
endif
	@$(call install_finish, dbus)

	@$(call touch)

# vim: syntax=make
