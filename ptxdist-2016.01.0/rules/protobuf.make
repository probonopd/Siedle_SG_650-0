# -*-makefile-*-
#
# Copyright (C) 2012 by Adrian Baumgarth <adrian.baumgarth@l-3com.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PROTOBUF) += protobuf

#
# Paths and names
#
PROTOBUF_VERSION	:= 2.5.0
PROTOBUF_MD5		:= a72001a9067a4c2c4e0e836d0f92ece4
PROTOBUF		:= protobuf-$(PROTOBUF_VERSION)
PROTOBUF_SUFFIX		:= tar.bz2
PROTOBUF_URL		:= http://protobuf.googlecode.com/files/$(PROTOBUF).$(PROTOBUF_SUFFIX)
PROTOBUF_SOURCE		:= $(SRCDIR)/$(PROTOBUF).$(PROTOBUF_SUFFIX)
PROTOBUF_DIR		:= $(BUILDDIR)/$(PROTOBUF)
PROTOBUF_LICENSE	:= BSD-3-Clause

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PROTOBUF_CONF_ENV	:= \
	$(CROSS_ENV)

ifdef PTXCONF_ARCH_PPC
# protobuf-2.5.0 has no atomics for PPC
# abuse PTHREAD_* because those flags a propagated via protobuf.pc
PROTOBUF_CONF_ENV	+= \
	PTHREAD_LIBS="-lpthread" \
	PTHREAD_CFLAGS="-DGOOGLE_PROTOBUF_NO_THREAD_SAFETY"
endif

#
# autoconf
#
PROTOBUF_CONF_TOOL	:= autoconf
PROTOBUF_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-static \
	--$(call ptx/wwo, PTXCONF_PROTOBUF_ZLIB)-zlib \
	--with-protoc=$(PTXDIST_SYSROOT_HOST)/bin/protoc

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/protobuf.targetinstall:
	@$(call targetinfo)

	@$(call install_init, protobuf)
	@$(call install_fixup, protobuf,PRIORITY,optional)
	@$(call install_fixup, protobuf,SECTION,base)
	@$(call install_fixup, protobuf,AUTHOR,"Adrian Baumgarth <adrian.baumgarth@l-3com.com>")
	@$(call install_fixup, protobuf,DESCRIPTION,missing)

	@$(call install_lib, protobuf, 0, 0, 0644, libprotobuf-lite)
	@$(call install_lib, protobuf, 0, 0, 0644, libprotobuf)

	@$(call install_finish, protobuf)

	@$(call touch)

# vim: syntax=make
