#!/bin/bash

main() {
    while getopts "d:s:" opt; do
	case "${opt}" in
	    d)
		local deb="${OPTARG}"
		;;
	    s)
		local src="${OPTARG}"
	esac
    done

    if [ -z "${deb}" -o -z "${src}" ]; then
	exit 1
    fi

    local ifs_orig="${IFS}"
    IFS="_"
    set -- ${deb}
    IFS="${ifs_orig}"

    local package="${1##*/}"
    local inst="${1%${package}}"
    local version="${2}"
    local arch="${3%%.deb}"

    local prefix="opt/${src##*/opt/}"
    local destdir="${src%${prefix}}"
    local debian_tmp="${inst}/${package}-temp"
    if [ -e "${debian_tmp}" ]; then
	rm -rf "${debian_tmp}"
    fi

    mkdir -p "${debian_tmp}"/{tmp,debian}

    cat <<EOF > "${debian_tmp}/debian/control"
Source: oselas.toolchain
Maintainer: Pengutronix <ptxdist@pengutronix.de>
Uploaders: $(git config user.name) <$(git config user.email)>
Section: devel
Priority: optional
Origin: pengutronix
Homepage: http://www.pengutronix.de/oselas/toolchain/
Bugs: mailto:bugs@pengutronix.de

Package: ${package}
Architecture: ${arch}
Depends: \${shlibs:Depends}
Description: ${package}
EOF
    cat <<EOF > "${debian_tmp}/debian/changelog"
oselas.toolchain (${version}) unstable; urgency=low

  * New upstream release.

 -- $(git config user.name) <$(git config user.email)>  $(date -R)
EOF
    cat <<EOF > "${debian_tmp}/debian/compat"
9
EOF
    # copy data
    echo "Copy data..."
    tar -C "${destdir}" --exclude=gcc-first -c "${prefix}" | tar -C "${debian_tmp}/tmp" -x

    (
	cd "${debian_tmp}"

	# generate dependencies
	echo "Generate dependencies..."
	local sysroot="$(ptxd_get_ptxconf PTXCONF_SYSROOT_TARGET)"
	sysroot="${debian_tmp}/tmp${sysroot#${PTX_AUTOBUILD_DESTDIR}}"
	local other="$(ptxd_get_ptxconf PTXCONF_SYSROOT_CROSS)"
	other="${debian_tmp}/tmp${other#${PTX_AUTOBUILD_DESTDIR}}/$(ptxd_get_ptxconf PTXCONF_GNU_TARGET)"

	fakeroot dh_shlibdeps \
	    -P"${debian_tmp}/tmp" \
	    -X/$(ptxd_get_ptxconf PTXCONF_GNU_TARGET)/lib/ \
	    -X/sysroot-$(ptxd_get_ptxconf PTXCONF_GNU_TARGET)/ \
	    -- \
	    -T"${debian_tmp}/substvars" &&

	# generate DEBIAN/control
	dpkg-gencontrol \
	    -P"${debian_tmp}/tmp" \
	    -T"${debian_tmp}/substvars"
    ) || exit

    # make a deb out of it
    echo dpkg-deb --build -Zxz "${debian_tmp}/tmp" "${deb}" | fakeroot

    rm -rf "${debian_tmp}"
}


main "${@}"
exit $?
