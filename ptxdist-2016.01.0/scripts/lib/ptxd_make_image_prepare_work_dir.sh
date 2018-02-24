#!/bin/bash
#
# Copyright (C) 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2011 by George McCollister <george.mccollister@gmail.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#


#
# ptxd_make_image_extract_ipkg_files - extract ipkg for later image generation
#
# in:
# - $1				directory where ipkg are extracted
# - $PTXDIST_IPKG_ARCH_STRING	ARCH variable for ipkg files
#
# out:
# - $image_permissions		file containing all permissions
#
ptxd_make_image_extract_xpkg_files() {
    # FIXME: consolidate "ptxd_install_setup_src"
    local src="/etc/ipkg.conf"
    local xpkg_conf="${PTXDIST_TEMPDIR}/${FUNCNAME}_xpkg.conf"
    local work_dir="$1"
    local -a list ptxd_reply
    if ptxd_get_ptxconf "PTXCONF_HOST_PACKAGE_MANAGEMENT_OPKG" > /dev/null; then
	echo "option force_postinstall 1" > "${xpkg_conf}"
	src="/etc/opkg/opkg.conf"
    else
	src="/etc/ipkg.conf"
    fi
    list=( \
	"${PTXDIST_WORKSPACE}/projectroot${PTXDIST_PLATFORMSUFFIX}${src}" \
	"${PTXDIST_WORKSPACE}/projectroot${src}${PTXDIST_PLATFORMSUFFIX}" \
	"${PTXDIST_WORKSPACE}/projectroot${src}" \
	"${PTXDIST_TOPDIR}/projectroot${src}" \
	)

    if ! ptxd_get_path "${list[@]}"; then
	local IFS="
"
	ptxd_bailout "
unable to find '${src}'

These location have been searched:
${list[*]}
"
    fi

    rm -rf "${work_dir}" &&
    mkdir -p "${work_dir}" &&

    ARCH="${PTXDIST_IPKG_ARCH_STRING}" \
    SRC="" \
    CHECKSIG="" \
    CAPATH="" \
    CAFILE="" \
	ptxd_replace_magic "${ptxd_reply}" >> "${xpkg_conf}" &&

    DESTDIR="${work_dir}" \
	${ptx_xpkg_type} -f "${xpkg_conf}" -o "${work_dir}" \
	install "${ptxd_reply_ipkg_files[@]}" &&

    # fix directory permissions
    {
	echo "cd '${work_dir}' &&"
	ptxd_dopermissions "${ptxd_reply_perm_files[@]}"
	echo ":"
    } | sh &&

    ptxd_install_fixup_timestamps "${work_dir}"
}
export -f ptxd_make_image_extract_xpkg_files

#
# ptxd_make_image_prepare_work_dir_impl
#
# in:
# - $image_work_dir		directory where ipkg are extracted
# - $image_permissions		name of file that should contain all permissions
# - $image_pkgs_selected_target	space seperated list of selected
#
ptxd_make_image_prepare_work_dir_impl() {
    ptxd_make_image_init &&
    ptxd_get_ipkg_files ${image_pkgs_selected_target} &&
    ptxd_make_image_extract_xpkg_files "${image_work_dir}" &&
    if ! cat "${ptxd_reply_perm_files[@]}" > "${image_permissions}"; then
	echo "${PTXDIST_LOG_PROMPT}error: failed read permission files" >&2
	return 1
    fi
}
export -f ptxd_make_image_prepare_work_dir_impl

ptxd_make_image_prepare_work_dir() {
    fakeroot ptxd_make_image_prepare_work_dir_impl
}
export -f ptxd_make_image_prepare_work_dir
