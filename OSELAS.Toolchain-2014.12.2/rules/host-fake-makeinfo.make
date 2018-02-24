# -*-makefile-*-
#
# Copyright (C) 2013 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_FAKE_MAKEINFO) += host-fake-makeinfo

#
# Paths and names
#
HOST_FAKE_MAKEINFO	:= fake-makeinfo

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-fake-makeinfo.install:
	@$(call targetinfo)
	install -d $(HOST_FAKE_MAKEINFO_PKGDIR)/bin
	echo 'if [ "$${1}" == "--version" ]; then  echo "makeinfo (GNU texinfo) 5.2"; fi' > $(HOST_FAKE_MAKEINFO_PKGDIR)/bin/makeinfo
	chmod +x $(HOST_FAKE_MAKEINFO_PKGDIR)/bin/makeinfo
	@$(call touch)

# vim: syntax=make
