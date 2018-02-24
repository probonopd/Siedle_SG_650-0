#!/bin/bash
#
# Copyright (C) 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

ptxd_abs2rel() {
    local from from_parts to to_parts max orig_IFS

    [ ${#} -eq 2 ] || { echo "supply exactly two paths" >&2; exit 1; }

    from="${1}"
    to="${2}"

    orig_IFS="${IFS}"
    IFS="/"
    from_parts=(${from#/})
    to_parts=(${to#/})

    if [ ${#from_parts[@]} -gt ${#to_parts[@]} ]; then
	max=${#from_parts[@]}
    else
	max=${#to_parts[@]}
    fi

    for ((i = 0; i < ${max}; i++)); do
	from="${from_parts[i]}"
	to="${to_parts[i]}"

	if [ "${from}" = "${to}" ]; then
	    unset from_parts[$i]
	    unset to_parts[$i]
	elif [ -n "${from}" ]; then
	    from_parts[$i]=".."
	fi
    done

    echo "${from_parts[*]}${from_parts[*]:+/}${to_parts[*]}"
    IFS="${orig_IFS}"
}

ptxd_abs2rel "${@}"
