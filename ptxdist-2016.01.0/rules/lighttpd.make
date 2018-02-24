# -*-makefile-*-
#
# Copyright (C) 2007 by Daniel Schnell
#		2008, 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIGHTTPD) += lighttpd

#
# Paths and names
#
LIGHTTPD_VERSION	:= 1.4.39
LIGHTTPD_MD5		:= 63c7563be1c7a7a9819a51f07f1af8b2
LIGHTTPD		:= lighttpd-$(LIGHTTPD_VERSION)
LIGHTTPD_SUFFIX		:= tar.xz
LIGHTTPD_URL		:= http://download.lighttpd.net/lighttpd/releases-1.4.x/$(LIGHTTPD).$(LIGHTTPD_SUFFIX)
LIGHTTPD_SOURCE		:= $(SRCDIR)/$(LIGHTTPD).$(LIGHTTPD_SUFFIX)
LIGHTTPD_DIR		:= $(BUILDDIR)/$(LIGHTTPD)
LIGHTTPD_LICENSE	:= BSD-3-Clause

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIGHTTPD_CONF_TOOL	:= autoconf
LIGHTTPD_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--runstatedir=/run \
	--libdir=/usr/lib/lighttpd \
	--$(call ptx/endis, PTXCONF_GLOBAL_LARGE_FILE)-lfs \
	$(GLOBAL_IPV6_OPTION) \
	--disable-mmap \
	--without-libev \
	--without-mysql \
	--without-ldap \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_ATTR)-attr \
	--without-valgrind \
	--without-libunwind \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_OPENSSL)-openssl \
	--without-kerberos5 \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_PCRE)-pcre \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_ZLIB)-zlib \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_BZ2LIB)-bzip2 \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_FAM)-fam \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_WEBDAV_PROPS)-webdav-props \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_WEBDAV_LOCKS)-webdav-locks \
	--without-gdbm \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_MEMCACHE)-memcache \
	--$(call ptx/wwo, PTXCONF_LIGHTTPD_LUA)-lua

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

LIGHTTPD_MODULES-y :=
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_ACCESS)		+= mod_access
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_ACCESSLOG)	+= mod_accesslog
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_ALIAS)		+= mod_alias
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_AUTH)		+= mod_auth
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_CML)		+= mod_cml
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_COMPRESS)	+= mod_compress
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_FASTCGI)	+= mod_fastcgi
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_MAGNET)		+= mod_magnet
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_REWRITE)	+= mod_rewrite
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_TRIGGER_B4_DL)	+= mod_trigger_b4_dl
LIGHTTPD_MODULES-$(PTXCONF_LIGHTTPD_MOD_WEBDAV)		+= mod_webdav
LIGHTTPD_MODULES-y += $(call remove_quotes,$(PTXCONF_LIGHTTPD_MOD_EXTRA))

LIGHTTPD_MODULE_STRING := $(subst $(space),$(comma),$(addsuffix \",$(addprefix \",$(LIGHTTPD_MODULES-y))))

# add modules that are always loaded
LIGHTTPD_MODULES_INSTALL := mod_indexfile mod_dirlisting mod_staticfile $(LIGHTTPD_MODULES-y)

$(STATEDIR)/lighttpd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, lighttpd)
	@$(call install_fixup, lighttpd,PRIORITY,optional)
	@$(call install_fixup, lighttpd,SECTION,base)
	@$(call install_fixup, lighttpd,AUTHOR,"Daniel Schnell <danielsch@marel.com>")
	@$(call install_fixup, lighttpd,DESCRIPTION,missing)

#	# bins
	@$(call install_copy, lighttpd, 0, 0, 0755, -, \
		/usr/sbin/lighttpd)
	@$(call install_copy, lighttpd, 0, 0, 0755, -, \
		/usr/sbin/lighttpd-angel)

ifdef PTXCONF_LIGHTTPD_INSTALL_SELECTED_MODULES
	@$(foreach mod,$(LIGHTTPD_MODULES_INSTALL), \
		$(call install_lib, lighttpd, 0, 0, 0644, lighttpd/$(mod));)
else
#	# modules
	@$(call install_tree, lighttpd, 0, 0, -, /usr/lib/lighttpd)
endif

#	#
#	# configs
#	#
	@$(call install_alternative, lighttpd, 0, 0, 0644, \
		/etc/lighttpd/lighttpd.conf)
	@$(call install_copy, lighttpd, 0, 0, 0755, /etc/lighttpd/conf.d)
	@$(call install_replace, lighttpd, /etc/lighttpd/lighttpd.conf, \
		@MODULES@, $(LIGHTTPD_MODULE_STRING))

ifdef PTXCONF_LIGHTTPD_MOD_FASTCGI_PHP
	@$(call install_alternative, lighttpd, 0, 0, 0644, \
		/etc/lighttpd/conf.d/mod_fastcgi_php.conf)
endif

#	#
#	# busybox init: start script
#	#

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_LIGHTTPD_STARTSCRIPT
	@$(call install_alternative, lighttpd, 0, 0, 0755, /etc/init.d/lighttpd)

ifneq ($(call remove_quotes, $(PTXCONF_LIGHTTPD_BBINIT_LINK)),)
	@$(call install_link, lighttpd, \
		../init.d/lighttpd, \
		/etc/rc.d/$(PTXCONF_LIGHTTPD_BBINIT_LINK))
endif
endif
endif

ifdef PTXCONF_LIGHTTPD_SYSTEMD_UNIT
	@$(call install_alternative, lighttpd, 0, 0, 0644, \
		/lib/systemd/system/lighttpd.service)
	@$(call install_link, lighttpd, ../lighttpd.service, \
		/lib/systemd/system/multi-user.target.wants/lighttpd.service)
endif

ifdef PTXCONF_LIGHTTPD_GENERIC_SITE
ifdef PTXCONF_LIGHTTPD_MOD_FASTCGI_PHP
	@$(call install_copy, lighttpd, 12, 102, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/lighttpd.html, \
		/var/www/index.html)

	@$(call install_copy, lighttpd, 12, 102, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/bottles.php, \
		/var/www/bottles.php)

	@$(call install_copy, lighttpd, 12, 102, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/more_bottles.php, \
		/var/www/more_bottles.php)
else
	@$(call install_copy, lighttpd, 12, 102, 0644, \
		$(PTXDIST_TOPDIR)/projectroot/var/www/httpd.html, \
		/var/www/index.html)
endif
endif

	@$(call install_finish, lighttpd)
	@$(call touch)

# vim: syntax=make
