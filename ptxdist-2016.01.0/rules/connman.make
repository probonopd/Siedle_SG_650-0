# -*-makefile-*-
#
# Copyright (C) 2009 by Robert Schwebel <r.schwebel@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CONNMAN) += connman

#
# Paths and names
#
CONNMAN_VERSION	:= 1.30
CONNMAN_MD5	:= 3e4006236e53b61c966213331df91f35
CONNMAN		:= connman-$(CONNMAN_VERSION)
CONNMAN_SUFFIX	:= tar.gz
CONNMAN_URL	:= $(call ptx/mirror, KERNEL, network/connman/$(CONNMAN).$(CONNMAN_SUFFIX))
CONNMAN_SOURCE	:= $(SRCDIR)/$(CONNMAN).$(CONNMAN_SUFFIX)
CONNMAN_DIR	:= $(BUILDDIR)/$(CONNMAN)
CONNMAN_LICENSE	:= GPL-2.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
CONNMAN_CONF_TOOL	:= autoconf
CONNMAN_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-debug \
	--disable-hh2serial-gps \
	--disable-openconnect \
	--disable-openvpn \
	--disable-vpnc \
	--disable-l2tp \
	--disable-pptp \
	--disable-iospm \
	--disable-tist \
	--disable-session-policy-local \
	--disable-test \
	--disable-nmcompat \
	--$(call ptx/endis, PTXCONF_CONNMAN_POLKIT)-polkit \
	--$(call ptx/endis, PTXCONF_GLOBAL_SELINUX)-selinux \
	--$(call ptx/endis, PTXCONF_CONNMAN_LOOPBACK)-loopback \
	--$(call ptx/endis, PTXCONF_CONNMAN_ETHERNET)-ethernet \
	--$(call ptx/endis, PTXCONF_CONNMAN_GADGET)-gadget \
	--$(call ptx/endis, PTXCONF_CONNMAN_WIFI)-wifi \
	--$(call ptx/endis, PTXCONF_CONNMAN_BLUETOOTH)-bluetooth \
	--disable-ofono \
	--disable-dundee \
	--disable-pacrunner \
	--disable-neard \
	--disable-wispr \
	--disable-tools \
	--$(call ptx/endis, PTXCONF_CONNMAN_CLIENT)-client \
	--enable-datafiles \
	--with-dbusconfdir=/usr/share \
	--with-systemdunitdir=/lib/systemd/system

CONNMAN_TESTS := \
	backtrace \
	connect-provider \
	disable-tethering \
	enable-tethering \
	get-global-timeservers \
	get-proxy-autoconfig \
	get-services \
	get-state \
	list-services \
	monitor-connman \
	monitor-services \
	monitor-vpn \
	p2p-on-supplicant \
	remove-provider \
	service-move-before \
	set-clock \
	set-domains \
	set-global-timeservers \
	set-ipv4-method \
	set-ipv6-method \
	set-nameservers \
	set-proxy \
	set-timeservers \
	show-introspection \
	simple-agent \
	test-clock \
	test-compat \
	test-connman \
	test-counter \
	test-manager \
	test-new-supplicant \
	test-session \
	vpn-connect \
	vpn-disconnect \
	vpn-get \
	vpn-property

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/connman.install:
	@$(call targetinfo)
	@$(call install, CONNMAN)
ifdef PTXCONF_CONNMAN_CLIENT
	install -D -m 755 "$(CONNMAN_DIR)/client/connmanctl" \
		"$(CONNMAN_PKGDIR)/usr/sbin/connmanctl"
endif
ifdef PTXCONF_CONNMAN_TESTS
	@$(foreach test, $(CONNMAN_TESTS), \
		install -D -m 755 "$(CONNMAN_DIR)/test/$(test)" \
			"$(CONNMAN_PKGDIR)/usr/sbin/cm-$(test)";)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/connman.targetinstall:
	@$(call targetinfo)

	@$(call install_init, connman)
	@$(call install_fixup, connman,PRIORITY,optional)
	@$(call install_fixup, connman,SECTION,base)
	@$(call install_fixup, connman,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, connman,DESCRIPTION,missing)

#	# binary
	@$(call install_copy, connman, 0, 0, 0755, -, /usr/sbin/connmand)

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_CONNMAN_STARTSCRIPT
	@$(call install_alternative, connman, 0, 0, 0755, /etc/init.d/connman, n)

ifneq ($(call remove_quotes, $(PTXCONF_CONNMAN_BBINIT_LINK)),)
	@$(call install_link, connman, \
		../init.d/connman, \
		/etc/rc.d/$(PTXCONF_CONNMAN_BBINIT_LINK))
endif
endif
endif
ifdef PTXCONF_CONNMAN_SYSTEMD_UNIT
	@$(call install_alternative, connman, 0, 0, 0644, \
		/lib/systemd/system/connman.service)
	@$(call install_link, connman, ../connman.service, \
		/lib/systemd/system/multi-user.target.wants/connman.service)
	@$(call install_alternative, connman, 0, 0, 0644, \
		/lib/systemd/system/connman-ignore.service)
	@$(call install_link, connman, ../connman-ignore.service, \
		/lib/systemd/system/connman.service.wants/connman-ignore.service)
	@$(call install_alternative, connman, 0, 0, 0755, \
		/lib/systemd/connman-ignore)
endif
ifdef PTXCONF_CONNMAN_POLKIT
	@$(call install_alternative, connman, 0, 0, 0644, \
		/usr/share/polkit-1/actions/net.connman.policy)
endif

#	# ship settings which enable wired interfaces per default
	@$(call install_alternative, connman, 0, 0, 0600, \
		/var/lib/connman/settings)

#	# dbus config
	@$(call install_alternative, connman, 0, 0, 0644, /usr/share/dbus-1/system.d/connman.conf)

#	# command line client
ifdef PTXCONF_CONNMAN_CLIENT
	@$(call install_copy, connman, 0, 0, 0755, -, /usr/sbin/connmanctl)
endif

#	# python tests
ifdef PTXCONF_CONNMAN_TESTS
	@$(foreach test, $(CONNMAN_TESTS), \
		$(call install_copy, connman, 0, 0, 0755, -, \
			/usr/sbin/cm-$(test));)
endif

	@$(call install_finish, connman)

	@$(call touch)

# vim: syntax=make
