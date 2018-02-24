# -*-makefile-*-
#
# Copyright (C) 2006 by Robert Schwebel
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2013 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GLIBC_HEADERS) += glibc-headers

#
# Paths and names
#
GLIBC_HEADERS_DIR	= $(BUILDDIR)/$(GLIBC)-headers-build

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

GLIBC_HEADERS_PATH := PATH=$(CROSS_PATH)
#
# these various env variables are necessary, because we are using the host compiler
# it doesn't matter if we define ppc stuff during arm build
# they aren't tested :)
# no ifdefs for simplicity
#
GLIBC_HEADERS_ENV  := \
	$(HOST_ENV) \
	CC="gcc $(PTXCONF_GLIBC_HEADERS_FAKE_CROSS)" \
	\
	libc_cv_prog_as_gnu=yes \
	libc_cv_prog_ld_gnu=yes \
	ac_cv_prog_AS="$(PTXCONF_SYSROOT_CROSS)/bin/$(COMPILER_PREFIX)as" \
	ac_cv_prog_LD="$(PTXCONF_SYSROOT_CROSS)/bin/$(COMPILER_PREFIX)ld.bfd" \
	\
	libc_cv_asm_cfi_directive_sections=yes \
	libc_cv_asm_cfi_directives=yes \
	libc_cv_asm_cfi_personality=yes \
	libc_cv_asm_protected_directive=yes \
	libc_cv_asm_symver_directive=yes \
	libc_cv_broken_alias_attribute=no \
	libc_cv_broken_visibility_attribute=no \
	libc_cv_forced_unwind=yes \
	libc_cv_gcc___thread=yes \
	libc_cv_initfini_array=yes \
	libc_cv_visibility_attribute=yes \
	libc_cv_z_initfirst=yes \
	libc_cv_z_nodelete=yes \
	libc_cv_z_nodlopen=yes \
	libc_cv_z_relro=yes \
	libc_cv_predef_stack_protector=no \
	\
	libc_cv_386_tls=yes \
	\
	libc_cv_x86_64_tls=yes \
	\
	libc_cv_arm_tls=yes \
	\
	libc_cv_mips_tls=yes \
	libc_cv_have_sdata_section=yes \
	\
	libc_cv_powerpc32_tls=yes \
	libc_cv_powerpc64_tls=yes \
	libc_cv_mlong_double_128ibm=set \
	libc_cv_mlong_double_128=set \
	libc_cv_ppc_machine=yes \
	libc_cv_ppc_rel16=yes \
	\
	ac_cv_header_cpuid_h=yes \
	ac_cv_path_GREP=grep

#
# autoconf
#
GLIBC_HEADERS_CONF_TOOL	:= autoconf
GLIBC_HEADERS_CONF_OPT	 = \
	$(GLIBC_AUTOCONF_COMMON) \
	\
	--enable-hacker-mode

$(STATEDIR)/glibc-headers.prepare: $(STATEDIR)/glibc.extract
	@$(call targetinfo)
	@$(call clean, $(GLIBC_HEADERS_DIR))
	mkdir -p $(GLIBC_HEADERS_DIR)
	cd $(GLIBC_HEADERS_DIR) && \
		$(GLIBC_HEADERS_PATH) $(GLIBC_HEADERS_ENV) \
		$(GLIBC_DIR)/configure $(GLIBC_HEADERS_CONF_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/glibc-headers.compile:
	@$(call targetinfo)
	cd $(GLIBC_HEADERS_DIR) && \
		$(GLIBC_HEADERS_PATH) $(GLIBC_HEADERS_ENV) \
		$(MAKE) sysdeps/gnu/errlist.c

	mkdir -p $(GLIBC_HEADERS_DIR)/stdio-common
	touch $(GLIBC_HEADERS_DIR)/stdio-common/errlist-compat.c
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/glibc-headers.install:
	@$(call targetinfo)
	cd $(GLIBC_HEADERS_DIR) && \
		$(GLIBC_HEADERS_PATH) $(GLIBC_HEADERS_ENV) \
		$(MAKE) cross_compiling=yes install_root=$(SYSROOT) install-headers

	mkdir -p $(SYSROOT)/usr/include/gnu
	touch $(SYSROOT)/usr/include/gnu/stubs.h

	cp $(GLIBC_DIR)/include/features.h $(SYSROOT)/usr/include/features.h
	cp $(GLIBC_HEADERS_DIR)/bits/stdio_lim.h $(SYSROOT)/usr/include/bits/stdio_lim.h
	@$(call touch)

# vim: syntax=make
