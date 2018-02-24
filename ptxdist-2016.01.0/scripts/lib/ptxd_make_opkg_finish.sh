#!/bin/bash
#
# Copyright (C) 2005, 2006, 2007 Robert Schwebel <r.schwebel@pengutronix.de>
#               2008, 2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2011 by George McCollister <george.mccollister@gmail.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# the actual opkg package creation, will run in fakeroot
#
ptxd_make_opkg_finish_impl() {
    chown -R 0:0 "${pkg_xpkg_tmp}" &&
    ptxd_make_xpkg_pkg "${pkg_opkg_tmp}" "${pkg_xpkg_cmds}" "${pkg_xpkg_perms}" &&
    opkg-build ${ptx_xpkg_extra_args} "${pkg_opkg_tmp}" "${ptx_pkg_dir}"
}
export -f ptxd_make_opkg_finish_impl

