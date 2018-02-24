# -*-makefile-*-
#
# Copyright (C) 2009 by Michael Olbrich <m.olbrich@pengutronix.de>
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBEVENT) += libevent

#
# Paths and names
#
LIBEVENT_VERSION	:= 2.0.21
LIBEVENT_MD5		:= b2405cc9ebf264aa47ff615d9de527a2
LIBEVENT		:= libevent-$(LIBEVENT_VERSION)-stable
LIBEVENT_SUFFIX		:= tar.gz
LIBEVENT_URL		:= https://github.com/downloads/libevent/libevent/$(LIBEVENT).$(LIBEVENT_SUFFIX)
LIBEVENT_SOURCE		:= $(SRCDIR)/$(LIBEVENT).$(LIBEVENT_SUFFIX)
LIBEVENT_DIR		:= $(BUILDDIR)/$(LIBEVENT)
LIBEVENT_LICENSE	:= BSD-3-Clause, 0BSD

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBEVENT_CONF_TOOL	:= autoconf
LIBEVENT_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-thread-support \
	--disable-malloc-replacement \
	--$(call ptx/endis, PTXCONF_LIBEVENT_OPENSSL)-openssl \
	--disable-debug-mode \
	--enable-libevent-install \
	--disable-libevent-regress

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libevent.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libevent)
	@$(call install_fixup, libevent,PRIORITY,optional)
	@$(call install_fixup, libevent,SECTION,base)
	@$(call install_fixup, libevent,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, libevent,DESCRIPTION,missing)

	@$(call install_lib, libevent, 0, 0, 0644, libevent-2.0)
	@$(call install_lib, libevent, 0, 0, 0644, libevent_core-2.0)
	@$(call install_lib, libevent, 0, 0, 0644, libevent_extra-2.0)
	@$(call install_lib, libevent, 0, 0, 0644, libevent_pthreads-2.0)
ifdef PTXCONF_LIBEVENT_OPENSSL
	@$(call install_lib, libevent, 0, 0, 0644, libevent_openssl-2.0)
endif

	@$(call install_finish, libevent)

	@$(call touch)

# vim: syntax=make
