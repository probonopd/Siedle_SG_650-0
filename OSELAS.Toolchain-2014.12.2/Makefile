version		:= $(shell ./scripts/setlocalversion ./.tarball-version)
project		:= OSELAS.Toolchain-$(version)
v		:= v$(version)

all:
	@echo "Please use 'build_all_v2.mk'"

dirty-check:
	@case "$(version)" in \
		*-dirty) echo "refusing to install or package a dirty git tree!" ; exit 1 ;; \
	esac

dist: dirty-check
	rm -rf "$(project)" "$(project).tar.bz2"
	git archive "$(v)" --prefix="$(project)"/ > "${project}.tar"
	mkdir -p "${project}"
	echo -n "${version}" > "${project}/.tarball-version"

	tar -rf "${project}.tar" \
		"${project}/.tarball-version"
	bzip2 "${project}.tar"
	md5sum "${project}.tar.bz2" > "${project}.tar.bz2.md5"
	rm -rf "${project}"

release:
	@scripts/make_$@.sh
