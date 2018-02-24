# -*-makefile-*-
#
# Copyright (C) 2011 by Robert Schwebel <r.schwebel@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_FLUP6) += flup6

#
# Paths and names
#
FLUP6_VERSION	:= 1.1
FLUP6_MD5	:= 67c20571a6e637b1f457031767fe8ae7
FLUP6		:= flup6-$(FLUP6_VERSION)
FLUP6_SUFFIX	:= tar.gz
FLUP6_URL	:= http://pypi.python.org/packages/source/f/flup6/$(FLUP6).$(FLUP6_SUFFIX)
FLUP6_SOURCE	:= $(SRCDIR)/$(FLUP6).$(FLUP6_SUFFIX)
FLUP6_DIR	:= $(BUILDDIR)/$(FLUP6)
FLUP6_LICENSE	:= BSD, MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

FLUP6_CONF_TOOL	:= python3

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/flup6.targetinstall:
	@$(call targetinfo)

	@$(call install_init, flup6)
	@$(call install_fixup, flup6,PRIORITY,optional)
	@$(call install_fixup, flup6,SECTION,base)
	@$(call install_fixup, flup6,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, flup6,DESCRIPTION,missing)

	@$(call install_copy, flup6, 0, 0, 0755, $(PYTHON_SITEPACKAGES))
	@$(call install_copy, flup6, 0, 0, 0755, $(PYTHON_SITEPACKAGES)/flup6)
	@$(call install_copy, flup6, 0, 0, 0755, $(PYTHON_SITEPACKAGES)/flup6/client)
	@$(call install_copy, flup6, 0, 0, 0755, $(PYTHON_SITEPACKAGES)/flup6/server)

	for file in $(shell cd $(FLUP6_PKGDIR) && find . -name "*.pyc"); \
	do \
		$(call install_copy, flup6, 0, 0, 0644, -, /$$file) \
	done

	@$(call install_finish, flup6)

	@$(call touch)

# vim: syntax=make
