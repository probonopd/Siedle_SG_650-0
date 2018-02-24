# -*-makefile-*-
#
# Copyright (C) 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PYTHON3) += python3

#
# Paths and names
#
PYTHON3_VERSION		:= 3.5.0
PYTHON3_MD5		:= d149d2812f10cbe04c042232e7964171
PYTHON3_MAJORMINOR	:= $(basename $(PYTHON3_VERSION))
PYTHON3_SITEPACKAGES	:= /usr/lib/python$(PYTHON3_MAJORMINOR)/site-packages
PYTHON3			:= Python-$(PYTHON3_VERSION)
PYTHON3_SUFFIX		:= tar.xz
PYTHON3_SOURCE		:= $(SRCDIR)/$(PYTHON3).$(PYTHON3_SUFFIX)
PYTHON3_DIR		:= $(BUILDDIR)/$(PYTHON3)

PYTHON3_URL		:= \
	http://python.org/ftp/python/$(PYTHON3_VERSION)/$(PYTHON3).$(PYTHON3_SUFFIX) \
	http://python.org/ftp/python/$(PYTHON3_MAJORMINOR)/$(PYTHON3).$(PYTHON3_SUFFIX)

CROSS_PYTHON3		:= $(PTXCONF_SYSROOT_CROSS)/bin/python$(PYTHON3_MAJORMINOR)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

# Note: the LDFLAGS are used by Python scripts
PYTHON3_ENV	:= \
	$(CROSS_ENV) \
	ac_sys_system=Linux \
	ac_sys_release=2 \
	MACHDEP=linux2 \
	ac_cv_have_chflags=no \
	ac_cv_have_lchflags=no \
	ac_cv_py_format_size_t=yes \
	ac_cv_broken_sem_getvalue=no \
	ac_cv_file__dev_ptmx=no \
	ac_cv_file__dev_ptc=no \
	ac_cv_working_tzset=yes \
	LDFLAGS="-L $(PTXDIST_SYSROOT_TARGET)/lib/ \
		 -L $(PTXDIST_SYSROOT_TARGET)/usr/lib/"


PYTHON3_BINCONFIG_GLOB := ""

#
# autoconf
#
PYTHON3_CONF_TOOL	:= autoconf
PYTHON3_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_IPV6_OPTION) \
	--enable-shared \
	--with-system-expat \
	--with-system-ffi \
	--with-signal-module \
	--with-threads=pthread \
	--without-doc-strings \
	--without-tsc \
	--with-pymalloc \
	--without-valgrind \
	--without-ensurepip

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python3.install:
	@$(call targetinfo)

#	# remove unneeded stuff
	@find $(PYTHON3_DIR) \( -name test -o -name tests \) -print0 | xargs -0 rm -vrf

	@$(call install, PYTHON3)

	@rm -vrf $(PYTHON3_PKGDIR)/usr/lib/python$(PYTHON3_MAJORMINOR)/config-$(PYTHON3_MAJORMINOR)m
	@$(call world/env, PYTHON3) ptxd_make_world_install_python_cleanup

	@$(call touch)

PYTHON3_PLATFORM := $(call remove_quotes,$(PTXCONF_ARCH_STRING))
ifdef PTXCONF_ARCH_ARM64
PYTHON3_PLATFORM := arm
endif

$(STATEDIR)/python3.install.post:
	@$(call targetinfo)
	@$(call world/install.post, PYTHON3)

	@rm -f "$(CROSS_PYTHON3)"
	@echo '#!/bin/sh'						>> "$(CROSS_PYTHON3)"
	@echo '_PYTHON_PROJECT_BASE=$(PYTHON3_DIR)'			>> "$(CROSS_PYTHON3)"
	@echo '_PYTHON_HOST_PLATFORM=linux2-$(PYTHON3_PLATFORM)'	>> "$(CROSS_PYTHON3)"
	@echo 'export _PYTHON_PROJECT_BASE _PYTHON_HOST_PLATFORM'	>> "$(CROSS_PYTHON3)"
	@echo 'exec $(HOSTPYTHON3) "$${@}"'				>> "$(CROSS_PYTHON3)"
	@chmod a+x "$(CROSS_PYTHON3)"
	@ln -sf "python$(PYTHON3_MAJORMINOR)" \
		"$(PTXCONF_SYSROOT_CROSS)/bin/python3"
	sed -e 's;prefix_real=.*;prefix_real=$(SYSROOT)/usr;' \
		"$(PTXCONF_SYSROOT_HOST)/bin/python$(PYTHON3_MAJORMINOR)-config" \
		> "$(PTXCONF_SYSROOT_CROSS)/bin/python$(PYTHON3_MAJORMINOR)-config"
	@chmod +x "$(PTXCONF_SYSROOT_CROSS)/bin/python$(PYTHON3_MAJORMINOR)-config"

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

# These cannot be disabled during build, so just don't install the disabled modules
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_NCURSES)	+= curses _curses*.so
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_SQLITE)	+= sqlite3
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_BZ2)		+= bz2.pyc _bz2*.so
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_LZMA)		+= lzma.pyc _lzma*.so
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_ZLIB)		+= gzip.pyc zlib*so
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_READLINE)	+= readline*so
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_DB)		+= dbm _dbm*so
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_SSL)		+= ssl.pyc _ssl*.so
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_LIBTK)		+= tkinter
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_IDLELIB)	+= idlelib
PYTHON3_SKIP-$(call ptx/opt-dis, PTXCONF_PYTHON3_DISTUTILS)	+= distutils

ifneq ($(PYTHON3_SKIP-y),)
PYTHON3_SKIP_LIST_PRE  :=-o -name $(quote)
PYTHON3_SKIP_LIST_POST :=$(quote)

PYTHON3_SKIP_LIST := $(subst $(space),$(PYTHON3_SKIP_LIST_POST) $(PYTHON3_SKIP_LIST_PRE),$(PYTHON3_SKIP-y))
PYTHON3_SKIP_LIST := $(PYTHON3_SKIP_LIST_PRE)$(PYTHON3_SKIP_LIST)$(PYTHON3_SKIP_LIST_POST)
endif

$(STATEDIR)/python3.targetinstall:
	@$(call targetinfo)

	@$(call install_init, python3)
	@$(call install_fixup, python3,PRIORITY,optional)
	@$(call install_fixup, python3,SECTION,base)
	@$(call install_fixup, python3,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, python3,AUTHOR,"Han Sierkstra <han@protonic.nl>")
	@$(call install_fixup, python3,DESCRIPTION,missing)

	@cd $(PYTHON3_PKGDIR) && \
		find ./usr/lib/python$(PYTHON3_MAJORMINOR) \
		\( -name test -o -name tests -o -name __pycache__ \
		$(PYTHON3_SKIP_LIST) \) -prune \
		-o -name "*.so" -print -o -name "*.pyc" -print | \
		while read file; do \
		$(call install_copy, python3, 0, 0, 644, -, $${file##.}); \
	done

	@$(call install_copy, python3, 0, 0, 755, -, /usr/bin/python$(PYTHON3_MAJORMINOR))
	@$(call install_lib, python3, 0, 0, 644, libpython$(PYTHON3_MAJORMINOR)m)

ifdef PTXCONF_PYTHON3_SYMLINK
	@$(call install_link, python3, python$(PYTHON3_MAJORMINOR), /usr/bin/python3)
endif

	@$(call install_finish, python3)

	@$(call touch)

# vim: syntax=make
