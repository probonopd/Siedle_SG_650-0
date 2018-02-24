# -*-makefile-*-
#
# Copyright (C) 2003 by wschmitt@envicomp.de
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_RSYNC3) += rsync3

#
# Paths and names
#
RSYNC3_VERSION	:= 3.0.5
RSYNC3_MD5	:= a130e736c011572cb423b6245e97fc4b
RSYNC3		:= rsync-$(RSYNC3_VERSION)
RSYNC3_SUFFIX	:= tar.gz
RSYNC3_URL	:= http://rsync.samba.org/ftp/rsync/src/$(RSYNC3).$(RSYNC3_SUFFIX)
RSYNC3_SOURCE	:= $(SRCDIR)/$(RSYNC3).$(RSYNC3_SUFFIX)
RSYNC3_DIR	:= $(BUILDDIR)/$(RSYNC3)
RSYNC3_LICENSE	:= GPL-3.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

RSYNC3_PATH	:= PATH=$(CROSS_PATH)
RSYNC3_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
RSYNC3_AUTOCONF  := \
	 $(CROSS_AUTOCONF_USR) \
	$(GLOBAL_IPV6_OPTION) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--$(call ptx/endis, PTXCONF_RSYNC3_ACL)-acl-support \
	--$(call ptx/endis, PTXCONF_RSYNC3_ATTR)-xattr-support \
	--with-included-popt \
	--disable-debug \
	--disable-locale

ifneq ($(call remove_quotes,$(PTXCONF_RSYNC3_CONFIG_FILE)),)
RSYNC3_AUTOCONF += --with-rsync3d-conf=$(PTXCONF_RSYNC3_CONFIG_FILE)
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/rsync3.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  rsync3)
	@$(call install_fixup, rsync3,PRIORITY,optional)
	@$(call install_fixup, rsync3,SECTION,base)
	@$(call install_fixup, rsync3,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, rsync3,DESCRIPTION,missing)

	@$(call install_copy, rsync3, 0, 0, 0755, -, \
		/usr/bin/rsync)

	@$(call install_alternative, rsync3, 0, 0, 0644, /etc/rsyncd.conf, n)
	@$(call install_alternative, rsync3, 0, 0, 0640, /etc/rsyncd.secrets, n)

	#
	# busybox init
	#

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_RSYNC3_STARTSCRIPT
	@$(call install_alternative, rsync3, 0, 0, 0755, /etc/init.d/rsyncd, n)

ifneq ($(call remove_quotes,$(PTXCONF_RSYNC3_BBINIT_LINK)),)
	@$(call install_link, rsync3, \
		../init.d/rsyncd, \
		/etc/rc.d/$(PTXCONF_RSYNC3_BBINIT_LINK))
endif
endif
endif
	@$(call install_finish, rsync3)
	@$(call touch)

# vim: syntax=make
