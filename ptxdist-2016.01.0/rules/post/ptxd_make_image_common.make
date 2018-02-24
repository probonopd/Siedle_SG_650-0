# -*-makefile-*-
#
# Copyright (C) 2003-2010 by the ptxdist project <ptxdist@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

DOPERMISSIONS := '{	\
	if ($$1 == "f")	\
		printf("chmod %s \".%s\"; chown %s.%s \".%s\";\n", $$5, $$2, $$3, $$4, $$2);	\
	if ($$1 == "n")	\
		printf("rm -f \".%s\"; mkdir -p \".`dirname \"%s\"`\"; mknod -m %s \".%s\" %s %s %s; chown %s.%s \".%s\";\n", $$2, $$2, $$5, $$2, $$6, $$7, $$8, $$3, $$4, $$2);}'

IMAGE_REPO_DIST_DIR := $(call ptx/escape,$(PTXCONF_SETUP_IPKG_REPOSITORY)/$(call remove_quotes,$(PTXCONF_PROJECT))/dists/$(call remove_quotes,$(PTXCONF_PROJECT))$(call remove_quotes,$(PTXCONF_PROJECT_VERSION)))

image/env = \
	$(call ptx/env) \
	image_pkgs_selected_target="$(call ptx/escape,$(PTX_PACKAGES_INSTALL))" \
	image_repo_dist_dir="$(IMAGE_REPO_DIST_DIR)" \
	\
	image_work_dir="$(call ptx/escape,$(image/work_dir))" \
	image_permissions="$(call ptx/escape,$(image/permissions))"

world/image/env/impl = \
	$(call world/env, $(1))					\
	image_repo_dist_dir="$(IMAGE_REPO_DIST_DIR)"		\
	image_env="$(call ptx/escape,$($(1)_ENV))"		\
	image_pkgs="$(call ptx/escape,$($(1)_PKGS))"		\
	image_files="$(call ptx/escape,$($(1)_FILES))"		\
	image_image="$(call ptx/escape,$($(1)_IMAGE))"

world/image/env = \
	$(call world/image/env/impl,$(strip $(1)))

# vim: syntax=make
