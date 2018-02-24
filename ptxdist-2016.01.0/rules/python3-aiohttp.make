# -*-makefile-*-
#
# Copyright (C) 2015 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PYTHON3_AIOHTTP) += python3-aiohttp

#
# Paths and names
#
PYTHON3_AIOHTTP_VERSION	:= 0.20.0
PYTHON3_AIOHTTP_MD5	:= 5db14a019b10d80e9b7f438d99aec83b
PYTHON3_AIOHTTP		:= aiohttp-$(PYTHON3_AIOHTTP_VERSION)
PYTHON3_AIOHTTP_SUFFIX	:= tar.gz
PYTHON3_AIOHTTP_URL	:= https://pypi.python.org/packages/source/a/aiohttp/$(PYTHON3_AIOHTTP).$(PYTHON3_AIOHTTP_SUFFIX)
PYTHON3_AIOHTTP_SOURCE	:= $(SRCDIR)/$(PYTHON3_AIOHTTP).$(PYTHON3_AIOHTTP_SUFFIX)
PYTHON3_AIOHTTP_DIR	:= $(BUILDDIR)/$(PYTHON3_AIOHTTP)
PYTHON3_AIOHTTP_LICENSE	:= Apache-2.0

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PYTHON3_AIOHTTP_CONF_TOOL	:= python3

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python3-aiohttp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, python3-aiohttp)
	@$(call install_fixup, python3-aiohttp,PRIORITY,optional)
	@$(call install_fixup, python3-aiohttp,SECTION,base)
	@$(call install_fixup, python3-aiohttp,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, python3-aiohttp,DESCRIPTION,missing)

	@for file in `find $(PYTHON3_AIOHTTP_PKGDIR)/usr/lib/python$(PYTHON3_MAJORMINOR)/site-packages/aiohttp  \
			! -type d ! -name "*.py" -printf "%P\n"`; do \
		$(call install_copy, python3-aiohttp, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON3_MAJORMINOR)/site-packages/aiohttp/$$file); \
	done

	@$(call install_finish, python3-aiohttp)

	@$(call touch)

# vim: syntax=make
