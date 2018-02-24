#!/bin/bash

pattern="$1"
config=( $(find ptxconfigs/ -path "*${pattern}*.ptxconfig") )
shift

if [ ${#config[@]} -eq 0 ]; then
	echo "Could not find config for '${pattern}'!"
	exit 1
fi
if [ ${#config[@]} -gt 1 ]; then
	echo "'${pattern}' matches more than on config:"
	for cfg in "${config[@]}"; do
		echo -e "\t${cfg}"
	done
	exit 1
fi

target="$(basename "${config}")"
target="${target%.ptxconfig}"

target="gstate/${target//_/-}.build"
if [ $# -gt 0 ]; then
	exec "$(dirname $0)/build_all_v2.mk" "${target}" ARG="${*}"
else
	exec "$(dirname $0)/build_all_v2.mk" "${target}"
fi
