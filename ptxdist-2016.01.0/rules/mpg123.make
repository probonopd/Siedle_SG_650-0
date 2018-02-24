# -*-makefile-*-
#
# Copyright (C) 2010 by Juergen Kilb
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_MPG123) += mpg123

#
# Paths and names
#
MPG123_VERSION	:= 1.22.4
MPG123_MD5	:= 2dfafae3bbc532b4c8b04a77c6a6de89
MPG123		:= mpg123-$(MPG123_VERSION)
MPG123_SUFFIX	:= tar.bz2
MPG123_URL	:= http://www.mpg123.org/download/$(MPG123).$(MPG123_SUFFIX)
MPG123_SOURCE	:= $(SRCDIR)/$(MPG123).$(MPG123_SUFFIX)
MPG123_DIR	:= $(BUILDDIR)/$(MPG123)
MPG123_LICENSE	:= LGPL-2.1
MPG123_LICENSE_FILES := \
	file://COPYING;md5=1e86753638d3cf2512528b99079bc4f3

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
MPG123_CONF_TOOL	:= autoconf
MPG123_CONF_OPT		:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_IPV6_OPTION) \
	--with-audio=alsa\
	--with-default-audio=alsa\
	--enable-modules=no
# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/mpg123.targetinstall:
	@$(call targetinfo)

	@$(call install_init, mpg123)
	@$(call install_fixup, mpg123,PRIORITY,optional)
	@$(call install_fixup, mpg123,SECTION,base)
	@$(call install_fixup, mpg123,AUTHOR,"Juergen Kilb <J.Kilb@phytec.de>")
	@$(call install_fixup, mpg123,DESCRIPTION,missing)

	@$(call install_copy, mpg123, 0, 0, 0755, -, /usr/bin/mpg123)
	@$(call install_lib, mpg123, 0, 0, 0644, libmpg123)

	@$(call install_finish, mpg123)

	@$(call touch)

# vim: syntax=make
