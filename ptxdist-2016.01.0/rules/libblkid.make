# -*-makefile-*-
#
# Copyright (C) 2010 by Carsten Schlote <c.schlote@konzeptpark.de>
#           (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBBLKID) += libblkid

# ----------------------------------------------------------------------------
# Virtual fake package (spawned from util-linux)
# ----------------------------------------------------------------------------
LIBBLKID_LICENSE	:= LGPL-2.1+

# vim: syntax=make
